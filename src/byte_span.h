#pragma once

#include <stddef.h>

// A bytespan is a pointer to a piece of memory.
// Like any other pointer, it does not necesarily own what it points to.
struct _ByteSpan {
	void* begin;
	void* end;
};
typedef struct _ByteSpan ByteSpan;

ByteSpan
byte_span_create(void* begin, size_t length);

size_t
byte_span_length(ByteSpan span);

void
byte_span_write(void* destination, ByteSpan span);


