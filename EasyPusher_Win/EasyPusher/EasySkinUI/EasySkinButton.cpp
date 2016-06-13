/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// ButtonEx.cpp : 实现文件
//

#include "stdafx.h"
#include "EasySkinButton.h"


// CEasySkinButton

IMPLEMENT_DYNAMIC(CEasySkinButton, CButton)

CEasySkinButton::CEasySkinButton()
{
	m_pBackImgN = m_pBackImgH = m_pBackImgD = m_pBackImgF = NULL;
	m_pCheckImgN = m_pCheckImgH = m_pCheckImgTickN = m_pCheckImgTickH = NULL;
	m_pArrowImg = m_pIconImg = NULL;
	
	m_bFocus = m_bPress = m_bHover = m_bMouseTracking = FALSE;
	m_nBtnType = en_PushButton;

	m_hMenu = NULL;
	m_hBackDC = NULL;
	m_bPushed = false;
	m_bCurPushState = false;
}

CEasySkinButton::~CEasySkinButton()
{
}


BEGIN_MESSAGE_MAP(CEasySkinButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CEasySkinButton 消息处理程序
void CEasySkinButton::SetButtonType( UI_BUTTON_TYPE type )
{
	m_nBtnType = type;
}

//鼠标移动消息
void CEasySkinButton::OnMouseMove(UINT nFlags, CPoint point)
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

//鼠标离开消息
LRESULT CEasySkinButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bMouseTracking = FALSE;
	m_bHover = FALSE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return 0;
}

//鼠标按下消息
void CEasySkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPress = TRUE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	__super::OnLButtonDown(nFlags, point);
}

//鼠标抬起消息
void CEasySkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bPress)
	{
		//static bool bPushed = false;

		if( !m_bPushed && !m_bCurPushState )
			m_bPress = FALSE;
		
		if ( m_bPushed )
		{
			if( m_bCurPushState ) m_bPress = FALSE;
			else m_bPress = TRUE;

			m_bCurPushState = !m_bCurPushState;
		}

				
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
		
		if (m_nBtnType == en_MenuButton)
		{
			if (::IsMenu(m_hMenu))
			{
				CRect rc;
				GetClientRect(&rc);
				ClientToScreen(&rc);
				::TrackPopupMenuEx(m_hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
					rc.left, rc.bottom, ::GetParent(GetSafeHwnd()), NULL);
			}
		}
	}

	__super::OnLButtonUp(nFlags, point);
}

//鼠标双击消息
void CEasySkinButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bPress = TRUE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	__super::OnLButtonDown(nFlags, point);
}

//获取焦点消息
void CEasySkinButton::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);

	m_bFocus = TRUE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}

//失去焦点消息
void CEasySkinButton::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	m_bFocus = FALSE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}

//重画消息
BOOL CEasySkinButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//设置资源
BOOL CEasySkinButton::SetBackImage( LPCTSTR lpNormal, LPCTSTR lpHoven, LPCTSTR lpDown, LPCTSTR lpFocus,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImgN);
	UIRenderEngine->RemoveImage(m_pBackImgH);
	UIRenderEngine->RemoveImage(m_pBackImgD);
	UIRenderEngine->RemoveImage(m_pBackImgF);

	m_pBackImgN = UIRenderEngine->GetImage(lpNormal);
	m_pBackImgH = UIRenderEngine->GetImage(lpHoven);
	m_pBackImgD = UIRenderEngine->GetImage(lpDown);
	m_pBackImgF = UIRenderEngine->GetImage(lpFocus);

	if( lprcNinePart != NULL )
	{
		if (m_pBackImgN != NULL)
			m_pBackImgN->SetNinePart(lprcNinePart);

		if (m_pBackImgH != NULL)
			m_pBackImgH->SetNinePart(lprcNinePart);

		if (m_pBackImgD != NULL)
			m_pBackImgD->SetNinePart(lprcNinePart);

		if (m_pBackImgF != NULL)
			m_pBackImgF->SetNinePart(lprcNinePart);
	}

	if ((lpNormal != NULL && NULL == m_pBackImgN) || 
		(lpHoven  != NULL && NULL == m_pBackImgH) ||
		(lpDown   != NULL && NULL == m_pBackImgD) ||
		(lpFocus  != NULL && NULL == m_pBackImgF))
		return FALSE;
	else
		return TRUE;
}

