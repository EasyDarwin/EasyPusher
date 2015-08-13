/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"
#include <winsock2.h>


#define RTSPURL "rtsp://admin:admin@192.168.66.189/"

#define SHOST	"127.0.0.1"		//EasyDarwin流媒体服务器地址115.29.139.20
#define SPORT	554					//EasyDarwin流媒体服务器端口

Easy_Pusher_Handle pusherHandle = 0;
Easy_RTSP_Handle fRTSPHandle = 0;

/* NVSource从RTSPClient获取数据后回调给上层 */
int CALLBACK __NVSourceCallBack( int _chid, int *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo)
{
	if (NULL != frameinfo)
	{
		if (frameinfo->height==1088)		frameinfo->height=1080;
		else if (frameinfo->height==544)	frameinfo->height=540;
	}

	if (_mediatype == MEDIA_TYPE_VIDEO)
	{
		if(pusherHandle == 0 )
			return 0;

		if(frameinfo && frameinfo->length)
		{
				EASY_AV_Frame  avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
				avFrame.u32AVFrameLen = frameinfo->length;
				avFrame.pBuffer = (unsigned char*)pbuf;
				avFrame.u32VFrameType = (frameinfo->type==FRAMETYPE_I)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				EasyPusher_PushFrame(pusherHandle, &avFrame);
		}	
	}
	return 0;
}

int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
    if (_state == EASY_PUSH_STATE_CONNECTING)               printf("Connecting...\n");
    else if (_state == EASY_PUSH_STATE_CONNECTED)           printf("Connected\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)      printf("Connect failed\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)       printf("Connect abort\n");
    //else if (_state == EASY_PUSH_STATE_PUSHING)             printf("P->");
    else if (_state == EASY_PUSH_STATE_DISCONNECTED)        printf("Disconnect.\n");

    return 0;
}

int main()
{

	//创建NVSource
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;

	unsigned int mediaType = MEDIA_TYPE_VIDEO;
	//mediaType |= MEDIA_TYPE_AUDIO;	//换为NVSource, 屏蔽声音

	EasyRTSP_SetCallback(fRTSPHandle, __NVSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, RTSPURL, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);
	
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData); 

    EASY_MEDIA_INFO_T mediainfo;

    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec =   0x1C;

    pusherHandle = EasyPusher_Create();

    EasyPusher_SetEventCallback(pusherHandle, __EasyPusher_Callback, 0, NULL);

    EasyPusher_StartStream(pusherHandle, SHOST, SPORT, "live.sdp", "admin", "admin", &mediainfo, 512);
	printf("*** live streaming url:rtsp://%s:%d/live.sdp ***\n", SHOST, SPORT);

	while(1)
	{
		Sleep(10);	
	};

    EasyPusher_StopStream(pusherHandle);
    EasyPusher_Release(pusherHandle);
    pusherHandle = 0;
   
	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}