#pragma once

#include "contact.h"
#include "vector.h"

#include <stdint.h>

struct _ContactSetSlot {
	Contact data;
	uint8_t masks;
};

#define CSET_EMPTY (0x1)
#define CSET_REFERENCED_IN_AVL (0x2)
#define CSET_REFERENCED_IN_HISTORY_CREATE (0x4)
#define CSET_REFERENCED_IN_HISTORY_DELETE (0x8)

struct _ContactSet {
	Vector holes;
	Vector slots;
};

