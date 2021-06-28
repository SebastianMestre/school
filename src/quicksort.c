#include "span.h"

#include "iter.h"
#include <assert.h>

static void
iter_swap(Iter lhs, Iter rhs) {
	assert(lhs.element_width == rhs.element_width);
	if (lhs.pos == rhs.pos)
		return;
	uint8_t* lhs_data = lhs.pos;
	uint8_t* rhs_data = rhs.pos;
	size_t element_width = lhs.element_width;
	for (size_t i = 0; i != element_width; ++i) {
		lhs_data[i] ^= rhs_data[i];
		rhs_data[i] ^= lhs_data[i];
		lhs_data[i] ^= rhs_data[i];
	}
}

static Iter
partition(Iter i, Iter j, Iter pivot, Comparator cmp) {
	Iter k = i;
	for (; !iter_eq(i, j); iter_advance(&i)) {
		if (call_cmp(cmp, pivot.pos, i.pos) < 0) {
			iter_swap(i, k);
			iter_advance(&k);
		}
	}
	return k;
}

static void
quicksort_impl(Iter l, Iter r, Comparator cmp) {
	if (iter_distance(l, r) <= 1)
		return;
	Iter partition_point = partition(iter_next(l), r, l, cmp);
	iter_swap(l, iter_prev(partition_point));
	quicksort_impl(l, iter_prev(partition_point), cmp);
	quicksort_impl(partition_point, r, cmp);
}

void
quicksort(Span data, size_t element_width, Comparator cmp) {
	quicksort_impl(
		(Iter){data.begin, element_width},
		(Iter){data.end, element_width},
		cmp);
}
