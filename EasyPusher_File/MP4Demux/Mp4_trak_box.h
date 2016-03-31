#pragma once
#include "Base_box.h"
#include "Mp4_mdia_box.h"
#include "Mp4_tkhd_box.h"

class CMp4_trak_box
{
public:
	CMp4_trak_box(void);
	~CMp4_trak_box(void);
	CMp4_trak_box mp4_read_trak_box(FILE *f, int size);
	void write_trak(FILE *fin,FILE * fout,uint32_t trak_size,CMp4_trak_box trak,std::string str);

	MP4_BASE_BOX;
	CMp4_tkhd_box tkhd;
	CMp4_mdia_box mdia;
};

