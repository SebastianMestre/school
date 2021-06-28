#pragma once

#include <stddef.h>

struct _Iter {
	void* pos;
	size_t element_width;
};
typedef struct _Iter Iter;

Iter
iter_next(Iter i);

void
iter_advance(Iter* i);


Iter
iter_prev(Iter i);

void
iter_retreat(Iter* i);


Iter
iter_plus(Iter i, ptrdiff_t n);

void
iter_add(Iter* i, ptrdiff_t n);


bool
iter_eq(Iter i, Iter j);

ptrdiff_t
iter_distance(Iter i, Iter j);
