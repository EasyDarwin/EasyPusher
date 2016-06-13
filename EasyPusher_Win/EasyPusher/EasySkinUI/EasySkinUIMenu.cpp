/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "StdAfx.h"
#include "EasySkinUIMenu.h"

//////////////////////////////////////////////////////////////////////////////////
//常量定义

//间隙变量
#define SPACE_LEFT					8									//空隙大小
#define SPACE_RIGHT					5									//空隙大小

//大小变量
#define MENU_BAR_CX					24									//标题高度
#define MENU_ITEM_CY				24									//子项高度
#define SEPARATOR_CY				3									//拆分高度
#define MENU_BORDER					6									//菜单边宽

//////////////////////////////////////////////////////////////////////////////////

//菜单变量
CMenuStringArray					CUIMenu::m_MenuItemString;		//字符子项
CMenuSeparatorArray					CUIMenu::m_MenuItemSeparator;		//拆分子项
HHOOK								CUIMenu::m_hMenuHook = NULL;

WNDPROC OldWndProc = NULL; 
LRESULT lResult=0;
//////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{ 
	switch(message) 
	{ 
	case WM_CREATE: 
		{ 
			DWORD dwStyle = ::GetWindowLong(hWnd ,GWL_STYLE); 
			DWORD dwNewStyle =   (dwStyle & ~WS_BORDER); 
			::SetWindowLong(hWnd ,GWL_STYLE ,dwNewStyle); 
			DWORD dwExStyle = ::GetWindowLong(hWnd ,GWL_EXSTYLE); 
			DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE)); 
			::SetWindowLong(hWnd ,GWL_EXSTYLE, dwNewExStyle); 
			
			break;
		}
 	case WM_PRINT:   
		{
			return CallWindowProc(OldWndProc,hWnd,WM_PRINTCLIENT,wParam,lParam); 
		}
	case WM_ERASEBKGND:
		return TRUE;
 	case WM_WINDOWPOSCHANGING: 
 		{ 
 			LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam; 
 
  			lpPos->cx -= MENU_BORDER; 
  			lpPos->cy -= MENU_BORDER; 

 			break;
 		}
	}

	return CallWindowProc(OldWndProc,hWnd,message,wParam,lParam); 
} 

//////////////////////////////////////////////////////////////////////////

//构造函数
CUIMenu::CUIMenu()
{
	m_pMenuBar = NULL;
	m_pMenuBack = NULL;
	m_pMenuHoven = NULL;
	m_pSeparator = NULL;
	m_pCheck = NULL;
	m_pArrow = NULL;
	m_colFrameNormal = RGB(112,123,136);
}

//析构函数
CUIMenu::~CUIMenu()
{
	DestroyMenu();
}

