#include "./pantallas.h"

#include <stdio.h>
#include <stdlib.h>

#include "../utils/cross_compatible.h"
#include "../utils/constantes.h"

transicion_t finalizacion ( estado_t* estado ) {
    clear_screen();

    printf("El ganador es: %s !", estado->nombres[estado->ronda]);
    puts("presione [enter] para cerrar el juego");

    getchar();

    for(size_t i = 0; i < sizeof(estado->nombres)/sizeof(estado->nombres[0]); ++i)
        free(estado->nombres[i]);

    for (int i = 0; i < CATEGORIAS; ++i)
        BST_free(estado->diccionario[i]);

    free(estado);

    return (transicion_t){-1, NULL};
}
