#ifndef PARSER_H
#define PARSER_H

typedef struct TablaOps TablaOps;
typedef struct EntradaTablaOps EntradaTablaOps;

typedef enum {
	T_NOMBRE,   // una cadena alfanumrica
	T_OPERADOR, // un operador
	T_NUMERO,   // un numero (secuencia de digitos)
	T_IMPRIMIR, // 'imprimir'
	T_EVALUAR,  // 'evaluar'
	T_CARGAR,   // 'cargar'
	T_SALIR,    // 'salir'
	T_IGUAL,    // '='
	T_FIN,      // el final del string
	T_INVALIDO, // un error
} TokenTag;

typedef struct Token {
	TokenTag tag;
	char const* inicio;
	int valor;
	EntradaTablaOps* op;
} Token;

typedef struct Tokenizado {
	char const* resto;
	Token token;
} Tokenizado;


typedef struct ExpresionPostfija {
	int tokens_n;
	Token* tokens;
} ExpresionPostfija;


typedef enum {
	S_CARGA,    // ALIAS = cargar EXPR
	S_IMPRIMIR, // imprimir ALIAS
	S_EVALUAR,  // evaluar ALIAS
	S_SALIR,    // salir
	S_INVALIDO, // (un error)
} SentenciaTag;

typedef struct {
	SentenciaTag tag;
	char const* resto;

	char const* alias;
	int alias_n;
	ExpresionPostfija expresion;
} Parseado;

Tokenizado tokenizar(char const* str, TablaOps* tabla_ops);
Parseado parsear(char const* str, TablaOps* tabla_ops);

#endif // PARSER_H
