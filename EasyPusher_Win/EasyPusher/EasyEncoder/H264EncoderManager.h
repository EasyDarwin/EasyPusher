// H264EncoderManager.h: interface for the CH264EncoderManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H264ENCODERMANAGER_H__EFBEFE23_C3C3_4AC4_AEAA_B4C853E7F74C__INCLUDED_)
#define AFX_H264ENCODERMANAGER_H__EFBEFE23_C3C3_4AC4_AEAA_B4C853E7F74C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_H264ENCODER_NUM 20
#include "H264Encoder.h"

class CH264EncoderManager  
{
private:
//	int m_nCount;
	CH264Encoder* m_H264Encoder[MAX_H264ENCODER_NUM];
//	CRITICAL_SECTION m_hcritical_section;	
public:
	bool	IsWorking(int nIndex);
	byte*	Encoder(int nIndex,byte *indata,int inlen,int &outlen, bool& bIsKeyFrame);
	void	Clean();
//	void	Init(int nIndex,int width,int height,int fps=25,int keyframes=50,int bitrate=0,int level=3,int qp=0,int scrwidth,int scrheight,int nUseQP);
	void	Init(int nIndex,int width,int height,int fps,int keyframes,int bitrate,int level,int qp,int nUseQP=0);
	void	GetSPSAndPPS(int nIndex,byte*sps,long&spslen,byte*pps,long&ppslen);
	CH264EncoderManager();
	virtual ~CH264EncoderManager();

};

#endif // !defined(AFX_H264ENCODERMANAGER_H__EFBEFE23_C3C3_4AC4_AEAA_B4C853E7F74C__INCLUDED_)
