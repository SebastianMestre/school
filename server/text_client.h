/**
 * Este modulo maneja la interaccion con el cliente en modo texto.
 * Se ocupa de interpretar los mensajes recibidos, procesarlos y responder al cliente.
 * Determina si ocurrio un error (potencialmente una falta del cliente) y responde
 * adecuadamente (actualmente en caso de error terminamos la conexion con el cliente).
 *
 * Referirse al informe para ver manejo de exceso de tamaño del input (EBIG). 
 */
#pragma once

#include "kv_store.h"
#include "message_action.h"

// Crea estructura de estado del cliente.
// -- intenta alocar memoria en el heap --
struct text_client_state* create_text_client_state(kv_store* store);
// Libera info del cliente.
void free_text_client_state(struct text_client_state* state);
// Manejamos mensaje recibido en el socket del cliente:
//	parseamos
//	corremos el comando
//	respondemos al cliente
enum message_action handle_text_message(struct text_client_state* state, int sock, int events, kv_store* store);

