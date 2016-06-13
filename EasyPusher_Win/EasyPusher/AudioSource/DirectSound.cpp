/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/

#include "stdafx.h"
#include "DirectSound.h"
#include <time.h>

#define AUDIO_SAMPLE_RATE	16000
#define WAVE_FORMAT_TAG		WAVE_FORMAT_PCM
#define BITS_PER_SAMPLE		16
#define NUM_AUDIO_CHANNELS	1
#define AUDIO_LENGTH_IN_SEC	10

typedef struct {
	long	chunkID;
	long	chunkSize;
	WAVEFORMATEX	waveformat;
} FORMATCHUNK;

typedef struct {
	long	chunkID;
	long	chunkSize;
	long	sampleLength;
} FACTCHUNK;

typedef struct {
	long	chunkID;
	long	chunkSize;
} DATACHUNK;


BOOL	g_bRecording = FALSE;
//________________________________________________________________________________
DWORD WINAPI ThreadRecord(LPVOID lpParam)
{
	DSOUND_OBJ_T *pDSoundObj = (DSOUND_OBJ_T *)lpParam;
	CDirectSound	*pThis = (CDirectSound *)pDSoundObj->pEx;

	pDSoundObj->flag = 0x02;

	//FFE_Init(&pDSoundObj->ffeAudioHandle);
	//FFE_SetAudioEncodeParam(pDSoundObj->ffeAudioHandle, ENCODER_AAC, DS_CHANNEL, DS_SAMPLERATE, 256000);

	AAC_Init(&pDSoundObj->ffeAudioHandle, DS_SAMPLERATE, 2);
	

	DWORD dwResult = 0;
	g_bRecording	=	TRUE;

	while (1)
	{
		if (pDSoundObj->flag == 0x03)		break;

		//dwResult = WaitForMultipleObjects(1, &pThis->m_hNotificationEvent, FALSE, INFINITE, QS_ALLEVENTS);
		dwResult = WaitForMultipleObjects(1, &pDSoundObj->m_hNotificationEvent, FALSE, INFINITE);
		switch (dwResult)
		{
		case WAIT_OBJECT_0 + 0:
			{
				if (! pThis->RecordCapturedData())
				{
					break;
				}
			}
			break;
		case WAIT_OBJECT_0 + 1:
			{
			}
			break;
		default:
			break;
		}
	}

	//FFE_Deinit(&pDSoundObj->ffeAudioHandle);
	AAC_Deinit(&pDSoundObj->ffeAudioHandle);

	pDSoundObj->flag = 0x00;

	return 0;
}

















//________________________________________________________________________________
CDirectSound::CDirectSound()
{
	memset(&m_DSoundObj, 0x00, sizeof(DSOUND_OBJ_T));
	m_DSoundObj.pcmdata_size = DS_BUFFER_SIZE*2;
	m_DSoundObj.pcmdata = new unsigned char[m_DSoundObj.pcmdata_size];
	m_DSoundObj.pcmdata_pos = 0;

	m_DSoundObj.aacdata_size = DS_BUFFER_SIZE*2;
	m_DSoundObj.aacdata = new unsigned char[m_DSoundObj.aacdata_size];

	CoInitialize(NULL);
}

CDirectSound::~CDirectSound()
{
	
	Stop();

	if (NULL != m_DSoundObj.pcmdata)
	{
		delete []m_DSoundObj.pcmdata;
		m_DSoundObj.pcmdata = NULL;
	}
	if (NULL != m_DSoundObj.aacdata)
	{
		delete []m_DSoundObj.aacdata;
		m_DSoundObj.aacdata = NULL;
	}

	FreeDirectSound();




	CoUninitialize();
}

void CDirectSound::SetCallback(MediaSourceCallBack _callback, void *_userPtr)
{
	m_DSoundObj.pCallback	=	_callback;
	m_DSoundObj.pUserPtr	=	_userPtr;
}