//加载资源
BOOL CEasySkinButton::SetCheckImage( LPCTSTR lpNormal, LPCTSTR lpHoven, LPCTSTR lpTickNormal, LPCTSTR lpTickHoven )
{
	UIRenderEngine->RemoveImage(m_pCheckImgN);
	UIRenderEngine->RemoveImage(m_pCheckImgH);
	UIRenderEngine->RemoveImage(m_pCheckImgTickN);
	UIRenderEngine->RemoveImage(m_pCheckImgTickH);

	m_pCheckImgN = UIRenderEngine->GetImage(lpNormal);
	m_pCheckImgH = UIRenderEngine->GetImage(lpHoven);
	m_pCheckImgTickN = UIRenderEngine->GetImage(lpTickNormal);
	m_pCheckImgTickH = UIRenderEngine->GetImage(lpTickHoven);

	if (NULL == m_pCheckImgN || NULL == m_pCheckImgH
		|| NULL == m_pCheckImgTickN || NULL == m_pCheckImgTickH)
		return FALSE;
	else
		return TRUE;
}

//加载资源
BOOL CEasySkinButton::SetIconImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pIconImg);

	m_pIconImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pIconImg)
		return FALSE;
	else
		return TRUE;
}

//加载资源
BOOL CEasySkinButton::SetMenuImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pArrowImg);

	m_pArrowImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pArrowImg)
		return FALSE;
	else
		return TRUE;
}

//设置大小
void CEasySkinButton::SetSize( int nWidth,int nHeight )
{
	SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_NOMOVE);
}

//设置菜单
void CEasySkinButton::SetMenu( HMENU hMenu )
{
	m_hMenu = hMenu;
}


//销毁消息
void CEasySkinButton::OnDestroy()
{
	__super::OnDestroy();

	UIRenderEngine->RemoveImage(m_pBackImgN);
	UIRenderEngine->RemoveImage(m_pBackImgH);
	UIRenderEngine->RemoveImage(m_pBackImgD);
	UIRenderEngine->RemoveImage(m_pBackImgF);

	UIRenderEngine->RemoveImage(m_pCheckImgN);
	UIRenderEngine->RemoveImage(m_pCheckImgH);
	UIRenderEngine->RemoveImage(m_pCheckImgTickN);
	UIRenderEngine->RemoveImage(m_pCheckImgTickH);

	UIRenderEngine->RemoveImage(m_pArrowImg);
	UIRenderEngine->RemoveImage(m_pIconImg);

	m_bFocus = m_bPress = m_bHover = m_bMouseTracking = FALSE;
	m_nBtnType = en_PushButton;
	m_hMenu = NULL;
	m_dwTextAlign = 0;
	m_hBackDC = NULL;
}

//消息循环
LRESULT CEasySkinButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//针对RadioButton组，选择其他Radio时，拦截Check消息
	if ( message == BM_SETCHECK )
	{
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	//鼠标移开按钮之后，会出发此消息，这里需要重新捕获，重绘按钮
	if ( message == BM_SETSTATE )
	{
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}
	
	return __super::DefWindowProc(message, wParam, lParam);
}

//绘制消息
void CEasySkinButton::OnPaint()
{
	CPaintDC dc(this); 

	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(&dc, rcClient);

	//绘制背景
	DrawParentWndBg(GetSafeHwnd(),MemDC->GetSafeHdc());

	switch (m_nBtnType)
	{
	case en_PushButton:
		DrawPushButton(&MemDC,rcClient);
		break;

	case en_CheckButton:
	case en_RadioButton:
		DrawCheckButton(&MemDC,rcClient);
		break;

	case en_IconButton:
		DrawIConButton(&MemDC,rcClient);
		break;

	case en_MenuButton:
		DrawMenuButton(MemDC,rcClient);
		break;
	}
}

