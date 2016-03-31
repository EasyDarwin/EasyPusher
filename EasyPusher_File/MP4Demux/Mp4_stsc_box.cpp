#include "Mp4_stsc_box.h"
#include "read_write.h"

CMp4_stsc_box::CMp4_stsc_box(void)
{
}


CMp4_stsc_box::~CMp4_stsc_box(void)
{
}

CMp4_stsc_box CMp4_stsc_box::mp4_read_stsc_box(FILE *f, int size)  //level 8
{
	CMp4_stsc_box box_sc;
	box_sc.size=size;
	box_sc.type= 's'|'t'<<8|'s'<<16|'c'<<24;
    printf("\t\t\t\t\t+%s\n", "stsc");
	box_sc.version    = read_uint8(f);
    fread(&box_sc.flags, sizeof(box_sc.flags), 1, f);
    box_sc.map_amount = read_uint32_lit(f);//sample-to-chunk 的数目
   
    printf("map-amount: %u\n", box_sc.map_amount);
    
    box_sc.scmap= new mp4_list_t[box_sc.map_amount];
    printf("first chunk:\tsamples-per-chunk:\tsample-description-ID\n");
	/*这个表类似于行程编码，第一个first chunk 减去第二个first chunk 就是一共有多少个trunk
	包含相同的sample 数目，这样通过不断的叠加，就可以得到一共有多少个chunk，每个chunk 包含多
	少个sample，以及每个chunk 对应的description。*/
	unsigned int cur_pos=_ftelli64(f);
	for(int i = 0; i < box_sc.map_amount; ++i){
		_fseeki64(f,cur_pos,SEEK_SET);
        box_sc.scmap[i].first_chunk_num = read_uint32_lit(f);
        box_sc.scmap[i].sample_amount_in_cur_table = read_uint32_lit(f);
        box_sc.scmap[i].sample_description_id = read_uint32_lit(f);
		cur_pos+=12;
    }
	for (int i=0;i<10;i++)
	{
         printf("%13d", box_sc.scmap[i].first_chunk_num);
         printf("\t%13d", box_sc.scmap[i].sample_amount_in_cur_table);
         printf("\t%13d\n", box_sc.scmap[i].sample_description_id);
	}
	std::cout<<"stsc:chunk_num="<<box_sc.scmap[box_sc.map_amount-1].first_chunk_num<<std::endl;
    return box_sc;
}