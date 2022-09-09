#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "try_alloc.h"

#define OK 			101
#define EINVAL 		111
#define ENOTFOUND 	112
#define EBINARY		113
#define EBIG		114
#define EUNK		115
#define EOOM		116 // codigo nuevo

// TODO hace falta esto?
static void reset_text_command(struct text_command* cmd) {
	cmd->key_len = cmd->val_len = 0;
}

#define ALLOC_ATTEMPTS 10

// retorna el tamaño del resultado
// si val no es NULL, almacena las stats en val
static int stats(kv_store* store, char* val) {
	struct kv_store_stat stats;
	kv_store_stat(store, &stats);
	
	int val_len;
	const char* format = "PUTS=%lu DELS=%lu GETS=%lu TAKES=%lu KEYS=%lu"; 
	if (val == NULL) {
		val_len = snprintf(val, 0, format, 	stats.put_count, 
											stats.del_count, 
											stats.get_count, 
											stats.take_count, 
											stats.key_count);	
	}
	else {
		val_len = sprintf(val, format, 	stats.put_count, 
										stats.del_count, 
										stats.get_count, 
										stats.take_count, 
										stats.key_count);
	}
	return val_len;
}

static int imprimible(char* str, int len) {
	for (int i = 0; i < len; i++) {
		if (!isprint(str[i])) return 0;	
	}
	return 1;
}

enum cmd_output run_text_command(kv_store* store, struct text_command* cmd) {
	switch (cmd->tag) {
		case PUT: {

			unsigned char* key = try_alloc(store, cmd->key_len);
			if (key == NULL) {
				return CMD_EOOM;
			}
			memcpy(key, cmd->key, cmd->key_len);

			unsigned char* val = try_alloc(store, cmd->val_len);
			if (val == NULL) {
				free(key);
				return CMD_EOOM;
			}
			memcpy(val, cmd->val, cmd->val_len);

			int res = kv_store_put(store, key, cmd->key_len, val, cmd->val_len);
			reset_text_command(cmd);
			if (res != KV_STORE_OK) {
				free(val);
				free(key);
				return (res == KV_STORE_OOM) ? CMD_EOOM : CMD_EUNK;
			}

			return CMD_OK;
		}
		case DEL: {
			int res = kv_store_del(store, cmd->key, cmd->key_len);
			reset_text_command(cmd);
			if (res == KV_STORE_OK) return CMD_OK; 
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			return CMD_EUNK;
		}
		case GET: {
			unsigned char* val;
			size_t val_len;
			int res = kv_store_get(store, cmd->key, cmd->key_len, &val, &val_len);
			reset_text_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == KV_STORE_OOM) return CMD_EOOM;
			
			if (res == CMD_OK) {
				assert(val != NULL);
				if (val_len > WORD_SIZE) {
					free(val);
					return CMD_EBIG;
				}
				if (!imprimible(val, val_len)) {
					free(val);
					return CMD_EBINARY; 
				}
				memcpy(cmd->val, val, val_len);
				cmd->val_len = val_len;
				free(val);
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case TAKE: {
			unsigned char* val;
			size_t val_len;
			int res = kv_store_take(store, cmd->key, cmd->key_len, &val, &val_len);
			reset_text_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			
			if (res == CMD_OK) {
				assert(val != NULL);
				if (val_len > WORD_SIZE) {
					free(val);
					return CMD_EBIG;
				}
				if (!imprimible(val, val_len)) {
					free(val);
					return CMD_EBINARY; 
				}
				memcpy(cmd->val, val, val_len);
				cmd->val_len = val_len;
				free(val);
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case STATS: {
			// no estamos chequeando que alcance el tamaño del buffer
			// deberia alcanzar...
			cmd->val_len = stats(store, cmd->val);
			return CMD_OK;
		} break;
		default:
			assert(0);
	}
}

// TODO hace falta esto?
static void reset_biny_command(struct biny_command* cmd) {
	cmd->key_size = cmd->val_size = 0;
	cmd->key = cmd->val = NULL;
}

static void free_biny_command(struct biny_command* cmd) {
	if (cmd->key_size > 0) free(cmd->key);
	if (cmd->val_size > 0) free(cmd->val);
	reset_biny_command(cmd);
}

enum cmd_output run_biny_command(kv_store* store, struct biny_command* cmd) {
	switch (cmd->tag) {
		case PUT: {
			int attempts, res;
			for (attempts = 0; attempts < ALLOC_ATTEMPTS; attempts++) {
				res = kv_store_put(store, cmd->key, cmd->key_size, cmd->val, cmd->val_size);
				if (res != KV_STORE_OOM) break;
				if (kv_store_evict(store) < 0) {
					free_biny_command(cmd);
					return CMD_EOOM;
				}
			}
			if (res == KV_STORE_OK) {
				reset_biny_command(cmd);
				return CMD_OK;
			}
			free_biny_command(cmd);
			if (res == KV_STORE_OOM) return CMD_EOOM;
			return CMD_EUNK;
		}
		case DEL: {
			int res = kv_store_del(store, cmd->key, cmd->key_size);
			free_biny_command(cmd);
			if (res == KV_STORE_OK) return CMD_OK; 
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			return CMD_EUNK;
		}
		case GET: {
			uint8_t* val;
			size_t val_size;
			int res = kv_store_get(store, cmd->key, cmd->key_size, &val, &val_size);
			free_biny_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == KV_STORE_OOM) return CMD_EOOM;
			if (res == CMD_OK) {
				assert(val != NULL);
				cmd->val = val;
				cmd->val_size = val_size;
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case TAKE: {
			uint8_t* val;
			size_t val_size;
			int res = kv_store_take(store, cmd->key, cmd->key_size, &val, &val_size);
			free_biny_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == CMD_OK) {
				assert(val != NULL);
				cmd->val = val;
				cmd->val_size = val_size;
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case STATS: {
			size_t size = stats(store, NULL);
			cmd->val = try_alloc(store, size);
			if (cmd->val == NULL) return CMD_EOOM;
			cmd->val_size = stats(store, cmd->val);
			return CMD_OK;
		} break;
		default:
			assert(0);
	}
}

const char* cmd_output_name(enum cmd_output output, int* len) {
	const char* res;
	switch (output) {
		case CMD_OK: 		
			res = "OK";
			break;
		case CMD_EINVAL: 	
			res = "EINVAL";
			break;
		case CMD_ENOTFOUND: 
			res = "ENOTFOUND";
			break;
		case CMD_EBINARY: 	
			res = "EBINARY";
			break;
		case CMD_EBIG: 		
			res = "EBIG";
			break;
		case CMD_EUNK: 		
			res = "EUNK";
			break;
		case CMD_EOOM: 		
			res = "EOOM";
			break;
		default:
			assert(0);
	}
	
	*len = strlen(res);
	return res;
}

uint8_t cmd_output_code(enum cmd_output output) {
	switch (output) {
		case CMD_OK: 		return OK;
		case CMD_EINVAL: 	return EINVAL; 
		case CMD_ENOTFOUND: return ENOTFOUND;
		case CMD_EBINARY: 	return EBINARY;
		case CMD_EBIG: 		return EBIG;
		case CMD_EUNK: 		return EUNK;
		case CMD_EOOM: 		return EOOM;
		default:			assert(0);
	}
}
