#include "implementacion.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// leer_palabra : () -> String
// lee una palabra desde la entrada estandar
char* leer_palabra() {
	// esto es medio feo porque se rompe con strings muy grandes y desperdicia
	// espacio para strings chicos, pero la solucion a eso es molesta y creo que
	// no hace falta para este TP
	char* buffer = malloc(100);
	scanf("%99s", buffer);
	return buffer;
}

// leer_int : () -> Int
// lee un entero desde la entrada estandar
int leer_int() {
	int resultado;
	scanf("%d", &resultado);
	return resultado;
}

int main() {
	int dimension = leer_int();

	struct lista* palabras = NULL;

	while (1) {
		char* palabra = leer_palabra();

		// quizas una comparacion mas debil seria mejor?
		if (strcmp(palabra, "BASTA") == 0)
			break;

		hacer_minuscula(palabra);

		int direccion = leer_int();

		if (lista_contiene_palabra(palabras, palabra)) {
			puts("Palabra ya ingresada!");
		} else {
			lista_insertar(&palabras, (struct palabra_con_direccion){palabra, direccion});
		}
	}

	FILE* fp = fopen("salida", "w");

	fprintf(fp, "DIMENSION\n");
	fprintf(fp, "%d\n", dimension);

	fprintf(fp, "PALABRAS\n");
	for(struct lista* it = palabras; it; it = it->siguiente){ 
		struct palabra_con_direccion* datos = &it->datos;
		fprintf(fp, "%s %d\n", datos->palabra, datos->direccion);
	}

	fclose(fp);

	lista_liberar(palabras);
}
