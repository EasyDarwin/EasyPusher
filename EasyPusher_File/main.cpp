/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include "./MP4Demux/Head.h"

#include <process.h>
using namespace std;

#include "EasyPusherAPI.h"
//#include "trace.h"
#include <stdio.h>

#ifdef _WIN32
#define KEY "6A36334A743469576B5A75417848395A706F41347065784659584E355548567A6147567958305A4A544555755A58686C567778576F50306C34456468646D6C754A6B4A68596D397A595541794D4445325257467A65555268636E6470626C526C5957316C59584E35"
#include "getopt.h"
#elif define _ARM
#define KEY "6A36334A742F2B2B72624B417848395A706F41347066426C59584E356348567A6147567958325A7062475658444661672F535867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#else //x86 linux
#define KEY "6A36334A74354F576B5971417848395A706F41347066426C59584E356348567A6147567958325A7062475658444661672F535867523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"
#include "unistd.h"
#include <signal.h>
#endif

char* ConfigIP		= "www.easydarwin.org";		//Default EasyDarwin Address
char* ConfigPort	= "554";				//Default EasyDarwin Port
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


#ifndef HANDLE
#define HANDLE void*
#endif//HANDLE

#define MAX_TRACK_NUM 32

//Globle Func for thread callback
unsigned int _stdcall  VideoThread(void* lParam);
unsigned int _stdcall  AudioThread(void* lParam);

//
// Globle var for all of us to use
// 
// Handler use for thrack demux thread
HANDLE g_mp4TrackThread[MAX_TRACK_NUM];
bool  g_bThreadLiving[MAX_TRACK_NUM];
//获取MP4头box信息
CMp4_root_box g_root;
FILE * g_fin = NULL; 
FILE* g_finA = NULL;
Easy_Pusher_Handle g_fPusherHandle = 0;
CRITICAL_SECTION m_cs;
bool g_bVideoStart = false;
EASY_MEDIA_INFO_T   mediainfo;

#define VEDIO_PUSH 0
#define AUDIO_PUSH 1

typedef struct _SYN_CLOCK_CTRL_
{
	unsigned long ClockBase;
	unsigned long ClockCurr;
	unsigned long VedioBase;
	unsigned long AudioBase;

}Sync_clock_Ctl;

Sync_clock_Ctl g_clock;

// Add by Ricky
//Audio and video Sync lock
unsigned long Sync_clock(unsigned long TimeScale, unsigned long duration, int type, unsigned long* out)
{
	unsigned long timebase;
	unsigned long DiffClock;
	double TimeCalbase;
	double Timenext;
	unsigned long CurrentTime;
	unsigned long NextTime;
	unsigned long delay;
#ifdef _WIN32
	if(g_clock.ClockBase == 0)
	{
		g_clock.ClockBase = ::GetTickCount()*1000;

	}
	g_clock.ClockCurr = ::GetTickCount()*1000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	g_clock.ClockCurr = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
	if(g_clock.ClockBase == 0)		{
		g_clock.ClockBase = g_clock.ClockCurr;
	}
	
#endif
	if(type == VEDIO_PUSH)
	{
		timebase = g_clock.VedioBase;
	}else
	{
		timebase = g_clock.AudioBase;	
	}
	
	DiffClock = g_clock.ClockCurr - g_clock.ClockBase;//时钟的耗时间Tick数//微妙级别忽略不计	
	TimeCalbase = (double)timebase/TimeScale;
	Timenext = (double)(timebase+duration)/TimeScale;
	//开始计算当前和小一个Sample的时间估计决定延迟//
	NextTime = (unsigned long)(Timenext*1000000);	
	CurrentTime = (unsigned long)(TimeCalbase*1000000);
	*out = CurrentTime;
	if(DiffClock > NextTime) //已经落后，快进
	{
		delay =  0;
	}else
	{
		delay = (NextTime- DiffClock);//重新计算时间
	}
	if(type == VEDIO_PUSH)
	{
		g_clock.VedioBase += duration;
	}else
	{
		g_clock.AudioBase  += duration;	
	}
	return delay;
}

