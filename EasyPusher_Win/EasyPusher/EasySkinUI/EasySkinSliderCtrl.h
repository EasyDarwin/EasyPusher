/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#pragma once


// CEasySkinSliderCtrl
#include "EasySkinUI.h"

class  CEasySkinSliderCtrl : public CSliderCtrl,public IEasySkinControl
{
	DECLARE_DYNAMIC(CEasySkinSliderCtrl)
	
	//资源定义
public:
	CImageEx * m_pBackImg, * m_pThumImg, * m_pDisImg, * m_pBtImg, * m_pTraceImg;

	//变量定义
protected:
	BOOL m_bPress, m_bHover, m_bMouseTracking;		//按钮状态
	bool					m_bDragging;			//拖放标识
	CRect					m_rcThumRect;			//滑块区域
	CRect					m_rcChannelRect;		//轨迹区域
	CSize					m_szThum;				//滑块大小

	//函数定义
public:
	//构造函数
	CEasySkinSliderCtrl();
	//析构函数
	virtual ~CEasySkinSliderCtrl();

public:
	//设置资源
	BOOL SetBackImage(LPCTSTR lpszFileName,bool bFixed = true);
	//设置资源
	BOOL SetThumImage(LPCTSTR lpszFileName);
	//设置资源
	BOOL SetDisImage(LPCTSTR lpszFileName);
	//设置资源
	BOOL SetButtonImage(LPCTSTR lpszFileName);
	//设置资源
	BOOL SetTraceImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	
	//功能函数
public:
	//滑块位置
	void SetThumbRect(int nPos=0);
	//设置轨迹
	void SetChannelRect();
	//设置位置
	void SetPos(int nPos);

	//消息映射
public:
	//鼠标离开
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//绘制消息
	afx_msg void OnPaint();
	//窗口销毁
	afx_msg void OnDestroy();
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//左键按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//左键抬起
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//背景重绘
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()
};


