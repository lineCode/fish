#include "Network.h"


namespace Network {
	int SocketClose(int fd) {
#if defined( WIN32 )
		int ret = closesocket(fd);
#else
		int ret = close(fd);
#endif
		return ret;
	}

	int SocketAccept(int lfd, Addr* addr) {
		socklen_t len = sizeof(addr->sockaddr);
		int fd = accept(lfd, addr->Address(), &len);
		if (fd < 0) {
			return -1;
		}
		addr->SetAddrLen(len);
		return fd;
	}

	int SocketBind(Addr& addr) {
		//IPPROTO_TCP
		int fd = socket(addr.Family(), SOCK_STREAM, 0);
		if (fd < 0) {
			return -1;
		}

		int reuse = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int))==-1) {
			SocketClose(fd);
			return -1;
		}
		
		int status = bind(fd, addr.Address(),addr.AddrLen());
		if (status != 0) {
			SocketClose(fd);
			return -1;
		}

		return fd;
	}

	int SocketListen(int fd,int backlog) {
		if (listen(fd, backlog) == -1) {
			SocketClose(fd);
			return -1;
		}
		return 0;
	}

	int SocketConnect(Addr& addr, bool nonblock, bool& connected) { 
		int fd = socket(addr.Family(), SOCK_STREAM, IPPROTO_TCP);
		if (fd < 0) {
			return -1;
		}
		if (nonblock) {
			SocketSetKeepalive(fd,true);
			SocketSetNonblocking(fd,true);
		}
		
		int status = connect(fd, addr.Address(), addr.AddrLen());

		if (!nonblock) {
			if (status != 0) {
				SocketClose(fd);
				connected = false;
				return -1;
			} else {
				connected = true;
				SocketSetNonblocking(fd,true);
				return fd;
			}
		}

#if defined( WIN32 )

		int err = WSAGetLastError();
		if ( status != 0 && err != WSAEWOULDBLOCK) {
#else
		if ( status != 0 && errno != EINPROGRESS) {
#endif
			SocketClose(fd);
			return -1;
		}

		if(status == 0) {
			connected = true;
		} else {
			connected = false;
		}
		return fd;
	}

	int SocketSetKeepalive(int fd ,bool keepalive) {
#if defined( WIN32 )
		bool val;
#else
		int val;
#endif
		val = keepalive ? 1 : 0;
		return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
	}

	int SocketSetNodelay(int fd,bool nodelay) {
		int arg = int(nodelay);
		return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&arg, sizeof(int));
	}

	int SocketSetNonblocking(int fd,bool nonblocking) {
#if defined( WIN32 )
		u_long val = nonblocking ? 1 : 0;
		return ioctlsocket(fd, FIONBIO, &val);
#else
		int val = nonblocking ? O_NONBLOCK : 0;
		return fcntl(fd, F_SETFL, val);
#endif
	}

	int SocketRead(int fd,char* data,int size) {
#if defined( WIN32 )
		int n = recv(fd,data,size,0);
#else
		int n =(int)read(fd, data, size);
#endif
		if (n == 0) {
			return ReadError;
		} else if (n < 0) {
#if defined( WIN32 )
			int error =  WSAGetLastError();
			switch(error) {
			case WSAEINTR:
				break;
			case WSAEWOULDBLOCK:
				break;
			default:
				return ReadError;
				break;
			}
#else
			switch(errno) {
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
		return n > 0 ? n : 0;
	}

	int SocketWrite(int fd,const char* data,int size) {
		int total = 0;
		for (;;) {
#if defined( WIN32 )
			int sz = send(fd,data,size,0);
#else
			int sz =(int)write(fd, data, size);
#endif
			if (sz < 0) {
#if defined( WIN32 )
				int error =  WSAGetLastError();
				switch(error) {
				case WSAEINTR:
					continue;
				case WSAEWOULDBLOCK:
					return total;
				default:
					fprintf(stderr,"send fd :%d error\n",fd);
					return WriteError;
				}
#else
				switch(errno) {
				case EINTR:
					continue;
				case EAGAIN:
					return total;
				default:
					fprintf(stderr,"send fd :%d error\n",fd);
					return WriteError;
				}
#endif
			} else if (sz == 0) {
				return WriteError;
			} else {
				size -= sz;
				data += sz;
				total += sz;
				if (0 == size)
					break;
			}
		}

		return total;
	}

	bool SocketHasError(int fd) {
		int error;
		socklen_t len = sizeof(error);  
		int code = getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);

		if (code < 0 || error)
			return true;
		return false;
	}
}





