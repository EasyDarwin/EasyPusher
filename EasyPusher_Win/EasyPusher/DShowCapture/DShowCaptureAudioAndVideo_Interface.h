/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// Add by SwordTwelve

//////////////////////////////////////////////////////////////////////////
//
// DShow获取摄像机视频和输入音频原始数据Dll导出类定义
// DShowCaptureAudioAndVideo_Interface.h
// 
//////////////////////////////////////////////////////////////////////////

#ifndef DSHOWCAPTUREAUDIOANDVIDEO_H
#define DSHOWCAPTUREAUDIOANDVIDEO_H

#ifdef __cplusplus

//为了实现而做的纯虚函数
//因为C＋＋中引入的话就需要源代码，然而按这样去做的话才能只要文件头不用代码了。

//实时数据流的类型
typedef enum _REALDATA_TYPE
{
	REALDATA_RAW = 0,				//网络中直接收到的码流,包含视频、音频等等
	REALDATA_VIDEO,					//原始视频码流,可能是YUY2 UYVY等等
	REALDATA_AUDIO,					//原始音频码流,可能是PCM等等
	REALDATA_H264,					//视频编码后的H264视频数据
	REALDATA_AAC,					//音频编码后的AAC音频数据
}RealDataStreamType;

// 设备捕获参数信息
typedef struct tagDEVICE_CONFIG_INFO
{
	int nVideoId;//视频	ID -1==不用，0-n == id
	int nAudioId;//音频id -1==不用，0-n == id
	int  nDeviceId;//设备序号 1,2,3,4,5....
	struct tagVideoInfo
	{
		int nWidth;
		int nHeight;
		int nVideoWndId;
		int nFrameRate;//视频帧率
		char strDataType[64];//标识YUV数据类型，如"YUY2" "YUYV" "UYVY等"
		int nRenderType;//呈现方式
		int nPinType;//捕获口
	}VideoInfo;

	struct tagAudioInfo
	{
		int nChannaels;//通道数
		int nBytesPerSample;//采样位数
		int nSampleRate;//采样率
		DWORD nAudioBufferType;//音频缓存区的大小
		int nPinType;//捕获口
	}AudioInfo;

}DEVICE_CONFIG_INFO;

//实时数据流的信息 与 REALDATA_TYPE 配合使用
typedef struct _realdata_callback_param
{	
	int		size;
	struct _video{
		int iEnable;	//该结构体内的信息是否有效 //REALDATA_VIDEO or REALDATA_YUV 有效
		unsigned int timestampe;	//时间戳信息，单位毫秒(ms)
		int width;		//视频宽度	//REALDATA_YUV 有效
		int height;		//视频长度  //REALDATA_YUV 有效
		int encodeType;	//视频编码类型 0:未知 H264 = 1 or mpeg4 = 2 //当前默认为H264 //REALDATA_VIDEO 有效
		int frameType;	//帧类型 0:未知 IDR帧=1 I帧=2 P帧=3 B帧=4 //REALDATA_VIDEO 有效
	}video;
	struct _audio{
		int iEnable;	//该结构体内的信息是否有效 //REALDATA_AUDIO or REALDATA_PCM 有效
		unsigned int timestampe;	//时间戳信息，单位毫秒(ms)
		int	bits;		//位宽 16bits or 8bits	//REALDATA_PCM 有效
		int samplerate;	//采样率 8000Hz 16000Hz 44100Hz等等	//REALDATA_PCM 有效
		int channels;	//声道数 单声道=1 or 双声道=2	//REALDATA_PCM 有效
		int encodeType;	//音频编码类型 0:未知 AAC=1 MP3=2 G729=10	//REALDATA_AUDIO 有效
	}audio;
}RealDataStreamInfo;

