#pragma once
#include <stdint.h>


// OK: 			comando parseado correctamente
// INVALID:	 	linea invalida
// MISMATCH: 	(para parsear un comando) no hay match en el prefijo
// INCOMPLETE: 	faltan datos
// KEY_NEXT: 	reservar memoria para `key` y parsear
// VAL_NEXT: 	reservar memoria para `val` y parsear 
enum status { OK, INVALID, MISMATCH, INCOMPLETE, KEY_NEXT, VAL_NEXT };
enum cmd_tag { PUT, DEL, GET, TAKE, STATS };
struct biny_command {
	enum cmd_tag tag;
	uint8_t* key;
	uint32_t key_size;
	uint32_t key_len;
	uint8_t* val;
	uint32_t val_size;
	uint32_t val_len;   
};


enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd);
