/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "EasyPusherAPI.h"
#include "trace.h"
#include <stdio.h>

#ifdef _WIN32
#include "getopt.h"
#else
#include "unistd.h"
#include <signal.h>
#endif

char* ConfigIP	=	"127.0.0.1";			//Default EasyDarwin Address 183.220.236.189
char* ConfigPort=	"554";					//Default EasyDarwin Port121.40.50.44
char* ConfigName=	"easypusher_file.sdp";	//Default RTSP Push StreamName
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

    fES = fopen("./EasyPusher.264", "rb");
    if (NULL == fES)        return 0;

    fPusherHandle = EasyPusher_Create();

    EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);

	EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 1024, 0);

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
