#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "vm_value.h"

#define STACK_BASE_CAPACITY 256

typedef struct
{
	TValue *data;
	size_t sp;
	size_t capacity;
} VM_Stack;

int stack_expand(VM_Stack *stack);

int stack_init(VM_Stack *stack);

int stack_push(VM_Stack *stack, TValue data);

TValue stack_pop(VM_Stack *stack);

TValue stack_get(VM_Stack *stack, size_t index);

TValue stack_peek(VM_Stack *stack);

void stack_free(VM_Stack *stack);

VM_Stack stack_new();
