#ifndef FDSET_H
#define FDSET_H
#include "../Platform.h"
#include <stdlib.h>

class FdSet
{
	struct win_fd_set
	{
		u_int fd_count;
		SOCKET fd_array[1];
	};

public:
	enum { default_size = 64 };

	FdSet() : _capacity(default_size)
	{
		_fd_set = (win_fd_set*)malloc(sizeof(win_fd_set) - sizeof(SOCKET) + sizeof(SOCKET) * _capacity);
		_fd_set->fd_count = 0;
	}

	~FdSet()
	{
		free((void*)_fd_set);
	}

	void Copy(FdSet& fdset)
	{
		Reserve(fdset._fd_set->fd_count);
		_fd_set->fd_count = fdset._fd_set->fd_count;
		memcpy(_fd_set->fd_array,fdset._fd_set->fd_array,fdset._fd_set->fd_count * sizeof(SOCKET));
	}

	void Reset()
	{
		_fd_set->fd_count = 0;
	}

	bool Set(SOCKET descriptor)
	{
		for (u_int i = 0; i < _fd_set->fd_count; ++i)
			if (_fd_set->fd_array[i] == descriptor)
				return true;

		Reserve(_fd_set->fd_count + 1);
		_fd_set->fd_array[_fd_set->fd_count++] = descriptor;
		return true;
	}

	void Clear(SOCKET descriptor)
	{
		u_int __i; 
		for (__i = 0; __i < _fd_set->fd_count ; __i++) 
		{ 
			if (_fd_set->fd_array[__i] == descriptor)
			{ 
				while (__i < _fd_set->fd_count-1)
				{ 
					_fd_set->fd_array[__i] =  _fd_set->fd_array[__i+1]; 
					__i++; 
				} 
				_fd_set->fd_count--; 
				break; 
			} 
		}
	}

	bool Empty()
	{
		return _fd_set->fd_count == 0;
	}

	bool IsSet(SOCKET descriptor) const
	{
		return !!__WSAFDIsSet(descriptor,const_cast<fd_set*>(reinterpret_cast<const fd_set*>(_fd_set)));
	}

	operator fd_set*()
	{
		return reinterpret_cast<fd_set*>(_fd_set);
	}
private:
	void Reserve(u_int n)
	{
		if (n <= _capacity)
			return;

		u_int ncap = _capacity + _capacity / 2;
		if (ncap < n)
			ncap = n;

		win_fd_set* nfd_set = static_cast<win_fd_set*>(malloc(sizeof(win_fd_set) - sizeof(SOCKET) + sizeof(SOCKET) * (ncap)));

		nfd_set->fd_count = _fd_set->fd_count;
		for (u_int i = 0; i < _fd_set->fd_count; ++i)
			nfd_set->fd_array[i] = _fd_set->fd_array[i];

		free((void*)_fd_set);
		_fd_set = nfd_set;
		_capacity = ncap;
	}

	win_fd_set* _fd_set;
	u_int _capacity;
};
#endif