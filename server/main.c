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



void register_epoll(int epollfd, int fd, int op, int flags, struct fd_data* data) {
	fprintf(stderr, "epoll_ctl <~~ epollfd = %d, fd = %d, op = %d, flags = %x, data = %p\n", epollfd, fd, op, flags, data);
	struct epoll_event evt;
	evt.events = flags;
	evt.data.ptr = data;
	int err = epoll_ctl(epollfd, op, fd, &evt);
	fprintf(stderr, "epoll_ctl ~~> retval = %d, errno = %d\n", err, errno);
	if (err < 0) {
		perror("epoll.2");
		exit(EXIT_FAILURE);
	}
}

void register_listen_socket_first_time(int epollfd, int sock) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->type = FD_TYPE_TEXT_LISTEN;
	data->fd = sock;

	register_epoll(epollfd, sock, EPOLL_CTL_ADD, EPOLLIN | EPOLLONESHOT, data);
}

void register_listen_socket_again(int epollfd, int sock, struct fd_data* data) {
	register_epoll(epollfd, sock, EPOLL_CTL_MOD, EPOLLIN | EPOLLONESHOT, data);
}

void register_client_socket_first_time(int epollfd, int sock) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->type = FD_TYPE_TEXT_CONN;
	data->fd = sock;
	data->text_client_state = create_text_client_state();

	register_epoll(epollfd, sock, EPOLL_CTL_ADD, EPOLLIN | EPOLLONESHOT | EPOLLRDHUP, data);
}

void register_client_socket_again(int epollfd, int sock, struct fd_data* data) {
	register_epoll(epollfd, sock, EPOLL_CTL_MOD, EPOLLIN | EPOLLONESHOT | EPOLLRDHUP, data);
}

int create_listen_socket(char const* address, char const* port) {
	int err;

	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		perror("sock.1");
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints;
	struct addrinfo* addrinfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(address, port, &hints, &addrinfo);
	if (err) {
		fprintf(stderr, "getaddrinfo: ???\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr = *(struct sockaddr_in*)addrinfo->ai_addr;

	freeaddrinfo(addrinfo);

	err = bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("bind.1");
		exit(EXIT_FAILURE);
	}

	err = listen(listen_sock, 10); // medio trucho el tamanno del backlog
	if (err < 0) {
		perror("listen.1");
		exit(EXIT_FAILURE);
	}

	return listen_sock;
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

	char read_buf[TEXT_CLIENT_BUF_SIZE];
	char cmd_buf[TEXT_CLIENT_BUF_SIZE];

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

	char* cursor = state->buf;
	char* buf_end = state->buf + state->buf_size;
	for (; line_count >= 0; --line_count) {
		cursor = parse_command(cursor, buf_end);
	}

	// TODO: matar el cliente si parse_command encuentra un comando invalido
	// TODO: resetear el buffer si parse_command llega al final mientras parsea
	// TODO: correr comandos

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
