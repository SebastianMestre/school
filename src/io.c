#include "io.h"

#include <assert.h>
#include <ctype.h>

#include "string.h"

bool
get_line(char* buf, size_t n, FILE* f) {
	while (--n != 0) {
		char c = getchar();
		*buf++ = c;
		if (c == '\n') {
			*buf = '\0';
			return true;
		}
	}

	*buf = '\0';
	return false;
}

bool
get_line_as_int(char* buf, size_t n, int* out, FILE* f) {
	if (!get_line(buf, n, f))
		return false;

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

bool
get_line_as_uint(char* buf, size_t n, unsigned int* out, FILE* f) {
	if (!get_line(buf, n, f))
		return false;

	for (; *buf == ' ' || *buf == '\t'; ++buf) {}
	if (*buf == '\0')
		return false;

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

	return true;
}

void
print_title_case(char const* str, FILE* f) {
	if (!str) return;
	bool last_was_whitespace = true;
	for (; *str != '\0'; ++str) {
		char c = *str;
		if (last_was_whitespace) {
			c = toupper(c);
		}
		fputc(c, f);
		last_was_whitespace = is_whitespace(*str);
	}
}
