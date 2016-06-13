/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

#include "DlgVideo.h"

//#include "SourceManager.h"

// CDlgLocalPanel 对话框

class CDlgLocalPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLocalPanel)

public:
	CDlgLocalPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLocalPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOCAL_PANEL };

protected:

	CDlgVideo		*pDlgVideo;
	void		UpdateComponents();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();

	CWnd* GetDlgVideoHwnd();

};
