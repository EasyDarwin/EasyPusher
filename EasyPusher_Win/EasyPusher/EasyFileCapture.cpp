/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// Add by SwordTwelve

#include "StdAfx.h"
#include "EasyFileCapture.h"

#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))

CEasyFileCapture::CEasyFileCapture(void)
{
	m_fin = NULL; 
	m_finA = NULL;
	m_bVideoStart = false;
	InitializeCriticalSection(&m_cs);
	m_nVideoTrackId = -1;
	m_nAudioTrackId = -1;
	m_pMaster = NULL;
	m_pCallback = NULL;
	memset(&m_clock, 0, sizeof(m_clock));
	m_bAutoLoop = true;
	m_nStartTime = -1;//推送开始时间
	m_nEndTime = -1;//推送截止时间
}

CEasyFileCapture::~CEasyFileCapture(void)
{
	DeleteCriticalSection(&m_cs);
}

// Add by Ricky
//Audio and video Sync lock
unsigned long CEasyFileCapture::Sync_clock(unsigned long TimeScale, unsigned long duration, int type, unsigned long* out)
{
	unsigned long timebase;
	unsigned long DiffClock;
	double TimeCalbase;
	double Timenext;
	unsigned long CurrentTime;
	unsigned long NextTime;
	unsigned long delay;
#ifdef _WIN32
	if(m_clock.ClockBase == 0)
	{
		m_clock.ClockBase = ::GetTickCount()*1000;
	}
	m_clock.ClockCurr = ::GetTickCount()*1000;
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
		timebase = m_clock.VedioBase;
	}else
	{
		timebase = m_clock.AudioBase;	
	}

	DiffClock = m_clock.ClockCurr - m_clock.ClockBase;//时钟的耗时间Tick数//微妙级别忽略不计	
	TimeCalbase = (double)timebase/TimeScale;
	Timenext = (double)(timebase+duration)/TimeScale;
	//开始计算当前和小一个Sample的时间估计决定延迟//
	NextTime = (unsigned long)(Timenext*1000000);	
	CurrentTime = (unsigned long)(TimeCalbase*1000000);
	*out = CurrentTime;
	if(DiffClock > NextTime) //已经落后，快进
	{
		delay =  0;
	}
	else
	{
		delay = (NextTime- DiffClock);//重新计算时间
	}
	if(type == VEDIO_PUSH)
	{
		m_clock.VedioBase += duration;
	}else
	{
		m_clock.AudioBase  += duration;	
	}
	return delay;
}

