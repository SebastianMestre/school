#pragma once

#include "byte_span.h"

#include <stddef.h>

// INVARIANT: data.width = element_width * capacity
// INVARIANT: begin and end point into the data span
struct _CircularBuffer {
	ByteSpan data;

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

CircularBuffer
circular_buffer_create_trival(size_t element_width, size_t capacity);


void
circular_buffer_pop_front(CircularBuffer* buffer);

void
circular_buffer_pop_back(CircularBuffer* buffer);

void
circular_buffer_push_back(CircularBuffer* buffer, ByteSpan data);
