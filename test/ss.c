#include "stdio.h"
#include "string.h"
#include "stdlib.h"


int compare(void *a, void *b)
{
	char **a1 = (char **)a;
	char **b1 = (char **)b;
	return strcmp(*a1, *b1);
}
int main()
{
	char *a[] = {"test1",
				"test4", 
				"test2",
				"test10"
	};

	qsort(a, 4, sizeof(void *),  compare);
	int i;

	for(i = 0; i < 4; i++){
		printf("%s\n", a[i]);
	}
	return 0;
}
