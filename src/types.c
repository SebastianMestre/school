#include "types.h"

inline static void nop_func1(void * arg0, void* metadata) {
	arg0 = arg0;
	metadata = metadata;
}

inline static void nop_func2(void const* arg0, void const* arg1, void* metadata) {
	arg0 = arg0;
	arg1 = arg1;
	metadata = metadata;
}

Destructor const nop_dtor = (Destructor){ nop_func1, nullptr };
