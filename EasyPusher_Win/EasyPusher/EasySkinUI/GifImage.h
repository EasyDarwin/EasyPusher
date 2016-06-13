/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef GIF_IMAGE_HEAD_FILE
#define GIF_IMAGE_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

//////////////////////////////////////////////////////////////////////////
//GIF 枚举定义

//类型枚举
enum enGIFBlockTypes
{
	BLOCK_UNKNOWN,					//未知类型
	BLOCK_APPEXT,					//图形控制
	BLOCK_COMMEXT,					//注释标签
	BLOCK_CONTROLEXT,				//图形标签
	BLOCK_PLAINTEXT,				//图形标签
	BLOCK_IMAGE,					//图象标识
	BLOCK_TRAILER					//文件终结
};

//扩展枚举
enum enControlExtValues
{
	GCX_PACKED_DISPOSAL,			//处置方法
	GCX_PACKED_USERINPUT,			//输入标志
	GCX_PACKED_TRANSPCOLOR			//透明标志
};

//显示描述
enum enLSDPackedValues
{
	LSD_PACKED_GLOBALCT,			//颜色标志
	LSD_PACKED_CRESOLUTION,			//颜色深度
	LSD_PACKED_SORT,				//分类标志
	LSD_PACKED_GLOBALCTSIZE			//全局颜色
};

//描述枚举
enum enIDPackedValues
{
	ID_PACKED_LOCALCT,				//颜色标志
	ID_PACKED_INTERLACE,			//交织标志
	ID_PACKED_SORT,					//分类标志
	ID_PACKED_LOCALCTSIZE			//颜色大小
};

//////////////////////////////////////////////////////////////////////////
//GIF 结构定义

#pragma pack(1)

//文件头结构
struct tagGIFHeader
{
	BYTE							cbSignature[3];						//GIF 署名
	BYTE							cbVersion[3];						//GIF 版本
};

//逻辑屏幕标识符
struct tagGIFLSDescriptor
{
	WORD							wWidth;								//屏幕宽度
	WORD							wHeight;							//屏幕高度
	BYTE		 					cbPacked;							//颜色标志
	BYTE							cbBkIndex;							//背景颜色
	BYTE							cbPixelAspect;						//像素宽高比
};

//图象标识符
struct tagGIFImageDescriptor
{
	BYTE							cbImageSeparator;					//图象标识符
	WORD							wXExcursion;						//X 偏移量
	WORD							wYExcursion;						//Y 偏移量
	WORD							wWidth;								//图象宽度
	WORD							wHeight;							//图象高度
	BYTE							cbPacked;							//颜色标志
};

//图形控制扩展
struct tagGIFControlExt
{
	BYTE							cbExtIntroducer;					//标识标志
	BYTE							cbControlLabel;						//标识标志
	BYTE							cbBlockSize;						//区块大小
	BYTE							cbPacked;							//处置方法
	WORD							wDelayTime;							//延迟时间
	BYTE							cbTColorIndex;						//透明色索引
	BYTE							cbBlockTerm;						//块终结器
};

//注释扩展
struct tagGIFCommentExt
{
	BYTE							cbExtIntroducer;					//扩展标识
	BYTE							cbCommentLabel;						//注释标签
};

//图形文本扩展
struct tagGIFPlainTextExt
{
	BYTE							cbExtIntroducer;					//扩展标识
	BYTE							cbPlainTextLabel;					//图形标签
	BYTE							cbBlockSize;						//区块大小
	WORD							wLeftPos;							//边界位置
	WORD							wTopPos;							//边界位置
	WORD							wGridWidth;							//文本框宽
	WORD							wGridHeight;						//文本框高
	BYTE							cbCellWidth;						//单元宽度
	BYTE							cbCellHeight;						//单元高度
	BYTE							cbForeGroundColor;					//颜色索引
	BYTE							cbBackGroundColor;					//颜色索引
};

