#pragma once

#include "span.h"

#include <stddef.h>

// una cola circular
// en el contexto de este archivo 'eliminar' involucra llamar el destructor

struct _CircularBuffer {
	Span data;

	void* begin;
	void* end;

	size_t element_width;
	size_t capacity;
	size_t size;

	Destructor dtor;
};
typedef struct _CircularBuffer CircularBuffer;

// crea una cola circular de elementos del tamanno especificado, con la
// capacidad especificada
CircularBuffer
circular_buffer_create(size_t element_width, size_t capacity, Destructor dtor);

// elimina todos los datos de la cola, y libera el bufer
void
circular_buffer_release(CircularBuffer* buffer);

// elimina todos los datos de la cola
void
circular_buffer_clear(CircularBuffer* buffer);

// elimina un dato del principio de la cola
void
circular_buffer_pop_front(CircularBuffer* buffer);

// elimina un dato del final de la cola
void
circular_buffer_pop_back(CircularBuffer* buffer);

// agrega un dato al final de la cola
void
circular_buffer_push_back(CircularBuffer* buffer, Span data);
