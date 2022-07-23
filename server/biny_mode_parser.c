#include "biny_mode_parser.h"

#define PUT_ID 11
#define DEL_ID 12 
#define GET_ID 13
#define TAKE_ID 14
#define STATS_ID 15

enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd) {
	return INVALID;
}