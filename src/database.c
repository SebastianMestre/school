#include "database.h"

#include "storage.h"
#include "string.h"

#include <string.h>
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
	string_trim(name);
	string_tolower(name);

	string_trim(surname);
	string_tolower(surname);

	string_trim(phone_number);

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
	string_trim(name);
	string_tolower(name);

	string_trim(surname);
	string_tolower(surname);

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

bool
database_update(
	Database* database,
	char* name,
	char* surname,
	unsigned age,
	char* phone_number
) {
	string_trim(name);
	string_tolower(name);

	string_trim(surname);
	string_tolower(surname);

	string_trim(phone_number);

	ContactId new_id = storage_insert(database->storage, name, surname, age, phone_number);
	OptionalContactId found = index_find(database->index, new_id);

	if (!found.active) {
		storage_delete(database->storage, new_id);
		return false;
	}

	ContactId old_id = found.id;

	history_clear_future(&database->history);
	history_record_updated(&database->history, old_id, new_id);

	index_delete(&database->index, old_id);
	bool success = index_insert(&database->index, new_id);
	assert(success);
	history_advance_cursor(&database->history);

	return true;
}

OptionalContactId
database_find(Database* database, char* name, char* surname) {
	string_trim(name);
	string_tolower(name);

	string_trim(surname);
	string_tolower(surname);

	ContactId temp_id = storage_insert(database->storage, name, surname, 0, nullptr);
	OptionalContactId found = index_find(database->index, temp_id);
	storage_delete(database->storage, temp_id);
	return found;
}

bool
database_has(Database* database, char* name, char* surname) {
	string_trim(name);
	string_tolower(name);

	string_trim(surname);
	string_tolower(surname);

	OptionalContactId found = database_find(database, name, surname);
	return found.active;
}

void
database_clear_history(Database* database) {
	history_clear(&database->history);
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

Vector
database_contacts(Database* database) {
	Vector result = vector_create(sizeof(ContactId));
	for (size_t i = 0; i < database->storage->slot_map.cells.size; ++i) {
		ContactId slot = slot_map_get_cell_slot(&database->storage->slot_map, i);
		if (storage_is_indexed(database->storage, slot)) {
			vector_push(&result, SPANOF(slot));
		}
	}
	return result;
}

Vector
database_query_and(Database* database, IncompleteContact query_data) {
	Vector result = vector_create(sizeof(Contact*));
	Contact* const l = storage_begin(database->storage);
	Contact* const r = storage_end(database->storage);
	for (Contact* it = l; it < r; ++it) {
		bool c0 = query_data.name == nullptr ||
			strcmp(query_data.name, it->name) == 0;

		bool c1 = query_data.surname == nullptr ||
			strcmp(query_data.surname, it->surname) == 0;

		bool c2 = !query_data.has_age ||
			query_data.age == it->age;

		bool c3 = query_data.phone_number == nullptr ||
			strcmp(query_data.phone_number, it->phone_number) == 0;

		if (c0 && c1 && c2 && c3)
			vector_push(&result, SPANOF(it));
	}
	return result;
}

Vector
database_query_or(Database* database, IncompleteContact query_data) {
	Vector result = vector_create(sizeof(Contact*));
	Contact* const l = storage_begin(database->storage);
	Contact* const r = storage_end(database->storage);
	for (Contact* it = l; it < r; ++it) {
		bool c0 = query_data.name != nullptr &&
			strcmp(query_data.name, it->name) == 0;

		bool c1 = query_data.surname != nullptr &&
			strcmp(query_data.surname, it->surname) == 0;

		bool c2 = query_data.has_age &&
			query_data.age == it->age;

		bool c3 = query_data.phone_number != nullptr &&
			strcmp(query_data.phone_number, it->phone_number) == 0;

		if (c0 || c1 || c2 || c3)
			vector_push(&result, SPANOF(it));
	}
	return result;
}
