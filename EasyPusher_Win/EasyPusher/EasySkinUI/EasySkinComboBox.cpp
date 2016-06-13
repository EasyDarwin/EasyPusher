/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
// EasySkinComboBox.cpp : 实现文件
//

#include "stdafx.h"
#include "Control.h"
#include "EasySkinComboBox.h"

static WNDPROC m_pWndProc=0;

extern "C" LRESULT FAR PASCAL ComboBoxListBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return CallWindowProc(m_pWndProc, hWnd, nMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEasySkinComboBox_Edit, CEdit)
 	ON_WM_ERASEBKGND()
 	ON_WM_MOUSEMOVE()
 	ON_WM_MOUSELEAVE()
 	ON_WM_SETFOCUS()
 	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


CEasySkinComboBox_Edit::CEasySkinComboBox_Edit( void )
{
	m_hOwnerWnd = NULL;
	m_bMouseTracking = FALSE;
	m_bIsDefText = FALSE;
}

CEasySkinComboBox_Edit::~CEasySkinComboBox_Edit( void )
{
}

void CEasySkinComboBox_Edit::SetOwnerWnd( HWND hWnd )
{
	m_hOwnerWnd = hWnd;
}

void CEasySkinComboBox_Edit::SetDefaultText( LPCTSTR lpszText )
{
	m_strDefText = lpszText;
}

BOOL CEasySkinComboBox_Edit::IsDefaultText()
{
	return m_bIsDefText;
}

void CEasySkinComboBox_Edit::SetDefaultTextMode( BOOL bIsDefText )
{
	if (bIsDefText == m_bIsDefText)
		return;

	m_bIsDefText = bIsDefText;
	if (m_bIsDefText)
	{
		SetWindowText(m_strDefText);
	}
	else
	{
		SetWindowText(_T(""));
	}
}

BOOL CEasySkinComboBox_Edit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CEasySkinComboBox_Edit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		m_bMouseTracking = TRUE;
		TrackMouseLeave(GetSafeHwnd());

		if (::IsWindow(m_hOwnerWnd))
			::SendMessage(m_hOwnerWnd, WM_CBO_EDIT_MOUSE_HOVER, 0, 0);
	}

	__super::OnMouseMove(nFlags, point);
}

void CEasySkinComboBox_Edit::OnMouseLeave()
{
	m_bMouseTracking = FALSE;

	if (::IsWindow(m_hOwnerWnd))
		::SendMessage(m_hOwnerWnd, WM_CBO_EDIT_MOUSE_LEAVE, 0, 0);

	__super::OnMouseLeave();
}

void CEasySkinComboBox_Edit::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);

	if (m_bIsDefText)
	{
		m_bIsDefText = FALSE;
		SetWindowText(TEXT(""));
	}

	//将焦点返回到原焦点
	if ( GetStyle() & ES_READONLY )
	{
		if( pOldWnd != NULL && pOldWnd->GetSafeHwnd() != NULL )
			pOldWnd->SetFocus();  
	}
}

void CEasySkinComboBox_Edit::OnLButtonDown(UINT nFlags, CPoint point)
{
	//将焦点返回到原焦点
	if ( GetStyle() & ES_READONLY )
	{
		if (::IsWindow(m_hOwnerWnd))
			::SendMessage(m_hOwnerWnd, WM_LBUTTONDOWN,0,0);

		return;
	}

	__super::OnLButtonDown(nFlags, point);
}

void CEasySkinComboBox_Edit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//将焦点返回到原焦点
	if ( GetStyle() & ES_READONLY )
	{
		return;
	}

	__super::OnLButtonDblClk(nFlags, point);
}

LRESULT CEasySkinComboBox_Edit::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//如果只读属性，且有选中消息则过滤掉
	if ( ( GetStyle() & ES_READONLY ) && (message == EM_SETSEL) )
	{
		return TRUE;
	}

	return __super::DefWindowProc(message, wParam, lParam);
}

BOOL CEasySkinComboBox_Edit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//将焦点返回到原焦点
	if ( GetStyle() & ES_READONLY )
	{
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd, nHitTest, message);
}


