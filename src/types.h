#pragma once

#include <stdbool.h>

typedef int (*Comparator)(void const*, void const*);
typedef void (*Destructor)(void* object, void* metadata);

#define nullptr ((void*)0)
