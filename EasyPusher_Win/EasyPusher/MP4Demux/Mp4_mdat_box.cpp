// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Mp4_mdat_box.h"


CMp4_mdat_box::CMp4_mdat_box(void)
{
}


CMp4_mdat_box::~CMp4_mdat_box(void)
{
}

/*@a special final box
 */
void CMp4_mdat_box::mp4_read_mdat_box(FILE *f, unsigned int size)
{
	int cur=ftell(f);

	printf("\t+%s\n", "mdat");
	printf("\t\tthis is the real media data\n");
}