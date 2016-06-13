/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "SoundPlayer.h"
#include "trace.h"
#include "vstime.h"
#include <time.h>

DWORD WINAPI __SOUND_PLAY_THREAD(LPVOID lpParam);


CSoundPlayer::CSoundPlayer(void)
{
	memset(&soundObj, 0x00, sizeof(SOUND_OBJ_T));


	soundObj.waveHdrNum = MAX_WAVEHDR_BUFFER_COUNT;
	soundObj.pWaveHdr = new WAVEHDR[soundObj.waveHdrNum];
	memset(soundObj.pWaveHdr, 0x00, sizeof(WAVEHDR) * soundObj.waveHdrNum);

	for (unsigned int i=0; i<soundObj.waveHdrNum; i++)
	{
		soundObj.pWaveHdr[i].lpData = (LPSTR)new BYTE[MAX_AUDIO_BUFFER_LENGTH];
		soundObj.pWaveHdr[i].dwBufferLength = MAX_AUDIO_BUFFER_LENGTH;
	}

	soundObj.hNotify = CreateEvent( NULL, FALSE, FALSE, 0 );

	InitializeCriticalSection(&crit);
}


CSoundPlayer::~CSoundPlayer(void)
{
	if (NULL != soundObj.hThread)
	{
		if (soundObj.flag != 0x00)	soundObj.flag = 0x03;
		while (soundObj.flag!=0x00)	{Sleep(100);}

		CloseHandle(soundObj.hThread);
		soundObj.hThread = NULL;
	}

	if (NULL != soundObj.pWaveHdr)
	{
		for (unsigned int i=0; i<soundObj.waveHdrNum; i++)
		{
			if (NULL!=soundObj.pWaveHdr[i].lpData)
			{
				delete []soundObj.pWaveHdr[i].lpData;
				soundObj.pWaveHdr[i].lpData = NULL;
			}
			soundObj.pWaveHdr[i].dwBufferLength = 0;
		}
		delete []soundObj.pWaveHdr;
		soundObj.pWaveHdr = NULL;
	}

	if( soundObj.hNotify ) CloseHandle( soundObj.hNotify );
	soundObj.hNotify = NULL;

	DeleteCriticalSection(&crit);
}

int CSoundPlayer::Open(WAVEFORMATEX _tOutWFX)
{
	DWORD dwFlag = 0;
	HRESULT	hr = NOERROR;
	MMRESULT	mmhr;

	if (NULL != soundObj.hWaveOut)	return -1;

	//if (NULL==soundObj.hNotify)	soundObj.hNotify = CreateEvent( NULL, FALSE, FALSE, 0 );

#if 0
	// Query WaveOut Device for _tOutWFX
	dwFlag = WAVE_FORMAT_DIRECT | WAVE_FORMAT_QUERY; //WAVE_MAPPED
	dwFlag |= CALLBACK_NULL;
	mmhr = waveOutOpen( &soundObj.hWaveOut, WAVE_MAPPER, &_tOutWFX, NULL, 0, dwFlag );
	if( MMSYSERR_NOERROR != mmhr ) {
		_TRACE("waveOutOpen Query Fail ret:%d \n", mmhr );
		return -3;
	}
#endif
	// Open WaveOut Device for _tOutWFX
	dwFlag = WAVE_FORMAT_DIRECT;
	dwFlag |= CALLBACK_EVENT;
	mmhr = waveOutOpen( &soundObj.hWaveOut, WAVE_MAPPER, &_tOutWFX, 
						//(DWORD)NULL, (DWORD_PTR)0, dwFlag );
						(DWORD)soundObj.hNotify, (DWORD_PTR)0, dwFlag );
	if( MMSYSERR_NOERROR != mmhr ) {
		_TRACE("waveOutOpen Fail ret:%d \n", mmhr );
		return -4;
	}

	for( unsigned int i=0; i<soundObj.waveHdrNum; i++ ) 
	{
		soundObj.pWaveHdr[i].dwFlags = 0;
		soundObj.pWaveHdr[i].dwBufferLength = MAX_AUDIO_BUFFER_LENGTH;
		mmhr = waveOutPrepareHeader( soundObj.hWaveOut, &soundObj.pWaveHdr[i], sizeof(WAVEHDR) );
		soundObj.pWaveHdr[i].dwBufferLength = 0;
	}
	soundObj.initWaveHdr = 0x01;

	soundObj.samplerate = _tOutWFX.nSamplesPerSec;
	soundObj.channel    = _tOutWFX.nChannels;
	soundObj.bitpersec  = _tOutWFX.wBitsPerSample;

	ResetData();

	if (NULL == soundObj.hThread)
	{
		soundObj.flag = 0x01;
		soundObj.pEx  = this;
		soundObj.hThread = CreateThread(NULL, 0, __SOUND_PLAY_THREAD, &soundObj, 0, NULL);
		while (soundObj.flag != 0x02 && soundObj.flag != 0x00)	{Sleep(100);}
	}
	soundObj.waveHdrWriteIdx = 0;



	return 0;
}

