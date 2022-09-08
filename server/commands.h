#pragma once

#include <stdint.h>

#include "kv_store.h"

// PARSED: comando parseado correctamente
// INVALID: linea invalida
// INCOMPLETE: linea no terminada; puede ser correcta
// MISMATCH: (para parsear un comando) no hay match en el prefijo
// KEY_NEXT: parsear la clave (reservar memoria si es necesario)
// KEY_VAL: parsear el valor (reservar memoria si es necesario)
enum cmd_tag { PUT, DEL, GET, TAKE, STATS };

#define WORD_SIZE 2048

// MAYBE usar uint32_t para `key_len` y `key_size`
struct text_command {
	enum cmd_tag tag;
	char key[WORD_SIZE];
	int key_len;
	char val[WORD_SIZE];
	int val_len;   
};

struct biny_command {
	enum cmd_tag tag;

	uint8_t* key;
	uint32_t key_size;

	// En un PUT es un argumento. En un GET o un TAKE es un resultado.
	uint8_t* val;
	uint32_t val_size;
};

enum cmd_output { 
	CMD_OK, 
	CMD_EINVAL,
	CMD_ENOTFOUND, 
	CMD_EBINARY, 
	CMD_EBIG, 
	CMD_EUNK, 
	CMD_EOOM 
};
// Toman un comando y lo corren sobre `store`.
// Resultado en `cmd.val`.
enum cmd_output run_text_command(kv_store* store, struct text_command* cmd);
// Toma ownership sobre `cmd.key` (y `cmd.value`).
enum cmd_output run_biny_command(kv_store* store, struct biny_command* cmd);

const char* cmd_output_name(enum cmd_output output, int* len);

uint8_t cmd_output_code(enum cmd_output output);
