#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>

#include "connections.h"

static int register_epoll(int epollfd, int fd, int op, int flags, void* data) {
	fprintf(stderr, "epoll_ctl <~~ epollfd = %d, fd = %d, op = %d, flags = %x, data = %p\n", epollfd, fd, op, flags, data);
	struct epoll_event evt = {0};
	evt.events = flags;
	evt.data.ptr = data;
	int err = epoll_ctl(epollfd, op, fd, &evt);
	fprintf(stderr, "epoll_ctl ~~> retval = %d, errno = %d\n", err, errno);
	if (err < 0) {
		perror("epoll.2");
		return -1;
	}
    return 0;
}

int create_listen_socket(char const* address, char const* port) {
	int err;

	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		perror("sock.1");
		return -1;
	}

	struct addrinfo hints;
	struct addrinfo* addrinfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(address, port, &hints, &addrinfo);
	if (err) {
		fprintf(stderr, "getaddrinfo: ???\n");
		return -1;
	}

	struct sockaddr_in server_addr = *(struct sockaddr_in*)addrinfo->ai_addr;

	freeaddrinfo(addrinfo);

	err = bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err < 0) {
		perror("bind.1");
		return -1;
	}

	err = listen(listen_sock, 10); // medio trucho el tamanno del backlog
	if (err < 0) {
		perror("listen.1");
		return -1;
	}

	return listen_sock;
}

int register_listen_socket_first(int epollfd, int sock, void* data) {
	int flags = EPOLLIN | EPOLLONESHOT;
	register_epoll(epollfd, sock, EPOLL_CTL_ADD, flags, data);
}

int register_listen_socket_again(int epollfd, int sock, void* data) {
	int flags = EPOLLIN | EPOLLONESHOT;
	register_epoll(epollfd, sock, EPOLL_CTL_MOD, flags, data);
}

int register_client_socket_first(int epollfd, int sock, void* data) {
	int flags = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
	register_epoll(epollfd, sock, EPOLL_CTL_ADD, flags, data);
}

int register_client_socket_again(int epollfd, int sock, void* data) {
	int flags = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
	register_epoll(epollfd, sock, EPOLL_CTL_MOD, flags, data);
}