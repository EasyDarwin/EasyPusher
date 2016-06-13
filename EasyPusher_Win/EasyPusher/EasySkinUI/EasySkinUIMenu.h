/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef EASYSKIN_MENU_HEAD_FILE
#define EASYSKIN_MENU_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

//////////////////////////////////////////////////////////////////////////////////

//子项类型
enum enMenuItemType
{
	MenuItemType_String,			//字符类型
	MenuItemType_Separator,			//拆分类型
};

//////////////////////////////////////////////////////////////////////////////////

//菜单子项
class CEasySkinMenuItem
{
	//变量定义
public:
	const enMenuItemType			m_MenuItemType;						//子项类型
	HMENU							m_hSubMenu;							//子菜单

	//函数定义
public:
	//构造函数
	CEasySkinMenuItem(enMenuItemType MenuItemType) : m_MenuItemType(MenuItemType) {m_hSubMenu = NULL;}
	//析构函数
	virtual ~CEasySkinMenuItem() {}
};

//字符菜单
class CEasySkinMenuString : public CEasySkinMenuItem
{
	//变量定义
public:
	CString							m_strString;						//菜单字符
	CImageEx						*m_pImageN;							//图像句柄
	CImageEx						*m_pImageH;							//图像句柄

	//函数定义
public:
	//构造函数
	CEasySkinMenuString() : CEasySkinMenuItem(MenuItemType_String) {m_pImageN=m_pImageH=NULL;}
	//析构函数
	virtual ~CEasySkinMenuString() {}
};

//拆分菜单
class CEasySkinMenuSeparator : public CEasySkinMenuItem
{
	//函数定义
public:
	//构造函数
	CEasySkinMenuSeparator() : CEasySkinMenuItem(MenuItemType_Separator) {}
	//析构函数
	virtual ~CEasySkinMenuSeparator() {}
};

//////////////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArray<CEasySkinMenuItem *,CEasySkinMenuItem *>			CMenuItemArray;					//子项数组
typedef CArray<CEasySkinMenuString *,CEasySkinMenuString *>		CMenuStringArray;				//子项数组
typedef CArray<CEasySkinMenuSeparator *,CEasySkinMenuSeparator *>	CMenuSeparatorArray;			//子项数组

//////////////////////////////////////////////////////////////////////////////////

//菜单内核
class  CUIMenu : public CMenu,public IEasySkinControl
{
	//图片资源
protected:
	CImageEx						*m_pMenuBar;
	CImageEx						*m_pMenuBack;
	CImageEx						*m_pMenuHoven;
	CImageEx						*m_pSeparator;
	CImageEx						*m_pCheck;
	CImageEx						*m_pArrow;

	//内核变量
protected:
	CMenuItemArray					m_MenuItemActive;					//子项数组
	
	//静态变量
protected:
	static CMenuStringArray			m_MenuItemString;					//字符子项
	static CMenuSeparatorArray		m_MenuItemSeparator;				//拆分子项
	static HHOOK					m_hMenuHook;

protected:
	COLORREF						m_crSeparator;

	//函数定义
public:
	//构造函数
	CUIMenu();
	//析构函数
	virtual ~CUIMenu();

	//重载函数
public:
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//测量位置
	virtual VOID MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	//管理函数
public:
	//创建菜单
	bool CreatePopupMenu();
	//销毁菜单
	BOOL DestroyMenu();
	//弹出菜单
	bool TrackPopupMenu(CWnd * pWnd);
	//弹出菜单
	bool TrackPopupMenu(INT nXPos, INT nYPos, CWnd * pWnd);

	//资源设置
public:
	//设置资源
	BOOL SetMenuBarImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetMenuBackImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetMenuHovenImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetSeparatorImage(LPCTSTR lpszFileName,CONST LPRECT lprcNinePart=NULL);
	//设置资源
	BOOL SetCheckImage(LPCTSTR lpszFileName);
	//设置资源
	BOOL SetArrowImage(LPCTSTR lpszFileName);

	//增加函数
public:
	//插入拆分
	bool AppendSeparator();
	//插入字符
	bool AppendMenu(UINT nMenuID, LPCTSTR pszString, LPCTSTR lpszIconN=NULL, LPCTSTR lpszIconH=NULL, UINT nFlags=0);

	//插入函数
public:
	//插入拆分
	bool InsertSeparator(UINT nPosition);
	//插入字符
	bool InsertMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition,  LPCTSTR lpszIconN=NULL, LPCTSTR lpszIconH=NULL, UINT nFlags=0);

	//修改函数
public:
	//删除菜单
	bool RemoveMenu(UINT nPosition, UINT nFlags);
	//修改菜单
	bool ModifyMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition,  LPCTSTR lpszIconN=NULL, LPCTSTR lpszIconH=NULL, UINT nFlags=0);

	//内部函数
private:
	//拆分数量
	int GetSeparatorCount();
	//释放子项
	VOID FreeMenuItem(CEasySkinMenuItem * pEasySkinMenuItem);
	//获取子项
	CEasySkinMenuItem * AcitveMenuItem(enMenuItemType MenuItemType);

	static LRESULT CALLBACK WindowsHook(int code,WPARAM wParam,LPARAM lParam);

};

//////////////////////////////////////////////////////////////////////////////////

#endif