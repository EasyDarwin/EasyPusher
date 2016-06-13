// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "Public_func.h"


int get_track_size_in_moov(CMp4_root_box& box)
{
	return box.trk.size();
}

//找到moov中的音（"audio"）、视频("video")轨 (不考虑多个音频轨，或者多个视频轨的情况)
int get_track_id_by_name(CMp4_root_box& box, std::string name )
{
	int nTrackCount = get_track_size_in_moov(box);
	for (int i=0; i<nTrackCount; i++)
	{
		if (name == "video")
		{
			for(int i =0; i < box.sd[i].number_of_entries; ++i)
			{
				if (box.sd[i].sample_description[i].type == "avc1")//video just support avc1
				{
					return i;
				}
			}
		}
		else if (name == "audio" )
		{
			for(int i =0; i < box.sd[i].number_of_entries; ++i)
			{
				if (box.sd[i].sample_description[i].type == "mp4a")//video just support mp4a
				{
					return i;
				}
			}
		}
	}
	return -1;
}

int get_video_info_in_moov(CMp4_root_box& box, CMp4_avcC_box& videoInfo)
{
	int nTrackCount = get_track_size_in_moov(box);
	for (int i=0; i<nTrackCount; i++)
	{
		for(int j =0; j < box.sd[i].number_of_entries; ++i)
		{
			if (box.sd[i].sample_description[j].type == "avc1")//video just support avc1
			{
				memcpy(&videoInfo, box.sd[i].sample_description[j].avc1->avcC, sizeof(CMp4_avcC_box) );
				return i;
			}
		}
	}
	return -1;
}

int get_audio_info_in_moov(CMp4_root_box& box, CMp4_mp4a_box& audioInfo)
{
	int nTrackCount = get_track_size_in_moov(box);
	for (int i=0; i<nTrackCount; i++)
	{
		for(int j =0; j < box.sd[i].number_of_entries; ++i)
			{
				if (box.sd[i].sample_description[j].type == "mp4a")//video just support mp4a
				{
					memcpy(&audioInfo, box.sd[i].sample_description[j].mp4a, sizeof(CMp4_mp4a_box) );
					return i;
				}
			}
	}
	return -1;
}


uint32_t get_sample_num_in_cur_chunk(const CMp4_stsc_box & box,
                                     const uint32_t chunk_index)  //chunk_index begin from 1,2,...
{
    int sample_num_in_cur_chunk_ = 0;
    
    for(int i = 0; i < box.map_amount; ++i) {
        if(i+1 == box.map_amount){
                
            sample_num_in_cur_chunk_
                = box.scmap[i].sample_amount_in_cur_table;
        }

        if(chunk_index >= box.scmap[i].first_chunk_num
           && chunk_index < box.scmap[i+1].first_chunk_num){
            sample_num_in_cur_chunk_
                =  box.scmap[i].sample_amount_in_cur_table;
            
                 break;
        }
    }
    
    //std::cout<<"sample_num_in_cur_chunk_:"<< sample_num_in_cur_chunk_;
    
    return sample_num_in_cur_chunk_;
}

/*@a return index from 0*/
uint32_t get_sample_index(const CMp4_stsc_box &box,
                          const uint32_t chunk_index)  //[1,end)
{
    /*chunk   samples   id
      1         8        1
      2         7        1
      46        6        1
    */
    uint32_t me= chunk_index;
    uint32_t sindex = 0;
    
    for(int i = 0; i < box.map_amount; ++i) {
		uint32_t chunk_amount = 0;
		uint32_t curr= box.scmap[i].first_chunk_num;
        
        if(i+1 == box.map_amount){ //end() , we can't have next one to index
            chunk_amount = me - curr;  //do not add ONE
            sindex+= box.scmap[i].sample_amount_in_cur_table * chunk_amount;
            break;
        }
        uint32_t next= box.scmap[i + 1].first_chunk_num;
        if(me > curr){
            if(me < next){
                chunk_amount = me - curr;  //do not add ONE
                sindex+= box.scmap[i].sample_amount_in_cur_table * chunk_amount;
                break;
            }else{
                chunk_amount = next - curr;  //do not add ONE
                sindex+= box.scmap[i].sample_amount_in_cur_table * chunk_amount;
            }
        } else if(me==curr){
            break;
        }
    }
    return sindex+1;																					/*改动*/
}

