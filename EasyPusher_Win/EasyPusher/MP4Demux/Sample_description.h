#pragma once
#include "Mp4_avc1_box.h"
#include "Mp4_mp4a_box.h"
#include <string>
#include "inttype.h"
class CSample_description
{
public:

	uint32_t size;
	std::string type;
	CMp4_avc1_box *avc1;
	CMp4_mp4a_box *mp4a;
};

