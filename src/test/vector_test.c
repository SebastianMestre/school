#include "../vector.h"

#include "lt.h"

#define CREATE_VECTOR() vector_create(sizeof(int))

void test_increase_size(int value) {
	LT_TEST_ONCE

	if (value < 0) value = -value;
	value %= 15;

	Vector v = CREATE_VECTOR();
	for (int i = 0; i < value; ++i)
		vector_push(&v, SPANOF(value));

	size_t size_before = v.size;
	vector_push(&v, SPANOF(value));
	size_t size_after = v.size;

	LT_ASSERT(size_after == size_before+1);

	vector_release(&v);
}

/*
void test_decrease_size(int value) {
	LT_TEST_ONCE

	if (value < 0) value = -value;
	value %= 15;
	if (value < 1) value = 1;

	Vector v = CREATE_VECTOR();
	for (int i = 0; i < value; ++i)
		vector_push(&v, SPANOF(value));

	size_t size_before = v.size;
	vector_pop(&v);
	size_t size_after = v.size;

	assert(size_after == size_before-1);
}
*/

void test_all(int value) {
	lt_reset();
	test_increase_size(value);
}

void vector_test() {
	LT_ITERATE(test_all);
	lt_report("Vector");
}
