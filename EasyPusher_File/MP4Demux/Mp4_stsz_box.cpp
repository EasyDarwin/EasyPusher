#include "Mp4_stsz_box.h"
#include "read_write.h"

CMp4_stsz_box::CMp4_stsz_box(void)
{
}


CMp4_stsz_box::~CMp4_stsz_box(void)
{
}

CMp4_stsz_box CMp4_stsz_box::mp4_read_stsz_box(FILE *f, int size)  //level 8   sample-size 帧的大小
{
	CMp4_stsz_box box_sz;
    printf("\t\t\t\t\t+%s\n", "stsz");
    box_sz.size= size;
	box_sz.type= 's'|'t'<<8|'s'<<16|'z'<<24;
    box_sz.version= read_uint8(f);
    fread(box_sz.flags, sizeof(box_sz.flags), 1, f);
	box_sz.samples_size_intotal = read_uint32_lit(f);/*全部sample 的数目。如果所有的sample 有相同的长度，这个字段就是这个值
												  。否则，这个字段的值就是0。那些长度存在sample size 表中*/
    
    if(box_sz.samples_size_intotal == 0){
        box_sz.table_size = read_uint32_lit(f);//sample size 的数目
        box_sz.sample_size_table = new uint32_t[box_sz.table_size];
        // printf("\t\t\t\t\t\tflags: 0x%x\n",
        //        box_sz.flags[0]|box_sz.flags[1]|box_sz.flags[2]);
//         printf("all samples amount: %u\n", box_sz.samples_size_intotal);
        printf("sample table size: %u\n", box_sz.table_size);
        
        printf("smple_size_table:\n");
		int s=0;
		unsigned int cur_pos=_ftelli64(f);
        for(int i = 0; i < box_sz.table_size; ++i){
			_fseeki64(f,cur_pos,SEEK_SET);
            box_sz.sample_size_table[i] = read_uint32_lit(f);
            if (i<300)
			{	
				printf("%8u ", box_sz.sample_size_table[i]);
				if((i+1) % 10 == 0) printf("\n");
            }
			s+=box_sz.sample_size_table[i];
			cur_pos+=4;
        }
		std::cout<<"size="<<s<<std::endl;
        printf("\n");
    }
    return box_sz;
}