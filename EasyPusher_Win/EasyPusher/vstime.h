/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef __VS_TIME_H__
#define __VS_TIME_H__


#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mmsystem.h"

#pragma comment(lib, "winmm.lib")
typedef struct __VS_TIME_T
{
	unsigned int time_sec;
	unsigned int msec;
}VS_TIME_T;



void _VS_BEGIN_TIME_PERIOD(unsigned int _msec);
void _VS_END_TIME_PERIOD(unsigned int _msec);

void __VS_Delay(unsigned int _msec);
unsigned int _VS_GetTime(VS_TIME_T *_usagetime);
unsigned int _VS_CalcTimeInterval(VS_TIME_T *_starttime, VS_TIME_T *_endtime);





#endif
