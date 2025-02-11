#pragma once

#include <stddef.h>

// ordena el rango desde l hasta r, segun el orden definido por cmp
// size es el tamano de cada elemento
void
quicksort(void* l, void* r, size_t size, Comparator cmp);
