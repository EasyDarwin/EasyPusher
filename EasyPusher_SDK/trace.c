#include "trace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



void _TRACE(char* szFormat, ...)
{
#ifdef _DEBUG
  char buff[1024] = {0,};
  va_list args;
  va_start(args,szFormat);
#ifdef _WIN32
  _vsnprintf(buff, 1023, szFormat,args);
#else
  vsnprintf(buff, 1023, szFormat,args);
#endif
  va_end(args);

#ifdef _WIN32
	OutputDebugString(buff);
#endif
	printf("%s", buff);
#endif
}

void _TRACE_LOG(char *szFormat, ...)
{
  char buff[1024] = {0,};
  va_list args;
  va_start(args,szFormat);
#ifdef _WIN32
  _vsnprintf(buff, 1023, szFormat,args);
#else
  vsnprintf(buff, 1023, szFormat,args);
#endif
  va_end(args);

#ifdef _WIN32
	OutputDebugString(buff);
#endif
	printf("%s", buff);
}


void _TRACE_ERR(char *szFormat, ...)
{
  char buff[1024] = {0,};
  va_list args;
  va_start(args,szFormat);
#ifdef _WIN32
  _vsnprintf(buff, 1023, szFormat,args);
#else
  vsnprintf(buff, 1023, szFormat,args);
#endif
  va_end(args);

#ifdef _WIN32
	OutputDebugString(buff);
#endif
	printf("%s", buff);
}

