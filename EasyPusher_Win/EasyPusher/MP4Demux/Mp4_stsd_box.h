#pragma once
#include "Sample_description.h"
#include "Base_box.h"
#include "inttype.h"
class CMp4_stsd_box
{
public:
	CMp4_stsd_box(void);
	~CMp4_stsd_box(void);
	CMp4_stsd_box mp4_read_stsd_box(FILE *f, int size);

	MP4_FULL_BOX;                           //12 bytes
	uint32_t            number_of_entries;  //4 bytes
	CSample_description *sample_description;
};

