#pragma once

// EasySkinUI Support
#include "EasySkinManager.h"
#include "SourceManager.h"

// CSettingDlg dialog

class CSettingDlg : public CEasySkinManager
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	void PreSetting(AVCapParamInfo sParamInfo);
	void SaveSetting(AVCapParamInfo& sParamInfo);

private:
	AVCapParamInfo m_sParamInfo;

public:
	virtual BOOL OnInitDialog();
};
