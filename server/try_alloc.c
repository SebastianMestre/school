#include "try_alloc.h"

#include "kv_store.h"

#include <stdlib.h>

#define ALLOC_ATTEMPTS 10

void* try_alloc(kv_store* store, size_t size) {
	for (int attempts = 0; attempts < ALLOC_ATTEMPTS; attempts++) {
		void* ptr = malloc(size);
		if (ptr != NULL) return ptr;
		if (kv_store_evict(store) < 0) return NULL;
	}
	return NULL;
}
