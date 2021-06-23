#include "../bst.h"

#include "lt.h"

static int int_cmp(int const* lhs, int const* rhs) {
	return lhs - rhs;
}

static void test_find_empty(int data) {
	LT_TEST

	Bst bst = bst_create((Comparator)int_cmp);

	// searching in an empty bst return nullptr
	LT_ASSERT(bst_find(bst, &data) == nullptr);
}

static void test_insert_empty(int data) {
	LT_TEST
	LT_ONCE

	Bst bst = bst_create((Comparator)int_cmp);

	BstInsertResult insert_result = bst_insert(&bst, &data);

	// inserting into an empty bst should succeed
	LT_ASSERT(insert_result.success);
	// and the content of the node should be what we inserted
	LT_ASSERT(insert_result.position->datum == &data);
}

static void test_insert_find(int data) {
	LT_TEST
	LT_ONCE

	test_insert_empty(data);

	Bst bst = bst_create((Comparator)int_cmp);

	bst_insert(&bst, &data);

	// finding a value that has been inserted should not return nullptr
	LT_ASSERT(bst_find(bst, &data) != nullptr);
}

static void test_double_insert(int data) {
	LT_TEST
	LT_ONCE

	test_insert_empty(data);

	Bst bst = bst_create((Comparator)int_cmp);

	bst_insert(&bst, &data);

	// inserting a value a second time should not succeed
	LT_ASSERT(!bst_insert(&bst, &data).success);
}

static void test_insert_different_values(int data) {
	LT_TEST
	LT_ONCE

	test_insert_empty(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = bst_create((Comparator)int_cmp);

	bst_insert(&bst, &data);

	BstInsertResult insert_result = bst_insert(&bst, &other);

	// inserting a value different than the one that was inserted should succeed
	LT_ASSERT(insert_result.success);
	// and its content should be what we inserted
	LT_ASSERT(insert_result.position->datum == &other);
}

static void test_find_different_values(int data) {
	LT_TEST
	LT_ONCE

	test_insert_different_values(data);
	test_insert_find(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = bst_create((Comparator)int_cmp);

	bst_insert(&bst, &data);
	bst_insert(&bst, &other);

	BstNode* p1 = bst_find(bst, &data);
	BstNode* p2 = bst_find(bst, &other);

	LT_ASSERT(p1 != nullptr);
	LT_ASSERT(p2 != nullptr);
	LT_ASSERT(p1 != p2);
}

void test_all(int value) {
	lt_reset();
	test_find_empty(value);
	test_insert_empty(value);
	test_insert_find(value);
	test_double_insert(value);
	test_insert_different_values(value);
	test_find_different_values(value);
}

void bst_test() {
	LT_ITERATE(test_all);
	lt_report("Bst");
}

