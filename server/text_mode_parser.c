#include "text_mode_parser.h"

enum status { OK, INVALID, INCOMPLETE };

enum status parsear_put(char* start, char* end, char** out_start) {
	char const* prefijo = "PUT";

	int buf_len = end - start;
	int len = strlen(prefijo);

	if (len > buf_len) return INCOMPLETE;
	if (memcmp(start, prefijo, len) != 0) return INVALID;

	start += len;
	buf_len -= len;

	if (1 > buf_len) return INCOMPLETE;
	if (*start != ' ') return INVALID;

	start += 1;
	buf_len -= 1;

	char* w1_start = start;
	char* w1_end = start;
	while (*w1_end != ' ' && w1_end != end) {
		++w1_end;
	}

	int w1_len = w1_end - w1_start;

	start += w1_len;
	buf_len -= w1_len;

	if (1 > buf_len) return INCOMPLETE;
	if (*start != ' ') return INVALID;

	start += 1;
	buf_len -= 1;

	char* w2_start = start;
	char* w2_end = start;
	while (*w2_end != '\n' && w2_end != end) {
		++w2_end;
	}

	int w2_len = w2_end - w2_start;

	start += w2_len;
	buf_len -= w2_len;

}

char* parse_command(char* start, char* end) {
#define N_PREFIJOS 2
	int buf_len = end-start;

	if (parsear_put(start, end));
	if (parsear_get(start, end));

	if (que ? que : que) return INCOMPLETE;
	if (que ? que : que) return INVALID;

}


