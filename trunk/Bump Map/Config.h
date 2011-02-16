#ifndef _Config_H
#define _Config_H

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string>
#include <assert.h>

typedef __int16					int16;
typedef __int32					int32;
typedef __int64					int64;
typedef unsigned __int16		uint16;
typedef unsigned __int32		uint32;
typedef unsigned __int64		uint64;

typedef uint16 ushort;
typedef uint32 uint;
typedef uint64 ulong;

#ifndef  M_PI 
#define M_PI float(3.1415926)
#endif

#endif