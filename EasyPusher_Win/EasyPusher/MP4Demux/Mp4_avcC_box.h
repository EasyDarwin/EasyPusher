#ifndef MP4_AVCC_BOX_H
#define MP4_AVCC_BOX_H

#include "inttype.h"
#include <iostream>

class CMp4_avcC_box{
public:
	CMp4_avcC_box * mp4_read_avcC_box(FILE *f);

	uint32_t      size;
	uint32_t      type;
	uint8_t       configurationVersion;      //=1
	uint8_t       AVCProfileIndication;
	uint8_t       profile_compatibility;
	uint8_t       AVCLevelIndication;

	uint8_t       lengthSizeMinusOne;        // & 0x3,  ==2 bit
	uint8_t       numOfSequenceParameterSet; // & 0x1F  ==5bit
	struct SPS{
		uint16_t  sequenceParameterSetLength;
		uint8_t   *sequenceParameterSetNALUnit;
	}             *sps;

	uint8_t       numOfPictureParameterSets;
	struct PPS{
		uint16_t  pictureParameterSetLength;
		uint8_t   *pictureParameterSetNALUnit;
	}             *pps; 
};
#endif