//绘画函数
VOID CUIMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//效验参数
	ASSERT(lpDrawItemStruct->CtlType==ODT_MENU);
	if (lpDrawItemStruct->CtlType!=ODT_MENU) return;

	//变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CEasySkinMenuItem * pEasySkinMenuItem=(CEasySkinMenuItem *)lpDrawItemStruct->itemData;

	CMemoryDC BufferDC(pDC,rcItem);

	//状态变量
	bool bChecked=((lpDrawItemStruct->itemState&ODS_CHECKED)!=0);
	bool bSelected=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);
	bool bGrayed=((lpDrawItemStruct->itemState&ODS_DISABLED)||(lpDrawItemStruct->itemState&ODS_GRAYED));

	//绘画背景
	COLORREF crMenuBar=RGB(255,255,255);
	COLORREF crBorder=RGB(213,233,242);
	COLORREF crNormalBack=RGB(255,255,255);

	//菜单左边
	if ( m_pMenuBar != NULL && m_pMenuBar->IsNull() == false )
		m_pMenuBar->Draw(&BufferDC,CRect(rcItem.left,rcItem.top,MENU_BAR_CX,rcItem.bottom));
	else
		BufferDC.FillSolidRect(rcItem.left,rcItem.top,MENU_BAR_CX,rcItem.Height(),crMenuBar);
	
	//菜单节点的背景
 	if ( m_pMenuBack != NULL && m_pMenuBack->IsNull() == false )
 		m_pMenuBack->Draw(&BufferDC,CRect(rcItem.left+MENU_BAR_CX,rcItem.top,rcItem.Width(),rcItem.bottom));
 	else
		BufferDC.FillSolidRect(rcItem.left+MENU_BAR_CX,rcItem.top,rcItem.Width()-MENU_BAR_CX,rcItem.Height(),crNormalBack);

	//焦点边框
	if ((bSelected==true)&&(bGrayed==false)&&(pEasySkinMenuItem!=NULL)&&(pEasySkinMenuItem->m_MenuItemType!=MenuItemType_Separator))
	{
		if ( m_pMenuHoven != NULL && m_pMenuHoven->IsNull() == false )
		{
			CRect rcSelItem(rcItem);
			rcSelItem.left += 1;
			rcSelItem.top += 1;
			rcSelItem.bottom -= 1;
			m_pMenuHoven->Draw(&BufferDC,CRect(rcSelItem.left+1,rcSelItem.top+1,rcSelItem.Width()-1,rcSelItem.top+rcSelItem.Height()-1));
		}
		else
			BufferDC.FillSolidRect(rcItem.left+2,rcItem.top+2,rcItem.Width()-4,rcItem.Height()-4,crBorder);
	}

	//绘制箭头
	if ( pEasySkinMenuItem->m_hSubMenu != NULL )
	{
		if ( m_pArrow != NULL && m_pArrow->IsNull() == false )
		{
			CRect rcArrow(rcItem.right-5-m_pArrow->GetWidth(),rcItem.top+(rcItem.Height()-m_pArrow->GetHeight())/2,rcItem.right-5,rcItem.top+(rcItem.Height()+m_pArrow->GetHeight())/2);
			m_pArrow->DrawImage(&BufferDC,rcArrow.left,rcArrow.top);
		}
	}


	//选择标志
	if (bChecked)
	{	
		if ( m_pCheck != NULL && m_pCheck->IsNull() == false )
		{
			CRect rcIcon(rcItem.left,rcItem.top,MENU_BAR_CX,rcItem.bottom);
			m_pCheck->DrawImage(&BufferDC,(rcIcon.Width()-m_pCheck->GetWidth())/2,rcIcon.top+(rcIcon.Height()-m_pCheck->GetHeight())/2);
		}
	}

	//其他菜单
	if (pEasySkinMenuItem==NULL)
	{
		//获取信息
		MENUITEMINFO MenuItemInfo;
		MenuItemInfo.cbSize=sizeof(MenuItemInfo);
		MenuItemInfo.fMask=MIIM_FTYPE|MIIM_BITMAP;
		GetMenuItemInfo(lpDrawItemStruct->itemID,&MenuItemInfo);

		return;
	}

	//界面菜单
	switch (pEasySkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_String:		//字符菜单
		{
			//变量定义
			CEasySkinMenuString * pEasySkinMenuString=(CEasySkinMenuString *)pEasySkinMenuItem;

			CRect rcIcon(rcItem.left,rcItem.top,MENU_BAR_CX,rcItem.bottom);

			if ( bSelected )
			{
				if ( pEasySkinMenuString->m_pImageH != NULL && pEasySkinMenuString->m_pImageH->IsNull() == false )
					pEasySkinMenuString->m_pImageH->DrawImage(&BufferDC,(rcIcon.Width()-pEasySkinMenuString->m_pImageH->GetWidth())/2,rcIcon.top+(rcIcon.Height()-pEasySkinMenuString->m_pImageH->GetHeight())/2);
				else if ( pEasySkinMenuString->m_pImageN != NULL && pEasySkinMenuString->m_pImageN->IsNull() == false )
					pEasySkinMenuString->m_pImageN->DrawImage(&BufferDC,(rcIcon.Width()-pEasySkinMenuString->m_pImageN->GetWidth())/2,rcIcon.top+(rcIcon.Height()-pEasySkinMenuString->m_pImageN->GetHeight())/2);
			}
			else
			{
				if ( pEasySkinMenuString->m_pImageN != NULL && pEasySkinMenuString->m_pImageN->IsNull() == false )
					pEasySkinMenuString->m_pImageN->DrawImage(&BufferDC,(rcIcon.Width()-pEasySkinMenuString->m_pImageN->GetWidth())/2,rcIcon.top+(rcIcon.Height()-pEasySkinMenuString->m_pImageN->GetHeight())/2);
			}

			//设置颜色
			BufferDC.SetBkMode(TRANSPARENT);
			BufferDC.SelectObject(GetCtrlFont());

			//设置颜色
			if (bGrayed==true) BufferDC.SetTextColor(m_colDisableText);
			else if (bSelected==true) BufferDC.SetTextColor(m_colSelectText);
			else BufferDC.SetTextColor(m_colNormalText);

			//绘画字符
			CRect rcString;
			rcString.top=rcItem.top;
			rcString.bottom=rcItem.bottom;
			rcString.right=rcItem.right-SPACE_RIGHT;
			rcString.left=rcItem.left+MENU_BAR_CX+SPACE_LEFT;
			BufferDC.DrawText(pEasySkinMenuString->m_strString,pEasySkinMenuString->m_strString.GetLength(),&rcString,DT_SINGLELINE|DT_VCENTER|DT_NOCLIP);

			break;
		}
	case MenuItemType_Separator:	//拆分菜单
		{
			//绘画拆分
			if ( m_pSeparator != NULL && m_pSeparator->IsNull() == false )
				m_pSeparator->Draw(&BufferDC,CRect(rcItem.left+MENU_BAR_CX+SPACE_LEFT,rcItem.top+1,rcItem.right-SPACE_RIGHT,rcItem.Height()-2));
			else
				BufferDC.FillSolidRect(rcItem.left+MENU_BAR_CX+SPACE_LEFT,rcItem.top+1,rcItem.Width()-MENU_BAR_CX-SPACE_LEFT-SPACE_RIGHT,rcItem.Height()-2,m_crSeparator);

			//补齐分隔条处的边框颜色
			UIRenderEngine->DrawLine(BufferDC.GetSafeHdc(),CRect(rcItem.left,rcItem.top,rcItem.left+1,rcItem.top+rcItem.Height()+1),1,m_colFrameNormal);
			UIRenderEngine->DrawLine(BufferDC.GetSafeHdc(),CRect(rcItem.right-1,rcItem.top,rcItem.right,rcItem.top+rcItem.Height()+1),1,m_colFrameNormal);

			break;
		}
	}

	//绘制边框色
	int nSeparatorCount = GetSeparatorCount();

	CRect rcClient(0,0,rcItem.right,0);
	rcClient.bottom = rcItem.Height()*(GetMenuItemCount()-nSeparatorCount)+SEPARATOR_CY*nSeparatorCount;

	UIRenderEngine->DrawRect(BufferDC.GetSafeHdc(),rcClient,1,m_colFrameNormal);

	//绘画界面
	pDC->BitBlt(rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),&BufferDC,rcItem.left,rcItem.top,SRCCOPY);

	//抠掉子节点区域，从而屏蔽系统绘制的倒三角
	if ( (pEasySkinMenuItem->m_hSubMenu != NULL) &&  (m_pArrow != NULL && m_pArrow->IsNull() == false))
		pDC->ExcludeClipRect(rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);

	return;
}