BOOL CALLBACK	DSoundEnumCallback(GUID *pGUID, LPSTR strDesc, LPSTR strDrvName, VOID *pContext)
{
#if 1
	static GUID AudioDriverGUIDs[20];
	static DWORD dwAudioDriverIndex = 0;

	GUID *pTemp = NULL;

	if (pGUID)
	{
		if (dwAudioDriverIndex >= 20)	return TRUE;

		pTemp = &AudioDriverGUIDs[dwAudioDriverIndex++];
		memcpy(pTemp, pGUID, sizeof(GUID));
	}

	HWND hSoundDeviceCombo = (HWND)pContext;

	if (pTemp)
	{
		//Add the string to the combo box
		SendMessage(hSoundDeviceCombo, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strDesc);

		//Get the index of the string in the combo box
		INT nIndex = (INT)SendMessage(hSoundDeviceCombo, CB_FINDSTRING, 0, (LPARAM)(LPCTSTR)strDesc);

		//Set the item data to pointer to the static guid stored in AudioDriverGUIDs
		SendMessage(hSoundDeviceCombo, CB_SETITEMDATA, nIndex, (LPARAM)pTemp);	
	}


	return TRUE;
#else
	HWND hCombo = (HWND)lpContext;

	LPGUID lpTemp = NULL;
	if (NULL != lpGUID)
	{
		if ( (lpTemp = (LPGUID)malloc(sizeof(GUID))) == NULL)
		{
			return TRUE;
		}
		memcpy(lpTemp, lpGUID, sizeof(GUID));
	}
	//将设备添加到CComboBox
	ComboBox_AddString(hCombo, lpszDesc);
	ComboBox_SetItemData(hCombo, ComboBox_FindString(hCombo, 0, lpszDesc), lpTemp);
	free(lpTemp);

	return TRUE;
#endif
}

BOOL CDirectSound::GetAudioInputDevices(HWND hComboWnd)
{
	m_hComboWnd = hComboWnd;
    // Enumerate the capture devices and place them in the combo box
    DirectSoundCaptureEnumerate( (LPDSENUMCALLBACK)DSoundEnumCallback,
                                 (VOID*)hComboWnd );

    // Select the first device in the combo box
    SendMessage( hComboWnd, CB_SETCURSEL, 0, 0 );

	return TRUE;
}
BOOL CDirectSound::InitDirectSound(HWND hComboWnd)
{
	if (hComboWnd != NULL)
	{
		m_hComboWnd = hComboWnd;
	}
	INT nCaptureIndex = (INT)SendMessage(m_hComboWnd, CB_GETCURSEL, 0, 0);
	GUID *pCaptureGUID = (GUID *)SendMessage( m_hComboWnd, CB_GETITEMDATA, nCaptureIndex, 0);

	if (NULL != pCaptureGUID)
	{
		m_DSoundObj.m_guidCaptureDevice = *pCaptureGUID;

		return InitDirectSound(&m_DSoundObj.m_guidCaptureDevice);
	}

	return FALSE;
}
BOOL CDirectSound::InitDirectSound(GUID *pDeviceGuid)
{
    ZeroMemory( &m_DSoundObj.m_aPosNotify, sizeof(DSBPOSITIONNOTIFY) * 
                               (NUM_REC_NOTIFICATIONS + 1) );
    m_DSoundObj.m_dwCaptureBufferSize = 0;
    m_DSoundObj.m_dwNotifySize        = 0;

	if (FAILED(DirectSoundCaptureCreate(pDeviceGuid, &m_DSoundObj.m_pDSCapture, NULL) ))
	{
		if (FAILED(DirectSoundCaptureCreate(&DSDEVID_DefaultCapture, &m_DSoundObj.m_pDSCapture, NULL)))
		{
			return FALSE;
		}
	}

	return TRUE;
}
BOOL CDirectSound::FreeDirectSound()
{
    // Release DirectSound interfaces
    SAFE_RELEASE( m_DSoundObj.m_pDSNotify );
    SAFE_RELEASE( m_DSoundObj.m_pDSBCapture );
    SAFE_RELEASE( m_DSoundObj.m_pDSCapture ); 

	return TRUE;
}

