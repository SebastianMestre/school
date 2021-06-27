#pragma once

#include "contact.h"

#include "circular_buffer.h"

typedef struct _Storage Storage;

struct _HistoryEvent {
	OptionalContactId forwards;
	OptionalContactId backwards;
};
typedef struct _HistoryEvent HistoryEvent;

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

// elimina todos los eventos de la historia
void
history_clear(History* history);

// elimina todos los eventos futuros de la historia
void
history_clear_future(History* history);

// guarda un evento indicando que se inserto un contacto
void
history_record_inserted(History* history, ContactId id);

// guarda un evento indicando que se elimino un contacto
void
history_record_deleted(History* history, ContactId id);

// guarda un evento indicando que se actualizo un contacto
void
history_record_updated(History* history, ContactId old_id, ContactId new_id);

// devuelve un puntero al evento que le sigue al cursor.
// el puntero se invalida al llamar cualquier otra funcion de history
HistoryEvent*
history_next_action(History* history);

// hace que el cursor de la historia apunte al siguiente evento
void
history_advance_cursor(History* history);

// hace que el cursor de la historia apunte al evento anterior
void
history_retreat_cursor(History* history);

// devuelve true sii el cursor esta al principio de la historia
bool
history_cursor_at_begin(History const* history);

// devuelve true sii el cursor esta al final de la historia
bool
history_cursor_at_end(History const* history);
