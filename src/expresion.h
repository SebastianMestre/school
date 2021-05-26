#ifndef EXPRESION_H
#define EXPRESION_H

typedef struct Expresion Expresion;

struct Expresion {
	int operador; // TODO? puntero a funcion, o que?
	Expresion* operando[2];
};

#endif // EXPRESION_H
