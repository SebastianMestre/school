#ifndef EXPRESION_H
#define EXPRESION_H

#include "../funcion_evaluacion.h"
#include "../tabla_ops.h"

typedef struct Expresion Expresion;

// este enum, en el contexto de una expresion, nos permite distinguir si la
// expresion es un alias, un numeros o una operacion
typedef enum {
	X_OPERACION,
	X_NUMERO,
	X_ALIAS,
} ExpressionTag;

struct Expresion {
	ExpressionTag tag;
	// para guardar los valores numericos, o la longitud de un alias, 
	// dependiendo del tag.
	int valor;
	// para guardar el texto de un alias.  
	char const* alias;
	// para guardar las sub-expresiones de una operacion.
	Expresion* sub[2];
	// para guardar la informacion de la operacion (de haber una).
	EntradaTablaOps* op; 
};

/**
 * Devuelve una expresion de numero asociada al valor dado.
 */
Expresion* expresion_numero(int valor);

/**
 * Devuelve una expresion de alias asociada al alias dado. 
 */
Expresion* expresion_alias(char const* alias, int alias_n);

/**
 * Devuelve una expresion de operacion asociada a la operacion dada.
 */
Expresion* expresion_operacion(
	EntradaTablaOps* op, 
	Expresion* sub0, 
	Expresion* sub1);

/**
 * Libera recursivamente el espacio de memoria ocupado por el arbol de
 * expresiones, sin liberar el puntero "alias".  
 */
void expresion_limpiar(Expresion* expresion);

#endif // EXPRESION_H
