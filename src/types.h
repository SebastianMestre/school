#pragma once

#include <stdbool.h>
#include <stdint.h>

#define nullptr ((void*)0)

typedef int (*ComparatorFunction)(void const*, void const*, void*);
typedef struct { ComparatorFunction call; void* metadata; } Comparator;
inline static int call_cmp(Comparator cmp, void const* lhs, void const* rhs) {
	return cmp.call(lhs, rhs, cmp.metadata);
}

typedef void (*DestructorFunction)(void* object, void* metadata);
typedef struct { DestructorFunction call; void* metadata; } Destructor;
inline static void call_dtor(Destructor dtor, void* arg) {
	dtor.call(arg, dtor.metadata);
}
extern Destructor const nop_dtor;

typedef void (*CallbackFunction)(void* argument, void* metadata);
typedef struct { CallbackFunction call; void* metadata; } Callback;
inline static void call_cb(Callback cb, void* arg) {
	cb.call(arg, cb.metadata);
}