//测量位置
VOID CUIMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//效验参数
	ASSERT(lpMeasureItemStruct->CtlType==ODT_MENU);
	if (lpMeasureItemStruct->CtlType!=ODT_MENU) return;

	//获取数据
	CEasySkinMenuItem * pEasySkinMenuItem=(CEasySkinMenuItem *)lpMeasureItemStruct->itemData;
	if (pEasySkinMenuItem==NULL)
	{
		//获取信息
		MENUITEMINFO MenuItemInfo;
		MenuItemInfo.cbSize=sizeof(MenuItemInfo);
		MenuItemInfo.fMask=MIIM_FTYPE|MIIM_STRING;
		GetMenuItemInfo(lpMeasureItemStruct->itemID,&MenuItemInfo);

		return;
	}

	//界面菜单
	switch (pEasySkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_String:		//字符菜单
		{
			//变量定义
			CEasySkinMenuString * pEasySkinMenuString=(CEasySkinMenuString *)pEasySkinMenuItem;

			//设置 DC
			HDC hDC=GetDC(NULL);
			SelectObject(hDC,GetCtrlFont());

			//计算长度
			CSize SizeString;
			GetTextExtentPoint32(hDC,pEasySkinMenuString->m_strString,pEasySkinMenuString->m_strString.GetLength(),&SizeString);

			//计算位置
			lpMeasureItemStruct->itemHeight=MENU_ITEM_CY;
			lpMeasureItemStruct->itemWidth=SizeString.cx+MENU_BAR_CX+SPACE_LEFT+SPACE_RIGHT;

			//释放资源
			ReleaseDC(NULL,hDC);

			break;
		}
	case MenuItemType_Separator:	//拆分菜单
		{
			lpMeasureItemStruct->itemWidth=0;
			lpMeasureItemStruct->itemHeight=SEPARATOR_CY;

			break;
		}
	}

	return;
}

