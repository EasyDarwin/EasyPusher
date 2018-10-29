/*
	Copyright (c) 2013-2018 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "EasyPusherAPI.h"
#include "trace.h"
#include <stdio.h>
#include <stdlib.h>


/*
*本Key为3个月临时授权License，如需商业使用，请邮件至support@easydarwin.org咨询商业授权方案。
*/
#ifdef _WIN32
#include "getopt.h"
#define KEY "6A36334A743469576B5A754147546862704D652F634F784659584E355548567A6147567958305A4A544555755A58686C567778576F4E6A773430566863336C4559584A33615735555A57467453584E55614756435A584E30514449774D54686C59584E35"
#else //linux
#include "unistd.h"
#include <signal.h>
#define KEY "6A36334A74354F576B596F412F4E5262704373447066426C59584E356348567A6147567958325A7062475658444661672F36586A5257467A65555268636E6470626C526C5957314A6331526F5A554A6C633352414D6A41784F47566863336B3D"
#endif

char* ConfigIP	=	"cloud.easydarwin.org";			//Default EasyDarwin Address 
char* ConfigPort=	"554";					//Default EasyDarwin Port
char* ConfigName=	"easypusher_filetest.sdp";	//Default RTSP Push StreamName
char* ProgName;								//Program Name
char* FileName = "EasyDarwin.h264";								//File Name

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
	printf("%s [-s <sourceFile> -a <dest ip> -p <port>]\n", ProgName);
	printf("Help Mode:   %s -h \n", ProgName );
	printf("For example: %s -s test.h264 -a 192.168.1.127 -p 10000]\n", ProgName); 
	printf("------------------------------------------------------\n");
}

int main(int argc, char * argv[])
{
	int isActivated = 0 ;
#ifndef _WIN32
   signal(SIGPIPE, SIG_IGN);
#endif

     WORD sockVersion = MAKEWORD(2,2);//版本号
	WSADATA data;    //用来保存WSAStartup调用后返回的windows Sockets数据
	struct sockaddr_in serAddr;
     SOCKET sock_Client;
    SOCKADDR_IN addr_server;   //服务器的地址数据结构

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


	while ((ch = getopt(argc,argv, "h:p:a:s:")) != EOF) 
	{
		switch(ch)
		{
		case 'h':
			PrintUsage();
			return 0;
			break;
		case 'p':
			ConfigPort =optarg;
			break;
		case 'a':
			ConfigName =optarg;
			break;
        case 's':
			FileName =optarg;
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

    fES = fopen(FileName, "rb");
    if (NULL == fES)        return 0;

    
	if(WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	sock_Client=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);//创建客户端用于通信的Socket
    
    addr_server.sin_family=AF_INET;
    addr_server.sin_port=htons(atoi(ConfigPort));//端口号为4567
    addr_server.sin_addr.S_un.S_addr=inet_addr(ConfigName);   //127.0.0.1为本电脑IP地址

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

				timestamp += 1000/mediainfo.u32VideoFps;

                sendto(sock_Client,pbuf,framesize,0,(SOCKADDR*)&addr_server,sizeof(SOCKADDR));

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
    closesocket(sock_Client);
	free(pbuf);
    return 0;
}
