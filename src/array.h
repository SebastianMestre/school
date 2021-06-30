#pragma once

#include "span.h"

struct _Array {
	Span data;
	size_t element_width;
	size_t size;
};
typedef struct _Array Array;

Array
array_malloc(size_t element_width, size_t size);

void
array_free(Array* arr);

Span
array_at(Array const* arr, size_t i);
