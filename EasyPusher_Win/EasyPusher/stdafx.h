/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

//#pragma comment(lib,"libcmt.lib")

// EasySkinUI的支持
//图片控件
#import "ImageOle.dll" named_guids
//头文件
//#include "./EasySkinUI/detours.h"
#include "./EasySkinUI/EasySkinUI_ScrollBar.h"
#pragma comment(lib,"Gdiplus.lib")
#include "./EasySkinUI/EasySkinUI.h"
#define COLOR_BACK					RGB(194,247,255)//249,245,237

#define	VIDEO_WINDOW_BORDER_WIDTH	0

#define __CREATE_WINDOW(_x, _class, _id) {if (NULL == _x) {_x = (_class*)GetDlgItem(_id);}}
#define __MOVE_WINDOW(x, _rect)	{if (NULL != x) {x->MoveWindow(&_rect);}}
#define __MOVE_WINDOW_INVALIDATE(x, _rect)	{if (NULL != x) {x->MoveWindow(&_rect); x->Invalidate();}}
#define __DESTROY_WINDOW(x)	{if (NULL != x) {x->DestroyWindow(); delete x; x=NULL;}}

bool __WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);
bool __MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);

#define	WM_UPDATE_LOCAL_VIDEO		(WM_USER+6001)
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