uint32_t get_sample_size(CMp4_stsz_box &box,
                         const uint32_t sample_index_)  //[1, end)
{
    for(int i = 0; i < box.table_size; ++i) {
        if(sample_index_ == i+1)																			/*改动*/
            return box.sample_size_table[i];
    }
	return 0;
}

// 获取stts中某个sample的时间(映射下一帧到来的时间)
uint32_t get_sample_time(CMp4_stts_box& box, const uint32_t sample_index)
{
	int nSampleId = 0;
	for(int i = 0; i < box.number_of_entries; ++i) 
	{
		for(int j = 0; j < box.sample_count[i]; j++) 
		{
			if (nSampleId == sample_index)
			{
				return 	box.sample_duration[i];
			}
			nSampleId ++;
		}
	}
	return 0;
}


int floatToInt(float f)
{  
	int i = 0;  
	if(f>0) //正数   
	i = (f*10+ 5)/10;  
	else if(f<0) //负数   
	i = (f*10-5)/10;  
	else i = 0;  
	 
	return i;  
 
} 

vector<t_time> videoTime_To_soundTime(vector<t_time> vc_time, vector<varible> sample_key, CMp4_root_box root ) 
{
	vector<t_time> vc_t;
	t_time t_tm;
	uint32_t num=0;
	double tm_t=0;
	vector<t_time>::iterator it_t=vc_time.begin();
	for (auto it_k=sample_key.begin();it_k<sample_key.end();it_k++,it_t++)
	{
		t_tm.t_beg=it_t->t_beg;
		if (root.ts[0].number_of_entries==1)
		{
			tm_t=it_k->t*root.ts[0].sample_duration[0];
		}
		else
			for (int i=0;i<root.ts[0].number_of_entries;i++)
			{
				num+=root.ts[0].sample_count[i];
				if (num<it_k->t)
				{
					tm_t+=root.ts[0].sample_count[i]*root.ts[0].sample_duration[i];
				}
				else if (num>=it_k->t)
				{
					tm_t+=(num-it_k->t)*root.ts[0].sample_duration[i];
					break;
				}
			}
		float time=tm_t/root.trk[0].mdia.mdhd.timescale;
		num=0;
		tm_t=0;
		t_tm.t_end=time;
		vc_t.push_back(t_tm);
	}
	return vc_t;
}

