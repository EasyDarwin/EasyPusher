/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once

#include "libEasyPlayerAPI.h"
#include "EasyRTSPClient\EasyRTSPClientAPI.h"
#include "FFDecoder\FFDecoderAPI.h"
#include "D3DRender\D3DRenderAPI.h"
#include "SoundPlayer.h"
#include "ssqueue.h"
#pragma comment(lib, "EasyRTSPClient/libEasyRTSPClient.lib")
#pragma comment(lib, "FFDecoder/FFDecoder.lib")
#pragma comment(lib, "D3DRender/D3DRender.lib")

extern "C"
{
#include "mp4creator\libmp4creator.h"
//#pragma comment(lib, "libMp4Creator.lib")
}

#define		MAX_CHANNEL_NUM		64		//可以解码显示的最大通道数
#define		MAX_DECODER_NUM		5		//一个播放线程中最大解码器个数
#define		MAX_YUV_FRAME_NUM	3		//解码后的最大YUV帧数
#define		MAX_CACHE_FRAME		30		//最大帧缓存,超过该值将只播放I帧
#define		MAX_AVQUEUE_SIZE	(1024*1024)	//队列大小
//#define		MAX_AVQUEUE_SIZE	(1920*1080*2)	//队列大小

typedef struct __CODEC_T
{
	//Video Codec
	unsigned int	vidCodec;
	int				width;
	int				height;
	int				fps;
	float			bitrate;

	//Audio Codec
	unsigned int	audCodec;
	int				samplerate;
	int				channels;
}CODEC_T;
typedef struct __DECODER_OBJ
{
	CODEC_T			codec;
	FFD_HANDLE		ffDecoder;
	int				yuv_size;
}DECODER_OBJ;


typedef struct __THREAD_OBJ
{
	int			flag;
	HANDLE		hThread;
}THREAD_OBJ;

typedef struct _YUV_FRAME_INFO			//YUV信息
{
	MEDIA_FRAME_INFO	frameinfo;
	char	*pYuvBuf;
	int		Yuvsize;
}YUV_FRAME_INFO;

typedef struct __PLAY_THREAD_OBJ
{
	THREAD_OBJ		decodeThread;		//解码线程
	THREAD_OBJ		displayThread;		//显示线程

	Easy_RTSP_Handle		nvsHandle;
	HWND			hWnd;				//显示视频的窗口句柄
	int				channelId;			//通道号
	int				showStatisticalInfo;//显示统计信息

	int				frameCache;		//帧缓存(用于调整流畅度),由上层应用设置
	int				initQueue;		//初始化队列标识
	SS_QUEUE_OBJ_T	*pAVQueue;		//接收rtsp的帧队列
	int				frameQueue;		//队列中的帧数
	int				findKeyframe;	//是否需要查找关键帧标识
	int				decodeYuvIdx;

	DWORD			dwLosspacketTime;	//丢包时间
	DWORD			dwDisconnectTime;	//断线时间

	DECODER_OBJ		decoderObj[MAX_DECODER_NUM];
	D3D_HANDLE		d3dHandle;		//显示句柄
	D3D_SUPPORT_FORMAT	renderFormat;	//显示格式
	int				ShownToScale;		//按比例显示
	int				decodeKeyFrameOnly;	//仅解码显示关键帧

	unsigned int	rtpTimestamp;
	LARGE_INTEGER	cpuFreq;		//cpu频率
	_LARGE_INTEGER	lastRenderTime;	//最后显示时间

	int				yuvFrameNo;		//当前显示的yuv帧号
	YUV_FRAME_INFO	yuvFrame[MAX_YUV_FRAME_NUM];
	CRITICAL_SECTION	crit;
	bool			resetD3d;		//是否需要重建d3dRender
	RECT			rcSrcRender;
	D3D9_LINE		d3d9Line;

	char			manuRecordingFile[MAX_PATH];
	int				manuRecording;
	MP4C_Handler	mp4cHandle;
	int				vidFrameNum;

	MediaSourceCallBack pCallback;
	void			*pUserPtr;
}PLAY_THREAD_OBJ;


//音频播放线程
typedef struct __AUDIO_PLAY_THREAD_OBJ
{
	int				channelId;		//当前播放通道号

	unsigned int	samplerate;	//采样率
	unsigned int	audiochannels;	//声道
	unsigned int	bitpersample;

	//CWaveOut		*pWaveOut;
	CSoundPlayer	*pSoundPlayer;
}AUDIO_PLAY_THREAD_OBJ;


class CChannelManager
{
public:
	CChannelManager(void);
	virtual ~CChannelManager(void);

	int		Initial();

	//OpenStream 返回一个可用的通道ID
	int		OpenStream(const char *url, HWND hWnd, RENDER_FORMAT renderFormat, int _rtpovertcp, const char *username, const char *password, MediaSourceCallBack callback=NULL, void *userPtr=NULL);
	void	CloseStream(int channelId);
	int		ShowStatisticalInfo(int channelId, int _show);
	int		SetFrameCache(int channelId, int _cache);
	int		SetShownToScale(int channelId, int ShownToScale);
	int		SetDecodeType(int channelId, int _decodeKeyframeOnly);
	int		SetRenderRect(int channelId, LPRECT lpSrcRect);
	int		DrawLine(int channelId, LPRECT lpRect);
	int		SetDragStartPoint(int channelId, POINT pt);
	int		SetDragEndPoint(int channelId, POINT pt);
	int		ResetDragPoint(int channelId);

	//同一时间只支持一路声音播放
	int		PlaySound(int channelId);
	int		StopSound();


	int		StartManuRecording(int channelId);
	int		StopManuRecording(int channelId);


	static LPTHREAD_START_ROUTINE __stdcall _lpDecodeThread( LPVOID _pParam );
	static LPTHREAD_START_ROUTINE __stdcall _lpDisplayThread( LPVOID _pParam );



	int		ProcessData(int _chid, int mediatype, char *pbuf, RTSP_FRAME_INFO *frameinfo);
protected:
	PLAY_THREAD_OBJ			*pRealtimePlayThread;		//实时播放线程
	AUDIO_PLAY_THREAD_OBJ	*pAudioPlayThread;			//音频播放线程
	CRITICAL_SECTION		crit;

	D3D_ADAPTER_T		d3dAdapter;
	bool				GetD3DSupportFormat();			//获取D3D支持的格式

	void	CreatePlayThread(PLAY_THREAD_OBJ	*_pPlayThread);
	void	ClosePlayThread(PLAY_THREAD_OBJ		*_pPlayThread);

	int		SetAudioParams(unsigned int _channel, unsigned int _samplerate, unsigned int _bitpersample);
	void	ClearAllSoundData();

	void	Release();
};
extern CChannelManager	*pChannelManager;
