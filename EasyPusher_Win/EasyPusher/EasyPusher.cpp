/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyPusher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EasyPusher.h"
#include "EasyPusherDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEasyPusherApp

BEGIN_MESSAGE_MAP(CEasyPusherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEasyPusherApp construction

CEasyPusherApp::CEasyPusherApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEasyPusherApp object

CEasyPusherApp theApp;


// CEasyPusherApp initialization

BOOL CEasyPusherApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Easy Client"));

	//EasySkinUI Support
	UIRenderEngine->GetWorkDirectory(GlobalUnits->m_szDefaultSkin,MAX_PATH<<1);
	//加载主界面背景图片，该界面可继承于CEasySkinManager
	StrCat(GlobalUnits->m_szDefaultSkin,TEXT("\\SkinUI\\图标\\头部背景循环.png"));//\\SkinUI\\Skin\\main14.jpg
	CFileFind find;
	if ( find.FindFile(GlobalUnits->m_szDefaultSkin) == FALSE )
	{
		AfxMessageBox(TEXT("资源加载失败"));
		return FALSE;
	}

	CEasyPusherDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



BOOL CEasyPusherApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

const ModuleFileInfomations& GetModuleFileInformations()
{
	static ModuleFileInfomations __s_mi;
	static BOOL bLoad = FALSE;

	if(!bLoad)
	{
		// Get application's full path.

		::GetModuleFileName(NULL, __s_mi.strFullPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		__s_mi.strFullPath.ReleaseBuffer();

		// Break full path into seperate components.
		_tsplitpath_s(
			__s_mi.strFullPath, 
			__s_mi.strDrive.GetBufferSetLength(_MAX_DRIVE + 1), _MAX_DRIVE + 1,
			__s_mi.strDir.GetBufferSetLength(_MAX_DIR + 1), _MAX_DIR + 1,
			__s_mi.strName.GetBufferSetLength(_MAX_FNAME + 1), _MAX_FNAME + 1,
			__s_mi.strExt.GetBufferSetLength(_MAX_EXT + 1), _MAX_EXT + 1);

		__s_mi.strDrive.ReleaseBuffer();//盘符
		__s_mi.strDir.ReleaseBuffer();//目录中间路径
		__s_mi.strName.ReleaseBuffer();//文件名称
		__s_mi.strExt.ReleaseBuffer();//文件扩展名(.exe)

		TCHAR   sDrive[_MAX_DRIVE];   
		TCHAR   sDir[_MAX_DIR];   
		TCHAR   sFilename[_MAX_FNAME],Filename[_MAX_FNAME];   
		TCHAR   sExt[_MAX_EXT];   

		GetModuleFileName(AfxGetInstanceHandle(),   Filename,   _MAX_PATH);   
		_tsplitpath_s(Filename,   sDrive,   sDir,   sFilename,   sExt); 

		CString  homeDir(CString(sDrive) + CString(sDir));   
		int      nLen = homeDir.GetLength(); 

		if(homeDir.GetAt(nLen-1) != _T('\\'))   
			homeDir   +=   _T('\\');   

		__s_mi.strPath = homeDir;

		bLoad = TRUE;
	}

	return __s_mi;
}