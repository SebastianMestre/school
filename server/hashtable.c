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

#define MAX_ITERS 15
#define MAX_ATTEMPTS 5

struct casilla {
	pthread_mutex_t lock;
	struct list nodos;
};

struct hashtable {
	pthread_mutex_t cache_lock;
	struct list cache;

	pthread_mutex_t key_count_lock;
	size_t key_count;

	struct casilla casillas[SIZE];
};

struct nodo {
	size_t bucket;

	size_t key_size;
	unsigned char* key;

	size_t value_size;
	unsigned char* value;

	struct list cache;
	struct list probe;
};

static bool nodo_matches(struct nodo* nodo, char* key, size_t key_size);
struct nodo* nodo_from_lru_list(struct list* list);
struct nodo* nodo_from_probing_list(struct list* list);
static void nodo_free(struct nodo* nodo);
static void nodo_assign(struct nodo* nodo, char* key, size_t key_size, char* value, size_t value_size);

static void update_lru(struct hashtable* tabla, struct nodo* nodo);
static bool evict_one(struct hashtable* tabla);
static size_t hash(unsigned char* s, size_t n);
static struct list* scan_probing_list(struct list* centinela, char* key, size_t key_size);
static void increment_key_count(struct hashtable* tabla);
static void decrement_key_count(struct hashtable* tabla);


struct hashtable* hashtable_create() {
	struct hashtable* tabla = malloc(sizeof(*tabla));

	list_init(&tabla->cache);
	pthread_mutex_init(&tabla->cache_lock, NULL);

	tabla->key_count = 0;
	pthread_mutex_init(&tabla->key_count_lock, NULL);

	for (int i = 0; i < SIZE; ++i) {
		struct casilla* casilla = &tabla->casillas[i];
		pthread_mutex_init(&casilla->lock, NULL);
		list_init(&casilla->nodos);
	}

	return tabla;
}

void* evict_until_malloc(struct hashtable* tabla, size_t size) {
	void* result = malloc(size);

	if (result != NULL)
		return result;

	for (int attempts = 0; attempts < MAX_ATTEMPTS; ++attempts) {
		if (!evict_one(tabla))
			break;

		result = malloc(size);

		if (result != NULL)
			break;
	}

	return result;
}

int hashtable_lookup(
	struct hashtable* tabla,

	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size
) {
	size_t bucket = hash(key, key_size) % SIZE;

	pthread_mutex_lock(&tabla->casillas[bucket].lock);

	struct list* centinela = &tabla->casillas[bucket].nodos;

	struct list* it = scan_probing_list(centinela, key, key_size);

	int result = KV_STORE_UNK;

	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		char* buf = evict_until_malloc(tabla, nodo->value_size);

		if (buf == NULL) {
			// Si no podemos reservar suficiente memoria para
			// responder una query entonces no la respondemos.
			*out_value_size = 0;
			*out_value = NULL;
			result = KV_STORE_OOM;
		} else {
			memcpy(buf, nodo->value, nodo->value_size);
			*out_value = buf;
			*out_value_size = nodo->value_size;
			result = KV_STORE_OK;
		}

		update_lru(tabla, nodo);
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

	return result;
}

int hashtable_insert(
	struct hashtable* tabla,

	unsigned char* key,
	size_t key_size,

	unsigned char* value,
	size_t value_size
) {
	size_t bucket = hash(key, key_size) % SIZE;

	pthread_mutex_lock(&tabla->casillas[bucket].lock);

	struct list* centinela = &tabla->casillas[bucket].nodos;

	struct list* it = scan_probing_list(centinela, key, key_size);

	int result = KV_STORE_UNK;

	char* to_free_key = NULL;
	char* to_free_value = NULL;

	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		// haremos el free fuera de la region critica
		to_free_key = nodo->key;
		to_free_value = nodo->value;

		nodo_assign(nodo, key, key_size, value, value_size);

		update_lru(tabla, nodo);

		result = KV_STORE_OK;

	} else {
		struct nodo* nodo = evict_until_malloc(tabla, sizeof(*nodo));

		if (nodo == NULL) {
			result = KV_STORE_OOM;
		} else {
			nodo->bucket = bucket;

			nodo_assign(nodo, key, key_size, value, value_size);

			list_push_start(&tabla->casillas[bucket].nodos, &nodo->probe);

			update_lru(tabla, nodo);

			increment_key_count(tabla);

			result = KV_STORE_OK;
		}
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

	free(to_free_key);
	free(to_free_value);

	return result;

}

int hashtable_delete(
	struct hashtable* tabla,
	unsigned char* key,
	size_t key_size
) {

	size_t bucket = hash(key, key_size) % SIZE;

	pthread_mutex_lock(&tabla->casillas[bucket].lock);

	struct list* centinela = &tabla->casillas[bucket].nodos;

	struct list* it = scan_probing_list(centinela, key, key_size);

	struct nodo* to_free = NULL;

	int result;
	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		decrement_key_count(tabla);

		list_remove(&nodo->probe);

		pthread_mutex_lock(&tabla->cache_lock);
		list_remove(&nodo->cache);
		pthread_mutex_unlock(&tabla->cache_lock);

		// haremos free fuera de la region critica
		to_free = nodo;

		result = KV_STORE_OK;

	} else {
		result = KV_STORE_UNK;
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

	nodo_free(to_free);

	return result;
}

