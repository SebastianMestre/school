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

static void
call_if_indexed(void* arg, void* metadata) {
	Contact* contact = arg;
	Callback* cb = metadata;

	if (contact->indexed) {
		call_cb(*cb, contact);
	}
}

void
database_for_each(Database* database, Callback cb) {
	storage_for_each(database->storage, (Callback){call_if_indexed, &cb});
}

Vector
database_contacts(Database* database) {
	Vector result = vector_create(sizeof(ContactId));
	for (size_t i = 0; i < database->storage->slot_map.cells.size; ++i) {
		ContactId slot = slot_map_get_cell_slot(&database->storage->slot_map, i);
		if (storage_at(database->storage, slot)->indexed) {
			vector_push(&result, SPANOF(slot));
		}
	}
	return result;
}

typedef struct {
	Vector out;
	QueryData query_data;
} QueryHelper;

static void
query_and_cb(void* arg, void* metadata) {
	Contact* contact = arg;
	QueryHelper* helper = metadata;

	if (!contact->indexed) {
		return;
	}


	bool c0 = helper->query_data.name == nullptr ||
		strcmp(helper->query_data.name, contact->name) == 0;

	bool c1 = helper->query_data.surname == nullptr ||
		strcmp(helper->query_data.surname, contact->surname) == 0;

	bool c2 = !helper->query_data.has_age ||
		helper->query_data.age == contact->age;

	bool c3 = helper->query_data.phone_number == nullptr ||
		strcmp(helper->query_data.phone_number, contact->phone_number) == 0;

	if (c0 && c1 && c2 && c3) {
		vector_push(&helper->out, SPANOF(contact));
	}
}

Vector
database_query_and(Database* database, QueryData query_data) {
	QueryHelper helper = {
		.out = vector_create(sizeof(Contact*)),
		.query_data = query_data,
	};
	storage_for_each(database->storage, (Callback){ query_and_cb, &helper });
	return helper.out;
}

static void
query_or_cb(void* arg, void* metadata) {
	Contact* contact = arg;
	QueryHelper* helper = metadata;

	if (!contact->indexed) {
		return;
	}

	bool c0 = helper->query_data.name != nullptr &&
		strcmp(helper->query_data.name, contact->name) == 0;

	bool c1 = helper->query_data.surname != nullptr &&
		strcmp(helper->query_data.surname, contact->surname) == 0;

	bool c2 = helper->query_data.has_age &&
		helper->query_data.age == contact->age;

	bool c3 = helper->query_data.phone_number != nullptr &&
		strcmp(helper->query_data.phone_number, contact->phone_number) == 0;

	if (c0 || c1 || c2 || c3) {
		vector_push(&helper->out, SPANOF(contact));
	}
}

Vector
database_query_or(Database* database, QueryData query_data) {
	QueryHelper helper = {
		.out = vector_create(sizeof(Contact*)),
		.query_data = query_data,
	};
	storage_for_each(database->storage, (Callback){ query_or_cb, &helper });
	return helper.out;
}
