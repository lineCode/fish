#ifndef TYPEDEF_H
#define TYPEDEF_H


#if defined( WIN32 )
#include <BaseTsd.h>
typedef INT8	int8;
typedef INT16	int16;
typedef INT32	int32;
typedef INT64	int64;
typedef UINT8	uint8;
typedef UINT16	uint16;
typedef UINT32	uint32;
typedef UINT64	uint64;

#include <process.h>  

#define THREAD_ID											HANDLE
#define THREAD_SINGNAL										HANDLE
#define THREAD_SINGNAL_INIT(x)								x = CreateEvent(NULL, TRUE, FALSE, NULL)
#define THREAD_SINGNAL_DELETE(x)							CloseHandle(x)
#define THREAD_SINGNAL_SET(x)								SetEvent(x)
#define THREAD_MUTEX										CRITICAL_SECTION
#define THREAD_MUTEX_INIT(x)								InitializeCriticalSection(&x)
#define THREAD_MUTEX_DELETE(x)								DeleteCriticalSection(&x)
#define THREAD_MUTEX_LOCK(x)								EnterCriticalSection(&x)
#define THREAD_MUTEX_UNLOCK(x)								LeaveCriticalSection(&x)

#else
#include <stdint.h>
typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

#include <pthread.h>
#include <unistd.h>
#define THREAD_ID											pthread_t
#define THREAD_SINGNAL										pthread_cond_t
#define THREAD_SINGNAL_INIT(x)								pthread_cond_init(&x, NULL)
#define THREAD_SINGNAL_DELETE(x)							pthread_cond_destroy(&x)
#define THREAD_SINGNAL_SET(x)								pthread_cond_signal(&x);
#define THREAD_MUTEX										pthread_mutex_t
#define THREAD_MUTEX_INIT(x)								pthread_mutex_init (&x, NULL)
#define THREAD_MUTEX_DELETE(x)								pthread_mutex_destroy(&x)
#define THREAD_MUTEX_LOCK(x)								pthread_mutex_lock(&x)
#define THREAD_MUTEX_UNLOCK(x)								pthread_mutex_unlock(&x)

#endif

#define TYPE_BOOL		1
#define TYPE_UINT8		2
#define TYPE_UINT16		3
#define TYPE_UINT32		4
#define TYPE_UINT64		5
#define TYPE_INT8		6
#define TYPE_INT16		7
#define TYPE_INT32		8
#define TYPE_INT64		9
#define TYPE_FLOAT		10
#define TYPE_DOUBLE		11
#define TYPE_STRING		12

#endif



#include <map>
#define FishMap std::map 

//#define SELECT_USE_STDSET