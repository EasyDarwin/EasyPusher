/*

 Copyright (C) 2009 Anton Burdinuk

 clark15b@gmail.com

*/
// Get code from tsdemux git demo [6/22/2016 Administrator]

#ifndef __COMMON_H
#define __COMMON_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <map>
#include <string>
#include <list>
#include <memory.h>
#include <stdlib.h>
#ifndef _WIN32
#include <dirent.h>
#include <getopt.h>
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
//#include "getopt.h"
#endif
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <memory>
#include <vector>
#include <fcntl.h>
#include <stdarg.h>

#ifndef _WIN32
#define O_BINARY 0
#else
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned long u_int32_t;
typedef unsigned long long u_int64_t;
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

#ifndef _WIN32
#define os_slash        '/'
#else
#define os_slash        '\\'
#endif

#endif
