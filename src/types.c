#include "types.h"

inline static void nop_func1(void * arg0, void* metadata) { }

inline static void nop_func2(void const* arg0, void const* arg1, void* metadata) { }

Destructor const nop_dtor = (Destructor){ nop_func1, nullptr };
