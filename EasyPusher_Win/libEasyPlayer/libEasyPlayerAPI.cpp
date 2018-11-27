/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "libEasyPlayerAPI.h"
#include "ChannelManager.h"

CChannelManager	*g_pChannelManager = NULL;

#define KEY "79393674363469576B5A7341636564627042694E6B76464659584E355548567A614756794C6D56345A534E58444661672F704C67523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"

// ³õÊ¼»¯SDK
LIB_EASYPLAYER_API int EasyPlayer_Init()
{
	int isEasyRTSPClientActivated = EasyRTSP_Activate(KEY);
	switch(isEasyRTSPClientActivated)
	{
	case EASY_ACTIVATE_INVALID_KEY:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_INVALID_KEY!\n");
		break;
	case EASY_ACTIVATE_TIME_ERR:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_TIME_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_PROCESS_NAME_LEN_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_ERR:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_PROCESS_NAME_ERR!\n");
		break;
	case EASY_ACTIVATE_VALIDITY_PERIOD_ERR:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_VALIDITY_PERIOD_ERR!\n");
		break;
	case EASY_ACTIVATE_SUCCESS:
		printf("EasyRTSPClient_KEY is EASY_ACTIVATE_SUCCESS!\n");
		break;
	}

	if(EASY_ACTIVATE_SUCCESS != isEasyRTSPClientActivated)
		return -1;

	if (NULL == g_pChannelManager)
	{
		g_pChannelManager = new CChannelManager();
		g_pChannelManager->Initial();
	}

	if (NULL == g_pChannelManager)		return -1;

	return 0;
}

// Release
LIB_EASYPLAYER_API void EasyPlayer_Release()
{
	if (NULL != g_pChannelManager)
	{
		delete g_pChannelManager;
		g_pChannelManager = NULL;
	}
}


LIB_EASYPLAYER_API int EasyPlayer_OpenStream(const char *url, HWND hWnd, RENDER_FORMAT renderFormat, int rtpovertcp,const char *username, const char *password, MediaSourceCallBack callback, void *userPtr)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->OpenStream(url, hWnd, renderFormat, rtpovertcp, username, password, callback, userPtr);
}

LIB_EASYPLAYER_API void EasyPlayer_CloseStream(int channelId)
{
	if (NULL == g_pChannelManager)		return;

	g_pChannelManager->CloseStream(channelId);
}

LIB_EASYPLAYER_API int EasyPlayer_SetFrameCache(int channelId, int cache)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetFrameCache(channelId, cache);
}
LIB_EASYPLAYER_API int EasyPlayer_SetShownToScale(int channelId, int shownToScale)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetShownToScale(channelId, shownToScale);
}

LIB_EASYPLAYER_API int EasyPlayer_SetDecodeType(int channelId, int decodeKeyframeOnly)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetDecodeType(channelId, decodeKeyframeOnly);
}
LIB_EASYPLAYER_API int EasyPlayer_SetRenderRect(int channelId, LPRECT lpSrcRect)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetRenderRect(channelId, lpSrcRect);
}

LIB_EASYPLAYER_API int EasyPlayer_ShowStatisticalInfo(int channelId, int show)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->ShowStatisticalInfo(channelId, show);
}

LIB_EASYPLAYER_API int EasyPlayer_SetDragStartPoint(int channelId, POINT pt)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetDragStartPoint(channelId, pt);
}
LIB_EASYPLAYER_API int EasyPlayer_SetDragEndPoint(int channelId, POINT pt)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->SetDragEndPoint(channelId, pt);
}
LIB_EASYPLAYER_API int EasyPlayer_ResetDragPoint(int channelId)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->ResetDragPoint(channelId);
}


LIB_EASYPLAYER_API int EasyPlayer_PlaySound(int channelId)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->PlaySound(channelId);
}
LIB_EASYPLAYER_API int EasyPlayer_StopSound()
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->StopSound();
}

LIB_EASYPLAYER_API int EasyPlayer_StartManuRecording(int channelId)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->StartManuRecording(channelId);
}

LIB_EASYPLAYER_API int EasyPlayer_StopManuRecording(int channelId)
{
	if (NULL == g_pChannelManager)		return -1;

	return g_pChannelManager->StopManuRecording(channelId);
}

