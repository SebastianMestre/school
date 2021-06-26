#pragma once

#include "contact.h"
#include "vector.h"

#include <stdint.h>

/*

Esta estructura guarda todos los Contact en un Vector.

Por cada Contact, tambien guarda algunas banderas, que se utilizan para saber
cuando es seguro eliminar un contacto, para abrir espacio a uno nuevo.

Aparte, en 'holes', guarda todas las posiciones vacias que hay en 'slots'.

*/

struct _Storage {
	Vector holes;
	Vector slots;
};
typedef struct _Storage Storage;

Storage
storage_create();

Contact*
storage_at(Storage storage, ContactId id);

// toma ownership de name, surname, age, y phone_number
ContactId
storage_insert(Storage* storage, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownership
void
storage_increase_refcount(Storage storage, ContactId id);

// no toma ownership
void
storage_decrease_refcount(Storage* storage, ContactId id);