int CEasyFileCapture::InitMP4FileCapture(char* sPath, int nStartTime, int nStopTime, bool bAutoLoop)
{
	//Open mp4 file
	std::string sFilm  = sPath;//[阳光电影www.ygdy8.com].港囧.HD.720p.国语中字.mp4";//6004501011.MP4";
	if (sFilm.find(".mp4", 0 )<=0 && sFilm.find(".MP4", 0 )<=0 )
	{
		// now we just support mp4 file
		return 0;
	}

	m_fin = _fsopen(sFilm.c_str(), "rb",  _SH_DENYNO );	//共享方式打开
	if(m_fin == (FILE*)0)
	{
		//printf("failed to open mp4 file: %s\n", sFilm.c_str());
		return -1;
	}
	m_finA = _fsopen(sFilm.c_str(), "rb",  _SH_DENYNO );//共享方式打开	
	if(m_finA == (FILE*)0)
	{
		//printf("failed to open pmp4 file: %s\n", sFilm.c_str());
		return -1;
	}

	unsigned int cur_pos= _ftelli64(m_fin);
	for(;!feof(m_fin); )
	{
		_fseeki64(m_fin, cur_pos, SEEK_SET);
		//printf("----------------------------------------level 0\n");
		cur_pos += m_root.mp4_read_root_box(m_fin);
	}

	// 	printf("---------------------------------------- 0ye\n");
	// 	printf("  %s   MP4Demux is Completed!\n", sFilm.c_str());
	// 	printf("---------------------------------------- 0ye\n");

	//Get AV Frameinfo
	//从MP4文件获取音视频编码信息，填入pusher媒体信息结构中
	memset(&m_videoInfo, 0x00, sizeof(CMp4_avcC_box));
	memset(&m_audioInfo, 0x00, sizeof(CMp4_mp4a_box));
	m_nVideoTrackId = get_video_info_in_moov(m_root,  m_videoInfo );
	m_nAudioTrackId = get_audio_info_in_moov(m_root,  m_audioInfo );

	double dbTimeDuration = (double)m_root.moov.mvhd.duration;
	double dbTimeScale = (double)m_root.moov.mvhd.timescale;

	double dbDuration = dbTimeDuration/dbTimeScale;//m_root.trk[m_nVideoTrackId].mdia.mdhd.duration*1000/m_root.trk[m_nVideoTrackId].mdia.mdhd.timescale; //已视频为参考，获取整个MP4的时间长度
	m_mediaInfo.nDuration = dbDuration*1000;
	m_mediaInfo.nVideoSampleSize = m_root.sz[m_nVideoTrackId].table_size;
	m_mediaInfo.nVideoWidth = m_root.sd[m_nVideoTrackId].sample_description->avc1->width ;  //m_root.trk[m_nVideoTrackId].tkhd.width;
	m_mediaInfo.nVideoHeight = m_root.sd[m_nVideoTrackId].sample_description->avc1->height ;//m_root.trk[m_nVideoTrackId].tkhd.height;
	m_mediaInfo.nFps = m_mediaInfo.nVideoSampleSize / dbDuration;// frame per second
	m_mediaInfo.nAudioSampleRate = m_audioInfo.samplerate;
	m_mediaInfo.nChannels = m_audioInfo.channelcount;
	m_mediaInfo.nSampleSize = m_audioInfo.samplesize;

	m_bAutoLoop = bAutoLoop;
	m_nStartTime = nStartTime*1000;//nStartTime;//推送开始时间
	m_nEndTime = nStopTime*1000;//推送截止时间
	if (m_nStartTime<0|| m_nStartTime> m_mediaInfo.nDuration)
	{
		m_nStartTime = 0;
	}

	if (m_nEndTime<=0 || m_nEndTime > m_mediaInfo.nDuration)
	{
		m_nEndTime = m_mediaInfo.nDuration;
	}

	if (m_nStartTime>=m_nEndTime)
	{
		m_nStartTime = 0;
	}

	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		m_mp4TrackThread[nI] = 0;
		m_bThreadLiving[nI] = false;
	}

	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		m_mp4TrackThread[nI] = 0;
		m_bThreadLiving[nI] = false;
	}

	m_bVideoStart = false;
	return 1;
}

void CEasyFileCapture::StartMP4FileCapture()
{
	//视频轨存在
	if (m_nVideoTrackId>-1)
	{
		m_mp4TrackThread[m_nVideoTrackId] = (HANDLE)_beginthreadex(NULL, 0, VideoThread, (void*)this,0,0);
		m_bThreadLiving[m_nVideoTrackId] = true;
	}

	//音频轨存在
	if (m_nAudioTrackId>-1)
	{
		//Create thread to push mp4 demux data( aac)
		m_mp4TrackThread[m_nAudioTrackId] = (HANDLE)_beginthreadex(NULL, 0, AudioThread,  (void*)this,0,0);
		m_bThreadLiving[m_nAudioTrackId] = true;
	}
	memset(&m_clock, 0, sizeof(m_clock));

}

void CEasyFileCapture::StopMP4FileCapture()
{
	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		m_mp4TrackThread[nI] = 0;
		m_bThreadLiving[nI] = false;
	}
	//等待线程结束
	Sleep(300);
	//获取线程结束代码 ,如果线程还在运行就等她结束
	// 	DWORD dwExitCode ;
	// 	::GetExitCodeThread(m_hScreenCaptureThread,&dwExitCode);
	// 	if(dwExitCode == STILL_ACTIVE)
	// 	{
	// 		WaitForSingleObject(m_hScreenCaptureThread, INFINITE); 
	// 		CloseHandle(m_hScreenCaptureThread);
	// 		m_hScreenCaptureThread = INVALID_HANDLE_VALUE;
	// 	}
}

