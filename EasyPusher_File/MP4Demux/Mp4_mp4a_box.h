#pragma once
#include <iostream>
class CMp4_mp4a_box
{
public:
	CMp4_mp4a_box(void);
	~CMp4_mp4a_box(void);
	CMp4_mp4a_box * mp4_read_mp4a_box(FILE*f, int size);

	unsigned int  size;
	unsigned int  type;
	unsigned char reserved_format[6];
	unsigned char  data_reference_index;
	unsigned int reserved[2];
	unsigned char channelcount;
	unsigned char samplesize;
	unsigned char pre_defined;
	unsigned int samplerate;
};