int hashtable_take(
	struct hashtable* tabla,
	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size
) {
	size_t bucket = hash(key, key_size) % SIZE;

	pthread_mutex_lock(&tabla->casillas[bucket].lock);

	struct list* centinela = &tabla->casillas[bucket].nodos;

	struct list* it = scan_probing_list(centinela, key, key_size);

	struct nodo* to_free = NULL;

	int result;
	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		decrement_key_count(tabla);

		list_remove(it);

		pthread_mutex_lock(&tabla->cache_lock);
		list_remove(&nodo->cache);
		pthread_mutex_unlock(&tabla->cache_lock);

		*out_value = nodo->value;
		*out_value_size = nodo->value_size;

		nodo->value = NULL;
		to_free = nodo; // haremos free fuera de la region critica

		result = KV_STORE_OK;

	} else {
		result = KV_STORE_UNK;

		*out_value = NULL;
		*out_value_size = 0;
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

	nodo_free(to_free);

	return result;
}

int hashtable_evict(
	struct hashtable* tabla
) {
	if (evict_one(tabla))
		return KV_STORE_OK;
	else
		return KV_STORE_UNK;
}

size_t hashtable_size(struct hashtable* tabla) {
	pthread_mutex_lock(&tabla->key_count_lock);
	size_t result = tabla->key_count;
	pthread_mutex_unlock(&tabla->key_count_lock);
	return result;
}


// Elimina una entrada de la tabla, en orden LRU aproximado.
// Puede fallar si las filas de todas las entradas que intenta liberar están lockeadas.
bool evict_one(struct hashtable* tabla) {
	pthread_mutex_lock(&tabla->cache_lock);

	bool result = false;

	int iters = 0;

	struct list* centinela = &tabla->cache;
	struct list* it;
	for (it = centinela->next; it != centinela; it = it->next) {

		struct nodo* nodo = nodo_from_lru_list(it);
		struct casilla* casilla = &tabla->casillas[nodo->bucket];

		if (pthread_mutex_trylock(&casilla->lock) == 0) {

			decrement_key_count(tabla);

			list_remove(&nodo->cache);
			list_remove(&nodo->probe);

			nodo_free(nodo);

			result = true;
			break;
		}

		iters++;
	}

	pthread_mutex_unlock(&tabla->cache_lock);

	return result;
}

static void increment_key_count(struct hashtable* tabla) {
	pthread_mutex_lock(&tabla->key_count_lock);
	tabla->key_count++;
	pthread_mutex_unlock(&tabla->key_count_lock);
}

static void decrement_key_count(struct hashtable* tabla) {
	pthread_mutex_lock(&tabla->key_count_lock);
	assert(tabla->key_count > 0);
	tabla->key_count--;
	pthread_mutex_unlock(&tabla->key_count_lock);
}

// PRE: el lock de la fila del nodo está tomado por el thread actual
static void update_lru(struct hashtable* tabla, struct nodo* nodo) {
	pthread_mutex_lock(&tabla->cache_lock);
	list_remove(&nodo->cache);
	list_push_start(&tabla->cache, &nodo->cache);
	pthread_mutex_unlock(&tabla->cache_lock);
}

// PRE: el lock de la fila está tomado por el thread actual
static struct list* scan_probing_list(struct list* centinela, char* key, size_t key_size) {

	struct list* it;
	for (it = centinela->next; it != centinela; it = it->next) {

		struct nodo* nodo = nodo_from_probing_list(it);

		if (nodo_matches(nodo, key, key_size)) {
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
static void nodo_assign(struct nodo* nodo, char* key, size_t key_size, char* value, size_t value_size){
	nodo->key = key;
	nodo->key_size = key_size;
	nodo->value = value;
	nodo->value_size = value_size;
}

// libera un nodo y sus datos (clave y valor)
// si nodo es NULL, no se hace ninguna operacion
static void nodo_free(struct nodo* nodo) {
	if (!nodo) return;
	free(nodo->key);
	free(nodo->value);
	free(nodo);
}

// devuelve true cuando la clave del nodo es igual a la dada
static bool nodo_matches(struct nodo* nodo, char* key, size_t key_size) {
	return key_size == nodo->key_size &&
	       memcmp(key, nodo->key, key_size) == 0;
}

// dada la direccion de la lista lru en un nodo, devuelve la direccion del nodo
struct nodo* nodo_from_lru_list(struct list* list) {
	return (struct nodo*)((void*)list - offsetof(struct nodo, cache));
}

// dada la direccion de la lista de probing en un nodo, devuelve la direccion del nodo
struct nodo* nodo_from_probing_list(struct list* list) {
	return (struct nodo*)((void*)list - offsetof(struct nodo, probe));
}


