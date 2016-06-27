/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "stdafx.h"
#include "OrderRecord.h"
#include "EasyPusher.h"
#include "IniFileLoad.h"

COrderRecordInfo::COrderRecordInfo()
{

}
COrderRecordInfo::~COrderRecordInfo()
{

}
COrderRecord::COrderRecord(void)
{
	m_bStopManual=FALSE;
	m_nCurRecordIndex=-1;
}

COrderRecord::~COrderRecord(void)
{
}

BOOL COrderRecord::LoadOrderRecordData(CString strOrderRcPath)
{
	CString strFilePath("");
	strFilePath = GET_MODULE_FILE_INFO.strPath + _T("ini\\config.ini");
	CIniFileLoad IniFileLoad;
	IniFileLoad.SetFileName(strFilePath);
	int nValue = 0;
	//是否使用定时录制
	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("bUserOrderRecord"),nValue);
	if(nValue<=0)
	{
		m_bUserOrderRecord = FALSE;
		if(nValue<0)
		{
			nValue=0;
			IniFileLoad.SetDataInt(_T("OrderRecord"), _T("bUserOrderRecord"),nValue);
		}
		return FALSE;

	}
	else
	{
		m_bUserOrderRecord = TRUE;
	}
	//使用定时推送的类型  0=本地摄像头和声卡 1=网络RTSP流 2=屏幕推送 3=文件推送
	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("nOrderType"),m_nOrderRecordType);
	if(m_nOrderRecordType<0)
	{
		m_nOrderRecordType = 0;
		IniFileLoad.SetDataInt(_T("OrderRecord"), _T("nOrderType"),m_nOrderRecordType);
	}

	//手动录像的时间,以秒为单位,如果为-1则不限制手动录制
	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("nManualTime"),m_nManualTime);

	//时间偏差
	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("nSubTime"),m_nSubTime);
	if(m_nSubTime<0)
	{
		m_nSubTime = 0;
		IniFileLoad.SetDataInt(_T("OrderRecord"), _T("nSubTime"),m_nSubTime);
	}

	//是否启动时载入数据
	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("bStartLoadList"),nValue);
	if(nValue<=0)
	{
		m_bStartLoadList = FALSE;
		if(nValue<0)
		{
			IniFileLoad.SetDataInt(_T("OrderRecord"), _T("bStartLoadList"),0);
		}
	}
	else
	{
		m_bStartLoadList = TRUE;
	}

	m_strOrderInfoPath = GET_MODULE_FILE_INFO.strPath + _T("ini\\")+strOrderRcPath;
	//IniFileLoad.LoadString(_T("OrderRecord"), _T("strOrderInfoPath"),m_strOrderInfoPath);

	IniFileLoad.LoadDataInt(_T("OrderRecord"),_T("nCheckTimer"),m_nCheckTimer);
	if(m_nCheckTimer<=0)
	{
		m_nCheckTimer=1000;
		IniFileLoad.SetDataInt(_T("OrderRecord"), _T("nCheckTimer"), m_nCheckTimer);
	}


	IniFileLoad.LoadDataInt(_T("OrderRecord"), _T("bStopManual"), nValue);
	if(nValue<=0)
	{
		m_bStopManual=FALSE;
		if(nValue<0)
		{
			IniFileLoad.SetDataInt(_T("OrderRecord"), _T("bStopManual"),0);
		}
	}
	else
	{
		m_bStopManual=TRUE;
	}
	return 1;
}

int COrderRecord::LoadOrderRecordList(CString strOrderInfoPath)
{
	
	m_listOrderRecord.RemoveAll();
	if(strOrderInfoPath.IsEmpty())
		return 0;
	return LoadOrderRecordList(strOrderInfoPath,m_listOrderRecord);
/*	CIniFileLoad IniFileLoad;

	IniFileLoad.SetFileName(strOrderInfoPath);
	
	CStringArray arrKey,arrValue;
	CString strSection = "OrderRecordList";

	int nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);
	if(nArrayKeyCount<=0)
	{
		strSection=_T("");
		nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);
	}
	CString strKey="";
	CString strValue="";
	for(int k=0;k<nArrayKeyCount;k++)
	{
		strKey = arrKey.GetAt(k);
		strValue = arrValue.GetAt(k);
		if(strValue.IsEmpty())
		{
			continue;
		}
		COrderRecordInfo tmpOrderRecordInfo;
		CString2OrderRecordInfo(strValue,tmpOrderRecordInfo);
		tmpOrderRecordInfo.name=strKey;
		m_listOrderRecord.AddTail(tmpOrderRecordInfo);
	}
	return nArrayKeyCount;*/
}

CString COrderRecord::ReadKeyValueFromS(CString strKey,CString strValue)
{
	CString strRet(_T(""));
	if(strValue.IsEmpty()||strKey.IsEmpty())
	{
		return strRet;
	}
	//得到name值
	int nKey=strKey.GetLength();
	if(nKey<=0)
		return strRet;
	//查找关键值
	int nFind = strValue.Find(strKey);
	if(nFind<0) return strRet;

	//找到关键值后找关键值后面参数
	CString strTmp0=strValue.Mid(nFind+nKey);

	int nFind1 = strTmp0.Find(_T("\""));
	if(nFind1<0)
	{
		return strRet;
	}
	CString strTmp1=strTmp0.Mid(nFind1+1);
	int nFind2 = strTmp1.Find(_T("\""));
	if(nFind2<0)
	{
		strRet=strTmp1.Left(nFind1);
	}
	else
		strRet=strTmp1.Left(nFind2);
	return strRet;	
}

CString COrderRecord::GetArrayData(CStringArray &array,int nIndex)
{
	int nCount = array.GetSize();
	if(nIndex<0||nIndex>=nCount)
		return _T("");
	CString strValue = array.GetAt(nIndex);
	return strValue;
}

int COrderRecord::GetParamData(CString strValue,CStringArray &arrayValue,CString strFlag)
{
	int nPos=0;
	while(nPos>=0)
	{
		nPos = strValue.Find(strFlag);
		if(nPos>=0)
		{
			CString strTmp;
			strTmp = strValue.Left(nPos);
			strTmp.TrimLeft();
			strTmp.TrimRight();
			arrayValue.Add(strTmp);
			strValue.Delete(0, nPos+1);
			strValue.TrimLeft();
			strValue.TrimRight();
		}
		else
		{
			strValue.TrimLeft();
			strValue.TrimRight();
			CString strTmp;
			strTmp = strValue;
			arrayValue.Add(strTmp);
		}
	}
	return arrayValue.GetSize();
}

/*
开始时间+结束时间+标志( 0--计划日期,1--每天)+是否可用(isenable)+名称(name)+	介绍(intro)+教师(teacherid)		//
CString strValue的格式为:2010-04-12 13:45:12,2010-03-17 14:45:12,1
*/
void COrderRecord::CString2OrderRecordInfo(CString strValue,COrderRecordInfo &tmpOrderRecordInfo)
{
	if(strValue.IsEmpty())
		return ;
	CStringArray arrayValue;

	int nCount = GetParamData(strValue,arrayValue,_T(","));
	CString strName=_T("");
	CString strStartTime = GetArrayData(arrayValue,0);
	CString strEndTime = GetArrayData(arrayValue,1);
	CString strProperty1 =  GetArrayData(arrayValue,2);//Eg: 课程名称,
	CString strProperty2 = GetArrayData(arrayValue,3); //Eg: 教师名称,
	CString strProperty3 =  GetArrayData(arrayValue,4);//Eg: 授课地点
	
	tmpOrderRecordInfo.nState=0;
	if(!strStartTime.IsEmpty())
	{
		tmpOrderRecordInfo.strStartTimeString = strStartTime;
		tmpOrderRecordInfo.starttime = CString2CTimer(strStartTime);
		if(tmpOrderRecordInfo.starttime.GetYear()<=1900)
		{
			tmpOrderRecordInfo.nState=1;
		}	
	}
	if(!strEndTime.IsEmpty())
	{
		tmpOrderRecordInfo.strEndTimeString = strEndTime;
		tmpOrderRecordInfo.endtime = CString2CTimer(strEndTime);
	}
	tmpOrderRecordInfo.name = strName;
	tmpOrderRecordInfo.property1 = strProperty1;
	tmpOrderRecordInfo.property2 = strProperty2;
	tmpOrderRecordInfo.property3 = strProperty3;

}

