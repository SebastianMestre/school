#include <assert.h>
#include <string.h>

#include "biny_mode_parser.h"

#define MIN(a, b) (a) > (b) ? (b) : (a)
// constantes
#define N_CMDS 5
#define PUT_ID 11
#define DEL_ID 12 
#define GET_ID 13
#define TAKE_ID 14
#define STATS_ID 15

static enum status parse_val(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	uint32_t nbytes = cmd->val_size - cmd->val_len;
	if (nbytes == 0) return OK;

	uint32_t nbuffer = end - *start;
	if (nbuffer == 0) return INCOMPLETE;

	uint32_t ndata = MIN(nbuffer, nbytes);
	memcpy(cmd->val + val_len, *start, ndata);
	*start += ndata;
	cmd->val_len += ndata;
	
	if (cmd->val_len < cmd->val_size) return INCOMPLETE;

	return OK;
}

static enum status parse_val_size(uint8_t** start, uint8_t* end, struct biniy_command* cmd) {
	if (end - *start < 4) return INCOMPLETE;
	cmd->val_size = ntohl((uint32_t)*start);
	*start += 4;
	return OK; 
}

static enum status parse_key(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	uint32_t nbytes = cmd->key_size - cmd->key_len;
	if (nbytes == 0) return OK;

	uint32_t nbuffer = end - *start;
	if (nbuffer == 0) return INCOMPLETE;

	uint32_t ndata = MIN(nbuffer, nbytes);
	memcpy(cmd->key + key_len, *start, ndata);
	*start += ndata;
	cmd->key_len += ndata;
	
	if (cmd->key_len < cmd->key_size) return INCOMPLETE;

	return OK;
}

static enum status parse_key_size(uint8_t** start, uint8_t* end, struct biniy_command* cmd) {
	if (end - *start < 4) return INCOMPLETE;
	cmd->key_size = ntohl((uint32_t)*start);
	*start += 4;
	return OK; 
}

static enum status parse_pfx_by_id(uint8_t id, uint8_t** start, uint8_t* end) {
	if (end - *start <= 0) return INCOMPLETE;
	if (**start == id) {
		start += 1;
		return OK;
	}
	return MISMATCH;
}

static enum status parse_pfx(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	uint8_t* out_start;
	enum status status;
	for (enum cmd_tag tag = 0; tag < N_CMDS; tag++) {
		switch (tag) {
			case PUT:
				status = parse_pfx_by_id(PUT_ID, &out_start, end);
				break;
			case DEL:
				status = parse_pfx_by_id(DEL_ID, &out_start, end);
				break;
			case GET:
				status = parse_pfx_by_id(GET_ID, &out_start, end);
				break;
			case TAKE:
				status = parse_pfx_by_id(TAKE_ID, &out_start, end);
				break;
			case STATS:
				status = parse_pfx_by_id(STATS_ID, &out_start, end);
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


enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	uint8_t* out_start = *start;
	enum status status;
	// tenemos que parsear el prefijo
	if (cmd->key_size == 0) {
		status = parse_pfx(&out_start, end, cmd);
		if (status != OK) return status;
		if (cmd->tag == STATS) {
			*start = out_start;
			return OK;
		}
		status = parse_key_size(&out_start, end, cmd);
		if (status != OK) return status;
		*start = out_start;
		return KEY_NEXT; 
	}
	// tenemos que parsear la clave
	if (cmd->key_len < cmd->key_size) {
		status = parse_key(start, end, cmd);
		if (status == INCOMPLETE) return INCOMPLETE;
		if (cmd->tag != PUT) return OK;
		out_start = *start;
		status = parse_val_size(&out_start, end, cmd);
		if (status != OK) return status;
		*start = out_start;
		return VAL_NEXT;
	}
	// no hay que parsear el valor
	if (cmd->val_size == 0) return OK;
	// tenemos que parsear el valor
	if (cmd->val_len < cmd->val_size) return parse_val(start, end, cmd);

	return OK;
} 