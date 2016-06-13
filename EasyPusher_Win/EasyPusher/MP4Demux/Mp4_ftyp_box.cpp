// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_ftyp_box.h"
#include "read_write.h"

CMp4_ftyp_box::CMp4_ftyp_box(void)
{
}


CMp4_ftyp_box::~CMp4_ftyp_box(void)
{
}

void CMp4_ftyp_box::mp4_read_ftyp_box(FILE *f, int size)  //level 2
{
	printf("+ftyp\n");
	//int cur=ftell(f);
	size -= 8;
	char *ch = new char[size+1];
	for(int i = 0; i < size; ++i) {
		fread(&ch[i], sizeof(char), 1, f);
	}
	ch[size] = 0;

	printf("\tftyp: %s\n",  ch);
	delete [] ch;
}
void CMp4_ftyp_box::write_ftyp(FILE *fin,FILE *fout) 
{
	_fseeki64(fin,0,SEEK_SET);
	int buf_size  = read_uint32_lit(fin);
	_fseeki64(fin,0,SEEK_SET);
	char *ptr=new char [buf_size];
	fread(ptr, buf_size, 1, fin);
	fwrite(ptr, buf_size, 1, fout);
	delete [] ptr;
}
