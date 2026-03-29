#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vm_value.h"

#define GROUP_SIZE 16

#define SLOT_EMPTY   0xFF
#define SLOT_DELETED 0x80
#define SLOT_TAKEN   0b10000000 // 1xxxxxxx - free, 0xxxxxxx - taken

typedef struct Entry
{
    TValue key; 
    TValue value;
} Entry;

typedef struct Table 
{
    uint8_t *ctrl;
    Entry   *slots;
    size_t   capacity;
    size_t   size;
    size_t   num_groups;
    size_t   num_deleted;
} Table;

int table_init(Table *t);

int table_init_with_groups(Table *t, size_t num_groups); // i don't recommend to use

int table_set(Table *t, TValue key, TValue value);

TValue table_get(Table *t, TValue key);

int table_delete(Table *t, TValue key);

void table_free(Table *t);