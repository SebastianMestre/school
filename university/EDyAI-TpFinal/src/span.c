#include "span.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

Span
span_create(void* begin, size_t width) {
	return (Span) {
		.begin = begin,
		.end = begin + width
	};
}

Span
span_malloc(size_t width) {
	return span_create(malloc(width), width);
}

void
span_free(Span* span) {
	free(span->begin);
	span->begin = nullptr;
	span->end = nullptr;
}

Span
span_at(Span span, size_t element_width, size_t i) {
	assert(span_width(span) >= element_width*(i+1));
	return span_create(span.begin + element_width * i, element_width);
}

Span
span_slice(Span span, size_t offset, size_t width) {
	assert(offset + width <= span_width(span));
	return (Span){
		span.begin + offset,
		span.begin + offset + width,
	};
}

size_t
span_width(Span span) {
	return span.end - span.begin;
}

void
span_write(void* destination, Span span) {
	memcpy(destination, span.begin, span_width(span));
}

Span remove_prefix(Span span, size_t width) {
	assert(width <= span_width(span));
	return (Span){
		.begin = span.begin + width,
		.end = span.end,
	};
}

Span
fill_prefix(Span destination, Span data) {
	assert(span_width(data) <= span_width(destination));
	span_write(destination.begin, data);
	return remove_prefix(destination, span_width(data));
}
