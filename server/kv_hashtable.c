
#include "hashtable.h"

typedef struct hashtable kv_store;

#include "kv_store_interface.h"

kv_store* kv_store_init() {
	return hashtable_create();
}

int kv_store_put(
	struct hashtable* store,
	char* key, size_t key_length,
	char* value, size_t value_length
) {
	return hashtable_insert(store, key, key_length, value, value_length);
}

int kv_store_get(
	struct hashtable* store,
	char* key, size_t key_length,
	char** out_value, size_t* out_value_length
) {
	return hashtable_lookup(store, key, key_length, out_value, out_value_length);
}

int kv_store_del(
	struct hashtable* store,
	char* key, size_t key_length
) {
	return hashtable_delete(store, key, key_length);
}

int kv_store_take(
	struct hashtable* store,
	char* key, size_t key_length,
	char** out_value, size_t* out_value_length
) {
	return hashtable_take(store, key, key_length, out_value, out_value_length);
}

int kv_store_evict(
	struct hashtable* store
) {
	return hashtable_evict(store);
}

int kv_store_stat(struct hashtable* t, int* out) {
	*out = 0;
	return 0;
}
