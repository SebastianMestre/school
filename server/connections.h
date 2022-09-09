#pragma once

int create_listen_socket(char const* address, char const* port);
int register_listen_socket_first(int epollfd, int sock, void* data);
int register_listen_socket_again(int epollfd, int sock, void* data);
int register_client_socket_first(int epollfd, int sock, void* data);
int register_client_socket_again(int epollfd, int sock, void* data);