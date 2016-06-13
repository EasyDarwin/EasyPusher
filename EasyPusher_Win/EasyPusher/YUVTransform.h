/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// Add by SwordTwelve

#pragma once

//YUV×ª»»º¯Êý
void YUY2toI420(int inWidth, int inHeight, unsigned char* pSrc, unsigned char* pDest);
void YUY2toYV12(int inWidth, int inHeight, unsigned char* pSrc, unsigned char* pDest);

// Table used for RGB to YUV420 conversion
void InitLookupTable();
// Convert from RGB24 to YUV420
int ConvertRGB2YUV(int w,int h,unsigned char *bmp,unsigned char *yuv);
//Initialize conversion table for YUV420 to RGB
void InitConvertTable();
// Convert from YUV420 to RGB24
void ConvertYUV2RGB(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,int width,int height);
