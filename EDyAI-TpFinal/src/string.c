#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

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

bool
parse_u32(char* buf, unsigned int* out) {
	for (; *buf == ' ' || *buf == '\t' || *buf == '\n'; ++buf) {}
	if (*buf == '\0')
		return false;

	char* begin = buf;
	for (; isdigit(*buf); ++buf) {}
	char* end = buf;
	if (begin == end)
		return false;

	for (; *buf != '\0'; ++buf) {
		if (*buf != ' ' && *buf != '\t' && *buf != '\n')
			return false;
	}

	*out = 0;
	for (; end != begin; ++begin) {
		*out *= 10;
		*out += *begin - '0';
	}

	return true;
}

bool
parse_int(char* buf, int* out) {
	for (; *buf == ' ' || *buf == '\t'; ++buf) {}
	if (*buf == '\0')
		return false;

	bool minus = false;
	if (*buf == '-') {
		minus = true;
		buf += 1;
	}

	char* begin = buf;
	for (; isdigit(*buf); ++buf) {}
	char* end = buf;
	if (begin == end)
		return false;

	for (; *buf != '\n'; ++buf) {
		if (*buf != ' ' && *buf != '\t')
			return false;
	}
	assert(buf[1] == '\0');

	*out = 0;
	for (; end != begin; ++begin) {
		*out *= 10;
		*out += *begin - '0';
	}

	if (minus)
		*out = -*out;

	return true;
}

void
mem_swap(void* lhs, void* rhs, size_t n) {
	if (lhs == rhs)
		return;
	for (size_t i = 0; i != n; ++i) {
		((uint8_t*)lhs)[i] ^= ((uint8_t*)rhs)[i];
		((uint8_t*)rhs)[i] ^= ((uint8_t*)lhs)[i];
		((uint8_t*)lhs)[i] ^= ((uint8_t*)rhs)[i];
	}
}