vector<uint32_t> soun_trak_find_sample( vector<CMp4_trak_box>::iterator t, float timeFrom, float timeTo,int num,CMp4_root_box root ) 
{
	int sng_i=0;
	int i=1,j=0;
	uint32_t tm_scale=t->mdia.mdhd.timescale;
	int time=0,time_f=floatToInt(timeFrom*tm_scale),time_to=floatToInt(timeTo*tm_scale);
	vector<uint32_t> vc;
	uint32_t sum_sample=0,chunkSampleSum=0;
	uint32_t sampleFrom=0,sampleTo=0;/*需要减掉的起止sample*/
	uint32_t chunkFrom=0,chunkTo=0;/*起止sample对应的chunk*/
	uint32_t first_sample_chunkfrom=0,first_sample_chunkto=0;

	if (root.ts[num].number_of_entries==1)
	{
		sampleFrom=time_f/root.ts[num].sample_duration[0];
		sampleTo=time_to/root.ts[num].sample_duration[0];
	}
	else
		for (int i=0;i<root.ts[num].number_of_entries;i++)
		{
			sum_sample+=root.ts[num].sample_count[i];
			time+=root.ts[num].sample_count[i]*root.ts[num].sample_duration[i];
			if (timeFrom==0)
			{
				sampleFrom=1;
			}	
			else if (time_f<=(root.ts[num].sample_duration[i]+time)&&sng_i==0)
			{
				sng_i++;
				int m=root.ts[num].sample_duration[i];
				sampleFrom=sum_sample+((time_f-time)/m);

			} 
			if(time_to<root.ts[num].sample_duration[i]+time)
			{
				int m=root.ts[num].sample_duration[i];
				sampleTo=sum_sample+((time_to-time)/m);
				break;
			}
		}
		if (time_f>=t->mdia.mdhd.duration)
		{
			return vc;
			//sampleFrom=sz[num].table_size;
		}
		if (sampleTo==0)
		{
			sampleTo=root.sz[num].table_size;
		}
		cout<<"sampleFrom_soun:"<<sampleFrom<<"  "<<"sampleTo_soun:"<<sampleTo<<endl;
		vc.push_back(sampleFrom);
		vc.push_back(sampleTo);
		/* 在stsc中查找sample对应的chunk*/
		for (;j<root.sc[num].map_amount;j++)
		{
			for (;i<=root.sc[num].scmap[j+1].first_chunk_num-1;i++)
			{
				chunkSampleSum+=root.sc[num].scmap[j].sample_amount_in_cur_table;

				if (sampleFrom<=chunkSampleSum&&first_sample_chunkfrom==0)
				{
					chunkFrom=i;
					first_sample_chunkfrom
						= get_sample_index(root.sc[num], chunkFrom);
				}
				if (sampleTo<=chunkSampleSum)
				{
					chunkTo=i;
					first_sample_chunkto
						= get_sample_index(root.sc[num], chunkTo);
					break;
				}
			}
			if (chunkTo!=0)
			{
				break;
			}
		}
		cout<<"first_sample_chunkfrom_soun:"<<first_sample_chunkfrom<<"  "<<"first_sample_chunkto_soun:"<<first_sample_chunkto<<endl;
		cout<<"chunkFrom_soun:"<<chunkFrom<<"  "<<"chunkTo_soun:"<<chunkTo<<endl;
		vc.push_back(chunkFrom);
		vc.push_back(chunkTo);
		/*在stco中确定偏移量*/
		uint32_t from_chunk_offset=0,to_chunk_offset=0;
		for (i=0;i<root.co[num].chunk_offset_amount;i++)
		{
			if (i+1==chunkFrom)
			{
				uint32_t m=root.co[num].chunk_offset_from_file_begin[i];
				from_chunk_offset=m;
				if (first_sample_chunkfrom==sampleFrom)
				{
					from_chunk_offset=m;
				}
				else
				{
					for (j=first_sample_chunkfrom;j<sampleFrom;j++)
					{
						from_chunk_offset+=root.sz[num].sample_size_table[j-1];
					}
				}
			}
			if(i+1==chunkTo){
				uint32_t m=root.co[num].chunk_offset_from_file_begin[i];
				to_chunk_offset=m;
				if (first_sample_chunkto==sampleTo)
				{
					to_chunk_offset=m;
				}
				else
				{
					for (j=first_sample_chunkto;j<sampleTo;j++)
					{
						to_chunk_offset+=root.sz[num].sample_size_table[j-1];
					}
				}
				break;
			}
		}
		cout<<"from_chunk_offset_soun:"<<from_chunk_offset<<"  "<<"to_chunk_offset_soun:"<<to_chunk_offset<<endl<<endl<<endl;
		vc.push_back(from_chunk_offset);
		vc.push_back(to_chunk_offset);
		return vc;
}

vector<uint32_t> video_trak_find_sample(vector<CMp4_trak_box>::iterator t,float timeFrom,float timeTo,int num, CMp4_root_box root){
	int sng_i=0;
	int i=1,j=0;
	uint32_t tm_scale=t->mdia.mdhd.timescale;
	int time=0,time_f=floatToInt(timeFrom*tm_scale),time_to=floatToInt(timeTo*tm_scale);
	vector<uint32_t> vc;
	uint32_t sum_sample=0,chunkSampleSum=0;
	uint32_t sampleFrom=0,sampleTo=0;/*需要减掉的起止sample*/
	uint32_t chunkFrom=0,chunkTo=0;/*起止sample对应的chunk*/
	uint32_t first_sample_chunkfrom=0,first_sample_chunkto=0;
	uint32_t sync_from=0,sync_to=0;
	if (root.ts[num].number_of_entries==1)
	{
		sampleFrom=time_f/root.ts[num].sample_duration[0];
		sampleTo=time_to/root.ts[num].sample_duration[0];
	}
	else
		for (int i=0;i<root.ts[num].number_of_entries;i++)
		{
			sum_sample+=root.ts[num].sample_count[i];
			time+=root.ts[num].sample_count[i]*root.ts[num].sample_duration[i];
			if (timeFrom==0)
			{
				sampleFrom=1;
			}	
			else if (time_f<=time&&sng_i==0)/*ts[num].sample_duration[i]+*/
			{
				sng_i++;
				int m=root.ts[num].sample_duration[i];
				sampleFrom=sum_sample+((time_f-time)/m);

			} 
			if(time_to<time)/*ts[num].sample_duration[i]+*/
			{
				int m=root.ts[num].sample_duration[i];
				sampleTo=sum_sample+((time_to-time)/m);
				break;
			}
		}
		if (sampleTo==0)
		{
			sampleTo=root.sz[num].table_size;
		}
		cout<<"sampleFrom:"<<sampleFrom<<"  "<<"sampleTo:"<<sampleTo<<endl;
		vc.push_back(sampleFrom);
		vc.push_back(sampleTo);
		/* 在stsc中查找sample对应的chunk*/
		for (;j<root.sc[num].map_amount;j++)
		{
			for (;i<=root.sc[num].scmap[j+1].first_chunk_num-1;i++)
			{
				chunkSampleSum+=root.sc[num].scmap[j].sample_amount_in_cur_table;

				if (sampleFrom<=chunkSampleSum&&first_sample_chunkfrom==0)
				{
					chunkFrom=i;
					first_sample_chunkfrom=chunkSampleSum-root.sc[num].scmap[j].sample_amount_in_cur_table+1;
				}
				if (sampleTo<=chunkSampleSum)
				{
					chunkTo=i;
					first_sample_chunkto=chunkSampleSum-root.sc[num].scmap[j].sample_amount_in_cur_table+1;
					break;
				}
			}
			if (chunkTo!=0)
			{
				break;
			}
		}
		cout<<"chunkFrom:"<<chunkFrom<<"  "<<"chunkTo:"<<chunkTo<<endl;
		cout<<"first_sample_chunkfrom:"<<first_sample_chunkfrom<<"  "<<"first_sample_chunkto:"<<first_sample_chunkto<<endl;
		vc.push_back(chunkFrom);
		vc.push_back(chunkTo);
		/*查找距离起止sample最近的关键帧*/
		int s=0;
		for (;s<root.box_ss.number_of_entries;s++)
		{
			if (sampleFrom<root.box_ss.sync_sample_table[s]&&sync_from==0)
			{
				sync_from=root.box_ss.sync_sample_table[s-1];
			}
			if (sampleTo<=root.box_ss.sync_sample_table[s]&&sync_to==0)
			{
				sync_to=root.box_ss.sync_sample_table[s];
			}
			if ((s==root.box_ss.number_of_entries-1)&&sync_to==0)
			{
				sync_to=root.box_ss.sync_sample_table[s];
			}
		}
		if (sampleTo>root.box_ss.sync_sample_table[root.box_ss.number_of_entries-1])
		{
			sync_to=root.sz[num].table_size;
		}
		cout<<"sync_from:"<<sync_from<<"  "<<"sync_to:"<<sync_to<<endl;
		vc.push_back(sync_from);
		vc.push_back(sync_to);
		/*查找关键帧所对应的chunk*/
		int m=1,n=0;
		uint32_t sync_chunkSampleSum=0;
		uint32_t sync_chunkFrom=0,sync_chunkTo=0;/*起止sync_sample对应的chunk*/
		uint32_t sync_first_sample_chunkfrom=0,sync_first_sample_chunkto=0;
		/* 在stsc中查找sync_sample对应的chunk*/
		for (;n<root.sc[num].map_amount;n++)
		{
			for (;m<=root.sc[num].scmap[n+1].first_chunk_num-1;m++)
			{
				sync_chunkSampleSum+=root.sc[num].scmap[n].sample_amount_in_cur_table;

				if (sync_from<=sync_chunkSampleSum&&sync_first_sample_chunkfrom==0)
				{
					sync_chunkFrom=m;
					sync_first_sample_chunkfrom=sync_chunkSampleSum-root.sc[num].scmap[n].sample_amount_in_cur_table+1;
				}
				if (sync_to<=sync_chunkSampleSum)
				{
					sync_chunkTo=m;
					sync_first_sample_chunkto=sync_chunkSampleSum-root.sc[num].scmap[n].sample_amount_in_cur_table+1;
					break;
				}

				//chunk++;
			}
			if (sync_chunkTo!=0)
			{
				break;
			}
		}
		cout<<"sync_chunkFrom:"<<sync_chunkFrom<<"  "<<"sync_chunkTo:"<<sync_chunkTo<<endl;
		cout<<"first_sample_chunkfrom:"<<sync_first_sample_chunkfrom<<"  "<<"first_sample_chunkto:"<<sync_first_sample_chunkto<<endl;
		vc.push_back(sync_chunkFrom);
		vc.push_back(sync_chunkTo);
		/*在stco中确定sync的偏移量*/
		uint32_t sync_from_chunk_offset=0,sync_to_chunk_offset=0;
		for (m=0;m<root.co[num].chunk_offset_amount;m++)
		{
			if (m+1==sync_chunkFrom)
			{
				uint32_t q=root.co[num].chunk_offset_from_file_begin[m];
				sync_from_chunk_offset=q;
				if (sync_first_sample_chunkfrom==sync_from)
				{
					sync_from_chunk_offset=q;
				}
				else
				{
					for (n=sync_first_sample_chunkfrom;n<sync_from;n++)
					{					
						sync_from_chunk_offset+=root.sz[num].sample_size_table[n];					
					}
				}
			}
			if(m+1==sync_chunkTo){
				uint32_t p=root.co[num].chunk_offset_from_file_begin[m];
				sync_to_chunk_offset=p;
				if (sync_first_sample_chunkto==sync_to)
				{
					sync_to_chunk_offset=p;
				}
				else
				{
					for (n=sync_first_sample_chunkto;n<sync_to;n++)
					{					
						sync_to_chunk_offset+=root.sz[num].sample_size_table[n-1];
					}
				}
				break;
			}
		}
		cout<<"sync_from_chunk_offset:"<<sync_from_chunk_offset<<"  "<<"sync_to_chunk_offset:"<<sync_to_chunk_offset<<endl<<endl<<endl;
		vc.push_back(sync_from_chunk_offset);
		vc.push_back(sync_to_chunk_offset);

		return vc;
}

