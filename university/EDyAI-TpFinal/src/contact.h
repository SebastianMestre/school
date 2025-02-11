#pragma once

#include <stddef.h>

struct _Contact {
	char* name;
	char* surname;
	uint32_t age;
	char* phone_number;
};
typedef struct _Contact Contact;

// si has_age es false, el valor de age esta indefinido
struct _IncompleteContact {
	char* name;
	char* surname;
	bool has_age;
	uint32_t age;
	char* phone_number;
};
typedef struct _IncompleteContact IncompleteContact;

typedef size_t ContactId;

struct _OptionalContactId {
	bool active;
	ContactId id;
};
typedef struct _OptionalContactId OptionalContactId;