void CSoundPlayer::Close()
{
	if (NULL != soundObj.pWaveHdr && soundObj.initWaveHdr == 0x01)
	{
		MMRESULT	mmhr;
		for( unsigned int i=0; i<soundObj.waveHdrNum; i++ ) 
		{
			//if (soundObj.pWaveHdr[i].dwBufferLength < 1)		continue;

			soundObj.pWaveHdr[i].dwFlags = 0;
			mmhr = waveOutUnprepareHeader( soundObj.hWaveOut, &soundObj.pWaveHdr[i], sizeof(WAVEHDR) );
			soundObj.pWaveHdr[i].dwBufferLength = 0;
		}
	}
	soundObj.initWaveHdr = 0x00;

	if (NULL != soundObj.hWaveOut)
	{
		waveOutClose(soundObj.hWaveOut);
		soundObj.hWaveOut = NULL;
	}
	//if( soundObj.hNotify ) CloseHandle( soundObj.hNotify );
	//soundObj.hNotify = NULL;
}


int	CSoundPlayer::Write(char *pbuf, int bufsize)//, unsigned int _timestamp)
{
	if (NULL == soundObj.pWaveHdr)	return -1;

	int idx = soundObj.waveHdrWriteIdx;

	if (soundObj.clearFlag == 0x01)		return -2;		//正在清空中...

	while ( (NULL!=soundObj.pWaveHdr) && (soundObj.pWaveHdr[idx].dwBufferLength > 0) )
	{
		Sleep(2);
	}
	if (NULL == soundObj.pWaveHdr)	return -1;


	memcpy(soundObj.pWaveHdr[idx].lpData, pbuf, bufsize);
	soundObj.pWaveHdr[idx].dwBufferLength = bufsize;

	soundObj.waveHdrWriteIdx++;
	if (soundObj.waveHdrWriteIdx>=soundObj.waveHdrNum)
	{
		soundObj.waveHdrWriteIdx = 0;
	}

	unsigned int uiTime = (unsigned int)time(NULL);

	/*
	if (uiTime > soundObj.totaltime+1)
	{
		soundObj.packettotal = soundObj.packetnum/2;
		soundObj.packetnum = 1;
		soundObj.totaltime = uiTime;
	}
	else
	{
		soundObj.packetnum ++;
	}
	*/

#if 1
	float f = soundObj.samplerate * soundObj.bitpersec * soundObj.channel / 8.0f / 1000.0f;
	unsigned int uiSleepTimesAvg = (unsigned int) ( (float)bufsize / f);
	unsigned int uiSleepTimeMin  = (unsigned int) ( (float)( (float)uiSleepTimesAvg)*0.95f);
#endif

	//if (soundObj.extTimestamp == 0x00)
	{
		if (soundObj.sleeptimes == 0)	soundObj.sleeptimes = 1000/43;
		if (soundObj.totaltime == uiTime)
		{
			soundObj.packetnum ++;
		}
		else
		{
			soundObj.packettotal = soundObj.packetnum;
			soundObj.packetnum = 1;

			soundObj.totaltime = uiTime;
			if (soundObj.packettotal == 0)	soundObj.packettotal = 43;
			soundObj.sleeptimes = 1000 / soundObj.packettotal;

			if (soundObj.sleeptimes > uiSleepTimesAvg)
			{
				soundObj.sleeptimes = uiSleepTimesAvg;
			}
			else if (soundObj.sleeptimes < uiSleepTimeMin)
			{
#ifdef _DEBUG1
				_TRACE("MIN: %d\n", uiSleepTimeMin);
#endif
				soundObj.sleeptimes = uiSleepTimeMin;
			}
		}
		// 这里写死后将导致44100Hz的声音播放不正常 [12/30/2015 Dingshuai]
		//soundObj.sleeptimes = 64;

	}
	
	soundObj.pWaveHdr[idx].dwUser = soundObj.sleeptimes;

	soundObj.framenum ++;
#ifdef _DEBUG1
	_TRACE("Write sound data.. %d   frameNum:%d   sleeptimes: %d\n", soundObj.waveHdrWriteIdx, soundObj.framenum, soundObj.sleeptimes);
#endif
	return 0;
}

void CSoundPlayer::Clear()
{
	soundObj.clearFlag = 0x01;
	soundObj.waveHdrWriteIdx = 0x00;
}

