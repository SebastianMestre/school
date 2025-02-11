#include "./pantallas.h"

#include <stdio.h>

#include "../utils/cross_compatible.h"

transicion_t confirmacion ( estado_t* estado ) {
    clear_screen();

    return (transicion_t){5, estado};
}