void CEasySkinComboBox_Edit::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	if (GetWindowTextLength() <= 0 && !m_strDefText.IsEmpty())
	{
		m_bIsDefText = TRUE;
		SetWindowText(m_strDefText);
	}
}

HBRUSH CEasySkinComboBox_Edit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	bool bIsReadOnly = ((GetStyle() & ES_READONLY) != 0) ? true : false;
 
 	if (m_bIsDefText)
 	{
 		if( bIsReadOnly )
 			pDC->SetTextColor(m_colReadOnlyText);
 		else pDC->SetTextColor(m_colDefText);
 	}
 	else
 	{
 		if( bIsReadOnly )
 			pDC->SetTextColor(m_colReadOnlyText);
 		else pDC->SetTextColor(m_colNormalText);
 	}

	return (HBRUSH)NULL_BRUSH;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEasySkinComboBox_ListBox, CListBox)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CEasySkinComboBox_ListBox::CEasySkinComboBox_ListBox( void )
{
	m_hOwnerWnd = NULL;
	m_pBackImgN = NULL;
	m_pSelectImg = NULL;
}

CEasySkinComboBox_ListBox::~CEasySkinComboBox_ListBox( void )
{

}

void CEasySkinComboBox_ListBox::SetOwnerWnd( HWND hWnd )
{
	m_hOwnerWnd = hWnd;
}

void CEasySkinComboBox_ListBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListBox::OnShowWindow(bShow, nStatus);

	if (!bShow)
	{
		::SendMessage(m_hOwnerWnd, WM_CBO_LIST_HIDE, 0, 0);
	}
}

BOOL CEasySkinComboBox_ListBox::SetBackNormalImage( LPCTSTR lpszFileName, CONST LPRECT lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pBackImgN);
	m_pBackImgN = UIRenderEngine->GetImage(lpszFileName);

	if (m_pBackImgN != NULL)
		m_pBackImgN->SetNinePart(lpNinePart);

	return (m_pBackImgN != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinComboBox_ListBox::SetSelectImage( LPCTSTR lpszFileName, CONST LPRECT lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pSelectImg);
	m_pSelectImg = UIRenderEngine->GetImage(lpszFileName);

	if (m_pSelectImg != NULL)
		m_pSelectImg->SetNinePart(lpNinePart);

	return (m_pSelectImg != NULL) ? TRUE : FALSE;
}

void CEasySkinComboBox_ListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDCControl=CDC::FromHandle(lpDrawItemStruct->hDC);

	//创建缓冲
 	CDC BufferDC;
 	CBitmap ImageBuffer;
 	BufferDC.CreateCompatibleDC(pDCControl);
 	ImageBuffer.CreateCompatibleBitmap(pDCControl,rcItem.Width(),rcItem.Height());
 
 	//设置环境
 	BufferDC.SelectObject(&ImageBuffer);
	BufferDC.SelectObject(GetCtrlFont());

	//获取字符
	CString strString;
	GetText(lpDrawItemStruct->itemID,strString);

	//计算位置
	CRect rcString;
	rcString.SetRect(4,0,rcItem.Width()-8,rcItem.Height());

	//颜色定义
	COLORREF crTextColor=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0)?m_colSelectText:m_colNormalText;
	COLORREF crBackColor=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0)?RGB(10,36,106):RGB(255,255,255);

	//绘画背景
	BufferDC.FillSolidRect(0,0,rcItem.Width(),rcItem.Height(),crBackColor);

	if ( (lpDrawItemStruct->itemState&ODS_SELECTED) != 0 )
	{
		if ( m_pSelectImg!= NULL && !m_pSelectImg->IsNull() )
		{
			rcItem.DeflateRect(1,1,1,1);
			m_pSelectImg->Draw(&BufferDC,CRect(0,0,rcItem.Width(),rcItem.Height()));
		}
	}

	//绘画字符
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(crTextColor);
	BufferDC.DrawText(strString,&rcString,DT_VCENTER|DT_SINGLELINE);

	//绘画界面
 	pDCControl->BitBlt(rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
 	BufferDC.DeleteDC();
 	ImageBuffer.DeleteObject();
}

