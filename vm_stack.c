#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm_stack.h"
#include "vm_value.h"

int stack_expand(VM_Stack *stack)
{
	size_t new_capacity = stack->capacity << 1; // similarly to stack->capacity * 2

	void *new_data = realloc(stack->data, new_capacity * sizeof(TValue));

	if (!new_data)
	{
		fprintf(stderr, "Memory allocation error\n");
        exit(1);
	}

	stack->data = new_data;
	stack->capacity = new_capacity;

	return 0;
}
	
int stack_init(VM_Stack *stack)
{
	stack->capacity = STACK_BASE_CAPACITY;
	stack->sp = -1;

	TValue *data = malloc(STACK_BASE_CAPACITY * sizeof(TValue));

	if (!data)
	{
		return -1;
	}

	stack->data = data;

	return 0;
}

int stack_push(VM_Stack *stack, TValue data)
{	
	if (++stack->sp >= stack->capacity)
	{
		stack_expand(stack);
	}

	stack->data[stack->sp] = data;

    return 0;
}

int stack_set(VM_Stack *stack, TValue data, size_t index)
{	
	if (index > stack->sp)
	{
		stack->sp = index;

		if (index >= stack->capacity)
		{
			stack_expand(stack);
		}
	}

	stack->data[index] = data;

    return 0;
}

TValue stack_pop(VM_Stack *stack)
{	
	TValue top_value = stack_get(stack, stack->sp--);

    return top_value;
}

TValue stack_get(VM_Stack *stack, size_t index)
{
	return stack->data[index];
}

TValue stack_peek(VM_Stack *stack)
{
	return stack->data[stack->sp];
}

void stack_free(VM_Stack *stack)
{
	free(stack->data);
	stack->data = NULL;
	stack->capacity = 0;
	stack->sp = -1;
}

VM_Stack stack_new()
{
	VM_Stack stack;
	stack_init(&stack);

	return stack;
}
