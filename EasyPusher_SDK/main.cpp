/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include <stdio.h>
#include <string.h>
#include "EasyPusherAPI.h"
/*
*本Key为3个月临时授权License，如需商业使用，请邮件至support@easydarwin.org咨询商业授权方案。
*/
#ifdef _WIN32
#define KEY "6A36334A743469576B5A75414D6170597031584E4576706C59584E356346634D5671442B6B75424859585A7062695A4359574A76633246414D6A41784E6B566863336C4559584A33615735555A5746745A57467A65513D3D"
#include "getopt.h"
#elif define _ARM
#define KEY "6A36334A742F2B2B72624B41314B785970314D714576426C59584E356348567A6147567958325A7062475658444661672F704C67523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#else //x86 linux
#define KEY "6A36334A74354F576B597141785A785A706D4D35792F426C59584E356348567A6147567958325A7062475658444661672F307667523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#endif
#include <stdlib.h>
#include "hi_type.h"
#include "hi_net_dev_sdk.h"
#include "hi_net_dev_errors.h"

char* ConfigIP		= "www.easydarwin.org";		//Default EasyDarwin Address
char* ConfigPort	= "554";				//Default EasyDarwin Port
char* ConfigName	= "easypusher_sdk.sdp";//Default Push StreamName
char* ConfigUName	= "admin";			//SDK UserName
char* ConfigPWD		= "admin";			//SDK Password
char* ConfigDHost	= "192.168.2.100";	//SDK Host
char* ConfigDPort	= "80";				//SDK Port
char *ProgName;		//Program Name

HI_U32 u32Handle = 0;
Easy_Pusher_Handle fPusherHandle = 0;

HI_S32 OnEventCallback(HI_U32 u32Handle,			/* 句柄 */
                                HI_U32 u32Event,	/* 事件 */
                                HI_VOID* pUserData  /* 用户数据*/
                                )
{
	return HI_SUCCESS;
}


HI_S32 NETSDK_APICALL OnStreamCallback(HI_U32 u32Handle,/* 句柄 */
                                HI_U32 u32DataType,		/* 数据类型，视频或音频数据或音视频复合数据 */
                                HI_U8* pu8Buffer,		/* 数据包含帧头 */
                                HI_U32 u32Length,		/* 数据长度 */
                                HI_VOID* pUserData		/* 用户数据*/
                                )
{
    HI_S_AVFrame* pstruAV = HI_NULL;
	HI_S_SysHeader* pstruSys = HI_NULL;
	
	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;

		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{
			if(fPusherHandle == 0 ) return 0;

			if(pstruAV->u32AVFrameLen > 0)
			{
				unsigned char* pbuf = (unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame));

				EASY_AV_Frame  avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
				avFrame.u32AVFrameLen = pstruAV->u32AVFrameLen;
				avFrame.pBuffer = (unsigned char*)pbuf;
				avFrame.u32VFrameType = (pstruAV->u32VFrameType==HI_NET_DEV_VIDEO_FRAME_I)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
				avFrame.u32TimestampSec = pstruAV->u32AVFramePTS/1000;
				avFrame.u32TimestampUsec = (pstruAV->u32AVFramePTS%1000)*1000;
				EasyPusher_PushFrame(fPusherHandle, &avFrame);
			}	
		}
		else
		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		{
			if(fPusherHandle == 0 ) return 0;

			if(pstruAV->u32AVFrameLen > 0)
			{
				unsigned char* pbuf = (unsigned char*)(pu8Buffer+sizeof(HI_S_AVFrame));

				EASY_AV_Frame  avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
				avFrame.u32AVFrameLen = pstruAV->u32AVFrameLen-4;//去掉厂家自定义的4字节头
				avFrame.pBuffer = (unsigned char*)pbuf+4;
				avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
				avFrame.u32TimestampSec = pstruAV->u32AVFramePTS/1000;
				avFrame.u32TimestampUsec = (pstruAV->u32AVFramePTS%1000)*1000;
				EasyPusher_PushFrame(fPusherHandle, &avFrame);
			}			
		}
	}
	else
	if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;
		printf("Video W:%u H:%u Audio: %u \n", pstruSys->struVHeader.u32Width, pstruSys->struVHeader.u32Height, pstruSys->struAHeader.u32Format);
	}
	return HI_SUCCESS;
}

HI_S32 OnDataCallback(HI_U32 u32Handle,					/* 句柄 */
                                HI_U32 u32DataType,		/* 数据类型*/
                                HI_U8*  pu8Buffer,      /* 数据 */
                                HI_U32 u32Length,		/* 数据长度 */
                                HI_VOID* pUserData		/* 用户数据*/
                                )
{
	return HI_SUCCESS;
}

