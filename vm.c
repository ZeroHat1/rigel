#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_value.h"
#include "vm_const_table.h"
#include "vm_state.h"
#include "vm_stack.h"
#include "vm_globals.h"
#include "vm_types.h"
#include "vm_opcodes.h"
#include "vm_heap_obj.h"

int main()
{
	VM vm;
	VM_Stack stack;
	ConstTable const_table;
	Table globals;
	FrameStack frame_stack;
	VM_Stack locals;

	frame_stack_init(&frame_stack);
	globals_init(&globals);
	stack_init(&stack);
	stack_init(&locals);
	const_table_init(&const_table);

	vm_init(&vm, &stack, &const_table, &frame_stack, &globals, &locals);

	TValue val = new_value(VAL_FLOAT);
	val.as.floating = 11.3;

	const_table_push(&const_table, val);

	// globals_set(&globals, "b", val);
	// printf("from globals: %f\n", globals_get(&globals, "b").as.floating);

	Bytecode *bytecode = malloc(3 * sizeof(Bytecode));
	bytecode[0] = LOAD_CONST;
	bytecode[1] = 0;
	bytecode[2] = END;

	frame_stack_push(vm.frame_stack, bytecode);

	vm_exec(&vm);

	printf("%f\n", stack_pop(vm.stack).as.floating);
	printf("bc: %hhu\n", frame_stack_peek(vm.frame_stack)->bytecode[2]);

	char *func_name = malloc(strlen("func") + 1);
	strcpy(func_name, "func");

	printf("%s\n", func_name);

	Bytecode *func_bc = malloc(3 * sizeof(Bytecode));
	func_bc[0] = 190;

	ObjFunction *func = new_function(func_bc, 0, func_name);

	printf("%s %s\n", func->name->chars, func_name);

	return 0;
}
