/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "stdafx.h"
#include "MemoryDC.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMemoryDC::CMemoryDC( CDC* pDC, CRect rect /*= CRect(0,0,0,0)*/, BOOL bCopyFirst /*= FALSE*/ ): CDC(), m_oldBitmap(NULL), m_pDC(pDC)
{
	ASSERT(m_pDC != NULL);			// 断言参数不为空

	m_bMemDC = !pDC->IsPrinting();

	if (m_bMemDC)
	{
		//创建一个内存DC
		CreateCompatibleDC(pDC);
		if ( rect == CRect(0,0,0,0) )
		{
			pDC->GetClipBox(&m_rect);
		}
		else
		{
			m_rect = rect;
		}
		//创建兼容位图
		m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
		//保留旧位图对象
		m_oldBitmap = SelectObject(&m_bitmap);
		//移动窗口原点
		SetWindowOrg(m_rect.left, m_rect.top);
		//初次复制
		if(bCopyFirst)
		{
			this->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),m_pDC, m_rect.left, m_rect.top, SRCCOPY);
		}
	} 
	else
	{
		//创建一个跟当前DC绘制有关的副本
		m_bPrinting = pDC->m_bPrinting;
		m_hDC = pDC->m_hDC;
		m_hAttribDC = pDC->m_hAttribDC;
	}
}

CMemoryDC::~CMemoryDC()
{
	if (m_bMemDC) 
	{
		//复制屏幕外位图到屏幕内
		m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),	this, m_rect.left, m_rect.top, SRCCOPY);
		//交换原来的位图
		SelectObject(m_oldBitmap);
	} 
	else
	{
		m_hDC = m_hAttribDC = NULL;
	}
}

CMemoryDC* CMemoryDC::operator->()
{
	return this;
}

CMemoryDC::operator CMemoryDC*()
{
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

