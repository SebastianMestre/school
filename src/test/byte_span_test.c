#include "byte_span_test.h"

#include "../byte_span.h"

#include "lt.h"

void length_test(int value) {
	LT_TEST

	ByteSpan span = byte_span_create(nullptr, value);
	LT_ASSERT(byte_span_width(span) == value);
}

void run_tests(int value) {
	lt_reset();
	length_test(value);
}

void byte_span_test() {
	LT_ITERATE(run_tests);
	lt_report("ByteSpan");
}
