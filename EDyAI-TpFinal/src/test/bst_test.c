#include "../bst.h"

#include "lt.h"

static int cmp(size_t lhs, size_t rhs) {
	if (lhs < rhs) return -1;
	return rhs < lhs;
}

static int id_cmp(void const* arg0, void const* arg1, void* metadata) {
	metadata = metadata;
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	return cmp(*lhs, *rhs);
}

#define CREATE_BST() bst_create((Comparator){id_cmp, nullptr}, nop_dtor)

static void
run_test(void (*test)(Bst*, int), int arg) {
	Bst bst = CREATE_BST();
	test(&bst, arg);
	bst_release(&bst);
}

static void test_find_empty(Bst* bst, int arg) {
	LT_TEST

	// searching in an empty bst returns nullptr
	ContactId id = arg;
	LT_ASSERT(bst_find(bst, id) == nullptr);
}

static void test_insert_empty(Bst* bst, int arg) {
	LT_TEST_ONCE

	ContactId id = arg;
	BstInsertResult insert_result = bst_insert(bst, id);

	// inserting into an empty bst should succeed
	LT_ASSERT(insert_result.success);
	// and the content of the node should be what we inserted
	LT_ASSERT(insert_result.position->data == id);
}

static void test_insert_find(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_empty, data);

	bst_insert(bst, data);

	// finding a value that has been inserted should not return nullptr
	LT_ASSERT(bst_find(bst, data) != nullptr);
}

static void test_double_insert(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_empty, data);

	bst_insert(bst, data);

	// inserting a value a second time should not succeed
	LT_ASSERT(!bst_insert(bst, data).success);
}

static void test_insert_different_values(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_empty, data);

	int other = (data % 1000) * 19;
	if (data == other) return;

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(bst, id1);

	BstInsertResult insert_result = bst_insert(bst, id2);

	// inserting a value different than the one that was inserted should succeed
	LT_ASSERT(insert_result.success);
	// and its content should be what we inserted
	LT_ASSERT(insert_result.position->data == id2);
}

static void test_find_different_values(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_different_values, data);
	run_test(test_insert_find, data);

	int other = (data % 1000) * 19;
	if (data == other) return;

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(bst, id1);
	bst_insert(bst, id2);

	BstNode* p1 = bst_find(bst, id1);
	BstNode* p2 = bst_find(bst, id2);

	LT_ASSERT(p1 != nullptr);
	LT_ASSERT(p2 != nullptr);
	LT_ASSERT(p1 != p2);
}

static void test_erase_then_find_fails(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_empty, data);
	run_test(test_insert_find, data);

	ContactId id = data;

	bst_insert(bst, id);
	bst_erase(bst, id);

	BstNode* p = bst_find(bst, id);

	LT_ASSERT(p == nullptr);
}

static void test_erase_one_not_the_other(Bst* bst, int data) {
	LT_TEST_ONCE

	run_test(test_insert_different_values, data);
	run_test(test_insert_find, data);

	int other = (data % 1000) * 19;
	if (data == other) return;

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(bst, id1);
	bst_insert(bst, id2);

	bst_erase(bst, id2);

	BstNode* p1 = bst_find(bst, id1);
	BstNode* p2 = bst_find(bst, id2);

	LT_ASSERT(p1 != nullptr);
	LT_ASSERT(p2 == nullptr);
}

static void test_random_values(Bst* bst, int data) {

	lt_srand(data);

	ContactId x = lt_rand();
	bst_insert(bst, x);

	for (int i = 0; i < 100; ++i) {
		ContactId y = lt_rand();
		bst_insert(bst, y);
	}

	LT_ASSERT(bst_find(bst, x) != nullptr);
	bst_erase(bst, x);
	LT_ASSERT(bst_find(bst, x) == nullptr);
}

static void test_all(int value) {
	lt_reset();
	run_test(test_find_empty, value);
	run_test(test_insert_empty, value);
	run_test(test_insert_find, value);
	run_test(test_double_insert, value);
	run_test(test_insert_different_values, value);
	run_test(test_find_different_values, value);
	run_test(test_erase_then_find_fails, value);
	run_test(test_erase_one_not_the_other, value);
	run_test(test_random_values, value);
}

void bst_test() {
	LT_ITERATE(test_all);
	lt_report("Bst");
}

