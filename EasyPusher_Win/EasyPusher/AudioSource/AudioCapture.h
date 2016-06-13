/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

//#include <streams.h>
#include ".\DShowCapture\DirectShow\Include\Qedit.h"
#include <initguid.h>
#include <uuids.h>

//#pragma comment(lib, "strmbasd.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "winmm.lib")

class CAudioCapture
{
public:
	CAudioCapture(void);
	virtual ~CAudioCapture(void);

	IGraphBuilder* m_pGB ;
	IBaseFilter* m_pInputDevice;
	IBaseFilter* m_pAudioSender;
	//IMediaControl *m_pMC;

	HRESULT CreateInputDevice();
};

