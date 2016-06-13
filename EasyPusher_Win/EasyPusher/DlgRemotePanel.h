/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once


#include "DlgVideo.h"
#include "../libEasyPlayer/libEasyPlayerAPI.h"

#pragma comment(lib, "../bin/libEasyPlayer.lib")

typedef struct __EASY_CHANNEL_INFO_T
{
	int		channelId;


}EASY_CHANNEL_INFO_T;

// CDlgRemotePanel 对话框

class CDlgRemotePanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRemotePanel)

public:
	CDlgRemotePanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRemotePanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_REMOTE_PANEL };


protected:
	CDlgVideo		*pDlgVideo;
	void		UpdateComponents();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CWnd* GetDlgVideo();

	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
};
