#pragma once

#include "vector.h"

// devuelve un Vector de size_t
// si no es posible formar sum con los numeros dados devuelve un vector vacio
Vector search_by_sum(unsigned* arr, size_t n, unsigned sum);
