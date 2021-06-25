#pragma once

#include "span.h"

#include <stddef.h>

// INVARIANT: data.width = element_width * capacity
// INVARIANT: begin and end point into the data span
struct _CircularBuffer {
	Span data;

	void* begin;
	void* end;

	size_t element_width;
	size_t capacity;
	size_t size;

	Destructor dtor;
};
typedef struct _CircularBuffer CircularBuffer;

CircularBuffer
circular_buffer_create(size_t element_width, size_t capacity, Destructor dtor);

void
circular_buffer_pop_front(CircularBuffer* buffer);

void
circular_buffer_pop_back(CircularBuffer* buffer);

void
circular_buffer_push_back(CircularBuffer* buffer, Span data);

void
circular_buffer_release(CircularBuffer* buffer);
