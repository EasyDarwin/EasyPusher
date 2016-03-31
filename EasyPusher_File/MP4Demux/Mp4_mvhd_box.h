#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>

class CMp4_mvhd_box
{
public:
	CMp4_mvhd_box(void);
	~CMp4_mvhd_box(void);
	CMp4_mvhd_box mp4_read_mvhd_box(FILE *f, int size);

	MP4_FULL_BOX;                //12
	uint32_t creation_time;      //4
	uint32_t modification_time;  //4
	uint32_t timescale;          //4
	uint32_t duration;           //4
	uint32_t rate;               //4
	uint16_t volume;             //2
	uint16_t reserved1;          //2
	uint32_t reserved2[2];       //8
	uint32_t matrix[9];          //36
	uint32_t pre_defined[6];     //24
	uint32_t next_track_id;      //4
};

