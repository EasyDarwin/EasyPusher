/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// RTSP流接收(播放)管理类，接收音视频流 [11/8/2015 Dingshuai]
// Add by SwordTwelve

#pragma once

#include "../libEasyPlayer/EasyRTSPClient/EasyTypes.h"

#include "../libEasyPlayer/libEasyPlayerAPI.h"
#include "../libEasyPlayer/D3DRender/D3DRenderAPI.h"
#pragma comment(lib,  "../libEasyPlayer/D3DRender/D3DRender.lib")
#pragma comment(lib, "../bin/libEasyPlayer.lib")

#include "./EasyPusher/EasyPusherAPI.h"
#pragma comment(lib, "./EasyPusher/libEasyPusher.lib")

//Gavin's Source Struct流信息结构
typedef struct __EASY_LOCAL_SOURCE_T
{	
	int		rtspSourceId;
	Easy_Pusher_Handle	pusherHandle;

	char	pushServerAddr[128];
	int		pushServerPort;
	char	sdpName[64];
	void* pMaster;
}EASY_LOCAL_SOURCE_T;

class EasyPlayerManager
{
public:
	EasyPlayerManager(void);
	~EasyPlayerManager(void);

	//Member Function
public:
	//初始化
	static int Init()
	{
		return EasyPlayer_Init();
	}

	// 释放
	static void UnInit()
	{
		EasyPlayer_Release();
	}

	//打开流
	int Start(char* szURL, HWND hShowWnd, RENDER_FORMAT eRenderFormat, int rtpovertcp, const char *username, const char *password, MediaSourceCallBack callback=NULL, void *userPtr=NULL) ;
	//设置参数
	void Config(int nFrameCache,  BOOL bPlaySound, BOOL bShowToScale = TRUE, BOOL  bShowStatisticInfo = FALSE);
	//关闭流
	void	Close();
	int InRunning();

	//Member Var
private:
	//接收的流信息
	EASY_LOCAL_SOURCE_T		m_sSourceInfo;

};

