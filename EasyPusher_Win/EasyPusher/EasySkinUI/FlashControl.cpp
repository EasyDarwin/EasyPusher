/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "stdafx.h"
#include "FlashControl.h"

/////////////////////////////////////////////////////////////////////////////
// ShockwaveFlash

IMPLEMENT_DYNCREATE(CFlashControl, CWnd)

long CFlashControl::s_lCount = 0;

CFlashControl::CFlashControl()
{
#if 0
	if (0 == InterlockedExchangeAdd((volatile long *)&s_lCount, 1))
	{
		TCHAR szWorkDirectory[MAX_PATH<<1]={0};
		UIRenderEngine->GetWorkDirectory(szWorkDirectory,MAX_PATH<<1);
		StrCat(szWorkDirectory,TEXT("\\Flash8.ocx"));

		HMODULE	hmod=LoadLibrary (szWorkDirectory);
		ASSERT(hmod);
		if (hmod)
		{
			FARPROC  pReg =GetProcAddress (hmod,"DllRegisterServer");
			(*pReg)();
		}
	}
#endif

	m_bHasInit = FALSE;
	m_lOldProc = 0;
	m_pIFlashContrlSink = NULL;
}

CFlashControl::~CFlashControl()
{
#if 0
	if (1 == InterlockedExchangeAdd((volatile long *)&s_lCount, -1))
	{
		TCHAR szWorkDirectory[MAX_PATH<<1]={0};
		UIRenderEngine->GetWorkDirectory(szWorkDirectory,MAX_PATH<<1);
		StrCat(szWorkDirectory,TEXT("\\Flash8.ocx"));

		HMODULE	hmod=LoadLibrary (szWorkDirectory);
		ASSERT(hmod);
		if (hmod)
		{
			FARPROC  pReg =GetProcAddress (hmod,"DllUnregisterServer");
			(*pReg)();
		}
	}
#endif
}

LRESULT CFlashControl::FlashWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CFlashControl *pFlashCtrl = (CFlashControl *)FromHandle(hWnd);
	ASSERT(pFlashCtrl);

	switch ( msg )
	{
 	case WM_ERASEBKGND:
 		{
 			return TRUE;
 		}
	case WM_PAINT:
		{
			if ( pFlashCtrl->m_pIFlashContrlSink != NULL )
			{
				CDC*pDC = pFlashCtrl->GetDC();
				CRect rcClient;
				pFlashCtrl->GetClientRect(&rcClient);

				pFlashCtrl->m_pIFlashContrlSink->OnDraw(pDC,rcClient.Width(),rcClient.Height());
				pFlashCtrl->ReleaseDC(pDC);
			}

			return 0;
			return CallWindowProc((WNDPROC)(pFlashCtrl->m_lOldProc), hWnd, msg, wParam, lParam); 
		}
	case WM_RBUTTONDOWN:
		return 0;
	case WM_LBUTTONDOWN:
		{
			HWND hParentWnd = ::GetParent(hWnd);
			if ( hParentWnd != NULL )
			{
				UINT uID = pFlashCtrl->GetDlgCtrlID();
				::PostMessage(hParentWnd,WM_COMMAND,uID,0);
			}
			break;
		}
	}

	if ( msg == 144 || msg == 8720 || msg == 24 || msg == 2 || msg == 130)
	{
		return 0;
	}

	//调用原来的窗口过程
	return CallWindowProc((WNDPROC)(pFlashCtrl->m_lOldProc), hWnd, msg, wParam, lParam); 
}

BOOL CFlashControl::Init(IFlashContrlSink*pIFlashContrlSink)
{
	if (FALSE == m_bHasInit)
	{
		m_lOldProc = SetWindowLong(m_hWnd, GWL_WNDPROC, (long)FlashWinProc);
	}

	m_pIFlashContrlSink = pIFlashContrlSink;
	m_bHasInit = TRUE;

	return TRUE;
}
