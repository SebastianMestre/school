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

void
database_release(Database* database);

// toma ownership de name y surname
OptionalContactId
database_find(Database* database, char* name, char* surname);

// toma ownership de name y surname
bool
database_has(Database* database_has, char* name, char* surname);

// toma ownership sobre, name, surname, age, y phone_number
bool
database_insert(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// toma ownership de name y surname
// devuelve false si no existe lo que se quiere borrar
bool
database_delete(Database* database, char* name, char* surname);

// toma ownership sobre age, y phone number
void
database_update(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownership
void
database_clear_history(Database* database);

// no toma ownership
// devuelve false sii no hay eventos para deshacer
bool
database_rewind_history(Database* database);

// no toma ownership
// devuelve false sii no hay eventos para reshacer
bool
database_advance_history(Database* database);
