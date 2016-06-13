/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "trace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	// Get the required size of the buffer that receives the Unicode
	// string.
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);
 
	if(dwSize < dwMinSize)
	{
		return false;
	}
 
	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);  
	return true;
}

bool WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(dwSize < dwMinSize)
	{
		return false;
	}
	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
	return true;
}

void _TRACE(char* szFormat, ...)
{
#ifdef _DEBUG
	int iInterval = 0;
	static SYSTEMTIME	lastSystemtime;
	static int iInitSystemtime = 0x00;
	if (iInitSystemtime == 0x00)
	{
		GetSystemTime(&lastSystemtime);
		iInitSystemtime = 0x01;
	}
	else
	{
		SYSTEMTIME	systemTime;
		//GetSystemTime(&systemTime);
		GetLocalTime(&systemTime);

		if (systemTime.wSecond == lastSystemtime.wSecond)
		{
			iInterval = systemTime.wMilliseconds - lastSystemtime.wMilliseconds;
		}
		else
		{
			iInterval = 1000-lastSystemtime.wMilliseconds+systemTime.wMilliseconds;
		}
		//GetSystemTime(&lastSystemtime);
		GetLocalTime(&lastSystemtime);
	}

	SYSTEMTIME	systemTime;
	//GetSystemTime(&systemTime);
	GetLocalTime(&systemTime);
	_TRACE_W(TEXT("%u:%u:%u.%03d[%d]\t"), systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds, iInterval);


  char buff[1024] = {0,};
  wchar_t wszbuff[1024] = {0,};
  va_list args;
  va_start(args,szFormat);
  _vsnprintf(buff, 1023, szFormat,args);
  va_end(args);

  MByteToWChar(buff, wszbuff, sizeof(wszbuff)/sizeof(wszbuff[0]));
#ifdef _WIN32
	OutputDebugString(wszbuff);
#endif
	printf("TRACE: %s", buff);
#endif

}

void _TRACE_W(const wchar_t * szFormat, ...)
{
#ifdef _DEBUG
	wchar_t buff[1024] = {0};
	va_list args;
	va_start(args,szFormat);
	_vsnwprintf(buff, 1023, szFormat,args);
	va_end(args);

	#ifdef _WIN32
		OutputDebugString(buff);
	#endif
	printf("TRACE_W: %s", buff);
#endif
}

