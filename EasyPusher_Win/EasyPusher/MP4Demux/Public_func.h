#ifndef PUBLIC_FUNC_H
#define PUBLIC_FUNC_H

#include "inttype.h"
#include <iostream>
#include <vector>
#include "Mp4_root_box.h"
#include "Public_class.h"

//获取一个moov中有几个track
int get_track_size_in_moov(CMp4_root_box& box);
//找到moov中的音（"audio"）、视频("video")轨
int get_track_id_by_name(CMp4_root_box& box, std::string name );
int get_video_info_in_moov(CMp4_root_box& box, CMp4_avcC_box& videoInfo);
int get_audio_info_in_moov(CMp4_root_box& box, CMp4_mp4a_box& audioInfo);

uint32_t get_sample_num_in_cur_chunk(const CMp4_stsc_box & box,	const uint32_t chunk_index);//获得当前chunk中的sample数

uint32_t get_sample_index(const CMp4_stsc_box &box,	const uint32_t chunk_index);//获得当前chunk中的第一个sample号

uint32_t get_sample_size(CMp4_stsz_box &box,const uint32_t chunk_index);//获得sample的大小

// 获取stts中某个sample的时间(映射下一帧到来的时间)
uint32_t get_sample_time(CMp4_stts_box& box, const uint32_t sample_index);

int floatToInt(float f);//float转int，四舍五入

vector<t_time> videoTime_To_soundTime(vector<t_time> vc_time, 
	vector<varible> sample_key,CMp4_root_box root );//videotime转soundtime

vector<uint32_t> soun_trak_find_sample( vector<CMp4_trak_box>::iterator t, 
	float timeFrom, float timeTo,int num,CMp4_root_box root);//soun根据时间查对应的sample

vector<uint32_t> video_trak_find_sample(vector<CMp4_trak_box>::iterator t,
	float timeFrom,float timeTo,int num,CMp4_root_box root);//trak根据时间查对应的sample

vector<t_time> videoTime_To_soundTime_segment( vector<t_time> vc_time, 
	vector<varible> sample_key,CMp4_root_box root );//生成片段时，videotime转sountime 

int process_cut_chunk_data( FILE * fin, FILE * fout, int chunk_index,
	uint32_t chunkfrom, uint32_t sync_chunkTo,uint32_t sampleFrom, 
	uint32_t sync_to_chunk_offset, uint32_t sync_to,int num,CMp4_root_box root );	//处理需要删除的chunk数据


void copy_chunk_data(FILE *fin,
	const uint32_t chunk_index,  //[0, end) 标识为第几个Chunk
	FILE *fout,  
	int num,
	CMp4_root_box root
	);


void copy_sample_data(FILE *fin,
	const uint32_t chunk_index,  //[0, end)
	std::string name, 
	int num,CMp4_root_box root, 
	int& nSampleId);					//复制一个chunk的数据
	

void Error_corr( CMp4_root_box root, vector<t_time> ch_time, vector<t_time> vc_time1, vector<varible> skv, vector<varible> skcv, vector<varible> skcov, 
	vector<varible> &sample_key_video, vector<varible> &sample_key_chunk_video, vector<varible> &sync_chunk_offset_video, int i ) ;
#endif