#ifndef TYPEDEF_H
#define TYPEDEF_H


#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#include <BaseTsd.h>
typedef INT8	int8;
typedef INT16	int16;
typedef INT32	int32;
typedef INT64	int64;
typedef UINT8	uint8;
typedef UINT16	uint16;
typedef UINT32	uint32;
typedef UINT64	uint64;
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

#ifdef USE_BOOST
#include <boost/unordered_map.hpp>
#define FishMap boost::unordered_map 
#else
#include <map>
#define FishMap std::map 
#endif