int main(int argc, char * argv[])
{

	std::string sTestFilm  = "./test.mp4";//[阳光电影www.ygdy8.com].港囧.HD.720p.国语中字.mp4";//6004501011.MP4";
	//std::string sTestFilm  = "D:\\360Downloads\\[阳光电影www.ygdy8.com].港囧.HD.720p.国语中字.mp4";//6004501011.MP4";
	//std::string sTestFilm  = "D:\\360Downloads\\MV.mp4";//6004501011.MP4";

	//Open mp4 file, acturally we just support mp4 packaged by MP4Box
		g_fin = _fsopen(sTestFilm.c_str(), "rb",  _SH_DENYNO );	
		if(g_fin == (FILE*)0)
		{
			printf("failed to open pmp4 file: %s\n", sTestFilm.c_str());
			printf("Press Enter exit...\n");
			getchar();

			return 0;
		}
		g_finA = _fsopen(sTestFilm.c_str(), "rb",  _SH_DENYNO );	
		if(g_finA == (FILE*)0)
		{
			printf("failed to open pmp4 file: %s\n", sTestFilm.c_str());
			printf("Press Enter exit...\n");
			getchar();

			return 0;
		}

	unsigned int cur_pos= _ftelli64(g_fin);
	for(;!feof(g_fin); )
	{
		_fseeki64(g_fin, cur_pos, SEEK_SET);
		printf("----------------------------------------level 0\n");
		cur_pos += g_root.mp4_read_root_box(g_fin);
	}

	printf("---------------------------------------- 0ye\n");
	printf("  %s   MP4Demux is Completed!\n", sTestFilm.c_str());
	printf("---------------------------------------- 0ye\n");


	//////////////////////////////////////////////////////////////////////////
	//Init EasyPusher
	//////////////////////////////////////////////////////////////////////////
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

#ifdef _WIN32
	extern char* optarg;
#endif

	int ch;
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

	char szIP[16] = {0};
	Easy_Pusher_Handle fPusherHandle = 0;

	int buf_size = 1024*512;
	char *pbuf = (char *) malloc(buf_size);
	int position = 0;
	int iFrameNo = 0;
	int timestamp = 0;
	ProgName = argv[0];

	memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));

	//从MP4文件获取音视频编码信息，填入pusher媒体信息结构中
	CMp4_avcC_box videoInfo;
	memset(&videoInfo, 0x00, sizeof(CMp4_avcC_box));
	CMp4_mp4a_box audioInfo;
	memset(&audioInfo, 0x00, sizeof(CMp4_mp4a_box));
	int nVideoTrackId = get_video_info_in_moov(g_root,  videoInfo );
	int nAudioTrackId = get_audio_info_in_moov(g_root,  audioInfo );

	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		g_mp4TrackThread[nI] = 0;
		g_bThreadLiving[nI] = false;
	}

	InitializeCriticalSection(&m_cs);

	//视频轨存在
	if (nVideoTrackId>-1)
	{
		mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;
		mediainfo.u32VideoFps = 25;
		mediainfo.u32SpsLength = videoInfo.sps->sequenceParameterSetLength;
		mediainfo.u32PpsLength = videoInfo.pps->pictureParameterSetLength;
		if (videoInfo.sps->sequenceParameterSetNALUnit && videoInfo.sps->sequenceParameterSetLength>0 )
		{
			memcpy(mediainfo.u8Sps, videoInfo.sps->sequenceParameterSetNALUnit, mediainfo.u32SpsLength);
		}
		if (videoInfo.pps->pictureParameterSetNALUnit && videoInfo.pps->pictureParameterSetLength>0 )
		{
			memcpy(mediainfo.u8Pps, videoInfo.pps->pictureParameterSetNALUnit, mediainfo.u32PpsLength );
		}

		g_mp4TrackThread[nVideoTrackId] = (HANDLE)_beginthreadex(NULL, 0, VideoThread, (void*)nVideoTrackId,0,0);
		g_bThreadLiving[nVideoTrackId] = true;
	}

	//音频轨存在
	if (nAudioTrackId>-1)
	{
		mediainfo.u32AudioCodec =   EASY_SDK_AUDIO_CODEC_AAC;
		mediainfo.u32AudioSamplerate = audioInfo.samplerate;
		mediainfo.u32AudioChannel = audioInfo.channelcount;

		//Create thread to push mp4 demux data( aac)
		g_mp4TrackThread[nAudioTrackId] = (HANDLE)_beginthreadex(NULL, 0, AudioThread,  (void*)nAudioTrackId,0,0);
		g_bThreadLiving[nAudioTrackId] = true;
	}

	int isActivated = EasyPusher_Activate(KEY);
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

	g_fPusherHandle = EasyPusher_Create();

	if(g_fPusherHandle == NULL)
	{
		getchar();
		return -2;
	}

	EasyPusher_SetEventCallback(g_fPusherHandle, __EasyPusher_Callback, 0, NULL);
	EasyPusher_StartStream(g_fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, EASY_RTP_OVER_TCP, "admin", "admin", &mediainfo, 1024, 0);
	memset(&g_clock, 0, sizeof(g_clock));

	printf("Press Enter exit...\n");
	getchar();

	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		g_mp4TrackThread[nI] = 0;
		g_bThreadLiving[nI] = false;
	}

	EasyPusher_StopStream(g_fPusherHandle);
	EasyPusher_Release(g_fPusherHandle);
	g_fPusherHandle = 0;
	DeleteCriticalSection(&m_cs);

	return 0;
}


