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
index_insert(Index* index, ContactId id);

OptionalContactId
index_find(Index index, ContactId id);
