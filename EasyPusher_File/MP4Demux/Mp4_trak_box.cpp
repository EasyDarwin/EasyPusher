#include "Mp4_trak_box.h"
#include <string>
#include "read_write.h"

CMp4_trak_box::CMp4_trak_box(void)
{
}


CMp4_trak_box::~CMp4_trak_box(void)
{
}

CMp4_trak_box CMp4_trak_box::mp4_read_trak_box(FILE *f, int size)
{
	printf("\t+%s\n", "trak");
	CMp4_trak_box trak;
	//trak  = new struct mp4_trak_box;
	trak.size= size;
	trak.type= ('t' | 'r'<<8 | 'a'<<16 | 'k'<<24);
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
		std::string name= (char*)p;
		if(name == "tkhd") {
			CMp4_tkhd_box tkhd;
			trak.tkhd= tkhd.mp4_read_tkhd_box(f, box_size);
		}
		else if(name == "mdia"){
			CMp4_mdia_box mdia;
			trak.mdia= mdia.mp4_read_mdia_box(f, box_size);
		} 
		cur_pos    += box_size;
		inner_size += box_size;
	} while(inner_size+8 < size);

	return trak;
}

void CMp4_trak_box::write_trak(FILE *fin,FILE * fout,uint32_t trak_size,CMp4_trak_box trak,std::string str) 
{
	unsigned int cur=_ftelli64(fin);
	uint32_t cur_size=0;
	write_uint32_lit(fout,trak.size);
	fwrite(&trak.type,sizeof(trak.type),1,fout);		
	do 
	{
		_fseeki64(fin,cur,SEEK_SET);
		uint32_t buf_size  = read_uint32_lit(fin);
		unsigned char p[5];
		fread(p, 4, 1, fin);
		p[4]= 0;
		std::string name= (char*)p;
		if (name=="tkhd")
		{
			write_uint32_lit(fout,trak.tkhd.size);
			fwrite(&trak.tkhd.type,sizeof(trak.tkhd.type),1,fout);
			write_uint8(fout,trak.tkhd.version);
			fwrite(&trak.tkhd.flags,sizeof(trak.tkhd.flags),1,fout);
			write_uint32_lit(fout,trak.tkhd.creation_time);
			write_uint32_lit(fout,trak.tkhd.modification_time);
			write_uint32_lit(fout,trak.tkhd.track_ID);
			write_uint32_lit(fout,trak.tkhd.reserved1);
			write_uint32_lit(fout,trak.tkhd.duration);
			fwrite(&trak.tkhd.reserved2,sizeof(trak.tkhd.reserved2),1,fout);
			write_uint16_big(fout,trak.tkhd.layer);
			write_uint16_big(fout,trak.tkhd.alternate_group);
			write_uint16_big(fout,trak.tkhd.volume);
			write_uint16_big(fout,trak.tkhd.reserved3);
			fwrite(&trak.tkhd.matrix,sizeof(trak.tkhd.matrix),1,fout);
			write_uint32_lit(fout,trak.tkhd.width);
			write_uint32_lit(fout,trak.tkhd.height);
		} 
		else if (name=="mdia")
		{
			CMp4_mdia_box mdia;
			mdia.write_mdia(fin,fout,buf_size,trak.mdia,str);	
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
	} while (cur_size<=trak_size);
}
