#include "./pantallas.h"

#include <stdio.h>

#include "../utils/cross_compatible.h"

transicion_t preronda ( estado_t* estado ) {
    clear_screen();

    puts("Puntajes:");
    for(int i = 0; i < estado->cantidad_jugadores; ++i)
    printf("\t%s --- %d pts.\n", estado->nombres[i], estado->puntos[i]);

    putchar('\n');

    puts("Preparese para jugar\n"
         "Presione [enter] para continuar");

    getchar();

    return (transicion_t){2, estado};
}
