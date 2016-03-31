#include "Mp4_tkhd_box.h"
#include "read_write.h"

CMp4_tkhd_box::CMp4_tkhd_box(void)
{
}


CMp4_tkhd_box::~CMp4_tkhd_box(void)
{
}

/*final box
 */
CMp4_tkhd_box CMp4_tkhd_box::mp4_read_tkhd_box(FILE *f, int size)  //level 4
{
	printf("\t\t+%s\n", "tkhd"); 

	//VARIABLES;
	CMp4_tkhd_box box;

	box.size              = size;
	box.type= 't' | 'k'<<8 | 'h'<<16 | 'd'<<24;
	box.version           = read_uint8(f);
	fread(&box.flags, sizeof(box.flags), 1, f);
	box.creation_time     = read_uint32_lit(f);
	box.modification_time = read_uint32_lit(f);
	box.track_ID          = read_uint32_lit(f);
	//int cur=ftell(f);
	box.reserved1         = read_uint32_lit(f);
	box.duration          = read_uint32_lit(f);
	fread(&box.reserved2, sizeof(box.reserved2), 1, f);
	box.layer             = read_uint16_big(f);
	box.alternate_group   = read_uint16_big(f);
	box.volume            = read_uint16_big(f);
	box.reserved3         = read_uint16_big(f);
	fread(&box.matrix, sizeof(box.matrix), 1, f);
	box.width             = read_uint32_lit(f);
	box.height            = read_uint32_lit(f);

	std::cout<<"\t\t\tversion: "<<box.version<<std::endl;
	printf("\t\t\tflags: 0x%4x\n", box.flags[2]
	| box.flags[1] | box.flags[0]);
	printf("\t\t\tcreation time: %u\n", box.creation_time);
	printf("\t\t\tmodifaction time: %u\n",
		box.modification_time);
	printf("\t\t\ttrack id: %u\n", box.track_ID);
	printf("\t\t\treserved1: 0x%x\n", box.reserved1);
	printf("\t\t\tduration: %d\n",box.duration);
	printf("\t\t\treserved2: 0x%x, 0x%x\n",
		box.reserved2[0], box.reserved2[1]);
	printf("\t\t\tlayer: %d\n",box.layer);
	printf("\t\t\talternate group: %d\n", box.alternate_group);
	printf("\t\t\tvolume: 0x%x\n", box.volume);
	printf("\t\t\treserved3: 0x%x\n", box.reserved3);

	printf("\t\t\tmatrix:\n");
	for(int i = 0; i < 3; ++i){
		printf("\t\t\t");
		for(int j = 0; j < 3; ++j){
			printf(" %8u ", box.matrix[i*3+j]);
		}
		printf("\n");
	}
	////////////////////////////////////////////@a mark still don't know
	printf("\t\t\twidth: %u\n",box.width);
	printf("\t\t\theight: [%u].[%u]\n",
		box.height & 0xffff0000 >> 16,
		box.height & 0xffff);

	printf("\n");


	return box;
}