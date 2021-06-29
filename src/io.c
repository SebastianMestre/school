#include "io.h"

#include <assert.h>
#include <ctype.h>

#include "string.h"

bool
get_line(char* buf, size_t n, FILE* f) {
	while (--n != 0) {
		char c = fgetc(f);
		*buf++ = c;
		if (c == '\n') {
			*buf = '\0';
			return true;
		}
	}

	*buf = '\0';
	return false;
}

void
get_line_retry(char* buf, size_t n, char const* error_msg, FILE* f) {
	while (1) {
		bool ok = get_line(buf, n, f);
		if (!ok) {
			printf("%s", error_msg);
			continue;
		}
		break;
	}
}

static bool
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

bool
get_line_as_int(char* buf, size_t n, int* out, FILE* f) {
	if (!get_line(buf, n, f))
		return false;
	return parse_int(buf, out);
}

void
get_line_as_int_retry(char* buf, size_t n, int* out, char const* line_error_msg, char const* value_error_msg, FILE* f) {
	while (1) {
		bool ok;
		ok = get_line(buf, n, f);
		if (!ok) {
			printf("%s", line_error_msg);
			continue;
		}
		assert(!feof(f));
		ok = parse_int(buf, out);
		if (!ok) {
			printf("%s", value_error_msg);
			continue;
		}
		return;
	}
}


bool
parse_u32(char* buf, unsigned int* out) {
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

bool
get_line_as_u32(char* buf, size_t n, unsigned int* out, FILE* f) {
	if (!get_line(buf, n, f))
		return false;
	return parse_u32(buf, out);
}

void
get_line_as_u32_retry(char* buf, size_t n, unsigned int* out, char const* line_error_msg, char const* value_error_msg, FILE* f) {
	while (1) {
		bool ok;
		ok = get_line(buf, n, f);
		if (!ok) {
			printf("%s", line_error_msg);
			continue;
		}
		assert(!feof(f));
		ok = parse_u32(buf, out);
		if (!ok) {
			printf("%s", value_error_msg);
			continue;
		}
		return;
	}
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
