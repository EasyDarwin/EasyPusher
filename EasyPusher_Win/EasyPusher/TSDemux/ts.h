
/*

Copyright (C) 2009 Anton Burdinuk

clark15b@gmail.com

*/
// Get code from tsdemux git demo [6/22/2016 Administrator]

#ifndef __TS_H
#define __TS_H
#include "common.h"
#include "h264.h"
#include "ac3.h"
#include "EasyPusherAPI.h"

typedef int (WINAPI *CaptureFileCallback)(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);

namespace ts
{
	inline u_int8_t to_byte(const char* p)
	{ return *((unsigned char*)p); }

	inline u_int16_t to_int(const char* p)
	{ u_int16_t n=((unsigned char*)p)[0]; n<<=8; n+=((unsigned char*)p)[1]; return n; }

	inline u_int32_t to_int32(const char* p)
	{
		u_int32_t n=((unsigned char*)p)[0];
		n<<=8; n+=((unsigned char*)p)[1];
		n<<=8; n+=((unsigned char*)p)[2];
		n<<=8; n+=((unsigned char*)p)[3];
		return n;
	}

	class table
	{
	public:
		enum { max_buf_len=512 };

		char buf[max_buf_len];

		u_int16_t len;

		u_int16_t offset;

		table(void):offset(0),len(0) {}

		void reset(void) { offset=0; len=0; }
	};

	class file
	{
	protected:
		int fd;

		enum { max_buf_len=2048 };

		char buf[max_buf_len];

		int len,offset;
	public:
		std::string filename;
	public:
		file(void):fd(-1),len(0),offset(0) {}
		~file(void);

		enum { in=0, out=1 };

		bool open(int mode,const char* fmt,...);
		void close(void);
		int write(const char* p,int l);
		int flush(void);
		int read(char* p,int l);

		bool is_opened(void) { return fd==-1?false:true; }
	};

	namespace stream_type
	{
		enum
		{
			data                = 0,
			mpeg2_video         = 1,
			h264_video          = 2,
			vc1_video           = 3,
			ac3_audio           = 4,
			mpeg2_audio         = 5,
			lpcm_audio          = 6,
			dts_audio           = 7,
			// add aac audio support [6/21/2016 Dingshuai]
			aac_audio           = 8
		};
	}

	class counter_ac3
	{
	private:
	public:
		counter_ac3(void) {}

		void parse(const char* p,int l)
		{
		}

		u_int64_t get_frame_num(void) const { return 0; }

		void reset(void)
		{
		}
	};

	class stream
	{
	public:
		u_int16_t channel;                      // channel number (1,2 ...)
		u_int8_t  id;                           // stream number in channel
		u_int8_t  type;                         // 0xff                 - not ES
		// 0x01,0x02            - MPEG2 video
		// 0x80                 - MPEG2 video (for TS only, not M2TS)
		// 0x1b                 - H.264 video
		// 0xea                 - VC-1  video
		// 0x81,0x06,0x83       - AC3   audio
		// 0x03,0x04            - MPEG2 audio
		// 0x80                 - LPCM  audio
		// 0x82,0x86,0x8a       - DTS   audio
		//

		table psi;                              // PAT,PMT cache (only for PSI streams)

		u_int8_t stream_id;                     // MPEG stream id

		enum { max_cachebuf_len=2048*1024 };//2M buffer
		//数据帧缓存
		unsigned char* m_pCache;
		u_int32_t m_nCacheLength;

		ts::file file;                          // output ES file
		FILE* timecodes;

		u_int64_t dts;                          // current MPEG stream DTS (presentation time for audio, decode time for video)
		u_int64_t first_dts;
		u_int64_t first_pts;
		u_int64_t last_pts;
		u_int32_t frame_length;                 // frame length in ticks (90 ticks = 1 ms, 90000/frame_length=fps) 
		u_int64_t frame_num;                    // frame counter
		u_int64_t m_nTimeStamp;				 // frame timestamp
		h264::counter frame_num_h264;           // JVT NAL (h.264) frame counter
		ac3::counter  frame_num_ac3;            // A/52B (AC3) frame counter