vector<t_time> videoTime_To_soundTime_segment( vector<t_time> vc_time, vector<varible> sample_key ,CMp4_root_box root) 
{
	vector<t_time> vc_t;
	t_time t_tm;
	uint32_t num=0;
	double tm_t=0;
	vector<t_time>::iterator it_t=vc_time.begin();
	for (auto it_k=sample_key.begin();it_k<sample_key.end();it_k++,it_t++)
	{
		t_tm.t_end=it_t->t_end;
		if (root.ts[0].number_of_entries==1)
		{
			tm_t=it_k->f*root.ts[0].sample_duration[0];
		}
		else
			for (int i=0;i<root.ts[0].number_of_entries;i++)
			{
				num+=root.ts[0].sample_count[i];
				if (num<it_k->f)
				{
					tm_t+=root.ts[0].sample_count[i]*root.ts[0].sample_duration[i];
				}
				else if (num>=it_k->f)
				{
					tm_t+=(num-it_k->f)*root.ts[0].sample_duration[i];
					break;
				}
			}
			float time=tm_t/root.trk[0].mdia.mdhd.timescale;
			t_tm.t_beg=time;
			vc_t.push_back(t_tm);
	}
	return vc_t;
}


int process_cut_chunk_data( FILE * fin, FILE * fout, int chunk_index,
	uint32_t chunkfrom, uint32_t sync_chunkTo,uint32_t sampleFrom, 
	uint32_t sync_to_chunk_offset, uint32_t sync_to,int num ,CMp4_root_box root) 
{
	int i=0;
	if (chunk_index+1==chunkfrom)
	{
		uint32_t sample_num_in_cur_chunk_
			= get_sample_num_in_cur_chunk(root.sc[num], chunk_index+1);  //@a mark获取chunk中sample的数目
		uint32_t sample_index_
			= get_sample_index(root.sc[num], chunk_index+1);//chunk中第一个sample的序号
		_fseeki64(fin,root.co[num].chunk_offset_from_file_begin[chunk_index],SEEK_SET);
		unsigned int sam_off=_ftelli64(fin);
		for (int a=sample_index_;a<sampleFrom;a++)
		{
			uint32_t sample_size_ = get_sample_size(root.sz[num], a);//获取当前sample的大小
			//int buf=sample_size_+4;
			_fseeki64(fin,sam_off,SEEK_SET);
			char *ptr1=new char [sample_size_];
			fread(ptr1, sample_size_, 1, fin);
			fwrite(ptr1, sample_size_, 1, fout);
			delete [] ptr1;
			sam_off+=sample_size_;
		}
	}
	if (chunk_index+1==sync_chunkTo&&sync_chunkTo!=root.co[num].chunk_offset_amount)
	{
		uint32_t sample_num_in_cur_chunk_
			= get_sample_num_in_cur_chunk(root.sc[num], chunk_index+1);  //@a mark获取chunk中sample的数目
		uint32_t sample_index_
			= get_sample_index(root.sc[num], chunk_index+1);//chunk中第一个sample的序号
		_fseeki64(fin,sync_to_chunk_offset,SEEK_SET);
		unsigned int to_off=_ftelli64(fin);
		if (sync_to!=root.sz[num].table_size)
		{
			for (int a=sync_to;a<sample_index_+sample_num_in_cur_chunk_;a++)
			{
				uint32_t sample_size_ = get_sample_size(root.sz[num], a);//获取当前sample的大小
				_fseeki64(fin,to_off,SEEK_SET);
				char *ptr1=new char [sample_size_];
				fread(ptr1, sample_size_, 1, fin);
				fwrite(ptr1, sample_size_, 1, fout);
				delete [] ptr1; 
				to_off+=sample_size_;
			}
		}

		i=1;
	}
	return i;
}

