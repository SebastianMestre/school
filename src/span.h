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

// reserva un espacio de memoria, y devuelve un span que aputa a el
Span
span_malloc(size_t width);

// dado un puntero a un espacio de memoria y un tamano, devuelve su Span
Span
span_create(void* begin, size_t width);

// devuelve un 'subspan' que inicia en la posicion offset, y tiene largo length
Span
span_slice(Span span, size_t offset, size_t length);

// devuelve el tamano de un span
size_t
span_width(Span span);

// escribe los datos de un span en un espacio de memoria
void
span_write(void* destination, Span source);

// escribe los datos de un span en un espacio de memoria, devuelve el espacio
// restante.
Span
fill_prefix(Span destination, Span data);

Span
remove_prefix(Span destination, size_t width);

// devuelve el i-esimo bloque de ancho element_width en span
Span
span_at(Span span, size_t element_width, size_t i);

#define SPANOF(x) span_create(&(x), sizeof(x))

