#ifndef PARSER_H
#define PARSER_H

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
	void* expresion; // TODO reemplazar por Expresion*
} Parseado;

typedef struct TablaOps TablaOps;

Parseado parsear(char const* str, TablaOps* tabla_ops);

#endif // PARSER_H
