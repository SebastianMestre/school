#include "bst_test.h"
#include "span_test.h"
#include "vector_test.h"
#include "slot_map_test.h"
#include "database_test.h"
#include "search_by_sum_test.h"

int main() {
	bst_test();
	span_test();
	vector_test();
	slot_map_test();
	database_test();
	search_by_sum_test();
}
