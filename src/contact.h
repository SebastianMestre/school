#pragma once

#include <stddef.h>

struct _Contact {
	char* name;
	char* surname;
	unsigned age;
	char* phone_number;
};
typedef struct _Contact Contact;

typedef size_t ContactId;
