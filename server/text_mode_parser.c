#include <string.h>
#include <ctype.h>

#include "text_mode_parser.h"

#define N_CMDS 5

enum cmd_tag cmd_tags[N_CMDS] = {PUT, DEL, GET, TAKE, STATS};

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

	if (prefix_len > buf_len) return INCOMPLETE;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != '\n') return INVALID;

	*start += 1;
	buf_len -= 1;

	return OK;
}
// parser para comandos de la forma "PREFIJO CLAVE\n"
static enum status parse_k(char const* prefix, char** start, char* end, struct cmd* cmd){
	int buf_len = end - *start;
	int prefix_len = strlen(prefix);

	if (prefix_len > buf_len) return INCOMPLETE;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;

	if (buf_len <= 0) return INCOMPLETE;
	
	// parsear w1
	char* key_start = *start;
	int key_len = parse_word(key_start, end);
	if (key_len <= 0) return INVALID;

	*start += key_len;
	buf_len -= key_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != '\n') return INVALID;

	*start += 1;
	buf_len -= 1;

	memcpy(cmd->key, key_start, key_len);
	cmd->key_len = key_len;
	
	return OK;
}
// parser para comandos de la forma "PREFIJO CLAVE VALOR\n"
static enum status parse_kv(char const* prefix, char** start, char* end, struct cmd* cmd) {
	
	int buf_len = end - *start;
	int prefix_len = strlen(prefix);

	if (prefix_len > buf_len) return INCOMPLETE;
	if (memcmp(*start, prefix, prefix_len) != 0) return MISMATCH;

	*start += prefix_len;
	buf_len -= prefix_len;


	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;

	if (buf_len <= 0) return INCOMPLETE;
	
	// parsear key
	char* key_start = *start;
	int key_len = parse_word(key_start, end);
	if (key_len <= 0) return INVALID;

	*start += key_len;
	buf_len -= key_len;

	if (buf_len <= 0) return INCOMPLETE;
	if (**start != ' ') return INVALID;

	*start += 1;
	buf_len -= 1;

	if(buf_len <= 0) return INCOMPLETE;
	
	// parsear val
	char* val_start = *start;
	int val_len = parse_word(val_start, end);
	if (val_len <= 0) return INVALID;

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
	
	return OK;
}

static enum status parse_command_by_tag(char** start, char* end, struct cmd* cmd,
								enum cmd_tag tag) {
	cmd->tag = tag;
	switch (tag) {
		case PUT:
			return parse_kv("PUT", start, end, cmd);
		case DEL:
			return parse_k("DEL", start, end, cmd);
		case GET:
			return parse_k("DEL", start, end, cmd);
		case TAKE:
			return parse_k("TAKE", start, end, cmd);
		case STATS:
			return parse_("STATS", start, end);
		}
}

// MAYBE pasar el largo en vez de `end`
enum status parse_command(char** start, char* end, struct cmd* cmd) {
	char* out_start = *start;
	int incomplete = 0;
	enum status status;
	for (int i = 0; i < N_CMDS; i++) {
		status = parse_command_by_tag(&out_start, end, cmd, cmd_tags[i]);
		// error de formato
		if (status == INVALID) return INVALID;
		// comando parseado
		if (status == OK) {
			*start = out_start;
			return OK;
		}
		if (status == INCOMPLETE) incomplete = 1; 
	}

	// si se detecto incompleto, chequeamos que la linea no este terminada
	if (incomplete) {
		for (char* cursor = *start; cursor != end; cursor++) {
			if (*cursor == '\n') return INVALID;
		}
		return INCOMPLETE;
	}

	// si fueron todos MISMATCH, devolvemos INVALID
	return INVALID;
}


