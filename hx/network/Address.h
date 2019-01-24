#ifndef ADDRESS_H
#define ADDRESS_H

#include <assert.h>
#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#define snprintf _snprintf
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#endif
#include <string>

namespace Network {

struct Addr {
public:
	enum {
		SOCK_ADDR_EMPTY = 0,
		SOCK_ADDR_IPV4,
		SOCK_ADDR_IPV6,
		SOCK_ADDR_UNIX,
	};

	union {
        struct sockaddr_in 	in;
        struct sockaddr_in6 in6;
#ifndef _WIN32
        struct sockaddr_un 	un;
#endif
    } sockaddr;

	int addrType;
	int family;

    static Addr MakeIP4Addr(const char *ip,int port) {
    	Addr addr;
    	memset(&addr.sockaddr,0,sizeof(addr.sockaddr));
    	addr.sockaddr.in.sin_family = AF_INET;
    	addr.sockaddr.in.sin_port = htons(port);
    	if(inet_pton(AF_INET,ip,&addr.sockaddr.in.sin_addr) < 0) {
    		addr.addrType = SOCK_ADDR_EMPTY;
    	} else {
    		addr.addrType = SOCK_ADDR_IPV4;
    	    addr.family = AF_INET;
        }
    	return addr;
    }

#ifndef _WIN32
    static Addr MakeUNIXAddr(const char* file) {
        Addr addr;
        memset(&addr.sockaddr,0,sizeof(addr.sockaddr));
        addr.family = AF_UNIX;
        addr.addrType = SOCK_ADDR_UNIX;
        addr.sockaddr.un.sun_family = AF_UNIX;
        strcpy(addr.sockaddr.un.sun_path, file);
        return addr;
    }
#endif

    static Addr MakeBySockAddr(struct sockaddr *addr_,socklen_t len) {
        Addr addr;
        memset(&addr.sockaddr,0,sizeof(addr.sockaddr));
        if(len == sizeof(addr.sockaddr.in)){
            addr.family   = AF_INET;
            addr.addrType = SOCK_ADDR_IPV4;
            addr.sockaddr.in = *((struct sockaddr_in*)addr_);
        } else if(len == sizeof(addr.sockaddr.in6)){
            addr.family   = AF_INET6;
            addr.addrType = SOCK_ADDR_IPV6;
            addr.sockaddr.in6 = *((struct sockaddr_in6*)addr_);
#ifndef _WIN32
        } else if(len == sizeof(addr.sockaddr.un)){
            addr.family   = AF_UNIX;            
            addr.addrType = SOCK_ADDR_UNIX;
            addr.sockaddr.un = *((struct sockaddr_un*)addr_);
#endif
        } else {
            addr.addrType = SOCK_ADDR_EMPTY;
        }
        return addr;
    }

    struct sockaddr* Address() const {
    	return (struct sockaddr*)&this->sockaddr;
    }
	
    void SetAddrLen(socklen_t len) {
		if ( len == sizeof( struct sockaddr_in ) ) {
			family = AF_INET;
			addrType = SOCK_ADDR_IPV4;
		}
		else if ( len == sizeof( struct sockaddr_in6 ) ) {
			family = AF_INET6;
			addrType = SOCK_ADDR_IPV6;
		}
#ifndef _WIN32
		else if ( len == sizeof( struct sockaddr_un ) ) {
			family = AF_UNIX;
			addrType = SOCK_ADDR_UNIX;
		}
#endif
		else {
			addrType = SOCK_ADDR_EMPTY;
		}
    }
    socklen_t AddrLen() const {
		if ( this->addrType == SOCK_ADDR_IPV4 ) {
			return sizeof( this->sockaddr.in );
		}
		else if ( this->addrType == SOCK_ADDR_IPV6 ) {
			return sizeof( this->sockaddr.in6 );
		}
#ifndef _WIN32
		else if ( this->addrType == SOCK_ADDR_UNIX ) {
			return sizeof( this->sockaddr.un );
		}
#endif
		else {
			return 0;
		}
    }

    int Family() const {
        return this->family;
    }


    bool IsVaild() const {
    	return this->addrType >= SOCK_ADDR_IPV4 && this->addrType <= SOCK_ADDR_UNIX;
    }

    Addr():addrType(SOCK_ADDR_EMPTY),family(0){} 

    std::string ToStr() const {
		if ( this->addrType == SOCK_ADDR_IPV4 ) {
			char ip[INET6_ADDRSTRLEN] = { 0 };
			char ret[INET6_ADDRSTRLEN] = { 0 };
			uint16_t port = ntohs(this->sockaddr.in.sin_port);
#ifndef _WIN32
			if ( NULL == ::inet_ntop(this->family, (const char*)&this->sockaddr.in.sin_addr, ip, sizeof( ip )) ){
#else
			if ( NULL == ::inet_ntop(this->family, (PVOID)&this->sockaddr.in.sin_addr, ip, sizeof( ip )) ){
#endif
				return std::string("");
			}
			else {
				snprintf(ret, INET6_ADDRSTRLEN, "%s:%d", ip, port);
				return std::string(ret);
			}
		}
        return std::string("");
    }
};
}

#endif
