#pragma once

#include <stddef.h>

struct _Contact {
	char* name;
	char* surname;
	unsigned age;
	char* phone_number;

	// TODO: take this flag out of this structure
	bool indexed;
};
typedef struct _Contact Contact;

typedef size_t ContactId;

struct _OptionalContactId {
	bool active;
	ContactId id;
};
typedef struct _OptionalContactId OptionalContactId;
