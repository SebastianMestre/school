#ifndef ERROR_H
#define ERROR_H

// Identifica un error.
typedef enum {
	E_ALIAS, 			// se esperaba un alias
	E_CARGA, 			// sintaxis en la carga
	E_EXPRESION,	// expresion malformada
	E_OPERACION, 	// error en el grammar  
	E_VACIA, 			// expresion vacia
  E_EVAL,
} ErrorTag;

#endif // ERROR_H