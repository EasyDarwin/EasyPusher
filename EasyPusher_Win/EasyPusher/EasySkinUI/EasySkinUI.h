/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef EASYSKINUI_HEAD_FILE
#define EASYSKINUI_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include <GdiPlus.h>
#include <Shlwapi.h>
#include <afxwin.h>
#include <atlimage.h>
#include <vector>
#include <map>

using namespace Gdiplus;
using namespace std;

//FALSH 定义
#import "Flash.ocx" named_guids
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
interface IFlashContrlSink
{
public:
	//
	virtual void OnDraw(CDC*pDC,int nWidth,int nHeight)=NULL;
};
//////////////////////////////////////////////////////////////////////////
#define ON_RBN_DBLCLK(id, memberFxn) \
	ON_CONTROL(BN_CLICKED, id, memberFxn)

#define WM_EDIT_CLICK					WM_USER+0x50

#define CHECK_IMAGE(image)				((image != NULL) && (!image->IsNull()))

//////////////////////////////////////////////////////////////////////////
//ListBox Item
struct tagItemStruct
{
	UINT       itemID;
	UINT       itemWidth;
	UINT       itemHeight;
};

//导出类头文件
//////////////////////////////////////////////////////////////////////////
//平台头文件
#include "Macro.h"
#include "Function.h"
#include "MemoryDC.h"
#include "ImageEx.h"
#include "RenderManager.h"
#include "IEasySkinControl.h"
#include "GifImage.h"

#include "EasySkinButton.h"
#include "EasySkinComboBox.h"
#include "EasySkinDialog.h"
#include "EasySkinEdit.h"
#include "EasySkinListBox.h"
#include "EasySkinSliderCtrl.h"
#include "EasySkinProgressCtrl.h"
#include "EasySkinListCtrl.h"
#include "EasySkinTreeCtrl.h"
#include "EasySkinTabCtrl.h"
#include "EasySkinRichEdit.h"
#include "EasySkinUIMenu.h"
#include "FlashControl.h"
#include "EasySkinToolBar.h"


//////////////////////////////////////////////////////////////////////////

#endif