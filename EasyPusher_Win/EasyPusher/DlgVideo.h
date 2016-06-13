/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

#include "Resource.h"

// CDlgVideo 对话框
class CDlgPanel;

class CDlgVideo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVideo)

public:
	CDlgVideo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVideo();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

public:
	virtual BOOL OnInitDialog();

private:
	CImageEx	* m_pEasyLogo;
	CDlgPanel* m_pMainDlg;
public:
	void SetMainDlg(CDlgPanel* pMainDlg);

};
