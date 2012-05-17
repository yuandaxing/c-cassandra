#include "udpsocket.h"
#include <event.h>
//./server 115.156.232.16
int main(int argc, char **argv)
{
	event_init();
	socket_init();
	if(argc != 2){
		printf("you must address the listen node ");
	}
	new_udpsocket(argv[1], 3000);

	event_dispatch();
	return 0;
}
