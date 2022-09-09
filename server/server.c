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

#include "message_action.h"
#include "connections.h"
#include "text_client.h"
#include "biny_client.h"

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
	// TODO usar try_alloc
	struct fd_data *data = calloc(1, sizeof(*data));
	switch (protocol) {
	case TEXT:
		data->type = FD_TYPE_TEXT_LISTEN;
		break;
	case BINY:
		data->type = FD_TYPE_BINARY_LISTEN;
		break;
	}
	data->fd = sock;
	
	if (register_listen_socket_first(epollfd, sock, data) < 0) exit(EXIT_FAILURE);
}

static void register_client(int epollfd, int sock, enum protocol protocol, kv_store *store) {
	// TODO usar try_alloc
	struct fd_data *data = calloc(1, sizeof(*data));
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

	//  TODO no tiene sentido crashear si no se puede registrar al cliente
	if (register_client_socket_first(epollfd, sock, data) < 0) exit(EXIT_FAILURE);
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
					register_client(epollfd, conn_sock, TEXT, store);
				}

				// TODO manejar error igual que abajo
				register_listen_socket_again(epollfd, listen_sock, data);
				
			} break;
			case FD_TYPE_BINARY_LISTEN: {
				
				fprintf(stderr, "BINARIO: NUEVO CLIENTE!\n");

				int listen_sock = data->fd;

				int conn_sock;
				enum message_action action = handle_new_client(listen_sock, &conn_sock);

				if (action == MA_OK) {
					register_client(epollfd, conn_sock, BINY, store);
				}
				if (register_listen_socket_again(epollfd, listen_sock, data) < 0)
					exit(EXIT_FAILURE);
			
			} break;
			case FD_TYPE_TEXT_CONN: {

				fprintf(stderr, "TEXTO: ME HABLA UN CLIENTE!\n");
				fprintf(stderr, "evt flags = %8x\n", evt[i].events);

				enum message_action action = handle_text_message(data->client_state.text, data->fd, evt[i].events, store);

				int sock = data->fd;
				// TODO hacer lo mismo que abajo
				if (action == MA_OK) {
					register_client_socket_again(epollfd, sock, data);
				}
				else {
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
				fprintf(stderr, "evt flags = %8x\n", evt[i].events);

				enum message_action action = handle_biny_message(data->client_state.biny, data->fd, evt[i].events, store);

				int sock = data->fd;
				if (action == MA_OK) {
					// error
					if (register_client_socket_again(epollfd, sock, data) < 0)
						fprintf(stderr, "error registering client sock = %d\n", sock);
					// exito
					else break;
				}
				if (action == MA_ERROR) {
						fprintf(stderr, "error handle_biny_message sock = %d\n", sock);
					}
				free(data->client_state.biny);
				free(data);
				close(sock);
			} break;
			}
		}
	}

	return NULL;
}

