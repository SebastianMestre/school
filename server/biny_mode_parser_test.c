#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "biny_mode_parser.h"

void print_cmd_tag(enum cmd_tag tag) {
	switch (tag) {
		case PUT:
			printf("PUT ");
			break;
		case DEL:
			printf("DEL ");
			break;
		case GET:
			printf("GET ");
			break;
		case TAKE:
			printf("TAKE ");
			break;
		case STATS:
			printf("STATS ");
			break;
	}
}

void print_cmd(struct biny_command cmd) {
	print_cmd_tag(cmd.tag);
	printf("key_size: %u val_size: %u  ", cmd.key_size, cmd.val_size);
	for (uint32_t i = 0; i < cmd.key_len; i++)
		printf("%u ", cmd.key[i]);
	for (uint32_t i = 0; i < cmd.val_len; i++)
		printf("%u ", cmd.val[i]);
	puts("");
}

void main() {
	uint8_t buf[17] = {0};
	uint8_t* start = buf; 
	uint8_t* end = start + 17;

	buf[0] = 11; 	// tag
	buf[4] = 4;		// key_size
	buf[12] = 4;	// val_size
	// random data
	buf[5] = 1;	
	buf[13] = 1;

	struct biny_command cmd = {0};
	enum status status = parse_biny_command(&start, end, &cmd);

	switch (status) {
		case INVALID:
			puts("tag: INVALID");
			return;
		case INCOMPLETE:
			puts("tag: INCOMPLETE");
			return;
		case OK:
			puts("tag: OK");
			break;
		case KEY_NEXT:
			break;
		default:
			puts("tag: ERROR");
			return;
	}

	if (status == KEY_NEXT) {
		cmd.key = malloc(sizeof(*cmd.key)*cmd.key_size);
		assert(cmd.key);
		status = parse_biny_command(&start, end, &cmd);
		switch (status) {
			case INVALID:
				puts("key: INVALID");
				free(cmd.key);
				return;
			case INCOMPLETE:
				puts("key: INCOMPLETE");
				free(cmd.key);
				return;
			case OK:
				puts("key: OK");
				break;
			case VAL_NEXT:
				break;
			default:
				puts("key: ERROR");
				free(cmd.val);
				return;
		}
	}
	if (status == VAL_NEXT) {
		cmd.val = malloc(sizeof(*cmd.val)*cmd.val_size);
		assert(cmd.val);
		status = parse_biny_command(&start, end, &cmd);
		switch(status) {
			case OK:
				puts("val: OK");
				break;
			default:
				puts("val: ERROR");
				free(cmd.key);
				free(cmd.val);
				return;
		}
	}

	print_cmd(cmd);
	if (cmd.key_size > 0) free(cmd.key);
	if (cmd.val_size > 0) free(cmd.val);
	
	return;
}