#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
#include <string>
#include <vector>

class CMp4_hdlr_box
{
public:
	CMp4_hdlr_box(void);
	~CMp4_hdlr_box(void);
	CMp4_hdlr_box mp4_read_hdlr_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t       pre_defined;
	std::string        handler_type;
	uint32_t       reserved[3];
	std::vector<char>   name;  //end with '\0'
};

