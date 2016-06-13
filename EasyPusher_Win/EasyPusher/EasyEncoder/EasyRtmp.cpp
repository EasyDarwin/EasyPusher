/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyRtmp.cpp: implementation of the EasyRtmp class.
//
// Add by SwordTwelve
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "FLVCOMMON.h"
#include "EasyRtmp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char* EasyRtmp::FindNal(unsigned char*buff,int inlen,int&outlen,bool&end)
{
	unsigned char*tempstart=NULL;
	unsigned char*search=buff+2;
	unsigned char*searchper1=buff;
	unsigned char*searchper2=buff+1;
	outlen=0;
	end=false;
	while((search-buff)<inlen)
	{
		if (search[0]==0x01&&searchper1[0]==0x00&&searchper2[0]==0x00)
		{
			if (tempstart==NULL)
			{
				tempstart=search+1;
			}else{
				outlen=search-tempstart-3+1;
				break;
			}
		}
		searchper2=searchper1;
		searchper1=search;
		search++;
	}
	if (outlen==0&&tempstart!=NULL)
	{
		outlen=search-tempstart;
		end=true;
	}
	if (tempstart==NULL)
	{
		end=true;
	}
	return tempstart;
}

EasyRtmp::EasyRtmp()
{
	InitializeCriticalSection(&m_cs);

	m_rtmp=NULL;
	m_packet=NULL;
	m_pTempbuff=NULL;
	m_lStartimestamp = -1;
	m_psps=NULL;
	m_ppps=NULL;
	m_spslen=0;
	m_ppslen=0;
}

EasyRtmp::~EasyRtmp()
{
	Clean();
	DeleteCriticalSection(&m_cs);

}

void EasyRtmp::Init()
{
	Clean();
	m_rtmp=RTMP_Alloc();
	RTMP_Init(m_rtmp);
	m_rtmp->Link.timeout=20;
	m_packet=new RTMPPacket();
	RTMPPacket_Alloc(m_packet,1920*1024*3);//给packet分配数据空间
	RTMPPacket_Reset(m_packet);//重置packet状态
	m_pTempbuff=m_packet->m_body;

	m_bwritevideoinfo=false;
	m_bwriteaudiinfo=false;
// 	m_startimestamp=-1;
// 	m_lasttimestamp=-1;
	m_lStartimestamp = -1;
	if (m_psps)
	{
		delete m_psps;
		m_psps = NULL;
	}
	if (m_ppps)
	{
		delete m_ppps;
		m_ppps = NULL;
	}
	m_spslen=0;
	m_ppslen=0;	
}

void EasyRtmp::Clean()
{
	if (m_rtmp!=NULL)
	{
		if(RTMP_IsConnected(m_rtmp))
		{
			RTMP_Close(m_rtmp);
		}
		RTMP_Free(m_rtmp);
		m_rtmp=NULL;
	}

	if (m_packet!=NULL)
	{
		RTMPPacket_Free(m_packet);
		delete m_packet;
		m_packet=NULL;
	}
	if (m_psps)
	{
		delete m_psps;
		m_psps = NULL;
	}
	if (m_ppps)
	{
		delete m_ppps;
		m_ppps = NULL;
	}
	m_spslen=0;
	m_ppslen=0;

}

// 设置RTMP推送直播服务器的ChunkSize大小，用以解决在公网推送时，
// 服务器不能正确播放视频的问题 [7/29/2014-10:47:41 Dingshuai]
int EasyRtmp::SendReSetChunkSize(int nChunkSize)
{
	RTMPPacket pack;
	int nVal = nChunkSize;
	RTMPPacket_Alloc(&pack,4);
	pack.m_packetType = RTMP_PACKET_TYPE_CHUNK_SIZE;
	pack.m_nChannel = 0x02;
	pack.m_headerType = RTMP_PACKET_SIZE_LARGE;
	pack.m_nTimeStamp = 0;
	pack.m_nInfoField2 = 0;
	pack.m_nBodySize = 4;
	pack.m_body[3] = nVal & 0xff;
	pack.m_body[2] = nVal >> 8;
	pack.m_body[1] = nVal >> 16;
	pack.m_body[0] = nVal >> 24;

	m_rtmp->m_outChunkSize = nVal;
	int nRet = RTMP_SendPacket(m_rtmp, &pack, 0);
	RTMPPacket_Free(&pack);
	return nRet;
}

int EasyRtmp::Link(char* url, int nChunkSize)
{
	Init();	
	int res=0;
	RTMP_SetupURL(m_rtmp, url);
	RTMP_EnableWrite(m_rtmp);
 	res=RTMP_Connect(m_rtmp,NULL);
	if (res==0)
	{
		return -1;
	}
	res=RTMP_ConnectStream(m_rtmp,0);
	if (res==0)
	{
		return -2;
	}
	m_packet->m_body=m_pTempbuff;
	m_packet->m_hasAbsTimestamp = 0; 
	m_packet->m_nChannel = 0x04; 
	m_packet->m_nInfoField2 = m_rtmp->m_stream_id;
	SendReSetChunkSize(nChunkSize);
	return 1;
}

