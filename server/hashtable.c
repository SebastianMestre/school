#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "kv_store.h"

#include "list.h"

#define SIZE 1000000

#define MAX_EVICT_STEPS 10
#define MAX_EVICT_ATTEMPTS 10

struct row {
	pthread_mutex_t lock;
	struct list nodes;
};

struct hashtable {
	pthread_spinlock_t lru_lock;
	struct list lru;

	pthread_spinlock_t key_count_lock;
	size_t key_count;

	struct row rows[SIZE];
};

struct node {
	struct row* row;

	size_t key_size;
	unsigned char* key;

	size_t value_size;
	unsigned char* value;

	struct list lru;
	struct list probing;
};

static bool node_matches(struct node* node, unsigned char* key, size_t key_size);
static struct node* node_from_lru_list(struct list* list);
static struct node* node_from_probing_list(struct list* list);
static void node_free(struct node* node);
static void node_assign(struct node* node, unsigned char* key, size_t key_size, unsigned char* value, size_t value_size);

static void update_lru(struct hashtable* table, struct node* node);
static bool evict_one(struct hashtable* table);
static size_t hash(unsigned char* s, size_t n);
static struct list* find_in_probing_list(struct list* probing, unsigned char* key, size_t key_size);
static void increment_key_count(struct hashtable* table);
static void decrement_key_count(struct hashtable* table);
static struct row* find_row(struct hashtable* tabla, unsigned char* key, size_t key_size);
static struct node* find_in_row(struct row* row, unsigned char* key, size_t key_size);


struct hashtable* hashtable_create() {
	struct hashtable* table = malloc(sizeof(*table));

	list_init(&table->lru);
	pthread_spin_init(&table->lru_lock, PTHREAD_PROCESS_PRIVATE);

	table->key_count = 0;
	pthread_spin_init(&table->key_count_lock, PTHREAD_PROCESS_PRIVATE);

	for (int i = 0; i < SIZE; ++i) {
		struct row* row = &table->rows[i];
		pthread_mutex_init(&row->lock, NULL);
		list_init(&row->nodes);
	}

	return table;
}

void* evict_until_malloc(struct hashtable* table, size_t size) {
	void* result = malloc(size);

	if (result != NULL)
		return result;

	for (int attempts = 0; attempts < MAX_EVICT_ATTEMPTS; ++attempts) {
		if (!evict_one(table))
			break;

		result = malloc(size);

		if (result != NULL)
			break;
	}

	return result;
}

int hashtable_lookup(
	struct hashtable* table,
	unsigned char* key, size_t key_size,
	unsigned char** out_value, size_t* out_value_size
) {
	struct row* row = find_row(table, key, key_size);
	pthread_mutex_lock(&row->lock);
	struct node* node = find_in_row(row, key, key_size);

	int result = KV_STORE_UNK;
	if (node == NULL) {
		// por cortesia, devolvemos NULL
		*out_value = NULL;
		*out_value_size = 0;
		result = KV_STORE_UNK;
	} else {

		unsigned char* buf = evict_until_malloc(table, node->value_size);

		if (buf == NULL) {
			// Si no podemos reservar suficiente memoria para
			// responder una query entonces no la respondemos.
			*out_value = NULL;
			*out_value_size = 0;
			result = KV_STORE_OOM;
		} else {
			memcpy(buf, node->value, node->value_size);
			*out_value = buf;
			*out_value_size = node->value_size;
			result = KV_STORE_OK;
		}

		update_lru(table, node);
	}

	pthread_mutex_unlock(&row->lock);

	return result;
}

int hashtable_insert(
	struct hashtable* table,
	unsigned char* key, size_t key_size,
	unsigned char* value, size_t value_size
) {
	struct row* row = find_row(table, key, key_size);
	pthread_mutex_lock(&row->lock);
	struct node* node = find_in_row(row, key, key_size);

	int result = KV_STORE_UNK;

	unsigned char* to_free_key = NULL;
	unsigned char* to_free_value = NULL;

	if (node == NULL) {
		node = evict_until_malloc(table, sizeof(*node));

		if (node == NULL) {
			result = KV_STORE_OOM;
		} else {

			node->row = row;
			node_assign(node, key, key_size, value, value_size);
			list_init(&node->lru);

			list_push_start(&row->nodes, &node->probing);
			increment_key_count(table);

			update_lru(table, node);

			result = KV_STORE_OK;
		}
	} else {

		// haremos el free fuera de la region critica
		to_free_key = node->key;
		to_free_value = node->value;

		node_assign(node, key, key_size, value, value_size);

		update_lru(table, node);

		result = KV_STORE_OK;
	}

	pthread_mutex_unlock(&row->lock);

	free(to_free_key);
	free(to_free_value);

	return result;

}

int hashtable_delete(
	struct hashtable* table,
	unsigned char* key, size_t key_size
) {

	struct row* row = find_row(table, key, key_size);
	pthread_mutex_lock(&row->lock);
	struct node* node = find_in_row(row, key, key_size);

	int result;
	if (node == NULL) {
		result = KV_STORE_UNK;
	} else {
		decrement_key_count(table);
		list_remove(&node->probing);

		pthread_spin_lock(&table->lru_lock);
		list_remove(&node->lru);
		pthread_spin_unlock(&table->lru_lock);

		result = KV_STORE_OK;
	}

	pthread_mutex_unlock(&row->lock);

	// free fuera de la region critica
	node_free(node);

	return result;
}