//MP4 file pusher  calllback
unsigned int _stdcall  CEasyFileCapture::VideoThread(void* lParam)
{
	CEasyFileCapture* pCapture = (CEasyFileCapture*)lParam;
	if (pCapture)
	{
		pCapture->VideoProcess();
	}
	return 0;
}


unsigned int _stdcall  CEasyFileCapture::AudioThread(void* lParam)
{
	CEasyFileCapture* pCapture = (CEasyFileCapture*)lParam;
	if (pCapture)
	{
		pCapture->AudioProcess();
	}
	return 0;
}

int CEasyFileCapture::VideoProcess()
{
	bool bThreadActive = true;
	while (m_bThreadLiving[m_nVideoTrackId]&&bThreadActive)
	{
		if (!m_bAutoLoop)//不进行循环推送
		{
			bThreadActive = false;
		}
		m_bVideoStart = true;
		int chunk_offset_amount    = m_root.co[m_nVideoTrackId].chunk_offset_amount;
		unsigned long lTimeStamp = 0;
		unsigned long lTimeCount = 0;   
		int nSampleId = 0;
		for(int chunk_index = 0 ; chunk_index < chunk_offset_amount; ++chunk_index)
		{
			if (!m_bThreadLiving[m_nVideoTrackId])
			{
				return 0;
			}

			//copy_sample_data(g_fin, chunk_index, name,nID,root,nSampleId);
			_fseeki64(m_fin, m_root.co[m_nVideoTrackId].chunk_offset_from_file_begin[chunk_index], SEEK_SET);

			//获取当前chunk中有多少个sample
			uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(m_root.sc[m_nVideoTrackId], chunk_index+1);  //@a mark获取chunk中sample的数目
			uint32_t sample_index_ =  get_sample_index(m_root.sc[m_nVideoTrackId], chunk_index+1);//chunk中第一个sample的序号
			unsigned int cur=_ftelli64(m_fin);
			for(int i = 0; i < sample_num_in_cur_chunk_; i++)
			{
				if (!m_bThreadLiving[m_nVideoTrackId])
				{
					return 0;
				}
				// #ifdef _WIN32
				// 				DWORD dwStart = ::GetTickCount();
				// #endif
				uint32_t sample_size = get_sample_size(m_root.sz[m_nVideoTrackId], sample_index_+i);//获取当前sample的大小
				uint32_t sample_time = get_sample_time(m_root.ts[m_nVideoTrackId], nSampleId );
				double dbSampleTime = (double)sample_time/m_root.trk[m_nVideoTrackId].mdia.mdhd.timescale ;
				uint32_t uSampleTimeTicket = dbSampleTime*1000000;

				_fseeki64(m_fin,cur,SEEK_SET);
				cur+=sample_size;
				nSampleId++;

#if 0
				EnterCriticalSection(&m_cs);
				uint32_t uSampleTime = Sync_clock(m_root.trk[m_nVideoTrackId].mdia.mdhd.timescale, sample_time,VEDIO_PUSH, &lTimeStamp);
				LeaveCriticalSection(&m_cs);
#endif
				unsigned char *ptr=new unsigned char [sample_size];
				fread(ptr, sample_size, 1, m_fin);

				//写一帧数据 --- 可以直接进行网络推送
				//fwrite(ptr, sample_size, 1, fout);
				EASY_AV_Frame	avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));

				unsigned char* pFrame = ptr;
				unsigned char naltype = ( (unsigned char)pFrame[4] & 0x1F);
				// 判断I帧带SPS和PPS头的情况 [6/13/2016 SwordTwelve]
				if (naltype == 0x07)
				{
					int nHeaderLen = 4+m_videoInfo.sps->sequenceParameterSetLength+4+m_videoInfo.pps->pictureParameterSetLength;
					pFrame = ptr + nHeaderLen;
					sample_size -= nHeaderLen;
				}

				AvcToH264Frame(pFrame, sample_size );

// 				pFrame[0] = 0x00;
// 				pFrame[1] = 0x00;
// 				pFrame[2] = 0x00;
// 				pFrame[3] = 0x01;

				avFrame.pBuffer = (unsigned char*)pFrame;
				avFrame.u32AVFrameLen = sample_size;
				avFrame.u32VFrameType = (naltype==0x05)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
				avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
				avFrame.u32TimestampSec = lTimeCount/1000000;
				avFrame.u32TimestampUsec = (lTimeCount%1000000);

				//EasyPusher_PushFrame(g_fPusherHandle, &avFrame);
				//数据回调
				if (m_pCallback)
				{
					m_pCallback(0, &avFrame, m_pMaster);
					TRACE("Video:  sample_size = %d\n", sample_size);
				}

				// #ifdef _WIN32
				// 
				// 				DWORD dwStop = ::GetTickCount();
				// #endif
				//printf("Sleep=%d\r\n", uSampleTime/1000-(dwStop-dwStart));
				
				lTimeCount += 30000;//uSampleTimeTicket;

				if(uSampleTimeTicket!=0)//uSampleTime
				{
#ifndef _WIN32
					usleep(uSampleTimeTicket);
#else
					SleepEx(/*uSampleTimeTicket/1000*/30, FALSE);
#endif
				}
				delete [] ptr;
			}
		}
	}
		return 0;
}

