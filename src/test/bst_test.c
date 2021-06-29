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

static void test_find_empty(int arg) {
	LT_TEST

	Bst bst = CREATE_BST();

	// searching in an empty bst returns nullptr
	ContactId id = arg;
	LT_ASSERT(bst_find(bst, id) == nullptr);

	bst_release(&bst);
}

static void test_insert_empty(int arg) {
	LT_TEST_ONCE

	Bst bst = CREATE_BST();

	ContactId id = arg;
	BstInsertResult insert_result = bst_insert(&bst, id);

	// inserting into an empty bst should succeed
	LT_ASSERT(insert_result.success);
	// and the content of the node should be what we inserted
	LT_ASSERT(insert_result.position->data == id);

	bst_release(&bst);
}

static void test_insert_find(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	Bst bst = CREATE_BST();

	bst_insert(&bst, data);

	// finding a value that has been inserted should not return nullptr
	LT_ASSERT(bst_find(bst, data) != nullptr);

	bst_release(&bst);
}

static void test_double_insert(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	Bst bst = CREATE_BST();

	bst_insert(&bst, data);

	// inserting a value a second time should not succeed
	LT_ASSERT(!bst_insert(&bst, data).success);

	bst_release(&bst);
}

static void test_insert_different_values(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = CREATE_BST();

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(&bst, id1);

	BstInsertResult insert_result = bst_insert(&bst, id2);

	// inserting a value different than the one that was inserted should succeed
	LT_ASSERT(insert_result.success);
	// and its content should be what we inserted
	LT_ASSERT(insert_result.position->data == id2);

	bst_release(&bst);
}

static void test_find_different_values(int data) {
	LT_TEST_ONCE

	test_insert_different_values(data);
	test_insert_find(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = CREATE_BST();

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(&bst, id1);
	bst_insert(&bst, id2);

	BstNode* p1 = bst_find(bst, id1);
	BstNode* p2 = bst_find(bst, id2);

	LT_ASSERT(p1 != nullptr);
	LT_ASSERT(p2 != nullptr);
	LT_ASSERT(p1 != p2);

	bst_release(&bst);
}

static void test_erase_then_find_fails(int data) {
	LT_TEST_ONCE

	test_insert_empty(data);
	test_insert_find(data);

	Bst bst = CREATE_BST();

	ContactId id = data;

	bst_insert(&bst, id);
	bst_erase(&bst, id);

	BstNode* p = bst_find(bst, id);

	LT_ASSERT(p == nullptr);

	bst_release(&bst);
}

static void test_erase_one_not_the_other(int data) {
	LT_TEST_ONCE

	test_insert_different_values(data);
	test_insert_find(data);

	int other = data * 19;
	if (data == other) return;

	Bst bst = CREATE_BST();

	ContactId id1 = data;
	ContactId id2 = other;

	bst_insert(&bst, id1);
	bst_insert(&bst, id2);

	bst_erase(&bst, id2);

	BstNode* p1 = bst_find(bst, id1);
	BstNode* p2 = bst_find(bst, id2);

	LT_ASSERT(p1 != nullptr);
	LT_ASSERT(p2 == nullptr);

	bst_release(&bst);
}

static void test_random_values(int data) {

	lt_srand(data);

	Bst bst = CREATE_BST();

	ContactId x = lt_rand();
	bst_insert(&bst, x);

	for (int i = 0; i < 100; ++i) {
		ContactId y = lt_rand();
		bst_insert(&bst, y);
	}

	LT_ASSERT(bst_find(bst, x) != nullptr);
	bst_erase(&bst, x);
	LT_ASSERT(bst_find(bst, x) == nullptr);

	bst_release(&bst);
}

static void test_all(int value) {
	lt_reset();
	test_find_empty(value);
	test_insert_empty(value);
	test_insert_find(value);
	test_double_insert(value);
	test_insert_different_values(value);
	test_find_different_values(value);
	test_erase_then_find_fails(value);
	test_erase_one_not_the_other(value);
	test_random_values(value);
}

void bst_test() {
	LT_ITERATE(test_all);
	lt_report("Bst");
}

