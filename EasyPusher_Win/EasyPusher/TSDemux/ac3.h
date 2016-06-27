#ifndef __AC3_H
#define __AC3_H

#include "common.h"

namespace ac3
{
    class counter
    {
    private:
        u_int16_t st;
        u_int32_t ctx;
        u_int16_t skip;
        u_int64_t frame_num;
    public:
        counter(void):st(0),ctx(0),skip(0),frame_num(0) {}

        void parse(const char* p,int l)
        {
            static const u_int16_t frame_size_32khz[]=
            {
                96,96,120,120,144,144,168,168,192,192,240,240,288,288,336,336,384,384,480,480,576,576,672,672,768,768,960,
                960,1152,1152,1344,1344,1536,1536,1728,1728,1920,1920
            };
            static const u_int16_t frame_size_44khz[]=
            {
                69,70,87,88,104,105,121,122,139,140,174,175,208,209,243,244,278,279,348,349,417,418,487,488,557,558,696,
                697,835,836,975,976,1114,1115,1253,1254,1393,1394
            };
            static const u_int16_t frame_size_48khz[]=
            {
                64,64,80,80,96,96,112,112,128,128,160,160,192,192,224,224,256,256,320,320,384,384,448,448,512,512,640,640,
                768,768,896,896,1024,1024,1152,1152,1280,1280
            };

            for(int i=0;i<l;)
            {
                if(skip>0)
                {
                    int n=l-i;
                    if(n>skip)
                        n=skip;
                    i+=n;
                    skip-=n;

                    if(i>=l)
                        break;
                }


                ctx=(ctx<<8)+((unsigned char*)p)[i];

                switch(st)
                {
                case 0:             // wait 0x0b77 marker
                    if((ctx&0xffff0000)==0x0b770000)
                    {
                        st++;
                        frame_num++;
                    }
                    break;
                case 1:
                    st++;
                    break;
                case 2:
                    {
                        int frmsizecod=(ctx>>8)&0x3f;
                        if(frmsizecod>37)
                            frmsizecod=0;

                        int framesize=0;

                        switch((ctx>>14)&0x03)
                        {
                        case 0: framesize=frame_size_48khz[frmsizecod]; break;
                        case 1: framesize=frame_size_44khz[frmsizecod]; break;
                        case 2: framesize=frame_size_32khz[frmsizecod]; break;
                        }

                        skip=framesize*2-6;

                        st=0;
                        break;
                    }
                }

                i++;

            }
        }
        u_int64_t get_frame_num(void) const { return frame_num; }

        void reset(void)
        {
            st=0;
            ctx=0;
            skip=0;
            frame_num=0;
        }
    };
}

#endif
