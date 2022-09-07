#include "try_alloc.h"

#include "kv_store.h"

#include <stdlib.h>

#define ALLOC_ATTEMPTS 10

int try_alloc(kv_store* store, size_t size, void** ptr) {
	int attempts;
	for (attempts = 0; attempts < ALLOC_ATTEMPTS; attempts++) {
		*ptr = malloc(size);
		if (*ptr != NULL) break;
		if (kv_store_evict(store) < 0) return -1;
	}
	if (attempts == ALLOC_ATTEMPTS) return -1;
	return 0;
}
