/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// EasySkinTabCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "EasySkinTabCtrl.h"


// CEasySkinTabCtrl

IMPLEMENT_DYNAMIC(CEasySkinTabCtrl, CTabCtrl)

CEasySkinTabCtrlItem::CEasySkinTabCtrlItem(void)
{
	m_nID = 0;
	m_dwStyle = STCI_STYLE_BUTTON;
	m_nWidth = m_nHeight = 0;
	m_nLeftWidth = m_nRightWidth = 0;
	m_nPadding = 0;
	m_strText = _T("");
	m_strToolTipText = _T("");
	m_lpBgImgN = m_lpBgImgH = m_lpBgImgD = NULL;
	m_lpArrowImgH = m_lpArrowImgD = NULL;
	m_lpIconImg = m_lpSelIconImg = NULL;
}

CEasySkinTabCtrlItem::~CEasySkinTabCtrlItem(void)
{
	UIRenderEngine->RemoveImage(m_lpBgImgN);
	UIRenderEngine->RemoveImage(m_lpBgImgH);
	UIRenderEngine->RemoveImage(m_lpBgImgD);
	UIRenderEngine->RemoveImage(m_lpArrowImgH);
	UIRenderEngine->RemoveImage(m_lpArrowImgD);
	UIRenderEngine->RemoveImage(m_lpIconImg);
	UIRenderEngine->RemoveImage(m_lpSelIconImg);
}

CEasySkinTabCtrl::CEasySkinTabCtrl()
{
	m_lpBack = NULL;
	m_lpItemBgImgN = m_lpItemBgImgH = m_lpItemBgImgD = NULL;
	m_lpArrowImgH = m_lpArrowImgD = NULL;
	m_nSelIndex = m_nHoverIndex = -1;
	m_bPressArrow = FALSE;
	m_bMouseTracking = FALSE;
	m_nLeft = m_nTop = 0;
	m_bTransparent = FALSE;
}

CEasySkinTabCtrl::~CEasySkinTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CEasySkinTabCtrl, CTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CEasySkinTabCtrl 消息处理程序

void CEasySkinTabCtrl::SetLeftTop(int nLeft, int nTop)
{
	m_nLeft = nLeft;
	m_nTop = nTop;
}

BOOL CEasySkinTabCtrl::SetBackImage(LPCTSTR lpszFileName, CONST LPRECT lprcNinePart)
{
	UIRenderEngine->RemoveImage(m_lpBack);

	m_lpBack = UIRenderEngine->GetImage(lpszFileName);

	if (m_lpBack != NULL)
		m_lpBack->SetNinePart(lprcNinePart);

	if (NULL == m_lpBack)
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinTabCtrl::SetItemsImage(LPCTSTR lpNormal, LPCTSTR lpHighlight,LPCTSTR lpDown, CONST LPRECT lprcNinePart)
{
	UIRenderEngine->RemoveImage(m_lpItemBgImgN);
	UIRenderEngine->RemoveImage(m_lpItemBgImgH);
	UIRenderEngine->RemoveImage(m_lpItemBgImgD);

	m_lpItemBgImgN = UIRenderEngine->GetImage(lpNormal);
	m_lpItemBgImgH = UIRenderEngine->GetImage(lpHighlight);
	m_lpItemBgImgD = UIRenderEngine->GetImage(lpDown);

	if (m_lpItemBgImgN != NULL)
		m_lpItemBgImgN->SetNinePart(lprcNinePart);

	if (m_lpItemBgImgH != NULL)
		m_lpItemBgImgH->SetNinePart(lprcNinePart);

	if (m_lpItemBgImgD != NULL)
		m_lpItemBgImgD->SetNinePart(lprcNinePart);

	if ((lpNormal != NULL && NULL == m_lpItemBgImgN) || 
		(lpHighlight != NULL && NULL == m_lpItemBgImgH) ||
		(lpDown != NULL && NULL == m_lpItemBgImgD))
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinTabCtrl::SetItemsArrowImage(LPCTSTR lpHighlight, LPCTSTR lpDown)
{
	UIRenderEngine->RemoveImage(m_lpArrowImgH);
	UIRenderEngine->RemoveImage(m_lpArrowImgD);

	m_lpArrowImgH = UIRenderEngine->GetImage(lpHighlight);
	m_lpArrowImgD = UIRenderEngine->GetImage(lpDown);

	if ((lpHighlight != NULL && NULL == m_lpArrowImgH) ||
		(lpDown != NULL && NULL == m_lpArrowImgD))
		return FALSE;
	else
		return TRUE;
}

int CEasySkinTabCtrl::GetItemCount()
{
	return m_ItemArray.size();
}

int CEasySkinTabCtrl::GetCurSel()
{
	return m_nSelIndex;
}

void CEasySkinTabCtrl::SetCurSel(int nCurSel)
{
	m_nSelIndex = nCurSel;

	Invalidate(FALSE);
}

int CEasySkinTabCtrl::AddItem(int nID, DWORD dwStyle)
{
	CEasySkinTabCtrlItem * lpItem = new CEasySkinTabCtrlItem;
	if (NULL == lpItem)
		return -1;
	lpItem->m_nID = nID;
	lpItem->m_dwStyle = dwStyle;
	m_ItemArray.push_back(lpItem);

	__super::InsertItem(nID,TEXT(""));
	
	SetItemPadding(nID,1);

	return m_ItemArray.size() - 1;
}

void CEasySkinTabCtrl::SetItemID(int nIndex, int nID)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (lpItem != NULL)
		lpItem->m_nID = nID;
}

void CEasySkinTabCtrl::SetItemStyle(int nIndex, int dwStyle)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (lpItem != NULL)
		lpItem->m_dwStyle = dwStyle;
}

