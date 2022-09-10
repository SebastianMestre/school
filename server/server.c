#define _GNU_SOURCE

#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "message_action.h"
#include "connections.h"
#include "text_client.h"
#include "biny_client.h"
#include "try_alloc.h"

struct fd_data {
	enum fd_type {
		FD_TYPE_BINARY_LISTEN,
		FD_TYPE_TEXT_LISTEN,
		FD_TYPE_BINARY_CONN,
		FD_TYPE_TEXT_CONN,
	} type;
	int fd;
	union client_state {
		struct text_client_state *text;
		struct biny_client_state *biny;
	} client_state;
};

static enum message_action handle_new_client(int listen_sock, int *out_sock) {
	*out_sock = -1;
	int conn_sock = accept4(listen_sock, NULL, NULL, SOCK_NONBLOCK);

	if (conn_sock < 0) return MA_ERROR;

	*out_sock = conn_sock;
	return MA_OK;
}

void register_listener(int epollfd, int sock, enum protocol protocol) {
	struct fd_data *data = calloc(1, sizeof(*data));
	if (data == NULL) {
		fprintf(stderr, "fallo al iniciar: memoria insuficiente\n");
		exit(EXIT_FAILURE);
	}

	switch (protocol) {
	case TEXT:
		data->type = FD_TYPE_TEXT_LISTEN;
		break;
	case BINY:
		data->type = FD_TYPE_BINARY_LISTEN;
		break;
	}
	data->fd = sock;
	
	if (register_listen_socket_first(epollfd, sock, data) < 0) {
		fprintf(stderr, "fallo al iniciar: register listen_socket\n");
		exit(EXIT_FAILURE);
	}
}

static int register_biny_client(int epollfd, int sock, kv_store *store) {
	struct fd_data *data = try_alloc(store, sizeof(*data));
	if (data == NULL)
		goto error_nothing;

	memset(data, 0, sizeof(*data));
	data->fd = sock;
	data->type = FD_TYPE_BINARY_CONN;
	data->client_state.biny = create_biny_client_state(store);

	if (data->client_state.biny == NULL)
		goto error_data;

	if (register_client_socket_first(epollfd, sock, data) < 0)
		goto error_state;

	return 0;

error_state:
	free_biny_client_state(data->client_state.biny);
error_data:
	free(data);
error_nothing:
	return -1;
}

static int register_text_client(int epollfd, int sock, kv_store *store) {
	struct fd_data *data = try_alloc(store, sizeof(*data));
	if (data == NULL)
		goto error_nothing;

	memset(data, 0, sizeof(*data));
	data->fd = sock;
	data->type = FD_TYPE_TEXT_CONN;
	data->client_state.text = create_text_client_state(store);

	if (data->client_state.text == NULL)
		goto error_data;

	if (register_client_socket_first(epollfd, sock, data) < 0)
		goto error_state;

	return 0;

error_state:
	free_text_client_state(data->client_state.text);
error_data:
	free(data);
error_nothing:
	return -1;
}

void *server(void *server_data) {
	int epollfd = ((struct server_data *)server_data)->epollfd;
	kv_store *store = ((struct server_data *)server_data)->store;

	while (1) {
		fprintf(stderr, "WAIT\n");
#define MAX_EVENTS 10
		struct epoll_event evt[MAX_EVENTS];
		int event_count = epoll_wait(epollfd, evt, MAX_EVENTS, -1);

		fprintf(stderr, "DEJO DE ESPERAR\n");

		if (event_count < 0 && errno == EINTR) {
			continue;
		}

		assert(event_count > 0);

		fprintf(stderr, "event count: %d\n", event_count);

		for (int i = 0; i < event_count; i++) {
			struct fd_data *data = evt[i].data.ptr;

			switch (data->type) {
			case FD_TYPE_TEXT_LISTEN: {

				fprintf(stderr, "TEXTO: NUEVO CLIENTE!\n");

				int listen_sock = data->fd;

				int conn_sock;
				enum message_action action = handle_new_client(listen_sock, &conn_sock);

				if (action == MA_OK) {
					if (register_text_client(epollfd, conn_sock, store) < 0) {
						close(conn_sock);
					}
				}

				// si falla el registro del listen socket, perdemos la capacidad de tomar clientes
				// en ese caso morimos y listo
				if (register_listen_socket_again(epollfd, listen_sock, data) < 0) {
					fprintf(stderr, "TEXTO ERROR: no se pudo re-registrar el listen socket\n");
					exit(EXIT_FAILURE);
				}
				
			} break;
			case FD_TYPE_BINARY_LISTEN: {
				
				fprintf(stderr, "BINARIO: NUEVO CLIENTE!\n");

				int listen_sock = data->fd;

				int conn_sock;
				enum message_action action = handle_new_client(listen_sock, &conn_sock);

				if (action == MA_OK) {
					if (register_biny_client(epollfd, conn_sock, store) < 0) {
						close(conn_sock);
					}
				}

				// si falla el registro del listen socket, perdemos la capacidad de tomar clientes
				// en ese caso morimos y listo
				if (register_listen_socket_again(epollfd, listen_sock, data) < 0) {
					fprintf(stderr, "BINARIO ERROR: no se pudo re-registrar el listen socket\n");
					exit(EXIT_FAILURE);
				}
			
			} break;
			case FD_TYPE_TEXT_CONN: {

				fprintf(stderr, "TEXTO: ME HABLA UN CLIENTE!\n");
				fprintf(stderr, "evt flags = %8x\n", evt[i].events);

				int sock = data->fd;
				enum message_action action = handle_text_message(data->client_state.text, sock, evt[i].events, store);


				if (action != MA_OK || register_client_socket_again(epollfd, sock, data) < 0) {
					if (action == MA_OK)
						fprintf(stderr, "error registering client sock = %d\n", sock);
					if (action == MA_ERROR)
						fprintf(stderr, "error handle_text_message sock = %d\n", sock);
					free_text_client_state(data->client_state.text);
					free(data);
					close(sock);
				}
			} break;
			case FD_TYPE_BINARY_CONN: {

				fprintf(stderr, "BINARIO: ME HABLA UN CLIENTE!\n");
				fprintf(stderr, "evt flags = %8x\n", evt[i].events);

				int sock = data->fd;
				enum message_action action = handle_biny_message(data->client_state.biny, sock, evt[i].events, store);

				if (action != MA_OK || register_client_socket_again(epollfd, sock, data) < 0) {
					if (action == MA_OK)
						fprintf(stderr, "error registering client sock = %d\n", sock);
					if (action == MA_ERROR)
						fprintf(stderr, "error handle_biny_message sock = %d\n", sock);
					free_biny_client_state(data->client_state.biny);
					free(data);
					close(sock);
				}
			} break;
			}
		}
	}

	return NULL;
}

