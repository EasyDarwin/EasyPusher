/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#ifndef _DLLMANAGER_H_
#define _DLLMANAGER_H_

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) ||defined(_WIN32_WCE)
  #define DLL_EXPORT _declspec(dllexport)
  #define DLL_IMPORT _declspec(dllimport)
#else
  #define DLL_EXPORT 
  #define DLL_IMPORT 
#endif

#endif

