#include "tabla_ops.h"
#include "operadores.h"
#include "interpretar.h"

int main (int argc, char** argv) {
	TablaOps tabla = tabla_ops_crear();
	
	cargar_operador(&tabla, "+", 2, suma);
	cargar_operador(&tabla, "-", 2, resta);
	cargar_operador(&tabla, "--", 2, opuesto);
	cargar_operador(&tabla, "*", 2, producto);
	cargar_operador(&tabla, "/", 2, division);
	cargar_operador(&tabla, "%", 2, modulo);
	cargar_operador(&tabla, "^", 2, potencia);

	interpretar(&tabla);
}