void EasyRtmp::Send(char *data, int len, int type, int timestemp)
{
//	EnterCriticalSection(&m_cs);

    m_packet->m_headerType = RTMP_PACKET_SIZE_LARGE;//RTMP_PACKET_SIZE_MEDIUM;// 
	m_packet->m_nTimeStamp = timestemp;//m_lasttimestamp-m_startimestamp; 
	m_packet->m_packetType=type;
	m_packet->m_nBodySize=len;
//	m_packet->m_hasAbsTimestamp = 0;
// 	m_packet->m_nInfoField2 = m_rtmp->m_stream_id;
	int nRet = RTMP_SendPacket(m_rtmp,m_packet,0);
	TRACE("type:%d len:%d time:%d nRet=%d\n",type,len,timestemp, nRet);

//	LeaveCriticalSection(&m_cs);
}

bool EasyRtmp::WriteH264SPSandPPS(byte*sps,int spslen,byte*pps,int ppslen,long timestamp)
{
	EnterCriticalSection(&m_cs);

	memset(m_pTempbuff,0,100);
	int datalen=spslen+ppslen+16;
	int templen=0;
	m_pTempbuff[0]=0x17;
	m_pTempbuff[1]=0x00;
	m_pTempbuff[5]=0x01;
	m_pTempbuff[6]=sps[1];
	m_pTempbuff[7]=sps[2];
	m_pTempbuff[8]=sps[3];
	m_pTempbuff[9]=0x03;
	m_pTempbuff[10]=(char)0xe1;
	
	templen=HTON16(spslen);
	memcpy(m_pTempbuff+11,&templen,2);
	memcpy(m_pTempbuff+13,sps,spslen);
	
	m_pTempbuff[13+spslen]=0x01;
	
	templen=HTON16(ppslen);
	memcpy(m_pTempbuff+14+spslen,&templen,2);
	memcpy(m_pTempbuff+16+spslen,pps,ppslen);

	m_bwritevideoinfo=1;
	Send(m_pTempbuff,datalen,9,timestamp);
	LeaveCriticalSection(&m_cs);

	return true;
}
//写入AAC信息
bool EasyRtmp::WriteAACInfo(byte*info,int len,long timestamp)
{
	EnterCriticalSection(&m_cs);

	memset(m_pTempbuff,0,100);
	int datalen=len+2;
	m_pTempbuff[0]=(char)0xaf;
	m_pTempbuff[1]=0x00;
	memcpy(m_pTempbuff+2,info,len);
	m_bwriteaudiinfo=1;
	Send(m_pTempbuff,datalen,8,timestamp);
	LeaveCriticalSection(&m_cs);
	return true;
}
//写入一帧，前四字节为该帧NAL长度
bool EasyRtmp::WriteH264Frame(byte*data,int len,long timestamp,bool keyframe)
{	
	if (m_lStartimestamp==-1&&keyframe)
	{
		m_lStartimestamp=timestamp;
	}

	if (m_lStartimestamp>0)
	{
		memset(m_pTempbuff,0,100);
		int datalen=len+5;	
		if (keyframe)
			m_pTempbuff[0]=0x17;
		else
			m_pTempbuff[0]=0x27;
		m_pTempbuff[1]=0x01;
		memcpy(m_pTempbuff+5,data,len);
		Send(m_pTempbuff,datalen,9,timestamp-m_lStartimestamp);
	}
	return true;
}

//写入一帧，以0x65 0x61开头
bool EasyRtmp::WriteH264Frame2(byte*data,int len,long timestamp,bool keyframe)
{	
	//  [1/26/2016 SwordTwelve]
	//什么时候需要加临界区？
	// 这里详细说明一下：当没有进缓存队列时，音视频可能同时进行推送，那么服务器处理不过来时就可能断掉该推送连接，
	// 所以，音频和视频推送就要加临界区进行强制分别发送，防止抢占服务器资源而被断开连接

	EnterCriticalSection(&m_cs);
	if (m_lStartimestamp==-1&&keyframe)
	{
		m_lStartimestamp=timestamp;
	}

	if (m_lStartimestamp>0)
	{
		memset(m_pTempbuff,0,100);
		int datalen=len+9;
		int templen=HTON32(len);

		if (keyframe)
			m_pTempbuff[0]=0x17;
		else
			m_pTempbuff[0]=0x27;
		m_pTempbuff[1]=0x01;

//		memcpy(m_pTempbuff+5,data,len);
		memcpy(m_pTempbuff+5,&templen,4);
		memcpy(m_pTempbuff+9,data,len);
		Send(m_pTempbuff,datalen,9,timestamp-m_lStartimestamp);
	}
	LeaveCriticalSection(&m_cs);
	return true;
}

