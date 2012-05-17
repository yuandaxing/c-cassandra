#include "stdlib.h"
#include "stdio.h"

inline int bbbbbbbbb(int k)
{
	printf("hello world, %d\n", k);
	bbbbbbbbb(k);
	return  0;
}
static int ccccccccc(int k)
{
	printf("cc %d\n", k);
	return k;
}
int aaaaaaaaaaa(int k)
{
	printf("this is %d\n", k);
	ccccccccc(k);
	return bbbbbbbbb(k);
}
int dddddddd(int k)
{
	int a = 1, b = 1;
	return a+b+k;
}
int main(int argc, char **argv)
{
	int k;
	k = dddddddd(argc);
	return aaaaaaaaaaa(k);
}
