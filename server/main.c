#define _GNU_SOURCE

#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "connections.h"
#include "kv_store.h"
#include "commands.h"
#include "text_mode_parser.h"
#include "biny_mode_parser.h"

enum protocol { TEXT, BINY };

struct fd_data {
	enum fd_type {
		FD_TYPE_BINARY_LISTEN,
		FD_TYPE_TEXT_LISTEN,
		FD_TYPE_BINARY_CONN,
		FD_TYPE_TEXT_CONN,
	} type;
	int fd;
	union client_state {
		struct text_client_state* text;
		struct biny_client_state* biny;
	} client_state; 	
};

// TODO que hacemos si no podemos alocar el client_state?

#define TEXT_CLIENT_BUF_SIZE 2048

struct text_client_state {
	int buf_size;
	char buf[TEXT_CLIENT_BUF_SIZE];
};

struct text_client_state* create_text_client_state() {
	struct text_client_state* result = malloc(sizeof(*result));
	if (result != NULL) {
		memset(result, 0, sizeof(*result));
	}
	return result;
}

#define BINY_CLIENT_BUF_SIZE 2048

struct biny_client_state {
	int buf_size;
	uint8_t buf[BINY_CLIENT_BUF_SIZE];
	struct biny_command cmd;
};

struct biny_client_state* create_biny_client_state() {
	struct biny_client_state* result = malloc(sizeof(*result));
	if (result != NULL) {
		memset(result, 0, sizeof(*result));
	}
	return result;
}


void register_listen_socket_first(int epollfd, int sock, enum protocol protocol) {
	struct fd_data* data = calloc(1, sizeof(*data));
	switch (protocol) {
		case TEXT:
			data->type = FD_TYPE_TEXT_LISTEN;
			break;
		case BINY:
			data->type = FD_TYPE_BINARY_LISTEN;
			break;
	}
	data->fd = sock;
	int flags = EPOLLIN | EPOLLONESHOT;
	if (register_epoll(epollfd, sock, EPOLL_CTL_ADD, flags, data) < 0)
		exit(EXIT_FAILURE);
}

void register_listen_socket_again(int epollfd, int sock, struct fd_data* data) {
	int flags = EPOLLIN | EPOLLONESHOT;
	if (register_epoll(epollfd, sock, EPOLL_CTL_MOD, flags, data) < 0)
		exit(EXIT_FAILURE);
}

void register_client_socket_first(int epollfd, int sock, enum protocol protocol) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->fd = sock;
	switch (protocol) {
		case TEXT:
			data->type = FD_TYPE_TEXT_CONN;
			data->client_state.text = create_text_client_state();
			break;
		case BINY:
			data->type = FD_TYPE_BINARY_CONN;
			data->client_state.biny = create_biny_client_state();
			break;
	}

	int flags = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
	if (register_epoll(epollfd, sock, EPOLL_CTL_ADD, flags, data) < 0)
		exit(EXIT_FAILURE);
}

void register_client_socket_again(int epollfd, int sock, struct fd_data* data) {
	int flags = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
	if (register_epoll(epollfd, sock, EPOLL_CTL_MOD, flags, data) < 0)
		exit(EXIT_FAILURE);
}


enum message_action { MA_ERROR, MA_STOP, MA_OK };

enum message_action handle_new_client(int listen_sock, int* out_sock) {
	*out_sock = -1;
	int conn_sock = accept4(listen_sock, NULL, NULL, SOCK_NONBLOCK);

	if (conn_sock < 0)
		return MA_ERROR;

	*out_sock = conn_sock;
	return MA_OK;
}

