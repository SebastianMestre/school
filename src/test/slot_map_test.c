#include "slot_map_test.h"
#include "lt.h"

#include "../slot_map.h"

#define CREATE_SLOT_MAP() slot_map_create(sizeof(int), nop_dtor)

static void
random_test(int arg) {
	LT_TEST_ONCE
	arg = arg;

#define N 100
#define M 10

	SlotMap m = CREATE_SLOT_MAP();

	int value[N];
	size_t id[N];

	for (int i = 0; i < N; ++i) {
		value[i] = rand();
		id[i] = slot_map_insert(&m, SPANOF(value[i]));
	}

	int erased[M];
	for (int i = 0; i < M; ++i) {
		bool repeated;
		do {
			erased[i] = rand() % N;
			repeated = false;
			for (int j = 0; j < i; ++j) {
				if (erased[j] == erased[i]) {
					repeated = true;
					break;
				}
			}
		} while (repeated);
	}

	for (int i = 0; i < M; ++i) {
		int idx = erased[i];
		slot_map_delete(&m, id[idx]);
	}

	for (int i = 0; i < M; ++i) {
		int idx = erased[i];
		value[idx] = rand();
		id[idx] = slot_map_insert(&m, SPANOF(value[idx]));
	}

	for (int i = 0; i < N; ++i) {
		int map_value;
		span_write(&map_value, slot_map_at(&m, id[i]));
		LT_ASSERT(map_value == value[i]);
	}

	slot_map_release(&m);

#undef N
}

static void
test_all(int value) {
	lt_reset();
	random_test(value);
}

void
slot_map_test() {
	LT_ITERATE(test_all);
	lt_report("SlotMap");
}
