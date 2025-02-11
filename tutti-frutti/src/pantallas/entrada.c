#include "./pantallas.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../utils/cross_compatible.h"
#include "../utils/bst.h"
#include "../utils/constantes.h"

bool es_valida (char* palabra, char letra);

void sumar_puntos (estado_t* estado, char*** palabras);

transicion_t entrada ( estado_t* estado ) {

    // string[][]
    char*** input = malloc(sizeof(char**) * estado->cantidad_jugadores);

    const char* categoria[] = {
        "animales",
        "colores",
        "comidas",
        "flores y plantas",
        "frutas",
        "nombres",
        "paises"
    };

    // se toma el input de cada jugador
    for (int i = 0; i < estado->cantidad_jugadores; ++i) {
        input[i] = malloc(sizeof(char*) * CATEGORIAS);

        clear_screen();

        printf("La letra de esta ronda es: %c\n", estado->letras[estado->ronda]);
        printf("\tTurno : %s\n", estado->nombres[i]);

        for (int j = 0; j < CATEGORIAS; ++j) {
            printf("Ingrese su respuesta para la categoria %s:\n\t", categoria[j]);
            input[i][j] = read_line();
        }
    }

    sumar_puntos(estado, input);

    for(int i = 0; i < estado->cantidad_jugadores; ++i)
        for(int j = 0; j < CATEGORIAS; ++j)
            free(input[i][j]);

    for(int i = 0; i < estado->cantidad_jugadores; ++i)
        free(input[i]);

    free(input);

    bool fin_del_juego = false;

    for (int i = 0; i < estado->cantidad_jugadores; ++i) {
        if (estado->puntos[i] >= 200) {
            fin_del_juego = true;

            // uso la variable donde guardo la ronda para guardar el
            // ganador una vez termina el juego
            estado->ronda = i;
            break;
        }
    }

    if ( fin_del_juego )
        return (transicion_t){4, estado};
    else {
        estado->ronda++;
        return (transicion_t){1, estado};
    }
}

bool es_valida (char* palabra, char letra) {
    if (letra == 'K' || letra == 'Z' || letra == ntilde()) {
        // si es K Z o ntilde se busca una aparicion
        for(int i = 0; palabra[i]; ++i)
            if (palabra[i] == letra)
                return true;
        return false;
    } else {
        // si no se busca coincidir en la primera letra
        return palabra[0] == letra;
    }
}

void sumar_puntos (estado_t* estado, char*** palabras) {

    for (int j = 0; j < CATEGORIAS; ++j) {
        BST palabras_usadas = BST_new(&strcmp_wrapper);

        // para cada jugador
        for (int i = 0; i < estado->cantidad_jugadores; ++i)
            // si se verifica que su j-esima palabra sea valida
            if(es_valida(palabras[i][j], estado->letras[estado->ronda]))
                // y pertenezca al diccionario
                if(BST_find(estado->diccionario[j], palabras[i][j]))
                    // se anota
                    BST_insert(palabras_usadas, palabras[i][j]);

        // para cada jugador
        for (int i = 0; i < estado->cantidad_jugadores; ++i) {
            // cuya j-esima palabra es valida
            if (es_valida(palabras[i][j], estado->letras[estado->ronda])) {
                // y pertenece al diccionario
                if (BST_find(estado->diccionario[j], palabras[i][j])) {

                    /* perdon por los numeros magicos :( */

                    // si este fue el unico en responder, suma 20
                    if(palabras_usadas->size == 1) {
                        estado->puntos[i] += 20;
                    // si no, si este tuvo una respuesta unica, suma 10
                    } else if (BST_count(palabras_usadas, palabras[i][j]) < 2) {
                        estado->puntos[i] += 10;
                    // si no, la respuesta esta repetida y suma 5
                    } else {
                        estado->puntos[i] += 5;
                    }
                }
            }
        }

        // se borra el arbol de palabras pero no las palabras
        BST_free_shared(palabras_usadas);
    }
}
