#include "superColumn.h"


inline int superColumnComp(superColumn *x, superColumn *y)
{
	POINTER_NOT_NULL(x);
	POINTER_NOT_NULL(y);
	return strcmp(x->name, y->name);
}

RB_PROTOTYPE(superColumnSet, superColumn, link, superColumnComp); 
RB_GENERATE(superColumnSet, superColumn, link, superColumnComp);

