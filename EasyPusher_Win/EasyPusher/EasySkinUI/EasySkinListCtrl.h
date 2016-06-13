/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef EasySkin_LISTCTRL_HEAD_FILE
#define EasySkin_LISTCTRL_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

//////////////////////////////////////////////////////////////////////////////////

//排序位置
#define SORT_AFTER					1									//后面位置
#define SORT_FRONT					-1									//前面位置

//////////////////////////////////////////////////////////////////////////////////
//枚举定义

//排序类型
enum enSortType
{
	SortType_NoSort,				//没有排序
	SortType_Ascend,				//升序排序
	SortType_Descale,				//降序排序
};

//////////////////////////////////////////////////////////////////////////////////

//列头控件
class  CEasySkinHeaderCtrl : public CHeaderCtrl,public IEasySkinControl
{
	//锁定列表
protected:
	UINT							m_uLockCount;						//锁定数目
	UINT							m_uItemHeight;						//子项高度
	UINT							m_uActiveItem;						//
	bool							m_bPress;

	//资源定义
public:
	CImageEx * m_pBackImg, * m_pPressImg, *m_pGridImg;

	//函数定义
public:
	//构造函数
	CEasySkinHeaderCtrl();
	//析构函数
	virtual ~CEasySkinHeaderCtrl();

	//资源加载
public:
	//设置资源
	BOOL SetBackImage(LPCTSTR lpNormal,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetPressImage(LPCTSTR lpNormal,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetGridImage(LPCTSTR lpNormal);

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();
	//控件消息
	virtual BOOL OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult);

	//功能函数
public:
	//设置锁定
	VOID SetLockCount(UINT uLockCount);
	//设置列高
	VOID SetItemHeight(UINT uItemHeight);

	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//销毁消息
	afx_msg void OnDestroy();
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//左键抬起
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////
struct  tagItemImage 
{
	CImageEx		*pImage;
	int				nItem;
};

typedef vector<tagItemImage>	CItemImgArray;

//////////////////////////////////////////////////////////////////////////

//列表控件
class  CEasySkinListCtrl : public CListCtrl,public IEasySkinControl
{
	//数据变量
protected:
	bool							m_bAscendSort;						//升序标志
	int								m_nHeightItem;
	UINT							m_uActiveItem;
	CItemImgArray					m_ItemImgArray;

	//资源定义
public:
	CImageEx * m_pHovenImg, * m_pSelectImg;
	CImageEx * m_pCheckImg, * m_pUnCheckImg;

	//变量定义
public:
	CEasySkinHeaderCtrl					m_EasySkinHeaderCtrl;					//列头控件

	//函数定义
public:
	//构造函数
	CEasySkinListCtrl();
	//析构函数
	virtual ~CEasySkinListCtrl();

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//Report设置资源
public:
	//设置资源
	BOOL SetHovenImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetSelectImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetCheckImage(LPCTSTR lpszCheckName,LPCTSTR lpszUnCheckName);
	//插入图标
	BOOL InsertImage(int nItem,LPCTSTR lpszFileName);

	//伪重载函数
public:
	//插入节点
	int InsertItem(const LVITEM* pItem);
	//插入节点
	int InsertItem(int nItem, LPCTSTR lpszItem);
	//插入节点
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	//插入节点
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState,UINT nStateMask, int nImage, LPARAM lParam);
	//设置风格
	DWORD SetExtendedStyle(DWORD dwNewStyle);

	//绘画控制
protected:
	//绘画数据
	VOID DrawReportItem(CDC * pDC, INT nItem, CRect & rcSubItem, INT nColumnIndex);

	//功能设置
public:
	//高度设置
	void SetItemHeight(int nHeight);

	//消息函数
protected:
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//销毁消息
	afx_msg void OnDestroy();
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//重绘背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif