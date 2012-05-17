#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_
#ifdef	__cplusplus
extern "C" {
#endif

int socket_init();
int new_udpsocket(char *node, int port);

#ifdef __cplusplus
}
#endif
#endif
