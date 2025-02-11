#include "fd_utils.h"

#include<errno.h>
#include<unistd.h>
#include<sys/epoll.h>

int read_until(int fd, unsigned char* buf, size_t* buf_size, size_t buf_capacity) {

	while (*buf_size < buf_capacity) {
		int read_bytes = read(fd, buf + *buf_size, buf_capacity - *buf_size);

		if (read_bytes < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return FD_EAGAIN; // no hay mas input, hay que volver a epoll
			} else {
				return FD_ERROR; // error, hay que frenar ahora
			}
		}

		if (read_bytes == 0) {
			return FD_END; // fin del input. procesar lo que queda y cortar
		}

		*buf_size += read_bytes;
	}

	return 0; // buffer lleno, se puede volver a llamar cuando haya mas espacio
}
