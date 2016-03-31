#include "Mp4_minf_box.h"
#include "read_write.h"
#include <string>

CMp4_minf_box::CMp4_minf_box(void)
{
}


CMp4_minf_box::~CMp4_minf_box(void)
{
}

/*container box
  (vmhd, smhd, hmhd, nmhd)
*/
CMp4_minf_box CMp4_minf_box::mp4_read_minf_box(FILE *f, int size)
{
    CMp4_minf_box box;

    printf("\t\t\t+%s\n", "minf");
	box.size               = size;
	box.type               = ('m' | 'i'<<8 | 'n'<<16 | 'f'<<24);

	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;

    int level_5_box_size = 0;
    unsigned int cur_pos= _ftelli64(f) ;
    do{
        _fseeki64(f, cur_pos, SEEK_SET);
        level_5_box_size = read_uint32_lit(f);
        fread(p, 4, 1, f);
        p[4] = 0;
        std::string name = (char*)p;
       if(name == "stbl") {
			CMp4_stbl_box stbl;
            box.stbl = stbl.mp4_read_stbl_box(f, level_5_box_size);
		}
        cur_pos    += level_5_box_size;
        inner_size += level_5_box_size;
    } while(inner_size+8 < size);
    printf("\n");

    return box;
}

void CMp4_minf_box::write_minf( FILE * fin, FILE * fout, uint32_t minf_size ,CMp4_minf_box minf,std::string str) 
{
	unsigned int cur=_ftelli64(fin);
	uint32_t cur_size=0;
	write_uint32_lit(fout,minf.size);
	fwrite(&minf.type,sizeof(minf.type),1,fout);
	do 
	{
		uint32_t buf_size  = read_uint32_lit(fin);
		unsigned char p1[5];
		fread(p1, 4, 1, fin);
		p1[4]= 0;
		std::string minf_name= (char*)p1;
		if (minf_name=="stbl")
		{
			CMp4_stbl_box stbl;
			stbl.write_stbl(fin,fout,buf_size,minf.stbl,str);
		}
		else
		{
			write_uint32_lit(fout,buf_size);
			fwrite(&p1,4,1,fout);
			int buf=buf_size-8;
			char *ptr=new char [buf];
			fread(ptr, buf, 1, fin);
			fwrite(ptr, buf, 1, fout);
			delete [] ptr;
		}
		cur_size+=buf_size+8;
		cur+=buf_size;
	} while (cur_size<minf_size);

}
