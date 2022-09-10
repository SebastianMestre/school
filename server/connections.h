/**
 * Este modulo ofrece utiles para el  manejo de conexiones:
 *  manejo de sockets
 *  manejo de epoll.
 */
#pragma once

// Crea un socket para escucha.
int create_listen_socket(char const* address, char const* port);
// Registra socket de escucha en el epoll por primera vez.
int register_listen_socket_first(int epollfd, int sock, void* data);
// Registra de nuevo el socket de escucha en el epoll.
int register_listen_socket_again(int epollfd, int sock, void* data);
// Registra socket de cliente en el epoll por primera vez.
int register_client_socket_first(int epollfd, int sock, void* data);
// Registra de nuevo el socket de cliente en el epoll.
int register_client_socket_again(int epollfd, int sock, void* data);