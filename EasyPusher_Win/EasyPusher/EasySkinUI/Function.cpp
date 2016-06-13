/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "stdafx.h"
#include "Function.h"

  void WINAPI OutputString(LPCTSTR lpStr, ...)
 {
 	CString StrDes;
 
 	va_list argList;
 	va_start( argList, lpStr );
 	StrDes.FormatV( lpStr, argList );
 	va_end( argList );
 
 	OutputDebugString(StrDes);
 }
