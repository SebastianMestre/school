#include "database.h"

#include "storage.h"

Database
database_create(Storage* storage) {
	return (Database) {
		.index = index_create(storage),
		.history = history_create(storage),
	};
}
