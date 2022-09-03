#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>

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


static void update_lru(struct hashtable* tabla, struct nodo* nodo);

static bool evict_one(struct hashtable* tabla);

static size_t hash(unsigned char* s, size_t n);

static bool matches(char* key, size_t key_size, struct nodo* nodo);

struct nodo* nodo_from_lru_list(struct list* list);

struct nodo* nodo_from_probing_list(struct list* list);

static struct list* scan_probing_list(struct list* centinela, char* key, size_t key_size);


struct hashtable* hashtable_create() {
	struct hashtable* tabla = malloc(sizeof(*tabla));

	list_init(&tabla->cache);
	pthread_mutex_init(&tabla->cache_lock, NULL);

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

	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		// TODO: free fuera de la region critica
		free(nodo->value);
		free(nodo->key);

		nodo->key_size = key_size;
		nodo->key = key;

		nodo->value_size = value_size;
		nodo->value = value;

		update_lru(tabla, nodo);

		result = KV_STORE_OK;

	} else {
		struct nodo* nodo = evict_until_malloc(tabla, sizeof(*nodo));

		if (nodo == NULL) {
			result = KV_STORE_OOM;
		} else {
			nodo->bucket = bucket;

			nodo->key_size = key_size;
			nodo->key = key;

			nodo->value_size = value_size;
			nodo->value = value;

			list_push_start(&tabla->casillas[bucket].nodos, &nodo->probe);

			pthread_mutex_lock(&tabla->cache_lock);
			list_push_start(&tabla->cache, &nodo->cache);
			pthread_mutex_unlock(&tabla->cache_lock);

			result = KV_STORE_OK;
		}
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

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

	int result;
	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		list_remove(it);

		pthread_mutex_lock(&tabla->cache_lock);
		list_remove(&nodo->cache);
		pthread_mutex_unlock(&tabla->cache_lock);

		// TODO: free fuera de la region critica
		free(nodo->key);
		free(nodo->value);
		free(nodo);

		result = KV_STORE_OK;

	} else {
		result = KV_STORE_UNK;
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

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

	int result;
	if (it != centinela) {
		struct nodo* nodo = nodo_from_probing_list(it);

		list_remove(it);

		pthread_mutex_lock(&tabla->cache_lock);
		list_remove(&nodo->cache);
		pthread_mutex_unlock(&tabla->cache_lock);

		*out_value = nodo->value;
		*out_value_size = nodo->value_size;

		// TODO: free fuera de la region critica
		free(nodo->key);
		free(nodo);

		result = KV_STORE_OK;

	} else {
		result = KV_STORE_UNK;

		*out_value = NULL;
		*out_value_size = 0;
	}

	pthread_mutex_unlock(&tabla->casillas[bucket].lock);

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

bool evict_one(struct hashtable* tabla) {
	pthread_mutex_lock(&tabla->cache_lock);

	bool result = false;

	int iters = 0;

	struct list* centinela = &tabla->cache;
	struct list* it;
	for (
		it = centinela->next;
		it != centinela;
		it = it->next) {

		struct nodo* nodo = nodo_from_lru_list(it);
		struct casilla* casilla = &tabla->casillas[nodo->bucket];

		if (pthread_mutex_trylock(&casilla->lock) == 0) {
			list_remove(&nodo->cache);
			list_remove(&nodo->probe);

			free(nodo->key);
			free(nodo->value);
			free(nodo);

			result = true;
			break;
		}

		iters++;
	}

	pthread_mutex_unlock(&tabla->cache_lock);

	return result;
}

void update_lru(struct hashtable* tabla, struct nodo* nodo) {
	pthread_mutex_lock(&tabla->cache_lock);
	list_remove(&nodo->cache);
	list_push_start(&tabla->cache, &nodo->cache);
	pthread_mutex_unlock(&tabla->cache_lock);
}

static struct list* scan_probing_list(struct list* centinela, char* key, size_t key_size) {

	struct list* it;
	for (
		it = centinela->next;
		it != centinela;
		it = it->next) {

		struct nodo* nodo = nodo_from_probing_list(it);

		if (matches(key, key_size, nodo)) {
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

static bool matches(char* key, size_t key_size, struct nodo* nodo) {
	return key_size == nodo->key_size &&
	       memcmp(key, nodo->key, key_size) == 0;
}

struct nodo* nodo_from_lru_list(struct list* list) {
	return (struct nodo*)((void*)list - offsetof(struct nodo, cache));
}

struct nodo* nodo_from_probing_list(struct list* list) {
	return (struct nodo*)((void*)list - offsetof(struct nodo, probe));
}


