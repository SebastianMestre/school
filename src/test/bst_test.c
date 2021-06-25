#include "../bst.h"

#include "lt.h"

static int int_cmp_impl(int const* lhs, int const* rhs, void* metadata) {
	return *lhs - *rhs;
}

static int cmp_span_int(Span datum, int value) {
	return int_cmp_impl(datum.begin, &value, nullptr);
}

static ComparatorFunction int_cmp = (ComparatorFunction)int_cmp_impl;

#define CREATE_BST() bst_create(sizeof(int), (Comparator){int_cmp}, nop_dtor)

static void test_find_empty(int data) {
	LT_TEST

	Bst bst = CREATE_BST();

	// searching in an empty bst return nullptr
	LT_ASSERT(bst_find(bst, SPANOF(data)) == nullptr);
}

static void test_insert_empty(int data) {
	LT_TEST_ONCE

	Bst bst = CREATE_BST();

	BstInsertResult insert_result = bst_insert(&bst, SPANOF(data));

	// inserting into an empty bst should succeed
	LT_ASSERT(insert_result.success);
	// and the content of the node should be what we inserted
	LT_ASSERT(cmp_span_int(insert_result.position->datum, data) == 0);
}

static void test_insert_find(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	Bst bst = CREATE_BST();

	bst_insert(&bst, SPANOF(data));

	// finding a value that has been inserted should not return nullptr
	LT_ASSERT(bst_find(bst, SPANOF(data)) != nullptr);
}

static void test_double_insert(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	Bst bst = CREATE_BST();

	bst_insert(&bst, SPANOF(data));

	// inserting a value a second time should not succeed
	LT_ASSERT(!bst_insert(&bst, SPANOF(data)).success);
}

static void test_insert_different_values(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = CREATE_BST();

	bst_insert(&bst, SPANOF(data));

	BstInsertResult insert_result = bst_insert(&bst, SPANOF(other));

	// inserting a value different than the one that was inserted should succeed
	LT_ASSERT(insert_result.success);
	// and its content should be what we inserted
	LT_ASSERT(cmp_span_int(insert_result.position->datum, other) == 0);
}

static void test_find_different_values(int data) {
	LT_TEST_ONCE

	test_insert_different_values(data);
	test_insert_find(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = CREATE_BST();

	bst_insert(&bst, SPANOF(data));
	bst_insert(&bst, SPANOF(other));

	BstNode* p1 = bst_find(bst, SPANOF(data));
	BstNode* p2 = bst_find(bst, SPANOF(other));

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

