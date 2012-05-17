#include "stdio.h"

#define MALLOC_EXIT_NULL(pointer, size, type)	\
	if(!(pointer = (type)malloc(size))){	\
		LOG_WRITE("OUT OF MEMORY\n");	\
		exit(0);	\
	}
