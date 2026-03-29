#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm_frame.h"
#include "vm_types.h"

int frame_stack_expand(FrameStack *stack)
{
	size_t new_capacity = stack->capacity << 1; // similarly to stack->capacity * 2

	void *new_data = realloc(stack->frame, new_capacity * sizeof(Frame));

	if (!new_data)
	{
		fprintf(stderr, "Memory allocation error\n");
        return 1;
	}

	stack->frame = new_data;
	stack->capacity = new_capacity;

	return 0;
}
	
int frame_stack_init(FrameStack *stack)
{
	stack->capacity = FRAME_STACK_BASE_CAPACITY;
	stack->sp = -1;

	Frame *data = malloc(FRAME_STACK_BASE_CAPACITY * sizeof(Frame));

	if (!data)
	{
		return -1;
	}

	stack->frame = data;

	return 0;
}

int frame_stack_push(FrameStack *stack, Bytecode *bytecode)
{	
	if (stack->sp+1 >= stack->capacity)
	{
		if (frame_stack_expand(stack))
		{
			return 1;
		}
		
	}

	stack->frame[++stack->sp].bytecode = bytecode;
	stack->frame[stack->sp].ip = 0;

    return 0;
}

Frame* frame_stack_pop(FrameStack *stack)
{	
	Frame *top_Frame = frame_stack_get(stack, stack->sp);

	stack->sp--;

    return top_Frame;
}

Frame* frame_stack_get(FrameStack *stack, size_t index)
{
	return &stack->frame[index];
}

Frame* frame_stack_peek(FrameStack *stack)
{
	return &stack->frame[stack->sp];
}

void frame_stack_free(FrameStack *stack)
{
	free(stack->frame);
	stack->frame = NULL;
	stack->capacity = 0;
	stack->sp = -1;
}