//创建菜单
bool CUIMenu::CreatePopupMenu()
{
	m_hMenuHook = ::SetWindowsHookEx(WH_CALLWNDPROC,CUIMenu::WindowsHook, ::GetModuleHandle(AfxGetAppName()) ,::GetCurrentThreadId()); 

	return CMenu::CreatePopupMenu()?true:false;
}

//销毁菜单
BOOL CUIMenu::DestroyMenu()
{
	//销毁菜单
	BOOL bRes = CMenu::DestroyMenu();
	
	//释放子项
	for (INT_PTR i=0;i<m_MenuItemActive.GetCount();i++)
	{
		FreeMenuItem(m_MenuItemActive[i]);
	}
	m_MenuItemActive.RemoveAll();

 	//释放子项
 	for (INT_PTR i=0;i<m_MenuItemString.GetCount();i++)
 	{
		UIRenderEngine->RemoveImage(m_MenuItemString.GetAt(i)->m_pImageN);
		UIRenderEngine->RemoveImage(m_MenuItemString.GetAt(i)->m_pImageH);
 		SafeDelete(m_MenuItemString.GetAt(i));
 	}
	m_MenuItemString.RemoveAll();

 	//释放子项
 	for (INT_PTR i=0;i<m_MenuItemSeparator.GetCount();i++)
 	{
 		SafeDelete(m_MenuItemSeparator.GetAt(i));
 	}
	m_MenuItemSeparator.RemoveAll();

	//卸载钩子
	UnhookWindowsHookEx(m_hMenuHook);

	UIRenderEngine->RemoveImage(m_pMenuBar);
	UIRenderEngine->RemoveImage(m_pMenuBack);
	UIRenderEngine->RemoveImage(m_pMenuHoven);
	UIRenderEngine->RemoveImage(m_pSeparator);
	UIRenderEngine->RemoveImage(m_pCheck);
	UIRenderEngine->RemoveImage(m_pArrow);

	return bRes;
}

//弹出菜单
bool CUIMenu::TrackPopupMenu(CWnd * pWnd)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);

	//弹出菜单
	CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,MousePoint.x,MousePoint.y,pWnd);

	return true;
}

//弹出菜单
bool CUIMenu::TrackPopupMenu(INT nXPos, INT nYPos, CWnd * pWnd)
{
	//弹出菜单
	CMenu::TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,nXPos,nYPos,pWnd,NULL)?true:false;

	return true;
}

//插入拆分
bool CUIMenu::AppendSeparator()
{
	//获取子项
	CEasySkinMenuSeparator * pEasySkinMenuItem=(CEasySkinMenuSeparator *)AcitveMenuItem(MenuItemType_Separator);

	//插入菜单
	if (pEasySkinMenuItem!=NULL)
	{
		CMenu::AppendMenu(MF_OWNERDRAW,0,(LPCTSTR)(pEasySkinMenuItem));
		return true;
	}

	return false;
}

