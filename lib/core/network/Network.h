#ifndef NETWORK_H
#define NETWORK_H

#include <string.h>
#include <assert.h>
#include <stdio.h>

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

#include <WS2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#endif

namespace Network
{
	enum SocketError
	{
		ReadError = -1,
		WriteError = -2
	};
	int SocketClose(int fd);
	int SocketBind(const char* host,int port,int protocol);
	int SocketListen(int fd,int backlog);
	int SocketConnect(const char* host,int port,bool block,bool& connected);
	int SocketRead(int fd,char* data,int size);
	int SocketWrite(int fd,const char* data,int size);
	int SocketSetKeepalive(int fd ,bool keepalive);
	int SocketSetNodelay(int fd,bool nodelay);
	int SocketSetNonblocking(int fd,bool nonblocking);
	bool SocketHasError(int fd);
}


#endif