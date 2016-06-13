////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//    Author      :    Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//
//    File description : 
//    Name    : convert.h
//    Details : Conversion routine from RGB24 to YUV420 & YUV420 to RGB24.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined _CONVERT_H
#define _CONVERT_H

#include "DllManager.h"
#include<stdio.h>

class DLL_EXPORT RGBYUVConvert
{
public:
    // Conversion from RGB24 to YUV420
    static void InitLookupTable();
    static int  ConvertRGB2YUV(int w,int h,unsigned char *rgbdata, unsigned char *yuv);


    // Conversion from YUV420 to RGB24
    static void InitConvertTable();
    static void ConvertYUV2RGB(unsigned char *src,unsigned char *dst, int width,int height);
    static void ConvertYUV2RGB(unsigned char *src0,unsigned char *src1,
        unsigned char *src2,unsigned char *dst_ori, int width,int height);

};

#endif
