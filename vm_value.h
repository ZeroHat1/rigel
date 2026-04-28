#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "vm_heap_obj.h"

typedef enum
{
	VAL_NIL,
	VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_POINTER
} ValueType;

typedef struct 
{
    ValueType type;
    union 
    {
        int64_t integer;
        double floating;
        bool boolean;
        void* pointer;
    } as;
} TValue;

static inline TValue new_value(ValueType type) // don't use with a pointer type
{
    TValue val;
    val.type = type;

    return val;
}

static inline TValue new_integer(int64_t integer)
{
    TValue val;
    val.type = VAL_INT;
    val.as.integer = integer;

    return val;
}

static inline TValue new_floating(double floating)
{
    TValue val;
    val.type = VAL_FLOAT;
    val.as.floating = floating;

    return val;
}

static inline TValue new_boolean(bool boolean)
{
    TValue val;
    val.type = VAL_BOOL;
    val.as.boolean = boolean;

    return val;
}

static inline TValue new_nil(bool boolean)
{
    TValue val;
    val.type = VAL_NIL;

    return val;
}

static inline TValue new_pointer(void *pointer)
{
    TValue val;
    val.type = VAL_POINTER;
    val.as.pointer = pointer;

    AS_OBJ(val)->refcount++;

    return val;
}
