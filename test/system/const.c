#include "stdio.h"

int main()
{
	char **p = NULL;
	const char **cp = NULL;
	const char *p1 = NULL;
	char *p2 = NULL;
	cp = p;
	cp = &p1;
	p1 = p2;
	return 0;
}
