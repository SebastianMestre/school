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
database_release(Database* database) {
	index_release(&database->index);
	history_release(&database->history);
}

void
database_insert(
	Database* database,
	char* name,
	char* surname,
	unsigned age,
	char* phone_number
) {
	// TODO: what if it's already there?
	ContactId id = storage_insert(database->storage, name, surname, age, phone_number);
	OptionalContactId found = index_find(database->index, id);
	if (found.active) {
		return /* TODO false */;
	}

	history_record_inserted(&database->history, id);

}

bool
database_delete(Database* database, char* name, char* surname) {
	OptionalContactId found = database_find(database, name, surname);
	if (!found.active) {
		return false;
	}

	ContactId id = found.id;
	history_record_deleted(&database->history, id);
	index_delete(&database->index, id);
	return true;
}

OptionalContactId
database_find(Database* database, char* name, char* surname) {
	ContactId temp_id = storage_insert(database->storage, name, surname, 0, nullptr);
	OptionalContactId found = index_find(database->index, temp_id);
	storage_delete(database->storage, temp_id);
	return found;
}

void
database_undo(Database* database) {
}

void
database_advance_history(Database* database) {
	HistoryAction* action = history_next_action(&database->history);

	if (action->backwards.active) {
		/* TODO bool success = */ index_delete(&database->index, action->backwards.id);
		/* TODO assert(success); */
	}

	if (action->forwards.active) {
		/* TODO bool success = */ index_insert(&database->index, action->forwards.id);
		/* TODO assert(success); */
	}

	history_advance_cursor(&database->history);
}