CSize CEasySkinTabCtrl::SetItemSize(CSize size,CONST LPSIZE lpszArrow/*=NULL*/)
{
	for (int i=0;i<m_ItemArray.size();i++)
	{
		CEasySkinTabCtrlItem * lpItem = m_ItemArray.at(i);
		lpItem->m_nWidth = size.cx;
		lpItem->m_nHeight = size.cy;
		lpItem->m_nLeftWidth = lpszArrow ? lpszArrow->cx : 0;
		lpItem->m_nRightWidth = lpszArrow ? lpszArrow->cy : 0;
	}

	return __super::SetItemSize(size);
}

void CEasySkinTabCtrl::SetItemPadding(int nIndex, int nPadding)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (lpItem != NULL)
		lpItem->m_nPadding = nPadding;
}

void CEasySkinTabCtrl::SetItemText(int nIndex, LPCTSTR lpszText)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (lpItem != NULL)
		lpItem->m_strText = lpszText;
}

void CEasySkinTabCtrl::SetItemToolTipText(int nIndex, LPCTSTR lpszText)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (lpItem != NULL)
		lpItem->m_strToolTipText = lpszText;
}

BOOL CEasySkinTabCtrl::SetItemBack(int nIndex, LPCTSTR lpNormal, 
								LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (NULL == lpItem)
		return FALSE;

	UIRenderEngine->RemoveImage(lpItem->m_lpBgImgN);
	UIRenderEngine->RemoveImage(lpItem->m_lpBgImgH);
	UIRenderEngine->RemoveImage(lpItem->m_lpBgImgD);

	lpItem->m_lpBgImgN = UIRenderEngine->GetImage(lpNormal);
	lpItem->m_lpBgImgH = UIRenderEngine->GetImage(lpHighlight);
	lpItem->m_lpBgImgD = UIRenderEngine->GetImage(lpDown);

	if (lpItem->m_lpBgImgN != NULL)
		lpItem->m_lpBgImgN->SetNinePart(lprcNinePart);

	if (lpItem->m_lpBgImgH != NULL)
		lpItem->m_lpBgImgH->SetNinePart(lprcNinePart);

	if (lpItem->m_lpBgImgD != NULL)
		lpItem->m_lpBgImgD->SetNinePart(lprcNinePart);

	if ((lpNormal != NULL && NULL == lpItem->m_lpBgImgN) || 
		(lpHighlight != NULL && NULL == lpItem->m_lpBgImgH) ||
		(lpDown != NULL && NULL == lpItem->m_lpBgImgD))
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinTabCtrl::SetItemArrowImage(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (NULL == lpItem)
		return FALSE;

	UIRenderEngine->RemoveImage(lpItem->m_lpArrowImgH);
	UIRenderEngine->RemoveImage(lpItem->m_lpArrowImgD);

	lpItem->m_lpArrowImgH = UIRenderEngine->GetImage(lpHighlight);
	lpItem->m_lpArrowImgD = UIRenderEngine->GetImage(lpDown);

	if ((lpHighlight != NULL && NULL == lpItem->m_lpArrowImgH) ||
		(lpDown != NULL && NULL == lpItem->m_lpArrowImgD))
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinTabCtrl::SetItemIconImage(int nIndex, LPCTSTR lpIcon, LPCTSTR lpSelIcon)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (NULL == lpItem)
		return FALSE;

	UIRenderEngine->RemoveImage(lpItem->m_lpIconImg);
	UIRenderEngine->RemoveImage(lpItem->m_lpSelIconImg);

	lpItem->m_lpIconImg = UIRenderEngine->GetImage(lpIcon);
	lpItem->m_lpSelIconImg = UIRenderEngine->GetImage(lpSelIcon);

	if ((lpIcon != NULL && NULL == lpItem->m_lpIconImg) ||
		(lpSelIcon != NULL && NULL == lpItem->m_lpSelIconImg))
		return FALSE;
	else
		return TRUE;
}

BOOL CEasySkinTabCtrl::GetItemRectByIndex(int nIndex, CRect& rect)
{
	CEasySkinTabCtrlItem * lpItem;
	int nLeft = m_nLeft, nTop = m_nTop;

	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		lpItem = m_ItemArray[i];
		if (lpItem != NULL)
		{
			if (i == nIndex)
			{
				rect = CRect(nLeft, nTop, nLeft+lpItem->m_nWidth, nTop+lpItem->m_nHeight);
				return TRUE;
			}
			nLeft += lpItem->m_nWidth;
			nLeft += lpItem->m_nPadding;
		}
	}

	return FALSE;
}

