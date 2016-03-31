#pragma once
#include "Base_box.h"
#include "inttype.h"
#include <iostream>
class CMp4_stsz_box
{
public:
	CMp4_stsz_box(void);
	~CMp4_stsz_box(void);
	CMp4_stsz_box mp4_read_stsz_box(FILE *f, int size);

	MP4_FULL_BOX;
	uint32_t samples_size_intotal;// 全部sample的数目。如果所有的sample有相同的长度，这个字段就是这个值。
	// 否则，这个字段的值就是0。那些长度存在sample size表中
	uint32_t table_size;  
	uint32_t *sample_size_table;
};

