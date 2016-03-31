#include "Mp4_stts_box.h"
#include "read_write.h"

CMp4_stts_box::CMp4_stts_box(void)
{
}


CMp4_stts_box::~CMp4_stts_box(void)
{
}

CMp4_stts_box CMp4_stts_box::mp4_read_stts_box(FILE *f, int size)  //level 8    time-to-sample
{
	CMp4_stts_box box_ts;
	printf("\t\t\t\t\t+%s\n", "stts");
	box_ts.size= size;
	box_ts.type= 's'|'t'<<8|'t'<<16|'s'<<24;
	box_ts.version= read_uint8(f);
	fread(box_ts.flags, sizeof(box_ts.flags), 1, f);
	box_ts.number_of_entries=read_uint32_lit(f);//条目：time-to-sample 的数目
	printf("flags: %u\n",box_ts.flags[0]|box_ts.flags[1]|box_ts.flags[2]);
	//printf("%x\n",);
	printf("number of entries: %u\n",box_ts.number_of_entries);
	/*int cur=ftell(f);
	cout<<"cur:"<<cur<<endl;*/
	printf("entries:\n");
	//box_ts.time_to_sample_table = new uint32_t[box_ts.number_of_entries];  
	/*结构：sample count:有相同duration 的连续sample 的数目;
	sample duration:每个sample 的duration*/
	box_ts.sample_count=new uint32_t[box_ts.number_of_entries];
	box_ts.sample_duration=new uint32_t[box_ts.number_of_entries];
	int m=0,n=0;
	unsigned int cur_pos=_ftelli64(f);
	for(int i = 0; i < box_ts.number_of_entries*2; ++i){
		_fseeki64(f,cur_pos,SEEK_SET);
		if (i%2)
		{
			box_ts.sample_duration[m]=read_uint32_lit(f);
			if (i<100)
			{
				std::cout<<box_ts.sample_duration[m]<<"  ";
			}
			m++;
		} 
		else
		{
			box_ts.sample_count[n]=read_uint32_lit(f);			
			if (i<100)
			{
				std::cout<<box_ts.sample_count[n]<<"  ";
			}
			n++;
		}
		if((i+1)%20 == 0&&i<100) printf("\n");
		cur_pos+=4;
	}
	int tim=0,num=0;
	for (int i=0;i<box_ts.number_of_entries;i++)
	{
		tim+=box_ts.sample_count[i]*box_ts.sample_duration[i];
		num+=box_ts.sample_count[i];
	}
	std::cout<<"tim="<<tim<<"  "<<"num="<<num<<std::endl;
	printf("\n");
	return box_ts;
}