/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/

/////////////////////////////////////////////////////////////////
//
// Header file : EasySkinUI.h
//
// Descrption  : EasySkin scrollbar library
//
/////////////////////////////////////////////////////////////////

#pragma once
#include <windows.h>

// #ifdef __cplusplus
// extern "C"{
// #endif

//----------------------------------------------------------------
// Name : EasySkinUI_Init()
// Desc : Initialize and load the scrollbar EasySkin
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_Init(HWND hwnd, HBITMAP hBmp);


//----------------------------------------------------------------
// Name : EasySkinUI_Uninit()
// Desc : Remove the scrollbar EasySkin
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_Uninit(HWND hwnd);


//----------------------------------------------------------------
// Name : EasySkinUI_IsValid()
// Desc : Whether initialied ?
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_IsValid(HWND hwnd);


//----------------------------------------------------------------
// Name : EasySkinUI_GetScrollInfo()
// Desc : Gets the information for a EasySkin scroll bar
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi);


//----------------------------------------------------------------
// Name : EasySkinUI_SetScrollInfo()
// Desc : Sets the information for a EasySkin scroll bar
//---------------------------------------------------------------- 
int  WINAPI EasySkinUI_SetScrollInfo(HWND hwnd, int fnBar, LPCSCROLLINFO psi, BOOL fRedraw);


//----------------------------------------------------------------
// Name : EasySkinUI_GetScrollPos()
// Desc : Gets the thumb position in a EasySkin scroll bar
//----------------------------------------------------------------
int  WINAPI EasySkinUI_GetScrollPos(HWND hwnd, int fnBar);


//----------------------------------------------------------------
// Name : EasySkinUI_SetScrollPos()
// Desc : Sets the current position of the thumb in a EasySkin scroll bar
//----------------------------------------------------------------
int  WINAPI EasySkinUI_SetScrollPos(HWND hwnd, int nBar, int nPos, BOOL fRedraw);


//----------------------------------------------------------------
// Name : EasySkinUI_GetScrollRange()
// Desc : Gets the scroll range for a EasySkin scroll bar
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_GetScrollRange(HWND hwnd, int nBar, LPINT lpMinPos, LPINT lpMaxPos);


//----------------------------------------------------------------
// Name : EasySkinUI_SetScrollRange()
// Desc : Sets the scroll range of a EasySkin scroll bar
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_SetScrollRange(HWND hwnd, int nBar, int nMinPos, int nMaxPos, BOOL fRedraw);


//----------------------------------------------------------------
// Name : EasySkinUI_ShowScrollBar()
// Desc : Shows or hides a EasySkin scroll bar
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_ShowScrollBar(HWND hwnd, int wBar, BOOL fShow);


//----------------------------------------------------------------
// Name : EasySkinUI_EnableScrollBar()
// Desc : Enables or disables one or both EasySkin scroll bar direction buttons
//----------------------------------------------------------------
BOOL WINAPI EasySkinUI_EnableScrollBar(HWND hwnd, UINT wSBflags, UINT wArrows);


// #ifdef __cplusplus
// }
// #endif
