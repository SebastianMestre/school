#pragma once

#include "contact.h"

#include "circular_buffer.h"

struct _History {
	CircularBuffer actions;
};
typedef struct _History History;

typedef struct _Storage Storage;

History
history_create(Storage* storage);
