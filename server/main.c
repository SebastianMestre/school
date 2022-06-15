#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <netdb.h>

struct fd_data {
	enum fd_type {
		FD_TYPE_BINARY_LISTEN,
		FD_TYPE_TEXT_LISTEN,
		FD_TYPE_BINARY_CONN,
		FD_TYPE_TEXT_CONN,
	} type;
	int fd;
};

struct dictionary {
	int size, capacity;
	char** keys;
	int* key_lengths;
	char** values;
	int* value_lengths;
};

int main() {

	int err;

	int epollfd = epoll_create(1);
	if (epollfd < 0) {
		perror("epoll.1");
		exit(EXIT_FAILURE);
	}

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

	err = getaddrinfo("localhost", "8000", &hints, &addrinfo);
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

	err = listen(listen_sock, 4); // TODO: que valor usar para el backlog?
	if (err < 0) {
		perror("listen.1");
		exit(EXIT_FAILURE);
	}

	struct fd_data* sock_data = malloc(sizeof(*sock_data));
	sock_data->type = FD_TYPE_TEXT_LISTEN;
	sock_data->fd = listen_sock;

	struct epoll_event evt;
	evt.events = EPOLLIN | EPOLLONESHOT;
	evt.data.ptr = sock_data;

	err = epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &evt);
	if (err < 0) {
		perror("epoll.1");
		exit(EXIT_FAILURE);
	}

	while (1) {
		int event_count = epoll_wait(epollfd, &evt, 1, -1);
		assert(event_count > 0);

		assert(event_count == 1); // TODO: borrar cuando manejemos varios eventos

		struct fd_data* data = evt.data.ptr;

		switch (data->type) {
		case FD_TYPE_TEXT_LISTEN: {
			int listen_sock = data->fd;

			int conn_sock = accept(listen_sock, NULL, NULL);
			if (conn_sock < 0) {
				perror("accept.1");
				exit(EXIT_FAILURE);
			}

			struct fd_data* sock_data = malloc(sizeof(*sock_data));
			sock_data->type = FD_TYPE_TEXT_CONN;
			sock_data->fd = conn_sock;

			struct epoll_event conn_evt;
			conn_evt.events = EPOLLIN | EPOLLONESHOT;
			conn_evt.data.ptr = sock_data;

			err = epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &conn_evt);
			if (err < 0) {
				perror("epoll.2");
				exit(EXIT_FAILURE);
			}

			err = epoll_ctl(epollfd, EPOLL_CTL_MOD, listen_sock, &evt);
			if (err < 0) {
				perror("epoll.3");
				exit(EXIT_FAILURE);
			}
		} break;
		case FD_TYPE_BINARY_LISTEN: {
			int listen_sock = data->fd;

			int conn_sock = accept(listen_sock, NULL, NULL);
			if (conn_sock < 0) {
				perror("accept.1");
				exit(EXIT_FAILURE);
			}

			struct fd_data* sock_data = malloc(sizeof(*sock_data));
			sock_data->type = FD_TYPE_BINARY_CONN;
			sock_data->fd = conn_sock;

			struct epoll_event conn_evt;
			conn_evt.events = EPOLLIN | EPOLLONESHOT;
			conn_evt.data.ptr = sock_data;

			err = epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &conn_evt);
			if (err < 0) {
				perror("epoll.2");
				exit(EXIT_FAILURE);
			}

			err = epoll_ctl(epollfd, EPOLL_CTL_MOD, listen_sock, &evt);
			if (err < 0) {
				perror("epoll.3");
				exit(EXIT_FAILURE);
			}
		} break;
		case FD_TYPE_TEXT_CONN: {
			int sock = data->fd;
			// TODO
			exit(EXIT_FAILURE);
		} break;
		case FD_TYPE_BINARY_CONN: {
			int sock = data->fd;
			// TODO
			exit(EXIT_FAILURE);
		} break;
		}
	}
}
