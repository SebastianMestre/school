#include "byte_span.h"

#include <string.h>

ByteSpan
byte_span_create(void* begin, size_t length) {
	return (ByteSpan) {
		.begin = begin,
		.end = begin + length
	};
}

size_t
byte_span_length(ByteSpan span) {
	return span.end - span.begin;
}

void
byte_span_write(void* destination, ByteSpan span) {
	memcpy(destination, span.begin, byte_span_length(span));
}