//MP4 file pusher  calllback
unsigned int _stdcall  VideoThread(void* lParam)
{
	int nTrackId = (int)lParam;
	while (g_bThreadLiving[nTrackId])
	{
		g_bVideoStart = true;
		int chunk_offset_amount    = g_root.co[nTrackId].chunk_offset_amount;
		unsigned long lTimeStamp = 0;
		int nSampleId = 0;
		for(int chunk_index = 0 ; chunk_index < chunk_offset_amount; ++chunk_index)
		{
			if (!g_bThreadLiving[nTrackId])
			{
				return 0;
			}

			//copy_sample_data(g_fin, chunk_index, name,nID,root,nSampleId);
			_fseeki64(g_fin, g_root.co[nTrackId].chunk_offset_from_file_begin[chunk_index], SEEK_SET);

			//获取当前chunk中有多少个sample
			uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(g_root.sc[nTrackId], chunk_index+1);  //@a mark获取chunk中sample的数目
			uint32_t sample_index_ =  get_sample_index(g_root.sc[nTrackId], chunk_index+1);//chunk中第一个sample的序号
			unsigned int cur=_ftelli64(g_fin);
			for(int i = 0; i < sample_num_in_cur_chunk_; i++)
			{
				if (!g_bThreadLiving[nTrackId])
				{
					return 0;
				}
// #ifdef _WIN32
// 				DWORD dwStart = ::GetTickCount();
// #endif
				uint32_t sample_size = get_sample_size(g_root.sz[nTrackId], sample_index_+i);//获取当前sample的大小
				uint32_t sample_time = get_sample_time(g_root.ts[nTrackId], nSampleId );
				//double dbSampleTime = (double)sample_time/g_root.trk[nTrackId].mdia.mdhd.timescale ;
				//uint32_t uSampleTime = dbSampleTime*1000000;

				EnterCriticalSection(&m_cs);
				uint32_t uSampleTime = Sync_clock(g_root.trk[nTrackId].mdia.mdhd.timescale, sample_time,VEDIO_PUSH, &lTimeStamp);
				LeaveCriticalSection(&m_cs);

				_fseeki64(g_fin,cur,SEEK_SET);
				int nBufLen = sample_size+mediainfo.u32SpsLength+mediainfo.u32PpsLength+8;
				unsigned char *ptr=new unsigned char [nBufLen];
				fread(ptr, sample_size, 1, g_fin);

				//写一帧数据 --- 可以直接进行网络推送
				//fwrite(ptr, sample_size, 1, fout);
				EASY_AV_Frame	avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));

				byte btHeader[4] = {0x00,0x00,0x00,0x01};
				ptr[0] = 0x00;
				ptr[1] = 0x00;
				ptr[2] = 0x00;
				ptr[3] = 0x01;
				unsigned char naltype = ( (unsigned char)ptr[4] & 0x1F);
				if (naltype==0x05)//I帧
				{
					memmove(ptr+mediainfo.u32SpsLength+mediainfo.u32PpsLength+8, ptr, sample_size);
					memcpy(ptr, btHeader, 4);
					memcpy(ptr+4, mediainfo.u8Sps, mediainfo.u32SpsLength);
					memcpy(ptr+4+mediainfo.u32SpsLength, btHeader, 4);
					memcpy(ptr+4+mediainfo.u32SpsLength+4, mediainfo.u8Pps, mediainfo.u32PpsLength);
				}

				avFrame.pBuffer = (unsigned char*)ptr;
				avFrame.u32AVFrameLen = nBufLen;
				avFrame.u32VFrameType = (naltype==0x05)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
				avFrame.u32TimestampSec = lTimeStamp/1000000;
				avFrame.u32TimestampUsec = (lTimeStamp%1000000);

				//EnterCriticalSection(&m_cs);
				EasyPusher_PushFrame(g_fPusherHandle, &avFrame);
				//LeaveCriticalSection(&m_cs);
				
				//lTimeStamp += uSampleTime;
			
// #ifdef _WIN32
// 
// 				DWORD dwStop = ::GetTickCount();
// #endif
				//printf("Sleep=%d\r\n", uSampleTime/1000-(dwStop-dwStart));
				if(uSampleTime!=0)
				{
#ifndef _WIN32
				usleep(uSampleTime);
#else
				SleepEx(uSampleTime/1000, FALSE);
#endif
				}
				delete [] ptr;
				cur+=sample_size;
				nSampleId++;
			}
		}
	}
	return 0;
}


