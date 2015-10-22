#include "Network.h"


namespace Network
{
	int SocketClose(int fd) 
	{
#if defined( WIN32 )
		int ret = closesocket(fd);
#else
		int ret = close(fd);
#endif
		return ret;
	}

	int SocketBind(const char* host,int port,int protocol) 
	{
		int fd;
		int status;
		int reuse = 1;
		struct addrinfo aiHints;
		struct addrinfo* aiList = NULL;

		char portStr[16];
		if (host == NULL)
			host = "0.0.0.0";	// INADDR_ANY
		
		sprintf(portStr, "%d", port);

		memset( &aiHints, 0, sizeof( aiHints ) );
		aiHints.ai_family = AF_UNSPEC;
		if (protocol == IPPROTO_TCP)
			aiHints.ai_socktype = SOCK_STREAM;
		else 
		{
			assert(protocol == IPPROTO_UDP);
			aiHints.ai_socktype = SOCK_DGRAM;
		}
		aiHints.ai_protocol = protocol;

		status = getaddrinfo( host, portStr, &aiHints, &aiList );
		if ( status != 0 )
			return -1;
		
		int family = aiList->ai_family;
		fd = socket(family, aiList->ai_socktype, 0);
		if (fd < 0) {
			goto _failed_fd;
		}
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int))==-1)
			goto _failed;
		
		status = bind(fd, (struct sockaddr *)aiList->ai_addr, aiList->ai_addrlen);
		if (status != 0)
			goto _failed;

		freeaddrinfo( aiList );
		return fd;
_failed:
		SocketClose(fd);
_failed_fd:
		freeaddrinfo( aiList );
		return -1;
	}

	int SocketListen(int fd,int backlog) 
	{
		if (listen(fd, backlog) == -1) 
		{
			SocketClose(fd);
			return -1;
		}
		return 0;
	}

	int SocketConnect(const char* host,int port,bool block,bool& connected)
	{
		struct addrinfo ai_hints;
		struct addrinfo *ai_list = NULL;
		struct addrinfo *ai_ptr = NULL;

		char portstr[16];
		sprintf(portstr, "%d", port);
		memset(&ai_hints, 0, sizeof( ai_hints ) );

		ai_hints.ai_family = AF_UNSPEC;
		ai_hints.ai_socktype = SOCK_STREAM;
		ai_hints.ai_protocol = IPPROTO_TCP;

		int fd = -1;

		int status = getaddrinfo(host, portstr, &ai_hints, &ai_list );
		if ( status != 0 ) 
			goto _failed;

		for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next ) 
		{
			fd = socket( ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol );
			if ( fd < 0 )
				continue;

			if (!block) 
			{
				SocketSetKeepalive(fd,true);
				SocketSetNonblocking(fd,true);
			}
			status = connect( fd, ai_ptr->ai_addr, ai_ptr->ai_addrlen);

#if defined( WIN32 )

			int err = WSAGetLastError();
			if ( status != 0 && err != WSAEWOULDBLOCK)
			{
#else
			if ( status != 0 && errno != EINPROGRESS)
			{
#endif
				SocketClose(fd);
				fd = -1;
				continue;
			}
			break;
		}

		if (fd < 0)
			goto _failed;


		if(status == 0)
		{
			connected = true;
			return fd;
		}
		else 
		{
			connected = false;
			return fd;
		}

_failed:
		freeaddrinfo( ai_list );
		return -1;
	}

	int SocketSetKeepalive(int fd ,bool keepalive) 
	{
#if defined( WIN32 )
		bool val;
#else
		int val;
#endif
		val = keepalive ? 1 : 0;
		return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
	}

	int SocketSetNodelay(int fd,bool nodelay) 
	{
		int arg = int(nodelay);
		return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&arg, sizeof(int));
	}

	int SocketSetNonblocking(int fd,bool nonblocking) 
	{
#if defined( WIN32 )
		u_long val = nonblocking ? 1 : 0;
		return ioctlsocket(fd, FIONBIO, &val);
#else
		int val = nonblocking ? O_NONBLOCK : 0;
		return fcntl(fd, F_SETFL, val);
#endif
	}

	int SocketRead(int fd,char* data,int size) 
	{
#if defined( WIN32 )
		int n = recv(fd,data,size,0);
#else
		int n =(int)read(fd, data, size);
#endif
		if (n < 0) 
		{
#if defined( WIN32 )
			int error =  WSAGetLastError();
			switch(error)
			{
			case WSAEINTR:
				break;
			case WSAEWOULDBLOCK:
				break;
			default:
				return ReadError;
				break;
			}
#else
			switch(errno)
			{
			case EINTR:
				break;
			case EAGAIN:
				break;
			default:
				return ReadError;
				break;
			}
#endif
		}

		if (n == 0)
			return ReadError;

		return n > 0 ? n:0;
	}

	int SocketWrite(int fd,const char* data,int size) 
	{
		int total = 0;
		for (;;) {
#if defined( WIN32 )
			int sz = send(fd,data,size,0);
#else
			int sz =(int)write(fd, data, size);
#endif
			if (sz < 0) 
			{
#if defined( WIN32 )
				int error =  WSAGetLastError();
				switch(error)
				{
				case WSAEINTR:
					continue;
				case WSAEWOULDBLOCK:
					return total;
				default:
					fprintf(stderr,"send fd :%d error\n",fd);
					return WriteError;
				}
#else
				switch(errno)
				{
				case EINTR:
					continue;
				case EAGAIN:
					return total;
				default:
					fprintf(stderr,"send fd :%d error\n",fd);
					return WriteError;
				}
#endif
			}
			else if (sz == 0)
			{
				return WriteError;
			}
			else
			{
				size -= sz;
				data += sz;
				total += sz;
				if (0 == size)
					break;
			}
		}

		return total;
	}
}





