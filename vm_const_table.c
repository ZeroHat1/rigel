#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm_const_table.h"
#include "vm_value.h"

static int const_table_expand(ConstTable *table)
{
	size_t new_capacity = table->capacity << 1; // similarly to table->capacity * 2

	void *new_data = realloc(table->data, new_capacity * sizeof(TValue));

	if (!new_data)
	{
		fprintf(stderr, "Memory allocation error\n");
        exit(1);
	}

	table->data = new_data;
	table->capacity = new_capacity;

	return 0;
}
	
int const_table_init(ConstTable *table)
{
	table->capacity = CONST_TABLE_BASE_CAPACITY;
	table->sp = -1;

	TValue *data = malloc(CONST_TABLE_BASE_CAPACITY * sizeof(TValue));

	if (!data)
	{
		return -1;
	}

	table->data = data;

	return 0;
}

int const_table_push(ConstTable *table, TValue data)
{	
	if (table->sp+1 >= table->capacity)
	{
		const_table_expand(table);
	}

	table->data[++table->sp] = data;

    return 0;
}

TValue const_table_pop(ConstTable *table)
{	
	TValue top_value = const_table_get(table, table->sp);

	table->sp--;

    return top_value;
}

TValue const_table_get(ConstTable *table, size_t index)
{
	return table->data[index];
}

TValue const_table_peek(ConstTable *table)
{
	return table->data[table->sp];
}

void const_table_free(ConstTable *table)
{
	free(table->data);
	table->data = NULL;
	table->capacity = 0;
	table->sp = -1;
}

ConstTable const_table_new()
{
	ConstTable table;
	const_table_init(&table);

	return table;
}
