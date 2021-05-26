#ifndef EXPRESION_H
#define EXPRESION_H

typedef struct Expresion Expresion;

// TODO: definir la estructura de datos

struct Expresion {
	// concerns:
	// - distinguir entre aliases, numeros y operaciones
	// como guardo las operaciones? directo el puntero a funcion? o pongo un
	// int, representando un indice en una tabla?

	// ExpressionTag tag; ??? (un enum, que puede ser numero, alias, u operacion)
	// int valor; (para guardar los valores numericos)
	// Expresion* sub[2]; (para guardar las sub-expresiones de una operacion)
	// FuncionEvaluacion eval; ??? (para guardar la el tipo de operacion... puede ser que necesite tener la aridad?)
};

#endif // EXPRESION_H
