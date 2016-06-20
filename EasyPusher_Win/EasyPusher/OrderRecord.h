/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
//  [6/14/2016 SwordTwelve] 
#pragma once
class COrderRecordInfo  
{
public:
	COrderRecordInfo();
	virtual ~COrderRecordInfo();
public:
	int id;			//id 自增
	CString name;	//名称
	COleDateTime starttime; //开始时间
	COleDateTime endtime;	//结束时间
	CString strStartTimeString;//开始时间（字串）
	CString strEndTimeString; //结束时间（字串）
	int	isenable;			//是否可用
	CString intro;			//介绍

	int nState;				//标志 0--计划日期,1--每天 
	int nRunState;			//运行标志状态（-1=过期，0=等待，1=运行中，2=已完成）
	CString property1;
	CString property2;
	CString property3;
};

class COrderRecord
{
public:
	COrderRecord(void);
	~COrderRecord(void);

public:
	CList<COrderRecordInfo,COrderRecordInfo&> m_listOrderRecord;
	BOOL m_bUserOrderRecord;
	BOOL m_nOrderRecordType;//0---本机上的简单定时录制,1---access数据库的定时录制,2---sql数据库的定时录制任务
	int  m_nSubTime;
	BOOL m_bStartLoadList;
	int m_nManualTime;
	CString m_strOrderInfoPath;
	int m_nCheckTimer;
	int m_nCurManualTime;
	int m_nCurRecordIndex;
	int m_nCurRecordType;
	int m_nCurRecordTime;
	BOOL m_bStopManual;//是否按计划停止手动录制操作
private:
	CString ReadKeyValueFromS(CString strKey,CString strValue);
	void CString2OrderRecordInfo(CString strValue,COrderRecordInfo &tmpOrderRecordInfo);
	int GetParamData(CString strValue,CStringArray &arrayValue,CString strFlag);
	CString GetArrayData(CStringArray &array,int nIndex);
	
public:
	COleDateTime CString2CTimer(CString strTimer);
	BOOL LoadOrderRecordData(CString strOrderRcPath);
	int LoadOrderRecordList(CString strOrderInfoPath=_T(""));
	int CheckIsRecordFlag(int nCurrentRecordLen,int nRecordType);
	int GetCurOrderPlanInfo(CString& sName, CString& sStartTime, CString& sEndTime, CString &strFilePath1,CString &strFilePath2,CString &strFilePath3);

	int CheckIsRecordFlag_List(COleDateTime  curTime,int nCheckType,CUIntArray  &arrayTimeIn);

	int CheckIsRecordFlag_Index(COleDateTime  curTime,int nCheckType,int nCurIndex);
	void SetOrderRecordFlag(int nRecFlag);
	int CheckIsListFlag(int nCurRecFlag, int& nActiveOrderId);
	BOOL CheckOrderIni(CString strPath);
	int LoadOrderRecordList(CString strOrderInfoPath,CList<COrderRecordInfo,COrderRecordInfo&> &listOrderRecord);
	int GetOrderRecInfoById(int nIndex, COrderRecordInfo& tmpOrderRecordInfo);
	int SetOrderRecListRunState(int nIndex, int nRunState);
	int UpdateListInfoId();
};
