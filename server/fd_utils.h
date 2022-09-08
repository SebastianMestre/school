#pragma once

#include <stddef.h>

// intenta leer del file descriptor hasta llenar el bufer dado.
// devuelve un codigo de estado
//
// CODIGOS
//   0 -- ok, se logro llenar el buffer
//  -1 -- el file descriptor devolvio EAGAIN
//  -2 -- se alcanzo el fin del file descriptor
//  -3 -- se produjo un error al leer del file descriptor
int read_until(int fd, unsigned char* buf, size_t* buf_size, size_t buf_capacity);
