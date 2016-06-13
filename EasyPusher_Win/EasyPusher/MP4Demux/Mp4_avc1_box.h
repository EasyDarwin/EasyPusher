#ifndef MP4_AVC1_BOX_H
#define MP4_AVC1_BOX_H
#include "Base_box.h"
#include "Mp4_avcC_box.h"

class CMp4_avc1_box {
public:
	CMp4_avc1_box* mp4_read_avc1_box(FILE *f, int size);

	VISUAL_SAMPLE_ENTRY_FIELDS;
	CMp4_avcC_box *avcC;
	struct mp4_btrt_box *btrt;
	/*ext descriptors */
	struct mp4_m4ds_box *m4ds;
};
#endif