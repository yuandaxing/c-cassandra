/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-07-25 16:20
*
* Filename:udpsocket.c
*
* Description: 
*
=============================================================================*/
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <event.h>
#include "slist.h"
#include "log.h"


queue_t  shead;
int socket_init()
{
	initialize_queue(&shead);
	return 	set_log("log.txt");	
}
static void read_process(int fd, short which, void *arg)
{
	char buff[200];
	int ret;
	struct sockaddr request;
	socklen_t request_size;

	memset(buff, 0, sizeof(buff));
    ret = recvfrom(fd, buff, sizeof(buff),
                   0, &request, &request_size);
	printf("here\n");
	if(ret > 0){
		printf("%s\n", buff);
		LOG_WRITE("%s\n", buff);
	}

}

#define MAX_SENDBUF_SIZE  1024*1024*256
 /*
  * Sets a socket's send buffer size to the maximum allowed by the system.
  */
static void maximize_sndbuf(const int sfd) 
{
	socklen_t intsize = sizeof(int);
	int last_good = 0;
	int min, max, avg;
	int old_size;

	/* Start with the default size. */
	if (getsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &old_size, &intsize) != 0) {
		LOG_WRITE("get buff size error\n");
		return;
	}

	/* Binary-search for the real maximum. */
	min = old_size;
	max = MAX_SENDBUF_SIZE;

	while (min <= max) {
		avg = ((unsigned int)(min + max)) / 2;
		if (setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, (void *)&avg, intsize) == 0) {
			last_good = avg;
			min = avg + 1;
		} else {
			max = avg - 1;
		}
	}

	LOG_WRITE("buff size is %d\n", last_good);
}


static int new_socket(struct addrinfo *ai) 
{
	int sfd;
	int flags;

	if ((sfd = socket(ai->ai_family, ai->ai_socktype, 
								ai->ai_protocol)) == -1) {
		perror("socket()");
		return -1;
	}

	if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
		fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		perror("setting O_NONBLOCK");
		close(sfd);
		return -1;
	}
	return sfd;
}


int new_udpsocket(char *node, int port)
{
	struct addrinfo *ai;
	struct addrinfo *next;
	struct addrinfo hints;
	char port_buf[10];
	int flag = 1, error;
	int success = 0;

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = AI_PASSIVE|AI_ADDRCONFIG;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET; 


	snprintf(port_buf, sizeof(port_buf), "%d", port);

	error= getaddrinfo(node, port_buf, &hints, &ai);

	if(error != 0){
		LOG_WRITE("get address error\n");
		return -1;
	}

	for(next = ai; next; next = next->ai_next){
		
		int fd;
		if((fd = new_socket(ai)) < 0){
			printf("new socke error\n");
			freeaddrinfo(ai);
			return -1;
		}


        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&flag, 
												sizeof(flag)) < 0){
			LOG_WRITE("setsockopt error\n");
			freeaddrinfo(ai);
			close(fd);
			return -1;
		}

		if (bind(fd, next->ai_addr, next->ai_addrlen) == -1) {
			LOG_WRITE("bind error\n");
			//freeaddrinfo(ai);
			close(fd);
			continue;
		}
		maximize_sndbuf(fd);

		node_t *n = get_node();
		if(n == NULL){
			LOG_WRITE("not enough memory\n");
			freeaddrinfo(ai);
			close(fd);
			return -1;
		}

		memset(&n->e, 0, sizeof(n->e));
		event_set(&n->e, fd, EV_READ | EV_PERSIST,  read_process, 
													(void *)n);

		if(event_add(&n->e, 0) < 0){
			LOG_WRITE("cannot add event\n");
			free(n);
			close(fd);
			continue;
		}	

		push_into_queue(n, &shead);
		success++;
	}	
	freeaddrinfo(ai);
	return success > 0 ? 0 : -1;

}
