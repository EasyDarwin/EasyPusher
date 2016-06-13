/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef COMBOX_HEAD_FILE
#define COMBOX_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

#define		WM_CBO_EDIT_MOUSE_HOVER		WM_USER + 1
#define		WM_CBO_EDIT_MOUSE_LEAVE		WM_USER + 2
#define		WM_CBO_LIST_HIDE			WM_USER + 3
#define		WM_CBO_LIST_RBUTTONUP		WM_USER + 4
#define		WM_CBO_RBUTTONUP			WM_USER + 5
//////////////////////////////////////////////////////////////////////////

class  CEasySkinComboBox_Edit :public CEdit,public IEasySkinControl
{
	//基本变量
protected:
	HWND			m_hOwnerWnd;			//组合框句柄
	BOOL			m_bMouseTracking;		//鼠标事件
	BOOL			m_bIsDefText;			//默认文本
	CString			m_strDefText;			//文本内容

	//函数定义
public:
	//构造函数
	CEasySkinComboBox_Edit(void);
	//析构函数
	~CEasySkinComboBox_Edit(void);

	//重载函数
protected:
	//消息循环
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//设置函数
public:
	//设置句柄
	void SetOwnerWnd(HWND hWnd);
	//默认文本
	void SetDefaultText(LPCTSTR lpszText);
	//默认文本
	BOOL IsDefaultText();
	//文本形式
	void SetDefaultTextMode(BOOL bIsDefText);
	
	//消息定义
public:
	//重绘背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标移开
	afx_msg void OnMouseLeave();
	//设置焦点
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//销毁焦点
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//颜色反射
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//左键双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//鼠标样式
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

class  CEasySkinComboBox_ListBox : public CListBox,public IEasySkinControl
{
	//基本变量
protected:
	HWND							m_hOwnerWnd;			//组合框句柄
	
	//资源定义
protected:
	CImageEx 						* m_pBackImgN;			//正常资源
	CImageEx 						* m_pSelectImg;			//选中资源

	//函数定义
public:
	//构造函数
	CEasySkinComboBox_ListBox(void);
	//析构函数
	~CEasySkinComboBox_ListBox(void);

	//设置函数
public:
	//设置句柄
	void SetOwnerWnd(HWND hWnd);
	//绘画边框
	void DrawListFrame();

	//资源加载
public:
	//正常背景
	BOOL SetBackNormalImage(LPCTSTR lpszFileName, CONST LPRECT lpNinePart = NULL);
	//选中资源
	BOOL SetSelectImage(LPCTSTR lpszFileName, CONST LPRECT lpNinePart = NULL);

	//消息定义
private:
	//显示窗口
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//窗口销毁
	afx_msg void OnDestroy();
	
	//重载函数
protected:
	//控件重绘
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////

class  CEasySkinComboBox : public CComboBox,public IEasySkinControl
{
	DECLARE_DYNAMIC(CEasySkinComboBox)

	//资源变量
protected:
	CImageEx 							* m_lpBgImgN;		//Edit背景
	CImageEx 							* m_lpBgImgH;		//Edit高亮
	CImageEx 							* m_lpArrowImgN;	//按钮正常
	CImageEx 							* m_lpArrowImgH;	//按钮高亮
	CImageEx 							* m_lpArrowImgP;	//按钮按下

	//状态变量
protected:
	BOOL m_bFocus, m_bPress, m_bHover, m_bMouseTracking;	//鼠标状态
	BOOL m_bArrowPress, m_bArrowHover;						//按钮状态
	int									m_nArrowWidth;		//按钮宽度
	CRect								m_rcArrow;			//按钮区域
	UINT								m_cyItemHeight;
	
	//控件变量
protected:
	CEasySkinComboBox_Edit					m_EasySkinComboBoxEdit;	//Edit控件
	CEasySkinComboBox_ListBox				m_EasySkinComboBoxList;	//ListBox控件

	//函数定义
public:
	//构造函数
	CEasySkinComboBox();
	//析构函数
	virtual ~CEasySkinComboBox();

	//资源设置
public:
	//Edit背景
	BOOL SetBgNormalPic(LPCTSTR lpszFileName, RECT * lpNinePart = NULL);
	//Edit高亮
	BOOL SetBgHotPic(LPCTSTR lpszFileName, RECT * lpNinePart = NULL);
	//按钮正常
	BOOL SetArrowNormalPic(LPCTSTR lpszFileName);
	//按钮高亮
	BOOL SetArrowHotPic(LPCTSTR lpszFileName);
	//按钮按下
	BOOL SetArrowPushedPic(LPCTSTR lpszFileName);
	//加载资源
	BOOL SetScrollImage(LPCTSTR pszFileName);

	//设置函数
public:
	//设置文本
	void SetDefaultText(LPCTSTR lpszText);
	//默认文本
	BOOL IsDefaultText();
	//按钮宽度
	void SetArrowWidth(int nWidth);
	//按钮检测
	bool VerdictOverButton(CPoint MousePoint);
	//设置DropList
	void SetDropList();
	//文本颜色
	void SetEditTextColor(COLORREF col);
	//设置高度
	void SetAllItemHeight(UINT cyItemHeight);

	//控件句柄
public:
	//Edit句柄
	CEasySkinComboBox_Edit *GetEditWnd();
	//ListBox句柄
	CEasySkinComboBox_ListBox *GetListBoxWnd();

	//重载函数
protected:
	//子类化
	virtual void PreSubclassWindow();
	
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);

	//消息循环
public:
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//重绘背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//绘制背景
	afx_msg void OnPaint();
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标移开
	afx_msg void OnMouseLeave();
	//窗口销毁
	afx_msg void OnDestroy();
	//窗口尺寸
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//ListBox消息
	afx_msg LRESULT OnCtlColorListBox(WPARAM wParam, LPARAM lParam);

	//自定义消息
public:
	//Edit高亮
	LRESULT OnEditMouseHover(WPARAM wParam, LPARAM lParam);
	//Edit移开
	LRESULT OnEditMouseLeave(WPARAM wParam, LPARAM lParam);
	//Edit移开
	LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam);
	//ListBox收缩
	LRESULT OnListHide(WPARAM wParam, LPARAM lParam);	

protected:	
	DECLARE_MESSAGE_MAP()
};

#endif
