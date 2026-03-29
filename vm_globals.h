#pragma once

#include "vm_value.h"

typedef struct Table Table;

int globals_init(Table *globals);

TValue globals_get(Table *globals, TValue key);

int globals_set(Table *globals, TValue key, TValue value);