void CEasySkinComboBox_ListBox::OnDestroy()
{
	__super::OnDestroy();

	UIRenderEngine->RemoveImage(m_pBackImgN);
	UIRenderEngine->RemoveImage(m_pSelectImg);
}

void CEasySkinComboBox_ListBox::DrawListFrame()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	CDC *pDC = GetWindowDC();
	if( pDC == NULL ) pDC = GetDC();

	UIRenderEngine->DrawRect(pDC->GetSafeHdc(),rcWindow,1,m_colFrameNormal);

	ReleaseDC(pDC);
}

void CEasySkinComboBox_ListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CRect rcItem(0,0,0,0);

	int nIndex = -1;

	for (int i =0; i<GetCount(); i++)
	{
		GetItemRect(i, &rcItem);

		if(rcItem.PtInRect(point))
		{
			if ( m_hOwnerWnd != NULL ) nIndex = i;
		
			break;
		}
	}

	::PostMessage(m_hOwnerWnd,WM_CBO_LIST_RBUTTONUP,nIndex,0);

	__super::OnRButtonUp(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
// CEasySkinComboBox

IMPLEMENT_DYNAMIC(CEasySkinComboBox, CComboBox)

CEasySkinComboBox::CEasySkinComboBox()
{
	m_lpBgImgN = NULL;
	m_lpBgImgH = NULL;
	m_lpArrowImgN = NULL;
	m_lpArrowImgH = NULL;
	m_lpArrowImgP = NULL;
	m_bFocus = m_bPress = m_bHover = m_bMouseTracking = FALSE;
	m_bArrowPress = FALSE;
	m_bArrowHover = FALSE;
	m_nArrowWidth = 17;
	m_rcArrow.SetRectEmpty();
	m_cyItemHeight = 15;
}

CEasySkinComboBox::~CEasySkinComboBox()
{

}

BEGIN_MESSAGE_MAP(CEasySkinComboBox, CComboBox)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
 	ON_WM_MOUSEMOVE()
 	ON_WM_MOUSELEAVE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
 	ON_MESSAGE(WM_CBO_EDIT_MOUSE_HOVER, OnEditMouseHover)
 	ON_MESSAGE(WM_CBO_EDIT_MOUSE_LEAVE, OnEditMouseLeave)
	ON_MESSAGE(WM_CBO_LIST_RBUTTONUP,OnRButtonUp)
	ON_MESSAGE(WM_CBO_LIST_HIDE, OnListHide)
	ON_MESSAGE(WM_CTLCOLORLISTBOX,OnCtlColorListBox)
END_MESSAGE_MAP()

BOOL CEasySkinComboBox::SetBgNormalPic( LPCTSTR lpszFileName, RECT * lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_lpBgImgN);
	m_lpBgImgN = UIRenderEngine->GetImage(lpszFileName);
	if (m_lpBgImgN != NULL)
		m_lpBgImgN->SetNinePart(lpNinePart);
	return (m_lpBgImgN != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinComboBox::SetBgHotPic( LPCTSTR lpszFileName, RECT * lpNinePart /*= NULL*/ )
{
	UIRenderEngine->RemoveImage(m_lpBgImgH);
	m_lpBgImgH = UIRenderEngine->GetImage(lpszFileName);
	if (m_lpBgImgH != NULL)
		m_lpBgImgH->SetNinePart(lpNinePart);
	return (m_lpBgImgH != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinComboBox::SetArrowNormalPic( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_lpArrowImgN);
	m_lpArrowImgN = UIRenderEngine->GetImage(lpszFileName);
	return (m_lpArrowImgN != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinComboBox::SetArrowHotPic( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_lpArrowImgH);
	m_lpArrowImgH =UIRenderEngine->GetImage(lpszFileName);
	return (m_lpArrowImgH != NULL) ? TRUE : FALSE;
}

BOOL CEasySkinComboBox::SetArrowPushedPic( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_lpArrowImgP);
	m_lpArrowImgP = UIRenderEngine->GetImage(lpszFileName);
	return (m_lpArrowImgP != NULL) ? TRUE : FALSE;
}

void CEasySkinComboBox::SetDefaultText( LPCTSTR lpszText )
{
	m_EasySkinComboBoxEdit.SetDefaultText(lpszText);
}

BOOL CEasySkinComboBox::IsDefaultText()
{
	return m_EasySkinComboBoxEdit.IsDefaultText();
}

void CEasySkinComboBox::SetArrowWidth( int nWidth )
{
	m_nArrowWidth = nWidth;
}

// CEasySkinComboBox 消息处理程序

void CEasySkinComboBox::PreSubclassWindow()
{
	//变量定义
	COMBOBOXINFO ComboBoxInfo;
	ComboBoxInfo.cbSize=sizeof(ComboBoxInfo);

	//绑定控件
	if (GetComboBoxInfo(&ComboBoxInfo)==TRUE)
	{
		if (ComboBoxInfo.hwndItem!=NULL) 
		{
			m_EasySkinComboBoxEdit.SetOwnerWnd(GetSafeHwnd());
			m_EasySkinComboBoxEdit.SubclassWindow(ComboBoxInfo.hwndItem);
		}

		if (ComboBoxInfo.hwndList!=NULL) 
		{
			m_EasySkinComboBoxList.SetOwnerWnd(GetSafeHwnd());
			m_EasySkinComboBoxList.SubclassWindow(ComboBoxInfo.hwndList);
		}
	}

	CComboBox::PreSubclassWindow();
}

int CEasySkinComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	//变量定义
	COMBOBOXINFO ComboBoxInfo;
	ComboBoxInfo.cbSize=sizeof(ComboBoxInfo);

	//绑定控件
	if (GetComboBoxInfo(&ComboBoxInfo)==TRUE)
	{
		if (ComboBoxInfo.hwndItem!=NULL) 
		{
			m_EasySkinComboBoxEdit.SetOwnerWnd(GetSafeHwnd());
			m_EasySkinComboBoxEdit.SubclassWindow(ComboBoxInfo.hwndItem);
		}

		if (ComboBoxInfo.hwndList!=NULL) 
		{
			m_EasySkinComboBoxList.SetOwnerWnd(GetSafeHwnd());
			m_EasySkinComboBoxList.SubclassWindow(ComboBoxInfo.hwndList);
		}
	}

	return 0;
}


BOOL CEasySkinComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CEasySkinComboBox::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(&dc,rcClient);

	CRect rcArrow;
	HRGN hRgn2 = NULL;
	if (m_lpArrowImgN != NULL && !m_lpArrowImgN->IsNull())
	{
		int cxIcon = m_nArrowWidth;
		int cyIcon = m_lpArrowImgN->GetHeight();

		CalcCenterRect(rcClient, cxIcon, cyIcon, rcArrow);
		rcArrow.right = rcClient.right - 2;
		rcArrow.left = rcArrow.right - cxIcon;
	}

	DrawParentWndBg(GetSafeHwnd(),MemDC->GetSafeHdc());

	if (m_bHover)
	{
		if (m_lpBgImgH != NULL && !m_lpBgImgH->IsNull())
		{
			m_lpBgImgH->Draw(&MemDC, rcClient);
		}
		else
		{
			if (m_lpBgImgN != NULL && !m_lpBgImgN->IsNull())
				m_lpBgImgN->Draw(&MemDC, rcClient);
		}
	}
	else
	{
		if (m_lpBgImgN != NULL && !m_lpBgImgN->IsNull())
			m_lpBgImgN->Draw(&MemDC, rcClient);
	}

	if (m_bArrowPress)
	{
		if (m_lpArrowImgP != NULL && !m_lpArrowImgP->IsNull())
			m_lpArrowImgP->Draw(&MemDC, rcArrow);
	}
	else if (m_bArrowHover)
	{
		if (m_lpArrowImgH != NULL && !m_lpArrowImgH->IsNull())
			m_lpArrowImgH->Draw(&MemDC, rcArrow);
	}
	else
	{
		if (m_lpArrowImgN != NULL && !m_lpArrowImgN->IsNull())
			m_lpArrowImgN->Draw(&MemDC, rcArrow);
	}

	//绘制边框
	m_EasySkinComboBoxList.DrawListFrame();
}

void CEasySkinComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	//设置焦点
	SetFocus();

	//效验数据
	if( m_EasySkinComboBoxEdit.GetSafeHwnd() == NULL ) return;

	if ( (m_EasySkinComboBoxEdit.GetStyle() & ES_READONLY) == 0 )
	{
		if (VerdictOverButton(point))
		{
			m_bArrowPress = TRUE;

			//显示列表
			ShowDropDown(GetDroppedState()==FALSE);
		}
	}
	else
	{
		m_bArrowPress = TRUE;

		//显示列表
		ShowDropDown(GetDroppedState()==FALSE);

		m_EasySkinComboBoxEdit.SetSel(0,0);
	}

	//更新按钮
	RedrawWindow(&m_rcArrow,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);
	//__super::OnLButtonDown(nFlags, point);
}

void CEasySkinComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bRePaint = FALSE;

	if (!m_bMouseTracking)
	{
		TrackMouseLeave(GetSafeHwnd());
		m_bMouseTracking = TRUE;
		m_bHover = TRUE;
		bRePaint = TRUE;
	}

	if (VerdictOverButton(point))
	{
		if (!m_bArrowHover)
		{
			m_bArrowHover = TRUE;
			bRePaint = TRUE;
		}
	}
	else
	{
		if (m_bArrowHover)
		{
			m_bArrowHover = FALSE;
			bRePaint = TRUE;
		}
	}

	if (bRePaint)
		RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);

	__super::OnMouseMove(nFlags, point);
}

void CEasySkinComboBox::OnMouseLeave()
{
	m_bMouseTracking = FALSE;

	if (!m_EasySkinComboBoxList.IsWindowVisible())
	{
		CPoint pt;
		GetCursorPos(&pt);

		CRect rcWindow;
		GetWindowRect(&rcWindow);

		if (!rcWindow.PtInRect(pt))
			m_bHover = FALSE;

		m_bArrowHover = FALSE;
		RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);
	}

	__super::OnMouseLeave();
}

void CEasySkinComboBox::OnDestroy()
{
	__super::OnDestroy();

	UIRenderEngine->RemoveImage(m_lpBgImgN);
	UIRenderEngine->RemoveImage(m_lpBgImgH);
	UIRenderEngine->RemoveImage(m_lpArrowImgN);
	UIRenderEngine->RemoveImage(m_lpArrowImgH);
	UIRenderEngine->RemoveImage(m_lpArrowImgP);

	RemoveScorll();
}

