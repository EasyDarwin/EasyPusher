#include "Mp4_avcC_box.h"
#include "read_write.h"

CMp4_avcC_box * CMp4_avcC_box::mp4_read_avcC_box(FILE *f)
{
	CMp4_avcC_box *ptr= new CMp4_avcC_box;

	ptr->size= read_uint32_lit(f);
	ptr->type= read_uint32_lit(f);
	ptr->configurationVersion= read_uint8(f);
	ptr->AVCProfileIndication= read_uint8(f);
	ptr->profile_compatibility= read_uint8(f);
	ptr->AVCLevelIndication= read_uint8(f);
	ptr->lengthSizeMinusOne= 0x3 & read_uint8(f);  //2 bit
	ptr->numOfSequenceParameterSet = 0x1F & read_uint8(f);  //5bit
	ptr->sps= new CMp4_avcC_box::SPS[ptr->numOfSequenceParameterSet];
	ptr->pps= new CMp4_avcC_box::PPS[ptr->numOfPictureParameterSets];

	/*´òÓ¡*/
	printf("avcC_size:%u\n",ptr->size);
	if( ptr->type==  ('a'<<24|'v'<<16|'c'<<8|'C')){
		printf("type:avcC\n");
	}
// 	printf("configurationVersion:%u\n", ptr->configurationVersion);
// 	printf("AVCProfileIndication: %x\n", ptr->AVCProfileIndication);
// 	printf("lengthSizeMinusOne:%u\n", ptr->lengthSizeMinusOne);
// 	printf("numOfSequenceParameterSet:%x\n", ptr->numOfSequenceParameterSet);
	for( int i = 0; i < ptr->numOfSequenceParameterSet; ++i ){
		ptr->sps[i].sequenceParameterSetLength = read_uint16_lit(f);
		printf("sequenceParameterSetLength: %u\n", ptr->sps[i].sequenceParameterSetLength);

		ptr->sps[i].sequenceParameterSetNALUnit =
			new uint8_t[ptr->sps[i].sequenceParameterSetLength];
		fread((ptr->sps[i].sequenceParameterSetNALUnit),
			(ptr->sps[i].sequenceParameterSetLength), 1, f);

		for(int j = 0; j < ptr->sps[i].sequenceParameterSetLength; ++j){
			printf("%x", ptr->sps[i].sequenceParameterSetNALUnit[j]);
		}
		printf("============\n");
	}
	ptr->numOfPictureParameterSets = read_uint8(f);
	printf("numOfPictureParameterSets:%u\n", ptr->numOfPictureParameterSets);
	for( int i = 0; i < ptr->numOfPictureParameterSets; ++i){
		ptr->pps[i].pictureParameterSetLength = read_uint16_lit(f);
		printf("%d\n", ptr->pps[i].pictureParameterSetLength);

		ptr->pps[i].pictureParameterSetNALUnit
			= new uint8_t[ptr->pps[i].pictureParameterSetLength];

		fread(ptr->pps[i].pictureParameterSetNALUnit,
			ptr->pps[i].pictureParameterSetLength, 1, f);

		for(int j = 0; j < ptr->pps[i].pictureParameterSetLength; ++j){
			printf("%x", ptr->pps[i].pictureParameterSetNALUnit[j]);
		}
		printf("============\n");
	}

	return ptr;
}