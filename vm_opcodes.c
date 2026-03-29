#include "vm_frame.h"
#include "vm_stack.h"
#include "vm_value.h"
#include "vm_const_table.h"
#include "vm_state.h"
#include "vm_opcodes.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define get_ip(VM) (frame_stack_peek(VM->frame_stack)->ip)
#define inc_ip(VM) (frame_stack_peek(VM->frame_stack)->ip++)

#define get_bytecode(VM) (frame_stack_peek(VM->frame_stack)->bytecode)

static inline void load_const_impl(VM *vm)
{
	inc_ip(vm);
	uint8_t adress = get_ip(vm);

	TValue value = const_table_get(vm->const_table, get_bytecode(vm)[adress]);

	stack_push(vm->stack, value);

	inc_ip(vm);
}

static inline void load_true_impl(VM *vm)
{
	TValue value;
	value.type = VAL_BOOL;
	value.as.boolean = true;

	stack_push(vm->stack, value);
	inc_ip(vm);
}

static inline void load_false_impl(VM *vm)
{
	TValue value;
	value.type = VAL_BOOL;
	value.as.boolean = false;

	stack_push(vm->stack, value);
	inc_ip(vm);
}

static inline void load_nil_impl(VM *vm)
{
	TValue value;
	value.type = VAL_NIL;

	stack_push(vm->stack, value);
	inc_ip(vm);
}

#define dispatch(VM) goto *dispatch_table[get_bytecode(vm)[get_ip(vm)]]

void vm_exec(VM *vm)
{
	static void *dispatch_table[] = 
	{
    	[LOAD_CONST] = &&load_const,
    	[LOAD_TRUE] = &&load_true,
    	[LOAD_FALSE] = &&load_false,
    	[LOAD_NIL] = &&load_nil,
    	[END] = &&end
	};

	dispatch();

	load_const:
		load_const_impl(vm);
		dispatch(vm);
	load_true:
		load_true_impl(vm);
		dispatch(vm);
	load_false:
		load_false_impl(vm);
		dispatch(vm);
	load_nil:
		load_nil_impl(vm);
		dispatch(vm);
	end:
		return;
}