void CEasySkinComboBox::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcEdit;
	m_EasySkinComboBoxEdit.GetWindowRect(&rcEdit);
	ScreenToClient(&rcEdit);

	CDC* pDC = m_EasySkinComboBoxEdit.GetDC();
	TEXTMETRIC tm = {0};
	pDC->GetTextMetrics(&tm);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int nMargin = (rcEdit.Height() - nFontHeight) / 2;
	m_EasySkinComboBoxEdit.ReleaseDC(pDC);

	rcEdit.DeflateRect(0, nMargin);
	rcEdit.right = rcClient.right - 2 - m_nArrowWidth;

	m_EasySkinComboBoxEdit.MoveWindow(&rcEdit, FALSE);

	m_rcArrow.left = rcClient.right - 2 - m_nArrowWidth;
	m_rcArrow.right = m_rcArrow.left + m_nArrowWidth;
	m_rcArrow.top = rcClient.top;
	m_rcArrow.bottom = rcClient.bottom;
}

LRESULT CEasySkinComboBox::OnEditMouseHover( WPARAM wParam, LPARAM lParam )
{
	if (!m_bHover)
	{
		m_bHover = TRUE;
		RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);
	}

	return 0;
}

LRESULT CEasySkinComboBox::OnEditMouseLeave( WPARAM wParam, LPARAM lParam )
{
	CPoint pt;
	GetCursorPos(&pt);

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	if (!rcWindow.PtInRect(pt))
	{
		if (m_bHover)
		{
			m_bHover = FALSE;
			RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);
		}
	}

	return 0;
}

