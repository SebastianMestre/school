#include "circular_buffer.h"

#include <assert.h>
#include <stdlib.h>

CircularBuffer
circular_buffer_create(
	size_t element_width,
	size_t capacity,
	Destructor dtor
) {
	assert(element_width != 0);
	assert(capacity != 0);

	size_t data_width = element_width * capacity;
	void* data = malloc(data_width);
	return (CircularBuffer) {
		.data = {
			.begin = data,
			.end = data + data_width,
		},
		.begin = data,
		.end = data,

		.element_width = element_width,
		.capacity = capacity,
		.size = 0,

		.dtor = dtor,
	};
}

static void
advance_and_wrap(void** ptr, size_t stride, Span range) {
	*ptr += stride;
	if (*ptr == range.end)
		*ptr = range.begin;
}

static void
retreat_and_wrap(void** ptr, size_t stride, Span range) {
	if (*ptr == range.begin)
		*ptr = range.end;
	*ptr -= stride;
}

void
circular_buffer_pop_front(CircularBuffer* buffer) {
	assert(buffer->size != 0);

	call_dtor(buffer->dtor, buffer->begin);
	advance_and_wrap(&buffer->begin, buffer->element_width, buffer->data);

	buffer->size -= 1;
}

void
circular_buffer_pop_back(CircularBuffer* buffer) {
	assert(buffer->size != 0);

	retreat_and_wrap(&buffer->end, buffer->element_width, buffer->data);
	call_dtor(buffer->dtor, buffer->end);

	buffer->size -= 1;
}

void
circular_buffer_push_back(CircularBuffer* buffer, Span data) {
	assert(span_width(data) == buffer->element_width);

	if (buffer->begin == buffer->end && buffer->size != 0) {
		assert(buffer->size == buffer->capacity);
		circular_buffer_pop_front(buffer);
	}

	span_write(buffer->end, data);
	advance_and_wrap(&buffer->end, buffer->element_width, buffer->data);

	buffer->size += 1;
}

void
circular_buffer_release(CircularBuffer* buffer) {
	free(buffer->data.begin);
	buffer->data = (Span){};
	buffer->begin = nullptr;
	buffer->end = nullptr;
}
