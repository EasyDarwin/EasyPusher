/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "StdAfx.h"
#include "EasyPlayerManager.h"

EasyPlayerManager::EasyPlayerManager(void)
{
	memset(&m_sSourceInfo, 0x0, sizeof(EASY_LOCAL_SOURCE_T));
	m_sSourceInfo.rtspSourceId = -1;

}

EasyPlayerManager::~EasyPlayerManager(void)
{
}

	//Start  --  如果传入的是宽串，可以参考下面这段
// 	wchar_t wszURL[128] = {0,};
// 	if (NULL != pEdtServerAddr)	pEdtServerAddr->GetWindowTextW(wszURL, sizeof(wszURL));
// 	if (wcslen(wszURL) < 1)		return;
// 
// 	char szURL[128] = {0,};
// 	__WCharToMByte(wszURL, szURL, sizeof(szURL)/sizeof(szURL[0]));
int EasyPlayerManager::Start(char* szURL, HWND hShowWnd, 
	RENDER_FORMAT eRenderFormat, int rtpovertcp, const char *username, const char *password, MediaSourceCallBack callback, void *userPtr) 
{
	//Stop
	if (m_sSourceInfo.rtspSourceId > 0)
	{
		Close();
		return -1;
	}

	m_sSourceInfo.rtspSourceId = EasyPlayer_OpenStream(szURL, hShowWnd, eRenderFormat, rtpovertcp, username, password, callback, userPtr);
	return	m_sSourceInfo.rtspSourceId ;
}
 
void EasyPlayerManager::Config(int nFrameCache,  BOOL bPlaySound, BOOL bShowToScale, BOOL  bShowStatisticInfo )
{
	if (m_sSourceInfo.rtspSourceId > 0)
	{
		EasyPlayer_SetFrameCache(m_sSourceInfo.rtspSourceId, nFrameCache);		//设置缓存
		EasyPlayer_ShowStatisticalInfo(m_sSourceInfo.rtspSourceId, bShowStatisticInfo);
		//按比例显示
		EasyPlayer_SetShownToScale(m_sSourceInfo.rtspSourceId, bShowToScale );
		if (bPlaySound)
		{
			EasyPlayer_PlaySound(m_sSourceInfo.rtspSourceId);
		}
	}
}

void	EasyPlayerManager::Close()
{
	if (m_sSourceInfo.rtspSourceId > 0)
	{
		EasyPlayer_CloseStream(m_sSourceInfo.rtspSourceId);
		m_sSourceInfo.rtspSourceId = -1;
	}
}

int EasyPlayerManager::InRunning()
{
	return m_sSourceInfo.rtspSourceId;
}
