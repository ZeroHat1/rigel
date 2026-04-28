#pragma once

#include <stddef.h>
#include <stdint.h>

#include "table.h"
#include "vm_const_table.h"
#include "vm_locals.h"
#include "vm_stack.h"
#include "vm_frame.h"

typedef struct
{
	VM_Stack *stack;
	ConstTable *const_table;
	Table *globals;
	VM_Locals *locals;
	FrameStack *frame_stack;
} VM;

static inline void vm_init(VM *vm, VM_Stack *stack, ConstTable *const_table, FrameStack *frame_stack, Table *globals, VM_Stack *locals)
{
	vm->stack = stack;
	vm->const_table = const_table;
	vm->frame_stack = frame_stack;
}