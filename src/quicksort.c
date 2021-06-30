#include "quicksort.h"

#include <assert.h>

static void
mem_swap(void* lhs, void* rhs, size_t n) {
	if (lhs == rhs)
		return;
	for (size_t i = 0; i != n; ++i) {
		((uint8_t*)lhs)[i] ^= ((uint8_t*)rhs)[i];
		((uint8_t*)rhs)[i] ^= ((uint8_t*)lhs)[i];
		((uint8_t*)lhs)[i] ^= ((uint8_t*)rhs)[i];
	}
}

static void*
partition(void* i, void* j, void* pivot, size_t size, Comparator cmp) {
	void* k = i;
	for (; i != j; i += size) {
		if (call_cmp(cmp, i, pivot) < 0) {
			mem_swap(i, k, size);
			k += size;
		}
	}
	return k;
}

void
quicksort(void* l, void* r, size_t size, Comparator cmp) {
	assert(l <= r);
	assert((r - l) % size == 0);
	if ((size_t)(r - l) <= size)
		return;
	void* cut = partition(l + size, r, l, size, cmp);
	mem_swap(l, cut - size, size);
	quicksort(l, cut - size, size, cmp);
	quicksort(cut, r, size, cmp);
}
