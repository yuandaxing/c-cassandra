#ifndef _SUPERCOLUMN_H_
#define _SUPERCOLUMN_H_
#include "column.h"
#define MAX(x, y)	((x) > (y) ? (x) : (y))

typedef struct superColumn{
	char *name;
	unsigned int markedForDelete;
	unsigned int localDelete;
	int columnCount;
	struct columnSet columns;
	RB_ENTRY(superColumn) link;
}superColumn;


typedef RB_HEAD(superColumnSet,superColumn) superColumnSet; 

#endif
