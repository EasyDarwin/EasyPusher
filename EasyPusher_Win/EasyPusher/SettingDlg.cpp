// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyPusher.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CEasySkinDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CEasySkinManager(CSettingDlg::IDD, pParent, en_Wnd_CloseBox)
{
}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CEasySkinManager::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CEasySkinManager)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingDlg message handlers

void CSettingDlg::OnBnClickedOk()
{
	int nWidth = 640;
	int nHeight = 480;
	int nFps = 25;
	int nBitrate = 2048;
	int nSampleRate = 44100;
	int nChannels = 2;

	char szWidth[128] = {0,};
	wchar_t wszWidth[128] = {0,};
	char szHeight[128] = {0,};
	wchar_t wszHeight[128] = {0,};
	char szFPS[128] = {0,};
	wchar_t wszFPS[128] = {0,};
	char szBitrate[128] = {0,};
	wchar_t wszBitrate[128] = {0,};

	char szSampleRate[128] = {0,};
	wchar_t wszSampleRate[128] = {0,};
	char szChannels[128] = {0,};
	wchar_t wszChannels[128] = {0,};

	CEdit* pEdit = NULL;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VWIDTH);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszWidth, sizeof(wszWidth));
		if (wcslen(wszWidth) < 1)		
			return;
		__WCharToMByte(wszWidth, szWidth, sizeof(szWidth)/sizeof(szWidth[0]));
		nWidth = atoi(szWidth);
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VHEIGHT);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszHeight, sizeof(wszHeight));
		if (wcslen(wszHeight) < 1)		
			return;
		__WCharToMByte(wszHeight, szHeight, sizeof(szHeight)/sizeof(szHeight[0]));
		nHeight = atoi(szHeight);
	}
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FPS);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszFPS, sizeof(wszFPS));
		if (wcslen(wszFPS) < 1)		
			return;
		__WCharToMByte(wszFPS, szFPS, sizeof(szFPS)/sizeof(szFPS[0]));
		nFps = atoi(szFPS);
	}
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BITRATE);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszBitrate, sizeof(wszBitrate));
		if (wcslen(wszBitrate) < 1)		
			return;
		__WCharToMByte(wszBitrate, szBitrate, sizeof(szBitrate)/sizeof(szBitrate[0]));
		nBitrate = atoi(szBitrate);
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ASAMPLERATE);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszSampleRate, sizeof(wszSampleRate));
		if (wcslen(wszSampleRate) < 1)		
			return;
		__WCharToMByte(wszSampleRate, szSampleRate, sizeof(szSampleRate)/sizeof(szSampleRate[0]));
		nSampleRate = atoi(szSampleRate);	
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ACHANNELS);
	if (pEdit)
	{
		pEdit->GetWindowTextW(wszChannels, sizeof(wszChannels));
		if (wcslen(wszChannels) < 1)		
			return;
		__WCharToMByte(wszChannels, szChannels, sizeof(szChannels)/sizeof(szChannels[0]));
		nChannels = atoi(szChannels);	
	}

	m_sParamInfo.nVWidth = nWidth;
	m_sParamInfo.nVHeight = nHeight;
	m_sParamInfo.nFps = nFps;
	m_sParamInfo.nBitrate = nBitrate;
	m_sParamInfo.nASampleRate = nSampleRate;
	m_sParamInfo.nAChannels = nChannels;

	CComboBox* pCombo = (CComboBox*) GetDlgItem ( IDC_COMBO_COLORFORMAT );
	if (pCombo)
	{
		int nSel = pCombo->GetCurSel();
		if (nSel == 0)
		{
			strcpy_s(m_sParamInfo.strColorFormat , "YUY2");
		} 
		else
		{
			strcpy_s(m_sParamInfo.strColorFormat , "RGB24");
		}
	}
	OnOK();
}

void CSettingDlg::PreSetting(AVCapParamInfo sParamInfo)
{
	memcpy(&m_sParamInfo, &sParamInfo, sizeof(AVCapParamInfo));
}

void CSettingDlg::SaveSetting(AVCapParamInfo& sParamInfo)
{
	memcpy(&sParamInfo , &m_sParamInfo, sizeof(AVCapParamInfo));

}


BOOL CSettingDlg::OnInitDialog()
{
	CEasySkinManager::OnInitDialog();

	//Init code add here
	CString strTempString = _T("");

	CEdit* pEdit = NULL;

	strTempString.Format(_T("%d"), m_sParamInfo.nVWidth);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VWIDTH);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}
	strTempString.Format(_T("%d"), m_sParamInfo.nVHeight);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VHEIGHT);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}
	strTempString.Format(_T("%d"), m_sParamInfo.nFps);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FPS);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}

	strTempString.Format(_T("%d"), m_sParamInfo.nBitrate);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BITRATE);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}

	strTempString.Format(_T("%d"), m_sParamInfo.nASampleRate);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ASAMPLERATE);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}
	strTempString.Format(_T("%d"), m_sParamInfo.nAChannels);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ACHANNELS);
	if (pEdit)
	{
		pEdit->SetWindowText(strTempString);
	}

	CComboBox* pCombo = (CComboBox*) GetDlgItem ( IDC_COMBO_COLORFORMAT );
	if (pCombo)
	{
		pCombo->AddString(_T("YUY2"));
		pCombo->AddString(_T("RGB24"));
	}

	CString strDataType = _T("");
	strDataType = m_sParamInfo.strColorFormat;
	if (strDataType == _T("YUY2"))
	{
		pCombo->SetCurSel(0);
	}
	else //д╛хо==RGB24
	{
		pCombo->SetCurSel(1);

	}
	 
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
