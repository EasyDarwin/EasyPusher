/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include "EasyPusherAPI.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include "hi_type.h"
#include "hi_net_dev_sdk.h"
#include "hi_net_dev_errors.h"
#include "trace.h"

#define UNAME	"admin"
#define PWORD	"admin"
#define DHOST	"127.0.0.1"	//EasyCamera摄像机IP地址
#define DPORT	80					//EasyCamera摄像机端口

#define SHOST	"115.29.139.20"		//EasyDarwin流媒体服务器地址
#define SPORT	554					//EasyDarwin流媒体服务器端口

HI_U32 u32Handle = 0;
Easy_Pusher_Handle pusherHandle = 0;

HI_S32 OnEventCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32Event,      /* 事件 */
                                HI_VOID* pUserData  /* 用户数据*/
                                )
{
	return HI_SUCCESS;
}


HI_S32 NETSDK_APICALL OnStreamCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,     /* 数据类型，视频或音频数据或音视频复合数据 */
                                HI_U8*  pu8Buffer,      /* 数据包含帧头 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{

	printf("enter OnStreamCallback\n");
    HI_S_AVFrame* pstruAV = HI_NULL;
	HI_S_SysHeader* pstruSys = HI_NULL;
	

	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;

		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{
			if(pusherHandle == 0 )
				return 0;

			if(pstruAV->u32AVFrameLen > 5)
			{
				unsigned char* pbuf = (unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame));
				unsigned char naltype = ( (unsigned char)pbuf[4] & 0x1F);

				if ( (unsigned char)pbuf[0]== 0x00 && 
						(unsigned char)pbuf[1]== 0x00 && 
						(unsigned char)pbuf[2] == 0x00 &&
						(unsigned char)pbuf[3] == 0x01 &&
						(naltype==0x07 || naltype==0x01) )
				{
					EASY_AV_Frame  avFrame;

					naltype = (unsigned char)pbuf[4] & 0x1F;
					memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
					avFrame.u32AVFrameLen = pstruAV->u32AVFrameLen;
					avFrame.pBuffer = (unsigned char*)pbuf;
					avFrame.u32VFrameType = (naltype==0x07)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
					EasyPusher_PushFrame(pusherHandle, &avFrame);
                    printf("OnStreamCallback: EasyPusher_PushFrame\n");
				}
			}	
		}
		else
		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		{
			//printf("Audio %u PTS: %u \n", pstruAV->u32AVFrameLen, pstruAV->u32AVFramePTS);
			//SaveRecordFile("Video.hx", pu8Buffer, u32Length);			
		}
	}
	else
	if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;
		printf("Video W:%u H:%u Audio: %u \n", pstruSys->struVHeader.u32Width, pstruSys->struVHeader.u32Height, pstruSys->struAHeader.u32Format);
	} 
    printf("leave OnStreamCallback\n");
	return HI_SUCCESS;
}

HI_S32 OnDataCallback(HI_U32 u32Handle, /* 句柄 */
                                HI_U32 u32DataType,       /* 数据类型*/
                                HI_U8*  pu8Buffer,      /* 数据 */
                                HI_U32 u32Length,      /* 数据长度 */
                                HI_VOID* pUserData    /* 用户数据*/
                                )
{
	return HI_SUCCESS;
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
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S_STREAM_INFO struStreamInfo;
    HI_U32 a;
    
    HI_NET_DEV_Init();
    
    s32Ret = HI_NET_DEV_Login(&u32Handle, UNAME, PWORD, DHOST, DPORT);
    if (s32Ret != HI_SUCCESS)
    {
        HI_NET_DEV_DeInit();
		return -1;
    }
    
	//HI_NET_DEV_SetEventCallBack(u32Handle, OnEventCallback, &a);
	HI_NET_DEV_SetStreamCallBack(u32Handle, (HI_ON_STREAM_CALLBACK)OnStreamCallback, &a);
	//HI_NET_DEV_SetDataCallBack(u32Handle, OnDataCallback, &a);

	struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
	struStreamInfo.blFlag = HI_FALSE;//HI_FALSE;
	struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
	struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
	s32Ret = HI_NET_DEV_StartStream(u32Handle, &struStreamInfo);
	if (s32Ret != HI_SUCCESS)
	{
		HI_NET_DEV_Logout(u32Handle);
		u32Handle = 0;
		return -1;
	}    
	
#ifdef _WIN32
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData); 
#endif
    
    EASY_MEDIA_INFO_T mediainfo;

    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec =   0x1C;

    pusherHandle = EasyPusher_Create();

    EasyPusher_SetEventCallback(pusherHandle, __EasyPusher_Callback, 0, NULL);

    EasyPusher_StartStream(pusherHandle, SHOST, SPORT, "live.sdp", "admin", "admin", &mediainfo, 512);
	printf("*** live streaming url:rtsp://%s:%d/live.sdp ***\n", SHOST, SPORT);

	while(1)
	{
#ifdef _WIN32
		Sleep(10);	
#else
        usleep(10*1000);
#endif
	};

    EasyPusher_StopStream(pusherHandle);
    EasyPusher_Release(pusherHandle);
    pusherHandle = 0;
   
    HI_NET_DEV_StopStream(u32Handle);
    HI_NET_DEV_Logout(u32Handle);
    
    HI_NET_DEV_DeInit();

    return 0;
}