int CEasyFileCapture::AudioProcess()
{
	bool bThreadActive = true;
	while (m_bThreadLiving[m_nVideoTrackId]&&bThreadActive)
	{
		
		if (!m_bAutoLoop)//不进行循环推送
		{
			bThreadActive = false;
		}
		if (!m_bVideoStart)
		{
			Sleep(1);
			//printf("Audio Thread waiting.........\r\n");
			continue;
		}
		int chunk_offset_amount    = m_root.co[m_nAudioTrackId].chunk_offset_amount;
		unsigned long lTimeStamp = 0;
		unsigned long lTimeCount = 0;   
		int nSampleId = 0;
		for(int chunk_index = 0 ; chunk_index < chunk_offset_amount; ++chunk_index)
		{
			if (!m_bThreadLiving[m_nAudioTrackId])
			{
				return 0;
			}

			//copy_sample_data(g_fin, chunk_index, name,nID,root,nSampleId);
			_fseeki64(m_finA, m_root.co[m_nAudioTrackId].chunk_offset_from_file_begin[chunk_index], SEEK_SET);

			//获取当前chunk中有多少个sample
			uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(m_root.sc[m_nAudioTrackId], chunk_index+1);  //@a mark获取chunk中sample的数目
			uint32_t sample_index_ =  get_sample_index(m_root.sc[m_nAudioTrackId], chunk_index+1);//chunk中第一个sample的序号
			unsigned int cur=_ftelli64(m_finA);
			for(int i = 0; i < sample_num_in_cur_chunk_; i++)
			{
				if (!m_bThreadLiving[m_nAudioTrackId])
				{
					return 0;
				}

				// #ifdef _WIN32
				// 			DWORD dwStart = ::GetTickCount();
				// #endif
				uint32_t sample_size = get_sample_size(m_root.sz[m_nAudioTrackId], sample_index_+i);//获取当前sample的大小
				uint32_t sample_time = get_sample_time(m_root.ts[m_nAudioTrackId], nSampleId );
				double dbSampleTime = (double)sample_time/m_root.trk[m_nAudioTrackId].mdia.mdhd.timescale ;
				uint32_t uSampleTimeTicket = dbSampleTime*1000000;

				_fseeki64(m_finA,cur,SEEK_SET);
				cur+=sample_size;
				nSampleId++;
#if 0
				EnterCriticalSection(&m_cs);
				uint32_t uSampleTime = Sync_clock(m_root.trk[m_nAudioTrackId].mdia.mdhd.timescale, sample_time,AUDIO_PUSH, &lTimeStamp);
				LeaveCriticalSection(&m_cs);
#endif
				unsigned char *ptr=new unsigned char [sample_size];
				fread(ptr, sample_size, 1, m_finA);

				//写一帧数据 --- 可以直接进行网络推送
				//fwrite(ptr, sample_size, 1, fout);
				EASY_AV_Frame	avFrame;
				memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));

				avFrame.pBuffer = (unsigned char*)ptr;
				avFrame.u32AVFrameLen = sample_size;
				avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
				avFrame.u32TimestampSec = lTimeCount/1000000;
				avFrame.u32TimestampUsec = (lTimeCount%1000000);

				//EasyPusher_PushFrame(g_fPusherHandle, &avFrame);
				//数据回调
				if (m_pCallback)
				{
					m_pCallback(0, &avFrame, m_pMaster);
					//TRACE("Audio:  sample_size = %d\n", sample_size);
				}

				//lTimeStamp += uSampleTime;
				lTimeCount += uSampleTimeTicket;

				// #ifdef _WIN32
				// 				DWORD dwStop = ::GetTickCount();
				// #endif
				if(uSampleTimeTicket!=0)//uSampleTime
				{
#ifndef _WIN32
					usleep(uSampleTimeTicket);
#else
					SleepEx(uSampleTimeTicket/1000, FALSE);
#endif
				}
				delete [] ptr;
			}
		}
	}
	return 0;
}