int respond_text_command(int client_socket, struct text_command* cmd, enum cmd_output res) {
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

enum message_action handle_text_message(struct fd_data* data, int events, kv_store* store) {
	int sock = data->fd;

	struct text_client_state* state = data->client_state.text;
	assert(state);

	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		fprintf(stderr, "HANGUP\n");
		return MA_STOP;
	}

	int status = 0;

	while (1) {
		int read_bytes = read(sock, state->buf + state->buf_size, TEXT_CLIENT_BUF_SIZE - state->buf_size);

		if (read_bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				status = 1;
				break;
			} else {
				return MA_ERROR;
			}
		}

		if (read_bytes == 0) {
			fprintf(stderr, "texto: fin de la comunicacion -- sock = %d\n", sock);
			status = 2;
			break;
		}
		
		state->buf_size += read_bytes;
	}

	// parsear comando

	int line_count = 0;
	for (int i = 0; i < state->buf_size; ++i)
		line_count += state->buf[i] == '\n';

	if (state->buf_size == TEXT_CLIENT_BUF_SIZE && line_count == 0) {
		return MA_ERROR;
	}

	struct text_command cmd;
	char* cursor = state->buf;
	char* buf_end = state->buf + state->buf_size;
	enum cmd_output res;
	for (; line_count > 0; --line_count) {
		switch (parse_text_command(&cursor, buf_end, &cmd)) {
			case PARSED:
				fprintf(stderr, "corriendo comando: tag = %d\n", cmd.tag);
				res = run_text_command(store, &cmd);				
				if (respond_text_command(data->fd, &cmd, res) < 0) {
					// TODO sth
					return MA_ERROR;
				}
				break;
			default:
				fprintf(stderr, "no parse\n");
				return MA_ERROR;
		}
	}

	// los comandos recibidos son correctos y ya los corrimos
	// reseteamos el buffer conservando lo que queda
	state->buf_size = buf_end - cursor; 
	memmove(state->buf, cursor, state->buf_size);

	switch (status) {
		case 1: return MA_OK;
		case 2: return MA_STOP;
		default: assert(0);
	}

}

// Mandamos el mensaje en partes, abortando si el write falla
int respond_biny_command(int client_socket, struct biny_command* cmd, enum cmd_output res) {
	int out_val = 0;
	uint8_t res_code = cmd_output_code(res);
	if (write(client_socket, &res_code, 1) != 1) { 
		out_val = -1;
	}
	else if (res == CMD_OK) {
		if (cmd->tag == GET || cmd->tag == TAKE) {
			assert(cmd->val);
			uint32_t val_size = htonl(cmd->val_len);
			if (write(client_socket, &val_size, 4) != 4) {
				out_val = -1;
			}
			else if (write(client_socket, cmd->val, cmd->val_len) != cmd->val_len) {
				out_val = -1;
			}
		}
		if (cmd->tag == STATS) {
			fprintf(stderr, "no implementado :(\n");
		}
	}
	if (cmd->val_size > 0) {
		free(cmd->val);
		cmd->val = NULL;
		cmd->val_size = cmd->val_len = 0;
	}
	if (out_val < 0) {
		fprintf(stderr, "error comunicandose con el cliente\n");
	}
	return out_val;
}

enum message_action handle_biny_message(struct fd_data* data, int events, kv_store* store) {
	int sock = data->fd;

	struct biny_client_state* state = data->client_state.biny;
	assert(state);

	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		fprintf(stderr, "HANGUP\n");
		return MA_STOP;
	}

	int status = 0;

	while (1) {
		int read_bytes = read(sock, state->buf + state->buf_size, BINY_CLIENT_BUF_SIZE - state->buf_size);

		if (read_bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				status = 1;
				break;
			} else {
				return MA_ERROR;
			}
		}

		if (read_bytes == 0) {
			fprintf(stderr, "binario: fin de la comunicacion -- sock = %d\n", sock);
			status = 2;
			break;
		}
		
		state->buf_size += read_bytes;
	}

	uint8_t* cursor	= state->buf;
	uint8_t* buf_end = state->buf + state->buf_size; 
	enum status parse_status;
	enum cmd_output res;
PARSE: 
	parse_status = parse_biny_command(&cursor, buf_end, &state->cmd);
	switch (parse_status) {
		case PARSED:
			fprintf(stderr, "corriendo comando: tag = %d\n", state->cmd.tag);
			res = run_biny_command(store, &state->cmd);
			if (respond_biny_command(sock, &state->cmd, res) < 0) {
				// TODO reiniciar el buffer, algo mas?
				return MA_ERROR;
			}
			goto PARSE;
		case KEY_NEXT:
			state->cmd.key = malloc(state->cmd.key_size);
			assert(state->cmd.key); // TODO manejar esto
			goto PARSE;
		case VAL_NEXT:
			state->cmd.val = malloc(state->cmd.val_size);
			assert(state->cmd.val); // TODO manejar esto
			goto PARSE;
		case INCOMPLETE:
			state->buf_size = buf_end - cursor;
			memmove(state->buf, cursor, state->buf_size);
			break;
		default:
			fprintf(stderr, "no parse\n");
			return MA_ERROR; 
	}	
 

	switch (status) {
		case 1: return MA_OK;
		case 2: return MA_STOP;
		default: assert(0);
	}

}

