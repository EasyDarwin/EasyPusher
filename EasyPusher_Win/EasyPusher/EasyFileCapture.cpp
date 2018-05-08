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
	m_fileType = FILE_MP4;
	memset(&m_videoInfo, 0x00, sizeof(CMp4_avcC_box));
	memset(&m_audioInfo, 0x00, sizeof(CMp4_mp4a_box));
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
	if(g_clock.ClockBase == 0)	
	{
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

int CEasyFileCapture::InitFileCapture( char* sPath, int nStartTime, int nStopTime, bool bAutoLoop)
{
	std::string sFilm  = sPath;//[阳光电影www.ygdy8.com].港囧.HD.720p.国语中字.mp4";//6004501011.MP4";
	m_sFilmPath = sPath;

	//Open mp4 file
	int nPos = sFilm.find(".mp4", 0 );
	int nPos1 = sFilm.find(".MP4", 0 );
	if (nPos>0 || nPos1>0 )
	{
		// now we just support mp4 file
		m_fileType = FILE_MP4;
	}
	else
	{
		nPos = sFilm.find(".ts", 0 );
		nPos1 = sFilm.find(".TS", 0 );
		if ((nPos>0) || (nPos1>0) )
		{
			// now we just support mp4 file
			m_fileType = FILE_TS;
		}	
		else
		{
			return -1;
		}
	}
		
	if (m_fileType == FILE_MP4)
	{
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

		//m_audioInfo.samplerate = 44100;

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
	} 
	else if (m_fileType == FILE_TS)
	{
		bool parse_only=false;
		int dump=0;
		bool av_only=true;
		bool join=false;
		int channel=0;
		bool pes=false;
		bool verb=false;
		std::string output;
		bool mkvmerge_opts=false;
		int nDemuxID = 1;//Audio

		ts::demuxer tsDemuxer;
		tsDemuxer.parse_only=dump>0?true:parse_only;
		tsDemuxer.es_parse=tsDemuxer.parse_only;
		tsDemuxer.dump=dump;
		tsDemuxer.av_only=av_only;
		tsDemuxer.channel=channel;
		tsDemuxer.dst=output;
		tsDemuxer.verb=verb;
		tsDemuxer.m_nDemuxID = nDemuxID;
		tsDemuxer.m_bDemuxerLiving = true;
		tsDemuxer.demux_file(m_sFilmPath.c_str(),true);

		//for a test
		m_nVideoTrackId = 0;
		m_nAudioTrackId = 1;
		m_mediaInfo.nFps = 25;// frame per second

		// 增加从ts流中获取音频的采样率和通道数接口 [6/27/2016 SwordTwelve]
		tsDemuxer.GetTSAudioInfo(m_mediaInfo.nAudioSampleRate, m_mediaInfo.nChannels);
		if (m_mediaInfo.nAudioSampleRate==-1 || m_mediaInfo.nChannels==-1)
		{
			//maybe this is not a aac audio format ts
			//return -1;
		}
		m_mediaInfo.nSampleSize = 16;
	}

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

void CEasyFileCapture::StartFileCapture()
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

void CEasyFileCapture::StopFileCapture()
{
	for(int nI=0; nI<MAX_TRACK_NUM; nI++)
	{
		m_mp4TrackThread[nI] = 0;
		m_bThreadLiving[nI] = false;
		tsDemuxerVideo.m_bDemuxerLiving = false;
		tsDemuxerAudio.m_bDemuxerLiving = false;
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
		if ( m_fileType == FILE_MP4 )
		{
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
					// 				double dbSampleTime = (double)sample_time/m_root.trk[m_nVideoTrackId].mdia.mdhd.timescale ;
					// 				uint32_t uSampleTimeTicket = dbSampleTime*1000000;

					_fseeki64(m_fin,cur,SEEK_SET);
					cur+=sample_size;
					nSampleId++;

#if 1
					EnterCriticalSection(&m_cs);
					uint32_t uSampleTime = Sync_clock(m_root.trk[m_nVideoTrackId].mdia.mdhd.timescale, sample_time,VEDIO_PUSH, &lTimeStamp);
					LeaveCriticalSection(&m_cs);
#endif
					unsigned char *ptr=new unsigned char [sample_size+m_videoInfo.sps->sequenceParameterSetLength+m_videoInfo.pps->pictureParameterSetLength+8];
					fread(ptr, sample_size, 1, m_fin);

					//写一帧数据 --- 可以直接进行网络推送
					//fwrite(ptr, sample_size, 1, fout);
					EASY_AV_Frame	avFrame;
					memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
					byte btHeader[4] = {0x00,0x00,0x00,0x01};

					unsigned char* pFrame = ptr;
					uint32_t nFrameLength = sample_size;
					unsigned char naltype = ( (unsigned char)pFrame[4] & 0x1F);
					// 判断I帧带SPS和PPS头的情况 [6/13/2016 SwordTwelve]
					// 				if (naltype == 0x07)
					// 				{
					// 					int nHeaderLen = 4+m_videoInfo.sps->sequenceParameterSetLength+4+m_videoInfo.pps->pictureParameterSetLength;
					// 					pFrame = ptr + nHeaderLen;
					// 					nFrameLength -= nHeaderLen;
					// 				}
					bool bKeyFrame = false;
					AvcToH264Frame(ptr, sample_size, bKeyFrame, &pFrame, nFrameLength );
					if (bKeyFrame)//I帧
					{
						memmove(pFrame+m_videoInfo.sps->sequenceParameterSetLength+m_videoInfo.pps->pictureParameterSetLength+8, pFrame, nFrameLength);
						memcpy(pFrame, btHeader, 4);
						memcpy(pFrame+4, m_videoInfo.sps->sequenceParameterSetNALUnit, m_videoInfo.sps->sequenceParameterSetLength);
						memcpy(pFrame+4+m_videoInfo.sps->sequenceParameterSetLength, btHeader, 4);
						memcpy(pFrame+4+m_videoInfo.sps->sequenceParameterSetLength+4, m_videoInfo.pps->pictureParameterSetNALUnit, m_videoInfo.pps->pictureParameterSetLength);
					}

					avFrame.pBuffer = (unsigned char*)pFrame;
					avFrame.u32AVFrameLen = nFrameLength+m_videoInfo.sps->sequenceParameterSetLength+m_videoInfo.pps->pictureParameterSetLength+8;
					avFrame.u32VFrameType = (bKeyFrame==true)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
					avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
					avFrame.u32TimestampSec = lTimeStamp/1000000;
					avFrame.u32TimestampUsec = (lTimeStamp%1000000);

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

					//lTimeCount += uSampleTimeTicket;
					lTimeStamp += uSampleTime;
					if(!uSampleTime==0)//uSampleTime uSampleTimeTicket
					{
#ifndef _WIN32
						usleep(uSampleTime);
#else
						SleepEx(uSampleTime/1000, FALSE);
#endif
					}
					delete [] ptr;
				}
			}
		} 
		else if ( m_fileType == FILE_TS )
		{
			bool parse_only=false;
			int dump=0;
			bool av_only=true;
			bool join=false;
			int channel=0;
			bool pes=false;
			bool verb=false;
			std::string output;
			bool mkvmerge_opts=false;
			int nDemuxID = 0;

			tsDemuxerVideo.parse_only=dump>0?true:parse_only;
			tsDemuxerVideo.es_parse=tsDemuxerVideo.parse_only;
			tsDemuxerVideo.dump=dump;
			tsDemuxerVideo.av_only=av_only;
			tsDemuxerVideo.channel=channel;
			tsDemuxerVideo.dst=output;
			tsDemuxerVideo.verb=verb;
			tsDemuxerVideo.m_nDemuxID = nDemuxID;
			tsDemuxerVideo.m_bDemuxerLiving = true;
			tsDemuxerVideo.SetCaptureFileCallback(m_pCallback, m_pMaster);
			tsDemuxerVideo.demux_file(m_sFilmPath.c_str());
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

		if ( m_fileType == FILE_MP4 )
		{
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
					// 				double dbSampleTime = (double)sample_time/m_root.trk[m_nAudioTrackId].mdia.mdhd.timescale ;
					// 				uint32_t uSampleTimeTicket = dbSampleTime*1000000;

					_fseeki64(m_finA,cur,SEEK_SET);
					cur+=sample_size;
					nSampleId++;
#if 1
					EnterCriticalSection(&m_cs);
					uint32_t uSampleTime = Sync_clock(m_root.trk[m_nAudioTrackId].mdia.mdhd.timescale, sample_time,AUDIO_PUSH, &lTimeStamp);
					LeaveCriticalSection(&m_cs);
#endif
					unsigned char *ptr=new unsigned char [sample_size+7];
					fread(ptr, sample_size, 1, m_finA);

#if 0
					fwrite(ptr, sample_size, 1, pAACFile);
					fflush(pAACFile);
#endif		
					uint32_t uSampleSize = sample_size;
					if (ptr[0] == 0xFF && (ptr[1]&0xF0) == 0xF0)
					{
					}
					else//if there is not adt , I will try to  add adts header
					{
						static unsigned long tnsSupportedSamplingRates[16] = //音频采样率标准，下表为写入标志
						{ 96000, 88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0 };
						unsigned char  adts_header[7] ;
						memset(adts_header, 0x00, 7*sizeof(unsigned char));
						adts_header[0] = 0xFF;
						adts_header[1] = 0xF1;//MPEG4(0) ==0xF1   MPEG2(1) == 0xF9
						unsigned char samplerate_t = 0;
						unsigned char channelcount_t = 0;
						unsigned int  aac_frame_length = sample_size + 7;
						unsigned int num_data_block = sample_size/1024;
						unsigned int sample_rate_index = 0xc;//Reserved
						unsigned int channels = m_audioInfo.channelcount;

						// 注意：因为我还没法获取到AAC编码的等级，所以这里默认为AAC-LC，也许你应该知道这个值是多少，从而填在这里--! [4/10/2016 SwordTwelve]
						//obj_type,   4种(0~3对应实际MAIN(1) LOW(2) SSR(3) LTP(4))
						unsigned int obj_type = 1;
						int nI= 0;
						for ( nI = 0; nI<13; nI++)
						{
							if (tnsSupportedSamplingRates[nI] == m_audioInfo.samplerate )
							{
								sample_rate_index =nI; 
								break;
							}
						}

						/* Object type over first 2 bits */
						adts_header[2] = obj_type << 6;//
						/* rate index over next 4 bits */
						adts_header[2] |= (sample_rate_index << 2);
						/* channels over last 2 bits */
						adts_header[2] |= (channels & 0x4) >> 2;
						adts_header[3] = (channels & 0x3) << 6;
						adts_header[3] |= (aac_frame_length & 0x1800) >> 11;
						adts_header[4] = (aac_frame_length & 0x7F8) >> 3;
						adts_header[5] = (aac_frame_length & 0x7) << 5  |  0x1F;
						adts_header[6] = 0xFC  | num_data_block & 0x03; //Set raw Data blocks.;

						uSampleSize = sample_size+7;
						memmove(ptr+7, ptr, sample_size);
						memcpy(ptr, adts_header, 7 );
					}

					//写一帧数据 --- 可以直接进行网络推送
					//fwrite(ptr, sample_size, 1, fout);
					EASY_AV_Frame	avFrame;
					memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));

					avFrame.pBuffer = (unsigned char*)ptr;
					avFrame.u32AVFrameLen = uSampleSize;
					avFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
					avFrame.u32TimestampSec = lTimeStamp/1000000;
					avFrame.u32TimestampUsec = (lTimeStamp%1000000);

					//EasyPusher_PushFrame(g_fPusherHandle, &avFrame);
					//数据回调
					if (m_pCallback)
					{
						m_pCallback(0, &avFrame, m_pMaster);
						//TRACE("Audio:  sample_size = %d\n", sample_size);
					}

					lTimeStamp += uSampleTime;
					//lTimeCount += uSampleTimeTicket;

					// #ifdef _WIN32
					// 				DWORD dwStop = ::GetTickCount();
					// #endif
					if(uSampleTime!=0)//uSampleTimeTicket
					{
#ifndef _WIN32
						usleep(uSampleTime);
#else
						SleepEx(uSampleTime/1000, FALSE);
#endif
					}
					delete [] ptr;
				}
			}
		}
		else if ( m_fileType == FILE_TS )
		{
			bool parse_only=false;
			int dump=0;
			bool av_only=true;
			bool join=false;
			int channel=0;
			bool pes=false;
			bool verb=false;
			std::string output;
			bool mkvmerge_opts=false;
			int nDemuxID = 1;

			//ts::demuxer tsDemuxer;
			tsDemuxerAudio.parse_only=dump>0?true:parse_only;
			tsDemuxerAudio.es_parse=tsDemuxerAudio.parse_only;
			tsDemuxerAudio.dump=dump;
			tsDemuxerAudio.av_only=av_only;
			tsDemuxerAudio.channel=channel;
			tsDemuxerAudio.dst=output;
			tsDemuxerAudio.verb=verb;
			tsDemuxerAudio.m_nDemuxID = nDemuxID;
			tsDemuxerAudio.m_bDemuxerLiving = true;
			tsDemuxerAudio.SetCaptureFileCallback(m_pCallback, m_pMaster);
			tsDemuxerAudio.demux_file(m_sFilmPath.c_str());
		}
		//fclose(pAACFile);
	}

	return 0;
}

