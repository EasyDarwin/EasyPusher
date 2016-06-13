// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_mdia_box.h"
#include "read_write.h"

CMp4_mdia_box::CMp4_mdia_box(void)
{
}


CMp4_mdia_box::~CMp4_mdia_box(void)
{
}

/*container box
  (mdhd, hdlr, minf)
*/
CMp4_mdia_box CMp4_mdia_box::mp4_read_mdia_box(FILE *f, int size)
{
    printf("\t\t+%s\n", "mdia");
    
    CMp4_mdia_box box;
    box.size= size;
    box.type= 'm' | 'd'<<8 | 'i'<<16 | 'a'<<24;

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
        if(name == "mdhd") {
			CMp4_mdhd_box mdhd;
            box.mdhd=mdhd.mp4_read_mdhd_box(f, box_size);
        } else if(name == "hdlr") {
			CMp4_hdlr_box hdlr;
            box.hdlr= hdlr.mp4_read_hdlr_box(f, box_size);
        }else if(name == "minf"){
			CMp4_minf_box minf;
            box.minf= minf.mp4_read_minf_box(f, box_size);			
        }        
        cur_pos    += box_size;
        inner_size += box_size;
    } while(inner_size+8 < size);
    printf("\n");
    
    return box;
}

void CMp4_mdia_box::write_mdia(FILE *fin, FILE * fout, uint32_t mdia_size,CMp4_mdia_box mdia,std::string str) 
{
	unsigned int cur=_ftelli64(fin);
	uint32_t cur_size=0;
	write_uint32_lit(fout,mdia.size);
	fwrite(&mdia.type,sizeof(mdia.type),1,fout);
	/*дmdhd*/
	do 
	{
		_fseeki64(fin,cur,SEEK_SET);
		uint32_t buf_size  = read_uint32_lit(fin);
		unsigned char p[5];
		fread(p, 4, 1, fin);
		p[4]= 0;
		std::string name= (char*)p;
		if (name=="mdhd")
		{
			write_uint32_lit(fout,mdia.mdhd.size);
			fwrite(&mdia.mdhd.type,sizeof(mdia.mdhd.type),1,fout);
			write_uint8(fout,mdia.mdhd.version);
			fwrite(&mdia.mdhd.flags,sizeof(mdia.mdhd.flags),1,fout);
			write_uint32_lit(fout,mdia.mdhd.creation_time);
			write_uint32_lit(fout,mdia.mdhd.modification_time);
			write_uint32_lit(fout,mdia.mdhd.timescale);
			write_uint32_lit(fout,mdia.mdhd.duration);
			write_uint16_big(fout,mdia.mdhd.language);
			write_uint16_big(fout,mdia.mdhd.pre_defined);
		} 
		else if (name=="minf")
		{
			CMp4_minf_box minf;
			minf.write_minf(fin,fout,buf_size,mdia.minf,str);
		}
		else
		{
			write_uint32_lit(fout,buf_size);
			fwrite(&p,4,1,fout);
			int buf=buf_size-8;
			char *ptr=new char [buf];
			fread(ptr, buf, 1, fin);
			fwrite(ptr, buf, 1, fout);
			delete [] ptr;
		}
		cur_size+=buf_size+8;
		cur+=buf_size;
	} while (cur_size<=mdia_size);
}