//应用程序扩展
struct tagGIFAppExtension
{
	BYTE							cExtIntroducer;						//扩展标识					
	BYTE							cExtLabel;							//图形标签
	BYTE							cBlockSize;							//区块大小
	char							cAppIdentifier[8];					//程序标识
	char							cAppAuth[3];						//程序鉴别
};

#pragma pack()

//////////////////////////////////////////////////////////////////////////
//结构定义

//显示结构
struct tagGIFFrameInfo
{
	UINT							nDelay;								//显示延时
	UINT							nDisposal;							//处理方式
	WORD							wXExcursion;						//图像偏移
	WORD							wYExcursion;						//图像偏移
	WORD							wImageWidth;						//图像宽度
	WORD							wImageHeight;						//图像高度
	IPicture *						pIFramePicture;						//图像接口
};

//数据信息
struct tagImageDataInfo
{
	LONG							lDataSize;							//数据大小
	LONG							lCurentPos;							//当前指针
	LONG							lColorTableSize;					//颜色大小
	LPBYTE							cbImageDataBuffer;					//图形数据
};

//数组定义
typedef CArray<tagGIFFrameInfo,tagGIFFrameInfo>	CGIFFrameInfoArray;

//////////////////////////////////////////////////////////////////////////

//动画对象
class  CGIFImage
{
	//属性变量
protected:
	WORD							m_wImageWidth;						//屏幕宽度
	WORD							m_wImageHeight;						//屏幕高度
	COLORREF						m_crBackGroundColor;				//背景颜色

	//内核变量
protected:
	INT_PTR							m_nCurrentFrame;					//当前图帧
	CGIFFrameInfoArray				m_GIFFrameInfoArray;				//片断数组

	//函数定义
public:
	//构造函数
	CGIFImage();
	//析构函数
	virtual ~CGIFImage();

	//加载函数
public:
	//加载动画
	bool LoadFromFile(LPCTSTR pszFileName);
	//加载动画
	bool LoadFromMemroy(BYTE * pcbData, LONG lDataSize);
	//加载动画
	bool LoadFromResource(LPCTSTR pszResourceName, LPCTSTR pszResourceType);

	//控制函数
public:
	//是否加载
	bool IsNull();
	//释放图像
	VOID DestroyImage();

	//信息函数
public:
	//图像宽度
	WORD GetWidth() { return m_wImageWidth; }
	//图像高度
	WORD GetHeight() { return m_wImageHeight; }
	//当前图帧
	INT_PTR GetCurrentIndex() { return m_nCurrentFrame; }

	//图帧函数
public:
	//当前图帧
	tagGIFFrameInfo * GetCurrentFrame();
	//图像数目
	INT_PTR GetFrameCount() { return m_GIFFrameInfoArray.GetCount(); }

	//绘画函数
public:
	//绘画图像
	VOID DrawImageFrame(CDC * pDC, INT nXPos, INT nYPos);
	//绘画图像
	VOID DrawImageFrame(CDC * pDC, INT nXPos, INT nYPos, INT_PTR nFrameIndex);

	//区块函数
protected:
	//切换区块
	bool SwitchNextBlock(tagImageDataInfo & ImageDataInfo);
	//区块长度
	LONG GetCurrentBlockSize(tagImageDataInfo & ImageDataInfo);
	//区块长度
	LONG GetSubBlockSize(tagImageDataInfo & ImageDataInfo, LONG lBlockPos);
	//获取类型
	enGIFBlockTypes GetCurrentBlockType(tagImageDataInfo & ImageDataInfo);

	//内部函数
private:
	//获取数值
	inline INT GetPackedValue(const tagGIFControlExt * pGIFControlExt, enum enControlExtValues Value);
	//获取数值
	inline INT GetPackedValue(const tagGIFLSDescriptor * pGIFLSDescriptor, enum enLSDPackedValues Value);
	//获取数值
	inline INT GetPackedValue(const tagGIFImageDescriptor * pGIFControlExt, enum enIDPackedValues Value);
};

//////////////////////////////////////////////////////////////////////////

#endif