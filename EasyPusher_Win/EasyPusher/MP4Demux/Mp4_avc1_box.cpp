// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_avc1_box.h"
#include "read_write.h"


CMp4_avc1_box* CMp4_avc1_box::mp4_read_avc1_box(FILE *f, int size)  //level 9
{
	CMp4_avc1_box *ptr = new CMp4_avc1_box;

	ptr->size= size;
	ptr->type= (('a'<<24)|('v'<<16)|('c'<<8)|('1'));

	fread(ptr->reserved, sizeof(ptr->reserved), 1, f);  //6
	ptr->data_reference_index= read_uint16_lit(f);   //2
	ptr->pre_defined= read_uint16_big(f);   //2
	ptr->reserved1= read_uint16_big(f);   //2
	fread(ptr->pre_defined1, sizeof(ptr->pre_defined1), 1, f);  //3*4
	int cur=ftell(f);
	ptr->width= read_uint16_lit(f);   //2

	ptr->height= read_uint16_lit(f);   //2
	ptr->horiz_res= read_uint32_lit(f);   //4
	ptr->vert_res= read_uint32_lit(f);   //4
	ptr->reserved2= read_uint32_lit(f);   //4
	ptr->frames_count= read_uint16_lit(f);   //2
	fread(ptr->compressor_name, sizeof(ptr->compressor_name), 1, f);  //33
	ptr->bit_depth= read_uint8(f);        //1
	ptr->pre_defined2= read_uint16_big(f);   //2

	//avcC --AVCDecoderConfigurationRecord
	//     printf("-------------------------------------\n");
	// 	printf("data_reference_index:%x\n", ptr->data_reference_index);
	//     printf("width:%u\n", ptr->width);
	//     printf("height:%u\n", ptr->height);
	//     printf("frames_count:%x\n", ptr->frames_count);
	//     printf("bit_depth:%u\n", ptr->bit_depth);
	//     printf("pre_defined2: %x\n", ptr->pre_defined2);
	CMp4_avcC_box avcc;

	ptr->avcC = avcc.mp4_read_avcC_box(f);

	printf("-------------------------------------\n");
	return ptr;
}