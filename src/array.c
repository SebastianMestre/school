#include "array.h"

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
	return span_create(arr->data.begin + arr->element_width * i, arr->element_width);
}

Iter
array_begin(Array const* arr) {
	return (Iter) { arr->data.begin, arr->element_width };
}

Iter
array_end(Array const* arr) {
	return (Iter) { arr->data.end, arr->element_width };
}
