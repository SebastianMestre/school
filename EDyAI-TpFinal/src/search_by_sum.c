#include "search_by_sum.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Vector search_by_sum(uint32_t* arr, size_t n, uint32_t sum) {
	Vector result = vector_create(sizeof(size_t));

	if (sum == 0)
		return result;

	bool* can_add_up_to = malloc(sizeof(bool) * (sum + 1));

	memset(can_add_up_to, 0, sum + 1);
	can_add_up_to[0] = true;
	for (size_t i = 0; i < n; ++i) {
		for (uint32_t j = sum+1; j--;) {
			if (j < arr[i])
				break;
			if (can_add_up_to[j - arr[i]])
				can_add_up_to[j] = true;
		}
	}


	if (can_add_up_to[sum]) {
		unsigned remaining_sum = sum;
		for (size_t i = n; i--;) {
			if (remaining_sum == 0) {
				break;
			}

			if (remaining_sum < arr[i]) {
				continue;
			}

			if (can_add_up_to[remaining_sum - arr[i]]) {
				vector_push(&result, SPANOF(i));
				remaining_sum -= arr[i];
			}
		}
	}

	free(can_add_up_to);

	return result;
}
