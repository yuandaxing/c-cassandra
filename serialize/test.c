#include "stdio.h"
#include "string.h"
#include "time.h"
#include "serialize.h"


int main()
{
	char *str = "what is this doing\n";
	char  k = 'a';

	buffer *b = getBuffer();
	printf("return %d\n", writeByte(k, b));
	char v;
	reset(b);
	printf("return %d\n", readByte(b, &v));
	printf("writeInt %c readInt %c <F8>\n", k, v);
	reset(b);
	writeBytes(str, b);
	reset(b);
	char *ss;
	readBytes(b, &ss);
	printf("writeInt %s readInt %s\n", str,ss);
/*	printf("sizeof(int):%d\n", sizeof(int));
	printf("sizeof(size_t):%d\n", sizeof(size_t));
	printf("sizeof(time_t):%d\n", sizeof(time_t));
	printf("sizeof(long):%d\n", sizeof(long));
	printf("sizeof(ssize_t):%d\n", sizeof(ssize_t));
	printf("sizeof(off_t):%d\n", sizeof(off_t));
	memcpy(buff, str, strlen(str));
	printf("%s", buff);*/

	return 0;
}
