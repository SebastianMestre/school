#include "vector.h"

#include <assert.h>
#include <stdlib.h>

Vector
vector_create(size_t element_width) {
	size_t capacity = 4;
	size_t data_width = element_width * capacity;

	return (Vector) {
		.data = span_malloc(data_width),

		.element_width = element_width,
		.size = 0,
		.capacity = capacity,
	};
}

void
vector_release(Vector* v) {
	free(v->data.begin);
	v->data.begin = nullptr;
	v->data.end = nullptr;
}

static void*
vector_at_ptr(Vector v, size_t i) {
	return v.data.begin + v.element_width * i;
}

Span
vector_at(Vector v, size_t i) {
	assert(i < v.size);
	return span_slice(v.data, v.element_width * i, v.element_width);
}

Span
vector_last(Vector v) {
	return vector_at(v, v.size - 1);
}

void
vector_put_at(Vector v, size_t i, Span data) {
	assert(span_width(data) == v.element_width);
	span_write(vector_at(v, i).begin, data);
}

void
vector_resize_storage(Vector* v, size_t n) {
	assert(v->size <= n);

	size_t new_data_width = v->element_width * n;
	void* new_data = malloc(new_data_width);

	span_write(new_data, v->data);
	free(v->data.begin);
	v->data = span_create(new_data, new_data_width);

	v->capacity = n;
}

static Span
push(Vector* v, Span data) {
	if (v->size == v->capacity) {
		vector_resize_storage(v, v->capacity * 2);
	}

	void* location = vector_at_ptr(*v, v->size);
	span_write(location, data);
	v->size += 1;

	return span_create(location, v->element_width);
}

Span
vector_push(Vector* v, Span data) {
	assert(span_width(data) == v->element_width );
	return push(v, data);
}

Span
vector_push_incomplete(Vector* v, Span data) {
	assert(span_width(data) <= v->element_width );
	return push(v, data);
}
