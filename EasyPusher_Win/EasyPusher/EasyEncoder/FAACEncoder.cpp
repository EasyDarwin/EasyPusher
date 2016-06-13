
// AACEncoderManager.cpp: implementation of the CAACEncoderManager class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "FAACEncoder.h"


FAACEncoder::FAACEncoder()
{
	m_nSampleRate=16000;//44100;  // 采样率
    m_nChannels=2;         // 声道数
    m_nBits=16;      // 单样本位数
    m_nInputSamples=2048;	//最大输入样本数
    m_nMaxOutputBytes=0;	//最大输出字节
	
	m_hfaac=NULL;
    m_pfaacconf=NULL; 

	m_pfaacbuffer=NULL;
	m_pfaacbuffer=(unsigned char*)malloc(4096);

	m_pfaacinfobuffer=NULL;//(unsigned char*)malloc(100);
}

FAACEncoder::~FAACEncoder()
{
	Clean();
	if(m_pfaacbuffer)
	{
		free(m_pfaacbuffer);
		m_pfaacbuffer=NULL;
	}
}

unsigned char* FAACEncoder::Encoder(unsigned char*indata,int inlen,int &outlen)
{
	if(m_hfaac!=NULL)
	{
		int nInputSamples = inlen/2;
		outlen = faacEncEncode(m_hfaac, (int32_t*)indata, nInputSamples, m_pfaacbuffer, 4096);
	}else
	{
		outlen=-1;
	}
	if (outlen>0)
	{
		return m_pfaacbuffer;
	}else
	{
		return NULL;
	}
	return NULL;
}
bool FAACEncoder::GetInfo(unsigned char*data,int&len)
{
	memcpy(data,m_pfaacinfobuffer,m_nfaacinfosize);
	len=m_nfaacinfosize;
	return true;
}

int FAACEncoder::Init(int nSampleRate, int nChannels)
{
	if (IsWorking())
	{
		return 0;
	}
	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	m_hfaac = faacEncOpen(m_nSampleRate, m_nChannels, &m_nInputSamples, &m_nMaxOutputBytes);
	if (m_hfaac!=NULL)
	{	
		m_pfaacconf = faacEncGetCurrentConfiguration(m_hfaac);  
		m_pfaacconf->inputFormat = FAAC_INPUT_16BIT;
		m_pfaacconf->outputFormat=1;//0raw 1adst
		m_pfaacconf->useTns=true;
		m_pfaacconf->useLfe=false;
		m_pfaacconf->aacObjectType=LOW;
		m_pfaacconf->shortctl=SHORTCTL_NORMAL;
		m_pfaacconf->quantqual=100;
		m_pfaacconf->bandWidth=0;
		m_pfaacconf->bitRate=0;
		faacEncSetConfiguration(m_hfaac,m_pfaacconf);

		faacEncGetDecoderSpecificInfo(m_hfaac,&m_pfaacinfobuffer,&m_nfaacinfosize);
	}
	return 0;
}

int FAACEncoder::Clean()
{
	if (m_hfaac!=NULL)
	{
		faacEncEncode(m_hfaac, NULL, 0, m_pfaacbuffer, 4096);
		faacEncClose(m_hfaac);
		m_hfaac=NULL;

	}
	return 0;
}

bool FAACEncoder::IsWorking(void)
{
	return m_hfaac!=NULL;
}

