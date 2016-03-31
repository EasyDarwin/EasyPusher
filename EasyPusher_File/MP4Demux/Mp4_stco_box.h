#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
#include <vector>
#include "Public_class.h"
using namespace std;
class CMp4_stco_box
{
public:
	CMp4_stco_box(void);
	~CMp4_stco_box(void);
	CMp4_stco_box mp4_read_stco_box(FILE *f, int size);


	MP4_FULL_BOX;
	uint32_t chunk_offset_amount;
	uint32_t *chunk_offset_from_file_begin;
};

