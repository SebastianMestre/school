#include <stdio.h>
#include <string.h>

#include "text_mode_parser.h"


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

void print_cmd(struct text_command cmd) {
	print_cmd_tag(cmd.tag);
	if (cmd.tag == STATS) return;
	for (int i = 0; i < cmd.key_len; i++)
		putchar(cmd.key[i]);
	putchar(' ');
	if (cmd.tag != PUT) return;
	for (int i = 0; i < cmd.val_len; i++)
		putchar(cmd.val[i]);
	putchar(' '); 
}

int main() {
	char* buf = "TAKE K\nPUT K V\n";
	int buf_len = strlen(buf);

	char* cursor = buf;
	struct text_command cmd;
	switch (parse_text_command(&cursor, cursor + buf_len, &cmd)) {
		case INVALID:
			printf("INVALID\n");
			break;
		case INCOMPLETE:
			printf("INCOMPLETE\n");
			break;
		case PARSED:
			print_cmd(cmd);
			printf("\nOK\n");
      break;
	}
}