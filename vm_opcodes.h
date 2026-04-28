#pragma once

#include "vm_state.h"

typedef enum
{
	LOAD_CONST = 0,
	LOAD_TRUE,
	LOAD_FALSE,
	LOAD_NIL,
	PRINT,
	BINARY_ADD,
	BINARY_SUB,
	BINARY_MUL,
	BINARY_DIV,
	HALT
} Instruction_Name;

void vm_exec(VM *vm);