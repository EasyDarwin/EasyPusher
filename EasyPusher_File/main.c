/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "EasyPusherAPI.h"
#include "trace.h"
#include "stdio.h"

#define SHOST	"127.0.0.1"			//EasyDarwin流媒体服务器地址
#define SPORT	554					//EasyDarwin流媒体服务器端口
#define SNAME	"easypusher_file.sdp"

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

int main()
{
    char szIP[16] = {0};
    Easy_Pusher_Handle fPusherHandle = 0;
    EASY_MEDIA_INFO_T   mediainfo;

    int buf_size = 1024*512;
    char *pbuf = (char *) malloc(buf_size);
    FILE *fES = NULL;
	int position = 0;
	int iFrameNo = 0;

    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;

    fES = fopen("./EasyPusher.264", "rb");
    if (NULL == fES)        return 0;

    fPusherHandle = EasyPusher_Create();

    EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);

    EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
    EasyPusher_StartStream(fPusherHandle, SHOST, SPORT, SNAME, "admin", "admin", &mediainfo, 1024);

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

			if ( (unsigned char)pbuf[position-5]== 0x00 && 
					(unsigned char)pbuf[position-4]== 0x00 && 
					(unsigned char)pbuf[position-3] == 0x00 &&
					(unsigned char)pbuf[position-2] == 0x01 &&
					//(((unsigned char)pbuf[position-1] == 0x61) ||
					//((unsigned char)pbuf[position-1] == 0x67) ) )
					(naltype==0x07||naltype==0x01) )
			{
				int framesize = position - 5;
                EASY_AV_Frame   avFrame;

				naltype = (unsigned char)pbuf[4] & 0x1F;

                memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
                avFrame.u32AVFrameLen   =   framesize;
                avFrame.pBuffer = (unsigned char*)pbuf;
				avFrame.u32VFrameType = (naltype==0x07)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
                EasyPusher_PushFrame(fPusherHandle, &avFrame);
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

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
