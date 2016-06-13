/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// EasySkinProgressCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "EasySkinUI.h"
#include "EasySkinProgressCtrl.h"


// CEasySkinProgressCtrl

IMPLEMENT_DYNAMIC(CEasySkinProgressCtrl, CProgressCtrl)

CEasySkinProgressCtrl::CEasySkinProgressCtrl()
{
	m_pBackImg = m_pStepImg = m_pFloatImg = NULL;
	m_nFloatPos = 0;
	m_szFloatsize.SetSize(0,0);
}

CEasySkinProgressCtrl::~CEasySkinProgressCtrl()
{
}


BEGIN_MESSAGE_MAP(CEasySkinProgressCtrl, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()



// CEasySkinProgressCtrl 消息处理程序



void CEasySkinProgressCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient,rcStep;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(&dc, rcClient);

	//绘制背景
	DrawParentWndBg(GetSafeHwnd(),MemDC->GetSafeHdc());

	if (m_pBackImg != NULL && !m_pBackImg->IsNull())
	{
		m_pBackImg->DrawImage(&MemDC,0,0);
	}

	if (m_pStepImg != NULL && !m_pStepImg->IsNull())
	{
		rcStep.CopyRect(&rcClient);

		int nMin,nMax;
		GetRange(nMin,nMax);

		if ( GetStyle() & PBS_VERTICAL )
		{
			rcStep.bottom=rcStep.Height()*GetPos()/(nMax-nMin);
			m_pStepImg->DrawImage(&MemDC,0,rcClient.Height()-rcStep.Height(),rcStep.Width(),rcStep.Height(),0,m_pStepImg->GetHeight()-(m_pStepImg->GetHeight()*GetPos()/(nMax-nMin)),m_pStepImg->GetWidth(),m_pStepImg->GetHeight()*GetPos()/(nMax-nMin));
		}
		else
		{
			rcStep.right=rcStep.Width()*GetPos()/(nMax-nMin);
			m_pStepImg->DrawImage(&MemDC,0,0,rcStep.Width(),rcStep.Height(),0,0,m_pStepImg->GetWidth(),m_pStepImg->GetHeight()*GetPos()/(nMax-nMin));
		}
	}

	if (m_pFloatImg != NULL && !m_pFloatImg->IsNull())
	{
		if ( GetStyle() & PBS_VERTICAL )
			m_pFloatImg->DrawImage(&MemDC,0,rcClient.Height()-m_nFloatPos,rcClient.Width(),m_pFloatImg->GetHeight());
		else
			m_pFloatImg->DrawImage(&MemDC,m_nFloatPos,0,m_pFloatImg->GetWidth(),rcClient.Height());
	}

}

BOOL CEasySkinProgressCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CEasySkinProgressCtrl::SetBackImage( LPCTSTR lpszFileName,bool bFixed /*= true*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImg);

	m_pBackImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pBackImg)
		return FALSE;
	else
	{
		if(bFixed) SetWindowPos(NULL,0,0,m_pBackImg->GetWidth()+2,m_pBackImg->GetHeight()+2,SWP_NOMOVE);

		return TRUE;
	}
}

void CEasySkinProgressCtrl::OnDestroy()
{
	__super::OnDestroy();

	UIRenderEngine->RemoveImage(m_pBackImg);
	UIRenderEngine->RemoveImage(m_pStepImg);
	UIRenderEngine->RemoveImage(m_pFloatImg);
}

BOOL CEasySkinProgressCtrl::SetStepImage( LPCTSTR lpszFileName)
{
	UIRenderEngine->RemoveImage(m_pStepImg);

	m_pStepImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pStepImg)
		return FALSE;
	else
	{
		return TRUE;
	}
}

BOOL CEasySkinProgressCtrl::SetFloatImage( LPCTSTR lpszFileName,int nTime/*=100*/ )
{
	UIRenderEngine->RemoveImage(m_pFloatImg);

	m_pFloatImg = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pFloatImg)
		return FALSE;
	else
	{
		if ( GetSafeHwnd() != NULL )
		{
			m_szFloatsize.SetSize(m_pFloatImg->GetWidth(),m_pFloatImg->GetHeight());
			SetTimer(0,nTime,NULL);
		}
		return TRUE;
	}
}

void CEasySkinProgressCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 0 )
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		int nMin,nMax;
		GetRange(nMin,nMax);

		int nFloat=0;
		
		if ( GetStyle() & PBS_VERTICAL )
			nFloat = rcClient.Height()*GetPos()/(nMax-nMin);
		else
			nFloat = rcClient.Width()*GetPos()/(nMax-nMin);

		if ( m_nFloatPos > nFloat) 
		{
			if ( GetStyle() & PBS_VERTICAL )
				m_nFloatPos = m_szFloatsize.cy*(-1);
			else
				m_nFloatPos = m_szFloatsize.cx*(-1);
		}
		else m_nFloatPos+=5;

		RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	__super::OnTimer(nIDEvent);
}

void CEasySkinProgressCtrl::OnNcPaint()
{
	return;
}
