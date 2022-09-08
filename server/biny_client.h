#pragma once
#include <stdint.h>

#include "kv_store.h"
#include "message_action.h"

struct biny_client_state* create_biny_client_state(kv_store* store);

enum message_action handle_biny_message(struct biny_client_state* state, int sock, int events, kv_store* store);
