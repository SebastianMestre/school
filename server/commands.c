#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <stdio.h>

#define ATTEMPTS 10

static int try_alloc(struct kv_store* store, size_t size, void** ptr) {
	int attempts;
	for (attempts = 0; attempts < ATTEMPTS; attempts++) {
		*ptr = malloc(size);
		if (*ptr != NULL) break;
		if (kv_store_evict(store) < 0) return -1;
	}
	if (attempts == ATTEMPTS) return -1;
	return 0; 
}

enum cmd_output run_text_command(struct kv_store* store, struct text_command* cmd) {
	switch (cmd->tag) {
		case PUT: {
			char* key;
			if (try_alloc(store, cmd->key_len, (void**)&key) < 0) {
				return CMD_EOOM;
			}
			memcpy(key, cmd->key, cmd->key_len);
			char* val;
			if (try_alloc(store, cmd->val_len, (void**)&val) < 0) {
				free(key);
				return CMD_EOOM;
			}
			memcpy(val, cmd->val, cmd->val_len);
			int attempts, res;
			for (attempts = 0; attempts < ATTEMPTS; attempts++) {
				res = kv_store_put(store, key, cmd->key_len, val, cmd->val_len);
				if (res != KV_STORE_OOM) break;
				if (kv_store_evict(store) < 0) {
					free(key);
					free(val);
					return CMD_EOOM;
				}
			}
			if (res == KV_STORE_OK) return CMD_OK;
			free(val);
			free(key);
			if (res == KV_STORE_OOM) return CMD_EOOM;
			return CMD_EUNK;
		}
		case DEL: {
			int res = kv_store_del(store, cmd->key, cmd->key_len);
			if (res == KV_STORE_OK) return CMD_OK; 
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			return CMD_EUNK;
		}
		case GET: {
			char* val;
			uint32_t val_len;
			int res = kv_store_get(store, cmd->key, cmd->key_len, &val, &val_len);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == KV_STORE_OOM) return CMD_EOOM;
			
			/** opcion 1: chequeamos que el valor no sea binario
			if (res == KV_STORE_BIN) {
				free(val);
				return CMD_BIN;
			}
			*/
			if (res == CMD_OK) {
				assert(val != NULL);
				if (val_len > WORD_SIZE) {
					free(val);
					return CMD_EBIG;
				}
				/** opcion 2:
					chequeamos que el valor sea imprmible 
				if (!imprimible(val, val_len)) {
					free(val);
					return CMD_EBIN; 
				}
				*/
				memcpy(cmd->val, val, val_len);
				free(val);
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case TAKE: {
			char* val;
			uint32_t val_len;
			int res = kv_store_take(store, cmd->key, cmd->key_len, &val, &val_len);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			
			/** opcion 1: chequeamos que el valor no sea binario
			if (res == KV_STORE_BIN) {
				free(val);
				return CMD_BIN;
			}
			*/
			if (res == CMD_OK) {
				assert(val != NULL);
				if (val_len > WORD_SIZE) {
					free(val);
					return CMD_EBIG;
				}
				/** opcion 2:
					chequeamos que el valor sea imprmible 
				if (!imprimible(val, val_len)) {
					free(val);
					return CMD_EBIN; 
				}
				*/
				memcpy(cmd->val, val, val_len);
				free(val);
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case STATS: {
			// ni idea que se supone que pase aca jaja
			int stats;
			kv_store_stat(store, &stats);
			// TODO
			fprintf(stderr, "stats: no implementado\n");
		} break;
		default:
			assert(0);
	}
}

static void reset_biny_command(struct biny_command* cmd) {
	cmd->key_size = cmd->val_size = 0;
	cmd->key = cmd->val = NULL;
}

static void free_biny_command(struct biny_command* cmd) {
	if (cmd->key_size > 0) free(cmd->key);
	if (cmd->val_size > 0) free(cmd->val);
	reset_biny_command(cmd);
}

enum cmd_output run_biny_command(struct kv_store* store, struct biny_command* cmd) {
	switch (cmd->tag) {
		case PUT: {
			int attempts, res;
			for (attempts = 0; attempts < ATTEMPTS; attempts++) {
				res = kv_store_put(store, cmd->key, cmd->key_len, cmd->val,
									cmd->val_len);
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
			int res = kv_store_del(store, cmd->key, cmd->key_len);
			free_biny_command(cmd);
			if (res == KV_STORE_OK) return CMD_OK; 
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			return CMD_EUNK;
		}
		case GET: {
			uint8_t* val;
			uint32_t val_len;
			int res = kv_store_get(store, cmd->key, cmd->key_len,
									(char**)val, &val_len);
			free_biny_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == KV_STORE_OOM) return CMD_EOOM;
			if (res == CMD_OK) {
				assert(cmd->val != NULL);
				cmd->val = val;
				cmd->val_size = cmd->val_len = val_len;
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case TAKE: {
			uint8_t* val;
			uint32_t val_len;
			int res = kv_store_take(store, cmd->key, cmd->key_len,
									(char**)&val, &val_len);
			free_biny_command(cmd);
			if (res == KV_STORE_NOTFOUND) return CMD_ENOTFOUND;
			if (res == CMD_OK) {
				assert(cmd->val != NULL);
				cmd->val = val;
				cmd->val_len = cmd->val_size = val_len;	
				return CMD_OK;    
			}
		} return CMD_EUNK;
		case STATS: {
			// ni idea que se supone que pase aca jaja
			int stats;
			kv_store_stat(store, &stats);
			// TODO
			fprintf(stderr, "stats: no implementado\n");
		} break;
		default:
			assert(0);
	}
}