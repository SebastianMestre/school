#pragma once

int register_epoll(int epollfd, int fd, int op, int flags, void* data);
int create_listen_socket(char const* address, char const* port);