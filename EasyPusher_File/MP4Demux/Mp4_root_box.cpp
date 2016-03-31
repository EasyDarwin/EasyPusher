#include "Mp4_root_box.h"
#include "read_write.h"

CMp4_root_box::CMp4_root_box(void)
{
}


CMp4_root_box::~CMp4_root_box(void)
{
}

unsigned int CMp4_root_box::mp4_read_root_box(FILE *f) //level 1
{
	int k = 0;
	unsigned char p[5];

	unsigned int level_1_box_size  = read_uint32_lit(f);
	string name= Read4ByteString(f);
	if(name == "moov"){
		CMp4_moov_box mov;
		moov=mov.mp4_read_moov_box(f, level_1_box_size);  
		for (int i=0;i<moov.trak.end()-moov.trak.begin();i++)
		{
			trk.push_back(moov.trak[i]);
			sd.push_back(moov.trak[i].mdia.minf.stbl.stsd);
			ts.push_back(moov.trak[i].mdia.minf.stbl.stts);
			sz.push_back(moov.trak[i].mdia.minf.stbl.stsz);
			co.push_back(moov.trak[i].mdia.minf.stbl.stco);
			sc.push_back(moov.trak[i].mdia.minf.stbl.stsc);
			if (i==0)
			{
				box_ss=moov.trak[i].mdia.minf.stbl.stss;
			}			
		}

	}else if(name == "ftyp"){
		CMp4_ftyp_box ftyp1;
		ftyp1.mp4_read_ftyp_box(f, level_1_box_size);

	} else if(level_1_box_size == 0){  //till the end of file
		return 1;

	}  else if(name == "mdat"){
		CMp4_mdat_box mdat;
		mdat.mp4_read_mdat_box(f, level_1_box_size);

	} 
	return level_1_box_size;
}