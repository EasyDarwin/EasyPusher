#pragma once
#include <string>
#include "Base_box.h"
#include "Mp4_stsd_box.h"
#include "Mp4_stss_box.h"
#include "Mp4_stsz_box.h"
#include "Mp4_stsc_box.h"
#include "Mp4_stts_box.h"
#include "Mp4_stco_box.h"

class CMp4_stbl_box
{
public:
	CMp4_stbl_box(void);
	~CMp4_stbl_box(void);
	CMp4_stbl_box mp4_read_stbl_box(FILE *f, int size);
	void write_stbl(FILE *fin, FILE * fout, uint32_t stbl_size, CMp4_stbl_box stbl ,std::string str);

	MP4_BASE_BOX;
	CMp4_stsd_box stsd;
	CMp4_stts_box stts;
	CMp4_stss_box stss;
	CMp4_stsc_box stsc;
	CMp4_stsz_box stsz;
	CMp4_stco_box stco;
};

