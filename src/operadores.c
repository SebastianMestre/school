#include "operadores.h"

int suma(int* args) {
	return args[0] + args[1];
}

int resta(int* args) {
	return args[0] - args[1];
}

int opuesto(int* args) {
	return -args[0];
}

int producto(int* args) {
	return args[0] * args[1];
}

int division(int* args) {
	return args[0] / args[1];
}

int modulo(int* args) {
	return args[0] % args[1];
}

int potencia(int* args) {
	int b = args[0];
	int n = args[1];
	if (n < 0)
		return b > 1 ? 0 : 1;
	int k = 1;
	while (n--)
		k *= b;
	return k;
}
