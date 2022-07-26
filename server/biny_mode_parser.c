#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "biny_mode_parser.h"

#define MIN(a, b) (a) > (b) ? (b) : (a)
// constantes
#define N_CMDS 5
#define PUT_ID 11
#define DEL_ID 12 
#define GET_ID 13
#define TAKE_ID 14
#define STATS_ID 21

static enum status parse_val(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	assert(cmd->val);
	
	uint32_t nbytes = cmd->val_size - cmd->val_len;
	if (nbytes == 0) return OK;

	uint32_t nbuffer = end - *start;
	if (nbuffer == 0) return INCOMPLETE;

	uint32_t ndata = MIN(nbuffer, nbytes);
	memcpy(cmd->val + cmd->val_len, *start, ndata);
	*start += ndata;
	cmd->val_len += ndata;
	
	if (cmd->val_len < cmd->val_size) return INCOMPLETE;

	return OK;
}
// no modifica `start` si no hay suficientes datos
static enum status parse_val_size(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	if (end - *start < 4) return INCOMPLETE;
	uint32_t* val_size = (uint32_t*)*start;
	cmd->val_size = ntohl(*val_size);
	*start += 4;
	return OK; 
}

static enum status parse_key(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	assert(cmd->key);
	
	uint32_t nbytes = cmd->key_size - cmd->key_len;
	if (nbytes == 0) return OK;

	uint32_t nbuffer = end - *start;
	if (nbuffer == 0) return INCOMPLETE;

	uint32_t ndata = MIN(nbuffer, nbytes);
	memcpy(cmd->key + cmd->key_len, *start, ndata);
	*start += ndata;
	cmd->key_len += ndata;
	
	if (cmd->key_len < cmd->key_size) return INCOMPLETE;

	return OK;
}
// no modifica `start` si no hay suficientes datos
static enum status parse_key_size(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	if (end - *start < 4) return INCOMPLETE;
	uint32_t* key_size = (uint32_t*)*start;
	cmd->key_size = ntohl(*key_size);
	*start += 4;
	return OK; 
}
// no modifica `start` si no hay suficientes datos
static enum status parse_pfx_by_id(uint8_t id, uint8_t** start, uint8_t* end) {
	if (end - *start <= 0) return INCOMPLETE;
	if (**start == id) {
		*start += 1;
		return OK;
	}
	return MISMATCH;
}

static enum status parse_pfx(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	enum status status;
	for (enum cmd_tag tag = 0; tag < N_CMDS; tag++) {
		switch (tag) {
			case PUT:
				status = parse_pfx_by_id(PUT_ID, start, end);
				break;
			case DEL:
				status = parse_pfx_by_id(DEL_ID, start, end);
				break;
			case GET:
				status = parse_pfx_by_id(GET_ID, start, end);
				break;
			case TAKE:
				status = parse_pfx_by_id(TAKE_ID, start, end);
				break;
			case STATS:
				status = parse_pfx_by_id(STATS_ID, start, end);
				break;
			default:
				assert(0);
			}
		if (status == INCOMPLETE) return INCOMPLETE;
		if (status == OK) {
			cmd->tag = tag;
			return OK;
		}	 
	}
	
	return INVALID;
}

// para comenzar a parsear, el comando debe estar inicializado en 0
// el parser detecta que falta parsear y continua a partir de ahi
enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	enum status status;
	
	// tenemos que parsear el prefijo
	if (cmd->key_size == 0) {
		status = parse_pfx(start, end, cmd);
		if (status != OK) return status; 
	}

	if (cmd->tag == STATS) return OK;

	// tenemos que parsear el largo de la clave
	if (cmd->key_size == 0) {
		status = parse_key_size(start, end, cmd);
		if (status != OK) return status;
		return KEY_NEXT;
	}

	// tenemos que parsear la clave
	if (cmd->key_len < cmd->key_size) {
		status = parse_key(start, end, cmd);
		if (status == INCOMPLETE) return INCOMPLETE;
		assert(status == OK);
	}

	if (cmd->tag != PUT) return OK;

	// tenemos que parsear el largo del valor
	if (cmd->val_size == 0) {
		status = parse_val_size(start, end, cmd);
		if (status != OK) return status;
		return VAL_NEXT;
	}

	// tenemos que parsear el valor
	if (cmd->val_len < cmd->val_size) {
		return parse_val(start, end, cmd);
	}

	return OK;
} 