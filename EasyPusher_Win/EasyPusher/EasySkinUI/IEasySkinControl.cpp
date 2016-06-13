/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "StdAfx.h"
#include "IEasySkinControl.h"

IEasySkinControl::IEasySkinControl(void)
{
	m_hParentDC = NULL;
	m_bTransparent = FALSE;
	m_colDefText = RGB(128,128,128);
	m_colNormalText = RGB(0,0,0);
	m_colDisableText = RGB(128,128,128);
	m_colReadOnlyText = RGB(0,0,0);
	m_colSelectText = RGB(255,255,255);
	m_colFrameNormal = RGB(0,0,0);
	m_colBack=RGB(255,255,255);
	m_Font = UIRenderEngine->GetDeaultFont();
	m_dwTextAlign = 0;
}

IEasySkinControl::~IEasySkinControl(void)
{
	m_bTransparent = FALSE;
}

void IEasySkinControl::CalcCenterRect( RECT& rcDest, int cx, int cy, RECT& rcCenter )
{
	int x = ((rcDest.right-rcDest.left) - cx + 1) / 2;
	int y = ((rcDest.bottom-rcDest.top) - cy + 1) / 2;

	rcCenter.left = rcDest.left+x;
	rcCenter.top = rcDest.top+y;
	rcCenter.right = rcCenter.left+cx;
	rcCenter.bottom = rcCenter.top+cy;
}

void IEasySkinControl::DrawParentWndBg(HWND hWnd, HDC hDC )
{
	if( hWnd == NULL ) return;
	if ( !m_bTransparent ) return;

	HWND hParentWnd = ::GetParent(hWnd);

	CRect rcWindow;
	::GetWindowRect(hWnd,&rcWindow);
	::ScreenToClient(hParentWnd, (LPPOINT)&rcWindow); 
	::ScreenToClient(hParentWnd, ((LPPOINT)&rcWindow)+1);

	::BitBlt(hDC, 0, 0, rcWindow.Width(), rcWindow.Height(), m_hParentDC, rcWindow.left, rcWindow.top, SRCCOPY);
}

void IEasySkinControl::SetDefText( COLORREF colText )
{
	m_colDefText = colText;
}

void IEasySkinControl::SetNormalText( COLORREF colText )
{
	m_colNormalText = colText;
}

void IEasySkinControl::SetDisableText( COLORREF colText )
{
	m_colDisableText = colText;
}

void IEasySkinControl::SetSelectText( COLORREF colText )
{
	m_colSelectText = colText;
}

void IEasySkinControl::SetFrameColor( COLORREF colFrame )
{
	m_colFrameNormal = colFrame;
}

BOOL IEasySkinControl::SetScrollImage( CWnd * pOwn,LPCTSTR pszFileName )
{
	UIRenderEngine->RemoveImage(m_pImageScroll);
	m_pImageScroll = UIRenderEngine->GetImage(pszFileName);

	if ( (m_pImageScroll != NULL) && (pOwn != NULL) && (pOwn->GetSafeHwnd() != NULL) )
	{
		ASSERT(m_pImageScroll != NULL);
		if( m_pImageScroll == NULL) return FALSE;

		//ÉèÖÃ¹ö¶¯
		EasySkinUI_Init(pOwn->GetSafeHwnd(),m_pImageScroll->ImageToBitmap());
	}

	return TRUE;
}

void IEasySkinControl::RemoveScorll()
{
	UIRenderEngine->RemoveImage(m_pImageScroll);
}

BOOL IEasySkinControl::TrackMouseLeave( HWND hWnd )
{
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hWnd;

	return _TrackMouseEvent(&tme);
}

void IEasySkinControl::SetCtrlFont( HFONT hFont )
{
	m_Font = hFont;
}

HFONT IEasySkinControl::GetCtrlFont()
{
	return m_Font;
}

void IEasySkinControl::SetBackColor( COLORREF colBack )
{
	m_colBack = colBack;
}

void IEasySkinControl::SetUnTransparent()
{
	m_bTransparent  = false;
}

void IEasySkinControl::SetTextAlign( WORD wTextAlign )
{
	m_dwTextAlign = wTextAlign;
}
