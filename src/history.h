#pragma once

#include "contact.h"

#include "circular_buffer.h"

struct _HistoryAction {
	ContactId forwards;
	ContactId backwards;
};
typedef struct _HistoryAction HistoryAction;

struct _History {
	CircularBuffer actions;
};
typedef struct _History History;

#define HISTORY_SIZE (40)

History
history_create() {
	return (History){
		.actions = circular_buffer_create(sizeof(ContactId), HISTORY_SIZE, nop_dtor)
	};
}