unsigned int _stdcall  AudioThread(void* lParam)
{

	int nTrackId = (int)lParam;
	while (g_bThreadLiving[nTrackId])
	{
		if (!g_bVideoStart)
		{
			Sleep(1);
			printf("Audio Thread waiting.........\r\n");
			continue;
		}
		int chunk_offset_amount    = g_root.co[nTrackId].chunk_offset_amount;
		unsigned long lTimeStamp = 0;
		int nSampleId = 0;
		for(int chunk_index = 0 ; chunk_index < chunk_offset_amount; ++chunk_index)
		{
			if (!g_bThreadLiving[nTrackId])
			{
				return 0;
			}
			//copy_sample_data(g_fin, chunk_index, name,nID,root,nSampleId);
			_fseeki64(g_finA, g_root.co[nTrackId].chunk_offset_from_file_begin[chunk_index], SEEK_SET);

			//获取当前chunk中有多少个sample
			uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(g_root.sc[nTrackId], chunk_index+1);  //@a mark获取chunk中sample的数目
			uint32_t sample_index_ =  get_sample_index(g_root.sc[nTrackId], chunk_index+1);//chunk中第一个sample的序号
			unsigned int cur=_ftelli64(g_finA);
			for(int i = 0; i < sample_num_in_cur_chunk_; i++)
			{
				if (!g_bThreadLiving[nTrackId])
				{
					return 0;
				}

// #ifdef _WIN32
// 			DWORD dwStart = ::GetTickCount();
// #endif
				uint32_t sample_size = get_sample_size(g_root.sz[nTrackId], sample_index_+i);//获取当前sample的大小
				uint32_t sample_time = get_sample_time(g_root.ts[nTrackId], nSampleId );
				//double dbSampleTime = (double)sample_time/g_root.trk[nTrackId].mdia.mdhd.timescale ;
				//uint32_t uSampleTime = dbSampleTime*1000000;

				EnterCriticalSection(&m_cs);
				uint32_t uSampleTime = Sync_clock(g_root.trk[nTrackId].mdia.mdhd.timescale, sample_time,AUDIO_PUSH, &lTimeStamp);
				LeaveCriticalSection(&m_cs);

				_fseeki64(g_finA,cur,SEEK_SET);
				unsigned char *ptr=new unsigned char [sample_size];
				fread(ptr, sample_size, 1, g_finA);

				//写一帧数据 --- 可以直接进行网络推送
				//fwrite(ptr, sample_size, 1, fout);
				EASY_AV_Frame	avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));

				avFrame.pBuffer = (unsigned char*)ptr;
				avFrame.u32AVFrameLen = sample_size;
				avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
				avFrame.u32TimestampSec = lTimeStamp/1000000;
				avFrame.u32TimestampUsec = (lTimeStamp%1000000);

// 				EnterCriticalSection(&m_cs);
 				EasyPusher_PushFrame(g_fPusherHandle, &avFrame);
// 				LeaveCriticalSection(&m_cs);

				//lTimeStamp += uSampleTime;
// #ifdef _WIN32
// 				DWORD dwStop = ::GetTickCount();
// #endif
				if(uSampleTime!=0)
				{
#ifndef _WIN32
				usleep(uSampleTime);
#else
				SleepEx(uSampleTime/1000, FALSE);
#endif
				}
				delete [] ptr;
				cur+=sample_size;
				nSampleId++;
			}
		}
	}

	return 0;
}


