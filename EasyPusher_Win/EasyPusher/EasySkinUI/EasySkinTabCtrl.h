/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once

#include "EasySkinUI.h"

// CEasySkinTabCtrl
#define TCN_DROPDOWN				(NM_FIRST-5)

#define STCI_STYLE_BUTTON			0x0000
#define STCI_STYLE_DROPDOWN			0x0008

class  CEasySkinTabCtrlItem
{
public:
	CEasySkinTabCtrlItem(void);
	~CEasySkinTabCtrlItem(void);

public:
	int						m_nID;
	DWORD					m_dwStyle;
	int						m_nWidth, m_nHeight;
	int						m_nLeftWidth, m_nRightWidth;
	int						m_nPadding;
	CString					m_strText;
	CString					m_strToolTipText;
	CImageEx * m_lpBgImgN, * m_lpBgImgH, * m_lpBgImgD;
	CImageEx * m_lpArrowImgH, * m_lpArrowImgD;
	CImageEx * m_lpIconImg, * m_lpSelIconImg;
};

//////////////////////////////////////////////////////////////////////////
typedef vector<CEasySkinTabCtrlItem *>			CTabCtrlItemArray;

class  CEasySkinTabCtrl : public CTabCtrl,public IEasySkinControl
{
	DECLARE_DYNAMIC(CEasySkinTabCtrl)

protected:
	CToolTipCtrl				m_ToolTipCtrl;
	CTabCtrlItemArray			m_ItemArray;
	CImageEx					* m_lpBack;
	CImageEx					* m_lpItemBgImgN, * m_lpItemBgImgH, * m_lpItemBgImgD;
	CImageEx					* m_lpArrowImgH, * m_lpArrowImgD;
	int							m_nSelIndex, m_nHoverIndex;
	BOOL						m_bPressArrow;
	BOOL						m_bMouseTracking;
	int							m_nLeft, m_nTop;

	//函数定义
public:
	//构造函数
	CEasySkinTabCtrl();
	//析构函数
	virtual ~CEasySkinTabCtrl();

	//资源设置
public:
	//背景资源
	BOOL SetBackImage(LPCTSTR lpszFileName, CONST LPRECT lprcNinePart=NULL);
	//节点资源
	BOOL SetItemsImage(LPCTSTR lpNormal, LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart=NULL);
	//下拉按钮
	BOOL SetItemsArrowImage(LPCTSTR lpHighlight, LPCTSTR lpDown);
	
	//节点资源设置
public:
	//节点背景
	BOOL SetItemBack(int nIndex, LPCTSTR lpNormal, LPCTSTR lpHighlight, LPCTSTR lpDown, CONST LPRECT lprcNinePart=NULL);
	//下拉按钮
	BOOL SetItemArrowImage(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown);
	//图标资源
	BOOL SetItemIconImage(int nIndex, LPCTSTR lpIcon, LPCTSTR lpSelIcon);

	//界面辅助
public:
	//设置节点起始坐标
	void SetLeftTop(int nLeft, int nTop);
	//节点大小
	CSize SetItemSize(CSize size,CONST LPSIZE lpszArrow=NULL);
	//节点偏移
	void SetItemPadding(int nIndex, int nPadding);
	//点击测试
	int HitTest(POINT pt);
	//绘制节点
	void DrawItem(CDC*pDC, int nIndex);

	//功能函数
public:
	//获取节点数量
	int GetItemCount();
	//获取当前选择
	int GetCurSel();
	//设置当前选择
	void SetCurSel(int nCurSel);
	//添加节点
	int AddItem(int nID, DWORD dwStyle=STCI_STYLE_BUTTON);
	//设置节点ID
	void SetItemID(int nIndex, int nID);
	//节点风格
	void SetItemStyle(int nIndex, int dwStyle);
	//节点文字
	void SetItemText(int nIndex, LPCTSTR lpszText);
	//节点提示
	void SetItemToolTipText(int nIndex, LPCTSTR lpszText);

	//私有成员
private:
	//获取矩形
	BOOL GetItemRectByIndex(int nIndex, CRect& rect);
	//获取矩形
	BOOL GetItemRectByID(int nID, CRect& rect);
	//获取节点
	CEasySkinTabCtrlItem * GetItemByIndex(int nIndex);
	//获取节点
	CEasySkinTabCtrlItem * GetItemByID(int nID);
	
	//消息函数
public:
	//背景重绘
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//绘制消息
	afx_msg void OnPaint();
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标移开
	LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//销毁消息
	afx_msg void OnDestroy();
	//解析消息
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()	
};


