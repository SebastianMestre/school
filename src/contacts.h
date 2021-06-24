#pragma once

#include "contact.h"
#include "vector.h"

#include <stdint.h>

#define CONTACT_ACTIVE                    (0x1)
#define CONTACT_REFERENCED_IN_BST         (0x2)
#define CONTACT_REFERENCED_IN_HISTORY_FWD (0x4)
#define CONTACT_REFERENCED_IN_HISTORY_BWD (0x8)

/*

Esta estructura guarda todos los Contact en un Vector.

Por cada Contact, tambien guarda algunas banderas, que se utilizan para saber
cuando es seguro eliminar un contacto, para abrir espacio a uno nuevo.

*/

struct _Contacts {
	Vector holes;
	Vector slots;
};
typedef struct _Contacts Contacts;

Contacts
contacts_create();

void
contacts_set_flags(Contacts contacts, ContactId id, uint8_t flags);

void
contacts_unset_flags(Contacts* contacts, ContactId id, uint8_t flags);
