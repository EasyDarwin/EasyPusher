#pragma once
#include <iostream>
class CMp4_ftyp_box
{
public:
	CMp4_ftyp_box(void);
	~CMp4_ftyp_box(void);
	void mp4_read_ftyp_box(FILE *f, int size);
	void write_ftyp(FILE *fin,FILE *fout);
	
};

