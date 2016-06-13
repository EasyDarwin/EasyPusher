// H264EncoderManager.cpp: implementation of the CH264EncoderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "NetDirectedDev.h"
#include "H264EncoderManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH264EncoderManager::CH264EncoderManager()
{
//	m_nCount=0;	
	int i;
	for(i=0;i<MAX_H264ENCODER_NUM;i++)
		m_H264Encoder[i]=NULL;
//	InitializeCriticalSection(&m_hcritical_section);
}

CH264EncoderManager::~CH264EncoderManager()
{
	Clean();
//	DeleteCriticalSection(&m_hcritical_section);
}

/*
int scrwidth,int scrheight
*/
void CH264EncoderManager::Init(int nIndex, int width, int height, int fps,int keyframes,int bitrate,int level,int qp,int nUseQP)
{
//	EnterCriticalSection(&m_hcritical_section);

	if (m_H264Encoder[nIndex])
	{
		m_H264Encoder[nIndex]->Clean();
		delete m_H264Encoder[nIndex];
		m_H264Encoder[nIndex]=NULL;
	}
	m_H264Encoder[nIndex]=new CH264Encoder;
	m_H264Encoder[nIndex]->Init(width,height,fps,keyframes,bitrate,level,qp,nUseQP);
	m_H264Encoder[nIndex]->SetEncoderIndex(nIndex);


//	LeaveCriticalSection(&m_hcritical_section);

}

void CH264EncoderManager::Clean()
{
//	EnterCriticalSection(&m_hcritical_section);
	int i;
	for(i=0;i<MAX_H264ENCODER_NUM;i++)
	{
		if(m_H264Encoder[i])
		{
			m_H264Encoder[i]->Clean();
			delete m_H264Encoder[i];
			m_H264Encoder[i]=NULL;
		}
	}
//	LeaveCriticalSection(&m_hcritical_section);
}

byte* CH264EncoderManager::Encoder(int nIndex, byte *indata, int inlen, int &outlen, bool& bIsKeyFrame)
{
//	EnterCriticalSection(&m_hcritical_section);
	if(nIndex<0||nIndex>=MAX_H264ENCODER_NUM)
	{
		outlen=0;
		return NULL;
	}
	if (m_H264Encoder[nIndex]==NULL||!m_H264Encoder[nIndex]->IsWorking())
	{
		outlen=0;
		return NULL;
	}
//	LeaveCriticalSection(&m_hcritical_section);
//	TRACE("H264Encoder:%d\n",nIndex);
	return	m_H264Encoder[nIndex]->Encoder(indata,inlen,outlen, bIsKeyFrame);
}

bool CH264EncoderManager::IsWorking(int nIndex)
{
	if (m_H264Encoder[nIndex]==NULL)
	{
		return false;
	}
	else
	{
		return m_H264Encoder[nIndex]->IsWorking();
	}
	return	false;
}

void  CH264EncoderManager::GetSPSAndPPS(int nIndex,byte*sps,long&spslen,byte*pps,long&ppslen)
{
	if (m_H264Encoder[nIndex]==NULL)
	{
		spslen=0;
		ppslen=0;
		return;
	}
	m_H264Encoder[nIndex]->GetSPSAndPPS(sps,spslen,pps,ppslen);
}