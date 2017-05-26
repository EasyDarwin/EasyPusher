/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "EasyPusherAPI.h"
#include "trace.h"
#include <stdio.h>

/*
*本Key为3个月临时授权License，如需商业使用，请邮件至support@easydarwin.org咨询商业授权方案。
*/
#ifdef _WIN32
#include "getopt.h"
#define KEY "6A36334A743469576B5A73416343645A707469507065746C6432466B5A6D466D646D526E5A6D526E636E4E6B5A6D526E63694E58444661672F365867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#elif defined _ARM
#define KEY "6A36334A742F2B2B726249416343645A7074695070653168636D317764584E6F5A584A6D5957527A5A6D566D65474F45567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#include "unistd.h"
#include <signal.h>
#else //x86 linux
#include "unistd.h"
#include <signal.h>
#define KEY "6A36334A74354F576B596F416343645A7074695070664E325A47646E656E5A6C5A336836646D4F2B567778576F502B6C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#endif

char* ConfigIP	=	"www.easydarwin.org";			//Default EasyDarwin Address 183.220.236.189
char* ConfigPort=	"554";					//Default EasyDarwin Port121.40.50.44
char* ConfigName=	"easypusher_file_linux.sdp";	//Default RTSP Push StreamName
char* ProgName;								//Program Name

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
	printf("%s [-d <host> -p <port> -n <streamName>]\n", ProgName);
	printf("Help Mode:   %s -h \n", ProgName );
	printf("For example: %s -d 115.29.139.20 -p 554 -n easypusher_file.sdp\n", ProgName); 
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
    char szIP[16] = {0};
    Easy_Pusher_Handle fPusherHandle = 0;
    EASY_MEDIA_INFO_T   mediainfo;

    int buf_size = 1024*512;
    char *pbuf = (char *) malloc(buf_size);
    FILE *fES = NULL;
	int position = 0;
	int iFrameNo = 0;
	int timestamp = 0;
	ProgName = argv[0];
	PrintUsage();


	while ((ch = getopt(argc,argv, "hd:p:n:")) != EOF) 
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
		case '?':
			return 0;
			break;
		default:
			break;
		}
	}
    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;
	mediainfo.u32VideoFps = 25;

    fES = fopen("./EasyDarwin.264", "rb");
    if (NULL == fES)        return 0;

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

    fPusherHandle = EasyPusher_Create();

	if(fPusherHandle == NULL)
		return -2;

    EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);

	EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 2048, 0);

	while (1)
	{
		int nReadBytes = fread(pbuf+position, 1, 1, fES);
		if (nReadBytes < 1)
		{
			if (feof(fES))
			{
				position = 0;
				fseek(fES, 0, SEEK_SET);
				continue;
			}
			break;
		}

		position ++;

		if (position > 5)
		{
			unsigned char naltype = ( (unsigned char)pbuf[position-1] & 0x1F);

			if (	(unsigned char)pbuf[position-5]== 0x00 && 
					(unsigned char)pbuf[position-4]== 0x00 && 
					(unsigned char)pbuf[position-3] == 0x00 &&
					(unsigned char)pbuf[position-2] == 0x01 &&
					(naltype == 0x07 || naltype == 0x01 ) )
			{
				int framesize = position - 5;
                EASY_AV_Frame   avFrame;

				naltype = (unsigned char)pbuf[4] & 0x1F;

                memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
                avFrame.u32AVFrameLen   =   framesize;
                avFrame.pBuffer = (unsigned char*)pbuf;
				avFrame.u32VFrameType = (naltype==0x07)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
				avFrame.u32TimestampSec = timestamp/1000;
				avFrame.u32TimestampUsec = (timestamp%1000)*1000;
                EasyPusher_PushFrame(fPusherHandle, &avFrame);
				timestamp += 1000/mediainfo.u32VideoFps;

#ifndef _WIN32
                usleep(30*1000);
#else
                Sleep(30);
#endif
				memmove(pbuf, pbuf+position-5, 5);
				position = 5;

				iFrameNo ++;

                //if (iFrameNo > 100000) break;
                //break;
			}
		}
	}

    _TRACE("Press Enter exit...\n");
    getchar();

    EasyPusher_StopStream(fPusherHandle);
    EasyPusher_Release(fPusherHandle);
	free(pbuf);
    return 0;
}
