#pragma once
#include "Mp4_moov_box.h"
#include "Mp4_mdat_box.h"
#include "Mp4_ftyp_box.h"
class CMp4_root_box
{
public:
	CMp4_root_box(void);
	~CMp4_root_box(void);
	unsigned int mp4_read_root_box(FILE *f);

	CMp4_moov_box moov;

	vector<CMp4_stsd_box> sd;
	vector<CMp4_stts_box> ts;
	vector<CMp4_stsc_box> sc;
	vector<CMp4_stsz_box> sz;
	vector<CMp4_stco_box> co;
	CMp4_stss_box box_ss;
	vector<CMp4_trak_box> trk;
};

