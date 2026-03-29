#include "vm_globals.h"
#include "table.h"

int globals_init(Table *globals)
{
	return table_init(globals);
}

TValue globals_get(Table *globals, TValue key)
{
	return table_get(globals, key);
}

int globals_set(Table *globals, TValue key, TValue value)
{
	return table_set(globals, key, value);
}
