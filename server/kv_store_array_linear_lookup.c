#include "kv_store.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#define KV_STORE_SIZE 1000000

struct kv_entry {
	char* key;
	uint32_t key_length;

	char* value;
	uint32_t value_length;

	uint64_t generation;
};

struct kv_store {
	pthread_mutex_t m;
	size_t size;
	size_t capacity;
	uint64_t generation;
	struct kv_entry** entries;
};

struct kv_store* kv_store_init() {
	struct kv_store* store = malloc(sizeof(*store));
	if (!store) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	store->entries = malloc(sizeof(store->entries[0]) * KV_STORE_SIZE);
	if (!store->entries) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	memset(store->entries, 0, sizeof(store->entries[0]) * KV_STORE_SIZE);

	store->generation = 0;
	store->size = 0;
	store->capacity = KV_STORE_SIZE;

	pthread_mutex_init(&store->m, NULL);

	return store;
}

static struct kv_entry* kv_entry_init(char* key, uint32_t key_length, char* value, uint32_t value_length, size_t generation) {
	struct kv_entry* entry = malloc(sizeof(*entry));
	if (!entry) {
		return NULL;
	}

	entry->key = key;
	entry->key_length = key_length;

	entry->value = value;
	entry->value_length = value_length;

	entry->generation = generation;

	return entry;
}

static void kv_entry_free(struct kv_entry* entry) {
	free(entry->value);
	free(entry->key);
	free(entry);
}

static struct kv_entry** lookup_key(struct kv_store* store, char* key, uint32_t key_length) {
	for (int i = 0; i < store->size; ++i) {
		struct kv_entry* entry = store->entries[i];
		if (entry->key_length == key_length && memcmp(entry->key, key, key_length) == 0) {
			return &store->entries[i];
		}
	}
	return NULL;
}

static int kv_store_put_impl(struct kv_store* store, char* key, uint32_t key_length, char* value, uint32_t value_length) {

	struct kv_entry** old_entry_ptr = lookup_key(store, key, key_length);

	if (old_entry_ptr) {
		struct kv_entry* old_entry = *old_entry_ptr;

		free(old_entry->key);
		old_entry->key = key;
		old_entry->key_length = key_length;

		free(old_entry->value);
		old_entry->value = value;
		old_entry->value_length = value_length;

		return KV_STORE_OK;
	} 

	assert(store->size < store->capacity);
	struct kv_entry* new_entry = kv_entry_init(key, key_length, value, value_length, store->generation++);

	if (!new_entry) {
		return KV_STORE_OOM;
	}

	store->entries[store->size++] = new_entry;

	return KV_STORE_OK;
}

int kv_store_put(struct kv_store* store, char* key, uint32_t key_length, char* value, uint32_t value_length) {
	pthread_mutex_lock(&store->m);

	int result = kv_store_put_impl(store, key, key_length, value, value_length);

	pthread_mutex_unlock(&store->m);

	return result;
}

static int kv_store_get_impl(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length) {
	struct kv_entry** entry_ptr = lookup_key(store, key, key_length);

	if (!entry_ptr) {
		*out_value = NULL;
		*out_value_length = 0;
		return KV_STORE_NOTFOUND;
	}

	struct kv_entry* entry = *entry_ptr;
	char* new_value = malloc(entry->value_length);

	if (!new_value) {
		*out_value = NULL;
		*out_value_length = 0;
		return KV_STORE_OOM;
	}

	memcpy(new_value, entry->value, entry->value_length);

	*out_value = new_value;
	*out_value_length = entry->value_length;

	return KV_STORE_OK;
}

int kv_store_get(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length) {
	pthread_mutex_lock(&store->m);

	int result = kv_store_get_impl(store, key, key_length, out_value, out_value_length);

	pthread_mutex_unlock(&store->m);

	return result;
}

static int kv_store_del_impl(struct kv_store* store, char* key, uint32_t key_length) {
	struct kv_entry** entry_ptr = lookup_key(store, key, key_length);

	if (!entry_ptr) {
		return KV_STORE_NOTFOUND;
	}

	struct kv_entry* entry = *entry_ptr;
	*entry_ptr = store->entries[--store->size];
	kv_entry_free(entry);

	return KV_STORE_OK;
}

int kv_store_del(struct kv_store* store, char* key, uint32_t key_length) {
	pthread_mutex_lock(&store->m);

	int result = kv_store_del_impl(store, key, key_length);

	pthread_mutex_unlock(&store->m);

	return result;
}

static int kv_store_take_impl(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length) {
	struct kv_entry** entry_ptr = lookup_key(store, key, key_length);

	if (!entry_ptr) {
		return KV_STORE_NOTFOUND;
	}

	struct kv_entry* entry = *entry_ptr;

	*out_value = entry->value;
	*out_value_length = entry->value_length;

	entry->value = NULL;
	entry->value_length = 0;

	*entry_ptr = store->entries[--store->size];
	kv_entry_free(entry);

	return KV_STORE_OK;
}

int kv_store_take(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length) {
	pthread_mutex_lock(&store->m);

	int result = kv_store_take_impl(store, key, key_length, out_value, out_value_length);

	pthread_mutex_unlock(&store->m);

	return result;
}

/********************************************************************/
int kv_store_evict(struct kv_store* store) {
	fprintf(stderr, "kv_store_evict: no implementado\n");
	return 0;
}

int kv_store_stat(struct kv_store* store, int* out_stat) {
	fprintf(stderr, "kv_store_stat: no implementado\n");
	return 0;
}
/*******************************************************************/