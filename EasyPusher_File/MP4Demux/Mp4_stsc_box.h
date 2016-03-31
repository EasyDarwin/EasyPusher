#pragma once
#include "Base_box.h"
#include "Public_class.h"
#include <iostream>
class CMp4_stsc_box			//Sample-to-Chunk Atoms
{
public:
	CMp4_stsc_box(void);
	~CMp4_stsc_box(void);
	CMp4_stsc_box mp4_read_stsc_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t map_amount;
	mp4_list_t *scmap; /*sample-to-trunk表的结构*/
};

