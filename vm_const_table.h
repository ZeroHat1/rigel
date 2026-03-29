#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "vm_value.h"

#define CONST_TABLE_BASE_CAPACITY 16

typedef struct
{
	TValue *data;
	size_t sp;
	size_t capacity;
} ConstTable;

int const_table_init(ConstTable *table);

int const_table_push(ConstTable *table, TValue data);

TValue const_table_pop(ConstTable *table);

TValue const_table_get(ConstTable *table, size_t index);

TValue const_table_peek(ConstTable *table);

void const_table_free(ConstTable *table);

ConstTable const_table_new();
