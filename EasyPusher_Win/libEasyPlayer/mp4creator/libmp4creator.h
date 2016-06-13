/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#ifndef __LIB_MP4_CREATOR_H__
#define __LIB_MP4_CREATOR_H__


#define LIB_MP4CREATOR_API __declspec(dllexport)


#ifndef MEDIA_TYPE_VIDEO
#define MEDIA_TYPE_VIDEO		0x01
#endif
#ifndef MEDIA_TYPE_AUDIO
#define MEDIA_TYPE_AUDIO		0x02
#endif


#define	VIDEO_CODEC_H264		0x1C
#define AUDIO_CODEC_AAC			0x40

typedef void *MP4C_Handler;


//初始化句柄
int	LIB_MP4CREATOR_API	MP4C_Init(MP4C_Handler *handler);
//创建录像文件
int	LIB_MP4CREATOR_API	MP4C_CreateMp4File(MP4C_Handler handler, char *filename, unsigned int _file_buf_size/*内存缓冲,当缓冲满了之后才会写入文件, 如果为0则直接写入文件*/);
//设置视频参数
int	LIB_MP4CREATOR_API	MP4C_SetMp4VideoInfo(MP4C_Handler handler, unsigned int codec,	unsigned short width, unsigned short height, unsigned int fps);
//设置音频参数
int	LIB_MP4CREATOR_API	MP4C_SetMp4AudioInfo(MP4C_Handler handler, unsigned int codec,	unsigned int sampleFrequency, unsigned int channel);
//设置H264中的SPS
int	LIB_MP4CREATOR_API	MP4C_SetH264Sps(MP4C_Handler handler, unsigned short sps_len, unsigned char *sps);
//设置H264中的PPS
int	LIB_MP4CREATOR_API	MP4C_SetH264Pps(MP4C_Handler handler, unsigned short pps_len, unsigned char *pps);

//从帧数据中提取SPS和PPS,提取结果后调用MP4C_SetH264Sps和MP4C_SetH264Pps
//帧数据中需包含 start code
int LIB_MP4CREATOR_API  MP4C_GetSPSPPS(char *pbuf, int bufsize, char *_sps, int *_spslen, char *_pps, int *_ppslen);


/*
写入媒体数据
//不论输入是视频或视频, 直接调用MP4C_AddFrame写入即可, 库内部会进行判断,在写完一个GOP后写入对应时间段的音频
//现音频仅支持AAC   8KHz 和  44.1KHz
pbuf可以有start code 00 00 00 01也可以没有,库里面已做判断
如果没有start code, 则需调用MP4C_SetH264Sps和MP4C_SetH264Pps设置相应的sps和pps

帧数据为以下几种情况时可不调用MP4C_SetH264Sps和MP4C_SetH264Pps:
1.  start code + sps + start code + pps + start code + idr
2.  start code + sps            start code + pps   即start code+sps为一帧, start code+pps为一帧
*/
int	LIB_MP4CREATOR_API	MP4C_AddFrame(MP4C_Handler handler, unsigned int mediatype, unsigned char *pbuf, unsigned int framesize, unsigned char keyframe, unsigned int timestamp_sec, unsigned int timestamp_rtp, unsigned int fps);

//关闭MP4文件, 返回文件大小
unsigned int LIB_MP4CREATOR_API	MP4C_CloseMp4File(MP4C_Handler handler);

//释放句柄
int	LIB_MP4CREATOR_API	MP4C_Deinit(MP4C_Handler *handler);



/*
MP4C_Handler	mp4Handle = NULL;

MP4C_Init(&mp4Handle);
MP4C_SetMp4VideoInfo(mp4Handle, VIDEO_CODEC_H264, frameinfo.width, frameinfo.height, frameinfo.fps);
MP4C_SetMp4AudioInfo(mp4Handle, AUDIO_CODEC_AAC, 8000, 1);
MP4C_CreateMp4File(mp4Handle, szMp4Filename, 1024*1024*2);


MP4C_AddFrame(mp4Handle, MEDIA_TYPE_VIDEO, (unsigned char*)pbuf, frameinfo.length, frameinfo.type, frameinfo.timestamp_sec, frameinfo.rtptimestamp, pts);
MP4C_AddFrame(mp4Handle, MEDIA_TYPE_AUDIO, (unsigned char*)pAudBuf, lSize, 0x01, frameinfo.timestamp_sec, frameinfo.rtptimestamp, fps);


MP4C_CloseMp4File(mp4Handle);
MP4C_Deinit(&mp4Handle);
	
*/


#endif
