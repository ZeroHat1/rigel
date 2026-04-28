#include "raise.h"
#include "vm_frame.h"
#include "vm_stack.h"
#include "vm_value.h"
#include "vm_const_table.h"
#include "vm_state.h"
#include "vm_opcodes.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

static inline void print_impl(VM *vm)
{
	TValue val = stack_pop(vm->stack);

	if (val.type == VAL_INT)
	{
		printf("%ld\n", val.as.integer);
	}
	else if (val.type == VAL_FLOAT)
	{
		printf("%f\n", val.as.floating);
	}
	else if (val.type == VAL_BOOL) 
	{
		if (val.as.boolean == 0)
		{
			printf("false\n");
		}
		else
		{
			printf("true\n");
		}
	}
	else if (val.type == VAL_NIL) 
	{
		printf("nil\n");
	}
	else if (val.type == VAL_POINTER)
	{
		printf("pointer\n");
	}

	inc_ip(vm);
}

static inline void binary_add_impl(VM *vm)
{
	TValue right_operand = stack_pop(vm->stack);
	TValue left_operand = stack_pop(vm->stack);

	if (right_operand.type == VAL_POINTER || left_operand.type == VAL_POINTER)
	{
		puts("unfinished");
		raise_exception();
	}
	else if (right_operand.type == VAL_NIL || left_operand.type == VAL_NIL)
	{
		raise_exception();
	}
	else if (right_operand.type == VAL_BOOL || left_operand.type == VAL_BOOL)
	{
		raise_exception();
	}
	
	if (right_operand.type == VAL_INT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_integer(right_operand.as.integer + left_operand.as.integer));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating + left_operand.as.floating));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating + left_operand.as.integer));
	}
	else if (right_operand.type == VAL_INT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.integer + left_operand.as.floating));
	}

	inc_ip(vm);
}

static inline void binary_sub_impl(VM *vm)
{
	TValue right_operand = stack_pop(vm->stack);
	TValue left_operand = stack_pop(vm->stack);

	if (right_operand.type == VAL_POINTER || left_operand.type == VAL_POINTER)
	{
		puts("unfinished");
		raise_exception();
	}
	else if (right_operand.type == VAL_NIL || left_operand.type == VAL_NIL)
	{
		raise_exception();
	}
	else if (right_operand.type == VAL_BOOL || left_operand.type == VAL_BOOL)
	{
		raise_exception();
	}
	
	if (right_operand.type == VAL_INT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_integer(right_operand.as.integer - left_operand.as.integer));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating - left_operand.as.floating));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating - left_operand.as.integer));
	}
	else if (right_operand.type == VAL_INT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.integer - left_operand.as.floating));
	}

	inc_ip(vm);
}

static inline void binary_mul_impl(VM *vm)
{
	TValue right_operand = stack_pop(vm->stack);
	TValue left_operand = stack_pop(vm->stack);

	if (right_operand.type == VAL_POINTER || left_operand.type == VAL_POINTER)
	{
		puts("unfinished");
		raise_exception();
	}
	else if (right_operand.type == VAL_NIL || left_operand.type == VAL_NIL)
	{
		raise_exception();
	}
	else if (right_operand.type == VAL_BOOL || left_operand.type == VAL_BOOL)
	{
		raise_exception();
	}
	
	if (right_operand.type == VAL_INT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_integer(right_operand.as.integer * left_operand.as.integer));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating * left_operand.as.floating));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating * left_operand.as.integer));
	}
	else if (right_operand.type == VAL_INT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.integer * left_operand.as.floating));
	}

	inc_ip(vm);
}

static inline void binary_div_impl(VM *vm)
{
	TValue right_operand = stack_pop(vm->stack);
	TValue left_operand = stack_pop(vm->stack);

	if (right_operand.type == VAL_POINTER || left_operand.type == VAL_POINTER)
	{
		puts("unfinished");
		raise_exception();
	}
	else if (right_operand.type == VAL_NIL || left_operand.type == VAL_NIL)
	{
		raise_exception();
	}
	else if (right_operand.type == VAL_BOOL || left_operand.type == VAL_BOOL)
	{
		raise_exception();
	}
	
	if (right_operand.type == VAL_INT && left_operand.type == VAL_INT)
	{
		if (right_operand.as.integer % left_operand.as.integer == 0)
		{
			stack_push(vm->stack, new_integer(right_operand.as.integer / left_operand.as.integer));
		}
		else
		{
			stack_push(vm->stack, new_floating((double)right_operand.as.integer / left_operand.as.integer));
		}
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating / left_operand.as.floating));
	}
	else if (right_operand.type == VAL_FLOAT && left_operand.type == VAL_INT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.floating / left_operand.as.integer));
	}
	else if (right_operand.type == VAL_INT && left_operand.type == VAL_FLOAT)
	{
		stack_push(vm->stack, new_floating(right_operand.as.integer / left_operand.as.floating));
	}

	inc_ip(vm);
}

#define dispatch() goto *dispatch_table[get_bytecode(vm)[get_ip(vm)]]

void vm_exec(VM *vm)
{
	static void *dispatch_table[] = 
	{
    	[LOAD_CONST] = &&load_const,
    	[LOAD_TRUE] = &&load_true,
    	[LOAD_FALSE] = &&load_false,
    	[LOAD_NIL] = &&load_nil,
    	[PRINT] = &&print,
    	[BINARY_ADD] = &&add,
    	[BINARY_SUB] = &&sub,
    	[BINARY_MUL] = &&mul,
    	[BINARY_DIV] = &&div,
    	[HALT] = &&halt
	};

	dispatch();

	load_const:
		load_const_impl(vm);
		dispatch();
	load_true:
		load_true_impl(vm);
		dispatch();
	load_false:
		load_false_impl(vm);
		dispatch();
	load_nil:
		load_nil_impl(vm);
		dispatch();
	print:
		print_impl(vm);
		dispatch();
	add:
		binary_add_impl(vm);
		dispatch();
	sub:
		binary_sub_impl(vm);
		dispatch();
	mul:
		binary_mul_impl(vm);
		dispatch();
	div:
		binary_div_impl(vm);
		dispatch();
	halt:
		return;
}