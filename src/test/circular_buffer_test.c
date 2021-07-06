#include "../circular_buffer.h"

#include "lt.h"

#include <stdio.h>
#include <assert.h>

#define BUF_SIZE 5

static int* correct_ptr = nullptr;

static void
increment_int(void* arg, void* metadata) {
	arg = arg;
	int* num = metadata;
	*num += 1;
}

static void
test_full_buffer(CircularBuffer* b, int value) {
	LT_TEST_ONCE

	int other = (value % 10000) * 19;
	if (other == value) return;

	int dtor_calls = 0;
	correct_ptr = &dtor_calls;
	b->dtor = (Destructor){ increment_int, &dtor_calls };

	for (int i = 0; i < BUF_SIZE; ++i) {
		circular_buffer_push_back(b, SPANOF(value));
	}


	int steps = BUF_SIZE * 3;
	for (int i = 0; i < steps; ++i) {
		LT_ASSERT(dtor_calls == i);
		circular_buffer_push_back(b, SPANOF(other));
	}
	LT_ASSERT(dtor_calls == steps);

	circular_buffer_pop_front(b);

	LT_ASSERT(dtor_calls == steps+1);

	circular_buffer_push_back(b, SPANOF(value));

	LT_ASSERT(dtor_calls == steps+1);

	circular_buffer_push_back(b, SPANOF(value));

	LT_ASSERT(dtor_calls == steps+2);

	b->dtor = nop_dtor;
	return;
}

static void
run_test(void (*test)(CircularBuffer*, int), int value) {
	CircularBuffer b =
		circular_buffer_create(sizeof(int), BUF_SIZE, nop_dtor);
	test(&b, value);
	circular_buffer_release(&b);
}

static void
test_all(int value) {
	lt_reset();
	run_test(test_full_buffer, value);
}

void
circular_buffer_test() {
	LT_ITERATE(test_all);
	lt_report("CircularBuffer");
}
