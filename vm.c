#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm_frame.h"
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

	TValue val = new_integer(20); // 0
	TValue new_val = new_integer(10); // 1

	// TValue str = new_pointer(new_string("b"));
	// TValue str1 = new_pointer(new_string("ba"));
	// val.as.floating = 11.3;

	const_table_push(&const_table, val);
	const_table_push(&const_table, new_val);

	// globals_set(&globals, str, val);
	printf("from const_table: %f\n", const_table_peek(vm.const_table).as.floating);

	Bytecode *bytecode = malloc(16 * sizeof(Bytecode));
	bytecode[0] = LOAD_CONST;
	bytecode[1] = 0;
	bytecode[2] = LOAD_CONST;
	bytecode[3] = 1;
	bytecode[4] = BINARY_DIV;
	bytecode[5] = PRINT;
	bytecode[6] = HALT;

	Bytecode *func_bc = malloc(3 * sizeof(Bytecode));
	func_bc[0] = LOAD_CONST;
	func_bc[1] = 1;
	func_bc[2] = HALT;

	frame_stack_push(vm.frame_stack, bytecode);
	// frame_stack_push(vm.frame_stack, func_bc);
	// frame_stack_pop(vm.frame_stack);

	// printf("is correct: %hhu\n", frame_stack_peek(vm.frame_stack)->bytecode[1]);

	// TValue func_name = new_pointer(new_string("func"));

	// ObjFunction *func = new_function(func_bc, 0, AS_STRING(func_name));

	// printf("function name: %s also function name: %s\n", func->name->chars, AS_STRING(func_name)->chars);

	vm_exec(&vm);

	printf("from stack: %f\n", stack_peek(vm.stack).as.floating);

	// printf("on stack: %f\n", stack_pop(vm.stack).as.floating);

	return 0;
}