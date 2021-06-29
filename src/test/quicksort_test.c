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

static void
test_0(int arg) {
	arg = arg;
	LT_TEST_ONCE

	#define DATA_LEN 11
	int data[11] = { 3, 8, 4, 6, 3, 8, 9, 2, 5, 8, 1 };

	quicksort((Array){ SPANOF(data), sizeof(int), DATA_LEN }, (Comparator){int_cmp_fn, nullptr});

	for (size_t i = 1; i < DATA_LEN; ++i) {
		LT_ASSERT(data[i-1] <= data[i]);
	}
}

static void
test_all(int arg) {
	lt_reset();
	test_0(arg);
}

void
quicksort_test() {
	test_all(0);
	lt_report("quicksort");
}
