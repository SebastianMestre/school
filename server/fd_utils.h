#pragma once

#include <stddef.h>


#define FD_EAGAIN -1
#define FD_END -2
#define FD_ERROR -3


// intenta leer del file descriptor hasta llenar el bufer dado.
// en caso de error, devuelve un valor negativo
int read_until(int fd, unsigned char* buf, size_t* buf_size, size_t buf_capacity);
