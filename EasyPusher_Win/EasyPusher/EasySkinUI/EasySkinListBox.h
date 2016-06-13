/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once

#include "EasySkinUI.h"

class  CEasySkinListBox : public CListBox,public IEasySkinControl
{
	DECLARE_DYNAMIC(CEasySkinListBox)

	//基本变量
protected:
	int								m_nHovenItem;
	vector<tagItemStruct>			m_ItemStruct;
	int								nItemHeight;

	//资源定义
protected:
	CImageEx 						* m_pBackImgN;			//正常资源
	CImageEx 						* m_pBackImgH;			//正常资源
	CImageEx 						* m_pSelectImg;			//选中资源

	//函数定义
public:
	//构造函数
	CEasySkinListBox(void);
	//析构函数
	~CEasySkinListBox(void);

	//功能函数
public:
	//是否单选
	bool IsSingleSel();
	//增加文本
	void AddString(LPCTSTR lpszItem);

	void InsertItem();

	//设置高度
	int SetItemHeight( int nIndex, UINT cyItemHeight );
	//删除文本
	int DeleteString( UINT nIndex );

	//资源加载
public:
	//正常背景
	BOOL SetBackNormalImage(LPCTSTR lpszFileName, CONST LPRECT lpNinePart = NULL);
	//选中资源
	BOOL SetSelectImage(LPCTSTR lpszFileName, CONST LPRECT lpNinePart = NULL);
	//节点高亮
	BOOL SetHovenImage(LPCTSTR lpszFileName, CONST LPRECT lpNinePart = NULL);

	//消息定义
private:
	//窗口销毁
	afx_msg void OnDestroy();
	//鼠标右键
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//非客户区
	afx_msg void OnNcPaint();

	//重载函数
protected:
	//绘制函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);

protected:
	DECLARE_MESSAGE_MAP()
};


