#include "database.h"

#include "storage.h"

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
}
