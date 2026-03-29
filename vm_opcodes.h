#pragma once

#include "vm_state.h"

typedef enum
{
	LOAD_CONST = 0,
	LOAD_TRUE,
	LOAD_FALSE,
	LOAD_NIL,
	END
} Instruction_Name;

void vm_exec(VM *vm);