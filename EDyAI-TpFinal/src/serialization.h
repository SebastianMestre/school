#pragma once

#include <stdio.h>

typedef struct _IncompleteContact IncompleteContact;
typedef struct _Contact Contact;
typedef struct _Storage Storage;
typedef size_t ContactId;

void
write_contact(Contact const* contact, bool braces, FILE* f);

void
write_contacts(Contact const* begin, Contact const* end, bool braces, FILE* f);

void
write_contacts_by_ptr(Contact const** begin, Contact const** end, bool braces, FILE* f);

void
write_contacts_by_id(ContactId const* begin, ContactId const* end, Storage* storage, bool braces, FILE* f);

enum _ReadContactStatus {
	RCS_OK = 0,
	RCS_E_BAD_LINE,
	RCS_E_FIELD_COUNT,
	RCS_E_INVALID_AGE,
};
typedef enum _ReadContactStatus ReadContactStatus;

ReadContactStatus
read_contact(char* buf, size_t n, IncompleteContact* out, FILE* f);

ReadContactStatus
deserialize_contact(char* buf, IncompleteContact* out);
