#include "span.h"

#include <stddef.h>

// Un vector guarda secuencias de bytes, cada una del mismo ancho.
struct _Vector {
	Span data;

	size_t const element_width;
	size_t size;
	size_t capacity;
};
typedef struct _Vector Vector;

Vector
vector_create(size_t element_width);

Span
vector_at(Vector v, size_t i);

Span
vector_last(Vector v);

void
vector_put_at(Vector v, size_t i, Span data);

void
vector_resize_storage(Vector* v, size_t n);

// inserta data al final del vector
// devuelve el espacio de memoria donde quedaron los datos
Span
vector_push(Vector* v, Span data);

// inserta data al final del vector
// devuelve el espacio de memoria donde quedaron los datos
// tolera que data sea menos ancho que un elemento
Span
vector_push_incomplete(Vector* v, Span data);

void
vector_release(Vector* v);
