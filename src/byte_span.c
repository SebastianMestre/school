#include "byte_span.h"

#include <assert.h>
#include <string.h>

ByteSpan
byte_span_create(void* begin, size_t width) {
	return (ByteSpan) {
		.begin = begin,
		.end = begin + width
	};
}

ByteSpan
byte_span_slice(ByteSpan span, size_t offset, size_t width) {
	assert(offset + width <= byte_span_width(span));
	return (ByteSpan){
		span.begin + offset,
		span.begin + offset + width,
	};
}

size_t
byte_span_width(ByteSpan span) {
	return span.end - span.begin;
}

void
byte_span_write(void* destination, ByteSpan span) {
	memcpy(destination, span.begin, byte_span_width(span));
}