void copy_chunk_data(FILE *fin,
	const uint32_t chunk_index,  //[0, end) 标识为第几个Chunk
	FILE *fout,  
	int num,
	CMp4_root_box root
	)
{
	_fseeki64(fin, root.co[num].chunk_offset_from_file_begin[chunk_index],
		SEEK_SET);

	//获取当前chunk中有多少个sample
	uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(root.sc[num], chunk_index+1);  //@a mark获取chunk中sample的数目
	uint32_t sample_index_ =  get_sample_index(root.sc[num], chunk_index+1);//chunk中第一个sample的序号
	unsigned int cur=_ftelli64(fin);
	for(int i = 0; i < sample_num_in_cur_chunk_; i++)
	{
		uint32_t sample_size_ = get_sample_size(root.sz[num], sample_index_+i);//获取当前sample的大小
		_fseeki64(fin,cur,SEEK_SET);
		char *ptr=new char [sample_size_];
		fread(ptr, sample_size_, 1, fin);
		fwrite(ptr, sample_size_, 1, fout);
		delete [] ptr;
		cur+=sample_size_;
	}
}

void copy_sample_data(FILE *fin,
	const uint32_t chunk_index,  //[0, end) 标识为第几个Chunk
	std::string name, 
	int num,
	CMp4_root_box root,
	 int& nSampleId
	)
{
	_fseeki64(fin, root.co[num].chunk_offset_from_file_begin[chunk_index],
		SEEK_SET);

	//获取当前chunk中有多少个sample
	uint32_t sample_num_in_cur_chunk_ = get_sample_num_in_cur_chunk(root.sc[num], chunk_index+1);  //@a mark获取chunk中sample的数目
	uint32_t sample_index_ =  get_sample_index(root.sc[num], chunk_index+1);//chunk中第一个sample的序号
	unsigned int cur=_ftelli64(fin);
	for(int i = 0; i < sample_num_in_cur_chunk_; i++)
	{


		uint32_t sample_size_ = get_sample_size(root.sz[num], sample_index_+i);//获取当前sample的大小
		_fseeki64(fin,cur,SEEK_SET);
		char *ptr=new char [sample_size_];
		fread(ptr, sample_size_, 1, fin);
		
		uint32_t sample_time = get_sample_time(root.ts[num], nSampleId );
		
		char char_num[260];
		sprintf(char_num, "%u_%u", nSampleId, sample_time);
		//sprintf(char_num, "E:/%s/%u_%u", name.c_str(),nSampleId,sample_time);

	

		FILE *fout =	fopen(std::string(name + "//" + name + char_num).c_str(), "w");

		if(fout == (FILE*)0){
			printf("error\n");
			std::exit(-1);
		}
	
		//写一帧数据 --- 可以直接进行网络推送
		fwrite(ptr, sample_size_, 1, fout);

		delete [] ptr;
		cur+=sample_size_;
		nSampleId++;

		fclose(fout);
	}
}

