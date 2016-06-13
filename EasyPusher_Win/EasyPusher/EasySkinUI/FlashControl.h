/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef FLASH_CONTROL_HEAD_FILE
#define FLASH_CONTROL_HEAD_FILE

#pragma once

#include "EasySkinUI.h"

class  CFlashControl : public CWnd
{
protected:
	long						m_lOldProc;		
	static long					s_lCount;				//对象计数器
	BOOL						m_bHasInit;				//是否已经初始化
	IFlashContrlSink			*m_pIFlashContrlSink;

	DECLARE_DYNCREATE(CFlashControl)

	static LRESULT FlashWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:

	CFlashControl();
	virtual ~CFlashControl();

	BOOL Init(IFlashContrlSink*pIFlashContrlSink);

	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xD27CDB6E, 0xAE6D, 0x11CF, { 0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

	long get_ReadyState()
	{
		long result;
		InvokeHelper(DISPID_READYSTATE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	long get_TotalFrames()
	{
		long result;
		InvokeHelper(0x7c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL get_Playing()
	{
		BOOL result;
		InvokeHelper(0x7d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Playing(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x7d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_Quality()
	{
		long result;
		InvokeHelper(0x69, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_Quality(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x69, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ScaleMode()
	{
		long result;
		InvokeHelper(0x78, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ScaleMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x78, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_AlignMode()
	{
		long result;
		InvokeHelper(0x79, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_AlignMode(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x79, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BackgroundColor()
	{
		long result;
		InvokeHelper(0x7b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BackgroundColor(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Loop()
	{
		BOOL result;
		InvokeHelper(0x6a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Loop(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x6a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Movie()
	{
		CString result;
		InvokeHelper(0x66, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Movie(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x66, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_FrameNum()
	{
		long result;
		InvokeHelper(0x6b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_FrameNum(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetZoomRect(long left, long top, long right, long bottom)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x6d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, left, top, right, bottom);
	}
	void Zoom(long factor)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x76, DISPATCH_METHOD, VT_EMPTY, NULL, parms, factor);
	}
	void Pan(long x, long y, long mode)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x77, DISPATCH_METHOD, VT_EMPTY, NULL, parms, x, y, mode);
	}
	void Play()
	{
		InvokeHelper(0x70, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Stop()
	{
		InvokeHelper(0x71, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Back()
	{
		InvokeHelper(0x72, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Forward()
	{
		InvokeHelper(0x73, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Rewind()
	{
		InvokeHelper(0x74, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void StopPlay()
	{
		InvokeHelper(0x7e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void GotoFrame(long FrameNum)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, FrameNum);
	}
	long CurrentFrame()
	{
		long result;
		InvokeHelper(0x80, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL IsPlaying()
	{
		BOOL result;
		InvokeHelper(0x81, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	long PercentLoaded()
	{
		long result;
		InvokeHelper(0x82, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL FrameLoaded(long FrameNum)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x83, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FrameNum);
		return result;
	}
	long FlashVersion()
	{
		long result;
		InvokeHelper(0x84, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	CString get_WMode()
	{
		CString result;
		InvokeHelper(0x85, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_WMode(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x85, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_SAlign()
	{
		CString result;
		InvokeHelper(0x86, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_SAlign(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x86, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Menu()
	{
		BOOL result;
		InvokeHelper(0x87, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Menu(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x87, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Base()
	{
		CString result;
		InvokeHelper(0x88, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Base(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x88, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Scale()
	{
		CString result;
		InvokeHelper(0x89, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Scale(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x89, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_DeviceFont()
	{
		BOOL result;
		InvokeHelper(0x8a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_DeviceFont(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x8a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_EmbedMovie()
	{
		BOOL result;
		InvokeHelper(0x8b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_EmbedMovie(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0x8b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_BGColor()
	{
		CString result;
		InvokeHelper(0x8c, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_BGColor(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x8c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_Quality2()
	{
		CString result;
		InvokeHelper(0x8d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_Quality2(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x8d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void LoadMovie(long layer, LPCTSTR url)
	{
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x8e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, layer, url);
	}
	void TGotoFrame(LPCTSTR target, long FrameNum)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x8f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, FrameNum);
	}
	void TGotoLabel(LPCTSTR target, LPCTSTR label)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x90, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, label);
	}
	long TCurrentFrame(LPCTSTR target)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x91, DISPATCH_METHOD, VT_I4, (void*)&result, parms, target);
		return result;
	}
	CString TCurrentLabel(LPCTSTR target)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x92, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, target);
		return result;
	}
	void TPlay(LPCTSTR target)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x93, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target);
	}
	void TStopPlay(LPCTSTR target)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x94, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target);
	}
	void SetVariable(LPCTSTR name, LPCTSTR value)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x97, DISPATCH_METHOD, VT_EMPTY, NULL, parms, name, value);
	}
	CString GetVariable(LPCTSTR name)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x98, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, name);
		return result;
	}
	void TSetProperty(LPCTSTR target, long property, LPCTSTR value)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x99, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, property, value);
	}
	CString TGetProperty(LPCTSTR target, long property)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x9a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, target, property);
		return result;
	}
	void TCallFrame(LPCTSTR target, long FrameNum)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x9b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, FrameNum);
	}
	void TCallLabel(LPCTSTR target, LPCTSTR label)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x9c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, label);
	}
	void TSetPropertyNum(LPCTSTR target, long property, double value)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_R8 ;
		InvokeHelper(0x9d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, target, property, value);
	}
	double TGetPropertyNum(LPCTSTR target, long property)
	{
		double result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x9e, DISPATCH_METHOD, VT_R8, (void*)&result, parms, target, property);
		return result;
	}
	double TGetPropertyAsNumber(LPCTSTR target, long property)
	{
		double result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0xac, DISPATCH_METHOD, VT_R8, (void*)&result, parms, target, property);
		return result;
	}
	CString get_SWRemote()
	{
		CString result;
		InvokeHelper(0x9f, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_SWRemote(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x9f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_FlashVars()
	{
		CString result;
		InvokeHelper(0xaa, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_FlashVars(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xaa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_AllowScriptAccess()
	{
		CString result;
		InvokeHelper(0xab, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_AllowScriptAccess(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xab, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_MovieData()
	{
		CString result;
		InvokeHelper(0xbe, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_MovieData(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xbe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPUNKNOWN get_InlineData()
	{
		LPUNKNOWN result;
		InvokeHelper(0xbf, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
		return result;
	}
	void put_InlineData(LPUNKNOWN newValue)
	{
		static BYTE parms[] = VTS_UNKNOWN ;
		InvokeHelper(0xbf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_SeamlessTabbing()
	{
		BOOL result;
		InvokeHelper(0xc0, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_SeamlessTabbing(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xc0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void EnforceLocalSecurity()
	{
		InvokeHelper(0xc1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL get_Profile()
	{
		BOOL result;
		InvokeHelper(0xc2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Profile(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xc2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ProfileAddress()
	{
		CString result;
		InvokeHelper(0xc3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ProfileAddress(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xc3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ProfilePort()
	{
		long result;
		InvokeHelper(0xc4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ProfilePort(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xc4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString CallFunction(LPCTSTR request)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xc6, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, request);
		return result;
	}
	void SetReturnValue(LPCTSTR returnValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xc7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, returnValue);
	}
	void DisableLocalSecurity()
	{
		InvokeHelper(0xc8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	CString get_AllowNetworking()
	{
		CString result;
		InvokeHelper(0xc9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_AllowNetworking(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xc9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_AllowFullScreen()
	{
		CString result;
		InvokeHelper(0xca, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_AllowFullScreen(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xca, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_left()
	{
		float result;
		InvokeHelper(0x80010103, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_left(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x80010103, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_top()
	{
		float result;
		InvokeHelper(0x80010104, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_top(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x80010104, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_Height()
	{
		float result;
		InvokeHelper(0x80010105, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_Height(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x80010105, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	float get_Width()
	{
		float result;
		InvokeHelper(0x80010106, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
		return result;
	}
	void put_Width(float newValue)
	{
		static BYTE parms[] = VTS_R4 ;
		InvokeHelper(0x80010106, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_name()
	{
		CString result;
		InvokeHelper(0x80010000, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_name(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x80010000, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	LPDISPATCH get_Automation()
	{
		LPDISPATCH result;
		InvokeHelper(0x80010107, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
		return result;
	}
	void Select()
	{
		InvokeHelper(0x80010220, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Copy()
	{
		InvokeHelper(0x80010228, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Cut()
	{
		InvokeHelper(0x80010229, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Delete()
	{
		InvokeHelper(DISPID_Delete, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Activate()
	{
		InvokeHelper(0x80010251, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	CString get_AltHTML()
	{
		CString result;
		InvokeHelper(0x80010bc3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_AltHTML(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x80010bc3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
};

#endif