#include "quicksort.h"

#include "string.h"
#include <assert.h>

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
