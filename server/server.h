/**
 * Este modulo se ocupa de atender el servidor.
 * 
 */
#pragma once

#include "kv_store.h"

enum protocol { TEXT, BINY };

struct server_data {
	int epollfd;
	kv_store* store;
};

// Atiende el servidor.
// La idea es correr esta funcion con varios threads.
// -- intenta alocar memoria en el heap --
void* server(void* server_data); 
// Registra socket de escucha en epoll.
void register_listener(int epollfd, int sock, enum protocol protocol);