BOOL CDirectSound::CreateCaptureBuffer(WAVEFORMATEX *pwfxInput)
{
	DSCBUFFERDESC		dscbd;
    SAFE_RELEASE( m_DSoundObj.m_pDSNotify );
    SAFE_RELEASE( m_DSoundObj.m_pDSBCapture );

	//Set the notification size
	m_DSoundObj.m_dwNotifySize = MAX(1024, pwfxInput->nAvgBytesPerSec / 8);
	m_DSoundObj.m_dwNotifySize -= m_DSoundObj.m_dwNotifySize % pwfxInput->nBlockAlign;

	//Set the buffer sizes
	m_DSoundObj.m_dwCaptureBufferSize = m_DSoundObj.m_dwNotifySize * NUM_REC_NOTIFICATIONS;
    SAFE_RELEASE( m_DSoundObj.m_pDSNotify );
    SAFE_RELEASE( m_DSoundObj.m_pDSBCapture );

	//Create the capture buffer
	ZeroMemory(&dscbd, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);
	dscbd.dwBufferBytes = m_DSoundObj.m_dwCaptureBufferSize;
	dscbd.lpwfxFormat = pwfxInput;	//Set the format during creatation

	if ((!m_DSoundObj.m_pDSCapture)||FAILED(m_DSoundObj.m_pDSCapture->CreateCaptureBuffer(&dscbd, &m_DSoundObj.m_pDSBCapture, NULL)))
	{
		return FALSE;
	}

	m_DSoundObj.m_dwNextCaptureOffset = 0;

	//return TRUE;
	return InitNotifications();
}
BOOL CDirectSound::InitNotifications()
{
	if (NULL == m_DSoundObj.m_pDSBCapture)	return FALSE;

	m_DSoundObj.m_hNotificationEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Create a notification event, for when the sound stops playing
	if (FAILED(m_DSoundObj.m_pDSBCapture->QueryInterface(IID_IDirectSoundNotify, (VOID **)&m_DSoundObj.m_pDSNotify)))
	{
		return FALSE;
	}

	//setup the notification positions
	for (INT i = 0; i < NUM_REC_NOTIFICATIONS; i ++)
	{
		m_DSoundObj.m_aPosNotify[i].dwOffset = (m_DSoundObj.m_dwNotifySize * i) + m_DSoundObj.m_dwNotifySize - 1;
		m_DSoundObj.m_aPosNotify[i].hEventNotify = m_DSoundObj.m_hNotificationEvent;
	}

	if (FAILED(m_DSoundObj.m_pDSNotify->SetNotificationPositions(NUM_REC_NOTIFICATIONS, m_DSoundObj.m_aPosNotify)))
	{
		return FALSE;
	}

	//return ScanAvailableInputFormats();
	return TRUE;
}
BOOL CDirectSound::ScanAvailableInputFormats()
{
	WAVEFORMATEX	wfx;
	DSCBUFFERDESC	dscbd;
	LPDIRECTSOUNDCAPTUREBUFFER pDSCaptureBuffer = NULL;

	ZeroMemory(&wfx, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	ZeroMemory(&dscbd, sizeof(dscbd));
	dscbd.dwSize = sizeof(dscbd);

//	for (INT iIndex = 0; iIndex < 20; iIndex ++)
	{
	}
	wfx.nSamplesPerSec = DS_SAMPLERATE;	//16000   8000
	wfx.wBitsPerSample = 16;//DS_SAMPLERATE/1000;	//16	  8
	wfx.nChannels      = DS_CHANNEL;		//1		  2
	wfx.nBlockAlign  = wfx.nChannels * (wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	
	//wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nChannels * wfx.wBitsPerSample / 8;
	//wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;

	//formatchunk.waveformat.nAvgBytesPerSec = formatchunk.waveformat.nSamplesPerSec * formatchunk.waveformat.nChannels * formatchunk.waveformat.wBitsPerSample / 8;
	//formatchunk.waveformat.nBlockAlign = formatchunk.waveformat.nChannels * formatchunk.waveformat.wBitsPerSample / 8;


	dscbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dscbd.lpwfxFormat   = &wfx;

	if (FAILED(m_DSoundObj.m_pDSCapture->CreateCaptureBuffer(&dscbd, &pDSCaptureBuffer, NULL)))
	{
		SAFE_RELEASE( pDSCaptureBuffer );
		return FALSE;
	}
	SAFE_RELEASE( pDSCaptureBuffer );



	return TRUE;
}
BOOL CDirectSound::Start()
{
	if (m_DSoundObj.flag == 0x00)
	{

		WAVEFORMATEX	wfxInput;
		TCHAR			strInputFormat[255];

		ZeroMemory(&wfxInput, sizeof(wfxInput));
		if ((!m_DSoundObj.m_pDSBCapture)||FAILED(m_DSoundObj.m_pDSBCapture->GetFormat(&wfxInput, sizeof(wfxInput), NULL)))
		{
			return FALSE;
		}

		ZeroMemory(strInputFormat, sizeof(strInputFormat));
		ConvertWaveFormatToString( &wfxInput, strInputFormat );

		TRACE("Input: %s\n", strInputFormat);

		m_DSoundObj.flag = 0x01;
		m_DSoundObj.pEx = this;
		m_DSoundObj.hThread = CreateThread(NULL, 0, ThreadRecord, this, 0, NULL);
		while (m_DSoundObj.flag != 0x02 && m_DSoundObj.flag != 0x00)	{Sleep(200);}

		if (FAILED(m_DSoundObj.m_pDSBCapture->Start(DSCBSTART_LOOPING)))		return FALSE;

		return TRUE;
	}

	return FALSE;
}
VOID CDirectSound::Stop()
{
	if (NULL != m_DSoundObj.m_pDSBCapture)
	{
		m_DSoundObj.m_pDSBCapture->Stop();
	}
	if (m_DSoundObj.flag == 0x02)		m_DSoundObj.flag = 0x03;
	if (NULL != m_DSoundObj.m_hNotificationEvent)
	{
		SetEvent(m_DSoundObj.m_hNotificationEvent);
		CloseHandle(m_DSoundObj.m_hNotificationEvent);
		m_DSoundObj.m_hNotificationEvent = NULL;
	}
	while (m_DSoundObj.flag != 0x00)	{Sleep(200);}
	if (NULL != m_DSoundObj.hThread)
	{
		CloseHandle(m_DSoundObj.hThread);
		m_DSoundObj.hThread = NULL;
	}
}

VOID CDirectSound::ConvertWaveFormatToString( WAVEFORMATEX* pwfx, TCHAR* strFormatName)
{
    wsprintf( strFormatName, 
              TEXT("%u Hz, %u-bit %s"), 
              pwfx->nSamplesPerSec, 
              pwfx->wBitsPerSample, 
              ( pwfx->nChannels == 1 ) ? TEXT("Mono") : TEXT("Stereo") );
}
#if 0
VOID WriteWAV(char *filename, BYTE *pBuff, long size)
{
	FILE *fAudio = fopen(filename, "wb");

	FORMATCHUNK formatchunk;
	FACTCHUNK factchunk;
	DATACHUNK datachunk;
	int riffChunkSize;
	int numSamples;

	formatchunk.chunkID = 0x20746d66;	// "fmt "
	formatchunk.chunkSize = sizeof(WAVEFORMATEX);
	formatchunk.waveformat.wFormatTag = WAVE_FORMAT_TAG;
	formatchunk.waveformat.nChannels = NUM_AUDIO_CHANNELS;
	formatchunk.waveformat.nSamplesPerSec = AUDIO_SAMPLE_RATE;
	formatchunk.waveformat.wBitsPerSample = BITS_PER_SAMPLE;
	formatchunk.waveformat.nAvgBytesPerSec = formatchunk.waveformat.nSamplesPerSec * formatchunk.waveformat.nChannels * formatchunk.waveformat.wBitsPerSample / 8;
	formatchunk.waveformat.nBlockAlign = formatchunk.waveformat.nChannels * formatchunk.waveformat.wBitsPerSample / 8;
	formatchunk.waveformat.cbSize = 0;

	numSamples = size / formatchunk.waveformat.nBlockAlign;

	factchunk.chunkID = 0x74636166;	// "fact"
	factchunk.chunkSize = 4;
	factchunk.sampleLength = formatchunk.waveformat.nChannels * numSamples;

	datachunk.chunkID = 0x61746164;	// "data"
	datachunk.chunkSize = (formatchunk.waveformat.wBitsPerSample / 8) * formatchunk.waveformat.nChannels * numSamples;

 	riffChunkSize = 4 + sizeof(formatchunk) + sizeof(factchunk) + 8 + datachunk.chunkSize;

	fwrite("RIFF", strlen("RIFF"), 1, fAudio);
	fwrite(&riffChunkSize, sizeof(riffChunkSize), 1, fAudio);
	fwrite("WAVE", strlen("WAVE"), 1, fAudio);
	fwrite(&formatchunk, sizeof(formatchunk), 1, fAudio);
	fwrite(&factchunk, sizeof(factchunk), 1, fAudio);
	fwrite(&datachunk, sizeof(datachunk), 1, fAudio);
	fwrite(pBuff, size, 1, fAudio);
	fclose(fAudio);
	fAudio = NULL;
}
#endif

#ifdef _WIN32
unsigned int __gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst    = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

//int nTally = 0;
BOOL CDirectSound::RecordCapturedData()
{
	VOID	*pbCaptureData		=	NULL;
	DWORD	dwCaptureLength;
	VOID	*pbCaptureData2		=	NULL;
	DWORD	dwCaptureLength2;
	//UINT	dwDataWrote;
	DWORD	dwReadPos;
	DWORD	dwCapturePos;
	LONG	lLockSize;


	if (NULL == m_DSoundObj.m_pDSBCapture)		return FALSE;

	if (FAILED(m_DSoundObj.m_pDSBCapture->GetCurrentPosition(&dwCapturePos, &dwReadPos)))		return FALSE;

	lLockSize = dwReadPos - m_DSoundObj.m_dwNextCaptureOffset;
	if (lLockSize < 0)
	{
		lLockSize += m_DSoundObj.m_dwCaptureBufferSize;
	}
	lLockSize -= (lLockSize % m_DSoundObj.m_dwNotifySize);
	if (lLockSize == 0)		return FALSE;

	if (FAILED(m_DSoundObj.m_pDSBCapture->Lock(m_DSoundObj.m_dwNextCaptureOffset, lLockSize, 
		&pbCaptureData, &dwCaptureLength,
		&pbCaptureData2, &dwCaptureLength2, 0L)))
	{
		return FALSE;
	}

	//wav data
	//dwCaptureLength		pbCaptureData
	//TRACE("get wav data.\n");

	if (m_DSoundObj.pcmdata_pos + dwCaptureLength < DS_BUFFER_SIZE)
	{
		memcpy(m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos, pbCaptureData, dwCaptureLength);
		m_DSoundObj.pcmdata_pos += dwCaptureLength;
	}
	else
	{
/*
		char szTmp[32];
		memset(szTmp, 0, sizeof(szTmp));
		sprintf(szTmp, "C:\\test\\wav\\%d.wav", nTally++);
		WriteWAV(szTmp, pDSoundBuff, lAudSize);
*/
		memcpy(m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos, pbCaptureData, dwCaptureLength);
		m_DSoundObj.pcmdata_pos += dwCaptureLength;


		int max_packet_size = 4096;
		int iPacketNum = m_DSoundObj.pcmdata_pos / max_packet_size;
		if (m_DSoundObj.pcmdata_pos % max_packet_size != 0x00)	iPacketNum++;

		int ret = -1;
		int packetSize = max_packet_size;
		for (int iPacketNo=0; iPacketNo<iPacketNum; iPacketNo++)
		{
			if (NULL != m_DSoundObj.pCallback)
			{
				if (iPacketNo + 1 == iPacketNum)// && ((m_DSoundObj.pcmdata_pos - packetSize*iPacketNo)<max_packet_size ))
				{
					packetSize = (m_DSoundObj.pcmdata_pos - packetSize*iPacketNo);
					memmove(m_DSoundObj.pcmdata, m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos-packetSize, packetSize);
					m_DSoundObj.pcmdata_pos = packetSize;
					ret = 0;
					break;
				}

				int enc_size = 0;
				//int ret = FFE_EncodeAudio(m_DSoundObj.ffeAudioHandle, (unsigned short*)(m_DSoundObj.pcmdata+(max_packet_size*iPacketNo)), packetSize, (unsigned char*)m_DSoundObj.aacdata, &enc_size);

				unsigned char *pAACbuf= NULL;
				int ret = AAC_Encode(m_DSoundObj.ffeAudioHandle, (int*)(m_DSoundObj.pcmdata+(max_packet_size*iPacketNo)), packetSize, &pAACbuf, &enc_size);
				if (ret == 0x00 && enc_size>0)
				{
#if 0
					static FILE *fAAC = NULL;
					if (NULL == fAAC)	fAAC = fopen("1234.aac", "wb");
					if (NULL != fAAC)	fwrite(pAACbuf, 1, enc_size, fAAC);
#endif			

					RTSP_FRAME_INFO	frameinfo;
					memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
					frameinfo.codec = EASY_SDK_AUDIO_CODEC_AAC;
					frameinfo.length = enc_size;
					frameinfo.sample_rate	=	DS_SAMPLERATE;
					frameinfo.channels = DS_CHANNEL;

					static unsigned int uiAudioPts = 0;

					static struct timeval tvLast = {0,0};
					struct timeval tv;
					__gettimeofday(&tv, NULL);
					frameinfo.timestamp_sec = tv.tv_sec;
					frameinfo.timestamp_usec = tv.tv_usec;

					static int iPacketNum = 0, iTotalPacketNum;
					if (tvLast.tv_sec==0 && tvLast.tv_usec==0)	memcpy(&tvLast, &tv, sizeof(struct timeval));
					else
					{
						if (tv.tv_sec == tvLast.tv_sec)
						{
							iPacketNum ++;
						}
						else
						{
							iTotalPacketNum = iPacketNum;
							iPacketNum = 1;

							memcpy(&tvLast, &tv, sizeof(struct timeval));
						}
					}

// 					frameinfo.timestamp_sec = uiAudioPts / 1000;
// 					frameinfo.timestamp_usec = (uiAudioPts%1000) * 1000;
// 
// 					uiAudioPts += 64;
// 
// 					//TRACE("AUDIO Packet Num: %d\n", iTotalPacketNum);
					int uiPts = tv.tv_sec*1000+tv.tv_usec/1000;
					TRACE("Audio PTS: %d\n", uiPts);

					//m_DSoundObj.pCallback(0, (int *)m_DSoundObj.pUserPtr, EASY_SDK_AUDIO_FRAME_FLAG, (char *)m_DSoundObj.aacdata, &frameinfo);
					m_DSoundObj.pCallback(0, (int *)m_DSoundObj.pUserPtr, EASY_SDK_AUDIO_FRAME_FLAG, (char *)pAACbuf, &frameinfo);
				}
			}

		}


		//if (NULL != g_pYDFTestDlg)
		{
			//g_pYDFTestDlg->rtspClient[0].SendAudioData((unsigned char *)pDSoundBuff, lAudSize, lAudSize, true);
		}
		if (ret == -1)
		m_DSoundObj.pcmdata_pos	=	0;

		if (m_DSoundObj.pcmdata_pos > 0)
		{
			//TRACE("m_DSoundObj.pcmdata_pos: %d\n", m_DSoundObj.pcmdata_pos);
		}

		//memcpy(m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos, pbCaptureData, dwCaptureLength);
	}

	m_DSoundObj.m_dwNextCaptureOffset += dwCaptureLength;
	m_DSoundObj.m_dwNextCaptureOffset %= m_DSoundObj.m_dwCaptureBufferSize;		//Circular buffer

	if (NULL != pbCaptureData2)
	{
		//wav data
		//dwCaptureLength2		pbCaptureData2

		if (m_DSoundObj.pcmdata_pos + dwCaptureLength2 <= DS_BUFFER_SIZE)
		{
			memcpy(m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos, pbCaptureData2, dwCaptureLength2);
			m_DSoundObj.pcmdata_pos += dwCaptureLength2;
		}
		else
		{
			//if (NULL != g_pYDFTestDlg)
			{
				//g_pYDFTestDlg->rtspClient[0].SendAudioData((unsigned char *)pDSoundBuff, lAudSize, lAudSize, true);
			}

			/*
			char szTmp[32];
			memset(szTmp, 0, sizeof(szTmp));
			sprintf(szTmp, "C:\\test\\wav\\%d.wav", nTally++);
			WriteWAV(szTmp, pDSoundBuff, lAudSize);
			*/
			m_DSoundObj.pcmdata_pos	=	0;
			memcpy(m_DSoundObj.pcmdata+m_DSoundObj.pcmdata_pos, pbCaptureData2, dwCaptureLength2);
		}

		m_DSoundObj.m_dwNextCaptureOffset	+= dwCaptureLength2;
		m_DSoundObj.m_dwNextCaptureOffset   %= m_DSoundObj.m_dwCaptureBufferSize;	//Circular buffer
	}

	m_DSoundObj.m_pDSBCapture->Unlock(pbCaptureData, dwCaptureLength,
		pbCaptureData2, dwCaptureLength2);

	return TRUE;
}