#pragma once

#include "array.h"
#include "span.h"

#include <stddef.h>

// Un vector guarda secuencias de bytes, cada una del mismo ancho.
struct _Vector {
	Array buffer;
	size_t size;
};
typedef struct _Vector Vector;

Vector
vector_create(size_t element_width);

Span
vector_at(Vector const* v, size_t i);

Span
vector_last(Vector const* v);

void
vector_put_at(Vector* v, size_t i, Span data);

void
vector_resize_storage(Vector* v, size_t n);

// inserta un dato al final del vector
// devuelve el espacio de memoria donde quedaron los datos
void
vector_push(Vector* v, Span data);

// elimina el dato del final del vector
void
vector_pop(Vector* v);

void
vector_release(Vector* v);

// devuelve un array que apunta a la porcion llena del vector.
Array
vector_full_segment(Vector* v);
