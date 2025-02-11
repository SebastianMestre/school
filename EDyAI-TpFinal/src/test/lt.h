#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LT_TEST                                                                \
	static int lt_test_id = -1;                                                \
	if (lt_test_id == -1)                                                      \
		lt_test_id = lt_test_count++;                                          \
	lt_ran[lt_test_id] = true;                                                 \

#define LT_TEST_ONCE                                                           \
	static int lt_test_id = -1;                                                \
	if (lt_test_id == -1)                                                      \
		lt_test_id = lt_test_count++;                                          \
	if (lt_ran[lt_test_id])                                                    \
		return;                                                                \
	lt_ran[lt_test_id] = true;                                                 \

#define LT_ASSERT(expr) (lt_assertions_ran += 1, assert(expr))

#define LT_ITERS 20
#define LT_ITERATE(test) for(int i = 0; i < LT_ITERS; ++i) test(rand());

#define LT_MAX_TEST_COUNT 500

static bool lt_ran[LT_MAX_TEST_COUNT];
static int lt_test_count = 0;
static int lt_assertions_ran = 0;
static int lt_rand_seed = 0;

static void lt_reset() {
	memset(lt_ran, 0, sizeof(lt_ran));
	lt_assertions_ran = 0;
	lt_rand_seed = 0;
}

static void lt_srand(int new_seed) {
	lt_rand_seed = new_seed;
}
static int lt_rand() {
	// PRNG robado de internet (donde???)
	return lt_rand_seed = (lt_rand_seed * 1103515245 + 12345) & ((1U << 31) - 1);
}

static void lt_report(char const* module_name) {
	int tests_ran = 0;
	for (int i = 0; i < lt_test_count; ++i)
		tests_ran += lt_ran[i];
	printf(
		"%s tests passed -- %d tests ran with %d assertions \n",
		module_name, tests_ran, lt_assertions_ran);
}

static void lt_do_not_call_a();
static void lt_do_not_call_b() {
	lt_do_not_call_a();
}

static void lt_do_not_call_a() {
	lt_do_not_call_b();
	lt_rand();
	lt_srand(0);
}
