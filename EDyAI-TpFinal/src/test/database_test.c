#include "database_test.h"

#include "lt.h"

#include <string.h>

#include "../storage.h"
#include "../database.h"
#include "../string.h"
#include "../quicksort.h"

#define UNUSED(v) { void* x = &v; x = x; }

// A lo largo de este archivo, los valores que se insertan en database son
// completamente arbitrarios

static void run_test(void (*test)(Storage*, Database*, int), int arg) {
	Storage st = storage_create();
	Database db = database_create(&st);
	test(&st, &db, arg);
	database_release(&db);
	storage_release(&st);
}

static int
by_name_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Storage* storage = metadata;
	return strcmp(
		storage_get_name(storage, *lhs),
		storage_get_name(storage, *rhs));
}

static void
test_regr0(Storage* st, Database* db, int arg) {
	LT_TEST_ONCE;

	UNUSED(st);
	UNUSED(arg);

	// Este test de regresion no tiene asserts, la idea es correrlo en valgrind
	// para verificar que no tiene memory leaks

	database_insert(
		db,
		string_dup("a"),
		string_dup("b"),
		0xc,
		string_dup("d"));

	database_rewind(db);
}

static void
test0(Storage* st, Database* db, int arg) {
	LT_TEST_ONCE;

	UNUSED(arg);

	database_insert(db, string_dup("asd"), string_dup("asd"), 123, string_dup("123"));
	database_insert(db, string_dup("qwe"), string_dup("qwe"), 123, string_dup("543"));
	database_insert(db, string_dup("dfg"), string_dup("dfg"), 25,  string_dup("321"));

	Vector contacts = database_contacts(db);

	Comparator cmp = { by_name_cmp, st };
	quicksort(
		vector_begin(&contacts),
		vector_end(&contacts),
		vector_element_width(&contacts),
		cmp);

	for (size_t i = 1; i < contacts.size; ++i) {
		void* lhs = vector_at(&contacts, i-1).begin;
		void* rhs = vector_at(&contacts, i).begin;
		LT_ASSERT(call_cmp(cmp, lhs, rhs) < 0);
	}

	vector_release(&contacts);
}

static void
test1(Storage* st, Database* db, int arg) {
	LT_TEST_ONCE

	UNUSED(st);
	UNUSED(arg);

	database_insert(db, string_dup("asd"), string_dup("asd"), 123, string_dup("123"));
	database_insert(db, string_dup("qwe"), string_dup("qwe"), 123, string_dup("543"));

	LT_ASSERT(database_find(db, string_dup("asd"), string_dup("asd")).active);
	LT_ASSERT(database_find(db, string_dup("qwe"), string_dup("qwe")).active);

	database_rewind(db);

	LT_ASSERT(!database_find(db, string_dup("qwe"), string_dup("qwe")).active);

	database_insert(db, string_dup("dfg"), string_dup("dfg"), 25,  string_dup("321"));

	LT_ASSERT(database_find(db, string_dup("dfg"), string_dup("dfg")).active);

	database_rewind(db);

	LT_ASSERT(!database_find(db, string_dup("dfg"), string_dup("dfg")).active);

	database_advance(db);

	LT_ASSERT(database_find(db, string_dup("dfg"), string_dup("dfg")).active);
}

static void
test_all(int arg) {
	lt_reset();
	run_test(test_regr0, arg);
	run_test(test0, arg);
	run_test(test1, arg);
}

void
database_test() {
	LT_ITERATE(test_all);
	lt_report("Database");
}
