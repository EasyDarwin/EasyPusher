#ifndef BASE_BOX_H
#define BASE_BOX_H

#ifndef MP4_BASE_BOX
#define MP4_BASE_BOX							\
	uint32_t type;                              \
	uint32_t  size;
#endif

#ifndef MP4_FULL_BOX
#define MP4_FULL_BOX                            \
	MP4_BASE_BOX;                               \
	uint8_t version;                            \
	uint8_t flags[3]
#endif

#ifndef MP4_SAMPLE_ENTRY_FIELDS
#define MP4_SAMPLE_ENTRY_FIELDS                    \
	MP4_BASE_BOX;                                \
	char reserved[6];                            \
	uint16_t data_reference_index;
#endif

#ifndef VISUAL_SAMPLE_ENTRY_FIELDS
#define VISUAL_SAMPLE_ENTRY_FIELDS                \
	MP4_SAMPLE_ENTRY_FIELDS;                    \
	uint16_t      pre_defined;                  \
	uint16_t      reserved1;                    \
	uint32_t      pre_defined1[3];              \
	uint16_t      width;                        \
	uint16_t      height;                       \
	uint32_t      horiz_res;                    \
	uint32_t      vert_res;                     \
	uint32_t      reserved2;                    \
	/**/uint16_t  frames_count;                 \
	/*“‘œ¬ «AVCDecoderConfigurationRecord*/     \
	/**/char      compressor_name[33];          \
	/**/uint16_t  bit_depth;                    \
	/**/int16_t   pre_defined2
#endif

#endif