//插入字符
bool CUIMenu::AppendMenu(UINT nMenuID, LPCTSTR pszString, LPCTSTR lpszIconN, LPCTSTR lpszIconH, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CEasySkinMenuString * pEasySkinMenuItem=(CEasySkinMenuString *)AcitveMenuItem(MenuItemType_String);

	//插入菜单
	if (pEasySkinMenuItem!=NULL)
	{
		UIRenderEngine->RemoveImage(pEasySkinMenuItem->m_pImageN);
		UIRenderEngine->RemoveImage(pEasySkinMenuItem->m_pImageH);

		pEasySkinMenuItem->m_pImageN = UIRenderEngine->GetImage(lpszIconN);
		pEasySkinMenuItem->m_pImageH = UIRenderEngine->GetImage(lpszIconH);

		pEasySkinMenuItem->m_strString=pszString;
		CMenu::AppendMenu(MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pEasySkinMenuItem));
		return true;
	}

	return false;
}

//插入拆分
bool CUIMenu::InsertSeparator(UINT nPosition)
{
	//获取子项
	CEasySkinMenuSeparator * pEasySkinMenuItem=(CEasySkinMenuSeparator *)AcitveMenuItem(MenuItemType_Separator);

	//插入菜单
	if (pEasySkinMenuItem!=NULL)
	{
		CMenu::InsertMenu(nPosition,MF_OWNERDRAW,0,(LPCTSTR)(pEasySkinMenuItem));
		return true;
	}

	return false;
}

//插入字符
bool CUIMenu::InsertMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition, LPCTSTR lpszIconN, LPCTSTR lpszIconH, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CEasySkinMenuString * pEasySkinMenuItem=(CEasySkinMenuString *)AcitveMenuItem(MenuItemType_String);

	//插入菜单
	if (pEasySkinMenuItem!=NULL)
	{
		UIRenderEngine->RemoveImage(pEasySkinMenuItem->m_pImageN);
		UIRenderEngine->RemoveImage(pEasySkinMenuItem->m_pImageH);

		pEasySkinMenuItem->m_pImageN = UIRenderEngine->GetImage(lpszIconN);
		pEasySkinMenuItem->m_pImageH = UIRenderEngine->GetImage(lpszIconH);

		pEasySkinMenuItem->m_strString=pszString;
		pEasySkinMenuItem->m_hSubMenu = (HMENU)nMenuID;

		CMenu::InsertMenu(nPosition,MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pEasySkinMenuItem));
		return true;
	}

	return false;
}

//删除菜单
bool CUIMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return CMenu::RemoveMenu(nPosition,nFlags)?true:false;
}

//修改菜单
bool CUIMenu::ModifyMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition, LPCTSTR lpszIconN, LPCTSTR lpszIconH, UINT nFlags)
{
	return 0;
}

//释放子项
VOID CUIMenu::FreeMenuItem(CEasySkinMenuItem * pEasySkinMenuItem)
{
	//效验参数
	ASSERT(pEasySkinMenuItem!=NULL);
	if (pEasySkinMenuItem==NULL) return;

	//清理变量
	switch (pEasySkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_String:		//字符类型
		{
			//变量定义
			CEasySkinMenuString * pEasySkinMenuString=(CEasySkinMenuString *)pEasySkinMenuItem;

			//设置变量
			pEasySkinMenuString->m_strString.Empty();
			m_MenuItemString.Add(pEasySkinMenuString);

			break;
		}
	case MenuItemType_Separator:	//拆分类型
		{
			//变量定义
			CEasySkinMenuSeparator * pEasySkinMenuSeparator=(CEasySkinMenuSeparator *)pEasySkinMenuItem;

			//设置变量
			m_MenuItemSeparator.Add(pEasySkinMenuSeparator);

			break;
		}
	}

	return;
}

