/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// EasySkinSliderCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "EasySkinUI.h"
#include "EasySkinSliderCtrl.h"


// CEasySkinSliderCtrl

IMPLEMENT_DYNAMIC(CEasySkinSliderCtrl, CSliderCtrl)

CEasySkinSliderCtrl::CEasySkinSliderCtrl()
{
	m_pBackImg = m_pThumImg = m_pDisImg = m_pBtImg = m_pTraceImg = NULL;
	m_bPress = m_bHover = m_bMouseTracking = FALSE;
	m_bDragging = false;
	m_rcThumRect.SetRect(0,0,0,0);
	m_szThum.SetSize(0,0);
}

CEasySkinSliderCtrl::~CEasySkinSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CEasySkinSliderCtrl, CSliderCtrl)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BOOL CEasySkinSliderCtrl::SetBackImage( LPCTSTR lpszFileName,bool bFixed/* = true*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImg);

	m_pBackImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pBackImg)
		return FALSE;
	else
	{
		if(bFixed) SetWindowPos(NULL,0,0,m_pBackImg->GetWidth(),m_pBackImg->GetHeight(),SWP_NOMOVE);
		
		return TRUE;
	}
}

BOOL CEasySkinSliderCtrl::SetThumImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pThumImg);

	m_pThumImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pThumImg)
		return FALSE;
	else
	{
		m_szThum.SetSize(m_pThumImg->GetWidth(),m_pThumImg->GetHeight());
		m_rcThumRect.SetRect(0,0,m_szThum.cx,m_szThum.cy);
		SetThumbRect();
		SetChannelRect();
		return TRUE;
	}
}

BOOL CEasySkinSliderCtrl::SetDisImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pDisImg);

	m_pDisImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pDisImg)
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinSliderCtrl::SetTraceImage( LPCTSTR lpszFileName,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pTraceImg);

	m_pTraceImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pTraceImg)
		return FALSE;
	else
	{
		m_pTraceImg->SetNinePart(lprcNinePart);
		return TRUE;
	}
}

BOOL CEasySkinSliderCtrl::SetButtonImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pBtImg);

	m_pBtImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pBtImg)
		return FALSE;
	else
	{
		m_szThum.SetSize(m_pBtImg->GetWidth()/3,m_pBtImg->GetHeight());
		m_rcThumRect.SetRect(0,0,m_szThum.cx,m_szThum.cy);
		SetThumbRect();
		SetChannelRect();
		return TRUE;
	}
}

// CEasySkinSliderCtrl 消息处理程序
void CEasySkinSliderCtrl::OnDestroy()
{
	CSliderCtrl::OnDestroy();

	UIRenderEngine->RemoveImage(m_pBackImg);
	UIRenderEngine->RemoveImage(m_pThumImg);
	UIRenderEngine->RemoveImage(m_pDisImg);
	UIRenderEngine->RemoveImage(m_pBtImg);
	UIRenderEngine->RemoveImage(m_pTraceImg);
}

void CEasySkinSliderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(&dc, rcClient);

	//绘制背景
	DrawParentWndBg(GetSafeHwnd(),MemDC->GetSafeHdc());

	//绘制轨迹
	if ( !IsWindowEnabled() )
	{
		if (m_pDisImg != NULL && !m_pDisImg->IsNull())
		{
			m_pDisImg->DrawImage(&MemDC,0,0);
		}		
	}
	else
	{
		if (m_pBackImg != NULL && !m_pBackImg->IsNull())
		{
			m_pBackImg->DrawImage(&MemDC,0,0);
		}
	}

	//绘制痕迹
	if (m_pTraceImg != NULL && !m_pTraceImg->IsNull())
	{
		m_pTraceImg->Draw(&MemDC,CRect(0, 0,m_rcThumRect.left,m_pTraceImg->GetHeight()));
	}

	//绘制滑块
	if (m_pThumImg != NULL && !m_pThumImg->IsNull())
	{
		m_pThumImg->DrawImage(&MemDC,m_rcThumRect.left, m_rcThumRect.top);
	}

	//绘制按钮
	if (m_pBtImg != NULL && !m_pBtImg->IsNull())
	{
		if( m_bPress )
			m_pBtImg->DrawImage(&MemDC,m_rcThumRect.left, m_rcThumRect.top,m_szThum.cx,m_szThum.cy,m_szThum.cx*2,0);
		else if ( m_bHover )
			m_pBtImg->DrawImage(&MemDC,m_rcThumRect.left, m_rcThumRect.top,m_szThum.cx,m_szThum.cy,m_szThum.cx,0);
		else 
			m_pBtImg->DrawImage(&MemDC,m_rcThumRect.left, m_rcThumRect.top,m_szThum.cx,m_szThum.cy,0,0);
	}
}

void CEasySkinSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		int nMax = 0;
		int nMin = 0;
		GetRange(nMin,nMax);
		
		int nPos = 0;
		if ( GetStyle() & TBS_VERT )
			nPos = (nMax - nMin)*(point.y)/m_rcChannelRect.Height(); 
		else 
			nPos = (nMax - nMin)*(point.x)/m_rcChannelRect.Width(); 

		SetPos(nPos);

		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	//按钮事件
	if ( PtInRect(m_rcThumRect,point) && !m_bMouseTracking )
	{
		TrackMouseLeave(GetSafeHwnd());

		m_bMouseTracking = TRUE;
		m_bHover = TRUE;
		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	__super::OnMouseMove(nFlags, point);
}

//鼠标离开消息
LRESULT CEasySkinSliderCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bMouseTracking = FALSE;
	m_bHover = FALSE;
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);

	return 0;
}

void CEasySkinSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( PtInRect(&m_rcThumRect,point) )
	{
		m_bDragging = true;
		m_bPress = TRUE;
		SetCapture();

		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}
 	else
 	{
 		if ( PtInRect(&m_rcChannelRect,point) )
 		{
 			int nMax = 0;  
 			int nMin = 0;  
 			GetRange(nMin,nMax);  
 
			int nPos = 0;

			if ( GetStyle() & TBS_VERT )
 				nPos = (nMax - nMin)*(point.y - m_rcChannelRect.top)/m_rcChannelRect.Height(); 
			else 
				nPos = (nMax - nMin)*(point.x - m_rcChannelRect.left)/m_rcChannelRect.Width(); 
 
 			OutputString(TEXT("Down:%d\n"),nPos);
 			nPos += nMin;
 			SetPos(nPos); 
 			RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
 			return;
 		}
 	}

	__super::OnLButtonDown(nFlags, point);
}

void CEasySkinSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( PtInRect(&m_rcThumRect,point) && m_bPress) m_bPress = FALSE;

	m_bDragging = false;
	ReleaseCapture();
	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	
	__super::OnLButtonUp(nFlags, point);
}

BOOL CEasySkinSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CEasySkinSliderCtrl::SetThumbRect(int nPos)
{	
	CRect rcClient;
	GetClientRect(&rcClient);

	//判断是否横竖控件
	if ( GetStyle() & TBS_VERT )
	{
		m_rcThumRect.left = (rcClient.Width()-m_szThum.cx)/2;
		m_rcThumRect.top = nPos;
	}
	else
	{
		m_rcThumRect.left = nPos;
		m_rcThumRect.top = (rcClient.Height()-m_szThum.cy)/2;
	}

	m_rcThumRect.right = m_rcThumRect.left+m_szThum.cx;
	m_rcThumRect.bottom = m_rcThumRect.top+m_szThum.cy;
}

void CEasySkinSliderCtrl::SetPos( int nPos )
{
	int nMin,nMax;
	GetRange(nMin,nMax);

	//重新定位，防止超出客户区
	if( nPos <= nMin ) nPos = nMin;
	if ( nPos >= nMax ) nPos = nMax;
	
	__super::SetPos(nPos);

	int nPosNew = 0;

	if ( GetStyle() & TBS_VERT )

		nPosNew = m_rcChannelRect.Height()*nPos/(nMax-nMin);
	else 
		nPosNew = m_rcChannelRect.Width()*nPos/(nMax-nMin);
	
	if( nPosNew<0 ) nPosNew = 0;

	SetThumbRect(nPosNew);

	RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
}

void CEasySkinSliderCtrl::SetChannelRect()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	//设置轨迹区域
	if ( GetStyle() & TBS_VERT )
		m_rcChannelRect.SetRect(0,m_szThum.cy/2,rcClient.Width(),rcClient.Height()-m_szThum.cy/2);
	else 
		m_rcChannelRect.SetRect(m_szThum.cx/2,0,rcClient.Width()-m_szThum.cx/2,rcClient.Height());
}
