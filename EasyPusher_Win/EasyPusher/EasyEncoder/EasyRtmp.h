/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/

// EasyRtmp.h: interface for the EasyRtmp class.
//
//  Add by SwordTwelve
// 
//////////////////////////////////////////////////////////////////////

#if !defined(MYRTMP_H)
#define MYRTMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "StdAfx.h"
#include "librtmp/rtmp.h"
#pragma comment(lib,"librtmp.lib")

#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|x&0xff00)
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)|\
	(x<<8&0xff0000)|(x<<24&0xff000000))


class EasyRtmp  
{
	CRITICAL_SECTION m_cs;

	long m_lStartimestamp;

// 	long m_startimestamp;
// 	long m_lasttimestamp;
	RTMP*m_rtmp;
	char* m_pTempbuff;
	RTMPPacket* m_packet;
	bool m_bwritevideoinfo;
	bool m_bwriteaudiinfo;
	unsigned char* m_psps;
	unsigned char* m_ppps;
	int m_spslen;
	int m_ppslen;

public:
	unsigned char* FindNal(unsigned char*buff,int inlen,int&outlen,bool&end);

	bool WriteH264SPSandPPS(byte*sps,int spslen,byte*pps,int ppslen,long timestamp);
	//写入AAC信息
	bool WriteAACInfo(byte*info,int len,long timestamp);
	//写入一帧，前四字节为该帧NAL长度
	bool WriteH264Frame(byte*data,int len,long timestamp,bool keyframe);
	//写入一帧，以0x65 0x61开头
	bool WriteH264Frame2(byte*data,int len,long timestamp,bool keyframe);
	//写入aac数据，只有raw数据
	bool WriteAACFrame(byte*data,int len,long timestamp);
	int SendReSetChunkSize(int nChunkSize = 1024);

	bool WriteAudioAdst(unsigned char*buff,int length,int timestamp, BOOL bLocalAudio = TRUE);
	bool WriteVideoH264(unsigned char*buff,int length,int timestamp,bool bKeyFrame);
	void Send(char*data,int len,int type,int timestemp);
	int Link(char* url, int nChunkSize=1280);
	void Clean();
	void Init();
	bool IsLinked();
	EasyRtmp();
	virtual ~EasyRtmp();

};

#endif // !defined(AFX_MYRTMP_H__36C4FA2C_221E_4621_AD56_01467C871052__INCLUDED_)
