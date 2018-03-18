
////////////////////////////////////////////////////////////////////
//
//	视频特技制作Dll导出类定义
//  VideoVFXMakerInterface.h 
//
////////////////////////////////////////////////////////////////////

#ifndef VIDEOVFXMAKERINTERFACE_H
#define VIDEOVFXMAKERINTERFACE_H

#ifdef __cplusplus

//#define FENBUSHI_LUBO 1
//为了实现而做的纯虚函数
//因为C＋＋中引入的话就需要源代码，然而按这样去做的话才能只要文件头不用代码了。

#define FILTER_NORESIZE 0
#define FILTER_RESIZEMIN 1
#define FILTER_RESIZEMAX 2
#define MAX_VIV_VFX_COUNT 6
/*
#ifndef WATERMARK_ENTRY_TYPE
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
#endif*/

class VideoVFXMaker_Interface
{
//导出类导出函数声明
public:
	virtual void WINAPI SetVideoPlatFormSpeed(int nWidthSpeed,int nHeightSpeed,int nFreezeSpeed) = 0;
	virtual void WINAPI SetVideoPlatFormVideo(int nVideoWidthSrc,int nVideoHeightSrc,char* strBytesSrc,int nVideoWidthDes,
		int nVideoHeightDes,char* strBytesDes) = 0;
	virtual void WINAPI SetVideoPlatFormModel(int nModePlatForm) = 0;
	virtual BOOL WINAPI SetPlatFormStart()= 0;
	virtual int WINAPI DoVideoPlatFormChange(BYTE *pSrc,BYTE *pDes,long lLen,BOOL bCopy=FALSE) = 0;
	virtual void WINAPI SetPlatFormState(int nStatePlatForm) = 0;
	virtual int WINAPI GetPlatFormState() = 0;
public:
	virtual void WINAPI DoVideoBackGroundChange(BYTE *pSrc,int nSrcLen=0) = 0;//VideoBackGroundChange(BYTE *pSrc,int nWidth,int nHeight,CString strBytes);
	virtual void WINAPI DefaultBackGroundData() = 0;
	virtual BOOL WINAPI SetBackGround(char*strBackGroundPath,char* strDesBytes,BOOL bAutoConver=TRUE,BOOL bDelSrc=TRUE) = 0;
	virtual void WINAPI ReleaseBackGroundData() = 0;
	virtual BYTE* WINAPI GetVideoHeaderBuf() = 0;
public:
	virtual void WINAPI RealseOverlayTitle() = 0;
	virtual void WINAPI CreateOverlayTitle(int nVideoWidth,int nVideoHeight,char* strRGBBtyes) = 0;
	virtual void WINAPI SetOverlayTitleInfo(char *pTitle,LOGFONTA fontTitle,int nColorRed,int nColorGreen,int nColorBlue,POINT pointStart) = 0;
	virtual void WINAPI SetOverlayTitleState(int nState) = 0;
	virtual int WINAPI  DoOverlayTitle(PBYTE pInData) = 0;
public:
	virtual int WINAPI SetBackGroundTitle(char *pTitle,LOGFONTA fontTitle,int nColorRed,int nColorGreen,int nColorBlue,POINT pointStart) = 0;
	virtual int WINAPI CreateBackGroundTitle() = 0;
	virtual void WINAPI ReSizeYUY2Data(BYTE*scrdata,int scrwidth,int scrheight,BYTE*desdata,int deswidth,int desheight,int desx=0,int desy=0) = 0;
//设置画中画视频参数
	virtual void WINAPI SetVideoInVideoParam(int nVivVfxMode,int nXPos, int nYPos, int nSrcWidth, 
		int nSrcHeight, int nDesWidth, int nDesHeight, int nFrameCount) = 0;
	virtual void WINAPI SetVideoInVideoVfxSpeed(int nWidthSpeed,int nHeightSpeed,int nFreezeSpeed) = 0;
	virtual BOOL WINAPI SetVIVStart(int nVivVfxMode) = 0;
//画中画特效随意缩放位置设置
	virtual void WINAPI SetPlatDIY(int desx,int desy,int deswidth,int desheight,int flag=FILTER_RESIZEMIN) = 0;
//LOG水印相关
	virtual void WINAPI InitWaterMask(byte* prgb32, int x, int y, int width, int height,int color=32,WATERMARK_ENTRY_TYPE flag=WATERMARK_TYPE_COVER) = 0;
	virtual void WINAPI AddWaterMask(byte*scr) = 0;
//	virtual void WINAPI SetWaterMarkParam(BOOL bIsUseWaterMark, int nPosX, int nPosY, WATERMARK_ENTRY_TYPE eWaterMarkType = WATERMARK_TYPE_COVER) = 0;
	virtual int  WINAPI SetLogoImage(char* strPicPath, int nPosX, int nPosY,BOOL bIsUseWaterMark, WATERMARK_ENTRY_TYPE eWaterMarkType = WATERMARK_TYPE_COVER) = 0;
	virtual void WINAPI ReDrawYUY2DataBack(BYTE*scrdata,int scrwidth,int scrheight,int deep,int red,int green,int blue)=0;
	virtual void WINAPI SetVIVZoomType(int nVivZoomType=0) = 0;
};

//定义接口指针类型
typedef	VideoVFXMaker_Interface*	LPVideoVFXMaker;	

LPVideoVFXMaker	APIENTRY Create_VideoVFXMaker();//创建控制接口指针
void APIENTRY Release_VideoVFXMaker(LPVideoVFXMaker lpVideoVFXMaker);//销毁控制接口指针


#endif

#endif