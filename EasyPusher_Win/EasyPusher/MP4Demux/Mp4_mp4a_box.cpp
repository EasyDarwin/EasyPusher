// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_mp4a_box.h"
#include <iostream>
#include "read_write.h"

CMp4_mp4a_box::CMp4_mp4a_box(void)
{
}


CMp4_mp4a_box::~CMp4_mp4a_box(void)
{
}
CMp4_mp4a_box * CMp4_mp4a_box::mp4_read_mp4a_box(FILE*f, int size)  //level 9
{
	CMp4_mp4a_box *ptr= new CMp4_mp4a_box;

	ptr->size= size;
	ptr->type= (('m'<<24)|('p'<<16)|('4'<<8)|('a'));
	int nCount = sizeof(ptr->reserved_format);
	fread(ptr->reserved_format, nCount, 1, f);  //33
	ptr->data_reference_index = read_uint16_lit(f);
	ptr->reserved[0] = read_uint32_lit(f); 
	ptr->reserved[1] = read_uint32_lit(f); 
	ptr->channelcount = read_uint16_lit(f); 
	ptr->samplesize= read_uint16_lit(f); 
	ptr->pre_defined= read_uint16_lit(f); 
	ptr->samplerate= read_uint32_lit(f); 

	printf("\t\t\t\t\t\t+mp4a\n");
	printf("\t\t\t\tsamplesize=%d\n", ptr->samplesize);
	printf("\t\t\t\tchannelcount=%d\n", ptr->channelcount);
	printf("\t\t\t\tsamplerate=%d\n", ptr->samplerate);
	return ptr;
}