void CSoundPlayer::ResetData()
{
	for (unsigned int i=0; i<soundObj.waveHdrNum; i++)
	{
		memset(soundObj.pWaveHdr[i].lpData, 0x00, MAX_AUDIO_BUFFER_LENGTH);
		soundObj.pWaveHdr[i].dwBufferLength = 0;
		soundObj.framenum = 0;
	}
	soundObj.waveHdrWriteIdx = 0x00;
	soundObj.clearFlag = 0x00;
}

DWORD WINAPI __SOUND_PLAY_THREAD(LPVOID lpParam)
{
	SOUND_OBJ_T *pSoundObj = (SOUND_OBJ_T *)lpParam;
	if (NULL == pSoundObj)		return -1;

	CSoundPlayer *pThis = (CSoundPlayer *)pSoundObj->pEx;

	pSoundObj->flag = 0x02;

	unsigned int playIdx = 0;
	MMRESULT	mmhr;

	while (1)
	{
		if (pSoundObj->flag == 0x03)		break;

		if (NULL == pSoundObj->pWaveHdr)
		{
			Sleep(100);
			continue;
		}

		if (pSoundObj->clearFlag == 0x01)
		{
			pThis->ResetData();
			playIdx = 0;
		}

		if (pSoundObj->pWaveHdr[playIdx].dwBufferLength < 1)
		{
			Sleep(1);
			continue;
		}

		EnterCriticalSection(&pThis->crit);
		mmhr = waveOutWrite( pSoundObj->hWaveOut, &pSoundObj->pWaveHdr[playIdx], sizeof(WAVEHDR) );
		LeaveCriticalSection(&pThis->crit);
		if( MMSYSERR_NOERROR != mmhr ) 
		{
			_TRACE("waveOutWrite Fail ret:%d \n", mmhr );
			Sleep(10);
		}
#if 0
		//WaitForSingleObject(pSoundObj->hNotify,INFINITE);
		/*
		DWORD dwVal = 0;
		while((dwVal = WaitForSingleObject(pSoundObj->hNotify, 300))==WAIT_TIMEOUT)
		{
			if (pSoundObj->flag == 0x03)		break;
			Sleep(1);
		}
		*/
			_VS_BEGIN_TIME_PERIOD(1);
			
			Sleep(120);
			_VS_END_TIME_PERIOD(1);
		/*
		if( WHDR_DONE != (pSoundObj->pWaveHdr[playIdx].dwFlags & WHDR_DONE) ) 
		{
			Sleep(0);
		}
		*/
//#else
		int iOneUsageTimes = 1000 / 43;
		int msecs = iOneUsageTimes;
		if (pSoundObj->packettotal > 0)
		{
			iOneUsageTimes = 1000 / pSoundObj->packettotal;

			msecs = iOneUsageTimes;
/*
			int iCache = 1;
			if (pSoundObj->framenum<iCache)
			{
				int ii = ((iOneUsageTimes * (iCache-pSoundObj->framenum))/pSoundObj->packettotal);
				msecs += ii;
			}
			else if (iOneUsageTimes>iCache)
			{
				int ii = ((iOneUsageTimes * (pSoundObj->framenum-iCache))/pSoundObj->packettotal);
				msecs -= ii;
			}
*/
		}



		//_TRACE("Get sound data.. %d\ttimes:%d\n", playIdx, msecs);
		//if (pSoundObj->framenum > pSoundObj->waveHdrNum/2)	msecs /= 2;
		if (pSoundObj->framenum > ((float)pSoundObj->waveHdrNum*0.7))
		{
			msecs = (int)((float)msecs * (float)0.7f);
		}
		if (msecs > 0)
		{
			_VS_BEGIN_TIME_PERIOD(1);
			_TRACE("Sleep: %d\n", msecs);
			Sleep(msecs);
			_VS_END_TIME_PERIOD(1);
		}
#endif

		int msecs = pSoundObj->pWaveHdr[playIdx].dwUser;//    pSoundObj->sleeptimes;
		if (msecs > 500)	msecs = 500;
		if ( (pSoundObj->framenum > ((float)pSoundObj->waveHdrNum*0.7)) && (msecs>0))
		{
			msecs = (int)((float)msecs * (float)0.7f);
		}
		if (msecs > 0)
		{
			_VS_BEGIN_TIME_PERIOD(1);
#ifdef _DEBUG1
			_TRACE("Sleep: %d    datasize:%d\n", msecs, pSoundObj->pWaveHdr[playIdx].dwBufferLength);
#endif
			Sleep(msecs);
			_VS_END_TIME_PERIOD(1);
		}

		pSoundObj->pWaveHdr[playIdx].dwBufferLength = 0;
		playIdx ++;
		if (playIdx >= pSoundObj->waveHdrNum)		playIdx = 0;

		pSoundObj->framenum --;
	}
	pSoundObj->flag = 0x00;

	return 0;
}
