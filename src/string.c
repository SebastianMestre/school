#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

bool
is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

char*
string_dup(char const* str) {
	return strcpy(malloc(strlen(str) + 1), str);
}

void
string_trim(char* str) {
	char* out = str;

	for (; is_whitespace(*str); ++str) {}
	char* first_non_whitespace = str;
	char* after_last_non_whitespace = str;
	for (; *str != '\0'; ++str) {
		if (!is_whitespace(*str)) {
			after_last_non_whitespace = str + 1;
		}
	}

	for (char* it = first_non_whitespace; it != after_last_non_whitespace; ++it) {
		*out++ = *it;
	}
	*out = '\0';
}

void
string_tolower(char* str) {
	for (; *str != '\0'; ++str) {
		*str = tolower(*str);
	}
}
