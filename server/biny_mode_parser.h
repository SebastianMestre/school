#pragma once
#include <stdint.h>

#include "commands.h"

enum status parse_biny_command(uint8_t** start, uint8_t* end, struct biny_command* cmd);
