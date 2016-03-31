#include "Mp4_stsd_box.h"
#include "read_write.h"


CMp4_stsd_box::CMp4_stsd_box(void)
{
}


CMp4_stsd_box::~CMp4_stsd_box(void)
{
}

CMp4_stsd_box CMp4_stsd_box::mp4_read_stsd_box( FILE *f, int size )
{
	CMp4_stsd_box box_sd;
	printf("\t\t\t\t\t+%s\n", "stsd");
	box_sd.size= size;
	box_sd.type= 's'|'t'<<8|'s'<<16|'d'<<24;
	box_sd.version= read_uint8(f);
	fread(box_sd.flags, sizeof(box_sd.flags), 1, f);
	box_sd.number_of_entries=read_uint32_lit(f);

// 	printf("\t\t\t\t\t\tflags: %u\n",
// 		box_sd.flags[0]|box_sd.flags[1]|box_sd.flags[2]);
	printf("number of entries: %u\n", box_sd.number_of_entries);

	box_sd.sample_description = new CSample_description[box_sd.number_of_entries];
	for(int i =0; i < box_sd.number_of_entries; ++i){
		box_sd.sample_description[i].size = read_uint32_lit(f);
		box_sd.sample_description[i].type = Read4ByteString(f);

		std::cout<<"box_sd.sample_description.type"<<box_sd.sample_description[i].type<<std::endl;
		if( box_sd.sample_description[i].type == "mp4a")
		{
			CMp4_mp4a_box mp4a;
			box_sd.sample_description[i].mp4a
				= mp4a.mp4_read_mp4a_box(f, box_sd.sample_description[i].size);
			box_sd.sample_description[i].avc1 = 0;
		}
		else if(box_sd.sample_description[i].type == "avc1")
		{
				CMp4_avc1_box avc1;
				box_sd.sample_description[i].avc1
					= avc1.mp4_read_avc1_box(f, box_sd.sample_description[i].size);
				box_sd.sample_description[i].mp4a = 0;
		} 
	}
	//sd.push_back(box_sd);
	return box_sd;
}



