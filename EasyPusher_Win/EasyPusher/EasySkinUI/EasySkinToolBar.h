/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef EASYSKIN_TOOLBAR_EAD_FILE
#define EASYSKIN_TOOLBAR_EAD_FILE

#pragma once

#include "EasySkinUI.h"

// CEasySkinToolBar
#define STBI_STYLE_BUTTON			0x0000
#define STBI_STYLE_SEPARTOR			0x0001
#define STBI_STYLE_CHECK			0x0002
#define STBI_STYLE_DROPDOWN			0x0008
#define STBI_STYLE_WHOLEDROPDOWN	0x0080
// STBI_STYLE_WHOLEDROPDOWN不要和STBI_STYLE_CHECK一起用
// STBI_STYLE_DROPDOWN不要和STBI_STYLE_WHOLEDROPDOWN一起用

class CEasySkinToolBarItem
{
public:
	CEasySkinToolBarItem(void);
	~CEasySkinToolBarItem(void);

public:
	int							m_nID;
	DWORD						m_dwStyle;
	int							m_nWidth, m_nHeight;
	int							m_nLeftWidth, m_nRightWidth;
	int							m_nPadding;
	CString						m_strText;
	CString						m_strToolTipText;
	BOOL						m_bChecked;
	
	CImageEx * m_lpBgImgN, * m_lpBgImgH, * m_lpBgImgD;
	CImageEx * m_lpLeftH, * m_lpLeftD;
	CImageEx * m_lpRightH, * m_lpRightD;
	CImageEx * m_lpSepartorImg;
	CImageEx * m_lpArrowImg;
	CImageEx * m_lpIconImg;
};

//////////////////////////////////////////////////////////////////////////

class  CEasySkinToolBar : public CWnd,public IEasySkinControl
{
	DECLARE_DYNAMIC(CEasySkinToolBar)
protected:
	CToolTipCtrl					m_ToolTipCtrl;
	std::vector<CEasySkinToolBarItem *> m_arrItems;
	CImageEx						* m_lpBgImg;
	int								m_nPressIndex, m_nHoverIndex;
	BOOL							m_bPressLorR, m_bHoverLorR;
	BOOL							m_bMouseTracking;
	int								m_nLeft, m_nTop;
	BOOL							m_bAutoSize;
	
	//函数定义
public:
	//构造函数
	CEasySkinToolBar();
	//析构函数
	virtual ~CEasySkinToolBar();

	//资源加载
public:
	//背景资源
	BOOL SetItemBgPic(int nIndex, LPCTSTR lpNormal, LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart);
	//左边背景
	BOOL SetItemLeftBgPic(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart);
	//右边背景
	BOOL SetItemRightBgPic(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart);
	//分离图片
	BOOL SetItemSepartorPic(int nIndex, LPCTSTR lpszFileName);
	//箭头图片
	BOOL SetItemArrowPic(int nIndex, LPCTSTR lpszFileName);
	//图标资源
	BOOL SetItemIconPic(int nIndex, LPCTSTR lpszFileName);
	//背景资源
	BOOL SetBgPic(LPCTSTR lpszFileName, CONST LPRECT lprcNinePart);

	//功能函数
public:
	//左上角位置
	void SetLeftTop(int nLeft, int nTop);
	//自动尺寸
	void SetAutoSize(BOOL bAutoSize);
	//添加节点
	int AddItem(int nID, DWORD dwStyle);
	//设置风格
	void SetItemStyle(int nIndex, int dwStyle);
	//节点ID
	void SetItemID(int nIndex, int nID);
	//节点大小
	void SetItemSize(int nIndex, int nWidth, int nHeight, int nLeftWidth = 0, int nRightWidth = 0);
	//节点偏移
	void SetItemPadding(int nIndex, int nPadding);
	//节点文本
	void SetItemText(int nIndex, LPCTSTR lpszText);
	//提示标签
	void SetItemToolTipText(int nIndex, LPCTSTR lpszText);
	//获取矩形
	BOOL GetItemRectByIndex(int nIndex, CRect& rect);
	//获取矩形
	BOOL GetItemRectByID(int nID, CRect& rect);
	//获取check
	BOOL GetItemCheckState(int nIndex);
	//设置check
	void SetItemCheckState(int nIndex, BOOL bChecked);
	//获取节点
	CEasySkinToolBarItem * GetItemByIndex(int nIndex);
	//获取节点
	CEasySkinToolBarItem * GetItemByID(int nID);
	//点击测试
	int HitTest(POINT pt);
	//绘图节点
	void DrawItem(CDC*pDC, int nIndex);

	//消息函数
public:
	//擦出背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//重绘消息
	afx_msg void OnPaint();
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//左键抬起
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标离开
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//窗口销毁
	afx_msg void OnDestroy();
	//消息循环
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//创建控件
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
};

#endif