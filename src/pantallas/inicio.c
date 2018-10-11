#include "./pantallas.h"

#include <stdio.h>

#include "../utils/cross_compatible.h"

transicion_t inicio ( estado_t* estado ) {
    clear_screen();

    puts("ingrese la cantidad de jugadores [2,5]");

    int cantidad_jugadores;

    leer_cantidad_jugadores:
    scanf("%d%*c", &cantidad_jugadores);

    if ( cantidad_jugadores < 2 || 5 < cantidad_jugadores ) {
        puts("cantidad de jugadores fuera del rango [2,5]\n"
             "vuelva a ingresar la cantidad de jugadores");

        goto leer_cantidad_jugadores;
    }

    estado = crear_estado(cantidad_jugadores);

    for (int i = 0; i < cantidad_jugadores; ++i) {
        printf("jugador %d ingrese su nombre : ", i+1);

        estado->nombres[i] = read_line();
    }
    
    return (transicion_t){1, estado};
}
