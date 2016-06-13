#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
class CMp4_mdhd_box
{
public:
	CMp4_mdhd_box(void);
	~CMp4_mdhd_box(void);
	CMp4_mdhd_box mp4_read_mdhd_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t timescale;
	uint32_t duration;
	uint16_t language;
	uint16_t pre_defined;
};

