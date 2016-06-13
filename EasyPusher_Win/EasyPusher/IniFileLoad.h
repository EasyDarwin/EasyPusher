/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// IniFileLoad.h: interface for the CIniFileLoad class.
// 载入INI配置信息。
// Add by SwordTwelve
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILELOAD_H__18771C8D_4F28_4B96_9716_9B009AE2D46A__INCLUDED_)
#define AFX_INIFILELOAD_H__18771C8D_4F28_4B96_9716_9B009AE2D46A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ALLSECTIONS 2048 //全部的段名
#define MAX_SECTION 260 //一个段名长度
#define MAX_ALLKEYS 12000 //全部的键名
#define MAX_KEY 512 //一个键名长度

class CIniFileLoad  
{
public:
	void LoadDataIntMicPosDev(CString strSection,CString strKey,int &nMicPosIndex,int &nMicCamNum);
	CString GetFirstParam(CString &strInfo);
	void LoadDataIntAngle(CString strSection,CString strKey,int &nYPos,int &nZPos,int &nFocusPos);
	int GetParamData(CString strValue,CStringArray &arrayValue,CString strFlag=_T(","));
	CString GetArrayData(CStringArray &array,int nIndex);
	void SetDataDouble(CString strSection,CString strKey,double nValue);
	void SetDataInt(CString strSection,CString strKey,int nValue);
	void LoadDataDouble(CString strSection,CString strKey,double  &fValue);
	void LoadDataInt(CString strSection,CString strKey,int  &nValue);
	void SetString(CString strSection,CString strKey,CString strValue);
	void LoadString(CString strSection,CString strKey,CString &strValue);
	void SetFileName(CString strFileName);
	void InitIniFilePath();
	int GetSections(CStringArray& arrSection);
	int GetKeyValues(CStringArray& arrKey,CStringArray& arrValue, LPCTSTR lpSection);
	CIniFileLoad();
	virtual ~CIniFileLoad();

private:
	CString m_strCurPath;//当前文件夹路径
	CString m_strSettingPath;//当前配置文件路径
};

#endif // !defined(AFX_INIFILELOAD_H__18771C8D_4F28_4B96_9716_9B009AE2D46A__INCLUDED_)
