#include "Timestamp.h"

#if defined (WIN32)
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif


uint64 Now()
{
#if defined (WIN32)
	return time(NULL);
#else
	timeval tv;
	gettimeofday( &tv, NULL );
	return tv.tv_sec;
#endif
}

uint64 StampPersecond()
{
#if defined (WIN32)
	LARGE_INTEGER rate;
	QueryPerformanceFrequency(&rate);
	return rate.QuadPart;
#else
	return 1000000ULL;
#endif
}

uint64 TimeStamp()
{
#if defined (WIN32)
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
#else
	timeval tv;
	gettimeofday( &tv, NULL );
	return 1000000ULL * uint64( tv.tv_sec ) + uint64( tv.tv_usec );
#endif

}