//设置捕获数据回调函数
void CEasyFileCapture::SetCaptureFileCallback(CaptureFileCallback callBack, void * pMaster)
{
	m_pCallback = callBack;
	m_pMaster =	pMaster;
}
void CEasyFileCapture::GetMP4FileInfo(MediaInfo* pMediaInfo)
{
	memcpy(pMediaInfo, &m_mediaInfo, sizeof(MediaInfo));
}

void CEasyFileCapture::GetMediaInfo(EASY_MEDIA_INFO_T& mediainfo)
{
		//视频轨存在
		if (m_nVideoTrackId>-1)
		{
			mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;
			mediainfo.u32VideoFps = m_mediaInfo.nFps;
			mediainfo.u32H264SpsLength = m_videoInfo.sps->sequenceParameterSetLength;
			mediainfo.u32H264PpsLength = m_videoInfo.pps->pictureParameterSetLength;
			if (m_videoInfo.sps->sequenceParameterSetNALUnit && m_videoInfo.sps->sequenceParameterSetLength>0 )
			{
				memcpy(mediainfo.u8H264Sps, m_videoInfo.sps->sequenceParameterSetNALUnit, mediainfo.u32H264SpsLength);
			}
			if (m_videoInfo.pps->pictureParameterSetNALUnit && m_videoInfo.pps->pictureParameterSetLength>0 )
			{
				memcpy(mediainfo.u8H264Pps, m_videoInfo.pps->pictureParameterSetNALUnit, mediainfo.u32H264PpsLength );
			}
		}
		//音频轨存在
// 		if (m_nAudioTrackId>-1)
// 		{
// 			mediainfo.u32AudioCodec =   EASY_SDK_AUDIO_CODEC_AAC;
// 			mediainfo.u32AudioSamplerate = m_audioInfo.samplerate;
// 			mediainfo.u32AudioChannel = m_audioInfo.channelcount;
// 			mediainfo.u32AudioBitsPerSample = m_audioInfo.samplesize;
// 		}
}

int CEasyFileCapture::AvcToH264Frame(unsigned char* pFrame, uint32_t nFrameLen)
{
	if ( !pFrame )
	{
		return -1;
	}
	
	uint32_t nNalCount = 0;
	//第一个nal的大小s
	uint32_t nFirstNalSize = 0;
	while (nNalCount < nFrameLen)
	{
		unsigned char ucHeader[4];
		memcpy(ucHeader, pFrame+nNalCount, 4);
		SWAP(ucHeader[1], ucHeader[2]);
		SWAP(ucHeader[0], ucHeader[3]);
		memcpy(&nFirstNalSize,ucHeader,4 );
		
		(pFrame+nNalCount)[0] = 0x00;
		(pFrame+nNalCount)[1] = 0x00;
		(pFrame+nNalCount)[2] = 0x00;
		(pFrame+nNalCount)[3] = 0x01;

		nNalCount += nFirstNalSize+4;
	}
	return 1;
}
