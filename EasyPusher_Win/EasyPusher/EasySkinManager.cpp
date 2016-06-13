
/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// SkinManager.cpp : 实现文件
//

#include "stdafx.h"
#include "GlobalUnits.h"
#include "EasySkinManager.h"

// CEasySkinManager 对话框

IMPLEMENT_DYNAMIC(CEasySkinManager, CEasySkinDialog)

CEasySkinManager::CEasySkinManager(UINT nIDTemplate,CWnd* pParent /*=NULL*/,AFX_WND_STYLE Style/* = en_Wnd_Normal*/)
	: CEasySkinDialog(nIDTemplate, pParent)
{
	m_pImageBack = NULL;
	m_pImageLogo = NULL;
	m_pImageTitle = NULL;

	m_enWndStyle = Style;
	m_bIsZoomed = false;
	m_bShowLogo = true;
}

CEasySkinManager::~CEasySkinManager()
{
	UIRenderEngine->RemoveImage(m_pImageBack);
	UIRenderEngine->RemoveImage(m_pImageLogo);
	UIRenderEngine->RemoveImage(m_pImageTitle);
	
}

void CEasySkinManager::DoDataExchange(CDataExchange* pDX)
{
	CEasySkinDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEasySkinManager, CEasySkinDialog)
	ON_BN_CLICKED(IDC_WNDMIN,OnBnClickWindowMin)
	ON_BN_CLICKED(IDC_WNDMAX,OnBnClickWindowMax)
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CEasySkinManager::DrawClientArea( CDC*pDC,int nWidth,int nHeight )
{
	pDC->FillSolidRect(0,0,nWidth,nHeight,COLOR_BACK);

	if ( m_pImageBack != NULL && !m_pImageBack->IsNull() )
	{
		m_pImageBack->DrawExtrude(pDC,CRect(0,0,nWidth,nHeight),GlobalUnits->m_bLeftDrawExtrude);
	}

	//UIRenderEngine->DrawRoundRect(pDC->GetSafeHdc(),CRect(1,33,nWidth-1,1),6,6,1,RGB(12,192,143));
		pDC->FillSolidRect(1,32,nWidth-1,1,RGB(12,192,143));

	//绘制边框
	UIRenderEngine->DrawRoundRect(pDC->GetSafeHdc(),CRect(0,0,nWidth-1,nHeight-1),6,6,1,RGB(92,89,82));//0cb68f

	if ( m_bShowLogo )
	{
		if ( m_pImageLogo != NULL && !m_pImageLogo->IsNull() )
			m_pImageLogo->DrawImage(pDC,4,1);
	}
		if ( m_pImageTitle != NULL && !m_pImageTitle->IsNull() )
			m_pImageTitle->DrawImage(pDC,38,9);	
}

BOOL CEasySkinManager::OnInitDialog()
{
	CEasySkinDialog::OnInitDialog();

	HDC hParentDC = GetBackDC();

	m_pImageBack = UIRenderEngine->GetImage(GlobalUnits->m_szDefaultSkin);
	m_pImageLogo = UIRenderEngine->GetImage(TEXT("SkinUI\\图标\\Easy_32.png"));//Easy_32.png logo.png
	m_pImageTitle = UIRenderEngine->GetImage(TEXT("SkinUI\\图标\\logo.png"));//Easy_32.png 

	GlobalUnits->m_WindowArray.push_back(this);

	CRect rcClient;
	GetClientRect(&rcClient);

	if ( m_enWndStyle != en_Wnd_None )
	{
		m_btClose.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(rcClient.Width()-41,0,0,0),this,IDCANCEL);
		m_btClose.SetBackImage(TEXT("\\SkinUI\\Button\\btn_close_normal.png"),TEXT("\\SkinUI\\Button\\btn_close_highlight.png"),TEXT("\\SkinUI\\Button\\btn_close_down.png"),TEXT("\\SkinUI\\Button\\btn_close_normal.png"));
		//m_btClose.SetBackImage(TEXT("\\SkinUI\\图标\\关闭常规.png"),TEXT("\\SkinUI\\图标\\关闭选中.png"),TEXT("\\SkinUI\\图标\\关闭选中.png"),TEXT("\\SkinUI\\图标\\关闭常规.png"));
		m_btClose.SetButtonType(en_PushButton);
		m_btClose.SetParentBack(hParentDC);
		m_btClose.SetSize(39,20);
		//m_btClose.SetSize(29,24);

		if ( m_enWndStyle != en_Wnd_CloseBox )
		{
			if( m_enWndStyle != en_Wnd_MinimizeBox )
			{
				m_btMax.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(rcClient.Width()-69,0,0,0),this,IDC_WNDMAX);
				m_btMax.SetBackImage(TEXT("\\SkinUI\\Button\\btn_max_normal.png"),TEXT("\\SkinUI\\Button\\btn_max_highlight.png"),TEXT("\\SkinUI\\Button\\btn_max_down.png"),TEXT("\\SkinUI\\Button\\btn_max_normal.png"));
				m_btMax.SetButtonType(en_PushButton);
				m_btMax.SetParentBack(hParentDC);
				m_btMax.SetSize(28,20);
			}

			m_btMin.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(rcClient.Width()-69-(m_enWndStyle==en_Wnd_Normal?28:0),0,0,0),this,IDC_WNDMIN);
			m_btMin.SetBackImage(TEXT("\\SkinUI\\Button\\btn_mini_normal.png"),TEXT("\\SkinUI\\Button\\btn_mini_highlight.png"),TEXT("\\SkinUI\\Button\\btn_mini_down.png"),TEXT("\\SkinUI\\Button\\btn_mini_normal.png"));
			m_btMin.SetButtonType(en_PushButton);
			m_btMin.SetParentBack(hParentDC);
			m_btMin.SetSize(28,20);
		}
	}

	return TRUE;
}

