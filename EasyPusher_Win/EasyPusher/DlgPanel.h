/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

#include "DlgVideo.h"
#include "SourceManager.h"
#include "afxwin.h"
#include "SettingDlg.h"
#include "OrderRecord.h"

#define MSG_ORDER_RUN WM_USER+1001

// CDlgPanel 对话框
class CEasyPusherDlg;

class CDlgPanel : public CEasySkinDialog
{
	DECLARE_DYNAMIC(CDlgPanel)

public:
	CDlgPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_PANEL };

protected:
	CDlgVideo		*pDlgVideo;
	CEdit	*m_pEdtServerIP;		
	CEdit	*m_pEdtServerPort;		
	CEdit	*m_pEdtServerStream;		
	CComboBox* m_pCmbType;//直播/推送切选
	CComboBox* m_pCmbSourceType;//源类型选择
	CComboBox* m_pCmbCamera;
	CComboBox* m_pCmbMic;
	CComboBox* m_pCmbScreenMode;
	CEdit*	m_pEdtRtspStream;		
	CEdit*	m_pEditStartTime;
	CEdit*	m_pEditEndTime;

	CEasySkinButton m_btnStart;
	CEasyPusherDlg* m_pMainDlg;
	//EASY_CHANNEL_INFO_T	easyChannelInfo;
	//本地音视频参数设置
	AVCapParamInfo m_sAVCapParamInfo;

protected:
	//UI 界面绘制
	virtual void DrawClientArea(CDC*pDC,int nWidth,int nHeight);
	void		UpdateComponents();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboPannelType();
	afx_msg void OnCbnSelchangeComboPannelSource();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboCapscreenMode();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	CWnd* GetDlgVideo();
	void SetMainDlg(CEasyPusherDlg* pMainDlg, int nId);
	void UpdataResource();
	//流名称格式化
	void FormatStreamName(char* sStreamName);
	void ProcessVideoWndMenuMsg(int nId);
	//打开MP4文件
	CString OpenMp4File();
	//加载界面配置信息
	void LoadSourceConfigInfo( SourceConfigInfo*  pSourceInfo );
	
	// 计划相关函数 [6/15/2016 SwordTwelve]
	// 函数:LoadOrderRecordInfo
	// 功能:载入定时控制信息
	void LoadOrderRecordInfo(CString strOrderRcPath);
	//检测计划列表
	void CheckOrderList();

private:
	int m_nWndId;//自己的窗口Id
	CFont	m_ftSaticDefault;
	SourceConfigInfo  m_sSourceInfo;
	COrderRecord* m_pOrderRecord;
public:
	CSourceManager* m_pManager;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnOrderRun(WPARAM wparam, LPARAM lparam) ; 

};

