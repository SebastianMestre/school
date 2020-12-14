#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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

// Almacena una palabra y su orientacion en la sopa de letras
// la direccion indica lo siguiente:
// 0 : horizontal de izquierda a derecha
// 1 : horizontal de derecha a izquierda
// 2 : vertical de arriba a abajo
// 3 : vertical de abajo a arriba
// 4 : diagonal de izquierda arriba a derecha abajo
// 5 : diagonal de izquierda abajo a derecha arriba
struct palabra_con_direccion {
	char* palabra;
	int direccion;
};

// Almacena una lista de palabras, en una estructura enlazada
struct lista {
	struct palabra_con_direccion datos;
	struct lista* siguiente;
};

// palabras_iguales : String String -> Bool
// Responde si dos palabras son iguales, ignorando mayusculas y orientacion
// Ejemplos:
// "SEBASTIAN" es igual a    "sebastian"
// "mestre"    es igual a    "ertsem"
// "LopeZ"     es igual a    "zEPOl"
// "cosa"      no es igual a "casa"
// "Python"    no es igual a "C"
bool palabras_iguales(char* s1, char* s2) {
	const int n = strlen(s1);
	if (n != strlen(s2)) return false;
	bool eq_f = true;
	bool eq_r = true;
	for (int i = n; i--;) {
		char c1 = tolower(s1[i]);
		char c2f = tolower(s2[i]);
		char c2r = tolower(s2[n-1-i]);
		if (c1 != c2f) eq_f = false;
		if (c1 != c2r) eq_r = false;
	}
	return eq_f || eq_r;
}

// lista_insertar! : Lista** PalabraConDireccion -> ()
// inserta una palabra con direccion a una lista enlazada
void lista_insertar(struct lista** cabeza, struct palabra_con_direccion datos) {
	struct lista* nuevo_nodo = malloc(sizeof(struct lista));
	nuevo_nodo->datos = datos;
	nuevo_nodo->siguiente = *cabeza;
	*cabeza = nuevo_nodo;
}

// lista_contiene_palabra : Lista* String -> Bool
// responde si una lista contiene una palabra, ignorando mayusculas y direccion
bool lista_contiene_palabra(struct lista* cabeza, char* palabra) {
	for (struct lista* it = cabeza; it; it = it->siguiente)
		if (palabras_iguales(it->datos.palabra, palabra))
			return true;
	return false;
}

// lista_liberar! : Lista* -> ()
// libera la memoria de una lista, y los strings que contiene
void lista_liberar(struct lista* cabeza) {
	while (cabeza) {
		struct lista* proximo = cabeza->siguiente;
		free(cabeza->datos.palabra);
		free(cabeza);
		cabeza = proximo;
	}
}

int main() {
	int dimension = leer_int();

	struct lista* palabras = NULL;

	while (1) {
		char* palabra = leer_palabra();

		// TODO: deberia usar una comparacion mas floja?
		if (strcmp(palabra, "BASTA") == 0)
			break;

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
