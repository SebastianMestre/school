#pragma once

#include "contact.h"
#include "index.h"
#include "history.h"
#include "vector.h"

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
database_has(Database* database, char* name, char* surname);

// toma ownership sobre, name, surname, age, y phone_number
bool
database_insert(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// toma ownership de name y surname
// devuelve false si no existe lo que se quiere borrar
bool
database_delete(Database* database, char* name, char* surname);

// toma ownership sobre name, surname, age, y phone number
bool
database_update(Database* database, char* name, char* surname, unsigned age, char* phone_number);

// no toma ownership
// deshace el ultimo evento
// devuelve false sii no hay eventos para deshacer
bool
database_rewind(Database* database);

// no toma ownership
// rehace el ultimo evento que fue deshecho
// devuelve false sii no hay eventos para rehacer
bool
database_advance(Database* database);

// devuelve los Ids de todos los contactos activos
Vector
database_contacts(Database* database);

// no toma ownership
// devuelve punteros a todos los contactos que cumplen todas las condiciones
Vector
database_query_and(Database* database, IncompleteContact query);

// no toma ownership
// devuelve punteros a todos los contactos que cumplen alguna de las condiciones
Vector
database_query_or(Database* database, IncompleteContact query);
