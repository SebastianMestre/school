#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "biny_client.h"
#include "commands.h"
#include "fd_utils.h"
#include "try_alloc.h"

#define MIN(a, b) (a) > (b) ? (b) : (a)
// constantes
#define N_CMDS 5
#define PUT_ID 11
#define DEL_ID 12 
#define GET_ID 13
#define TAKE_ID 14
#define STATS_ID 21

#define BINY_CLIENT_BUF_SIZE 2048

enum biny_client_step { BC_START, BC_KEY_SIZE, BC_KEY, BC_VAL_SIZE, BC_VAL };

struct biny_client_state {

	enum biny_client_step step;

	uint8_t operation;

	size_t cursor;

	uint8_t size_buf[4];

	size_t key_size;
	uint8_t* key;

	size_t val_size;
	uint8_t* val;

	struct biny_command cmd;
};


struct biny_client_state* create_biny_client_state(kv_store* store) {
	struct biny_client_state* result = try_alloc(store, sizeof(*result));
	if (result != NULL) {
		memset(result, 0, sizeof(*result));
		result->step = BC_START;
	}
	return result;
}


// Mandamos el mensaje en partes, abortando si el write falla
int respond_biny_command(int client_socket, struct biny_command* cmd, enum cmd_output res) {
	int out_val = 0;
	uint8_t res_code = cmd_output_code(res);

	if (write(client_socket, &res_code, 1) != 1) { 
		out_val = -1;
		goto early_exit;
	}

	if (res != CMD_OK) {
		goto early_exit;
	}

	switch (cmd->tag) {
	case GET: case TAKE: case STATS:
		assert(cmd->val);
		uint32_t val_size = htonl(cmd->val_size);

		if (write(client_socket, &val_size, 4) != 4) {
			out_val = -1;
			goto early_exit;
		}

		if (write(client_socket, cmd->val, cmd->val_size) != cmd->val_size) {
			out_val = -1;
			goto early_exit;
		}
		break;
	case DEL: case PUT:
		break;
	}

early_exit:
	if (cmd->val_size > 0) {
		free(cmd->val);
		cmd->val = NULL;
		cmd->val_size = 0;
	}

	if (out_val < 0) {
		fprintf(stderr, "error comunicandose con el cliente\n");
	}

	return out_val;
}

enum message_action handle_biny_message(struct biny_client_state* state, int sock, int events, kv_store* store) {

	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		fprintf(stderr, "HANGUP\n");
		return MA_STOP;
	}

	int read_status;

	while (1) {

		switch (state->step) {
		case BC_START:
			read_status = read_until(sock, &state->operation, &state->cursor, 1);
			if (read_status < 0) {
				if (read_status == -2) return MA_STOP;
				else goto error_nothing;
			}

			state->cursor = 0;

			if (state->operation == STATS_ID) {
				break;
			} else {
				state->step = BC_KEY_SIZE;
			}


		case BC_KEY_SIZE: // fallthrough

			read_status = read_until(sock, state->size_buf, &state->cursor, 4);
			if (read_status < 0) goto error_nothing;

			state->key_size = ntohl(*(uint32_t*)state->size_buf);
			state->cursor = 0;
			state->key = try_alloc(store, state->key_size);
			if (state->key == NULL) goto error_oom;

			state->step = BC_KEY;

		case BC_KEY: // fallthrough

			read_status = read_until(sock, state->key, &state->cursor, state->key_size);
			if (read_status < 0) goto error_key;

			state->cursor = 0;

			if (state->operation == PUT_ID) {
				state->step = BC_VAL_SIZE;
			} else {
				break;
			}


		case BC_VAL_SIZE: // fallthrough

			read_status = read_until(sock, state->size_buf, &state->cursor, 4);
			if (read_status < 0) goto error_key;

			state->val_size = ntohl(*(uint32_t*)state->size_buf);
			state->cursor = 0;
			state->val = try_alloc(store, state->val_size);
			if (state->key == NULL) goto error_oom;

			state->step = BC_VAL;

		case BC_VAL: // fallthrough
			read_status = read_until(sock, state->val, &state->cursor, state->val_size);
			if (read_status < 0) goto error_key_value;

			state->cursor = 0;
		}

		struct biny_command cmd = {};

		switch (state->operation) {
		case PUT_ID:
			cmd.tag = PUT;
			cmd.key_size = state->key_size;
			cmd.key = state->key;

			cmd.val_size = state->val_size;
			cmd.val = state->val;
			break;
		case DEL_ID:
			cmd.tag = DEL;
			cmd.key_size = state->key_size;
			cmd.key = state->key;
			break;
		case GET_ID:
			cmd.tag = GET;
			cmd.key_size = state->key_size;
			cmd.key = state->key;
			break;
		case TAKE_ID:
			cmd.tag = TAKE;
			cmd.key_size = state->key_size;
			cmd.key = state->key;
			break;
		case STATS_ID:
			cmd.tag = STATS;
			break;
		}

		enum cmd_output res = run_biny_command(store, &cmd);
		if (respond_biny_command(sock, &cmd, res) < 0) {
			return MA_ERROR;
		}

		memset(state, 0, sizeof(*state));
		state->step = BC_START;
	}

error_key_value:
	if (read_status == -1) return MA_OK;
	free(state->val);
	state->val = NULL;
error_key:
	if (read_status == -1) return MA_OK;
	free(state->key);
	state->key = NULL;
error_nothing:
	if (read_status == -1) return MA_OK;
	return MA_ERROR;

error_oom:
	// TODO: respond OOM
	return MA_ERROR;
}
