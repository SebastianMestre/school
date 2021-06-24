#include "byte_span.h"

#include <stddef.h>

// A byte vector holds sequences of bytes, all of the same length.
struct _Vector {
	ByteSpan data;

	size_t const element_width;
	size_t size;
	size_t capacity;
};
typedef struct _Vector Vector;

Vector
vector_create(size_t element_width);

ByteSpan
vector_at(Vector v, size_t i);

void
vector_resize_storage(Vector* v, size_t n);

void
vector_push(Vector* v, ByteSpan data);

void
vector_release(Vector* v);
