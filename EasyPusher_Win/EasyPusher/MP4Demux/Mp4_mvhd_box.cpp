// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_mvhd_box.h"
#include "read_write.h"

CMp4_mvhd_box::CMp4_mvhd_box(void)
{
}


CMp4_mvhd_box::~CMp4_mvhd_box(void)
{
}

/*final box
 */
CMp4_mvhd_box CMp4_mvhd_box::mp4_read_mvhd_box(FILE *f, int size)  //level 3
{
	printf("\t+%s\n", "mvhd");
	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;

    int level_4_box_size    = 0;

    CMp4_mvhd_box mvhd;
    
    mvhd.size               = size;
    mvhd.type               = ('m' | 'v'<<8 | 'h'<<16 | 'd'<<24);
    mvhd.version            = read_uint8(f);                 //1
    fread(mvhd.flags, sizeof(mvhd.flags), 1, f);             //3
    mvhd.creation_time      = read_uint32_lit(f);            //4
    mvhd.modification_time  = read_uint32_lit(f);            //4
    mvhd.timescale          = read_uint32_lit(f);            //4
    mvhd.duration           = read_uint32_lit(f);            //4
    mvhd.rate               = read_uint32_lit(f);            //4
    mvhd.volume             = read_uint16_big(f);            //2
    fread(&mvhd.reserved1, sizeof(mvhd.reserved1), 1, f);    //2
    fread(&mvhd.reserved2, sizeof(mvhd.reserved2), 1, f);    //8
    fread(mvhd.matrix, sizeof(mvhd.matrix), 1, f);           //36
    fread(mvhd.pre_defined, sizeof(mvhd.pre_defined), 1, f); //24
    mvhd.next_track_id      = read_uint32_lit(f);            //4
	//int cur=ftell(f);
    
	printf("\t\ttimescale: %u\n", mvhd.timescale);
	printf("\t\tduration: %u\n", mvhd.duration);
	printf("\t\trate: %u\n", mvhd.rate );
	printf("\t\tvolume: 0x%x\n", mvhd.volume);

	printf("\t\tmatrix:\n");
	for(int i = 0; i < 3; ++i){
	printf("\t\t");
	for(int j = 0; j < 3; ++j){
	printf(" %8u ",mvhd.matrix[i*3+j]);
	}
	printf("\n");
	}

	printf("\t\tnext track id: %u\n", mvhd.next_track_id);

	printf("\n");
	return mvhd;
}