void Error_corr( CMp4_root_box root, vector<t_time> ch_time, vector<t_time> vc_time1, vector<varible> skv, vector<varible> skcv, vector<varible> skcov, 
	vector<varible> &sample_key_video, vector<varible> &sample_key_chunk_video, vector<varible> &sync_chunk_offset_video, int i ) 
{
	if (ch_time.end()-ch_time.begin()+1==vc_time1.end()-vc_time1.begin())
	{
		if (i<vc_time1.end()-vc_time1.begin()-1&&skv[i].f!=sample_key_video[i].t)
		{
			sample_key_video[i].t=skv[i].f;
			sample_key_chunk_video[i].t=skcv[i].f;
			sync_chunk_offset_video[i].t=skcov[i].f;
		}
	}
	else 
	{
		if (vc_time1[0].t_beg!=0&&i<ch_time.end()-ch_time.begin()-1&&skv[i+1].f!=sample_key_video[i].t)
		{
			int vc_num=vc_time1.end()-vc_time1.begin()-1;
			if (vc_time1[vc_num].t_end==(float)root.trk[0].mdia.mdhd.duration/root.trk[0].mdia.mdhd.timescale)
			{
				if (i<vc_time1.end()-vc_time1.begin()-1)
				{
					sample_key_video[i].t=skv[i+1].f;
					sample_key_chunk_video[i].t=skcv[i+1].f;
					sync_chunk_offset_video[i].t=skcov[i+1].f;	
				}
			}
			else
			{
				sample_key_video[i].t=skv[i+1].f;
				sample_key_chunk_video[i].t=skcv[i+1].f;
				sync_chunk_offset_video[i].t=skcov[i+1].f;
			}
		}
		else if (vc_time1[0].t_beg==0&&i<ch_time.end()-ch_time.begin()-1&&skv[i+1].f!=sample_key_video[i].t)
		{
			sample_key_video[i].t=skv[i].f;
			sample_key_chunk_video[i].t=skcv[i].f;
			sync_chunk_offset_video[i].t=skcov[i].f;
		}
	}//误差修正
}