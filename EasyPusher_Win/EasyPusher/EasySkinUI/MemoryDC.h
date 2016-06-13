/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef MEMORY_DC_HEAD_FILE
#define MEMORY_DC_HEAD_FILE

#pragma once

#include "EasySkinUI.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//内存DC类
class  CMemoryDC : public CDC 
{
	//变量定义
private:
	CBitmap						m_bitmap;										// 屏幕外位图
	CBitmap*					m_oldBitmap;									// 最初位图对象
	CDC*						m_pDC;											// 保存经过构造CDC
	CRect						m_rect;											// 长方形绘制区域
	BOOL						m_bMemDC;										// 是否为内存DC

	//函数定义
public:
	//构造函数定义
	CMemoryDC(CDC* pDC, CRect rect = CRect(0,0,0,0), BOOL bCopyFirst = FALSE);

	//析构函数
	~CMemoryDC();

	// 允许使用指针
	CMemoryDC* operator->();

	// 允许使用指针
	operator CMemoryDC*();
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