//获取子项
CEasySkinMenuItem * CUIMenu::AcitveMenuItem(enMenuItemType MenuItemType)
{
	//变量定义
	CEasySkinMenuItem * pEasySkinMenuItem=NULL;

	//创建子项
	switch (MenuItemType)
	{
	case MenuItemType_String:		//字符类型
		{
			if (m_MenuItemString.GetCount()>0)
			{
				INT_PTR nItemCount=m_MenuItemString.GetCount();
				pEasySkinMenuItem=m_MenuItemString[nItemCount-1];
				m_MenuItemString.RemoveAt(nItemCount-1);
			}
			else
			{
				pEasySkinMenuItem=new CEasySkinMenuString;
				if (pEasySkinMenuItem==NULL) return NULL;
			}

			break;
		}
	case MenuItemType_Separator:	//拆分类型
		{
			if (m_MenuItemSeparator.GetCount()>0)
			{
				INT_PTR nItemCount=m_MenuItemSeparator.GetCount();
				pEasySkinMenuItem=m_MenuItemSeparator[nItemCount-1];
				m_MenuItemSeparator.RemoveAt(nItemCount-1);
			}
			else
			{
				pEasySkinMenuItem=new CEasySkinMenuSeparator;
				if (pEasySkinMenuItem==NULL) return NULL;
			}

			break;
		}
	default:
		{
			ASSERT(FALSE);
			return NULL;
		}
	}

	//加入队列
	m_MenuItemActive.Add(pEasySkinMenuItem);

	return pEasySkinMenuItem;
}

LRESULT CALLBACK CUIMenu::WindowsHook( int code,WPARAM wParam,LPARAM lParam )
{
	PCWPSTRUCT lpCwp = (PCWPSTRUCT)lParam;

	while(code == HC_ACTION && lpCwp->message==WM_CREATE) 
	{ 
		TCHAR sClassName[10]; 
		int Count =::GetClassName(lpCwp->hwnd,sClassName,CountArray(sClassName)); 

		//检查是否菜单窗口 
		if( Count == 6 && _tcscmp(sClassName,TEXT("#32768"))==0) 
		{ 
			WNDPROC lastWndProc =  (WNDPROC)GetWindowLong(lpCwp->hwnd,GWL_WNDPROC); 
			if(lastWndProc != WndProc) 
			{ 
#pragma warning(disable:4311)
				SetWindowLong(lpCwp->hwnd,GWL_WNDPROC, (LONG)WndProc); 
				OldWndProc = lastWndProc; 
				break;
			} 
		} 
		break; 
	} 
	return CallNextHookEx(m_hMenuHook,code,wParam,lParam); 
}

BOOL CUIMenu::SetMenuBarImage( LPCTSTR lpszFileName,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pMenuBar);

	m_pMenuBar = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pMenuBar)
		return FALSE;
	else
	{
		m_pMenuBar->SetNinePart(lprcNinePart);
		return TRUE;
	}
}

BOOL CUIMenu::SetMenuBackImage( LPCTSTR lpszFileName,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pMenuBack);

	m_pMenuBack = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pMenuBack)
		return FALSE;
	else
	{
		m_pMenuBack->SetNinePart(lprcNinePart);
		return TRUE;
	}
}

BOOL CUIMenu::SetMenuHovenImage( LPCTSTR lpszFileName,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pMenuHoven);

	m_pMenuHoven = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pMenuHoven)
		return FALSE;
	else
	{
		m_pMenuHoven->SetNinePart(lprcNinePart);
		return TRUE;
	}
}

BOOL CUIMenu::SetSeparatorImage( LPCTSTR lpszFileName,CONST LPRECT lprcNinePart/*=NULL*/ )
{
	UIRenderEngine->RemoveImage(m_pSeparator);

	m_pSeparator = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pSeparator)
		return FALSE;
	else
	{
		m_pSeparator->SetNinePart(lprcNinePart);
		return TRUE;
	}
}

BOOL CUIMenu::SetCheckImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pCheck);

	m_pCheck = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pCheck)
		return FALSE;
	else
	{
		return TRUE;
	}
}
BOOL CUIMenu::SetArrowImage( LPCTSTR lpszFileName )
{
	UIRenderEngine->RemoveImage(m_pArrow);

	m_pArrow = UIRenderEngine->GetImage(lpszFileName);

	if (NULL == m_pArrow)
		return FALSE;
	else
	{
		return TRUE;
	}
}

int CUIMenu::GetSeparatorCount()
{
	int nSeparatorCount = 0;

	for (int i=0;i<m_MenuItemActive.GetCount();i++)
	{
		if ( m_MenuItemActive.GetAt(i)->m_MenuItemType == MenuItemType_Separator )
		{
			nSeparatorCount++;
		}
	}

	return nSeparatorCount;
}




//////////////////////////////////////////////////////////////////////////////////
