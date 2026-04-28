#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "vm_value.h"
#include "raise.h"

#define locals_BASE_CAPACITY 256

typedef struct
{
	TValue *data;
	size_t sp;
	size_t capacity;
} VM_Locals;

static inline int locals_expand(VM_Locals *locals)
{
	size_t new_capacity = locals->capacity << 1; // similarly to locals->capacity * 2

	void *new_data = realloc(locals->data, new_capacity * sizeof(TValue));

	if (!new_data)
	{
		raise_exception();
        exit(1);
	}

	locals->data = new_data;
	locals->capacity = new_capacity;

	return 0;
}
	
static inline int locals_init(VM_Locals *locals)
{
	locals->capacity = locals_BASE_CAPACITY;
	locals->sp = -1;

	TValue *data = malloc(locals_BASE_CAPACITY * sizeof(TValue));

	if (!data)
	{
		return -1;
	}

	locals->data = data;

	return 0;
}

static inline int locals_push(VM_Locals *locals, TValue data)
{	
	if (++locals->sp >= locals->capacity)
	{
		locals_expand(locals);
	}

	locals->data[locals->sp] = data;

    return 0;
}

static inline int locals_set(VM_Locals *locals, TValue data, size_t index)
{	
	if (index > locals->sp)
	{
		locals->sp = index;

		if (index >= locals->capacity)
		{
			locals_expand(locals);
		}
	}

	locals->data[index] = data;

    return 0;
}

static inline TValue locals_get(VM_Locals *locals, size_t index)
{
	return locals->data[index];
}

static inline TValue locals_pop(VM_Locals *locals)
{	
	TValue top_value = locals_get(locals, locals->sp--);

    return top_value;
}

static inline TValue locals_peek(VM_Locals *locals)
{
	return locals->data[locals->sp];
}

static inline void locals_free(VM_Locals *locals)
{
	free(locals->data);
	locals->data = NULL;
	locals->capacity = 0;
	locals->sp = -1;
}

static inline VM_Locals locals_new()
{
	VM_Locals locals;
	locals_init(&locals);

	return locals;
}
