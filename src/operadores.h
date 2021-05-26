#ifndef OPERADORES_H
#define OPERADORES_H

typedef int (*FuncionEvaluacion)(int* args);

int suma(int* args);
int resta(int* args);
int opuesto(int* args);
int producto(int* args);
int divison(int* args);
int modulo(int* args);
int potencia(int* args);

#endif // OPERADORES_H
