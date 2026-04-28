#pragma once

#include <stdio.h>
#include <stdlib.h>

static inline void raise_exception()
{
	printf("nothing but error");

	exit(1);
}