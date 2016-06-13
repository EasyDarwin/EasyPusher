/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

#include "FlashingWnd.h"
#include <d3d9.h>

//通知主界面开始捕获消息
#define WM_USER_RECORDSTART WM_USER+0x101
#define DINV    3

#ifndef  MAX_VIDEO_WINDOW_NUM
#define  MAX_VIDEO_WINDOW_NUM  16
#endif //MAX_VIDEO_WINDOW_NUM

#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))

typedef struct tagScreenCapDataInfo
{
	int nWidth;
	int nHeight;
	int nDataType;
}ScreenCapDataInfo;

typedef int (WINAPI *CaptureScreenCallback)(int nDevId, unsigned char *pBuffer, int nBufSize, int nRealDatatYPE, void* realDataInfo, void* pMaster);

class CEasyPusherDlg ;

class CCaptureScreen
{
public:
	CCaptureScreen(void);
	~CCaptureScreen(void);

public:
	//Interface Function
	int StartScreenCapture(HWND hShowWnd, int nCapMode);
	void StopScreenCapture();
	void GetCaptureScreenSize(int& nWidth, int& nHeight );
	//设置捕获数据回调函数
	void SetCaptureScreenCallback(CaptureScreenCallback callBack, void * pMaster);

	BOOL IsInCapturing()
	{
		return m_bCapturing;
	}

	int Init();
	void UnInit();

public:
		//本类静态实例化指针(数组主要是在多窗口时使用，我不知道这样是否合理，暂时也没有更好的办法？！)
		static CCaptureScreen* s_pCaptureScreen[MAX_VIDEO_WINDOW_NUM]; 
		static CCaptureScreen* Instance(int& nIndex);
		static void UnInstance(int nI);
//////////////////////////////////////////////////////////////////////////
	//屏幕捕获帧主函数
	//Use these 2 functions to create frames and free frames
	void* CaptureScreenFrame(int left,int top,int width, int height,int tempDisableRect);
	void FreeFrame(void*) ;
	void InsertHighLight(HDC hdc,int xoffset, int yoffset);

	//鼠标选区绘制 Region Select  Functions
	static LRESULT WINAPI MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
	int InitDrawShiftWindow();
	int InitSelectRegionWindow();
	int CreateShiftWindow();
	int DestroyShiftWindow();
	LRESULT ProcessMouseCapMsg(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);

	//inner call function
	BOOL isRectEqual(RECT a, RECT b);

	void SaveBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy);
	void RestoreBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy) ;
	void DeleteBitmapCopy();
	void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);
	void NormalizeRect(LPRECT prc);
	void FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen);
	void DrawFlashingRect(BOOL bDraw , int mode);
	//Mouse Capture functions 
	HCURSOR FetchCursorHandle();
	HANDLE Bitmap2Dib(HBITMAP, UINT);

	//创建线程进行屏幕捕获
	int CreateCaptureScreenThread();
	static UINT WINAPI CaptureScreenThread(LPVOID pParam);
	void CaptureVideoProcess();


public:
		HWND m_hMainWnd;
		//鼠标选区窗口句柄
		HWND m_hMouseCaptureWnd;
		//鼠标选区模式 : 0==固定大小选区 1=自定义选区（自己画） 2=全屏
		int m_nMouseCaptureMode;
		HBITMAP m_hSavedBitmap;
		CFlashingWnd* m_pFrame ;
		//屏幕设备的真彩色位数 24/32
		int m_nColorBits;

		//是否正在捕获
		BOOL m_bCapturing;
		// 鼠标是否正在拖拽
		BOOL m_bMouseDraging;
		//是否绘制闪烁的矩形框(Flashing Region)
		BOOL m_bFlashingRect;
		//是否选区跟随鼠标移动
		BOOL  m_bAutopan;
		int m_nAutopanSpeed;
		//鼠标高亮绘制(多一个圈圈底色，标示鼠标点击位置)
		BOOL m_bHighlightCursor;
		int m_highlightsize;
		int m_highlightshape;
		COLORREF m_highlightcolor ;

		//是否录制鼠标
		BOOL m_bRecordCursor;

		//about Region
		RECT   m_rcOffset;
		RECT   m_rcClip; 
		RECT   m_rcUse; 
		RECT   m_old_rcClip;
		POINT  m_ptOrigin;
		RECT m_rcFixed;//已设置好的Region 
		int m_nCaptureWidth;
		int m_nCaptureHeight;
		int m_nMaxxScreen;
		int m_nMaxyScreen;

		int m_nCursorType; 
		HCURSOR m_hSavedCursor;
		HANDLE m_hScreenCaptureThread;

		CaptureScreenCallback m_pCallback;
		void* m_pMaster;
		int m_nId;

		//图像RGB内存缓存
		PRGBTRIPLE m_hdib;
};

