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

//#include "text_mode_parser.h"

#define TEXT_CLIENT_BUF_SIZE 2048

struct text_client_state {
	size_t buf_size;
	char buf[TEXT_CLIENT_BUF_SIZE];
};

struct text_client_state* create_text_client_state(kv_store* store) {
	struct text_client_state* result = try_alloc(store, sizeof(*result));
	if (result != NULL) {
		memset(result, 0, sizeof(*result));
	}
	return result;
}

static int respond_text_command(int client_socket, struct text_command* cmd, enum cmd_output res) {
	int out_val = 0;
	char ans[2048];
	int ans_len = 0;  
	const char* output_name;

	const char* cmd_output = cmd_output_name(res, &ans_len);   
	if (res == CMD_OK) {
		if (cmd->tag == GET || cmd->tag == TAKE) {
			assert(cmd->val_len > 0);
			ans_len += 1 + cmd-> val_len;
		}
		if (cmd->tag == STATS) {
			// TODO
			fprintf(stderr, "no implementado :(\n");
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
        if (!isalnum(str[i])) return 0;
    }
    return 1;
}

static int count_spaces(char* start, char* end) {
    int spaces = 0;
    while (start != NULL && end - start > 0) {
        start = memchr(start, ' ', end - start);
        if (start != NULL) {
            spaces++;
            start++;
        }
    }
    return spaces;
}

int cmp_to_string(const char* val, int len, const char* str) {
    return (len == strlen(str)) && (memcmp(val, str, len) == 0);
}

int get_word(char* start, int len, char sep) {
    char* word_end = memchr(start, sep, len);
    if (word_end == NULL) return 0;
    if (!alphanumeric(start, word_end - start)) return 0;
    return word_end - start;
}

static enum status parse_text_command(char** start, char* buf_end, struct text_command* cmd) {
    char* cursor = *start;
    char* line_end = memchr(cursor, '\n', buf_end - cursor);
    if (line_end == NULL) return INCOMPLETE; 
   
    
    int word_len;
    switch (count_spaces(cursor, line_end)) {
        case 0:
            if (cmp_to_string(cursor, line_end - cursor, "STATS")) {
                cmd->tag = STATS;
                goto PARSED;
            }
            return INVALID;
        case 1:
            word_len = get_word(cursor, line_end - cursor, ' ');
            if (word_len <= 0) return INVALID;

            if (cmp_to_string(cursor, word_len, "DEL")) cmd ->tag = DEL;
            else if (cmp_to_string(cursor, word_len, "GET")) cmd ->tag = GET;
            else if (cmp_to_string(cursor, word_len, "TAKE")) cmd ->tag = TAKE;
            else return INVALID;

            cursor += word_len + 1;
            goto PARSE_LAST;
        case 2:
            word_len = get_word(cursor, line_end - cursor, ' ');
            if (word_len <= 0) return INVALID;
            
            if (cmp_to_string(cursor, word_len, "PUT")) cmd ->tag = PUT;
            else return INVALID;

            cursor += word_len + 1;
            goto PARSE_PUT;
        default:
            return INVALID;
    }
PARSE_PUT:
    word_len = get_word(cursor, line_end - cursor, ' ');
    if (word_len <= 0) return INVALID;
    cmd->key_len = word_len;
    memcpy(cmd->key, cursor, word_len);
    cursor += word_len + 1;
PARSE_LAST:
    word_len = line_end - cursor;
    if (word_len <= 0) return INVALID;
    if (cmd->tag == PUT) {
        cmd->val_len = word_len;
        memcpy(cmd->val, cursor, word_len);
        goto PARSED;
    }
    cmd->key_len = word_len;
    memcpy(cmd->key, cursor, word_len);
PARSED:
    *start = line_end + 1;
    return PARSED;
}

static int interpret_text_commands(int sock, struct text_client_state* state, kv_store* store) {
	struct text_command cmd;
	char* cursor = state->buf;
	char* buf_end = state->buf + state->buf_size;

	while (cursor != buf_end) {
		enum status parse_status = parse_text_command(&cursor, buf_end, &cmd);

		// el comando es invalido, devuelvo error
		if (parse_status == INVALID) return MA_ERROR;

		// necesito mas input, reseteo el bufer y salgo
		if (parse_status == INCOMPLETE) break;

		assert(parse_status == PARSED);

		enum cmd_output res = run_text_command(store, &cmd);				
		if (respond_text_command(sock, &cmd, res) < 0) return MA_ERROR;
	}

	state->buf_size = buf_end - cursor;
	memmove(state->buf, cursor, state->buf_size);

	return MA_OK;
}

enum message_action handle_text_message(struct text_client_state* state, int sock, int events, kv_store* store) {
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) return MA_STOP;

	while (1) {
		int read_status = read_until(sock, state->buf, &state->buf_size, TEXT_CLIENT_BUF_SIZE);
		if (read_status == -3) return MA_ERROR;

		int interpret_status = interpret_text_commands(sock, state, store);
		if (interpret_status == MA_ERROR) return MA_ERROR;

		if (read_status ==  0) continue;
		if (read_status == -1) return MA_OK;
		if (read_status == -2) return MA_STOP;
	}
}
