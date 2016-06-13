// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_stco_box.h"
#include "read_write.h"
#include "Public_func.h"
#include "Define_head.h"

CMp4_stco_box::CMp4_stco_box(void)
{
}


CMp4_stco_box::~CMp4_stco_box(void)
{
}

CMp4_stco_box CMp4_stco_box::mp4_read_stco_box(FILE *f, int size)//定义了每个thunk在媒体流中的位置
{
	CMp4_stco_box box_co;
	printf("\t\t\t\t\t+%s\n", "stco");  
	box_co.size=size;
	box_co.type= 's'|'t'<<8|'c'<<16|'o'<<24;
	box_co.version= read_uint8(f);
	fread(box_co.flags, sizeof(box_co.flags), 1, f);
	box_co.chunk_offset_amount= read_uint32_lit(f);
	printf("chunk offest amount: %u\n",
		box_co.chunk_offset_amount);
	printf("chunk offset:\n");
	box_co.chunk_offset_from_file_begin = new uint32_t[box_co.chunk_offset_amount];
	unsigned int cur_pos=_ftelli64(f);
	for(int i = 0 ; i < box_co.chunk_offset_amount; i++){
		_fseeki64(f,cur_pos,SEEK_SET);
		box_co.chunk_offset_from_file_begin[i] = read_uint32_lit(f);
		cur_pos+=4;
	}
	for (int i=0;i<30;i++)
	{
		std::cout<<box_co.chunk_offset_from_file_begin[i]<<"  ";
		if((i+1) % 10 == 0) printf("\n");
	}
	return box_co;
}

