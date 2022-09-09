#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>	
#include <pthread.h>
#include <unistd.h>

#include "connections.h"
#include "kv_store.h"
#include "server.h"

#define DATA_LIMIT RLIM_INFINITY
#define MAX_THREADS 4

int set_memory_limit(rlim_t limit) {
	struct rlimit r = {.rlim_cur = limit, .rlim_max = RLIM_INFINITY};
	if (setrlimit(RLIMIT_DATA, &r) < 0) {
		perror("set_memory_limit: ");
		exit(EXIT_FAILURE);
	}
	
	return 0;
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

	register_listener(epollfd, listen_text_sock, TEXT);
	register_listener(epollfd, listen_biny_sock, BINY);
	
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
		if (pthread_create(tid + i, NULL, server, (void*)&data) < 0) {
			fprintf(stderr, "error levantando threads\n");
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}
