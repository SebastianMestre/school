#pragma once

#include "contact.h"

#include "circular_buffer.h"

struct _History {
	CircularBuffer actions;
};
typedef struct _History History;

typedef struct _Contacts Contacts;

History
history_create(Contacts* contacts);
