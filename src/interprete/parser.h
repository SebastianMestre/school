#ifndef PARSER_H
#define PARSER_H

typedef struct TablaOps TablaOps;
typedef struct Expresion Expresion;

// En el contexto de una Sentencia, sirve para interpretar la informacion de esta
typedef enum {
	S_CARGA,    // ALIAS = cargar EXPR
	S_IMPRIMIR, // imprimir ALIAS
	S_EVALUAR,  // evaluar ALIAS
	S_SALIR,    // salir
	S_INVALIDO, // (un error)
} SentenciaTag;

// En el contexto de una sentencia, identifica un error encontrado en el parseo.
typedef enum {
	E_ALIAS, 			// debe especificarse un alias para esta operacion
	E_CARGA, 			// sintaxis en la carga
	E_EXPRESION,	// expresion malformada
	E_OPERACION, 	// error en el grammar  
	E_VACIA, 			// expresion vacia
} ErrorTag;

// Representa una accion que debe tomar el programa
typedef struct Sentencia {
	SentenciaTag tag;
	char const* alias;
	int alias_n;
	Expresion* expresion;
	ErrorTag error;
} Sentencia;

typedef struct {
	char const* resto;
	Sentencia sentencia;
} Parseado;

// Analiza el principio del string, y reconoce la primera accion que debe tomar
// el interprete. Luego, devuelve una representacion de ella, junto a un puntero
// al resto del string, que todavia no fue analizado.
//
// # uso de memoria
// argumentos: No limpia nada
// resultado: depende de sentencia.tag
//  - si es S_CARGA, se debe limpiar la sentencia.expresion
//  - En el resto de los casos, nada se debe limpiar
Parseado parsear(char const* str, TablaOps* tabla_ops);

#endif // PARSER_H