//绘制按钮
void CEasySkinButton::DrawPushButton(CDC* pDC,RECT &rcClient)
{
	if (m_bPress)		// 鼠标左键按下状态
	{
		if (m_pBackImgD != NULL && !m_pBackImgD->IsNull())
			m_pBackImgD->DrawImage(pDC, rcClient);
	}
	else if (m_bHover)	// 鼠标悬停状态
	{
		if (m_pBackImgH != NULL && !m_pBackImgH->IsNull())
			m_pBackImgH->DrawImage(pDC, rcClient);
	}
	else if (m_bFocus)	// 焦点状态
	{
		if (m_pBackImgF != NULL && !m_pBackImgF->IsNull())
			m_pBackImgF->DrawImage(pDC, rcClient);
	}
	else				// 普通状态
	{
		if (m_pBackImgN != NULL && !m_pBackImgN->IsNull())
			m_pBackImgN->DrawImage(pDC, rcClient);
	}

	if (m_bPress)
		::OffsetRect(&rcClient,1, 1);

	CString strText;
	GetWindowText(strText);

	BOOL bHasText = FALSE;
	if (strText.GetLength() > 0)
		bHasText = TRUE;

	BOOL bHasIcon = FALSE;
	if (m_pIconImg != NULL && !m_pIconImg->IsNull())
		bHasIcon = TRUE;

	if (bHasIcon && bHasText)	// 带图标和文字
	{
		int cxIcon = m_pIconImg->GetWidth();
		int cyIcon = m_pIconImg->GetHeight();

		int nMode = pDC->SetBkMode(TRANSPARENT);
		
		pDC->SelectObject(GetCtrlFont());

		pDC->SetTextColor(IsWindowEnabled()?m_colNormalText:m_colDisableText);
		CRect rcText(0,0,0,0);	// 计算文字宽高
		pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_CALCRECT);

		int cx = cxIcon+3+rcText.Width();
		int cy = cyIcon;

		CRect rcCenter;
		CalcCenterRect(rcClient, cx, cy, rcCenter);

		CRect rcIcon(rcCenter.left, rcCenter.top, rcCenter.left+cxIcon, rcCenter.bottom);
		m_pIconImg->DrawImage(pDC, rcIcon);

		UINT nFormat = m_dwTextAlign ? m_dwTextAlign : DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
		rcText = CRect(rcIcon.right+3, rcClient.top, rcIcon.right+3+rcText.Width(), rcClient.bottom);
		pDC->DrawText(strText, &rcText, nFormat);

		pDC->SetBkMode(nMode);
	}
	else if (bHasIcon)	// 仅图标
	{
		int cxIcon = m_pIconImg->GetWidth();
		int cyIcon = m_pIconImg->GetHeight();

		CRect rcIcon;
		CalcCenterRect(rcClient, cxIcon, cyIcon, rcIcon);

		m_pIconImg->DrawImage(pDC, rcIcon);
	}
	else if (bHasText)	// 仅文字
	{
		UINT nFormat = m_dwTextAlign ? m_dwTextAlign : DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(GetCtrlFont());
		pDC->SetTextColor(IsWindowEnabled()?m_colNormalText:m_colDisableText);
		pDC->DrawText(strText, &rcClient, nFormat);
		pDC->SetBkMode(nMode);
	}
}

void CEasySkinButton::DrawCheckButton( CDC* pDC,RECT &rcClient )
{
	int nWidth = 15, nHeight = 15;
	if (m_pCheckImgN != NULL && !m_pCheckImgN->IsNull())
	{
		nWidth = m_pCheckImgN->GetWidth();
		nHeight = m_pCheckImgN->GetHeight();
	}

	CRect rcCheck, rcText;

	rcCheck.left = rcClient.left;
	rcCheck.right = rcCheck.left + nWidth;
	rcCheck.top = ((rcClient.bottom-rcClient.top) - nHeight) / 2;
	rcCheck.bottom = rcCheck.top + nHeight;

	rcText = rcClient;
	rcText.left = rcCheck.right + 4;

	BOOL bChecked = ((GetCheck() == BST_CHECKED) ? TRUE : FALSE);

	if (m_bHover)	// 鼠标悬停状态
	{
		if (bChecked)
		{
			if (m_pCheckImgTickH != NULL && !m_pCheckImgTickH->IsNull())
				m_pCheckImgTickH->DrawImage(pDC, rcCheck);
		}
		else
		{
			if (m_pCheckImgH != NULL && !m_pCheckImgH->IsNull())
				m_pCheckImgH->DrawImage(pDC, rcCheck);
		}
	}
	else	// 普通状态
	{
		if (bChecked)
		{
			if (m_pCheckImgTickN != NULL && !m_pCheckImgTickN->IsNull())
				m_pCheckImgTickN->DrawImage(pDC, rcCheck);
		}
		else
		{
			if (m_pCheckImgN != NULL && !m_pCheckImgN->IsNull())
				m_pCheckImgN->DrawImage(pDC, rcCheck);
		}
	}

	CString strText;
	GetWindowText(strText);

	if (strText.GetLength() > 0)
	{
		UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(GetCtrlFont());
		pDC->SetTextColor(IsWindowEnabled()?m_colNormalText:m_colDisableText);
		pDC->DrawText(strText, &rcText, nFormat);
		pDC->SetBkMode(nMode);
	}
}