BOOL CEasySkinTabCtrl::GetItemRectByID(int nID, CRect& rect)
{
	CEasySkinTabCtrlItem * lpItem;
	int nLeft = m_nLeft, nTop = m_nTop;

	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		lpItem = m_ItemArray[i];
		if (lpItem != NULL)
		{
			if (lpItem->m_nID == nID)
			{
				rect = CRect(nLeft, nTop, nLeft+lpItem->m_nWidth, nTop+lpItem->m_nHeight);
				return TRUE;
			}
			nLeft += lpItem->m_nWidth;
			nLeft += lpItem->m_nPadding;
		}
	}

	return FALSE;
}


void CEasySkinTabCtrl::DrawItem(CDC*pDC, int nIndex)
{
	CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
	if (NULL == lpItem) return;

	CRect rcItem;
	GetItemRectByIndex(nIndex, rcItem);

	if (m_nSelIndex == nIndex)
	{
		if (lpItem->m_lpBgImgD != NULL && !lpItem->m_lpBgImgD->IsNull())
			lpItem->m_lpBgImgD->Draw(pDC, rcItem);
		else if (m_lpItemBgImgD != NULL && !m_lpItemBgImgD->IsNull())
			m_lpItemBgImgD->Draw(pDC, rcItem);
	}
	else if (m_nHoverIndex == nIndex)
	{
		if (lpItem->m_lpBgImgH != NULL && !lpItem->m_lpBgImgH->IsNull())
			lpItem->m_lpBgImgH->Draw(pDC, rcItem);
		else if (m_lpItemBgImgH != NULL && !m_lpItemBgImgH->IsNull())
			m_lpItemBgImgH->Draw(pDC, rcItem);
	}
	else
	{
		if (lpItem->m_lpBgImgN != NULL && !lpItem->m_lpBgImgN->IsNull())
			lpItem->m_lpBgImgN->Draw(pDC, rcItem);
		else if (m_lpItemBgImgN != NULL && !m_lpItemBgImgN->IsNull())
			m_lpItemBgImgN->Draw(pDC, rcItem);
	}

	CImageEx * lpIconImg = NULL;

	if (m_nSelIndex == nIndex)
		lpIconImg = lpItem->m_lpSelIconImg;
	else
		lpIconImg = lpItem->m_lpIconImg;

	BOOL bHasText = FALSE;
	if (lpItem->m_strText.GetLength() > 0)
		bHasText = TRUE;

	BOOL bHasIcon = FALSE;
	if (lpIconImg != NULL && !lpIconImg->IsNull())
		bHasIcon = TRUE;

	if (bHasIcon && bHasText)	// 带图标和文字
	{
		int cxIcon = lpIconImg->GetWidth();
		int cyIcon = lpIconImg->GetHeight();

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(GetCtrlFont());
		pDC->SetTextColor(m_colNormalText);

		CRect rcText(0,0,0,0);	// 计算文字宽高
		pDC->DrawText(lpItem->m_strText, lpItem->m_strText.GetLength(), &rcText, DT_SINGLELINE | DT_CALCRECT);

		int cx = cxIcon+1+rcText.Width();
		int cy = cyIcon;

		CRect rcCenter;
		CalcCenterRect(rcItem, cx, cy, rcCenter);

		CRect rcIcon(rcCenter.left, rcCenter.top, rcCenter.left+cxIcon, rcCenter.bottom);
		lpIconImg->Draw(pDC, rcIcon);

		UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
		rcText = CRect(rcIcon.right+1, rcItem.top, rcIcon.right+1+rcText.Width(), rcItem.bottom);
		pDC->DrawText(lpItem->m_strText, lpItem->m_strText.GetLength(), &rcText, nFormat);

		pDC->SetBkMode(nMode);
	}
	else if (bHasIcon)	// 仅图标
	{
		int cxIcon = lpIconImg->GetWidth();
		int cyIcon = lpIconImg->GetHeight();

		CRect rcIcon;
		CalcCenterRect(rcItem, cxIcon, cyIcon, rcIcon);

		lpIconImg->Draw(pDC, rcIcon);
	}
	else if (bHasText)	// 仅文字
	{
		UINT nFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		int nMode = pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_colNormalText);
		pDC->SelectObject(GetCtrlFont());

		pDC->DrawText(lpItem->m_strText, lpItem->m_strText.GetLength(), &rcItem, nFormat);
		pDC->SetBkMode(nMode);
	}

	if (m_nSelIndex == nIndex)
	{
		if (lpItem->m_dwStyle & STCI_STYLE_DROPDOWN)
		{
			CRect rcArrow;
			rcArrow.left = rcItem.left+lpItem->m_nLeftWidth;
			rcArrow.top = rcItem.top;
			rcArrow.bottom = rcItem.bottom;
			rcArrow.right = rcArrow.left + lpItem->m_nRightWidth;

			if (m_bPressArrow)
			{
				if (lpItem->m_lpArrowImgD != NULL && !lpItem->m_lpArrowImgD->IsNull())
					lpItem->m_lpArrowImgD->Draw(pDC, rcArrow);
				else if (m_lpArrowImgD != NULL && !m_lpArrowImgD->IsNull())
					m_lpArrowImgD->Draw(pDC, rcArrow);
			}
			else if (m_nHoverIndex == nIndex)
			{
				if (lpItem->m_lpArrowImgH != NULL && !lpItem->m_lpArrowImgH->IsNull())
					lpItem->m_lpArrowImgH->Draw(pDC, rcArrow);
				else if (m_lpArrowImgH != NULL && !m_lpArrowImgH->IsNull())
					m_lpArrowImgH->Draw(pDC, rcArrow);
			}
		}
	}
}

