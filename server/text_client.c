#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <ctype.h>

#include "message_action.h"
#include "commands.h"
#include "fd_utils.h"
#include "try_alloc.h"

#define TEXT_CLIENT_BUF_SIZE 2048

struct text_client_state {
	size_t buf_size;
	char buf[TEXT_CLIENT_BUF_SIZE];
};

struct text_client_state* create_text_client_state(kv_store* store) {
	struct text_client_state* result = try_alloc(store, sizeof(*result));
	if (result == NULL) return NULL;
	memset(result, 0, sizeof(*result));
	return result;
}

void free_text_client_state(struct text_client_state* state) {
	free(state);
}

static int respond_text_command(int client_socket, struct text_command* cmd, enum cmd_output res) {
	int out_val = 0;
	char ans[2048];
	int ans_len = 0;  

	const char* cmd_output = cmd_output_name(res, &ans_len);   
	if (res == CMD_OK) {
		if (cmd->tag == GET || cmd->tag == TAKE || cmd->tag == STATS) {
			assert(cmd->val_len > 0);
			ans_len += 1 + cmd-> val_len;
		}
		// reiniciamos el comando
		cmd->val_len = 0;
	}
	if (ans_len < 2048) {
		// ans = cmd_output ++ cmd->val?
		// cmd_output es un string literal y tiene terminador 
		// cmd->val no tiene terminador, funciona porque sabemos exactamente el largo
		snprintf(ans, ans_len + 1, "%s %s", cmd_output, cmd->val);
	}
	else {
		ans_len = sprintf(ans, "EBIG");
	}
	ans[ans_len++] = '\n';

	int nbytes = write(client_socket, ans, ans_len);
	if (nbytes != ans_len) {
		out_val = -1;
		fprintf(stderr, "no pude escribir al cliente \n");
	}
	
	return out_val;
}

int alphanumeric(char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (!isalnum(str[i]) && str[i] != ' ') return 0;
    }
    return 1;
}

int starts_with_word(char* start, char* end, char const* str) {
	size_t len = strlen(str);
	if (end - start < len) return 0;
	if (memcmp(start, str, len) != 0) return 0;
	return (end - start == len) || (start[len] == ' ');
}

enum parse_status {INCOMPLETE, INVALID, PARSED};

static enum parse_status parse_text_command(char** start, char* buf_end, struct text_command* cmd) {
    char* line_start = *start;

    char* line_end = memchr(line_start, '\n', buf_end - line_start);
    if (line_end == NULL) return INCOMPLETE; 

	if (!alphanumeric(line_start, line_end - line_start)) return INVALID;

	*start = line_end + 1;
    
	if (starts_with_word(line_start, line_end, "STATS")) cmd->tag = STATS;
	else if (starts_with_word(line_start, line_end, "DEL")) cmd->tag = DEL;
	else if (starts_with_word(line_start, line_end, "GET")) cmd->tag = GET;
	else if (starts_with_word(line_start, line_end, "TAKE")) cmd->tag = TAKE;
	else if (starts_with_word(line_start, line_end, "PUT")) cmd->tag = PUT;
	else return INVALID;

    switch (cmd->tag) {
	case STATS: {
		if (memchr(line_start, ' ', line_end - line_start) != NULL)
			return INVALID;

		return PARSED;
	}
	case DEL: case GET: case TAKE: {

		char* com_end = memchr(line_start, ' ', line_end - line_start);
		if (com_end == NULL) return INVALID;

		char* key_start = com_end + 1;
		char* key_end = line_end;

		if (key_start == key_end)
			return INVALID;

		if (memchr(key_start, ' ', line_end - key_start) != NULL)
			return INVALID;

		cmd->key_len = key_end - key_start;
		memcpy(cmd->key, key_start, cmd->key_len);

		return PARSED;
	}
	case PUT: {

		char* com_end = memchr(line_start, ' ', line_end - line_start);
		if (com_end == NULL) return INVALID;

		char* key_start = com_end + 1;

		char* key_end = memchr(key_start, ' ', line_end - key_start);
		if (key_end == NULL) return INVALID;

		char* val_start = key_end + 1;
		char* val_end = line_end;

		if (key_start == key_end || val_start == val_end)
			return INVALID;

		if (memchr(val_start, ' ', line_end - val_start) != NULL)
			return INVALID;

		cmd->key_len = key_end - key_start;
		memcpy(cmd->key, key_start, cmd->key_len);

		cmd->val_len = val_end - val_start;
		memcpy(cmd->val, val_start, cmd->val_len);

		return PARSED;
	}
	default: assert(0);
    }

}

static int interpret_text_commands(int sock, struct text_client_state* state, kv_store* store) {
	struct text_command cmd;
	char* cursor = state->buf;
	char* buf_end = state->buf + state->buf_size;

	enum cmd_output res;

	while (cursor != buf_end) {
		enum parse_status parse_status = parse_text_command(&cursor, buf_end, &cmd);

		// input incompleto
		if (parse_status == INCOMPLETE) {
			// el mensaje es demasiado largo, error
			if (buf_end - cursor == TEXT_CLIENT_BUF_SIZE) return MA_ERROR;
			else break;  
		};
		// el comando es invalido
		if (parse_status == INVALID) res = CMD_EINVAL;
		else if (parse_status == PARSED) res = run_text_command(store, &cmd);
		else assert(0);

		if (respond_text_command(sock, &cmd, res) < 0) return MA_ERROR;
	}

	state->buf_size = buf_end - cursor;
	memmove(state->buf, cursor, state->buf_size);

	return MA_OK;
}

enum message_action handle_text_message(struct text_client_state* state, int sock, int events, kv_store* store) {
	
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		fprintf(stderr, "HANGUP\n");
		return MA_STOP;
	}

	int read_status;
	while (1) {
		read_status = read_until(sock, state->buf, &state->buf_size, TEXT_CLIENT_BUF_SIZE);
		if (read_status == -3) return MA_ERROR;

		int interpret_status = interpret_text_commands(sock, state, store);
		if (interpret_status == MA_ERROR) return MA_ERROR;

		if (read_status ==  0) continue;
		if (read_status == -1) return MA_OK;
		if (read_status == -2) return MA_STOP;
	}
}
