#include "database_test.h"

#include "lt.h"

#include "../storage.h"
#include "../database.h"
#include "../string.h"

void
test_leak1(int arg) {
	LT_TEST_ONCE;

	arg = arg;
	
	Storage st = storage_create();
	Database db = database_create(&st);

	database_insert(
		&db,
		string_dup("a"),
		string_dup("b"),
		0xc,
		string_dup("d"));

	database_rewind(&db);

	database_release(&db);
	storage_release(&st);
}

static void
test_all(int arg) {
	lt_reset();
	test_leak1(arg);
}

void
database_test() {
	LT_ITERATE(test_all);
	lt_report("Database");
}
