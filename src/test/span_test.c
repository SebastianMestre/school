#include "span_test.h"

#include "../span.h"

#include "lt.h"

void length_test(int value) {
	LT_TEST

	Span span = span_create(nullptr, value);
	LT_ASSERT(span_width(span) == value);
}

void run_tests(int value) {
	lt_reset();
	length_test(value);
}

void span_test() {
	LT_ITERATE(run_tests);
	lt_report("Span");
}
