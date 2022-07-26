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

enum message_action handle_text_message(struct fd_data* data, int events) {
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
	for (; line_count > 0; --line_count) {
		switch (parse_text_command(&cursor, buf_end, &cmd)) {
			case OK:
				// TODO correr comando
				fprintf(stderr, "correr comando: tag = %d\n", cmd.tag);
				break;
			default:
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
	}

}

enum message_action handle_biny_message(struct fd_data* data, int events) {
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

	// TODO parsear comando
	state->buf_size = 0; 

	switch (status) {
		case 1: return MA_OK;
		case 2: return MA_STOP;
	}

}

int main() {

	int err;

	int epollfd = epoll_create(1);
	if (epollfd < 0) {
		perror("epoll.1");
		exit(EXIT_FAILURE);
	}

	int listen_text_sock = create_listen_socket("localhost", "8000");
	if (listen_text_sock < 0) exit(EXIT_FAILURE);

	int listen_biny_sock = create_listen_socket("localhost", "8001");
	if (listen_biny_sock < 0) exit(EXIT_FAILURE);

	register_listen_socket_first(epollfd, listen_text_sock, TEXT);
	register_listen_socket_first(epollfd, listen_biny_sock, BINY);
	
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

			enum message_action action = handle_text_message(data, evt.events);

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

			printf("Todavia no implementamos esto jeje adios\n");
			exit(EXIT_FAILURE);
			enum message_action action;
			// enum message_action action = handle_biny_message(data, evt.events);

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
