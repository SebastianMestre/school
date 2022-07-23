#pragma once

#define WORD_SIZE 2048

// consideramos el estado INCOMPLETE aunque realmente no es necesario
// MAYBE quitarlo

// OK: comando parseado correctamente
// INVALID: linea invalida
// INCOMPLETE: linea no terminada; puede ser correcta
// MISMATCH: (para parsear un comando) no hay match en el prefijo 
enum status { OK, INVALID, INCOMPLETE, MISMATCH };
enum cmd_tag { PUT, DEL, GET, TAKE, STATS };
struct cmd {
	enum cmd_tag tag;
	char key[WORD_SIZE];
	int key_len;
	char val[WORD_SIZE];
	int val_len;   
};


enum status parse_command(char** start, char* end, struct cmd* cmd);
