#pragma once

#include <stddef.h>

// A span is essentially a pointer to a piece of memory. Unlike a regular
// pointer, it knows the extent of what it points to.
// Like any other pointer, it does not necesarily own what it points to.
struct _Span {
	void* begin;
	void* end;
};
typedef struct _Span Span;

Span
span_malloc(size_t width);

Span
span_create(void* begin, size_t width);

Span
span_slice(Span span, size_t offset, size_t length);

size_t
span_width(Span span);

void
span_write(void* destination, Span source);

Span
fill_prefix(Span destination, Span data);

Span
remove_prefix(Span destination, size_t width);

#define SPANOF(x) span_create(&(x), sizeof(x))

