#pragma once

#include "bst.h"

typedef struct {
    int cantidad_jugadores;
    char* nombres[5];
    int puntos[5];

    char letras[27];
    int ronda;
    BST diccionario[7];
} estado_t;

estado_t* crear_estado (int cantidad_jugadores);
