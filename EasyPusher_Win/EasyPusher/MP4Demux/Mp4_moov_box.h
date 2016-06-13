#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
#include <vector>
#include "Mp4_mvhd_box.h"
#include "Mp4_trak_box.h"

class CMp4_moov_box
{
public:
	CMp4_moov_box(void);
	~CMp4_moov_box(void);
	CMp4_moov_box mp4_read_moov_box(FILE* f, int size);
	void write_moov(FILE *fin,FILE *fout,uint32_t moov_size,
		CMp4_moov_box moov,CMp4_moov_box moov1);

	MP4_BASE_BOX;
	CMp4_mvhd_box mvhd;
	std::vector<CMp4_trak_box> trak;
};

