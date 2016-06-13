#pragma once
#include "Base_box.h"
#include "Mp4_stbl_box.h"

class CMp4_minf_box
{
public:
	CMp4_minf_box(void);
	~CMp4_minf_box(void);
	CMp4_minf_box mp4_read_minf_box(FILE *f, int size);
	void write_minf( FILE * fin, FILE * fout, uint32_t minf_size,
		CMp4_minf_box minf, std::string str) ;

	MP4_BASE_BOX;
	CMp4_stbl_box stbl;
};

