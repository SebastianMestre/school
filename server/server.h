#pragma once

#include "kv_store.h"

enum protocol { TEXT, BINY };

struct server_data {
	int epollfd;
	kv_store* store;
};

// maneja el servidor
// la idea es correr esta funcion con varios threads
void* server(void* server_data); 
// registra socket de escucha
void register_listener(int epollfd, int sock, enum protocol protocol);