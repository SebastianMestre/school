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
#include "text_mode_parser.h"

struct fd_data {
	enum fd_type {
		FD_TYPE_BINARY_LISTEN,
		FD_TYPE_TEXT_LISTEN,
		FD_TYPE_BINARY_CONN,
		FD_TYPE_TEXT_CONN,
	} type;
	int fd;
	struct text_client_state* text_client_state;
};

#define TEXT_CLIENT_BUF_SIZE 2048

struct text_client_state {
	int gen_start;
	int gen;
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


void register_listen_socket_first_time(int epollfd, int sock) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->type = FD_TYPE_TEXT_LISTEN;
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

void register_client_socket_first_time(int epollfd, int sock) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->type = FD_TYPE_TEXT_CONN;
	data->fd = sock;
	data->text_client_state = create_text_client_state();
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

	struct text_client_state* state = data->text_client_state;
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
			fprintf(stderr, "modo texto: fin de la comunicacion -- sock = %d\n", sock);
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

int main() {

	int err;

	int epollfd = epoll_create(1);
	if (epollfd < 0) {
		perror("epoll.1");
		exit(EXIT_FAILURE);
	}

	int listen_sock = create_listen_socket("localhost", "8000");
	if (listen_sock < 0) exit(EXIT_FAILURE);

	register_listen_socket_first_time(epollfd, listen_sock);

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

			fprintf(stderr, "NUEVO CLIENTE!\n");

			int listen_sock = data->fd;

			int conn_sock;
			enum message_action action = handle_new_client(listen_sock, &conn_sock);

			if (action == MA_OK) {
				register_client_socket_first_time(epollfd, conn_sock);
			}

			register_listen_socket_again(epollfd, listen_sock, data);

		} break;
		case FD_TYPE_BINARY_LISTEN: {
			int listen_sock = data->fd;
			// TODO
			exit(EXIT_FAILURE);
		} break;
		case FD_TYPE_TEXT_CONN: {

			fprintf(stderr, "ME HABLA UN CLIENTE!\n");
			fprintf(stderr, "evt flags = %8x\n", evt.events);

			enum message_action action = handle_text_message(data, evt.events);

			int sock = data->fd;
			if (action == MA_OK) {
				register_client_socket_again(epollfd, sock, data);
			} else {
				if (action == MA_ERROR) {
					fprintf(stderr, "error handle_text_message sock = %d\n", sock);
				}
				free(data->text_client_state);
				free(data);
				close(sock);
			}


		} break;
		case FD_TYPE_BINARY_CONN: {
			int sock = data->fd;
			// TODO
			exit(EXIT_FAILURE);
		} break;
		}
	}

	return 0;
}
