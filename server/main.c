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

struct fd_data {
	enum fd_type {
		FD_TYPE_BINARY_LISTEN,
		FD_TYPE_TEXT_LISTEN,
		FD_TYPE_BINARY_CONN,
		FD_TYPE_TEXT_CONN,
	} type;
	int fd;
};

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
	struct fd_data* data = malloc(sizeof(*data));
	data->type = FD_TYPE_TEXT_LISTEN;
	data->fd = sock;

	register_epoll(epollfd, sock, EPOLL_CTL_ADD, EPOLLIN | EPOLLONESHOT, data);
}

void register_listen_socket_again(int epollfd, int sock, struct fd_data* data) {
	register_epoll(epollfd, sock, EPOLL_CTL_MOD, EPOLLIN | EPOLLONESHOT, data);
}

void register_client_socket_first_time(int epollfd, int sock) {
	struct fd_data* data = malloc(sizeof(*data));
	data->type = FD_TYPE_TEXT_CONN;
	data->fd = sock;

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

enum message_action_tag { MA_ERROR, MA_CONTINUE, MA_BREAK, MA_OK };
struct message_action { enum message_action_tag tag; int value; };
struct message_action ma_error()       { return (struct message_action){ MA_ERROR, -1 }; }
struct message_action ma_ok(int value) { return (struct message_action){ MA_OK, value }; }
struct message_action ma_continue()    { return (struct message_action){ MA_CONTINUE, -1 }; }
struct message_action ma_break()       { return (struct message_action){ MA_BREAK, -1 }; }

struct message_action handle_new_client(int listen_sock) {
	int conn_sock = accept4(listen_sock, NULL, NULL, SOCK_NONBLOCK);

	if (conn_sock < 0)
		return ma_error();

	return ma_ok(conn_sock);
}

struct message_action handle_text_message(struct fd_data* data, int events) {
	int sock = data->fd;

	#define READ_BUF_SIZE 1024
	char read_buf[READ_BUF_SIZE];

	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
		fprintf(stderr, "HANGUP\n");
		return ma_break();
	}

	while (1) {
		int read_bytes = read(sock, read_buf, READ_BUF_SIZE);

		if (read_bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return ma_continue();
			} else {
				// TODO: algo distinto???
				perror("read.1");
				return ma_error();
			}
		}

		if (read_bytes == 0) {
			fprintf(stderr, "????\n");
			return ma_break();;
		}
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


			struct message_action action = handle_new_client(listen_sock);

			if (action.tag == MA_OK) {
				register_client_socket_first_time(epollfd, action.value);
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


			struct message_action action = handle_text_message(data, evt.events);

			if (action.tag == MA_OK) {
				int sock = data->fd;
				register_client_socket_again(epollfd, sock, data);
			} else {
				int sock = data->fd;
				err = epoll_ctl(epollfd, EPOLL_CTL_DEL, sock, NULL);
				if (err < 0) {
					perror("epoll_ctl.5");
					exit(EXIT_FAILURE); // TODO -- tiene sentido ? NO SE
				}
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