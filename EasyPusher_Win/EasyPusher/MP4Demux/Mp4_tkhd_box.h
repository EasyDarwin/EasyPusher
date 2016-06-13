#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>

class CMp4_tkhd_box
{
public:
	CMp4_tkhd_box(void);
	~CMp4_tkhd_box(void);
	CMp4_tkhd_box mp4_read_tkhd_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t creation_time;
	uint32_t modification_time;
	uint32_t track_ID;
	uint32_t reserved1;
	uint32_t duration;

	uint32_t reserved2[2];
	uint16_t layer;
	uint16_t alternate_group;

	uint16_t volume;
	uint16_t reserved3;
	uint32_t matrix[9];
	uint32_t width;
	uint32_t height;
};

