/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "Stdafx.h"
#include "SampleGrabberCB.h"
//#include "Master.h"

//CMaster* g_pMaster = CMaster::Instance(NULL); 

CSampleGrabberCB::CSampleGrabberCB() 
{
	g_cbInfo.dblSampleTime=0;
	g_cbInfo.lBufferSize=0;
	g_cbInfo.pBuffer=NULL;
	g_cbInfo.bHaveData=FALSE;

	now_tick=0;
	first_tick=0;
	m_nIndex=-1;
	m_nDataType= -1;
	m_realDataCallback = NULL;
	m_pMaster = NULL;

}

STDMETHODIMP_(ULONG)   CSampleGrabberCB::AddRef()
{  
	return   2;   
} 
STDMETHODIMP_(ULONG)   CSampleGrabberCB::Release()   
{   
	return   1;   
}  
STDMETHODIMP   CSampleGrabberCB::QueryInterface(REFIID   riid,   void   **   ppv)
{
	CheckPointer(ppv,E_POINTER); 

	if(   riid   ==   IID_ISampleGrabberCB   ||   riid   ==   IID_IUnknown   )   
	{ 
		*ppv   =   (void   *)   static_cast <ISampleGrabberCB*>   (   this   ); 
		return   NOERROR; 
	}         

	return   E_NOINTERFACE; 
}
STDMETHODIMP   CSampleGrabberCB::SampleCB(   double   SampleTime,   IMediaSample   *   pSample   )
{
	static long counter = 0;

	if (pSample)
	{
		TRACE( "Sample received = %05ld  Clock = %ld  Length=%ld\r\n", 
			counter++, SampleTime,
			pSample->GetActualDataLength() ); 
	}
	else
	{
		;   //_tprintf( TEXT("Invalid IMediaSample passed to SampleCB!\r\n") );
	}

	return 0;
}

//统一的回调函数
STDMETHODIMP   CSampleGrabberCB::BufferCB( double dblSampleTime, BYTE *pBuffer, long lBufferSize )
{

	if   (!pBuffer) 
		return   E_POINTER; 

	now_tick=::GetTickCount();

	first_tick = now_tick;
	if (m_nDataType == 1)//视频
	{
 //		TRACE("BufferCB:%d-%d-%d-%d\r\n", m_nIndex, m_nDataType, now_tick - first_tick, lBufferSize);
	}
	else if (m_nDataType == 2)//音频
	{
//		TRACE("BufferCB:%d-%d-%d-%d\r\n", m_nIndex, m_nDataType, now_tick - first_tick, lBufferSize);
	}

	if(pBuffer==NULL||lBufferSize<=0)
		return 0;

	if (m_realDataCallback && m_pMaster)
	{
		m_realDataCallback(m_nIndex, pBuffer, lBufferSize, (RealDataStreamType)m_nDataType,NULL, m_pMaster);
		return 0;
	}
// 	else
// 	{
// 		AfxMessageBox(_T("回调函数未设置或者回调函数设置失败。"));
// 		return 0;
// 	}
// 	if (m_pMaster)
// 	{	
// 		if(pBuffer==NULL||lBufferSize<=0)
// 			return 0;
// 		//原始数据的显示及处理
// 		// m_nIndex：设备编号：音频-1，视频0---N
// 		m_pMaster->RealDataManager(m_nIndex, pBuffer, lBufferSize, (RealDataStreamType)m_nDataType,NULL);
// 		return 0;
// 	}

	if(   g_cbInfo.lBufferSize   <   lBufferSize   ) 
	{ 
		delete   []   g_cbInfo.pBuffer; 
		g_cbInfo.pBuffer   =   NULL; 
		g_cbInfo.lBufferSize   =   0; 
		g_cbInfo.bHaveData=FALSE;
	} 

	//   Since   we   can 't   access   Windows   API   functions   in   this   callback,   just 
	//   copy   the   bitmap   data   to   a   global   structure   for   later   reference. 
	g_cbInfo.dblSampleTime   =   dblSampleTime; 

	//   If   we   haven 't   yet   allocated   the   data   buffer,   do   it   now. 
	//   Just   allocate   what   we   need   to   store   the   new   bitmap. 
	if   (!g_cbInfo.pBuffer) 
	{ 
		g_cbInfo.pBuffer   =   new   BYTE[lBufferSize]; 
		g_cbInfo.lBufferSize   =   lBufferSize; 
	} 

	if(   !g_cbInfo.pBuffer   ) 
	{ 
		g_cbInfo.lBufferSize   =   0;
		g_cbInfo.bHaveData=FALSE;
		return   E_OUTOFMEMORY; 
	} 

	//   Copy   the   bitmap   data   into   our   global   buffer 
	memcpy(g_cbInfo.pBuffer,   pBuffer,   lBufferSize);
	g_cbInfo.bHaveData=TRUE;

	return   0;
}
/*
nDataType:数据类型1--音频，2--视频数据
nIndex：设备编号：音频-1，视频0---N
*/
void  CSampleGrabberCB::SetDataInfo(int nIndex,int nDataType)
{
	m_nIndex=nIndex;
	m_nDataType= nDataType;
}