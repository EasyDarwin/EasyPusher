#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>

class CMp4_stts_box
{
public:
	CMp4_stts_box(void);
	~CMp4_stts_box(void);
	CMp4_stts_box mp4_read_stts_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t number_of_entries;
	uint32_t *sample_count;
	uint32_t *sample_duration;
};

