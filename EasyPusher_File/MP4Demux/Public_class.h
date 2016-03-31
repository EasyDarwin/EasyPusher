
#ifndef PUBLIC_CLASS_H
#define PUBLIC_CLASS_H
#include "inttype.h"

class t_time
{
public:
	float t_beg;
	float t_end;
};

class varible{
public:
	uint32_t f;
	uint32_t t;
};

class mp4_list_t{
public:
	uint32_t first_chunk_num;
	uint32_t sample_amount_in_cur_table;
	uint32_t sample_description_id;
};
#endif