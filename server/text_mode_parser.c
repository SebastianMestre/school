#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "text_mode_parser.h"

#define N_CMDS 5

// parsea una palabra (alfanumerica)
// return: el largo de la palabra si es valida, un valor negativo en otro caso  
static int parse_word(char* start, char* end) {
	char* w_end = start;
	while (w_end != end && *w_end != ' ' && *w_end != '\n') {
		if (!isalnum(*w_end)) return -1;
		++w_end;
	} 
	return w_end - start;
}
// parser para comandos de la forma "PREFIJO\n"
static enum status parse_(char const* prefix, char** start, char* end) {
	int buf_len = end - *start;
	int prefix_len = strlen(prefix);

	if (prefix_len > buf_len) return MISMATCH;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != '\n') return INVALID;

	*start += 1;
	buf_len -= 1;

	return PARSED;
}
// parser para comandos de la forma "PREFIJO CLAVE\n"
static enum status parse_k(char const* prefix, char** start, char* end, 
							struct text_command* cmd){
	int buf_len = end - *start;
	int prefix_len = strlen(prefix);

	if (prefix_len > buf_len) return MISMATCH;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;
	
	// parsear w1
	char* key_start = *start;
	int key_len = parse_word(key_start, end);
	if (key_len < 0) return INVALID;
	if (key_len == 0) return INCOMPLETE;

	*start += key_len;
	buf_len -= key_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != '\n') return INVALID;

	*start += 1;
	buf_len -= 1;

	memcpy(cmd->key, key_start, key_len);
	cmd->key_len = key_len;
	
	return PARSED;
}
// parser para comandos de la forma "PREFIJO CLAVE VALOR\n"
static enum status parse_kv(char const* prefix, char** start, char* end, 
							struct text_command* cmd) {
	
	int buf_len = end - *start;
	int prefix_len = strlen(prefix);

	if (prefix_len > buf_len) return MISMATCH;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;


	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;
	
	// parsear key
	char* key_start = *start;
	int key_len = parse_word(key_start, end);
	if (key_len < 0) return INVALID;
	if (key_len == 0) return INCOMPLETE;

	*start += key_len;
	buf_len -= key_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;
	
	// parsear val
	char* val_start = *start;
	int val_len = parse_word(val_start, end);
	if (val_len < 0) return INVALID;
	if (val_len == 0) return INCOMPLETE;

	*start += val_len;
	buf_len -= val_len;
	
	if (buf_len <= 0) return INCOMPLETE;
	if (**start != '\n') return INVALID;

	*start += 1;
	buf_len -= 1;

	memcpy(cmd->key, key_start, key_len);
	cmd->key_len = key_len;
	memcpy(cmd->val, val_start, val_len);
	cmd->val_len = val_len;
	
	return PARSED;
}

// parsea el formato correcto en funcion de `tag` 
static enum status parse_command_by_tag(char** start, char* end, 
										struct text_command* cmd, enum cmd_tag tag) {
	cmd->tag = tag;
	switch (tag) {
		case PUT:
			return parse_kv("PUT", start, end, cmd);
		case DEL:
			return parse_k("DEL", start, end, cmd);
		case GET:
			return parse_k("GET", start, end, cmd);
		case TAKE:
			return parse_k("TAKE", start, end, cmd);
		case STATS:
			return parse_("STATS", start, end);
		default:
			assert(0);
		}
}

enum status parse_text_command(char** start, char* end, struct text_command* cmd) {
	enum status fallback = INVALID;
	for (int i = 0; i < N_CMDS; i++) {
		char* out_start = *start;
		enum status status = parse_command_by_tag(&out_start, end, cmd, i);
		
		if (status == INCOMPLETE) fallback = INCOMPLETE;
		
		// error de formato
		if (status == INVALID) return INVALID;
		
		// comando parseado
		if (status == PARSED) {
			*start = out_start;
			return PARSED;
		}
	}

	return fallback;
}
