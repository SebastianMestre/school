#include "array.h"

#include <assert.h>

Array
array_malloc(size_t element_width, size_t size) {
	return (Array) {
		.data = span_malloc(element_width * size),
		.element_width = element_width,
		.size = size,
	};
}

void
array_free(Array* arr) {
	span_free(&arr->data);
}

Span
array_at(Array const* arr, size_t i) {
	assert(i < arr->size);
	return span_create(arr->data.begin + arr->element_width * i, arr->element_width);
}
