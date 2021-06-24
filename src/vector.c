#include "vector.h"

#include <assert.h>
#include <stdlib.h>

ByteVector
byte_vector_create(size_t element_width) {
	size_t capacity = 4;

	size_t data_width = element_width * capacity;
	void* data = malloc(data_width);

	return (ByteVector) {
		.data = byte_span_create(data, data_width),

		.element_width = element_width,
		.size = 0,
		.capacity = capacity,
	};
}

static void*
byte_vector_at_unsafe(ByteVector v, size_t i) {
	return v.data.begin + v.element_width * i;
}

ByteSpan
byte_vector_at(ByteVector v, size_t i) {
	assert(i < v.size);
	return byte_span_create(byte_vector_at_unsafe(v, i), v.element_width);
}

void
byte_vector_resize_storage(ByteVector* v, size_t n) {
	assert(v->size <= n);

	size_t new_data_width = v->element_width * n;
	void* new_data = malloc(new_data_width);

	byte_span_write(new_data, v->data);
	free(v->data.begin);
	v->data = byte_span_create(new_data, new_data_width);

	v->capacity = n;
}

void
byte_vector_push(ByteVector* v, ByteSpan data) {
	if (v->size == v->capacity) {
		byte_vector_resize_storage(v, v->capacity * 2);
	}

	void* location = byte_vector_at_unsafe(*v, v->size);
	byte_span_write(location, data);
	v->size += 1;
}

void
byte_vector_release(ByteVector* v) {
	free(v->data.begin);
	// zero out the pointers, just in case
	v->data = (ByteSpan){};
}
