/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// DShowCaptureAudioAndVideoDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "CaptureAudio.h"
#include "CaptureVideo.h"
#include "SampleGrabberCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//输出函数在此定义

//创建控制接口指针
LPVideoCapturer	APIENTRY Create_VideoCapturer()
{
	CCaptureVideo* lpCaptureVideo = NULL; 
	lpCaptureVideo = new CCaptureVideo();

	return (LPVideoCapturer)lpCaptureVideo;
}

//销毁控制接口指针
void APIENTRY Release_VideoCapturer(LPVideoCapturer lpVideoCapturer)
{
 	CCaptureVideo* lpCaptureVideo = NULL;
 	lpCaptureVideo = (CCaptureVideo*)lpVideoCapturer;
 	if (lpCaptureVideo)
 	{	
 		delete lpCaptureVideo;
 		lpCaptureVideo = NULL;
 	}
}

//创建控制接口指针
LPAudioCapturer	APIENTRY Create_AudioCapturer()
{
	CCaptureAudio* lpCaptureAudio = NULL; 
	lpCaptureAudio = new CCaptureAudio();

	return (LPAudioCapturer)lpCaptureAudio;
}

//销毁控制接口指针
void APIENTRY Release_AudioCapturer(LPAudioCapturer lpAudioCapturer)
{
	CCaptureAudio* lpCaptureAudio = NULL;
	lpCaptureAudio = (CCaptureAudio*)lpAudioCapturer;
	if (lpCaptureAudio)
	{	
		delete lpCaptureAudio;
		lpCaptureAudio = NULL;
	}
}




