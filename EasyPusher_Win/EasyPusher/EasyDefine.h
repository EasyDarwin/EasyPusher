/* 标准头文件 */
#ifndef __EASYDEFINE_H__
#define __EASYDEFINE_H__

#if defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#define  usleep(us)  Sleep((us)/1000)
#else
// todo..
#endif

//水印结构信息
// //枚举标识台标LOGO所在的位置
typedef enum tagWATER_MARK_POS
{
	POS_LEFT_TOP = 1,
	POS_RIGHT_TOP,
	POS_LEFT_BOTTOM,
	POS_RIGHT_BOTTOM

}WATER_MARK_POS;

typedef enum tagWATERMARK_ENTRY_TYPE
{
	WATERMARK_TYPE_COVER			   = 0,
	WATERMARK_TYPE_OVERLYING		   = 1,
	WATERMARK_TYPE_OVERLYING_COVER	   = 2,
	WATERMARK_TYPE_ROLL_TO_LEFT		   = 3,
	WATERMARK_TYPE_JUMP_UP_DOWN		   = 4,
	WATERMARK_TYPE_ROLL_AND_JUMP	   = 5,
	WATERMARK_TYPE_TROTTING_HORSE_LAMP = 6

}WATERMARK_ENTRY_TYPE;

// //字幕信息
typedef struct tagVideoTittleInfo
{
	int nState;//字幕状态：	nState==1运行；nState==0暂停；nState==-1销毁
	//设置字体等信息
	int nTittleWidth;
	int nTittleHeight;
	int nFontWeight;//权重 FW_NORMAL FW_BOLD、、、
	char strFontType[64];//字体 "华文新魏");//"华文隶书");"隶书"
	char strTittleContent[512];//字幕内容
	// 字幕颜色
	int nColorR;
	int nColorG;
	int nColorB;
	POINT ptStartPosition;//字幕绘制开始点
	int   nMoveType;//0--固定位置，1--从左往右，2--从右往左，
	BOOL bResetTittleInfo;
}VideoTittleInfo;

typedef struct tagWaterMarkInfo
{
	BOOL bIsUseWaterMark;//是否使用水印
	WATER_MARK_POS eWaterMarkPos;//台标位置：1==leftttop 2==righttop 3==leftbottom 4==rightbottom
	int nLeftTopX;//水印左上角位置x
	int nLeftTopY;//水印左上角位置y
	int nWidth;//宽
	int nHeight;//高
	char strWMFilePath[512];//水印图片路径
	WATERMARK_ENTRY_TYPE eWatermarkStyle;//水印的风格
	BOOL bResetWaterMark;

}WaterMarkInfo;

//特技制作结构
typedef struct tagVFXMakerInfo
{
	WaterMarkInfo warkMarkInfo;
	VideoTittleInfo tittleInfo;//字幕信息123
}VFXMakerInfo;

#if 0
typedef struct tagMediaInfo
{
	//for stream
	int nBitsRate;
	//for Video
	int nVCodec;
	char sVCodec[32];
	char sVCodecDetails[64];
	int nWidth;
	int nHeight;
	int nFrameRate;
	int video_bit_rate;
	int video_total_bit_rate;
	//for Audio
	int nACodec;
	char sACodec[32];
	char sACodecDetails[64];
	int nSampleRate;
	int nChannels;
	int nBitsPerSample;
	int audio_bit_rate;

}MediaInfo;
#endif

#endif


