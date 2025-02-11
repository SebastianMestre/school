#pragma once

#include "span.h"

struct _Array {
	Span data;
	size_t element_width;
	size_t size;
};
typedef struct _Array Array;

// reserva espacio para size elementos de tamanno element_width
// y devuelve un array que apunta a ellos
Array
array_malloc(size_t element_width, size_t size);

// llama free sobre el puntero a los datos de arr
void
array_free(Array* arr);

// devuelve el span del i-esimo elemento de arr
Span
array_at(Array const* arr, size_t i);
