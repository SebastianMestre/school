#include "quicksort.h"

#include "span.h"

#include "iter.h"
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

static void
quicksort_impl(void* l, void* r, size_t size, Comparator cmp) {
	assert(l <= r);
	if ((size_t)(r - l) <= size)
		return;
	void* cut = partition(l + size, r, l, size, cmp);
	mem_swap(l, cut - size, size);
	quicksort_impl(l, cut - size, size, cmp);
	quicksort_impl(cut, r, size, cmp);
}

void
quicksort(Array arr, Comparator cmp) {
	quicksort_impl(array_begin(&arr).pos, array_end(&arr).pos, arr.element_width, cmp);
}
