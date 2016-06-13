// AACEncoderManager.h: interface for the CAACEncoderManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FAACENCODER_H)
#define FAACENCODER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <faac.h>
#pragma comment(lib,"libfaac.lib")

class FAACEncoder  
{
private:
	long m_nSampleRate;  // 采样率
    int m_nChannels;         // 声道数
    int m_nBits;      // 单样本位数
    unsigned long m_nInputSamples;	//最大输入样本数
    unsigned long m_nMaxOutputBytes;	//最大输出字节

	faacEncHandle m_hfaac;
    faacEncConfigurationPtr m_pfaacconf; 

	unsigned char* m_pfaacbuffer;

	unsigned char*m_pfaacinfobuffer;
	unsigned long m_nfaacinfosize;

public:
	FAACEncoder();
	virtual ~FAACEncoder();
	int Clean();
	int Init(int nSampleRate, int nChannels);
	unsigned char* Encoder(unsigned char*indata,int inlen,int &outlen);
	bool IsWorking(void);
	bool GetInfo(unsigned char*data,int&len);
};

#endif // !defined(AFX_AACENCODERMANAGER_H__CB1D102B_73A8_4C16_9DBA_4F3874CF446D__INCLUDED_)
