/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// Add by SwordTwelve
#pragma once

#include "./MP4Demux/Head.h"
#include <string>
#include <vector>
#include <process.h>
using namespace std;
#include "EasyPusher\EasyPusherAPI.h"

#ifndef HANDLE
#define HANDLE void*
#endif//HANDLE  for linux

#define MAX_TRACK_NUM 32
#define VEDIO_PUSH 0
#define AUDIO_PUSH 1

// for AV Sync 	[Add by Ricky]
typedef struct _SYN_CLOCK_CTRL_
{
	unsigned long ClockBase;
	unsigned long ClockCurr;
	unsigned long VedioBase;
	unsigned long AudioBase;
}Sync_clock_Ctl;

typedef struct tagMediaInfo
{
	double nDuration;//MP4播放时间长度(video)
	int nVideoSampleSize;//MP4文件中视频帧数
	int nVideoWidth;//
	int nVideoHeight;
	int nFps;
	int nAudioSampleRate;//音频采样率
	int nChannels;//声道数
	int nSampleSize;//采样位数 
	
}MediaInfo;

typedef int (WINAPI *CaptureFileCallback)(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);

class CEasyFileCapture
{
public:
	CEasyFileCapture(void);
	~CEasyFileCapture(void);

	//Member function
public:
	//Interface function
	int InitMP4FileCapture(char* sPath, int nStartTime, int nStopTime, bool bAutoLoop);
	void StartMP4FileCapture();
	void StopMP4FileCapture();
	//设置捕获数据回调函数
	void SetCaptureFileCallback(CaptureFileCallback callBack, void * pMaster);
	void GetMP4FileInfo(MediaInfo* pMediaInfo);
	void GetMediaInfo(EASY_MEDIA_INFO_T& mediaInfo);

	//Globle Func for thread callback
	static unsigned int _stdcall  VideoThread(void* lParam);
	static unsigned int _stdcall  AudioThread(void* lParam);
	int VideoProcess();
	int AudioProcess();
	//Audio and video Sync lock [Add by Ricky]
	unsigned long Sync_clock(unsigned long TimeScale, unsigned long duration, int type, unsigned long* out);
	int AvcToH264Frame(unsigned char* pFrame, uint32_t nFrameLen, bool& bKeyFrame, unsigned char** pOutBuffer, uint32_t& nFrameLength );

	//Member var
private:
	//
	// Globle var for all of class to use
	// 
	// Handler use for thrack demux thread
	HANDLE m_mp4TrackThread[MAX_TRACK_NUM];
	bool  m_bThreadLiving[MAX_TRACK_NUM];
	//获取MP4头box信息
	CMp4_root_box m_root;
	//FILE handle for video
	FILE * m_fin ; 
	//FILE handle for audio
	FILE* m_finA ;
	CRITICAL_SECTION m_cs;
	bool m_bVideoStart;
	Sync_clock_Ctl m_clock;
	CMp4_avcC_box m_videoInfo;
	CMp4_mp4a_box m_audioInfo;
	int m_nVideoTrackId;
	int m_nAudioTrackId;

	bool m_bAutoLoop;//是否循环推送
	int m_nStartTime ;//推送开始时间
	int m_nEndTime;//推送截止时间
	CaptureFileCallback m_pCallback;
	void* m_pMaster;
	MediaInfo m_mediaInfo;
};

