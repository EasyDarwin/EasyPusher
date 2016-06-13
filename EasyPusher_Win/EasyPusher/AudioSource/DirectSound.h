/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
//#include <windows.h>
//#include <windowsx.h>

#include <mmsystem.h>
#include <dsound.h>
#include <mmreg.h>
//#include <DSUtil.h>
//#include <DXUtil.h>
//#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound")

#include "../../libEasyPlayer/libEasyPlayerAPI.h"
#include "../FFEncoder/FFEncoderAPI.h"


#define		DS_CHANNEL		2
#define		DS_SAMPLERATE	16000
#define		DS_BUFFER_SIZE	8000*2

#define NUM_REC_NOTIFICATIONS  16
#define MAX(a,b)        ( (a) > (b) ? (a) : (b) )

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


typedef struct __DSOUND_OBJ_T
{
	int				flag;
	HANDLE			hThread;

	LPDIRECTSOUNDCAPTURE				m_pDSCapture;								//设备对象指针
	LPDIRECTSOUNDCAPTUREBUFFER			m_pDSBCapture;								//缓冲区对象指针
	LPDIRECTSOUNDNOTIFY					m_pDSNotify;								//用来设置通知的对象接口
	GUID								m_guidCaptureDevice;						//设备id

	DSBPOSITIONNOTIFY					m_aPosNotify[NUM_REC_NOTIFICATIONS+1];		//设置通知标志的数组
	HANDLE								m_hNotificationEvent;						//通知事件
	DWORD								m_dwCaptureBufferSize;						//录音用缓冲区的大小
	DWORD								m_dwNextCaptureOffset;						//偏移位置
	DWORD								m_dwNotifySize;								//通知位置

	unsigned char	*pcmdata;
	int				pcmdata_size;
	int				pcmdata_pos;

	unsigned char	*aacdata;
	int				aacdata_size;

	FFE_HANDLE							ffeAudioHandle;
	MediaSourceCallBack					pCallback;
	void								*pUserPtr;

	void	*pEx;
}DSOUND_OBJ_T;

class CDirectSound
{
public:
	CDirectSound();
	~CDirectSound();

	void	SetCallback(MediaSourceCallBack _callback=NULL, void *userPtr=NULL);

	BOOL	GetAudioInputDevices(HWND hComboWnd = NULL);

	BOOL	InitDirectSound(HWND hComboWnd = NULL);
	BOOL	InitDirectSound(GUID *pDeviceGuid);
	BOOL	FreeDirectSound();
	BOOL	CreateCaptureBuffer(WAVEFORMATEX *pwfxInput);

	BOOL	InitNotifications();
	BOOL	ScanAvailableInputFormats();
	BOOL	Start();
	VOID	Stop();
	VOID    ConvertWaveFormatToString( WAVEFORMATEX* pwfx, TCHAR* strFormatName);
	BOOL	RecordCapturedData();





	DSOUND_OBJ_T						m_DSoundObj;
//	CWaveFile*							g_pWaveFile;

	// Add by SwordTwelve [11/8/2015 Dingshuai]
private:
	//这个是DSound用来显示音频设备选择的Combox控件 (总觉得哪里不对-！)
	HWND m_hComboWnd;
};