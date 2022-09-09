#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/errno.h>
#include <unistd.h>

#include "connections.h"

#define ADDRESS "localhost"
#define TEXT_PORT "888"
#define BINY_PORT "889"

#define SERVER "build/server"

char* int_to_string(int a) {
	int len = snprintf(NULL, 0, "%d", a);
	char* str = malloc(len + 1); 
	assert(str);
	snprintf(str, len + 1, "%d", a);
	return str;
}  

int main() {
	gid_t gid = getgid();
	uid_t uid = getuid();
	
	if (uid != 0) {
		fprintf(stderr, "error: Necesito privilegios de root.\n");
		exit(1);
	}
	// corriendo como root

	int text_socket = create_listen_socket(ADDRESS, TEXT_PORT);
	if (text_socket < 0) {
		fprintf(stderr, "Error en la conexion al puerto %s\n", TEXT_PORT);
		exit(1);
	}
	int biny_socket = create_listen_socket(ADDRESS, BINY_PORT);
	if (biny_socket < 0) {
		close (text_socket);
		fprintf(stderr, "Error en la conexion al puerto %s\n", BINY_PORT);
		exit(1);
	}

	if (setgid(gid) < 0 || setuid(uid) < 0) {
		perror("No pude bajar privilegios");
		close(text_socket);
		close(biny_socket);
		exit(1);
	}

	// levanto el servidor
	char* arg1 = int_to_string(text_socket); assert(arg1);
	char* arg2 = int_to_string(biny_socket); assert(arg2);

	execl(SERVER, SERVER, arg1, arg2, NULL);
	
	// execl fallo
	fprintf(stderr, "No pude levantar el servidor\n");
	free(arg1); free(arg2);
	close(text_socket); close(biny_socket);
	exit(1);   
}