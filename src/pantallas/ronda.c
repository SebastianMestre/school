#include <stdio.h>

#include "./pantallas.h"
#include "../utils/cross_compatible.h"

transicion_t ronda ( estado_t* estado ) {
    clear_screen();

    printf("La letra de esta ronda es: %c\n", estado->letras[estado->ronda]);
    puts("presione [enter] para iniciar el ingreso de palabras");

    getchar();

    return (transicion_t){3, estado};
}
