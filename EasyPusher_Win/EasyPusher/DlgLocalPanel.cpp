/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// DlgLocalPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyPusher.h"
#include "DlgLocalPanel.h"
#include "afxdialogex.h"


// CDlgLocalPanel 对话框
CDlgLocalPanel	*gDlgLocalPanel = NULL;
IMPLEMENT_DYNAMIC(CDlgLocalPanel, CDialogEx)

CDlgLocalPanel::CDlgLocalPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLocalPanel::IDD, pParent)
{
	pDlgVideo			=	NULL;
}

CDlgLocalPanel::~CDlgLocalPanel()
{
}

void CDlgLocalPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLocalPanel, CDialogEx)

END_MESSAGE_MAP()


// CDlgLocalPanel 消息处理程序


BOOL CDlgLocalPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	pDlgVideo = new CDlgVideo();
	pDlgVideo->Create(IDD_DIALOG_VIDEO, this);
	pDlgVideo->ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


LRESULT CDlgLocalPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


BOOL CDlgLocalPanel::DestroyWindow()
{
	__DESTROY_WINDOW(pDlgVideo);

	return CDialogEx::DestroyWindow();
}


CWnd* CDlgLocalPanel::GetDlgVideoHwnd()
{
	if (!pDlgVideo)
	{
		return NULL;
	}
	return pDlgVideo;
}

void CDlgLocalPanel::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	CRect	rcPushAddr, rcEdtAddr;

	CRect	rcVideo;
	rcVideo.SetRect(rcClient.left+VIDEO_WINDOW_BORDER_WIDTH, 0, 
		rcClient.right-VIDEO_WINDOW_BORDER_WIDTH, rcClient.bottom-VIDEO_WINDOW_BORDER_WIDTH);

	__MOVE_WINDOW(pDlgVideo, rcVideo);
	if (pDlgVideo)
	{
		pDlgVideo->Invalidate(FALSE);
	}
}
