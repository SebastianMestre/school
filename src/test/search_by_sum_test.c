#include "search_by_sum_test.h"

#include "../search_by_sum.h"

#include "lt.h"

static void
test_0(int arg) {
	arg = arg;
	LT_TEST_ONCE

	uint32_t data[] = {10, 15};

	Vector result = search_by_sum(data, 2, 15);

	LT_ASSERT(result.size == 1);

	size_t idx; span_write(&idx, vector_at(&result, 0));
	LT_ASSERT(idx == 1);

	vector_release(&result);
}

static void
test_1(int arg) {
	arg = arg;
	LT_TEST_ONCE

	uint32_t data[] = {10, 15};

	Vector result = search_by_sum(data, 2, 10);

	LT_ASSERT(result.size == 1);

	size_t idx; span_write(&idx, vector_at(&result, 0));
	LT_ASSERT(idx == 0);

	vector_release(&result);
}

static void
test_2(int arg) {
	arg = arg;
	LT_TEST_ONCE

	uint32_t data[] = {10, 15};

	Vector result = search_by_sum(data, 2, 25);

	LT_ASSERT(result.size == 2);

	vector_release(&result);
}

static void
test_3(int arg) {
	LT_TEST_ONCE
	lt_srand(arg);

	uint32_t data[] = {10, 15};

	int x = lt_rand() % 100;
	while (x == 10 || x == 15 || x == 25) x = lt_rand() % 100;

	Vector result = search_by_sum(data, 2, x);

	LT_ASSERT(result.size == 0);

	vector_release(&result);
}

static void
test_all(int arg) {
	lt_reset();
	test_0(arg);
	test_1(arg);
	test_2(arg);
	test_3(arg);
}

void
search_by_sum_test() {
	LT_ITERATE(test_all);
	lt_report("search_by_sum");
}
