#ifndef EXPRESION_POSTFIJA_H
#define EXPRESION_POSTFIJA_H

typedef struct Token Token;

typedef struct ExpresionPostfija {
	int tokens_n;
	Token* tokens;
} ExpresionPostfija;

void expresion_postfija_limpiar(ExpresionPostfija* expresion);

#endif // EXPRESION_POSTFIJA_H
