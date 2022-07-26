#pragma once

#include <stdint.h>


// OK: comando parseado correctamente
// INVALID: linea invalida
// INCOMPLETE: linea no terminada; puede ser correcta
// MISMATCH: (para parsear un comando) no hay match en el prefijo 
enum status { OK, INVALID, INCOMPLETE, MISMATCH, KEY_NEXT, VAL_NEXT };
enum cmd_tag { PUT, DEL, GET, TAKE, STATS };

#define WORD_SIZE 2048

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
	uint32_t key_len;
	uint8_t* val;
	uint32_t val_size;
	uint32_t val_len;   
};

// command_output run_biny_command()
// command_output run_text_command()