/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef EasySkin_RICH_EDIT_HEAD_FILE
#define EasySkin_RICH_EDIT_HEAD_FILE

#pragma once

#include <RichOle.h>
#include "EasySkinUI.h"

//////////////////////////////////////////////////////////////////////////////////

//图像对象
class  CDataObject : public IDataObject
{
	//内核变量
protected:
	ULONG							m_ulRefCnt;							//引用计数
	STGMEDIUM 						m_StgMedium;						//对象信息
	FORMATETC 						m_FormatEtc;						//对象信息

	//接口变量
protected:
	IStorage *						m_pIStorage;						//对象接口
	IOleObject *					m_pIOleObject;						//对象接口

	//函数定义
public:
	//构造函数
	CDataObject();
	//析构函数
	virtual ~CDataObject();

	//基础接口
public:
	//增加引用
	virtual ULONG STDMETHODCALLTYPE AddRef();
	//释放对象
	virtual ULONG STDMETHODCALLTYPE Release();
	//查询接口
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, VOID * * ppvObject);

	//对象接口
public:
	//获取数据
	virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC * pFormatetcIn, STGMEDIUM * pStgMedium);
	//设置数据
	virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC * pFormatEtc, STGMEDIUM * pStgMedium, BOOL fRelease);
	
	//其他接口
public:
	virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC * pFormatEtc) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA * * ppenumAdvise) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC * pFormatEtc, STGMEDIUM *  pStgMedium ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC * * ppenumFormatEtc ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC * pFormatEtcIn, FORMATETC * pFormatEtcOut) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC * pFormatEtc, DWORD advf, IAdviseSink * pAdvSink, DWORD * pdwConnection) { return E_NOTIMPL; }

	//配置函数
public:
	//设置图片
	bool SetImage(HBITMAP hBitmap);
	//加载图片
	bool LoadImage(LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(HINSTANCE hInstance, LPCTSTR lpResourceName);

	//图片函数
public:
	//获取图片
	HBITMAP GetBitmap() { return m_StgMedium.hBitmap; }
	//获取信息
	bool IntercalateReObject(REOBJECT & ReObject, IOleClientSite * pIOleClientSite);
};

//////////////////////////////////////////////////////////////////////////////////

//消息框类
class  CEasySkinRichEdit : public CRichEditCtrl,public IEasySkinControl
{
	//接口变量
protected:
	IRichEditOle *					m_pIRichEditOLE;					//接口指针

	//函数定义
public:
	//构造函数
	CEasySkinRichEdit();
	//析构函数
	virtual ~CEasySkinRichEdit();

	//重载函数
protected:
	//控件绑定
	virtual VOID PreSubclassWindow();

	//功能函数
public:
	//清除屏幕
	bool CleanScreen();
	//加载消息
	bool LoadMessage(LPCTSTR pszFileName);
	//保存信息
	bool SaveMessage(LPCTSTR pszFileName);

	//图片插入
public:
	//插入图片
	bool InsertImage(LPCTSTR pszImage);
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入对象
	bool InsertDataObject(CDataObject * pDataObject);

	//字串插入
public:
	//插入字串
	bool InsertString(LPCTSTR pszString, COLORREF crTextColor);
	//插入字串
	bool InsertString(LPCTSTR pszString, COLORREF crTextColor, COLORREF crBackColor);
	//插入字串
	bool InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat);

	//超级链接
public:
	//插入字串
	bool InsertHyperLink(LPCTSTR pszString);
	//插入字串
	bool InsertHyperLink(LPCTSTR pszString, COLORREF crBackColor);

	//辅助函数
protected:
	//加载回调
	static DWORD CALLBACK LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);
	//保存回调
	static DWORD CALLBACK SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

	//内部函数
protected:
	//字符判断
	bool EfficacyUrlChar(TCHAR chChar);
	//地址判断
	bool EfficacyUrlString(LPCTSTR pszUrl);

	//消息映射
protected:
	//销毁消息
	VOID OnDestroy();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//右键消息
	VOID OnRButtonDown(UINT nFlags, CPoint point);
	//设置光标
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//链接事件
	void OnURLClick(NMHDR *pNmhdr, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif