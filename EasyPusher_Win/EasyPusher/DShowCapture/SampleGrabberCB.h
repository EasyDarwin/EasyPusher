/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <qedit.h> 
#include ".\DShowCapture\DirectShow\Include\qedit.h"

#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
//#include <Streams.h>
#include ".\DShowCapture\DirectShow\Streams.h"

#include "DShowCaptureAudioAndVideo_Interface.h"



class CSampleGrabberCB:public ISampleGrabberCB
{ 

public: 
	typedef struct _callbackinfo 
	{
		double dblSampleTime;
		long lBufferSize;
		BYTE *pBuffer;
		BOOL bHaveData;
	} CALLBACKINFO;
	CALLBACKINFO g_cbInfo;

	RealDataCallback m_realDataCallback;
	void* m_pMaster;

	int now_tick;
	int first_tick;
	CSampleGrabberCB();
	STDMETHODIMP_(ULONG)   AddRef();
	STDMETHODIMP_(ULONG)   Release();
	STDMETHODIMP   QueryInterface(REFIID   riid,   void   **   ppv);
	STDMETHODIMP   SampleCB(   double   SampleTime,   IMediaSample   *   pSample   );
	STDMETHODIMP   BufferCB(   double   dblSampleTime,   BYTE   *   pBuffer,   long   lBufferSize   );
	void SetDataInfo(int nIndex,int nDataType);
private:
	int m_nIndex;
	int m_nDataType;
};