//编码配置信息
//m_nResizeVideoWidth,m_nResizeVideoHeight,25,2000,3,m_nScrVideoWidth,m_nScrVideoHeight);
typedef struct tagEncoder_Config_Info
{
	int nScrVideoWidth;
	int nScrVideoHeight;
	int nResizeVideoWidth;
	int nResizeVideoHeight;
	int nFps;//帧率
	int nMainBitRate;//主视频码率
	int nSingleBitRate;//从视频码率
	int nMainEncLevel;//主编码级别
	int nSingleEncLevel;//从编码级别
	int nMainEncQuality;//主编码视频质量
	int nSingleEncQuality;//从编码视频质量


	int nMainKeyFrame;//主关键帧
	int nSingleKeyFrame;//从关键帧

	int nMainUseQuality;
	int nSingleUseQuality;//是否适用质量参数1--质量,0--码率
	int nRtmpUseQuality;//是否适用质量参数1--质量,0--码率
	int nRtmpVideoWidth;//rtmp
	int nRtmpVideoHeight;
	int nRtmpEncLevel;
	int nRtmpEncQuality;
	int nRtmpBitRate;
	int nRtmpKeyFrame;
	char* sRtmpURL;

	int nMaxVideoFrames;//电影模式最大视频帧数
	int nSingleVideoFrames;//资源模式最大视频帧数

	DWORD nMaxPerFileSize;//最大每个文件大小
	int nResizeType;
}Encoder_Config_Info;

//回调取得实时数据进行处理
// int RealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
// 					RealDataStreamType realDataType, void* realDataInfo);

typedef int (WINAPI *RealDataCallback)(int nDevId, unsigned char *pBuffer, int nBufSize, 
								RealDataStreamType realDataType, void* realDataInfo, void* pMaster);


typedef struct tagThreadCalInfo
{
	RealDataCallback realDataCalFunc;
	void* pMaster;
}ThreadCalInfo;

//获取视频数据导出类接口
class DShowCaptureVideo_Interface
{
public:
	//nIndex：设备编号：音频-1，视频0---N
	virtual void WINAPI SetVideoCaptureData(int nIndex,int iDeviceId,HWND hWnd,int nFrameRate,
		int iWidth,int iHeight,char* strRGBByte,int nRenderType,int nPinType, 
		int nDataType, BOOL bIsThread) = 0;
	virtual int WINAPI CreateCaptureGraph() = 0;
	virtual int WINAPI BulidPrivewGraph() = 0;
	virtual void WINAPI SetCaptureVideoErr(int nError) = 0;
	virtual void WINAPI BegineCaptureThread() = 0;
	virtual HRESULT WINAPI SetupVideoWindow(HWND hWnd=0) = 0;
	virtual void WINAPI ResizeVideoWindow() = 0;
	virtual void WINAPI SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster) = 0;

};

//获取音频数据导出类接口
class DShowCaptureAudio_Interface
{
public:
	virtual void WINAPI SetAudioCaptureData(int iDeviceId,int nChannels,
		int nBytesPerSample,int nSampleRate,int nAudioBufferType,int nPinType, 
		int nDataType, BOOL bIsThread) = 0;
	virtual int  WINAPI CreateCaptureGraph()  = 0;
	virtual int  WINAPI BulidCaptureGraph()   = 0;
	virtual void WINAPI BegineCaptureThread() = 0;
	virtual void WINAPI SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster) = 0;
};

//视频获取定义接口指针类型
typedef	DShowCaptureVideo_Interface*	LPVideoCapturer;	

LPVideoCapturer	APIENTRY Create_VideoCapturer();//创建控制接口指针
void APIENTRY Release_VideoCapturer(LPVideoCapturer lpVideoCapturer);//销毁控制接口指针


//音频获取定义接口指针类型
typedef	DShowCaptureAudio_Interface*	LPAudioCapturer;	

LPAudioCapturer	APIENTRY Create_AudioCapturer();//创建控制接口指针
void APIENTRY Release_AudioCapturer(LPAudioCapturer lpAudioCapturer);//销毁控制接口指针

//void APIENTRY SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster);

#endif//__cplusplus

#endif//DSHOWCAPTUREAUDIOANDVIDEO_H