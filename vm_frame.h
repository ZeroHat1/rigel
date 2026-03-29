#pragma once

#include <stddef.h>

#include "vm_types.h"

#define FRAME_STACK_BASE_CAPACITY 256

typedef struct
{
	size_t ip;
	Bytecode *bytecode;

	size_t *fp; // frame pointer
} Frame;

typedef struct
{
	Frame *frame;
	size_t sp;
	size_t capacity;
} FrameStack;

int frame_stack_expand(FrameStack *stack);
	
int frame_stack_init(FrameStack *stack);

int frame_stack_push(FrameStack *stack, Bytecode *bytecode);

Frame* frame_stack_pop(FrameStack *stack);

Frame* frame_stack_get(FrameStack *stack, size_t index);

Frame* frame_stack_peek(FrameStack *stack);

void frame_stack_free(FrameStack *stack);

static inline FrameStack frame_stack_new()
{
	FrameStack stack;
	frame_stack_init(&stack);

	return stack;
}