		stream(void):channel(0xffff),id(0),type(0xff),stream_id(0),
			dts(0),first_dts(0),first_pts(0),last_pts(0),frame_length(0),frame_num(0),timecodes(0), m_nCacheLength(0), m_nTimeStamp(0)
		{
			m_pCache = new unsigned char[max_cachebuf_len];
			memset(m_pCache, 0x00, max_cachebuf_len);
		}

		~stream(void);

		void reset(void)
		{
			psi.reset();
			dts=first_pts=last_pts=0;
			frame_length=0;
			frame_num=0;
			frame_num_h264.reset();
			frame_num_ac3.reset();
			if (m_pCache)
			{
				delete[] m_pCache;
				m_pCache = NULL;
			}
		}

		u_int64_t get_es_frame_num(void) const
		{
			if(frame_num_h264.get_frame_num())
				return frame_num_h264.get_frame_num();

			if(frame_num_ac3.get_frame_num())
				return frame_num_ac3.get_frame_num();

			return 0;
		}
	};

	class demuxer
	{
	public:
		std::map<u_int16_t,stream> streams;
		bool hdmv;                                      // HDMV mode, using 192 bytes packets
		bool av_only;                                   // Audio/Video streams only
		bool parse_only;                                // no demux
		int dump;                                       // 0 - no dump, 1 - dump M2TS timecodes, 2 - dump PTS/DTS, 3 - dump tracks
		int channel;                                    // channel for demux
		int pes_output;                                 // demux to PES
		std::string prefix;                             // output file name prefix (autodetect)
		std::string dst;                                // output directory
		bool verb;                                      // verbose mode
		bool es_parse;

		//用于输出TS包信息
		FILE *fp_dump;
		//回调函数参数
		CaptureFileCallback m_pCallback;
		void* m_pMaster;
		int m_nDemuxID;//ts流解析标志：0=V, 1=A, 2=AV
		bool m_bDemuxerLiving;//检测是否中断TSDemuxer
		// audio info
		int m_nSampleRate;
		int m_nChannels;

	public:
		u_int64_t base_pts;
		std::string subs_filename;
	protected:
		FILE* subs;
		u_int32_t subs_num;

		bool validate_type(u_int8_t type);
		u_int64_t decode_pts(const char* ptr);
		int get_stream_type(u_int8_t type);
		const char* get_stream_ext(u_int8_t type_id);

		void write_timecodes(FILE* fp,u_int64_t first_pts,u_int64_t last_pts,u_int32_t frame_num,u_int32_t frame_len);
#ifndef OLD_TIMECODES
		void write_timecodes2(FILE* fp,u_int64_t first_pts,u_int64_t last_pts,u_int32_t frame_num,u_int32_t frame_len);
#endif
	public:
		demuxer(void):hdmv(false),av_only(true),parse_only(false),dump(0),channel(0),base_pts(0),pes_output(0),
			verb(false),es_parse(false),subs(0),subs_num(0), m_pCallback(0), m_pMaster(0), m_nDemuxID(2), m_bDemuxerLiving(false), m_nSampleRate(-1), m_nChannels(-1) {}
		~demuxer(void)
		{ 
			if(subs) fclose(subs); 
			reset(); 
		}

		void show(void);


		// take 188/192 bytes TS/M2TS packet
		int demux_ts_packet(const char* ptr, bool bInitInfo=false);
		int demux_file(const char* name,bool bInitInfo=false);

		int gen_timecodes(const std::string& datetime);

		void reset(void)
		{
			for(std::map<u_int16_t,stream>::iterator i=streams.begin();i!=streams.end();++i)
				i->second.reset();
		}
		//设置回调函数
		//设置捕获数据回调函数
		void SetCaptureFileCallback(CaptureFileCallback callBack, void * pMaster);
		//从一个带adts头的ts流文件中获取音频信息
		void GetTSAudioInfo(int& nSampleRate, int& nChannels)
		{
			nSampleRate = m_nSampleRate ;
			nChannels = m_nChannels;
		}
	};

	const char* timecode_to_time(u_int32_t timecode);
	int TsToH264Frame(unsigned char* pFrame, u_int32_t nFrameLen,  bool& bKeyFrame, unsigned char** pOutBuffer, u_int32_t& nFrameLength);
}
#endif