void CEasySkinManager::OnBnClickWindowMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CEasySkinManager::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !m_bIsZoomed )
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CEasySkinDialog::OnLButtonDown(nFlags, point);
}

void CEasySkinManager::UpdateSkin()
{
	UIRenderEngine->RemoveImage(m_pImageBack);
	m_pImageBack = UIRenderEngine->GetImage(GlobalUnits->m_szDefaultSkin);

	for (int i=0;i<GlobalUnits->m_WindowArray.size();i++)
	{
		CEasySkinManager * pSkinManager = GlobalUnits->m_WindowArray.at(i);

		if (pSkinManager->GetSafeHwnd() == NULL ) continue;
		
		pSkinManager->ModifyStyle(WS_CLIPCHILDREN,0);

		pSkinManager->Invalidate(FALSE);

		pSkinManager->ModifyStyle(0,WS_CLIPCHILDREN);
	}
}

void CEasySkinManager::OnDestroy()
{
	vector<CEasySkinManager*>::iterator iter = GlobalUnits->m_WindowArray.begin();

	for(; iter != GlobalUnits->m_WindowArray.end(); ++iter )
	{
		CEasySkinManager* pSkinManager = (CEasySkinManager*)*iter;
		if( pSkinManager == (CEasySkinManager*)this )
		{
			GlobalUnits->m_WindowArray.erase(iter);
			break;
		}
	}

	__super::OnDestroy();
}

void CEasySkinManager::OnBnClickWindowMax()
{
	static CRect rcClient(0,0,0,0);

	if ( m_bIsZoomed )
	{
		m_btMax.SetBackImage(TEXT("\\SkinUI\\Button\\btn_max_normal.png"),TEXT("\\SkinUI\\Button\\btn_max_highlight.png"),TEXT("\\SkinUI\\Button\\btn_max_down.png"),TEXT("\\SkinUI\\Button\\btn_max_normal.png"));

		MoveWindow(&rcClient);

		m_bIsZoomed = false;
	}
	else 
	{
		GetWindowRect(&rcClient);
		m_btMax.SetBackImage(TEXT("\\SkinUI\\Button\\btn_restore_normal.png"),TEXT("\\SkinUI\\Button\\btn_restore_highlight.png"),TEXT("\\SkinUI\\Button\\btn_restore_down.png"),TEXT("\\SkinUI\\Button\\btn_restore_normal.png"));

		CRect rc;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0); 
		MoveWindow(&rc);

		m_bIsZoomed = true;
	}
}

void CEasySkinManager::OnSize( UINT nType, int cx, int cy )
{
	CEasySkinDialog::OnSize(nType, cx, cy);

	//移动准备
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	if ( m_enWndStyle != en_Wnd_None )
	{
		DeferWindowPos(hDwp,m_btClose,NULL,cx-41,0,0,0,uFlags|SWP_NOSIZE);

		if ( m_enWndStyle != en_Wnd_CloseBox )
		{
			if( m_enWndStyle != en_Wnd_MinimizeBox )
				DeferWindowPos(hDwp,m_btMax,NULL,cx-69,0,0,0,uFlags|SWP_NOSIZE);

			DeferWindowPos(hDwp,m_btMin,NULL,cx-69-(m_enWndStyle==en_Wnd_Normal?28:0),0,0,0,uFlags|SWP_NOSIZE);
		}
	}

	//结束调整
	//LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	//UnlockWindowUpdate();

	//设置圆角
	CRgn rgn;
	rgn.CreateRoundRectRgn(0,0,cx,cy,4,4);
	SetWindowRgn(rgn,FALSE);

	//更新界面
	Invalidate(FALSE);
}

BOOL CEasySkinManager::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CEasySkinManager::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( m_enWndStyle == en_Wnd_Normal )
		OnBnClickWindowMax();

	CEasySkinDialog::OnLButtonDblClk(nFlags, point);
}

// CEasySkinManager 消息处理程序
bool CEasySkinManager::IsWindowMax()
{
	return m_bIsZoomed;
}

void CEasySkinManager::SetSkinWindowStytle(AFX_WND_STYLE enWndStyle)
{
	m_enWndStyle = enWndStyle;
}