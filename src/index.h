#pragma once

#include "bst.h"
#include "contact.h"

typedef struct _Storage Storage;

struct _Index {
	Storage* storage;
	Bst bst;
};
typedef struct _Index Index;

Index
index_create(Storage* storage);

void
index_release(Index* index);

bool
index_insert(Index* index, ContactId id);

// TODO: return success status?
void
index_delete(Index* index, ContactId id);

// Si existe en el indice, devuelve un id de contacto, con el mismo nombre y
// apellido del que se pasa.
OptionalContactId
index_find(Index index, ContactId id);
