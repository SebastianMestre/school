#include "vector.h"

#include <assert.h>
#include <stdlib.h>

Vector
vector_create(size_t element_width) {
	const size_t capacity = 4;

	return (Vector) {
		.buffer = array_malloc(element_width, capacity),
		.size = 0,
	};
}

void
vector_release(Vector* v) {
	array_free(&v->buffer);
}

static void*
vector_at_ptr(Vector* v, size_t i) {
	return array_at(&v->buffer, i).begin;
}

Span
vector_at(Vector const* v, size_t i) {
	assert(i < v->size);
	return array_at(&v->buffer, i);
}

Span
vector_last(Vector const* v) {
	return vector_at(v, v->size - 1);
}

void
vector_put_at(Vector* v, size_t i, Span data) {
	assert(span_width(data) == v->buffer.element_width);
	span_write(vector_at(v, i).begin, data);
}

void
vector_resize_storage(Vector* v, size_t n) {
	assert(v->size <= n);
	Array new_buffer = array_malloc(v->buffer.element_width, n);

	span_write(new_buffer.data.begin, v->buffer.data);
	array_free(&v->buffer);
	v->buffer = new_buffer;
}

void
vector_push(Vector* v, Span data) {
	assert(span_width(data) == v->buffer.element_width );
	if (v->size == v->buffer.size) {
		vector_resize_storage(v, v->buffer.size * 2);
	}

	void* location = vector_at_ptr(v, v->size);
	span_write(location, data);
	v->size += 1;
}

void
vector_pop(Vector* v) {
	v->size -= 1;
}

Array
vector_full_segment(Vector* v) {
	return (Array){
		.data = {
			v->buffer.data.begin,
			vector_last(v).end,
		},
		v->buffer.element_width,
		v->size,
	};
}
