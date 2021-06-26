#pragma once

#include "contact.h"

#include "circular_buffer.h"

typedef struct _Storage Storage;

struct _HistoryAction {
	OptionalContactId forwards;
	OptionalContactId backwards;
};
typedef struct _HistoryAction HistoryAction;

struct _History {
	Storage* storage;
	CircularBuffer actions;
	void* next_action;
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

// devuelve un puntero a la siguiente accion que de debe realizar
// el puntero se invalida al llamar cualquier otra funcion de history
HistoryAction*
history_next_action(History* history);

// hace que el cursor de la historia apunte al siguiente evento
void history_advance_cursor(History* history);
