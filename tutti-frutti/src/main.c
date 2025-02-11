#include <stdbool.h>
#include <stdio.h>

#include "./utils/cross_compatible.h"
#include "./utils/estado.h"
#include "./pantallas/pantallas.h"
#include "./utils/transicion.h"

int main () {

    transicion_t(*estados[])(estado_t*) = {
        &inicio,
        &preronda,
        &ronda,
        &entrada,
        &confirmacion,
        &finalizacion
    };

    transicion_t transicion = {0, NULL};

    while ( transicion.id != -1 )
        transicion = (*estados[transicion.id])(transicion.estado);

    return 0;
}
