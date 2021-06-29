#include "database_test.h"

#include "lt.h"

#include <string.h>

#include "../storage.h"
#include "../database.h"
#include "../string.h"
#include "../quicksort.h"

static int
by_name_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Database* database = metadata;
	Storage* storage = database->storage;
	return strcmp(storage_at(storage, *lhs)->name, storage_at(storage, *rhs)->name);
}

static void
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
test0(int arg) {
	LT_TEST_ONCE;

	arg = arg;

	Storage st = storage_create();
	Database db = database_create(&st);

	database_insert(&db, string_dup("asd"), string_dup("asd"), 123, string_dup("123"));
	database_insert(&db, string_dup("qwe"), string_dup("qwe"), 123, string_dup("543"));
	database_insert(&db, string_dup("dfg"), string_dup("dfg"), 25,  string_dup("321"));

	Vector contacts = database_contacts(&db);

	Comparator cmp = { by_name_cmp, &db };
	quicksort(vector_full_segment(&contacts), cmp);

	for (size_t i = 1; i < contacts.size; ++i) {
		void* lhs = vector_at(&contacts, i-1).begin;
		void* rhs = vector_at(&contacts, i).begin;
		LT_ASSERT(call_cmp(cmp, lhs, rhs) < 0);
	}

	vector_release(&contacts);
	database_release(&db);
	storage_release(&st);
}

static void
test_all(int arg) {
	lt_reset();
	test_leak1(arg);
	test0(arg);
}

void
database_test() {
	LT_ITERATE(test_all);
	lt_report("Database");
}
