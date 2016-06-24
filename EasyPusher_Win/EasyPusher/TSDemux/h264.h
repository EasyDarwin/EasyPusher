
/*

 Copyright (C) 2009 Anton Burdinuk

 clark15b@gmail.com

*/

#ifndef __H264_H
#define __H264_H

#include "common.h"

namespace h264
{
    class counter
    {
    private:
        u_int32_t ctx;
        u_int64_t frame_num;                            // JVT NAL (h.264) frame counter
    public:
        counter(void):ctx(0),frame_num(0) {}

        void parse(const char* p,int l)
        {
            for(int i=0;i<l;i++)
            {
                ctx=(ctx<<8)+((unsigned char*)p)[i];
                    if((ctx&0xffffff1f)==0x00000109)    // NAL access unit
                        frame_num++;
            }
        }

        u_int64_t get_frame_num(void) const { return frame_num; }

        void reset(void)
        {
            ctx=0;
            frame_num=0;
        }
    };
}

#endif