LRESULT CEasySkinComboBox::OnListHide( WPARAM wParam, LPARAM lParam )
{
	m_bHover = FALSE;
	m_bArrowHover = FALSE;
	m_bArrowPress = FALSE;
	RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);

	return 0;
}

bool CEasySkinComboBox::VerdictOverButton( CPoint MousePoint )
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//下拉列表
	if ((rcClient.PtInRect(MousePoint)==TRUE)&&((GetStyle()&CBS_SIMPLE)!=0)) return true;

	//坐标计算
	if ((MousePoint.y>(rcClient.Height()-1))||(MousePoint.y<1)) return false;
	if ((MousePoint.x<(rcClient.Width()-(INT)m_rcArrow.Width()-1))||(MousePoint.x>(rcClient.Width()-1))) return false;

	return true;
}

void CEasySkinComboBox::SetDropList()
{
	//效验数据
	if ( m_EasySkinComboBoxEdit.GetSafeHwnd() == NULL ) return;
	
	//m_EasySkinComboBoxEdit.EnableWindow(FALSE);
	m_EasySkinComboBoxEdit.SetReadOnly();
}

void CEasySkinComboBox::SetEditTextColor( COLORREF col )
{
	//效验数据
	if ( m_EasySkinComboBoxEdit.GetSafeHwnd() == NULL ) return;
	
	if ( (m_EasySkinComboBoxEdit.GetStyle() & ES_READONLY) == 0)	
		m_EasySkinComboBoxEdit.m_colNormalText = col;
	else m_EasySkinComboBoxEdit.m_colReadOnlyText = col;

	RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW);
}

CEasySkinComboBox_Edit * CEasySkinComboBox::GetEditWnd()
{
	if ( m_EasySkinComboBoxEdit.GetSafeHwnd() == NULL ) return NULL;
	
	return &m_EasySkinComboBoxEdit;
}

CEasySkinComboBox_ListBox * CEasySkinComboBox::GetListBoxWnd()
{
	if ( m_EasySkinComboBoxList.GetSafeHwnd() == NULL ) return NULL;

	return &m_EasySkinComboBoxList;
}

void CEasySkinComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_cyItemHeight;
}

void CEasySkinComboBox::SetAllItemHeight(UINT cyItemHeight)
{
	m_cyItemHeight = cyItemHeight;
}

LRESULT CEasySkinComboBox::OnCtlColorListBox( WPARAM wParam, LPARAM lParam )
{
	HWND hListBoxWnd = m_EasySkinComboBoxList.GetSafeHwnd();
	if ( hListBoxWnd == NULL) 
	{
		HWND hWnd = (HWND)lParam;
		if (hWnd != 0 && hWnd != m_hWnd) 
		{
			hListBoxWnd = hWnd;
			m_pWndProc = (WNDPROC)GetWindowLong(hListBoxWnd, GWL_WNDPROC);
			SetWindowLong(hListBoxWnd, GWL_WNDPROC, (LONG)ComboBoxListBoxProc);
		}
	}

	return DefWindowProc(WM_CTLCOLORLISTBOX, wParam, lParam);
}

BOOL CEasySkinComboBox::SetScrollImage( LPCTSTR pszFileName )
{
	if ( m_EasySkinComboBoxList.GetSafeHwnd() == FALSE ) return FALSE;
	
	return m_EasySkinComboBoxList.SetScrollImage(&m_EasySkinComboBoxList,pszFileName);
}

LRESULT CEasySkinComboBox::OnRButtonUp( WPARAM wParam, LPARAM lParam )
{
	CWnd *pWnd = GetParent();

	if ( (pWnd != NULL) && (pWnd->GetSafeHwnd() != NULL) )
	{
		pWnd->PostMessage(WM_CBO_RBUTTONUP,wParam,lParam);
	}

	return 0L;
}