/* EasyPusher数据回调 */
int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
    if (_state == EASY_PUSH_STATE_CONNECTING)               printf("Connecting...\n");
    else if (_state == EASY_PUSH_STATE_CONNECTED)           printf("Connected\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)      printf("Connect failed\n");
    else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)       printf("Connect abort\n");
	else if (_state == EASY_PUSH_STATE_PUSHING)             printf("\r Pushing to rtsp://%s:%d/%s ...", ConfigIP, atoi(ConfigPort), ConfigName);
    else if (_state == EASY_PUSH_STATE_DISCONNECTED)        printf("Disconnect.\n");

    return 0;
}
void PrintUsage()
{
	printf("Usage:\n");
	printf("------------------------------------------------------\n");
	printf("%s [-d <host> -p <port> -n <streamName> -N <Device user> -P <Device password> -H <Device host> -T <Device Port>]\n", ProgName);
	printf("Help Mode:   %s -h \n", ProgName );
	printf("For example: %s -d 115.29.139.20 -p 554 -n easypusher_sdk.sdp -N admin -P admin -H 192.168.66.189 -T 80\n", ProgName); 
	printf("------------------------------------------------------\n");
}
int main(int argc, char * argv[])
{
	int isActivated = 0;
#ifndef _WIN32
   signal(SIGPIPE, SIG_IGN);
#endif

#ifdef _WIN32
	extern char* optarg;
#endif
	int ch;
	ProgName = argv[0];	
	PrintUsage();

	while ((ch = getopt(argc,argv, "hd:p:n:N:P:H:T:")) != EOF) 
	{
		switch(ch)
		{
		case 'h':
			PrintUsage();
			return 0;
			break;
		case 'd':
			ConfigIP =optarg;
			break;
		case 'p':
			ConfigPort =optarg;
			break;
		case 'n':
			ConfigName =optarg;
			break;
		case 'N':
			ConfigUName =optarg;
			break;
		case 'P':
			ConfigPWD =optarg;
			break;
		case 'H':
			ConfigDHost =optarg;
			break;
		case 'T':
			ConfigDPort =optarg;
			break;
		case '?':
			return 0;
			break;
		default:
			break;
		}
	}

	isActivated = EasyPusher_Activate(KEY);
	switch(isActivated)
	{
	case EASY_ACTIVATE_INVALID_KEY:
		printf("KEY is EASY_ACTIVATE_INVALID_KEY!\n");
		break;
	case EASY_ACTIVATE_TIME_ERR:
		printf("KEY is EASY_ACTIVATE_TIME_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_LEN_ERR!\n");
		break;
	case EASY_ACTIVATE_PROCESS_NAME_ERR:
		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_ERR!\n");
		break;
	case EASY_ACTIVATE_VALIDITY_PERIOD_ERR:
		printf("KEY is EASY_ACTIVATE_VALIDITY_PERIOD_ERR!\n");
		break;
	case EASY_ACTIVATE_SUCCESS:
		printf("KEY is EASY_ACTIVATE_SUCCESS!\n");
		break;
	}

	if(EASY_ACTIVATE_SUCCESS != isActivated)
		return -1;

    HI_S32 s32Ret = HI_SUCCESS;
    HI_S_STREAM_INFO struStreamInfo;
    
    HI_NET_DEV_Init();
    
    s32Ret = HI_NET_DEV_Login(&u32Handle, ConfigUName, ConfigPWD, ConfigDHost, atoi(ConfigDPort));
    if (s32Ret != HI_SUCCESS)
    {
        HI_NET_DEV_DeInit();
		return -1;
    }
    
	//HI_NET_DEV_SetEventCallBack(u32Handle, OnEventCallback, &a);
	HI_NET_DEV_SetStreamCallBack(u32Handle, (HI_ON_STREAM_CALLBACK)OnStreamCallback, NULL);
	//HI_NET_DEV_SetDataCallBack(u32Handle, OnDataCallback, &a);

	struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
	struStreamInfo.blFlag = HI_TRUE;;
	struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
	struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
	s32Ret = HI_NET_DEV_StartStream(u32Handle, &struStreamInfo);
	if (s32Ret != HI_SUCCESS)
	{
		HI_NET_DEV_Logout(u32Handle);
		u32Handle = 0;
		return -1;
	}    
    
    EASY_MEDIA_INFO_T mediainfo;
    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec = EASY_SDK_VIDEO_CODEC_H264;
	mediainfo.u32VideoFps = 25;
	mediainfo.u32AudioCodec = EASY_SDK_AUDIO_CODEC_G711A;//SDK output Audio PCMA
	mediainfo.u32AudioSamplerate = 8000;
	mediainfo.u32AudioChannel = 1;

    fPusherHandle = EasyPusher_Create();
    EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
	EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 1024, false);//1M缓冲区
    printf("Press Enter exit...\n");
    getchar();

    EasyPusher_StopStream(fPusherHandle);
    EasyPusher_Release(fPusherHandle);
    fPusherHandle = 0;
   
    HI_NET_DEV_StopStream(u32Handle);
    HI_NET_DEV_Logout(u32Handle);
    
    HI_NET_DEV_DeInit();

    return 0;
}