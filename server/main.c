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
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <unistd.h>

#include "connections.h"
#include "message_action.h"
#include "kv_store.h"
#include "commands.h"
#include "text_client.h"
#include "biny_client.h"
#include "try_alloc.h"
#include "fd_utils.h"

#define DATA_LIMIT RLIM_INFINITY
#define MAX_THREADS 4

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

void register_client_socket_first(int epollfd, int sock, enum protocol protocol, kv_store* store) {
	struct fd_data* data = calloc(1, sizeof(*data));
	data->fd = sock;

	switch (protocol) {
	case TEXT:
		data->type = FD_TYPE_TEXT_CONN;
		data->client_state.text = create_text_client_state(store);
		if (data->client_state.text == NULL) {
			return;
		}
		break;
	case BINY:
		data->type = FD_TYPE_BINARY_CONN;
		data->client_state.biny = create_biny_client_state(store);
		if (data->client_state.biny == NULL) {
			return;
		}
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


enum message_action handle_new_client(int listen_sock, int* out_sock) {
	*out_sock = -1;
	int conn_sock = accept4(listen_sock, NULL, NULL, SOCK_NONBLOCK);

	if (conn_sock < 0)
		return MA_ERROR;

	*out_sock = conn_sock;
	return MA_OK;
}


int set_memory_limit(rlim_t limit) {
	struct rlimit r = {.rlim_cur = limit, .rlim_max = RLIM_INFINITY};
	if (setrlimit(RLIMIT_DATA, &r) < 0) {
		perror("set_memory_limit: ");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

struct server_data {
	int epollfd;
	kv_store* store;
};

void* server(void* server_data) {
	int epollfd = ((struct server_data*)server_data)->epollfd;
	kv_store* store = ((struct server_data*)server_data)->store;
	
	while (1) {
		fprintf(stderr, "WAIT\n");
		struct epoll_event evt;
		int event_count = epoll_wait(epollfd, &evt, 1, -1);

		fprintf(stderr, "DEJO DE ESPERAR\n");

		if (event_count < 0 && errno == EINTR) {
			continue;
		}

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
				register_client_socket_first(epollfd, conn_sock, TEXT, store);
			}

			register_listen_socket_again(epollfd, listen_sock, data);

		} break;
		case FD_TYPE_BINARY_LISTEN: {
			
			fprintf(stderr, "BINARIO: NUEVO CLIENTE!\n");

			int listen_sock = data->fd;

			int conn_sock;
			enum message_action action = handle_new_client(listen_sock, &conn_sock);

			if (action == MA_OK) {
				register_client_socket_first(epollfd, conn_sock, BINY, store);
			}

			register_listen_socket_again(epollfd, listen_sock, data);

		} break;
		case FD_TYPE_TEXT_CONN: {

			fprintf(stderr, "TEXTO: ME HABLA UN CLIENTE!\n");
			fprintf(stderr, "evt flags = %8x\n", evt.events);

			enum message_action action = handle_text_message(data->client_state.text, data->fd, evt.events, store);

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

			enum message_action action = handle_biny_message(data->client_state.biny, data->fd, evt.events, store);

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

	return NULL;
} 

int main(int argc, char** argv) {

	set_memory_limit(DATA_LIMIT);

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
	
	// levantar threads
	int nprocs = get_nprocs();
	int nthreads = nprocs > MAX_THREADS ? MAX_THREADS : nprocs;
	struct server_data data = {.epollfd = epollfd, .store = store};
	pthread_t tid[nthreads];
	for (int i = 0; i < nthreads; i++) {
		// TODO error
		pthread_create(tid + i, NULL, server, (void*)&data);
	}
	for (int i = 0; i < nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}
