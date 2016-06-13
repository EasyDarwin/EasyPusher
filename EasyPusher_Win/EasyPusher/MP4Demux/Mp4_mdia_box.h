#pragma once
#include "Base_box.h"
#include "Mp4_minf_box.h"
#include "Mp4_mdhd_box.h"
#include "Mp4_hdlr_box.h"
#include "inttype.h"
class CMp4_mdia_box
{
public:
	CMp4_mdia_box(void);
	~CMp4_mdia_box(void);
	CMp4_mdia_box mp4_read_mdia_box(FILE *f, int size);
	void write_mdia(FILE *fin, FILE * fout, uint32_t mdia_size,CMp4_mdia_box mdia,std::string str) ;

	MP4_BASE_BOX;
	CMp4_mdhd_box mdhd;
	CMp4_hdlr_box hdlr;
	CMp4_minf_box minf;
};

