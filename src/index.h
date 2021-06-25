#pragma once

#include "bst.h"
#include "contact.h"

typedef struct _Contacts Contacts;

struct _Index {
	Bst bst;
};
typedef struct _Index Index;

Index
index_create(Contacts* contacts);

void
index_insert(Index* index, ContactId id);

OptionalContactId
index_find(Index index, ContactId id);