/*
函数：CString2CTimer
功能：将CString 型时间数据转换为CTimer型时间数据
参数：strTimer：格式为：_20090921102106，或者是：20090921102106
*/
COleDateTime COrderRecord::CString2CTimer(CString strTimer)
{
	COleVariant   VariantTime;   
	VariantTime   =   strTimer;   
	try 
	{
		VariantTime.ChangeType(VT_DATE);   
	}
	catch(COleException* e)
	{
		e->ReportError();

	}
	COleDateTime   DataTime   =   VariantTime;

	return DataTime;
}



void COrderRecord::SetOrderRecordFlag(int nRecordFlag)
{
	if(nRecordFlag==0)
	{
		m_nCurRecordIndex=-1;
	}
	else if(nRecordFlag==1||nRecordFlag==2)
	{
		
	}

}
/*
函数:CheckIsRecordFlag
功能:检测是否有录制或者停止录制状态命令
备注:返回:0--没有任何操作,1---开始,2---停止
	nCurrentRecordTime:当前录像时间,curTimer:当前时间
	0--手动停止 nRecordType = 1 手动点击按钮开始(定时),2--时间检测开始(列表)

*/
int COrderRecord::CheckIsListFlag(int nCurRecFlag, int& nActiveOrderId)
{
	COleDateTime curTime=curTime.GetCurrentTime();
	BOOL bStopManual=m_bStopManual;//是否停止手动录制
	if(nCurRecFlag==1)//推送中
	{
		//TRACE("录制中.检测..%d\r\n",m_nCurRecordIndex);
		if(m_nCurRecordIndex<0&&bStopManual)
		{		
			int nCheckType=1;//0--开始时间表中，1--结束时间表
			CUIntArray  arrayTimeIn;
			arrayTimeIn.RemoveAll();
			int nInCount = CheckIsRecordFlag_List(curTime,nCheckType,arrayTimeIn);
			if(nInCount>0)
			{
				nActiveOrderId = arrayTimeIn.GetAt(0);
				if (nActiveOrderId>-1)
				{	
					//置为已停止状态
					SetOrderRecListRunState(nActiveOrderId, 2);
					
				}
				return 2;
			}
		}
		else if(m_nCurRecordIndex>=0)
		{

			int nCheckType=1;
			int nRet = CheckIsRecordFlag_Index(curTime,nCheckType,m_nCurRecordIndex);
			if(nRet>=1)
			{
				nActiveOrderId = m_nCurRecordIndex;
				m_nCurRecordIndex=-1;
				//置为已停止状态
				SetOrderRecListRunState(nActiveOrderId, 2);
				return 2;
			}
		}
		return 0;
	}
	else if(nCurRecFlag==0)//当前停止，开启推送
	{
		if(m_nCurRecordIndex<0)//当前不在录制计划中
		{
			//检测开始录制的时间
			int nCheckType=0;//0--开始时间表中，1--结束时间表
			CUIntArray  arrayTimeIn;
			arrayTimeIn.RemoveAll();
			int nInCount = CheckIsRecordFlag_List(curTime,nCheckType,arrayTimeIn);
			//TRACE("停止中.检测..%d\r\n",nInCount);	
			if(nInCount>0)
			{
				m_nCurRecordIndex=arrayTimeIn.GetAt(0);	
				nActiveOrderId = m_nCurRecordIndex;
				//置为运行中状态
				SetOrderRecListRunState(nActiveOrderId, 1);
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

/*
-1-----不在列表中
返回0--不在指定时间范围内
返回1--在指定的时间范围内
*/
int COrderRecord::CheckIsRecordFlag_Index(COleDateTime  curTime,int nCheckType,int nCurIndex)
{
	if(nCurIndex<0)
		return -1;
	POSITION pos = m_listOrderRecord.FindIndex(nCurIndex);
	if(pos==NULL||pos<0)
	{
		return -1;
	}
	COrderRecordInfo tOrderRecordInfo;
	tOrderRecordInfo = m_listOrderRecord.GetAt(pos);
	COleDateTime useDateTime=tOrderRecordInfo.starttime;

	if(nCheckType==0)//开始时间
	{
		useDateTime=tOrderRecordInfo.starttime;
	}
	else if(nCheckType==1)//结束时间
	{
		useDateTime=tOrderRecordInfo.endtime;
	}

	BOOL bInTime=FALSE;
	if(tOrderRecordInfo.nState==0)//当天
	{
		COleDateTimeSpan  startSpan;
		startSpan = curTime-useDateTime;

		double nSeconds = startSpan.GetTotalSeconds();
		int dSub = abs(m_nSubTime-abs((int)nSeconds));
		//int dSub = (abs)((int)nSeconds);
	
		if(dSub<= m_nSubTime)
		{
			bInTime=TRUE;
		}
	}
	else if(tOrderRecordInfo.nState==1)//每天
	{

		int nHours1 = useDateTime.GetHour();
		int nMunites1 = useDateTime.GetMinute();
		int nSeconds1 = useDateTime.GetSecond();

		int nHours0 = curTime.GetHour();
		int nMunites0 = curTime.GetMinute();
		int nSeconds0 = curTime.GetSecond();

		int dSub = abs(nSeconds1-nSeconds0);
		if(nHours0==nHours1&&nMunites1==nMunites0&&(dSub<=m_nSubTime))
		{
			bInTime=TRUE;
		}
	}
	return bInTime;
}
/*
函数:CheckIsRecordFlag_List(COleDateTime   curTime)
功能:检测列表中是否有录制开始任务和结束任务
	0--没有,1--开始录制,2--结束录制
*/
int COrderRecord::CheckIsRecordFlag_List(COleDateTime  curTime,int nCheckType,CUIntArray  &arrayTimeIn)
{
	int nCount = m_listOrderRecord.GetCount();
	if(nCount<=0)
	{
		return -1;
	}
	arrayTimeIn.RemoveAll();
	for(int i=0;i<nCount;i++)
	{
		int nRet = CheckIsRecordFlag_Index(curTime,nCheckType,i);
		if(nRet>=1)
		{
			arrayTimeIn.Add(i);
		}
	}
	return arrayTimeIn.GetSize();
}

int COrderRecord::GetCurOrderPlanInfo(CString& sName, CString& sStartTime, CString& sEndTime, CString &strFilePath1,CString &strFilePath2,CString &strFilePath3)
{
	if(m_nCurRecordIndex<=-1)
	{
		return -1;
	}
	POSITION pos = m_listOrderRecord.FindIndex(m_nCurRecordIndex);
	if(pos==NULL||pos<0)
	{
		return -1;
	}
	COrderRecordInfo tOrderRecordInfo;
	tOrderRecordInfo = m_listOrderRecord.GetAt(pos);

	sName =tOrderRecordInfo.name;
	sStartTime = tOrderRecordInfo.strStartTimeString;
	sEndTime = tOrderRecordInfo.strEndTimeString;
	strFilePath1=tOrderRecordInfo.property1;
	strFilePath2=tOrderRecordInfo.property2;
	strFilePath3=tOrderRecordInfo.property3;
	return 1;	
}

BOOL COrderRecord::CheckOrderIni(CString strPath)
{

	if(strPath.IsEmpty())
		return FALSE;
	CIniFileLoad IniFileLoad;

	IniFileLoad.SetFileName(strPath);
	
	CStringArray arrKey,arrValue;
	CString strSection = _T("OrderRecordList");

	int nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);
	if(nArrayKeyCount<=0)
	{
		strSection=_T("");
		nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);
	}
	if(nArrayKeyCount<=0)
	{
		return FALSE;
	}
	return TRUE;
}

int COrderRecord::LoadOrderRecordList(CString strOrderInfoPath,CList<COrderRecordInfo,COrderRecordInfo&> &listOrderRecord)
{
	listOrderRecord.RemoveAll();
	if(strOrderInfoPath.IsEmpty())
		return 0;
	CIniFileLoad IniFileLoad;

	IniFileLoad.SetFileName(strOrderInfoPath);
	
	CStringArray arrKey,arrValue;
	CString strSection = _T("OrderRecordList");

	int nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);//找出等号两边的值
	if(nArrayKeyCount<=0)
	{
		strSection=_T("");
		nArrayKeyCount = IniFileLoad.GetKeyValues(arrKey,arrValue,strSection);
	}
	CString strKey=_T("");
	CString strValue=_T("");
	int nItemId = 0;
	for(int k=0;k<nArrayKeyCount;k++)
	{
		strKey = arrKey.GetAt(k);
		strValue = arrValue.GetAt(k);
		if(strValue.IsEmpty())
		{
			continue;
		}
		COrderRecordInfo tmpOrderRecordInfo;
		CString2OrderRecordInfo(strValue,tmpOrderRecordInfo);
		tmpOrderRecordInfo.id = nItemId;
		nItemId++;
		tmpOrderRecordInfo.name=strKey;
		if (tmpOrderRecordInfo.nState==0)//当天
		{	
			COleDateTime  curTime = curTime.GetCurrentTime();
			COleDateTimeSpan  startSpan;
			startSpan = curTime - tmpOrderRecordInfo.starttime;
			
			double nSeconds = startSpan.GetTotalSeconds();

			if (nSeconds>5)//过去的时间
			{
				tmpOrderRecordInfo.nRunState = -1;//初始化时，默认运行状态为 过去式。。。
			} 
			else
			{		
				tmpOrderRecordInfo.nRunState = 0;//初始化时，默认运行状态为 等待。。。
			}
		}
		else if (tmpOrderRecordInfo.nState==1)//每天
		{
			tmpOrderRecordInfo.nRunState = 0;//初始化时，默认运行状态为 等待。。。
		}
		listOrderRecord.AddTail(tmpOrderRecordInfo);//载入列表
	}
	return nArrayKeyCount;
}

int COrderRecord::GetOrderRecInfoById(int nIndex, COrderRecordInfo& tmpOrderRecordInfo)
{
	if(nIndex<0)
		return -1;
	POSITION pos = m_listOrderRecord.FindIndex(nIndex);
	if(pos==NULL||pos<0)
	{
		return -1;
	}

	tmpOrderRecordInfo = m_listOrderRecord.GetAt(pos);

	return 1;
}

int COrderRecord::SetOrderRecListRunState(int nIndex, int nRunState)
{
	if(nIndex<0)
		return -1;
	POSITION pos = m_listOrderRecord.FindIndex(nIndex);
	if(pos==NULL||pos<0)
	{
		return -1;
	}
	COrderRecordInfo tmpOrderRecordInfo;
	tmpOrderRecordInfo = m_listOrderRecord.GetAt(pos);
	tmpOrderRecordInfo.nRunState = nRunState;
	m_listOrderRecord.SetAt(pos, tmpOrderRecordInfo);
	return 1;
}

int COrderRecord::UpdateListInfoId()
{
	int nCount = m_listOrderRecord.GetCount();
	if(nCount<=0)
	{
		return -1;
	}

	for(int i=0;i<nCount;i++)
	{
		POSITION pos = m_listOrderRecord.FindIndex(i);
		if(pos==NULL||pos<0)
		{
			return -1;
		}
		COrderRecordInfo tmpOrderRecordInfo;
		tmpOrderRecordInfo = m_listOrderRecord.GetAt(pos);

		tmpOrderRecordInfo.id = i;

		m_listOrderRecord.SetAt(pos, tmpOrderRecordInfo);
	}
	return 1;
}