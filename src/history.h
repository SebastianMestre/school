#pragma once

#include "contact.h"

#include "circular_buffer.h"

typedef struct _Storage Storage;

struct _History {
	Storage* storage;
	CircularBuffer actions;
};
typedef struct _History History;

History
history_create(Storage* storage);

void
history_release(History* history);

void
history_record_inserted(History* history, ContactId id);

void
history_record_deleted(History* history, ContactId id);

void
history_record_updated(History* history, ContactId old_id, ContactId new_id);
