/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "EasyPusherAPI.h"
#include "trace.h"

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
    char szIP[16] = {0};
    int pusherId = 0;
    EASY_MEDIA_INFO_T   mediainfo;

    int buf_size = 1024*512;
    char *pbuf = (char *) malloc(buf_size);
    FILE *fES = NULL;
	int position = 0;
	int iFrameNo = 0;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec =   0x1C;

    fES = fopen("./EasyDarwin.264", "rb");
    if (NULL == fES)        return 0;

    pusherId = EASY_SDK_API_Create();

    EASY_SDK_API_SetEventCallback(pusherId, __EasyPusher_Callback, 0, NULL);

    EASY_SDK_API_StartStream(pusherId, "115.29.139.20", 554, "live.sdp", "admin", "admin", &mediainfo);
	printf("*** live streaming url:rtsp://115.29.139.20:554/live.sdp ***\n");

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
                EASY_SDK_API_PushFrame(pusherId, &avFrame);

                Sleep(30);

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
    EASY_SDK_API_StopStream(pusherId);
    EASY_SDK_API_Release(pusherId);





    WSACleanup();
    return 0;
}
