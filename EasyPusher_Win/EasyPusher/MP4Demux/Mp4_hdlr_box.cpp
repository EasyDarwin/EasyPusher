// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_hdlr_box.h"
#include "read_write.h"

CMp4_hdlr_box::CMp4_hdlr_box(void)
{
}


CMp4_hdlr_box::~CMp4_hdlr_box(void)
{
}

CMp4_hdlr_box CMp4_hdlr_box::mp4_read_hdlr_box(FILE *f, int size)
{
    printf("\t\t\t%s\n", "+hdlr");
    CMp4_hdlr_box box;
   
	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;
    
    box.size            = size;
    box.type            = 'h'|'d'<<8|'l'<<16|'r'<<24;
    box.version         = read_uint8(f);
    fread(&box.flags, sizeof(box.flags), 1, f);
    box.pre_defined     = read_uint32_lit(f);
//     fread(&box.handler_type, sizeof(box.handler_type), 1, f);
//     box.handler_type[4] = 0;
	//unsigned char p1[5];
	
	box.handler_type= Read4ByteString(f);

    fread(&box.reserved, sizeof(box.reserved), 1, f);
    fread(&inner_size, 1, 1, f);
    for(int i = 0; i < inner_size; ++i)
       {
		   char na;
		   fread(&na, sizeof(char), 1, f);
		   box.name.push_back(na);
		}
	std::cout<<"box.handler_type:"<<box.handler_type<<std::endl;
    return box;

}
