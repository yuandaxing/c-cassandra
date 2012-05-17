#include "auxiRBtree.h"
#include "stdio.h"


struct Int{
	int i;
	RB_ENTRY(Int) link;
};
int cmp(struct Int *x, struct Int *y)
{
	return x->i - y->i;
}

RB_TREE_GENERATE(Iops_type, Iops, Int, head, head, link, cmp); 

int main()
{
	printf("%p", &Iops.head);
	return 0;
}