BOOL CEasySkinTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CEasySkinTabCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC BufferDC(&dc, rcClient);

	DrawParentWndBg(GetSafeHwnd(),BufferDC.m_hDC);

	if (m_lpBack != NULL && !m_lpBack->IsNull())
		m_lpBack->Draw(&BufferDC, rcClient);

	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		DrawItem(&BufferDC, i);
	}
}

void CEasySkinTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	int nIndex = HitTest(point);
	if (nIndex != -1)
	{
		if (nIndex != m_nSelIndex)
		{
			m_nSelIndex = nIndex;
			Invalidate(FALSE);

			NMHDR nmhdr = {m_hWnd, GetDlgCtrlID(), TCN_SELCHANGE};
			::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmhdr);
		}
		else
		{
			CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
			if (lpItem != NULL && lpItem->m_dwStyle & STCI_STYLE_DROPDOWN)
			{
				CRect rcItem;
				GetItemRectByIndex(nIndex, rcItem);

				CRect rcArrow;
				rcArrow.left = rcItem.left+lpItem->m_nLeftWidth;
				rcArrow.right = rcArrow.left + lpItem->m_nRightWidth;
				rcArrow.top = rcItem.top;
				rcArrow.bottom = rcItem.bottom;

				if (rcArrow.PtInRect(point))
					m_bPressArrow = TRUE;
				else
					m_bPressArrow = FALSE;

				if (m_bPressArrow)
				{
					Invalidate(FALSE);

					NMHDR nmhdr = {0};
					nmhdr.hwndFrom = m_hWnd;
					nmhdr.idFrom = GetDlgCtrlID();
					nmhdr.code = TCN_DROPDOWN;
					::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&nmhdr);

					m_bPressArrow = FALSE;
					Invalidate(FALSE);
				}
			}
		}
	}

	__super::OnLButtonDown(nFlags, point);
}

void CEasySkinTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TrackMouseLeave(GetSafeHwnd());
		m_bMouseTracking = TRUE;
	}

	int nIndex;

	nIndex = HitTest(point);
	if (nIndex != -1)
	{
		CEasySkinTabCtrlItem * lpItem = GetItemByIndex(nIndex);
		if (lpItem != NULL)
		{
			CRect rcItem;
			GetItemRectByIndex(nIndex, rcItem);

			if (nIndex != m_nHoverIndex && lpItem->m_strToolTipText.GetLength() > 0)
			{
				if (!m_ToolTipCtrl.GetSafeHwnd())
				{
					m_ToolTipCtrl.Create(this);
					m_ToolTipCtrl.SetMaxTipWidth(200);
				}

				if (m_ToolTipCtrl.GetSafeHwnd())
				{
					if (m_ToolTipCtrl.GetToolCount() <= 0)
					{
						m_ToolTipCtrl.Activate(TRUE);
						m_ToolTipCtrl.AddTool(this, lpItem->m_strToolTipText.GetBuffer(), &rcItem, 1);
					}
					else
					{
						m_ToolTipCtrl.Activate(TRUE);
						m_ToolTipCtrl.UpdateTipText(lpItem->m_strToolTipText.GetBuffer(), this, 1);
						m_ToolTipCtrl.SetToolRect(this, 1, &rcItem);
					}
				}
			}
		}
	}

	if (nIndex != m_nHoverIndex)
	{
		m_nHoverIndex = nIndex;
		Invalidate();
	}

	__super::OnMouseMove(nFlags, point);
}

LRESULT CEasySkinTabCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bMouseTracking = FALSE;
	int nIndex = -1;
	if (nIndex != m_nHoverIndex)
	{
		m_nHoverIndex = nIndex;
		Invalidate();
	}

	return 0;
}

void CEasySkinTabCtrl::OnDestroy()
{
	__super::OnDestroy();

	if (m_ToolTipCtrl.GetSafeHwnd())
		m_ToolTipCtrl.DestroyWindow();

	m_ToolTipCtrl.m_hWnd = NULL;

	CEasySkinTabCtrlItem * lpItem;
	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		lpItem = m_ItemArray[i];
		if (lpItem != NULL)
			delete lpItem;
	}
	m_ItemArray.clear();

	UIRenderEngine->RemoveImage(m_lpBack);
	UIRenderEngine->RemoveImage(m_lpItemBgImgN);
	UIRenderEngine->RemoveImage(m_lpItemBgImgH);
	UIRenderEngine->RemoveImage(m_lpItemBgImgD);
	UIRenderEngine->RemoveImage(m_lpArrowImgH);
	UIRenderEngine->RemoveImage(m_lpArrowImgD);

	m_nSelIndex = m_nHoverIndex = -1;
	m_bPressArrow = FALSE;
	m_bMouseTracking = FALSE;
	m_nLeft = m_nTop = 0;
	m_bTransparent = FALSE;
}

BOOL CEasySkinTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_ToolTipCtrl.GetSafeHwnd())
		m_ToolTipCtrl.RelayEvent(pMsg);

	return __super::PreTranslateMessage(pMsg);
}

CEasySkinTabCtrlItem * CEasySkinTabCtrl::GetItemByIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_ItemArray.size())
		return m_ItemArray[nIndex];
	else
		return NULL;
}

CEasySkinTabCtrlItem * CEasySkinTabCtrl::GetItemByID(int nID)
{
	CEasySkinTabCtrlItem * lpItem;
	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		lpItem = m_ItemArray[i];
		if (lpItem != NULL && lpItem->m_nID == nID)
			return lpItem;
	}

	return NULL;
}

int CEasySkinTabCtrl::HitTest(POINT pt)
{
	CEasySkinTabCtrlItem * lpItem;
	int nLeft = m_nLeft, nTop = m_nTop;
	CRect rcItem;

	for (int i = 0; i < (int)m_ItemArray.size(); i++)
	{
		lpItem = m_ItemArray[i];
		if (lpItem != NULL)
		{
			rcItem = CRect(nLeft, nTop, nLeft+lpItem->m_nWidth, nTop+lpItem->m_nHeight);
			if (rcItem.PtInRect(pt))
				return i;
			nLeft += lpItem->m_nWidth;
			nLeft += lpItem->m_nPadding;
		}
	}

	return -1;
}