/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
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
#define KEY "6A36334A743469576B5A73416343645A707469507065746C6432466B5A6D466D646D526E5A6D526E636E4E6B5A6D526E63694E58444661672F365867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#define EasyRTSPClient_KEY "79393674363469576B5A73416343645A707469507065784659584E355548567A6147567958314A55553141755A58686C567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#include "getopt.h"
#elif defined _ARM
#define KEY "6A36334A742F2B2B726249416343645A7074695070653168636D317764584E6F5A584A6D5957527A5A6D566D65474F45567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#define EasyRTSPClient_KEY "79393674362F2B2B726249416343645A707469507066426C59584E356348567A6147567958334A3063334258444661672F365867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#else //x86 linux
#define KEY "6A36334A74354F576B596F416343645A7074695070664E325A47646E656E5A6C5A336836646D4F2B567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#define EasyRTSPClient_KEY "7939367436354F576B596F416343645A707469507066426C59584E356348567A6147567958334A3063334258444661672F365867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#endif
#include "EasyPusherAPI.h"
#include "EasyRTSPClientAPI.h"

char* ConfigIP		= "www.easydarwin.org";		//Default EasyDarwin Address
char* ConfigPort	= "554";				//Default EasyDarwin Port
char* ConfigName	= "easypusher_rtsp.sdp";//Default Stream Name
char* ConfigRTSPURL	= "rtsp://admin:admin@192.168.66.222/22";	//RTSP Source URL(With username:password@)
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
			mediainfo.u32H264SpsLength = frameinfo->reserved1-4;
			mediainfo.u32H264PpsLength = frameinfo->reserved2-frameinfo->reserved1-4;
			memcpy(mediainfo.u8H264Sps, pbuf+4, mediainfo.u32H264SpsLength);
			memcpy(mediainfo.u8H264Pps, pbuf+4+mediainfo.u32H264SpsLength+4, mediainfo.u32H264PpsLength);

			fPusherHandle = EasyPusher_Create();
			EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
			EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 1024, false);//1M缓冲区
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
		return -1;

	if(EASY_ACTIVATE_SUCCESS != EasyRTSP_Activate(EasyRTSPClient_KEY))
		return -1;

	//Create RTSPClient Handle
	EasyRTSP_Init(&fRTSPHandle);

	if (NULL == fRTSPHandle) return 0;

	unsigned int mediaType = EASY_SDK_VIDEO_FRAME_FLAG | EASY_SDK_AUDIO_FRAME_FLAG;	//Get Video & Audio

	EasyRTSP_SetCallback(fRTSPHandle, __RTSPSourceCallBack);
	EasyRTSP_OpenStream(fRTSPHandle, 0, ConfigRTSPURL, EASY_RTP_OVER_TCP, mediaType, 0, 0, NULL, 1000, 0, 0x01, 1);

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