//设置捕获数据回调函数
void CEasyFileCapture::SetCaptureFileCallback(CaptureFileCallback callBack, void * pMaster)
{
	m_pCallback = callBack;
	m_pMaster =	pMaster;
}

void CEasyFileCapture::GetMediaInfo(EASY_MEDIA_INFO_T& mediainfo)
{
		//视频轨存在
		if (m_nVideoTrackId>-1)
		{
			mediainfo.u32VideoCodec =   EASY_SDK_VIDEO_CODEC_H264;
			mediainfo.u32VideoFps = m_mediaInfo.nFps;
			if (m_videoInfo.sps&&m_videoInfo.pps)
			{
				mediainfo.u32SpsLength = m_videoInfo.sps->sequenceParameterSetLength;
				mediainfo.u32PpsLength = m_videoInfo.pps->pictureParameterSetLength;
				if (m_videoInfo.sps->sequenceParameterSetNALUnit && m_videoInfo.sps->sequenceParameterSetLength>0 )
				{
					memcpy(mediainfo.u8Sps, m_videoInfo.sps->sequenceParameterSetNALUnit, mediainfo.u32SpsLength);
				}
				if (m_videoInfo.pps->pictureParameterSetNALUnit && m_videoInfo.pps->pictureParameterSetLength>0 )
				{
					memcpy(mediainfo.u8Pps, m_videoInfo.pps->pictureParameterSetNALUnit, mediainfo.u32PpsLength );
				}
			}
		}
		//音频轨存在
		if (m_nAudioTrackId>-1)
		{
			mediainfo.u32AudioCodec =   EASY_SDK_AUDIO_CODEC_AAC;

			mediainfo.u32AudioSamplerate = m_mediaInfo.nAudioSampleRate;
			mediainfo.u32AudioChannel = m_mediaInfo.nChannels;
			mediainfo.u32AudioBitsPerSample = m_mediaInfo.nSampleSize;
		}
}