void CEasySkinButton::DrawIConButton( CDC* pDC,RECT &rcClient )
{
	if (m_bPress)	// 鼠标左键按下状态
	{
		if (m_pBackImgD != NULL && !m_pBackImgD->IsNull())
			m_pBackImgD->Draw(pDC, rcClient);
	}
	else if (m_bHover)	// 鼠标悬停状态
	{
		if (m_pBackImgH != NULL && !m_pBackImgH->IsNull())
			m_pBackImgH->Draw(pDC, rcClient);
	}

	if (m_bPress)
		::OffsetRect(&rcClient,1, 1);

	CString strText;
	GetWindowText(strText);

	BOOL bHasText = FALSE;
	if (strText.GetLength() > 0)
		bHasText = TRUE;

	BOOL bHasIcon = FALSE;
	if (m_pIconImg != NULL && !m_pIconImg->IsNull())
		bHasIcon = TRUE;

	if (bHasIcon && bHasText)	// 带图标和文字
	{
		int cxIcon = m_pIconImg->GetWidth();
		int cyIcon = m_pIconImg->GetHeight();

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(GetCtrlFont());

		CRect rcText(0,0,0,0);	// 计算文字宽高
		pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_CALCRECT);

		int cx = cxIcon+3+rcText.Width();
		int cy = cyIcon;

		CRect rcCenter;
		CalcCenterRect(rcClient, cx, cy, rcCenter);

		CRect rcIcon(rcCenter.left, rcCenter.top, rcCenter.left+cxIcon, rcCenter.bottom);
		m_pIconImg->DrawImage(pDC, rcIcon);

		UINT nFormat = m_dwTextAlign ? m_dwTextAlign : DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
		rcText = CRect(rcIcon.right+3, rcClient.top, rcIcon.right+3+rcText.Width(), rcClient.bottom);
		pDC->SetTextColor(IsWindowEnabled()?m_colNormalText:m_colDisableText);
		pDC->DrawText(strText, &rcText, nFormat);

		pDC->SetBkMode(nMode);	
	}
	else if (bHasIcon)	// 仅图标
	{
		int cxIcon = m_pIconImg->GetWidth();
		int cyIcon = m_pIconImg->GetHeight();

		CRect rcIcon;
		CalcCenterRect(rcClient, cxIcon, cyIcon, rcIcon);

		m_pIconImg->DrawImage(pDC, rcIcon);
	}
	else if (bHasText)	// 仅文字
	{
		CRect rcText(rcClient);
		rcText.left += 2;
		rcText.right -= 2;

		UINT nFormat = m_dwTextAlign ? m_dwTextAlign : DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(GetCtrlFont());
		pDC->SetTextColor(IsWindowEnabled()?m_colNormalText:m_colDisableText);
		pDC->DrawText(strText, &rcText, nFormat);
		pDC->SetBkMode(nMode);	
	}
}

void CEasySkinButton::DrawMenuButton( CDC* pDC,RECT &rcClient )
{
	if (m_bPress)	// 鼠标左键按下状态
	{
		if (m_pBackImgD != NULL && !m_pBackImgD->IsNull())
			m_pBackImgD->DrawImage(pDC, rcClient);
	}
	else if (m_bHover)	// 鼠标悬停状态
	{
		if (m_pBackImgH != NULL && !m_pBackImgH->IsNull())
			m_pBackImgH->DrawImage(pDC, rcClient);
	}

	CRect rcArrow(0, 0, 0, 0);

	if (m_pArrowImg != NULL && !m_pArrowImg->IsNull())
	{
		int cx = m_pArrowImg->GetWidth();
		int cy = m_pArrowImg->GetHeight();
		int x = rcClient.right - 3 - cx;
		int y = (rcClient.bottom-rcClient.top - cy + 1) / 2;
		rcArrow = CRect(x, y, x+cx, y+cy);
		m_pArrowImg->DrawImage(pDC, rcArrow);
	}

	if (m_pIconImg != NULL && !m_pIconImg->IsNull())
	{
		int cx = m_pIconImg->GetWidth();
		int cy = m_pIconImg->GetHeight();

		int nRight;
		if (rcArrow.left > 0)
			nRight = rcArrow.left;
		else
			nRight = rcClient.right;

		CRect rcIcon(rcClient.left, rcClient.top, nRight, rcClient.bottom);
		CalcCenterRect(rcIcon, cx, cy, rcIcon);
		
		if (m_bPress)
			rcIcon.OffsetRect(1, 1);
		m_pIconImg->DrawImage(pDC, rcIcon);
	}
}

void CEasySkinButton::SetPushed( bool bPush )
{
	m_bPushed = bPush;

	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}


// new Add Function [8/28/2015-10:35:58 Dingshuai]	
// 设置文本
void CEasySkinButton::SetWindowText(LPCTSTR lpszString)
{
	CWnd::SetWindowText(lpszString);
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}

void CEasySkinButton::SetPress(BOOL bPress)
{
	m_bPress = bPress;
	m_bCurPushState = bPress;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}

//获取按钮Press状态
BOOL CEasySkinButton::GetPress()
{	
	return m_bPress;
}