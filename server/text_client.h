#pragma once

#include "kv_store.h"
#include "message_action.h"

struct text_client_state* create_text_client_state(kv_store* store);

enum message_action handle_text_message(struct text_client_state* state, int sock, int events, kv_store* store);

void free_text_client_state(struct text_client_state* state);
