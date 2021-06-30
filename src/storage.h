#pragma once

#include "contact.h"
#include "slot_map.h"

#include <stdint.h>

/*

Esta estructura guarda todos los Contact en un Vector.

Por cada Contact, tambien guarda algunas banderas, que se utilizan para saber
cuando es seguro eliminar un contacto, para abrir espacio a uno nuevo.

Aparte, en 'holes', guarda todas las posiciones vacias que hay en 'slots'.

*/

struct _Storage {
	SlotMap slot_map;
};
typedef struct _Storage Storage;

Storage
storage_create();

void
storage_release(Storage* storage);

Contact*
storage_at(Storage* storage, ContactId id);

// toma ownership de name, surname, age, y phone_number
ContactId
storage_insert(Storage* storage, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownership
void
storage_delete(Storage* storage, ContactId id);

// no toma ownership
void
storage_increase_refcount(Storage* storage, ContactId id);

// no toma ownership
void
storage_decrease_refcount(Storage* storage, ContactId id);

char const*
storage_get_name(Storage* storage, ContactId id);

char const*
storage_get_surname(Storage* storage, ContactId id);

char const*
storage_get_phone_number(Storage* storage, ContactId id);

uint32_t
storage_get_age(Storage* storage, ContactId id);

bool
storage_is_indexed(Storage* storage, ContactId id);

void
storage_mark_indexed(Storage* storage, ContactId id);

void
storage_mark_not_indexed(Storage* storage, ContactId id);

size_t
storage_indexed_count(Storage const* storage);

// aplica el callback a cada contacto
void
storage_for_each(Storage const* storage, Callback cb);

void
storage_for_each_indexed(Storage const* storage, Callback cb);
