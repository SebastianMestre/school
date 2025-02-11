#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cross_compatible.h"
#include "estado.h"
#include "constantes.h"


BST tomar_archivo (const char* camino);

estado_t* crear_estado (int cantidad_jugadores) {
    estado_t* this = malloc(sizeof(*this));

    this->cantidad_jugadores = cantidad_jugadores;
    for (int i = 0; i < cantidad_jugadores; ++i)
        this->puntos[i] = 0;

   /* NOTA DE IMPLEMENTACION:
    * Para dar las letras en un orden aleatorio se usa un arreglo que
    * contiene todas las letras en un orden aleatorio.
    * Posteriormente, un indice (estado_t::ronda) caminara una letra a la vez,
    * lo que garantiza que no habra repeticiones.
    */

    this->ronda = 0;

    // inicializacion del alfabeto
    for (int i = 0; i < LARGO_ABECEDARIO-1; ++i)
        this->letras[i] = 'A' + i;
    this->letras[LARGO_ABECEDARIO-1] = ntilde();

    // mezcla
    srand(time(NULL));
    for (int i = 0; i < LARGO_ABECEDARIO; ++i) {
        int valor_aleatorio = rand() % LARGO_ABECEDARIO;

        char temp = this->letras[i];
        this->letras[i] = this->letras[valor_aleatorio];
        this->letras[valor_aleatorio] = temp;
    }

    // se crea un arbol de busqueda por categoria de palabras
    // estos se rellenan con los archivos provistos

    const char* archivos [CATEGORIAS] = {
        "./../data/palabras/animales.txt",
        "./../data/palabras/colores.txt",
        "./../data/palabras/comidas.txt",
        "./../data/palabras/flores y plantas.txt",
        "./../data/palabras/frutas.txt",
        "./../data/palabras/nombres.txt",
        "./../data/palabras/paises.txt"
    };

    for (int i = 0; i < CATEGORIAS; ++i)
        this->diccionario[i] = tomar_archivo(archivos[i]);

    return this;
}

BST tomar_archivo (const char* camino) {
    // recibe un camino a un archivo e inserta todas las
    // lineas que este contiene a un arbol de busqueda.

    FILE* archivo = fopen(camino, "r");

    BST arbol = BST_new(&strcmp_wrapper);

    for (char* palabra; (palabra = file_read_line(archivo)); BST_insert(arbol, palabra));

    fclose(archivo);

    return arbol;
}
