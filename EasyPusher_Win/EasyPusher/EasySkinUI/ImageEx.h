/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef IMAGE_HEAD_FILE
#define IMAGE_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

//////////////////////////////////////////////////////////////////////////////////

//图片对象
class  CImageEx: public CImage
{
	//变量定义
protected:
	RECT							m_rcNinePart;

	//函数定义
public:
	//构造函数
	CImageEx();
	//析构函数
	virtual ~CImageEx();

	//设置函数
public:
	//九宫格绘图
	void SetNinePart(CONST LPRECT lprcNinePart);
	//Alpha处理
	bool SetAlphaBit();
	//转HBITMAP
	HBITMAP ImageToBitmap();
	//灰度处理
	bool SetGray();

	//管理函数
public:
	//销毁图片
	bool DestroyImage();
	//加载图片
	bool LoadImage(LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName,LPCTSTR pszResourceType=TEXT("IMAGE"));

	//绘画函数
public:
	//绘画图像
	BOOL DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//绘画图像
	BOOL DrawImage(CDC * pDC, INT nXPos, INT nYPos, INT nDestWidth, INT nDestHeight);
	//绘画图像
	BOOL DrawImage(CDC * pDC, RECT &rc);
	//绘画图像
	BOOL DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc);
	//绘画图像
	BOOL DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc, INT nSrcWidth, INT nSrcHeight);

	//区域绘制
public:
	//绘画图像
	bool Draw( CDC * pDC, INT x, INT y, INT cx, INT cy,INT nLeft,INT nTop,INT nRight,INT nBottom );
	//绘画图像
	bool Draw( CDC * pDC, const RECT& rectDest, const RECT& rectSrc );
	//绘画图像
	bool Draw( CDC * pDC, const RECT& rectDest );

	//拉伸绘制
public:
	//
	bool DrawExtrude(CDC*pDC,const RECT& rectDest,bool bLeft=false,int nPixel=1);
};

//////////////////////////////////////////////////////////////////////////////////

#endif