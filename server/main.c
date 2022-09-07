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
#include "kv_store.h"
#include "commands.h"
#include "text_mode_parser.h"
#include "biny_mode_parser.h"
#include "try_alloc.h"

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

// TODO que hacemos si no podemos alocar el client_state?

#define TEXT_CLIENT_BUF_SIZE 2048

struct text_client_state {
	size_t buf_size;
	char buf[TEXT_CLIENT_BUF_SIZE];
};

struct text_client_state* create_text_client_state(kv_store* store) {
	struct text_client_state* result = try_alloc(store, sizeof(*result));
	if (result != NULL) {
		memset(result, 0, sizeof(*result));
	}
	return result;
}

#define BINY_CLIENT_BUF_SIZE 2048

struct biny_client_state {
	size_t buf_size;
	uint8_t buf[BINY_CLIENT_BUF_SIZE];
	struct biny_command cmd;
};

struct biny_client_state* create_biny_client_state(kv_store* store) {
	struct biny_client_state* result = try_alloc(store, sizeof(*result));
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


enum message_action { MA_ERROR, MA_STOP, MA_OK };

enum message_action handle_new_client(int listen_sock, int* out_sock) {
	*out_sock = -1;
	int conn_sock = accept4(listen_sock, NULL, NULL, SOCK_NONBLOCK);

	if (conn_sock < 0)
		return MA_ERROR;

	*out_sock = conn_sock;
	return MA_OK;
}


// intenta leer del file descriptor hasta llenar el bufer dado.
// devuelve un codigo de estado
//
// CODIGOS
//   0 -- ok, se logro llenar el buffer
//  -1 -- el file descriptor devolvio EAGAIN
//  -2 -- se alcanzo el fin del file descriptor
//  -3 -- se produjo un error al leer del file descriptor
int read_until(int fd, unsigned char* buf, size_t* buf_size, size_t buf_capacity) {

	while (*buf_size < buf_capacity) {
		int read_bytes = read(fd, buf + *buf_size, buf_capacity - *buf_size);

		if (read_bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return -1; // no hay mas input, hay que volver a epoll
			} else {
				return -3; // error, hay que frenar ahora
			}
		}

		if (read_bytes == 0) {
			return -2; // fin del input. procesar lo que queda y cortar
		}

		*buf_size += read_bytes;
	}

	return 0; // buffer lleno, se puede volver a llamar cuando haya mas espacio
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

int interpret_text_commands(int sock, struct text_client_state* state, kv_store* store) {
	struct text_command cmd;
	char* cursor = state->buf;
	char* buf_end = state->buf + state->buf_size;

	while (cursor != buf_end) {
		enum status parse_status = parse_text_command(&cursor, buf_end, &cmd);

		// el comando es invalido, devuelvo error
		if (parse_status == INVALID) return MA_ERROR;

		// necesito mas input, reseteo el bufer y salgo
		if (parse_status == INCOMPLETE) break;

		assert(parse_status == PARSED);

		enum cmd_output res = run_text_command(store, &cmd);				
		if (respond_text_command(sock, &cmd, res) < 0) return MA_ERROR;
	}

	state->buf_size = buf_end - cursor;
	memmove(state->buf, cursor, state->buf_size);

	return MA_OK;
}

enum message_action handle_text_message(struct fd_data* data, int events, kv_store* store) {
	int sock = data->fd;
	struct text_client_state* state = data->client_state.text;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) return MA_STOP;

	while (1) {
		int read_status = read_until(sock, state->buf, &state->buf_size, TEXT_CLIENT_BUF_SIZE);
		if (read_status == -3) return MA_ERROR;

		int interpret_status = interpret_text_commands(sock, state, store);
		if (interpret_status == MA_ERROR) return MA_ERROR;

		if (read_status ==  0) continue;
		if (read_status == -1) return MA_OK;
		if (read_status == -2) return MA_STOP;
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

	int status = read_until(sock, state->buf, &state->buf_size, BINY_CLIENT_BUF_SIZE);

	if (status == -3) return MA_ERROR;

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
		case 0: return MA_OK; // TODO: está ok esto? pueden haber quedado bytes en el socket

		case -1: return MA_OK;
		case -2: return MA_STOP;
		default: assert(0);
	}

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