// int main(int argc, char * argv[])
// {
// 	int isActivated = 0 ;
// #ifndef _WIN32
//    signal(SIGPIPE, SIG_IGN);
// #endif
// 
// #ifdef _WIN32
// 	extern char* optarg;
// #endif
// 	int ch;
//     char szIP[16] = {0};
//     Easy_Pusher_Handle fPusherHandle = 0;
//     EASY_MEDIA_INFO_T   mediainfo;
// 
//     int buf_size = 1024*512;
//     char *pbuf = (char *) malloc(buf_size);
//     FILE *fES = NULL;
// 	int position = 0;
// 	int iFrameNo = 0;
// 	int timestamp = 0;
// 	ProgName = argv[0];
// 	PrintUsage();
// 
// 
// 	while ((ch = getopt(argc,argv, "hd:p:n:")) != EOF) 
// 	{
// 		switch(ch)
// 		{
// 		case 'h':
// 			PrintUsage();
// 			return 0;
// 			break;
// 		case 'd':
// 			ConfigIP =optarg;
// 			break;
// 		case 'p':
// 			ConfigPort =optarg;
// 			break;
// 		case 'n':
// 			ConfigName =optarg;
// 			break;
// 		case '?':
// 			return 0;
// 			break;
// 		default:
// 			break;
// 		}
// 	}
//     memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
//     mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;
// 	mediainfo.u32VideoFps = 25;
// 
//     fES = fopen("./EasyPusher.264", "rb");
//     if (NULL == fES)        return 0;
// 
// 	isActivated = EasyPusher_Activate(KEY);
// 	switch(isActivated)
// 	{
// 	case EASY_ACTIVATE_INVALID_KEY:
// 		printf("KEY is EASY_ACTIVATE_INVALID_KEY!\n");
// 		break;
// 	case EASY_ACTIVATE_TIME_ERR:
// 		printf("KEY is EASY_ACTIVATE_TIME_ERR!\n");
// 		break;
// 	case EASY_ACTIVATE_PROCESS_NAME_LEN_ERR:
// 		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_LEN_ERR!\n");
// 		break;
// 	case EASY_ACTIVATE_PROCESS_NAME_ERR:
// 		printf("KEY is EASY_ACTIVATE_PROCESS_NAME_ERR!\n");
// 		break;
// 	case EASY_ACTIVATE_VALIDITY_PERIOD_ERR:
// 		printf("KEY is EASY_ACTIVATE_VALIDITY_PERIOD_ERR!\n");
// 		break;
// 	case EASY_ACTIVATE_SUCCESS:
// 		printf("KEY is EASY_ACTIVATE_SUCCESS!\n");
// 		break;
// 	}
// 
// 	if(EASY_ACTIVATE_SUCCESS != isActivated)
// 		return -1;
// 
//     fPusherHandle = EasyPusher_Create();
// 
// 	if(fPusherHandle == NULL)
// 		return -2;
// 
//     EasyPusher_SetEventCallback(fPusherHandle, __EasyPusher_Callback, 0, NULL);
// 
// 	EasyPusher_StartStream(fPusherHandle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 1024, 0);
// 
// 	while (1)
// 	{
// 		int nReadBytes = fread(pbuf+position, 1, 1, fES);
// 		if (nReadBytes < 1)
// 		{
// 			if (feof(fES))
// 			{
// 				position = 0;
// 				fseek(fES, 0, SEEK_SET);
// 				continue;
// 			}
// 			break;
// 		}
// 
// 		position ++;
// 
// 		if (position > 5)
// 		{
// 			unsigned char naltype = ( (unsigned char)pbuf[position-1] & 0x1F);
// 
// 			if (	(unsigned char)pbuf[position-5]== 0x00 && 
// 					(unsigned char)pbuf[position-4]== 0x00 && 
// 					(unsigned char)pbuf[position-3] == 0x00 &&
// 					(unsigned char)pbuf[position-2] == 0x01 &&
// 					(naltype == 0x07 || naltype == 0x01 ) )
// 			{
// 				int framesize = position - 5;
//                 EASY_AV_Frame   avFrame;
// 
// 				naltype = (unsigned char)pbuf[4] & 0x1F;
// 
//                 memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
//                 avFrame.u32AVFrameLen   =   framesize;
//                 avFrame.pBuffer = (unsigned char*)pbuf;
// 				avFrame.u32VFrameType = (naltype==0x07)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
// 				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
// 				avFrame.u32TimestampSec = timestamp/1000;
// 				avFrame.u32TimestampUsec = (timestamp%1000)*1000;
//                 EasyPusher_PushFrame(fPusherHandle, &avFrame);
// 				timestamp += 1000/mediainfo.u32VideoFps;
// 
// #ifndef _WIN32
//                 usleep(30*1000);
// #else
//                 Sleep(30);
// #endif
// 				memmove(pbuf, pbuf+position-5, 5);
// 				position = 5;
// 
// 				iFrameNo ++;
// 
//                 //if (iFrameNo > 100000) break;
//                 //break;
// 			}
// 		}
// 	}
// 
//     _TRACE("Press Enter exit...\n");
//     getchar();
// 
//     EasyPusher_StopStream(fPusherHandle);
//     EasyPusher_Release(fPusherHandle);
// 	free(pbuf);
//     return 0;
// }