//写入aac数据，只有raw数据
bool EasyRtmp::WriteAACFrame(byte*data,int len,long timestamp)
{	
	EnterCriticalSection(&m_cs);
	if (m_lStartimestamp > 0)
	{
		memset(m_pTempbuff,0,1024*32);
		int datalen=len+2;
		m_pTempbuff[0]=(char)0xaf;
		m_pTempbuff[1]=0x01;
		memcpy(m_pTempbuff+2,data,len);
		Send(m_pTempbuff,datalen,8,timestamp-m_lStartimestamp);
	}
	LeaveCriticalSection(&m_cs);
	return true;
}

bool EasyRtmp::IsLinked()
{
	if (m_rtmp==NULL)
	{
		return false;
	}
	return RTMP_IsConnected(m_rtmp);
}

bool EasyRtmp::WriteAudioAdst(unsigned char*buff,int length,int timestamp, BOOL bLocalAudio)
{
	if (timestamp==0||(!IsLinked()))
	{
		return true;
	}
	if (m_bwriteaudiinfo==0)
	{
		//unsigned char info[2]={0x12,0x10};
		//0x11,0x90
		//unsigned char info[2]={0x11,0x90};

		BYTE ucDecInfoBuff[2];
		if (bLocalAudio)
		{
			ucDecInfoBuff[0] =  0x12;  //44100采样率
			ucDecInfoBuff[1] =  0x10;  //44100采样率
			// 00010 | 0100 | 0010 | 000 == 44100 2
			// 00010 | 1000 | 0010 | 000 == 16000 2

		}
		else//KV
		{
			ucDecInfoBuff[0] =  0x11;  //48000采样率
			ucDecInfoBuff[1] =  0x90;  //48000采样率
		}
		WriteAACInfo(ucDecInfoBuff,2,0);
	}
	if (m_bwriteaudiinfo == 1)//音视频同步
	{	
		WriteAACFrame(buff+7,length-7,timestamp);//如果编码带有adst头，则需要去掉头7个字节
	}

	return true;
}

bool EasyRtmp::WriteVideoH264(unsigned char*buff,int length,int timestamp,bool bKeyFrame)
{
//	return true;
	if (timestamp==0||(!IsLinked()))
	{
		return true;
	}
	int inlen=length;
	unsigned char*pin=buff;
	int outlen=0;
	unsigned char*pout=NULL;
	bool bend;
	int datalen=0;

	BOOL bSPSOrPPS = FALSE;
	do 
	{
		pout=FindNal(pin,inlen,outlen,bend);
		if(pout!=NULL)
		{
			//	TRACE("%x len:%d ",pout[0],outlen);
			if (pout[0]==0x67 || pout[0]==0x27)
			{
// 				m_psps=pout;
// 				m_spslen=outlen;
				if(m_bwritevideoinfo==false)
				{
					m_psps = new unsigned char[outlen];
					memcpy(m_psps, pout, outlen);
					m_spslen=outlen;
				}
				bSPSOrPPS = TRUE;
			}
			if (pout[0]==0x68 || pout[0]==0x28)
			{
// 				m_ppps=pout;
// 				m_ppslen=outlen;
				if(m_bwritevideoinfo==false)
				{
					m_ppps = new unsigned char[outlen];
					memcpy(m_ppps, pout, outlen);
					m_ppslen = outlen;
				}
				bSPSOrPPS = TRUE;
			}
			inlen=inlen-outlen-(pout-pin);
			pin=pout+outlen;
		}
	} while (bend!=true);

	if (m_bwritevideoinfo==0&&m_ppps&&m_psps)
	{
		WriteH264SPSandPPS(m_psps,m_spslen,m_ppps,m_ppslen,0);
	}
	if (bKeyFrame)
	{
		outlen = length - 4-m_spslen-3-m_ppslen;
		pout = buff + 4+m_spslen+3+m_ppslen;
	}
	else
	{
		outlen = length - 4;
		pout = buff + 4;

	}

	if (m_bwritevideoinfo==0||pout==NULL )
	{
		return 0;//获取sps pps失败
	}

	if(/*bSPSOrPPS*/pout[0]==0x67||pout[0]==0x68)
	{
		return 0;
	}

	//bool bIsKeyFrame = (m_psps!=NULL)	? 1:0;
	
	WriteH264Frame2(pout,outlen,timestamp,/*bIsKeyFrame*/bKeyFrame);

	return true;
}