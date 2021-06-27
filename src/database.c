#include "database.h"

#include "storage.h"

#include <assert.h>

// TODO: delete future events when inserting/deleting/updating

// Nota de implementacion: advance/rewind asumen que las operacione que hacen
// siempre son exitosas. Para que esto sea verdad, tenemos que tener mucho
// cuidado de no meter operaciones invalidas en la historia.

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

bool
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
		storage_delete(database->storage, id);
		return false;
	}

	history_clear_future(&database->history);
	history_record_inserted(&database->history, id);

	bool success = index_insert(&database->index, id);
	assert(success);
	history_advance_cursor(&database->history);

	return true;
}

bool
database_delete(Database* database, char* name, char* surname) {
	OptionalContactId found = database_find(database, name, surname);
	if (!found.active) {
		return false;
	}

	ContactId id = found.id;

	history_clear_future(&database->history);
	history_record_deleted(&database->history, id);

	index_delete(&database->index, id);
	history_advance_cursor(&database->history);

	return true;
}

OptionalContactId
database_find(Database* database, char* name, char* surname) {
	ContactId temp_id = storage_insert(database->storage, name, surname, 0, nullptr);
	OptionalContactId found = index_find(database->index, temp_id);
	storage_delete(database->storage, temp_id);
	return found;
}

bool
database_rewind(Database* database) {
	if (history_cursor_at_begin(&database->history))
		return false;

	history_retreat_cursor(&database->history);
	HistoryEvent* action = history_next_action(&database->history);

	if (action->forwards.active) {
		index_delete(&database->index, action->forwards.id);
	}

	if (action->backwards.active) {
		bool success = index_insert(&database->index, action->backwards.id);
		assert(success);
	}

	return true;
}

bool
database_advance(Database* database) {
	if (history_cursor_at_end(&database->history))
		return false;

	HistoryEvent* action = history_next_action(&database->history);
	history_advance_cursor(&database->history);

	if (action->backwards.active) {
		index_delete(&database->index, action->backwards.id);
	}

	if (action->forwards.active) {
		bool success = index_insert(&database->index, action->forwards.id);
		assert(success);
	}

	return true;
}
