/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// EasySkinEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Control.h"
#include "EasySkinEdit.h"


// CEasySkinEdit

IMPLEMENT_DYNAMIC(CEasySkinEdit, CEdit)

CEasySkinEdit::CEasySkinEdit()
{
	m_pBackImgN = NULL;
	m_pBackImgH = NULL;
	m_pIconImg = NULL;

	m_bFocus = m_bPress = m_bHover = m_bMouseTracking = FALSE;
	m_nIconWidth = 0;
	m_bHandCursor = false;

	m_bIsDefText = FALSE;
	m_cPwdChar = 0;
	m_ptClient.SetPoint(0,0);
	m_colBack=RGB(255,255,255);
}

CEasySkinEdit::~CEasySkinEdit()
{
}


BEGIN_MESSAGE_MAP(CEasySkinEdit, CEdit)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONUP()
END_MESSAGE_MAP()

BOOL CEasySkinEdit::SetBackNormalImage( LPCTSTR lpszFileName, CONST LPRECT lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImgN);
	m_pBackImgN = UIRenderEngine->GetImage(lpszFileName);
	
	if (m_pBackImgN != NULL)
		m_pBackImgN->SetNinePart(lpNinePart);

	return (m_pBackImgN != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinEdit::SetBackHotImage( LPCTSTR lpszFileName,CONST LPRECT lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImgH);
	m_pBackImgH = UIRenderEngine->GetImage(lpszFileName);

	if (m_pBackImgH != NULL)
		m_pBackImgH->SetNinePart(lpNinePart);

	return (m_pBackImgH != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinEdit::SetIconImage( LPCTSTR lpszFileName,bool bHandCursor /*= false*/ )
{
	UIRenderEngine->RemoveImage(m_pIconImg);
	m_pIconImg = UIRenderEngine->GetImage(lpszFileName);

	if (m_pIconImg != NULL)
	{
		m_nIconWidth = m_pIconImg->GetWidth();
	
// 		CRect rcClient;
// 		GetClientRect(&rcClient);
// 		rcClient.right-=m_nIconWidth;
// 
// 		SetRect(&rcClient);

		//CRect rcWindow;
		//GetWindowRect(&rcWindow);
		//GetParent()->ScreenToClient(&rcWindow);
		//rcWindow.right+=1;
		//MoveWindow(rcWindow,TRUE);
	}

	m_bHandCursor = bHandCursor;

	return (m_pIconImg != NULL) ? TRUE : FALSE;
}

void CEasySkinEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (NULL != m_pIconImg && !m_pIconImg->IsNull())
	{
		if (bCalcValidRects)
		{
			lpncsp->rgrc[0].left += m_ptClient.x;
			lpncsp->rgrc[0].top += m_ptClient.y;
			lpncsp->rgrc[0].right -= m_nIconWidth;
			lpncsp->rgrc[1] = lpncsp->rgrc[0];
		}
	}

	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CEasySkinEdit::OnNcPaint()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	CDC *pWindowDC = GetWindowDC();
	CMemoryDC MemDC(pWindowDC,rcWindow);
	
	CRect rcIcon;
	if (m_pIconImg != NULL && !m_pIconImg->IsNull())
	{
		int cxIcon = m_pIconImg->GetWidth();
		int cyIcon = m_pIconImg->GetHeight();

 		CalcCenterRect(rcWindow, cxIcon, cyIcon, rcIcon);
 		rcIcon.right = rcWindow.right - 2;
 		rcIcon.left = rcIcon.right - cxIcon;
	}

	MemDC.FillSolidRect(&rcWindow,m_colBack);
	DrawParentWndBg(GetSafeHwnd(),MemDC.GetSafeHdc());

	if (m_bHover)
	{
		if (m_pBackImgH != NULL && !m_pBackImgH->IsNull())
		{
			m_pBackImgH->Draw(&MemDC, rcWindow);
		}
		else
		{
			if (m_pBackImgN != NULL && !m_pBackImgN->IsNull())
				m_pBackImgN->Draw(&MemDC, rcWindow);
		}
	}
	else
	{
		if (m_pBackImgN != NULL && !m_pBackImgN->IsNull())
			m_pBackImgN->Draw(&MemDC, rcWindow);
	}

 	if (m_pIconImg != NULL && !m_pIconImg->IsNull())
 		m_pIconImg->Draw(&MemDC, rcIcon);

	pWindowDC->BitBlt(rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(),&MemDC,0,0,SRCCOPY);

	ReleaseDC(pWindowDC);
}

BOOL CEasySkinEdit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CEasySkinEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcIcon;
	CPoint pt;
	GetCursorPos(&pt);
	GetIconRect(rcIcon);

	if( PtInRect(&rcIcon,pt)) return;

 	m_bPress = TRUE;

	__super::OnLButtonDown(nFlags, point);
}

void CEasySkinEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rcIcon;
	CPoint pt;
	GetCursorPos(&pt);
	GetIconRect(rcIcon);

	if( PtInRect(&rcIcon,pt)) 
	{
		return;
	}
	else
	{
		if (m_bPress)
		{
			m_bPress = FALSE;
			RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
		}
	}

	__super::OnLButtonUp(nFlags, point);
}

void CEasySkinEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TrackMouseLeave(GetSafeHwnd());

		m_bMouseTracking = TRUE;
		m_bHover = TRUE;
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	__super::OnMouseMove(nFlags, point);
}

LRESULT CEasySkinEdit::OnMouseLeave( WPARAM wparam, LPARAM lparam )
{
	m_bMouseTracking = FALSE;
	m_bHover = FALSE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return TRUE;
}

void CEasySkinEdit::OnDestroy()
{
	__super::OnDestroy();

	UIRenderEngine->RemoveImage(m_pBackImgN);
	UIRenderEngine->RemoveImage(m_pBackImgH);
	UIRenderEngine->RemoveImage(m_pIconImg);
	RemoveScorll();
}

void CEasySkinEdit::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);

	if (m_bIsDefText)
	{
		m_bIsDefText = FALSE;
		SetPasswordChar(m_cPwdChar);
		SetWindowText(_T(""));
	}

	m_bFocus = TRUE;
	Invalidate(FALSE);
}

void CEasySkinEdit::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	if (GetWindowTextLength() <= 0 && !m_strDefText.IsEmpty())
	{
		m_bIsDefText = TRUE;
		m_cPwdChar = GetPasswordChar();
		SetPasswordChar(0);
		SetWindowText(m_strDefText);
	}

	m_bFocus = FALSE;
	Invalidate(FALSE);
}

BOOL CEasySkinEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CRect rcIcon;
	CPoint pt;
	GetCursorPos(&pt);
	GetIconRect(rcIcon);

	if( PtInRect(&rcIcon,pt))
	{
		if( m_bHandCursor )
			SetCursor(LoadCursor(NULL,IDC_HAND));
		else
			SetCursor(LoadCursor(NULL,IDC_ARROW));

		return TRUE;
	}

	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CEasySkinEdit::GetIconRect( RECT &rcIcon )
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	CalcCenterRect(rcWindow, m_nIconWidth, rcWindow.Height(), rcIcon);
	rcIcon.right = rcWindow.right - 2;
	rcIcon.left = rcIcon.right - m_nIconWidth;
}

BOOL CEasySkinEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return 	__super::CreateEx(
		WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOPARENTNOTIFY|WS_EX_CLIENTEDGE,
		TEXT("EDIT"),
		NULL,
		WS_VISIBLE|WS_CHILDWINDOW|WS_TABSTOP|ES_LEFT/*|ES_AUTOHSCROLL*/|dwStyle,
		rect,pParentWnd,nID);


	return __super::Create(dwStyle, rect, pParentWnd, nID);
}

void CEasySkinEdit::SetDefaultText( LPCTSTR lpszText )
{
	m_strDefText = lpszText;
}

void CEasySkinEdit::SetDefaultTextMode( BOOL bIsDefText )
{
	if (bIsDefText == m_bIsDefText)
		return;

	m_bIsDefText = bIsDefText;
	if (m_bIsDefText)
	{
		m_cPwdChar = GetPasswordChar();
		SetPasswordChar(0);
		SetWindowText(m_strDefText);
	}
	else
	{
		SetPasswordChar(m_cPwdChar);
		SetWindowText(_T(""));
	}
}

HBRUSH CEasySkinEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	if (m_bIsDefText)
		pDC->SetTextColor(m_colDefText);
	else
		pDC->SetTextColor(m_colNormalText);

	return (HBRUSH)NULL_BRUSH;
}

LRESULT CEasySkinEdit::OnNcHitTest(CPoint point)
{
	CRect rcIcon;
	GetIconRect(rcIcon);

	if( PtInRect(&rcIcon,point))
	{
		return HTBORDER;
	}

	return __super::OnNcHitTest(point);
}

void CEasySkinEdit::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CWnd *pWnd = GetParent();

	if ( pWnd != NULL && pWnd->GetSafeHwnd() != NULL )
	{
		CRect rcIcon;
		CPoint pt;
		GetCursorPos(&pt);
		GetIconRect(rcIcon);

		if( PtInRect(&rcIcon,pt)) 
		{
			//m_bSendMsg = true;
			pWnd->PostMessage(WM_EDIT_CLICK,GetDlgCtrlID());
			//m_bSendMsg = false;
		}
	}

	__super::OnNcLButtonUp(nHitTest, point);
}

void CEasySkinEdit::SetClientPoint( CPoint pt )
{
	m_ptClient = pt;
}