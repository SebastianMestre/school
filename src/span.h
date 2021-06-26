#pragma once

#include <stddef.h>

// Un span es esencialemente un puntero a una region de memoria.
// A diferencia de un puntero comun, tambien sabe el final de la region.
// Como cualquier puntero, no necesariamente tiene ownership.
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

