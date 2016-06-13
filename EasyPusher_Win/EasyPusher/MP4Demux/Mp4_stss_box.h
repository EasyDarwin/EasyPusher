#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
class CMp4_stss_box
{
public:
	CMp4_stss_box(void);
	~CMp4_stss_box(void);
	CMp4_stss_box mp4_read_stss_box(FILE *f, int size); //level 8  ¹Ø¼üÖ¡ÁÐ±í

	MP4_FULL_BOX;
	uint32_t number_of_entries;
	uint32_t *sync_sample_table;	
};

