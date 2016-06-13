#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>

class CMp4_mdat_box
{
public:
	CMp4_mdat_box(void);
	~CMp4_mdat_box(void);
	void mp4_read_mdat_box(FILE *f, unsigned int size);

	MP4_BASE_BOX;
	uint64_t data_size;
	char     *data;
};

