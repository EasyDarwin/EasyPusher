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
#ifndef _WIN32
#include <unistd.h>
#else
#include <winsock2.h>
#endif

#define RTSPURL "rtsp://admin:admin@192.168.66.189/"

#define SHOST	"127.0.0.1"		//EasyDarwin流媒体服务器地址
#define SPORT	554				//EasyDarwin流媒体服务器端口
#define SNAME	"easypusher_rtsp.sdp"

Easy_Pusher_Handle fPusherHandle = 0;
Easy_RTSP_Handle fRTSPHandle = 0;

/* EasyPusher数据回调 */
int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
    if (_state == EASY_PUSH_STATE_CONNECTING)               printf("Connecting...\n");
    else if (_state == EASY_PUSH_STATE_CONNECTED)           printf("Connected\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)      printf("Connect failed\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)       printf("Connect abort\n");
    else if (_state == EASY_PUSH_STATE_PUSHING)             printf("\r Pushing to rtsp://%s:%d/%s ...", SHOST, SPORT, SNAME);
    else if (_state == EASY_PUSH_STATE_DISCONNECTED)        printf("Disconnect.\n");

    return 0;
}

/* EasyRTSPClient数据回调 */
int CALLBACK __RTSPSourceCallBack( int _chid, int *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo)
{
	if (_mediatype == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		if(fPusherHandle == 0 ) return 0;

		if(frameinfo && frameinfo->length)
		{
			EASY_AV_Frame  avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char*)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}	
	}

	if (_mediatype == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		if(fPusherHandle == 0 ) return 0;

		if(frameinfo && frameinfo->length)
		{
			EASY_AV_Frame  avFrame;
			memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
			avFrame.u32AVFrameLen = frameinfo->length;
			avFrame.pBuffer = (unsigned char*)pbuf;
			avFrame.u32VFrameType = frameinfo->type;
			avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
			avFrame.u32TimestampSec = frameinfo->timestamp_sec;
			avFrame.u32TimestampUsec = frameinfo->timestamp_usec;
			EasyPusher_PushFrame(fPusherHandle, &avFrame);
		}	
	}

	if (_mediatype == EASY_SDK_MEDIA_INFO_FLAG)
	{
		if((pbuf != NULL) && (fPusherHandle == NULL))
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, pbuf, sizeof(EASY_MEDIA_INFO_T));

			fPusherHandle = EasyPusher_Create();
			EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);

			EasyPusher_StartStream(fPusherHandle, SHOST, SPORT, SNAME, "admin", "admin", &mediainfo, 1024);//1M缓冲区
			printf("*** live streaming url:rtsp://%s:%d/%s ***\n", SHOST, SPORT, SNAME);
		}
	}
	return 0;
}

int main()
{
	//创建RTSPClient获取流媒体数据
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;	//获取音/视频数据

	EasyRTSP_SetCallback(fRTSPHandle, __RTSPSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, RTSPURL, RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0);

	while(1)
	{
#ifndef _WIN32
        usleep(10*1000);
#else
        Sleep(10);
#endif
	};

	if(fPusherHandle)
	{
		EasyPusher_StopStream(fPusherHandle);
		EasyPusher_Release(fPusherHandle);
		fPusherHandle = NULL;
	}
   
	EasyRTSP_CloseStream(fRTSPHandle);
	EasyRTSP_Deinit(&fRTSPHandle);
	fRTSPHandle = NULL;

    return 0;
}