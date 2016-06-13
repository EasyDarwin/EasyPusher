/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// CaptureAudio.h: interface for the CCaptureAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTUREAUDIO_H__4FD3D541_79D0_47AB_891C_BFE4FE9EA54A__INCLUDED_)
#define AFX_CAPTUREAUDIO_H__4FD3D541_79D0_47AB_891C_BFE4FE9EA54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DshowDef.h"
#include "DShowCaptureAudioAndVideo_Interface.h"


class CCaptureAudio : public DShowCaptureAudio_Interface 
{

public:
	CCaptureAudio();
	virtual ~CCaptureAudio();

private:
	IGraphBuilder *m_pGraphBuilder;//filter grap 由外部传递
	ICaptureGraphBuilder2* m_pCaptureGraphBulid;
	IBaseFilter* m_pBaseFilter;//设备的Filter;
	IVideoWindow* m_pVideoWin; //视频显示窗口
	IMediaControl* m_pMediaCon;//媒体控制
	IBaseFilter *m_pSampleGrabberFilter;
	ISampleGrabber *m_pSampleGrabber;

private:
	BOOL m_bThreadExit;//线程退出标志

	int m_nDataType;
	int m_nChannels;
	int m_nBytesPerSample;//位数16位
	int m_nSampleRate;//44100;,采样率
	int	m_nAudioBufferType;
	int m_nPinType;
	int m_iDeviceId;

private:
	BYTE *m_pDataBuffer;
	int m_nBufferSize;

private:
	HRESULT CreateCaptureGraphBuilder();
	BOOL BindToAudioDev(int deviceId, IBaseFilter **pFilter);
	HRESULT SetAudioFormat(int nChannels,int nBytesPerSample,long nFrequency,int nAudioBufferType,int nPinType);
	HRESULT CreateCaptureSampleGrabber();
	HRESULT StartPreview();
	HRESULT StopPreview();
	void TearDownGraph();
	void NukeDownstream(IGraphBuilder * inGraph, IBaseFilter * inFilter);
	void FreeMediaType(AM_MEDIA_TYPE& mt);
//	void ERR_DEBUG(CString strError);
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);

public:
	void WINAPI SetAudioCaptureData(int iDeviceId,int nChannels,int nBytesPerSample,
		int nSampleRate,int nAudioBufferType,int nPinType, int nDataType, BOOL bIsThread);
	int WINAPI CreateCaptureGraph();
	int WINAPI BulidCaptureGraph();
	void WINAPI BegineCaptureThread();

private:
	BOOL m_bThread;
	HANDLE m_hThreadEvent;

public:
	static UINT OnDataThread(LPVOID lParam);
	void OnThreadDeal();

public:
	CSampleGrabberCB m_cSampleGrabberCB;
	ThreadCalInfo    m_sThreadCalbackInfo;

	void  WINAPI SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster);
	BYTE* WINAPI GetCurrentBuffer(long &lDatasize);

};

#endif // !defined(AFX_CAPTUREAUDIO_H__4FD3D541_79D0_47AB_891C_BFE4FE9EA54A__INCLUDED_)
