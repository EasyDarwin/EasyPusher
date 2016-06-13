/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once

#include <winsock2.h>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "ssqueue.h"

#define		MAX_WAVEHDR_BUFFER_COUNT		(100)
#define		MAX_AUDIO_BUFFER_LENGTH			(96000*(16)*2)	// 96Khz,16bit,Stereo,1sec

// #define		MAX_WAVEHDR_BUFFER_COUNT		(50)
// #define		MAX_AUDIO_BUFFER_LENGTH			(32000*(16)*2)	// 16Khz,16bit,Stereo,1sec

typedef struct __SOUND_OBJ_T
{
	HWAVEOUT	hWaveOut;
	WAVEHDR		*pWaveHdr;
	unsigned int waveHdrNum;
	unsigned int waveHdrWriteIdx;
	HANDLE		hNotify;
	int			initWaveHdr;
	int			packetnum;
	int			packettotal;
	int			framenum;		//音频帧个数
	unsigned int samplerate;
	unsigned int channel;
	unsigned int bitpersec;
	unsigned int datasizePerSec;
	unsigned int sleeptimes;
	unsigned int totaltime;
	int			flag;
	HANDLE		hThread;

	int			clearFlag;
	void		*pEx;
}SOUND_OBJ_T;


class CSoundPlayer
{
public:
	CSoundPlayer(void);
	~CSoundPlayer(void);

	int Open(WAVEFORMATEX _tOutWFX);
	void Close();
	void ResetData();		//清空实际数据

	void Clear();		//置清空标志

	int	Write(char *pbuf, int bufsize);//, unsigned int _timestamp);


	SOUND_OBJ_T		soundObj;
	CRITICAL_SECTION	crit;
};

