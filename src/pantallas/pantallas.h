#pragma once

#include "../utils/estado.h"
#include "../utils/transicion.h"

transicion_t inicio       ( estado_t* estado );
transicion_t preronda     ( estado_t* estado );
transicion_t ronda        ( estado_t* estado );
transicion_t entrada      ( estado_t* estado );
transicion_t confirmacion ( estado_t* estado );
transicion_t finalizacion ( estado_t* estado );
