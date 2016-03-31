#include "Mp4_mdhd_box.h"
#include "read_write.h"

CMp4_mdhd_box::CMp4_mdhd_box(void)
{
}


CMp4_mdhd_box::~CMp4_mdhd_box(void)
{
}

/*final box
 */
CMp4_mdhd_box CMp4_mdhd_box::mp4_read_mdhd_box(FILE *f, int size)
{
   // printf("\t\t\t+mdhd\n");
    CMp4_mdhd_box box;
	std::cout<<"\t\t\t\t\tmdhd"<<std::endl;
	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;
        
    box.size              = size;
    box.type              = 'm' | 'd'<<8 | 'h'<<16 | 'd'<<24;
    box.version           = read_uint8(f);
    fread(&box.flags, sizeof(box.flags), 1, f);
    box.creation_time     = read_uint32_lit(f);
    box.modification_time = read_uint32_lit(f);
    box.timescale         = read_uint32_lit(f);
    box.duration          = read_uint32_lit(f);
    box.language          = read_uint16_big(f);
    box.pre_defined       = read_uint16_big(f);


	printf("\t\t\t\t\tflags: 0x%x\n", box.flags[2]
            | box.flags[1] | box.flags[0]);
     printf("\t\t\t\t\tcreation time: %u\n", box.creation_time);
     printf("\t\t\t\t\tmodifaction time: %u\n",
            box.modification_time);
    printf("\t\t\t\ttimescale: %u\n", box.timescale);
    printf("\t\t\t\tduration: %u\n", box.duration);
    printf("\t\t\t\tlanguage: %u\n", box.language);
     printf("\t\t\t\tpre-defined: %u\n", box.pre_defined);
    
    printf("\n");

    return box;
}