int CEasyFileCapture::AvcToH264Frame(unsigned char* pFrame, uint32_t nFrameLen,  bool& bKeyFrame, unsigned char** pOutBuffer, uint32_t& nFrameLength)
{
	if ( !pFrame )
	{
		return -1;
	}
	bKeyFrame = false;
	uint32_t nNalCount = 0;
	//第一个nal的大小s
	uint32_t nFirstNalSize = 0;
	bool bFindPFrame = false;
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

		unsigned char naltype =  (unsigned char)((pFrame+nNalCount)[4] & 0x1F);
		if (naltype==0x07&&bKeyFrame == false)//I
		{
			bKeyFrame = true;
			*pOutBuffer = pFrame+nNalCount;
			nFrameLength = nFrameLen-nNalCount;
		}
		if (naltype==0x05&&bKeyFrame == false)//I
		{
			bKeyFrame = true;
			*pOutBuffer = pFrame+nNalCount;
			nFrameLength = nFrameLen-nNalCount;
		}
		if (naltype==0x01&&bFindPFrame == false)//P/B
		{
			bFindPFrame = true;
			*pOutBuffer = pFrame+nNalCount;
			nFrameLength = nFrameLen-nNalCount;
		}

		nNalCount += nFirstNalSize+4;
	}
	return 1;
}
