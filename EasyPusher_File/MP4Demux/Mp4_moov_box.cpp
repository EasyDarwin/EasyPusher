#include "Mp4_moov_box.h"
#include "read_write.h"

CMp4_moov_box::CMp4_moov_box(void)
{
}


CMp4_moov_box::~CMp4_moov_box(void)
{
}

CMp4_moov_box CMp4_moov_box::mp4_read_moov_box(FILE* f, int size)   //level 2
{
	printf("+%s\n",  "moov");
	CMp4_moov_box moov;
	moov.size= size;
	moov.type= 'm'|'o'<<8|'o'<<16|'v'<<24;
	int k = 0;                                  \
	unsigned char p[5];                         \
	int inner_size = 0;
	int level_2_box_size= 0;
	int i=0;
	unsigned int cur_pos= _ftelli64(f);
	do{
		_fseeki64(f, cur_pos, SEEK_SET);

		level_2_box_size= read_uint32_lit(f);
		std::string name  = Read4ByteString(f);
		if(name == "mvhd"){
			CMp4_mvhd_box mvhd;
			moov.mvhd=mvhd.mp4_read_mvhd_box(f, level_2_box_size);
		} else if(name == "trak") {
			CMp4_trak_box trak;
			moov.trak.push_back(trak.mp4_read_trak_box(f, level_2_box_size));
		}
		cur_pos    += level_2_box_size;
		inner_size += level_2_box_size;
	}while( inner_size+8 < size&&level_2_box_size>0);

	return moov;
}

void CMp4_moov_box::write_moov(FILE *fin,FILE *fout,uint32_t moov_size,
	CMp4_moov_box moov,CMp4_moov_box moov1) //moov1 is changed moov
{

	unsigned int cur=_ftelli64(fin);
	uint32_t cur_size=0;
	write_uint32_lit(fout,moov1.size);
	fwrite(&moov1.type,sizeof(moov1.type),1,fout);
	int i=0;
	do 
	{
		_fseeki64(fin,cur,SEEK_SET);
		uint32_t buf_size  = read_uint32_lit(fin);
		unsigned char p[5];
		fread(p, 4, 1, fin);
		p[4]= 0;
		std::string name= (char*)p;
		if (name=="mvhd")
		{
			write_uint32_lit(fout,moov1.mvhd.size);
			fwrite(&moov1.mvhd.type,sizeof(moov1.mvhd.type),1,fout);
			write_uint8(fout,moov1.mvhd.version);
			fwrite(&moov1.mvhd.flags,sizeof(moov1.mvhd.flags),1,fout);
			write_uint32_lit(fout,moov1.mvhd.creation_time);
			write_uint32_lit(fout,moov1.mvhd.modification_time);
			write_uint32_lit(fout,moov1.mvhd.timescale);
			write_uint32_lit(fout,moov1.mvhd.duration);
			write_uint32_lit(fout,moov1.mvhd.rate);
			write_uint16_big(fout,moov1.mvhd.volume);
			fwrite(&moov1.mvhd.reserved1,sizeof(moov1.mvhd.reserved1),1,fout);
			fwrite(&moov1.mvhd.reserved2, sizeof(moov1.mvhd.reserved2), 1, fout);    //8
			fwrite(&moov1.mvhd.matrix, sizeof(moov1.mvhd.matrix), 1, fout);           //36
			fwrite(&moov1.mvhd.pre_defined, sizeof(moov1.mvhd.pre_defined), 1, fout); //24
			write_uint32_lit(fout,moov1.mvhd.next_track_id);
		} 
		else if(name=="trak")
		{
			if (moov.trak[i].mdia.hdlr.handler_type=="vide"||moov.trak[i].mdia.hdlr.handler_type=="soun")
			{
				CMp4_trak_box mp4_trak;
				mp4_trak.write_trak(fin,fout,buf_size,moov1.trak[i],moov.trak[i].mdia.hdlr.handler_type);
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
			i++;
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
	} while (cur_size<=moov_size);
}