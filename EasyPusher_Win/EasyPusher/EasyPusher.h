/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyPusher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "GlobalUnits.h"

// CEasyPusherApp:
// See EasyPusher.cpp for the implementation of this class
//

// get program path for globle use [6/12/2016 SwordTwelve]
// 通用结构定义 
struct ModuleFileInfomations
{
	CString		strFullPath;
	CString		strPath;
	CString		strDrive;
	CString		strDir;
	CString		strName;
	CString		strExt;
};
const ModuleFileInfomations&    GetModuleFileInformations();

#define GET_MODULE_FILE_INFO    (GetModuleFileInformations())

class CEasyPusherApp : public CWinApp
{
public:
	CEasyPusherApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CEasyPusherApp theApp;