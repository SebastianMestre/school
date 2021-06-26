#include "database.h"

#include "storage.h"

#include <assert.h>

Database
database_create(Storage* storage) {
	return (Database) {
		.storage = storage,
		.index = index_create(storage),
		.history = history_create(storage),
	};
}

void
database_insert(
	Database* database,
	char* name,
	char* surname,
	unsigned age,
	char* phone_number
) {
	ContactId id = storage_insert(database->storage, name, surname, age, phone_number);
	index_insert(&database->index, id);
	history_record_inserted(&database->history, id);
}

void
database_delete(Database* database, char* name, char* surname) {
	ContactId temp_id = storage_insert(database->storage, name, surname, 0, nullptr);
	OptionalContactId found = index_find(database->index, temp_id);
	/* TODO if (!found.active) return false; */
	assert(found.active);
	ContactId real_id = found.id;
	history_record_deleted(&database->history, real_id);
	/* TODO bool did_delete = */ index_delete(&database->index, real_id);
	/* TODO assert(did_delete); */
	/* TODO return true; */
}

void
database_undo(Database* database) {
}

void
database_redo(Database* database) {
}
