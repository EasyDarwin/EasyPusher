/*
Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
Github: https://github.com/EasyDarwin
WEChat: EasyDarwin
Website: http://www.EasyDarwin.org
*/
// EasyPusherDlg.h : header file
//

#pragma once

#include "DlgLocalPanel.h"
#include "DlgRemotePanel.h"
#include "DlgPanel.h"
#include "SourceManager.h"

// EasySkinUI Support
#include "EasySkinManager.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "EasyLinkCtrl.h"
//load ini file 
#include "IniFileLoad.h"

#define MAX_VIDEO_WND 1
#define MSG_LOG WM_USER +0x1001

#define	 MAX_VIDEO_WINDOW_NUM		16

typedef struct __VIDEO_NODE_T
{
	bool		fullscreen;
	int		maximizedId;
	int		selectedId;
	int		channels;
	CDlgPanel	* pDlgVideo;
}VIDEO_NODE_T;


// CEasyPusherDlg dialog
class CEasyPusherDlg : public CEasySkinManager
{
	// Construction
public:
	CEasyPusherDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_EasyPusher_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


protected:
	void		UpdateComponents();

	void CreateVideoComponents(int channels);

	void UpdateVideoComponents(LPRECT lpRect);
	void DeleteVideoComponents();

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint(); 
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL DestroyWindow();
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

private:
	CSourceManager* m_pManager;
	CFont	m_ftSaticDefault;
	CFont	m_ftSaticTittle;
	CEasySkinButton m_btnLocalView;
	CEasySkinButton m_btnPush;
	CEasySkinButton m_btnLiveView;
	CEasyLinkCtrl m_linkEasyDarwinWeb;
	CEasyLinkCtrl m_linkEasyPusherGethub;//CEasyLinkCtrl
	CEdit m_edtVdieoWidth;
	CEdit m_edtVideoHeight;
	CEdit m_edtFPS;
	CEdit m_edtVideoBitrate;
	CEdit m_edtPushBuffer;
	VIDEO_NODE_T	*m_pVideoWindow;		//视频窗口
	CEdit m_edtIp;
	SourceConfigInfo* m_pConfigInfo;
	int m_nSourceCount ;
public:
	afx_msg void OnCbnSelchangeComboSource();
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnPlay();
	LRESULT OnLog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMClickSyslinkEasydarwin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslinkEasyPusherGithub(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboWndMode();

	//EasySkinUI 界面美化
protected:
	//窗口绘制
	virtual void DrawClientArea(CDC*pDC,int nWidth,int nHeight);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void UpdataResource();
public:
	void GetPushServerInfo(ServerURLInfo* pPushServerInfo);
	void GetLocalDevInfo(CComboBox* pCam, CComboBox*  pMic);
	//读取ini配置文件
	int ReadSourceConfigInfo( );
	CString ReadKeyValueFromS(CString strKey,CString strValue);

};
