
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

typedef struct kv_hashtable_wrapper kv_store;
#include "kv_store_interface.h"

struct kv_hashtable_wrapper {
	pthread_spinlock_t stat_lock;
	struct kv_store_stat stat;
	struct hashtable* table;
};


struct kv_hashtable_wrapper* kv_store_init() {
	struct kv_hashtable_wrapper* result = malloc(sizeof(*result));

	memset(&result->stat, 0, sizeof(result->stat));
	pthread_spin_init(&result->stat_lock, PTHREAD_PROCESS_PRIVATE);
	result->table = hashtable_create();

	return result;
}

int kv_store_put(
	struct kv_hashtable_wrapper* store,
	unsigned char* key, size_t key_length,
	unsigned char* value, size_t value_length
) {
	pthread_spin_lock(&store->stat_lock);
	store->stat.put_count++;
	pthread_spin_unlock(&store->stat_lock);

	return hashtable_insert(store->table, key, key_length, value, value_length);
}

int kv_store_get(
	struct kv_hashtable_wrapper* store,
	unsigned char* key, size_t key_length,
	unsigned char** out_value, size_t* out_value_length
) {
	pthread_spin_lock(&store->stat_lock);
	store->stat.get_count++;
	pthread_spin_unlock(&store->stat_lock);

	return hashtable_lookup(store->table, key, key_length, out_value, out_value_length);
}

int kv_store_del(
	struct kv_hashtable_wrapper* store,
	unsigned char* key, size_t key_length
) {
	pthread_spin_lock(&store->stat_lock);
	store->stat.del_count++;
	pthread_spin_unlock(&store->stat_lock);

	return hashtable_delete(store->table, key, key_length);
}

int kv_store_take(
	struct kv_hashtable_wrapper* store,
	unsigned char* key, size_t key_length,
	unsigned char** out_value, size_t* out_value_length
) {
	pthread_spin_lock(&store->stat_lock);
	store->stat.take_count++;
	pthread_spin_unlock(&store->stat_lock);

	return hashtable_take(store->table, key, key_length, out_value, out_value_length);
}

int kv_store_evict(
	struct kv_hashtable_wrapper* store
) {
	return hashtable_evict(store->table);
}

int kv_store_stat(struct kv_hashtable_wrapper* store, struct kv_store_stat* out) {
	pthread_spin_lock(&store->stat_lock);
	struct kv_store_stat result = store->stat;
	pthread_spin_unlock(&store->stat_lock);

	result.key_count = hashtable_size(store->table);

	*out = result;

	return KV_STORE_OK;
}
