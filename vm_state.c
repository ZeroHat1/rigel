#include <stddef.h>

#include "vm_state.h"
#include "vm_const_table.h"
#include "vm_frame.h"
#include "vm_stack.h"

void vm_init(VM *vm, VM_Stack *stack, ConstTable *const_table, FrameStack *frame_stack, Table *globals, VM_Stack *locals)
{
	vm->stack = stack;
	vm->const_table = const_table;
	vm->frame_stack = frame_stack;
}