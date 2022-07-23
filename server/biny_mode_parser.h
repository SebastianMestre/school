#pragma once
#include <stdint.h>

#define CHUNK_SIZE 2048


// OK: comando parseado correctamente
// INVALID: linea invalida
// INCOMPLETE: linea no terminada; puede ser correcta
// MISMATCH: (para parsear un comando) no hay match en el prefijo 
enum status { OK, INVALID, INCOMPLETE, MISMATCH };
enum cmd_tag { PUT, DEL, GET, TAKE, STATS };
struct biny_command {
	enum cmd_tag tag;
	uint8_t key[CHUNK_SIZE];
	int key_len;
	uint8_t val[CHUNK_SIZE];
	int val_len;   
};


enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd);
