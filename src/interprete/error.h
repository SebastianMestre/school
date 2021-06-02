#ifndef ERROR_H
#define ERROR_H

// Identifica un error.
typedef enum {
	E_PARSER_ALIAS, 			// se esperaba un alias
	E_PARSER_CARGA, 			// sintaxis en la carga
	E_PARSER_EXPRESION,	// expresion malformada
	E_PARSER_OPERACION, 	// error en el grammar  
	E_PARSER_VACIA, 			// expresion vacia
  E_INTERPRETE_EVAL,
} ErrorTag;

#endif // ERROR_H