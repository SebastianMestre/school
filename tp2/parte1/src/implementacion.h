#ifndef IMPLEMENTACION_H
#define IMPLEMENTACION_H

#include <stdbool.h>

void hacer_minuscula(char* str);

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
bool palabras_iguales(char* s1, char* s2);

// lista_insertar! : Lista** PalabraConDireccion -> ()
// inserta una palabra con direccion a una lista enlazada
void lista_insertar(struct lista** cabeza, struct palabra_con_direccion datos);

// lista_contiene_palabra : Lista* String -> Bool
// responde si una lista contiene una palabra, ignorando mayusculas y direccion
bool lista_contiene_palabra(struct lista* cabeza, char* palabra);

// lista_liberar! : Lista* -> ()
// libera la memoria de una lista, y los strings que contiene
void lista_liberar(struct lista* cabeza);

#endif // IMPLEMENTACION_H
