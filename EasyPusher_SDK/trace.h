#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef _WIN32
#include <winsock2.h>
#endif



void _TRACE(char *szFormat, ...);

void _TRACE_LOG(char *szFormat, ...);
void _TRACE_ERR(char *szFormat, ...);


#endif
