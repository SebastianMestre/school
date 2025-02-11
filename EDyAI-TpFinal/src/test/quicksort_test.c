#include "quicksort_test.h"

#include "../quicksort.h"

#include "lt.h"

static int
int_cmp_fn(void const* arg0, void const* arg1, void* metadata) {
	metadata = metadata;
	int const* lhs = arg0;
	int const* rhs = arg1;
	return *lhs - *rhs;
}

static bool
is_sorted(int* data, size_t n) {
	for (size_t i = 1; i < n; ++i)
		if (data[i-1] > data[i])
			return false;
	return true;
}

static bool
check_sorted(int* data, size_t n) {
	quicksort(data, data + n, sizeof(*data), (Comparator){int_cmp_fn, nullptr});
	return is_sorted(data, n);
}

static bool
check_equals(int* data, int* want, size_t n) {
	quicksort(data, data + n, sizeof(*data), (Comparator){int_cmp_fn, nullptr});
	return memcmp(data, want, n * sizeof(*data)) == 0;
}

static void
test_0(int arg) {
	arg = arg;
	LT_TEST_ONCE
#define DATA_LEN 11
	int data[11] = { 3, 8, 4, 6, 3, 8, 9, 2, 5, 8, 1 };
	LT_ASSERT(check_sorted(data, DATA_LEN));
#undef DATA_LEN
}

static void
test_1(int arg) {
	arg = arg;
	LT_TEST_ONCE
	int data[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
	int want[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	LT_ASSERT(check_equals(data, want, 10));
}

static void
test_2(int arg) {
	arg = arg;
	LT_TEST_ONCE
	int data[] = { 9, 8, 7, 6, 5, 0, 4, 3, 2, 1 };
	int want[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	LT_ASSERT(check_equals(data, want, 10));
}

static void
test_3(int arg) {
	arg = arg;
	LT_TEST_ONCE
	int data[] = { 0, 1, 3, 4, 5, 6, 7, 2, 8, 9 };
	int want[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	LT_ASSERT(check_equals(data, want, 10));
}

static void
test_random(int arg) {
	LT_TEST_ONCE
	lt_srand(arg);
#define DATA_LEN 50
	int data[DATA_LEN];
	for (size_t i = 0; i < DATA_LEN; ++i) {
		data[i] = lt_rand();
	}
	LT_ASSERT(check_sorted(data, DATA_LEN));
#undef DATA_LEN
}

static void
test_all(int arg) {
	lt_reset();
	test_0(arg);
	test_1(arg);
	test_2(arg);
	test_3(arg);
	test_random(arg);
}

void
quicksort_test() {
	test_all(0);
	lt_report("quicksort");
}
