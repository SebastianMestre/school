#include "expresion_postfija.h"

#include <stdlib.h>

void expresion_postfija_limpiar(ExpresionPostfija* expresion) {
	free(expresion->tokens);
}
