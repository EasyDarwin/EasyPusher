#include "Mp4_stbl_box.h"
#include "read_write.h"

CMp4_stbl_box::CMp4_stbl_box(void)
{
}


CMp4_stbl_box::~CMp4_stbl_box(void)
{
}

/*container box
  (stsd, stts, stsz|stz2, stsc, stco|co64, ctts, stss)
*/
CMp4_stbl_box CMp4_stbl_box::mp4_read_stbl_box(FILE *f, int size)
{
    printf("\t\t\t\t+stbl\n");
    CMp4_stbl_box box;
	box.size= size;
	box.type= 's' | 't'<<8 | 'b'<<16 | 'l'<<24;
	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;
    int box_size= 0;
    unsigned int cur_pos= _ftelli64(f) ;
    do{
        _fseeki64(f, cur_pos, SEEK_SET);
        box_size= read_uint32_lit(f);
        fread(p, 4, 1, f);
        p[4]= 0;
        std::string name  = (char*)p;
        if(name == "stsd") {
			CMp4_stsd_box stsd;
			box.stsd= stsd.mp4_read_stsd_box(f, box_size);			
         /*   根据不同的编码方案和存储数据的文件数目，
            每个media可以有一个到多个sample description。
            sample-to-chunk atom通过这个索引表，
            找到合适medai中每个sample的description。
            */
        } else if(name == "stts"){
			CMp4_stts_box stts;
			box.stts= stts.mp4_read_stts_box(f, box_size);			
			/*   Time-to-sample atoms存储了media sample的duration 信息，
			提供了时间对具体data sample的映射方法，通过这个atom，
			你可以找到任何时间的sample，类型是'stts'。
			*/
        } else if(name == "stss"){
			CMp4_stss_box stss;
			box.stss= stss.mp4_read_stss_box(f, box_size);			
           // sync sample atom确定media中的关键帧。
        } else if(name == "stsc"){
			CMp4_stsc_box stsc;
			box.stsc= stsc.mp4_read_stsc_box(f, box_size);			
        } else if(name == "stsz"){
			CMp4_stsz_box stsz;
			box.stsz= stsz.mp4_read_stsz_box(f, box_size);			
        } else if(name == "stco"){
			CMp4_stco_box stco;
			box.stco= stco.mp4_read_stco_box(f, box_size);			
        } 
        cur_pos    += box_size;
        inner_size += box_size;
    } while(inner_size+8 < size&&!feof(f));
    //printf("\n");
    return box;
}

void CMp4_stbl_box::write_stbl(FILE *fin, FILE * fout, uint32_t stbl_size, CMp4_stbl_box stbl ,std::string str) 
{
	write_uint32_lit(fout,stbl.size);
	fwrite(&stbl.type,sizeof(stbl.type),1,fout);
	/*写stsd*/
	unsigned int cur=_ftelli64(fin);
	int stsd_size  = read_uint32_lit(fin);
	int buf_size=stsd_size-4;
	write_uint32_lit(fout,stsd_size);
	char *ptr=new char [buf_size];
	fread(ptr, buf_size, 1, fin);
	fwrite(ptr, buf_size, 1, fout);
	delete [] ptr;

	/*写stts*/
	write_uint32_lit(fout,stbl.stts.size);
	fwrite(&stbl.stts.type,sizeof(stbl.stts.type),1,fout);
	write_uint8(fout,stbl.stts.version);
	fwrite(&stbl.stts.flags,sizeof(stbl.stts.flags),1,fout);
	write_uint32_lit(fout,stbl.stts.number_of_entries);
	for(int i=0;i<stbl.stts.number_of_entries;i++)
	{
		write_uint32_lit(fout,stbl.stts.sample_count[i]);
		write_uint32_lit(fout,stbl.stts.sample_duration[i]);
	}
	/*写stss*/
	if (str=="vide")
	{
		write_uint32_lit(fout,stbl.stss.size);
		fwrite(&stbl.stss.type,sizeof(stbl.stss.type),1,fout);
		write_uint8(fout,stbl.stss.version);
		fwrite(&stbl.stss.flags,sizeof(stbl.stss.flags),1,fout);
		write_uint32_lit(fout,stbl.stss.number_of_entries);
		for (int i=0;i<stbl.stss.number_of_entries;i++)
		{
			write_uint32_lit(fout,stbl.stss.sync_sample_table[i]);
		}
	}

	/*写stsc*/
	write_uint32_lit(fout,stbl.stsc.size);
	fwrite(&stbl.stsc.type,sizeof(stbl.stsc.type),1,fout);
	write_uint8(fout,stbl.stsc.version);
	fwrite(&stbl.stsc.flags,sizeof(stbl.stsc.flags),1,fout);
	write_uint32_lit(fout,stbl.stsc.map_amount);
	for (int i=0;i<stbl.stsc.map_amount;i++)
	{
		write_uint32_lit(fout,stbl.stsc.scmap[i].first_chunk_num);
		write_uint32_lit(fout,stbl.stsc.scmap[i].sample_amount_in_cur_table);
		write_uint32_lit(fout,stbl.stsc.scmap[i].sample_description_id);
	}
	/*写stsz*/
	write_uint32_lit(fout,stbl.stsz.size);
	fwrite(&stbl.stsz.type,sizeof(stbl.stsz.type),1,fout);
	write_uint8(fout,stbl.stsz.version);
	fwrite(&stbl.stsz.flags,sizeof(stbl.stsz.flags),1,fout);
	write_uint32_lit(fout,stbl.stsz.samples_size_intotal);
	if (stbl.stsz.samples_size_intotal==0)
	{
		write_uint32_lit(fout,stbl.stsz.table_size);
		for (int i=0;i<stbl.stsz.table_size;i++)
		{
			write_uint32_lit(fout,stbl.stsz.sample_size_table[i]);
		}
	}
	/*写stco*/
	write_uint32_lit(fout,stbl.stco.size);
	fwrite(&stbl.stco.type,sizeof(stbl.stco.type),1,fout);
	write_uint8(fout,stbl.stco.version);
	fwrite(&stbl.stco.flags,sizeof(stbl.stco.flags),1,fout);
	write_uint32_lit(fout,stbl.stco.chunk_offset_amount);
	for (int i=0;i<stbl.stco.chunk_offset_amount;i++)
	{
		write_uint32_lit(fout,stbl.stco.chunk_offset_from_file_begin[i]);
	}

}

