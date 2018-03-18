/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyDarwin推送和接收源端的管理类 [11/8/2015 Dingshuai]
// Add by SwordTwelve
#pragma once

#define EasyPusher_VersionInfo _T("Version:1.3.17.0524 Powered By SwordTwelve/Gavin/Arno")

// //本地音频捕获
#include "AudioSource\DirectSound.h"
//本地视频捕获
#include "VideoSource\USBCamera\CameraDS.h"
//网络音视频流采集
#include "EasyPlayerManager.h"
//DShow音视频采集库头文件添加（添加该库主要是为了解决音视频源头上不同步的问题）
#include "./DShowCapture/DShowCaptureAudioAndVideo_Interface.h"
//屏幕采集
#include "CaptureScreen.h"
#include "D3DCaptureScreem.h"
//MP4 file 采集
#include "EasyFileCapture.h"

#include "./FFEncoder/FFEncoderAPI.h"
#pragma comment(lib, "./FFEncoder/FFEncoder.lib")
//MP4Box Package MP4
#include "EasyMP4Writer.h"

//x264编码+FAAC编码+librtmp推送 支持
#include "./EasyEncoder/FAACEncoder.h"
#include "./EasyEncoder/H264Encoder.h"
#include "./EasyEncoder/H264EncoderManager.h"
#include "./EasyEncoder/EasyRtmp.h"

// 字幕和图片叠加 [3/18/2018 SwordTwelve]
#include "EasyDefine.h"
#include "./EasyVFX/VideoVFXMakerInterface.h"
#pragma comment(lib, "./EasyVFX/VideoVFXMakerMFCDll.lib")

extern "C"
{
//MP4Creater Package MP4
#include "./mp4creator/libmp4creator.h"
#pragma comment(lib, "./mp4creator/libMp4Creator.lib")
}

typedef struct tagPushServerURLInfo
{
	char	pushServerAddr[128];
	int		pushServerPort;
	char	sdpName[64];
	int nPushBufferLenth;
}ServerURLInfo;

typedef enum tagSOURCE_TYPE
{
	SOURCE_LOCAL_CAMERA = 0,//本地音视频
	SOURCE_RTSP_STREAM=1,//RTSP流
	SOURCE_SCREEN_CAPTURE =2,//屏幕捕获
	SOURCE_FILE_STREAM = 3,       //文件流推送(mp4)
	// 	//SOURCE_ONVIF_STREAM=3,//Onvif流

}SOURCE_TYPE;

typedef struct tagSourceConfigInfo
{
	int nId;
	int nPannelType;
	int nSourceType;
	int nFileType;
	int nStartTime;
	int nEndTime;
	char strFilePath[MAX_PATH];
}SourceConfigInfo;

typedef struct tagAVCapParamInfo
{
	int nVWidth;
	int nVHeight;
	int nFps; 
	int nBitrate;
	char strColorFormat[64];//视频色彩格式
	int nASampleRate;//音频采样率
	int nAChannels;//音频通道数
}AVCapParamInfo;

class CEasyPusherDlg ;