int main(int argc, char** argv) {

	int listen_text_sock, listen_biny_sock;
	// no nos pasaron sockets --> estamos debuggeando
	if (argc == 1) {
		listen_text_sock = create_listen_socket("localhost", "8000");
		listen_biny_sock = create_listen_socket("localhost", "8001");
	}
	else if (argc == 3) {
		listen_text_sock = atoi(argv[1]);
		listen_biny_sock = atoi(argv[2]);
	}
	else {
		fprintf(stderr, "Numero de argumentos incompatible\n");
		exit(EXIT_FAILURE);
	}
	if (listen_text_sock < 0) exit(EXIT_FAILURE);
	if (listen_biny_sock < 0) exit(EXIT_FAILURE);

	int err;

	int epollfd = epoll_create(1);
	if (epollfd < 0) {
		perror("epoll.1");
		exit(EXIT_FAILURE);
	}

	register_listen_socket_first(epollfd, listen_text_sock, TEXT);
	register_listen_socket_first(epollfd, listen_biny_sock, BINY);
	
	// inicializar kv_store
	kv_store* store = kv_store_init();
	if (store == NULL) {
		fprintf(stderr, "error iniciando cache\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "ENTRANDO AL LOOP EPOLL\n");

	while (1) {
		fprintf(stderr, "WAIT\n");
		struct epoll_event evt;
		int event_count = epoll_wait(epollfd, &evt, 1, -1);

		fprintf(stderr, "DEJO DE ESPERAR\n");

		assert(event_count > 0);

		assert(event_count == 1); // TODO: borrar cuando manejemos varios eventos

		struct fd_data* data = evt.data.ptr;

		switch (data->type) {
		case FD_TYPE_TEXT_LISTEN: {

			fprintf(stderr, "TEXTO: NUEVO CLIENTE!\n");

			int listen_sock = data->fd;

			int conn_sock;
			enum message_action action = handle_new_client(listen_sock, &conn_sock);

			if (action == MA_OK) {
				register_client_socket_first(epollfd, conn_sock, TEXT);
			}

			register_listen_socket_again(epollfd, listen_sock, data);

		} break;
		case FD_TYPE_BINARY_LISTEN: {
			
			fprintf(stderr, "BINARIO: NUEVO CLIENTE!\n");

			int listen_sock = data->fd;

			int conn_sock;
			enum message_action action = handle_new_client(listen_sock, &conn_sock);

			if (action == MA_OK) {
				register_client_socket_first(epollfd, conn_sock, BINY);
			}

			register_listen_socket_again(epollfd, listen_sock, data);

		} break;
		case FD_TYPE_TEXT_CONN: {

			fprintf(stderr, "TEXTO: ME HABLA UN CLIENTE!\n");
			fprintf(stderr, "evt flags = %8x\n", evt.events);

			enum message_action action = handle_text_message(data, evt.events, store);

			int sock = data->fd;
			if (action == MA_OK) {
				register_client_socket_again(epollfd, sock, data);
			} else {
				if (action == MA_ERROR) {
					fprintf(stderr, "error handle_text_message sock = %d\n", sock);
				}
				free(data->client_state.text);
				free(data);
				close(sock);
			}


		} break;
		case FD_TYPE_BINARY_CONN: {
			
			fprintf(stderr, "BINARIO: ME HABLA UN CLIENTE!\n");
			fprintf(stderr, "evt flags = %8x\n", evt.events);

			enum message_action action = handle_biny_message(data, evt.events, store);

			int sock = data->fd;
			if (action == MA_OK) {
				register_client_socket_again(epollfd, sock, data);
			} else {
				if (action == MA_ERROR) {
					fprintf(stderr, "error handle_biny_message sock = %d\n", sock);
				}
				free(data->client_state.biny);
				free(data);
				close(sock);
			}
		} break;
		}
	}

	return 0;
}
