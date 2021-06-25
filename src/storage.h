#pragma once

#include "contact.h"
#include "vector.h"

#include <stdint.h>

#define CONTACT_ACTIVE                    (0x1)
#define CONTACT_REFERENCED_IN_INDEX       (0x2)
#define CONTACT_REFERENCED_IN_HISTORY_FWD (0x4)
#define CONTACT_REFERENCED_IN_HISTORY_BWD (0x8)

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

void
storage_set_flags(Storage storage, ContactId id, uint8_t flags);

void
storage_unset_flags(Storage* storage, ContactId id, uint8_t flags);