class CSourceManager
{
public:
	CSourceManager(void);
	~CSourceManager(void);

public:
	static CSourceManager* s_pSourceManager; 
	//初始化唯一的管理实例
	static CSourceManager* Instance();
	//销毁唯一的管理实例
	static void UnInstance();
	//释放Master所占相关资源
	void RealseManager();
	// Member Function Interface
public:
	//执行视频源初始化操作
	void InitSource();
	//反初始化
	void UnInitSource();
	//DShow实时数据回调函数
	static int CALLBACK RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
																	   RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	void DSRealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);
	//RTSP流数据回调
	static int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);
	int SourceManager(int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);

	static int CALLBACK CaptureScreenCallBack(int nId, unsigned char *pBuffer, int nBufSize,  RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	void CaptureScreenManager(int nId, unsigned char *pBuffer, int nBufSize,  RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);

	static  unsigned int CALLBACK CaptureFileCallBack(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);
	void CaptureFileManager(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);

	int StartDSCapture(int nCamId, int nAudioId,HWND hShowWnd, int nVideoWidth, int nVideoHeight, int nFps, int nBitRate, char* szDataype = "YUY2", int nSampleRate = 44100, int nChannel = 2);
	//文件源采集
	// 
	
	int InitFileCapture(char* sFilePath, int nStartTime, int nEndTime, bool bAutoLoop);
	void StartFileCapture();

	//开始捕获(采集)
	int StartCapture(SOURCE_TYPE eSourceType, int nCamId, int nAudioId,  HWND hCapWnd, char* szFilePath = NULL, int nStartTime=-1, int nEndTime=-1, bool bAutoLoop=true,
		char* szURL = NULL, int nVideoWidth=640, int nVideoHeight=480, int nFps=25, int nBitRate=2048, char* szDataType = "YUY2",  
		int nSampleRate=44100, int nChannel=2 );
	//停止采集
	void StopCapture();

	//开始推流
	int StartPush(char* ServerIp, int nPushPort, char* sPushName, int nPushBufSize = 1024, bool bPushRtmp = false);
	//停止推流
	void StopPush();
	
	//开始播放
	int StartPlay(char* szURL, HWND hShowWnd);
	//停止播放
	void StopPlay();
	void SetMainDlg(	CEasyPusherDlg* pMainDlg);
	void LogErr(CString strLog);
	void EnumLocalAVDevInfo(CWnd* pComboxMediaSource, CWnd* pComboxAudioSource);
	void ResizeVideoWnd();

	//屏幕采集
	int StartScreenCapture(HWND hShowWnd, int nCapMode);
	void StopScreenCapture();
	void RealseScreenCapture();
	int GetScreenCapSize(int& nWidth, int& nHeight);

	//写MP4文件(录制相关)
	int CreateMP4Writer(char* sFileName, int nVWidth, int nVHeight, int nFPS, int nSampleRate, int nChannel, int nBitsPerSample, int nFlag=ZOUTFILE_FLAG_FULL, BOOL bUseGpac = FALSE	);
	int WriteMP4VideoFrame(unsigned char* pdata, int datasize, bool keyframe, long nTimestamp, int nWidth, int nHeight);
	int WriteMP4AudioFrame(unsigned char* pdata,int datasize, long timestamp);
	void CloseMP4Writer();	

	// 字幕图片叠加接口 [3/18/2018 SwordTwelve]
	//   设置台标/LOGO
	// 		player				- 指向 EasyPlayerPro_Open 返回的 player 对象
	// 		bIsUse				- 是否使用水印 1=启用 0=不启用
	// 		ePos				- 台标位置：1==leftttop 2==righttop 3==leftbottom 4==rightbottom
	// 		eStyle				-  水印的风格，见WATERMARK_ENTRY_TYPE声明
	// 		x					- 水印左上角位置x坐标
	// 		y					- 水印左上角位置y坐标
	// 		width				- 宽
	// 		height				- 高
	// 		logopath			- 水印图片路径
	int   SetLogo (int bIsUse, int ePos, int eStyle, 
		int x, int y, int width, int height, char* logopath);

	//   设置叠加字幕
	// 		player				- 指向 EasyPlayerPro_Open 返回的 player 对象
	// 		bIsUse				- 是否使用水印 1=启用 0=不启用 -1=删除
	// 		nMoveType			- 移动类型：0--固定位置，1--从左往右，2--从右往左，
	//		R,G,B				- 字体颜色对应三个分量红绿蓝0-255
	// 		x					- 字幕显示左上角位置x坐标
	// 		y					- 字幕显示左上角位置y坐标
	// 		weight				- 字体权重，见如下声明
	// /* Font Weights */
	// #define FW_DONTCARE         0
	// #define FW_THIN             100
	// #define FW_EXTRALIGHT       200
	// #define FW_LIGHT            300
	// #define FW_NORMAL           400
	// #define FW_MEDIUM           500
	// #define FW_SEMIBOLD         600
	// #define FW_BOLD             700
	// #define FW_EXTRABOLD        800
	// #define FW_HEAVY            900
	// #define FW_ULTRALIGHT       FW_EXTRALIGHT
	// #define FW_REGULAR          FW_NORMAL
	// #define FW_DEMIBOLD         FW_SEMIBOLD
	// #define FW_ULTRABOLD        FW_EXTRABOLD
	// #define FW_BLACK            FW_HEA
	// 		width				- 宽
	// 		height				- 高
	// 		fontname			- 字体名称，如“宋体”“楷体”“隶书”“华文行楷”......
	//		tittleContent		- OSD显示内容
	int   SetOSD ( int bIsUse, int nMoveType, int R, int G, int B,
		int weight, int x, int y, int width, int height, char* fontname, char* tittleContent);

	//状态
	BOOL IsInCapture()
	{
		BOOL bCap = FALSE;
		int nStreamCap = m_netStreamCapture.InRunning();
		//int nVideoCap = m_videoCamera.InRunning();
		if (m_bDSCapture ||nStreamCap>0 )
			bCap = TRUE;
		else
			bCap = FALSE;
		return bCap ;
	}
	BOOL IsInPushing()
	{
		//return (int)m_sPushInfo.pusherHandle>0?TRUE:FALSE;
		return m_bPushing;
	}
	BOOL IsInPlaying()
	{
		return m_netStreamPlayer.InRunning()>0?TRUE:FALSE;
	}

	BOOL IsRecording()
	{
		return m_bRecording;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL IsUseGpac()
	{
		return m_bUseGpac;
	}
	BOOL IsWriteMP4()
	{
		return m_bWriteMp4;
	}
	BOOL IsUseFFEncoder()
	{
		return m_bUseFFEncoder;
	}
	BOOL IsPushRtmp()
	{
		return m_bPushRtmp;
	}
	//////////////////////////////////////////////////////////////////////////
	void SetUseGpac(BOOL bUse)
	{
		 m_bUseGpac = bUse;
	}
	void SetWriteMP4(BOOL bUse)
	{
		 m_bWriteMp4 = bUse;
	}
	void SetUseFFEncoder(BOOL bUse)
	{
		 m_bUseFFEncoder = bUse;
	}
	void SetPushRtmp(BOOL bUse)
	{
		 m_bPushRtmp = bUse;
	}

protected:
		void	UpdateLocalVideo(unsigned char *pbuf, int size, int width, int height);

	//Member Var
private:
	CEasyPusherDlg* m_pMainDlg;
	CDirectSound	m_audioCapture;
	CCameraDS		m_videoCamera;
	CCaptureScreen* m_pScreenCaptrue;
	CD3DCaptureScreem* m_pD3dScreenCaptrue;
	CEasyFileCapture* m_pEasyFileCapture;

	int m_nScreenCaptureId;
	//MP4Box Writer
	EasyMP4Writer* m_pMP4Writer;
	//MP4 Creater Handler
	MP4C_Handler m_handler;
	BOOL m_bUseGpac;
	BOOL m_bWriteMp4;
	BOOL m_bUseFFEncoder;
	BOOL m_bPushRtmp;

	//视频设备控制实例
	LPVideoCapturer m_pVideoManager;
	//音频设备控制实例
	LPAudioCapturer m_pAudioManager;

	//接收网络RTSP流进行推流
	EasyPlayerManager m_netStreamCapture;
	//接收EasyDarwin推出的RTSP流进行播放
	EasyPlayerManager m_netStreamPlayer;

	//本地Dshow捕获参数设置
	DEVICE_CONFIG_INFO m_sDevConfigInfo;
	EASY_MEDIA_INFO_T   m_mediainfo;
	EASY_LOCAL_SOURCE_T m_sSourceInfo;
	EASY_LOCAL_SOURCE_T m_sPushInfo;
	EASY_LOCAL_SOURCE_T m_sPlayInfo;

	BOOL m_bDSCapture;
	D3D_HANDLE		m_d3dHandle;
	HWND m_hCaptureWnd;
	HWND m_hPlayWnd;
	BOOL m_bPushing;
	BOOL m_bRecording;
	//编码器相关
	//FFEncoder --- Start
	FFE_HANDLE m_hFfeVideoHandle;
	FFE_HANDLE m_hFfeAudioHandle;
	int m_nFrameNum;
	char * m_EncoderBuffer;// = new char[1920*1080];	//申请编码的内存空间
	// FFEncoder --- End
	// x264+faac Encoder --- Start
	//AAC编码器
	FAACEncoder m_AACEncoderManager;
	//H264编码器
	CH264EncoderManager m_H264EncoderManager;
	//编码信息配置
	Encoder_Config_Info*	m_pEncConfigInfo;
	byte m_sps[100];
	byte  m_pps[100];
	long m_spslen;
	long m_ppslen;
	byte* m_pFrameBuf; 
	// x264+faac Encoder---End
	EasyRtmp* m_pEasyrtmp;

	// 字幕图片叠加特技制作管理实列 [3/18/2018 SwordTwelve]
	LPVideoVFXMaker m_pVFXMaker;
	//VFX配置信息
	VFXMakerInfo	 m_vfxConfigInfo;

};

