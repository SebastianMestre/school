#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define LT_TEST                                                                \
	static int lt_test_id = -1;                                                \
	if (lt_test_id == -1) {                                                    \
		lt_test_id = lt_test_count++;                                          \
	}

#define LT_ONCE                                                                \
	{                                                                          \
		if (lt_ran[lt_test_id])                                                \
			return;                                                            \
		lt_ran[lt_test_id] = true;                                             \
	}

#define LT_ASSERT(expr) (lt_assertions_ran += 1, assert(expr))

#define LT_ITERS 100
#define LT_ITERATE(test) for(int i = 0; i < LT_ITERS; ++i) test(rand());

#define LT_MAX_TEST_COUNT 500

static bool lt_ran[LT_MAX_TEST_COUNT];
static int lt_test_count = 0;
static int lt_assertions_ran = 0;

void lt_reset() {
	memset(lt_ran, 0, sizeof(lt_ran));
	lt_assertions_ran = 0;
}