int hashtable_take(
	struct hashtable* table,
	unsigned char* key, size_t key_size,
	unsigned char** out_value, size_t* out_value_size
) {
	struct row* row = find_row(table, key, key_size);
	pthread_mutex_lock(&row->lock);
	struct node* node = find_in_row(row, key, key_size);

	int result;
	if (node == NULL) {
		result = KV_STORE_UNK;

		*out_value = NULL;
		*out_value_size = 0;
	} else {
		result = KV_STORE_OK;

		decrement_key_count(table);

		list_remove(&node->probing);

		pthread_spin_lock(&table->lru_lock);
		list_remove(&node->lru);
		pthread_spin_unlock(&table->lru_lock);

		*out_value = node->value;
		*out_value_size = node->value_size;

		node->value = NULL;
	}

	pthread_mutex_unlock(&row->lock);

	// free fuera de la region critica
	node_free(node);

	return result;
}

int hashtable_evict(struct hashtable* table) {
	if (evict_one(table))
		return KV_STORE_OK;
	else
		return KV_STORE_UNK;
}

size_t hashtable_size(struct hashtable* table) {
	pthread_spin_lock(&table->key_count_lock);
	size_t result = table->key_count;
	pthread_spin_unlock(&table->key_count_lock);
	return result;
}


static struct row* find_row(struct hashtable* table, unsigned char* key, size_t key_size) {
	size_t bucket = hash(key, key_size) % SIZE;
	return &table->rows[bucket];
}

static struct node* find_in_row(struct row* row, unsigned char* key, size_t key_size) {
	struct list* it = find_in_probing_list(&row->nodes, key, key_size);
	if (it == &row->nodes)
		return NULL;
	else
		return node_from_probing_list(it);
}

// Elimina una entrada de la tabla, en orden LRU aproximado.
// Puede fallar si las filas de todas las entradas que intenta liberar están lockeadas.
bool evict_one(struct hashtable* table) {
	pthread_spin_lock(&table->lru_lock);

	bool result = false;


	struct node* to_free = NULL;

	int iters = 0;
	struct list* lru = &table->lru;
	for (struct list* it = lru->next; it != lru; it = it->next) {

		iters++;
		if (iters > MAX_EVICT_STEPS) break;

		struct node* node = node_from_lru_list(it);
		struct row* row = node->row;

		if (pthread_mutex_trylock(&row->lock) == 0) {

			decrement_key_count(table);

			list_remove(&node->lru);
			list_remove(&node->probing);
			to_free = node;

			result = true;

			pthread_mutex_unlock(&row->lock);

			break;
		}

	}

	pthread_spin_unlock(&table->lru_lock);

	node_free(to_free);

	return result;
}

static void increment_key_count(struct hashtable* table) {
	pthread_spin_lock(&table->key_count_lock);
	table->key_count++;
	pthread_spin_unlock(&table->key_count_lock);
}

static void decrement_key_count(struct hashtable* table) {
	pthread_spin_lock(&table->key_count_lock);
	assert(table->key_count > 0);
	table->key_count--;
	pthread_spin_unlock(&table->key_count_lock);
}

// PRE: el lock de la fila del nodo está tomado por el thread actual
static void update_lru(struct hashtable* table, struct node* node) {
	pthread_spin_lock(&table->lru_lock);
	list_remove(&node->lru);
	list_push_start(&table->lru, &node->lru);
	pthread_spin_unlock(&table->lru_lock);
}

// PRE: el lock de la fila está tomado por el thread actual
static struct list* find_in_probing_list(struct list* probing, unsigned char* key, size_t key_size) {

	struct list* it;
	for (it = probing->next; it != probing; it = it->next) {

		struct node* node = node_from_probing_list(it);

		if (node_matches(node, key, key_size)) {
			break;
		}
	}

	return it;
}

static size_t hash(unsigned char* s, size_t n) {
	const uint64_t fnv_basis = 0xcbf29ce484222325ull;
	const uint64_t fnv_prime = 0x100000001b3ull;
	uint64_t result = fnv_basis;
	while (n--) {
		result ^= *s++;
		result *= fnv_prime;
	}
	return result;
}

// asigna la clave y valor de un nodo
static void node_assign(struct node* node, unsigned char* key, size_t key_size, unsigned char* value, size_t value_size){
	node->key = key;
	node->key_size = key_size;
	node->value = value;
	node->value_size = value_size;
}

// libera un nodo y sus datos (clave y valor)
// si node es NULL, no se hace ninguna operacion
static void node_free(struct node* node) {
	if (!node) return;
	free(node->key);
	free(node->value);
	free(node);
}

// devuelve true cuando la clave del nodo es igual a la dada
static bool node_matches(struct node* node, unsigned char* key, size_t key_size) {
	return key_size == node->key_size &&
	       memcmp(key, node->key, key_size) == 0;
}

// dada la direccion de la lista lru en un nodo, devuelve la direccion del nodo
static struct node* node_from_lru_list(struct list* list) {
	return (struct node*)((void*)list - offsetof(struct node, lru));
}

// dada la direccion de la lista de probing en un nodo, devuelve la direccion del nodo
static struct node* node_from_probing_list(struct list* list) {
	return (struct node*)((void*)list - offsetof(struct node, probing));
}


