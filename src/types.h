#pragma once

#include <stdbool.h>

#define nullptr ((void*)0)

typedef int (*Comparator)(void const*, void const*);

typedef void (*DestructorFunction)(void* object, void* metadata);
typedef struct { DestructorFunction call; void* metadata; } Destructor;
inline static void call_dtor(Destructor dtor, void* arg) {
	dtor.call(arg, dtor.metadata);
}

typedef void (*CallbackFunction)(void* argument, void* metadata);
typedef struct { CallbackFunction call; void* metadata; } Callback;
inline static void call_cb(Callback cb, void* arg) {
	cb.call(arg, cb.metadata);
}
