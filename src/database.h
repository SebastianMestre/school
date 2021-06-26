#pragma once

#include "contact.h"
#include "index.h"
#include "history.h"

typedef struct _Storage Storage;

struct _Database {
	Storage* storage;
	Index index;
	History history;
};
typedef struct _Database Database;

// TODO: devolver status?

// no toma ownership
Database
database_create(Storage* storage);

// no toma ownership
OptionalContactId
database_find(Database database, char* name, char* surname);

// no toma ownership
bool database_has(Database database_has, char* name, char* surname);

// toma ownership sobre, name, surname, age, y phone_number
void
database_insert(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownership
void
database_delete(Database* database, char* name, char* surname);

// toma ownership sobre age, y phone number
void
database_update(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownrship
void
database_clear_history(Database* database);

// no toma ownership
void
database_undo(Database* database);

// no toma ownership
void
database_redo(Database* database);
