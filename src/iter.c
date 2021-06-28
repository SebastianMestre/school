#include "iter.h"

#include <assert.h>

Iter
iter_next(Iter i) {
	return iter_plus(i, 1);
}

Iter
iter_prev(Iter i) {
	return iter_plus(i, -1);
}

void
iter_advance(Iter* i) {
	iter_add(i, 1);
}

void
iter_retreat(Iter* i) {
	iter_add(i, -1);
}

Iter
iter_plus(Iter i, ptrdiff_t n) {
	iter_add(&i, n);
	return i;
}

void
iter_add(Iter* i, ptrdiff_t n) {
	i->pos += i->element_width * n;
}

bool
iter_eq(Iter i, Iter j) {
	assert(i.element_width == j.element_width);
	return i.pos == j.pos;
}

ptrdiff_t
iter_distance(Iter i, Iter j) {
	assert(i.element_width == j.element_width);
	ptrdiff_t pdiff = j.pos - i.pos;
	assert(pdiff % i.element_width == 0);
	return pdiff / i.element_width;
}
