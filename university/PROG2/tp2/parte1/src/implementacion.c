#include "implementacion.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// #############################################################################
// #                                                                           #
// # la documentacion de cada funcion est'a en la cabecera, `implementacion.h` #
// #                                                                           #
// #############################################################################


void hacer_minuscula(char* str) {
	for (; *str; ++str)
		*str = tolower(*str);
}

bool palabras_iguales(char* s1, char* s2) {
	const int n = strlen(s1);
	if (n != strlen(s2)) return false;
	bool eq_f = true;
	bool eq_r = true;
	for (int i = n; i--;) {
		// aca usamos tolower por las dudas. Realmente no hace falta porque
		// pasamos los strings a minusculas cuando se ingresan
		char c1 = tolower(s1[i]);
		char c2f = tolower(s2[i]);
		char c2r = tolower(s2[n-1-i]);
		if (c1 != c2f) eq_f = false;
		if (c1 != c2r) eq_r = false;
	}
	return eq_f || eq_r;
}

void lista_insertar(struct lista** cabeza, struct palabra_con_direccion datos) {
	struct lista* nuevo_nodo = malloc(sizeof(struct lista));
	nuevo_nodo->datos = datos;
	nuevo_nodo->siguiente = *cabeza;
	*cabeza = nuevo_nodo;
}

bool lista_contiene_palabra(struct lista* cabeza, char* palabra) {
	for (struct lista* it = cabeza; it; it = it->siguiente)
		if (palabras_iguales(it->datos.palabra, palabra))
			return true;
	return false;
}

void lista_liberar(struct lista* cabeza) {
	while (cabeza) {
		// guardamos el proximo en una variable intermedia porque no lo debemos
		// leer despues de liberar la cabeza
		struct lista* proximo = cabeza->siguiente;
		free(cabeza->datos.palabra);
		free(cabeza);
		cabeza = proximo;
	}
}
