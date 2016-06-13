// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_stss_box.h"
#include "read_write.h"

CMp4_stss_box::CMp4_stss_box(void)
{
}


CMp4_stss_box::~CMp4_stss_box(void)
{
}

CMp4_stss_box CMp4_stss_box::mp4_read_stss_box(FILE *f, int size)  //level 8  关键帧列表
{
	CMp4_stss_box box_ss;
	printf("\t\t\t\t\t+%s\n", "stss");
	box_ss.size= size;
	box_ss.type= 's'|'t'<<8|'s'<<16|'s'<<24;
	box_ss.version= read_uint8(f);
	fread(box_ss.flags, sizeof(box_ss.flags), 1, f);
	box_ss.number_of_entries = read_uint32_lit(f);//关键帧的数目

	printf("\t\t\t\t\t\t\tflags: %u\n",
		box_ss.flags[0]|box_ss.flags[1]|box_ss.flags[2]);
	printf("number of entries: %u\n",box_ss.number_of_entries);

	printf("entries:\n");
	box_ss.sync_sample_table = new uint32_t[box_ss.number_of_entries];
	unsigned int cur_pos=_ftelli64(f);
	for(int i =0; i < box_ss.number_of_entries; ++i){
		_fseeki64(f,cur_pos,SEEK_SET);
		box_ss.sync_sample_table[i] = read_uint32_lit(f);//关键帧的序号
	/*	if (i<10)*/
		//{
		printf("%6u ", box_ss.sync_sample_table[i]);
		if( (i)%10 == 0)printf("\n");
		//}

		cur_pos+=4;
	}
	printf("\n");

	return box_ss;
}