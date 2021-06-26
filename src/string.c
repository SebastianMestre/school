#include "string.h"

#include <string.h>
#include <stdlib.h>

char* string_dup(char const* str) {
	return strcpy(malloc(strlen(str) + 1), str);
}
