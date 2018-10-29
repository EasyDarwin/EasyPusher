/*
	Copyright (c) 2013-2018 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*本Key为3个月临时授权License，如需商业使用，请邮件至support@easydarwin.org咨询商业授权方案。
*/
#ifdef _WIN32
#define KEY "6A36334A743469576B5A73414B735662704472563465784659584E355548567A6147567958314A55553141755A58686C567778576F502F683430566863336C4559584A33615735555A57467453584E55614756435A584E30514449774D54686C59584E35"
#define EasyRTSPClient_KEY "79393674363469576B5A754165385A6270446D455965784659584E355548567A6147567958314A55553141755A58686C567778576F502F683430566863336C4559584A33615735555A57467453584E55614756435A584E30514449774D54686C59584E35"
#include "getopt.h"
#else //linux
#define KEY "7939367436354F576B596F412F4E5262704373447066426C59584E356348567A6147567958334A3063334258444661672F36586A5257467A65555268636E6470626C526C5957314A6331526F5A554A6C633352414D6A41784F47566863336B3D"
#define EasyRTSPClient_KEY "7939367436354F576B596F412F4E526270437344772F426C59584E356348567A6147567958334A3063334258444661672F38506A5257467A65555268636E6470626C526C5957314A6331526F5A554A6C633352414D6A41784F47566863336B3D"
#include "unistd.h"
#include <signal.h>
#endif
#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"

char* ConfigIP		= "192.168.1.55";		//Default EasyDarwin Address
char* ConfigPort	= "554";				//Default EasyDarwin Port
char* ConfigName	= "easypusher_rtsp.sdp";//Default Stream Name
char* ConfigRTSPURL	= "rtsp://admin:admin@192.168.1.222/22";	//RTSP Source URL(With username:password@)
char* ProgName;		//Program Name

Easy_Pusher_Handle	fPusherHandle = 0;		//libEasyPusher Handle
Easy_RTSP_Handle	fRTSPHandle = 0;		//libEasyRTSPClient Handle

EASY_MEDIA_INFO_T*	fSourceMediaInfo = NULL;

/* EasyPusher Callback */
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

/* EasyRTSPClient Callback */
int Easy_APICALL __RTSPSourceCallBack( int _chid, void *_chPtr, int _mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo)
{
	if (_mediatype == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		if((fPusherHandle == 0) && (fSourceMediaInfo != NULL) && (frameinfo->type == EASY_SDK_VIDEO_FRAME_I))
		{
			EASY_MEDIA_INFO_T mediainfo;
			memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
			memcpy(&mediainfo, fSourceMediaInfo, sizeof(EASY_MEDIA_INFO_T));
			mediainfo.u32SpsLength = frameinfo->reserved1-4;
			mediainfo.u32PpsLength = frameinfo->reserved2-frameinfo->reserved1-4;
			memcpy(mediainfo.u8Sps, pbuf+4, mediainfo.u32SpsLength);
			memcpy(mediainfo.u8Pps, pbuf+4+mediainfo.u32SpsLength+4, mediainfo.u32PpsLength);

			fPusherHandle = EasyPusher_Create();
			EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
			EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, EASY_RTP_OVER_UDP, "admin", "admin", &mediainfo, 1024, false);//1M缓冲区
			printf("*** live streaming url:rtsp://%s:%d/%s ***\n", ConfigIP, atoi(ConfigPort), ConfigName);
		}

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
		if((pbuf != NULL) && (fSourceMediaInfo == NULL))
		{
			fSourceMediaInfo = new EASY_MEDIA_INFO_T();
			memcpy(fSourceMediaInfo, pbuf, sizeof(EASY_MEDIA_INFO_T));
		}
	}
	return 0;
}
void PrintUsage()
{
	printf("Usage:\n");
	printf("------------------------------------------------------\n");
	printf("%s [-d <host> -p <port> -n <streamName> -u <RTSPURL>]\n", ProgName);
	printf("Help Mode:   %s -h \n", ProgName );
	printf("For example: %s -d 115.29.139.20 -p 554 -n easypusher_rtsp.sdp -u rtsp://admin:admin@anfengde.f3322.org/22\n", ProgName); 
	printf("------------------------------------------------------\n");
}
int main(int argc, char * argv[])
{
	int isActivated = 0 ;
#ifndef _WIN32
   signal(SIGPIPE, SIG_IGN);
#endif

#ifdef _WIN32
	extern char* optarg;
#endif
	int ch;
	ProgName = argv[0];
	PrintUsage();

	while ((ch = getopt(argc,argv, "hd:p:n:u:")) != EOF) 
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
		case 'u':
			ConfigRTSPURL =optarg;
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
	{
		getchar();
		return -1;
	}

	if(EASY_ACTIVATE_SUCCESS != EasyRTSP_Activate(EasyRTSPClient_KEY))
	{
		getchar();
		return -1;
	}

	//Create RTSPClient Handle
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;	//Get Video & Audio

	EasyRTSP_SetCallback(fRTSPHandle, __RTSPSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, ConfigRTSPURL, EASY_RTP_OVER_UDP, mediaType, 0, 0, NULL, 1000, 0, 0x01, 1);

    printf("Press Enter exit...\n");
    getchar();
	getchar();
	getchar();

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