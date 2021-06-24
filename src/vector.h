#include "byte_span.h"

#include <stddef.h>

// A byte vector holds sequences of bytes, all of the same length.
struct _ByteVector {
	ByteSpan data;

	size_t const element_width;
	size_t size;
	size_t capacity;
};
typedef struct _ByteVector ByteVector;

ByteVector
byte_vector_create(size_t element_width);

ByteSpan
byte_vector_at(ByteVector v, size_t i);

void
byte_vector_resize_storage(ByteVector* v, size_t n);

void
byte_vector_push(ByteVector* v, ByteSpan data);

void
byte_vector_release(ByteVector* v);
