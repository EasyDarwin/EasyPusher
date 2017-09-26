/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/

#include "StdAfx.h"
#include "CaptureScreen.h"
#include "resource.h"

CCaptureScreen* CCaptureScreen::s_pCaptureScreen[MAX_VIDEO_WINDOW_NUM] = {NULL}; 

CCaptureScreen::CCaptureScreen(void)
{
	m_hMouseCaptureWnd = NULL;
	m_bCapturing = FALSE;
	m_nMouseCaptureMode = 1;
	m_hSavedBitmap = NULL;
	m_bFlashingRect = TRUE;
	m_bAutopan=FALSE;
	m_pFrame = NULL;
	m_bHighlightCursor = FALSE;
	m_bRecordCursor = 1;
	m_nCursorType=0; 
	m_hSavedCursor = NULL;
	m_highlightsize=64;
	m_highlightshape=0;
	m_highlightcolor = RGB(255,255,125);
	m_nCaptureWidth = 640;
	m_nCaptureHeight = 480;
	m_nColorBits = 24;
	m_nAutopanSpeed = 50;//1-200
	m_pCallback = NULL;
	m_pMaster = NULL;
	m_nId = NULL;
	m_hdib = NULL;
}

CCaptureScreen::~CCaptureScreen(void)
{
}

int CCaptureScreen::Init()
{
	CreateShiftWindow();
	HDC hScreenDC = ::GetDC(NULL);	
	m_nColorBits = ::GetDeviceCaps(hScreenDC, BITSPIXEL );	
	::ReleaseDC(NULL,hScreenDC);		

	if (!m_hdib)
	{
		m_hdib = (PRGBTRIPLE)malloc(m_nMaxxScreen*m_nMaxyScreen*4);//32位图像大小
	}
	if (!m_pFrame)
	{
		m_pFrame = new CFlashingWnd;	
	}
	CRect rect(0, 0, m_nMaxxScreen-1, m_nMaxyScreen-1);
	m_pFrame->CreateFlashing(_T("Flashing"), rect);	
	return 1;
}

void CCaptureScreen::UnInit()
{
		DestroyShiftWindow();
		if (m_hSavedBitmap)
		{
			DeleteObject(m_hSavedBitmap);
			m_hSavedBitmap = NULL;
		}
		if (m_pFrame)
		{
			m_pFrame->DestroyWindow();
			delete m_pFrame;
			m_pFrame = NULL;
		}
		if (m_hdib)
		{
			free(m_hdib);
			m_hdib = NULL;
		}
}

//调用这个函数来申请该类的实例化指针
CCaptureScreen* CCaptureScreen::Instance(int& nIndex)
{
		for (int nI=0; nI<MAX_VIDEO_WINDOW_NUM; nI++)
		{
			//申请实例化指针
			if (s_pCaptureScreen[nI] ==NULL)
			{
				s_pCaptureScreen[nI] =new CCaptureScreen();
				s_pCaptureScreen[nI]->Init();
				s_pCaptureScreen[nI]->m_nId = nI;
				nIndex = nI;
				return s_pCaptureScreen[nI];
			}
		}
	nIndex = -1;
	return NULL;
}
void CCaptureScreen::UnInstance(int nI)
{
		//for (int nI=0; nI<MAX_VIDEO_WINDOW_NUM; nI++)
		{
			if (s_pCaptureScreen[nI])
			{
				//删除该实例之前你应该先停止正在进行的操作、释放占用的资源
				s_pCaptureScreen[nI]->StopScreenCapture();
				//等待捕获线程结束

				s_pCaptureScreen[nI]->UnInit();

				delete s_pCaptureScreen[nI];
				s_pCaptureScreen[nI] =NULL;
			}
		}
}


LRESULT CCaptureScreen::ProcessMouseCapMsg(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
	case WM_MOUSEMOVE:
		{
			if (m_nMouseCaptureMode==0) //Fixed Region(固定大小选区)
			{ 
				POINT pt;
				GetCursorPos(&pt);				

				m_rcClip.left = pt.x+m_rcOffset.left;   // Update rect with new mouse info
				m_rcClip.top = pt.y+m_rcOffset.top;               				
				m_rcClip.right = pt.x+m_rcOffset.right;
				m_rcClip.bottom = pt.y+m_rcOffset.bottom;

				if (m_rcClip.left<0) 
				{

					m_rcClip.left=0;
					m_rcClip.right=((m_rcFixed.right)-(m_rcFixed.left));						
				}
				if (m_rcClip.top<0) {

					m_rcClip.top=0;
					m_rcClip.bottom=((m_rcFixed.bottom)-(m_rcFixed.top));									

				}
				if (m_rcClip.right>m_nMaxxScreen-1)
				{

					m_rcClip.right=m_nMaxxScreen-1;
					m_rcClip.left=m_nMaxxScreen-1-((m_rcFixed.right)-(m_rcFixed.left));
				}
				if (m_rcClip.bottom>m_nMaxyScreen-1)
				{
					m_rcClip.bottom=m_nMaxyScreen-1;
					m_rcClip.top=m_nMaxyScreen-1-((m_rcFixed.bottom)-(m_rcFixed.top));					
				}                

				if (!isRectEqual(m_old_rcClip,m_rcClip))
				{
					HDC hScreenDC = GetDC(hWnd);		
					DrawSelect(hScreenDC, FALSE, &m_old_rcClip);  // erase old rubber-band																	
					DrawSelect(hScreenDC, TRUE, &m_rcClip); // new rubber-band
					ReleaseDC(hWnd,hScreenDC);

				}// if old

				m_old_rcClip=m_rcClip;			          
			}
			else if (m_nMouseCaptureMode==1)//Variable Region（自定义选区）
			{ 
				if (m_bMouseDraging)
				{						
					POINT pt;
					GetCursorPos(&pt);

					HDC hScreenDC = GetDC(hWnd);	

					DrawSelect(hScreenDC, FALSE, &m_rcClip);  // erase old rubber-band

					m_rcClip.left = m_ptOrigin.x;
					m_rcClip.top = m_ptOrigin.y;					
					m_rcClip.right = pt.x;
					m_rcClip.bottom = pt.y;

					NormalizeRect(&m_rcClip);
					DrawSelect(hScreenDC, TRUE, &m_rcClip); // new rubber-band
					//TextOut(hScreenDC,pt.x,pt.y,"Lolo",4);

					ReleaseDC(hWnd,hScreenDC);
				}
			}	

			return DefWindowProc(hWnd, wMessage, wParam, lParam);
		}
	case WM_LBUTTONUP:
		{
			if (m_nMouseCaptureMode==0) 
			{
				//erase final
				HDC hScreenDC = GetDC(hWnd);		
				DrawSelect(hScreenDC, FALSE, &m_old_rcClip);    
				m_old_rcClip=m_rcClip;
				ReleaseDC(hWnd,hScreenDC);
			}
			else if (m_nMouseCaptureMode==1)
			{

				NormalizeRect(&m_rcClip);
				m_old_rcClip=m_rcClip;
				m_bMouseDraging = FALSE;				
			}				

			ShowWindow(hWnd,SW_HIDE);

			if (!IsRectEmpty(&m_old_rcClip)) 
			{
				NormalizeRect(&m_old_rcClip);
				CopyRect(&m_rcUse, &m_old_rcClip);

				//PostMessage (m_hMainWnd,WM_USER_RECORDSTART, 0, (LPARAM) 0); 
				CreateCaptureScreenThread();
			}		        

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
		}		
	case WM_LBUTTONDOWN:
		{
			// User pressed left button, initialize selection
			// Set origin to current mouse position (in window coords)			
			if (m_nMouseCaptureMode==1)
			{
				POINT pt;
				GetCursorPos(&pt);

				m_ptOrigin=pt;
				m_rcClip.left = m_rcClip.right = pt.x;
				m_rcClip.top = m_rcClip.bottom = pt.y;                

				NormalizeRect(&m_rcClip);     // Make sure it is a normal rect
				HDC hScreenDC = GetDC(hWnd);
				DrawSelect(hScreenDC, TRUE, &m_rcClip); // Draw the rubber-band box
				ReleaseDC(hWnd,hScreenDC);

				m_bMouseDraging = TRUE;

			}

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
		}

	case WM_RBUTTONDOWN:
		{
			if (m_nMouseCaptureMode==0) 
			{	//Cancel the operation				 

				//erase final
				HDC hScreenDC = GetDC(hWnd);		
				DrawSelect(hScreenDC, FALSE, &m_old_rcClip);    
				ReleaseDC(hWnd,hScreenDC);

				//Cancel the operation
				ShowWindow(hWnd,SW_HIDE);				
			}

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
		}		
	case WM_KEYDOWN:
		{
			int nVirtKey = (int) wParam;    // virtual-key code 
			int lKeyData = lParam;          // key data 			

			if (nVirtKey==VK_ESCAPE) { //Cancel the operation

				if (m_nMouseCaptureMode==0)
				{
					//erase final
					HDC hScreenDC = GetDC(hWnd);		
					DrawSelect(hScreenDC, FALSE, &m_old_rcClip);    
					ReleaseDC(hWnd,hScreenDC);

				}
				else if (m_nMouseCaptureMode==1)
				{

					NormalizeRect(&m_rcClip);
					m_old_rcClip=m_rcClip;
					if (m_bMouseDraging) 
					{
						m_bMouseDraging = FALSE;				
					}
				}		

				ShowWindow(hWnd,SW_HIDE);				

			} //VK_ESCAPE

			return DefWindowProc(hWnd, wMessage, wParam, lParam);    
		}
	default:
		return DefWindowProc(hWnd, wMessage, wParam, lParam);    
	}
}

LRESULT WINAPI CCaptureScreen::MouseCaptureWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	for (int nI=0; nI<MAX_VIDEO_WINDOW_NUM; nI++)
	{
		if (s_pCaptureScreen[nI])
		{
			if (s_pCaptureScreen[nI]->m_hMouseCaptureWnd == hWnd)//通过鼠标捕获窗口句柄确定是哪个屏幕捕获类实例
			{
				return s_pCaptureScreen[nI]->ProcessMouseCapMsg(hWnd, wMessage, wParam, lParam);
			}
		}
	}
	return DefWindowProc(hWnd, wMessage, wParam, lParam);    
}

int CCaptureScreen::CreateShiftWindow() 
{
	// Code For Creating a Window for Specifying Region
	// A borderless, invisible window used only for capturing mouse input for the whole screen
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	WNDCLASS wndclass;

	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)(&CCaptureScreen::MouseCaptureWndProc);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, _T("WINCAP"));

	HICON hcur= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONCROSSHAIR));
	//wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclass.hCursor = hcur;
	wndclass.hbrBackground = NULL;

	//wndclass.lpszMenuName = (LPSTR)"MAINMENU";
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPCWSTR) _T("ShiftRegionWindow");

	if (!RegisterClass(&wndclass))
		return 0;

	HDC hScreenDC=::GetDC(NULL);
	m_nMaxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
	m_nMaxyScreen = GetDeviceCaps(hScreenDC,VERTRES);
	::ReleaseDC(NULL,hScreenDC);		

	 HWND hMouseWnd= CreateWindowEx(WS_EX_TOPMOST, _T("ShiftRegionWindow"), 
		_T("Title"),WS_POPUP,0,0,m_nMaxxScreen,m_nMaxyScreen,NULL,NULL, hInstance,  NULL);
	 m_hMouseCaptureWnd = hMouseWnd;
	DWORD dwCode = ::GetLastError();
	return 0;
}

int CCaptureScreen::InitSelectRegionWindow()
{
	return 0;
}

int CCaptureScreen::InitDrawShiftWindow() 
{
	HDC hScreenDC = ::GetDC(m_hMouseCaptureWnd);
	FixRectSizePos(&m_rcFixed, m_nMaxxScreen,  m_nMaxyScreen);			

	m_rcClip.left=m_rcFixed.left;
	m_rcClip.top=m_rcFixed.top;
	m_rcClip.right=m_rcFixed.right;
	m_rcClip.bottom=m_rcFixed.bottom;
	DrawSelect(hScreenDC, TRUE, &m_rcClip);

	m_old_rcClip = m_rcClip;

	//Set Curosr at the centre of the clip rectangle
	POINT   ptOrigin;
	ptOrigin.x=(m_rcClip.right+m_rcClip.left)/2;
	ptOrigin.y=(m_rcClip.top+m_rcClip.bottom)/2;

	m_rcOffset.left=m_rcClip.left-ptOrigin.x;
	m_rcOffset.top=m_rcClip.top-ptOrigin.y;
	m_rcOffset.right=m_rcClip.right-ptOrigin.x;
	m_rcOffset.bottom=m_rcClip.bottom-ptOrigin.y;  

	::ReleaseDC(m_hMouseCaptureWnd, hScreenDC);
	return 0;
}

int CCaptureScreen::DestroyShiftWindow() 
{
	if (m_hMouseCaptureWnd) 
		::DestroyWindow(m_hMouseCaptureWnd);
	return 0;
}

BOOL CCaptureScreen::isRectEqual(RECT a, RECT b)
{
	if ((a.left==b.left) && (a.right==b.right) && (a.top==b.top) && (a.bottom==b.bottom)) 
		return TRUE;
	else 
		return FALSE;
}

//***************************************************************************
//
// DrawSelect
//
// Draws the selected clip rectangle with its dimensions on the DC
//
//***************************************************************************
void CCaptureScreen::SaveBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy)
{
	if (m_hSavedBitmap) 
	{
		DeleteObject(m_hSavedBitmap);
		m_hSavedBitmap = NULL;
	}

	m_hSavedBitmap = (HBITMAP) CreateCompatibleBitmap(hdc,sx, sy);
	HBITMAP oldbitmap = (HBITMAP) SelectObject(hdcbits, m_hSavedBitmap);	
	BitBlt(hdcbits, 0, 0, sx, sy, hdc, x, y, SRCCOPY);
	
	SelectObject(hdcbits,oldbitmap);
}

void CCaptureScreen::RestoreBitmapCopy(HDC hdc,HDC hdcbits, int x, int y, int sx, int sy) 
{
	if (m_hSavedBitmap) 
	{
		HBITMAP oldbitmap = (HBITMAP) SelectObject(hdcbits, m_hSavedBitmap);
		BitBlt(hdc, x, y, sx, sy, hdcbits, 0, 0, SRCCOPY);
		SelectObject(hdcbits,oldbitmap);

		if (m_hSavedBitmap) 
			DeleteObject(m_hSavedBitmap);
		m_hSavedBitmap = NULL;
	}
}

void CCaptureScreen::DeleteBitmapCopy()
{
	if (m_hSavedBitmap)
	{
		DeleteObject(m_hSavedBitmap);
		m_hSavedBitmap = NULL;
	}
}

void CCaptureScreen::DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip)
{
	wchar_t sz[80];
	DWORD dw;
	int x, y, len, dx, dy;
	HDC hdcBits;    
	RECT rectDraw;
	SIZE sExtent;

	rectDraw = *lprClip;
	if (!IsRectEmpty(&rectDraw))
	{
		// If a rectangular clip region has been selected, draw it
		HBRUSH newbrush = (HBRUSH) CreateHatchBrush(HS_BDIAGONAL, RGB(0,0,100));
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc,newbrush);

		//PatBlt SRCINVERT regardless fDraw is TRUE or FALSE
		PatBlt(hdc, rectDraw.left, rectDraw.top, rectDraw.right-rectDraw.left, DINV, PATINVERT);
		PatBlt(hdc, rectDraw.left, rectDraw.bottom-DINV, DINV, -(rectDraw.bottom-rectDraw.top-2*DINV),  PATINVERT);
		PatBlt(hdc, rectDraw.right-DINV, rectDraw.top+DINV, DINV, rectDraw.bottom-rectDraw.top-2*DINV,   PATINVERT);
		PatBlt(hdc, rectDraw.right, rectDraw.bottom-DINV, -(rectDraw.right-rectDraw.left), DINV,  PATINVERT);

		SelectObject(hdc,oldbrush);
		DeleteObject(newbrush);

		hdcBits = CreateCompatibleDC(hdc);
		HFONT newfont = (HFONT) GetStockObject(ANSI_VAR_FONT);
		HFONT oldfont = (HFONT) SelectObject(hdc, newfont);            
		//HFONT oldfont = (HFONT) SelectObject(hdcBits, newfont);            

		wsprintf(sz, _T("Left : %d  Top : %d  Width : %d  Height : %d"), rectDraw.left, rectDraw.top, rectDraw.right - rectDraw.left+1, rectDraw.bottom -  rectDraw.top+1);
		len = lstrlen(sz);        
		dw = GetTextExtentPoint(hdc, sz, len, &sExtent);
		//dw = GetTextExtentPoint(hdcBits, sz, len, &sExtent);

		dx = sExtent.cx;
		dy = sExtent.cy;
		x=  rectDraw.left +10;

		if (rectDraw.top < (dy + DINV + 2)) 
			y=  rectDraw.bottom + DINV + 2;
		else
			y=  rectDraw.top - dy - DINV - 2;

		if (fDraw)	
		{		
			//Save Original Picture
			SaveBitmapCopy(hdc,hdcBits,  x-4, y-4, dx+8, dy+8); 

			//Text
			COLORREF oldtextcolor = SetTextColor(hdc,RGB(0,0,0));
			COLORREF oldbkcolor = SetBkColor(hdc,RGB(255,255,255));
			SetBkMode(hdc,TRANSPARENT);

			//Rectangle(hdc,x-1,y-1,x+dx, y+dy);
			RoundRect(hdc,x-4,y-4,x+dx+4, y+dy+4,10,10);

			SetBkMode(hdc,OPAQUE);

			ExtTextOut(hdc, x, y, 0, NULL, sz, len, NULL);
			SetBkColor(hdc,oldbkcolor);
			SetTextColor(hdc,oldtextcolor);
			SelectObject(hdc, oldfont);	
		}
		else 
			RestoreBitmapCopy(hdc,hdcBits,  x-4, y-4, dx+8, dy+8);

// 		//Icon
// 		if ((rectDraw.right-rectDraw.left-10 >  35) &&  (rectDraw.bottom-rectDraw.top-10 > dy + 40)) 
// 		{
// 			HBITMAP hbv = LoadBitmap( AfxGetInstanceHandle(),  MAKEINTRESOURCE(IDB_BITMAP1)); 
// 			HBITMAP old_bitmap = (HBITMAP) SelectObject(hdcBits, hbv);
// 			BitBlt(hdc, rectDraw.left+10, rectDraw.bottom-42, 30, 32,hdcBits, 0,0, SRCINVERT);
// 			SelectObject(hdcBits,old_bitmap);
// 			DeleteObject(hbv);
// 		}

		DeleteDC(hdcBits);
	}
}

void CCaptureScreen::NormalizeRect(LPRECT prc)
{
	if (prc->right  < prc->left) SWAP(prc->right,  prc->left);
	if (prc->bottom < prc->top)  SWAP(prc->bottom, prc->top);
}

void CCaptureScreen::FixRectSizePos(LPRECT prc,int maxxScreen, int maxyScreen)
{
	NormalizeRect(prc);

	int width=((prc->right)-(prc->left))+1;
	int height=((prc->bottom)-(prc->top))+1;

	if (width>maxxScreen)
	{
		prc->left=0;
		prc->right=maxxScreen-1;
	}

	if (height>maxyScreen) 
	{
		prc->top=0;
		prc->bottom=maxyScreen-1;
	}

	if (prc->left <0)
	{
		prc->left= 0;
		prc->right=width-1;
	}

	if (prc->top <0) 
	{
		prc->top= 0;
		prc->bottom=height-1;
	}

	if (prc->right > maxxScreen-1 )
	{
		prc->right = maxxScreen-1;
		prc->left= maxxScreen-width;
	}

	if (prc->bottom > maxyScreen-1 ) 
	{
		prc->bottom = maxyScreen-1;
		prc->top= maxyScreen-height;
	}
}

//////////////////////////////////////////////////////////////////////////
// About CaptureScreenFrame
// 
void CCaptureScreen::DrawFlashingRect(BOOL bDraw , int mode)
{
	if (m_pFrame)
	{
		if (mode == 1) 
		{ 
			m_pFrame->PaintInvertedBorder(RGB(0,255,80));
		}
		else
		{	
			//不断绘制闪烁，这样做是否合理？（简直亮瞎了24K钛合金狗眼~）
// 			if (bDraw)
// 				m_pFrame->PaintBorder(RGB(255,255,180));
// 			else
				m_pFrame->PaintBorder(RGB(0,255,80));
		}
	}
}

//Mouse Capture functions 
HCURSOR CCaptureScreen::FetchCursorHandle()
{
	if (m_nCursorType == 0) 
	{
		if (m_hSavedCursor == NULL) 
		{
			m_hSavedCursor = GetCursor();
		}
		return m_hSavedCursor;
	}
// 	else if (m_nCursorType == 1) 
// 	{
// 		return g_customcursor ;
// 	}
// 	else
// 	{
// 		return g_loadcursor;
// 	}
	return NULL;
}

HANDLE  CCaptureScreen::Bitmap2Dib( HBITMAP hbitmap, UINT bits )
{
	HDC                 hdc ;
	BITMAP              bitmap ;
	UINT                wLineLen ;
	DWORD               dwSize ;
	DWORD               wColSize ;


	GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

	//
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	//
	//wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
	wLineLen = ( (bitmap.bmWidth*bits+31)& 0xffffffe0) / 8;
	
	wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
// 	dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
// 		(DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;
	dwSize = /*sizeof(BITMAPINFOHEADER) + wColSize +*/ (DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

	//
	// Allocate room for a DIB and set the LPBI fields
	//
	//hdib = GlobalAlloc(GHND,dwSize); //全局的分配空间并且锁住，我们认为这是不合理的
	PRGBTRIPLE hdib =m_hdib;//(PRGBTRIPLE)malloc(dwSize);//动态分配的内存，一定要记得释放
	if (!hdib)
		return hdib ;

	BITMAPINFO pbi;

	pbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbi.bmiHeader.biWidth = bitmap.bmWidth;
	pbi.bmiHeader.biHeight = bitmap.bmHeight;
	pbi.bmiHeader.biPlanes = 1;
	pbi.bmiHeader.biBitCount = bits;
	pbi.bmiHeader.biCompression = BI_RGB;
	
	hdc = CreateCompatibleDC(NULL) ;
	GetDIBits(hdc, hbitmap, 0, bitmap.bmHeight, hdib, (LPBITMAPINFO)&pbi,  DIB_RGB_COLORS);

	DeleteDC(hdc) ;
	return hdib ;
}

void CCaptureScreen::InsertHighLight(HDC hdc,int xoffset, int yoffset)
{
	CSize fullsize;
	fullsize.cx=128;
	fullsize.cy=128;

	int highlightsize = m_highlightsize;
	COLORREF highlightcolor = m_highlightcolor;
	int highlightshape = m_highlightshape;

	double x1,x2,y1,y2;

	//OffScreen Buffer	
	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = ::CreateCompatibleDC(hdc);
	hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc,fullsize.cx,fullsize.cy);    	
	old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);		

	if ((highlightshape == 0) || (highlightshape == 2))//circle and square
	{ 
		x1 = (fullsize.cx - highlightsize)/2.0;
		x2 = (fullsize.cx + highlightsize)/2.0;
		y1 = (fullsize.cy - highlightsize)/2.0;
		y2 = (fullsize.cy + highlightsize)/2.0;
	}
	else if ((highlightshape == 1) || (highlightshape == 3)) //ellipse and rectangle
	{ 
		x1 = (fullsize.cx - highlightsize)/2.0;
		x2 = (fullsize.cx + highlightsize)/2.0;
		y1 = (fullsize.cy - highlightsize/2.0)/2.0;
		y2 = (fullsize.cy + highlightsize/2.0)/2.0;
	}

	HBRUSH ptbrush = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	HPEN nullpen = CreatePen(PS_NULL,0,0); 
	HBRUSH hlbrush = CreateSolidBrush( highlightcolor);

	HBRUSH oldbrush = (HBRUSH)  ::SelectObject(hdcBits,ptbrush);	
	HPEN oldpen = (HPEN) ::SelectObject(hdcBits,nullpen);			
	::Rectangle(hdcBits, 0,0,fullsize.cx+1,fullsize.cy+1);		

	//Draw the highlight
	::SelectObject(hdcBits,hlbrush);				

	if ((highlightshape == 0)  || (highlightshape == 1)) //circle and ellipse
	{
		::Ellipse(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
	}
	else if ((highlightshape == 2) || (highlightshape == 3)) //square and rectangle
	{ 
		::Rectangle(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
	}

	::SelectObject(hdcBits,oldbrush);		
	::SelectObject(hdcBits,oldpen);	
	DeleteObject(hlbrush);  	
	DeleteObject(nullpen);		

	//OffScreen Buffer
	BitBlt(hdc, xoffset, yoffset, fullsize.cx, fullsize.cy, hdcBits, 0, 0, SRCAND);  
	SelectObject(hdcBits, old_bitmap);        
	DeleteObject(hbm);
	DeleteDC(hdcBits);
}

void* CCaptureScreen::CaptureScreenFrame(int left,int top,int width, int height,int tempDisableRect)
{
	//获取桌面屏幕设备DC
	HDC hScreenDC = ::GetDC(NULL);

	//if flashing rect
	if (m_bFlashingRect && !tempDisableRect)
	{
		if (m_bAutopan) 
		{				
			m_pFrame->SetUpRegion(left,top,width,height,1);				
			DrawFlashingRect( TRUE , 1);			
		}
		else		
			DrawFlashingRect( TRUE , 0);
	}	

	HDC hMemDC = ::CreateCompatibleDC(hScreenDC);     
	HBITMAP hbm;

	hbm = CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP oldbm = (HBITMAP) SelectObject(hMemDC, hbm);	 
	BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, SRCCOPY);	 	

	//Get Cursor Pos
	POINT xPoint; 
	GetCursorPos( &xPoint ); 
	HCURSOR hcur= FetchCursorHandle();
	xPoint.x-=left;
	xPoint.y-=top;

	//Draw the HighLight	
	if (m_bHighlightCursor==1)
	{	
		POINT highlightPoint; 		
		highlightPoint.x = xPoint.x -64 ;
		highlightPoint.y = xPoint.y -64 ;		

		InsertHighLight( hMemDC, highlightPoint.x, highlightPoint.y);
	}

	//Draw the Cursor	
	if (m_bRecordCursor==1)
	{
		ICONINFO iconinfo ;	
		BOOL ret;
		ret	= GetIconInfo( hcur,  &iconinfo ); 
		if (ret) 
		{
			xPoint.x -= iconinfo.xHotspot;
			xPoint.y -= iconinfo.yHotspot;

			//need to delete the hbmMask and hbmColor bitmaps
			//otherwise the program will crash after a while after running out of resource
			if (iconinfo.hbmMask) DeleteObject(iconinfo.hbmMask);
			if (iconinfo.hbmColor) DeleteObject(iconinfo.hbmColor);
		}		

		::DrawIcon( hMemDC,  xPoint.x,  xPoint.y, hcur); 							
	}

	SelectObject(hMemDC,oldbm);    			
	void* pBM_HEADER = /*(LPBITMAPINFOHEADER)*/(Bitmap2Dib(hbm, 24));	//m_nColorBits
	//LPBITMAPINFOHEADER pBM_HEADER = (LPBITMAPINFOHEADER)GlobalLock(Bitmap2Dib(hbm, 24));	
	if (pBM_HEADER == NULL) 
	{ 
		MessageBox(NULL,_T("Error reading a frame!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);					
		exit(1);
	}    

	DeleteObject(hbm);			
	DeleteDC(hMemDC);	

	//if flashing rect
	if (m_bFlashingRect && !tempDisableRect)
	{
		if (m_bAutopan)	
		{
			DrawFlashingRect(FALSE , 1);
		}
		else
			DrawFlashingRect(FALSE , 0);
	}	

	ReleaseDC(NULL,hScreenDC) ;	
	return pBM_HEADER;
}

void CCaptureScreen::FreeFrame(void* alpbi)
{
	if (!alpbi)
		return ;

	free (alpbi);
	//GlobalFree(alpbi);
	alpbi = NULL;
}

//Interface Function

int CCaptureScreen::StartScreenCapture(HWND hShowWnd, int nCapMode )
{
	m_nMouseCaptureMode = nCapMode;
	m_hMainWnd = hShowWnd;
	if (m_nMouseCaptureMode==0)
	{
		m_rcFixed.top=0;
		m_rcFixed.left=0;
		m_rcFixed.right=m_nCaptureWidth-1;
		m_rcFixed.bottom=m_nCaptureHeight-1;	

		::ShowWindow(m_hMouseCaptureWnd,SW_MAXIMIZE);
		::UpdateWindow(m_hMouseCaptureWnd);

		InitDrawShiftWindow(); //will affect rc implicity
	}
	else if (m_nMouseCaptureMode==1)
	{
		::ShowWindow(m_hMouseCaptureWnd,SW_MAXIMIZE);
		::UpdateWindow(m_hMouseCaptureWnd);

		InitSelectRegionWindow(); //will affect rc implicity
	}
	else if (m_nMouseCaptureMode==2)
	{
		m_rcUse.left=0;
		m_rcUse.top=0;
		m_rcUse.right= m_nMaxxScreen-1;
		m_rcUse.bottom= m_nMaxyScreen-1;
		//::PostMessage (m_hMainWnd,WM_USER_RECORDSTART,0,(LPARAM) 0); 
		CreateCaptureScreenThread();
	}	

	return m_nMouseCaptureMode;
}
void CCaptureScreen::StopScreenCapture()
{
	m_bCapturing = 0;
	//等待线程结束
	Sleep(300);
	//获取线程结束代码 ,如果线程还在运行就等她结束
// 	DWORD dwExitCode ;
// 	::GetExitCodeThread(m_hScreenCaptureThread,&dwExitCode);
// 	if(dwExitCode == STILL_ACTIVE)
// 	{
// 		WaitForSingleObject(m_hScreenCaptureThread, INFINITE); 
// 		CloseHandle(m_hScreenCaptureThread);
// 		m_hScreenCaptureThread = INVALID_HANDLE_VALUE;
// 	}
}

//创建线程进行屏幕捕获
int CCaptureScreen::CreateCaptureScreenThread()
{
	//Check validity of rc and fix it		
	HDC hScreenDC = ::GetDC(NULL);	
	m_nMaxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
	m_nMaxyScreen = GetDeviceCaps(hScreenDC,VERTRES);		
	FixRectSizePos(&m_rcFixed,m_nMaxxScreen, m_nMaxyScreen);		

	::ReleaseDC(NULL,hScreenDC);	

	m_bCapturing=1;	

	int tdata=0;
	//CWinThread * pThread = AfxBeginThread(&CCaptureScreen::CaptureScreenThread, (LPVOID)this);	
	//创建线程
	m_hScreenCaptureThread=(HANDLE)_beginthreadex(NULL,0,(&CCaptureScreen::CaptureScreenThread),
		this,THREAD_PRIORITY_NORMAL,NULL);

	if (!m_hScreenCaptureThread)
	{
		AfxMessageBox(_T("Create Thread Error!"));
		return -1;
	}

	return 1;
}

UINT CCaptureScreen::CaptureScreenThread(LPVOID pParam)
{
	if (pParam)
	{
		CCaptureScreen* pMaster = (CCaptureScreen*)pParam;
		if (pMaster)
		{
			pMaster->CaptureVideoProcess();
		}
	}
	return 0;
}

void CCaptureScreen::GetCaptureScreenSize(int& nWidth, int& nHeight )
{
	nWidth = m_rcUse.right-m_rcUse.left+1;
	nHeight = m_rcUse.bottom - m_rcUse.top + 1;

	//  [1/27/2016 SwordTwelve]
	//长宽做一下修正，修正为16的倍数
	int nDivW = nWidth%16;
	int nDivH = nHeight%16;
	if (nDivW<8)
		nWidth -= nDivW;
	else
		nWidth += (16 - nDivW);

	if (nDivH<8)
		nHeight -= nDivH;
	else
		nHeight += (16 - nDivH);

	if (nWidth>m_nMaxxScreen)
	{
		nWidth = m_nMaxxScreen;
	}
	if (nHeight>m_nMaxyScreen)
	{
		nHeight = m_nMaxyScreen;
	}

}

void CCaptureScreen::CaptureVideoProcess()
{
	int top=m_rcUse.top;
	int left=m_rcUse.left;
	int width=m_rcUse.right-m_rcUse.left+1;
	int height=m_rcUse.bottom - m_rcUse.top + 1;

	//  [1/27/2016 SwordTwelve]
	//长宽做一下修正，修正为16的倍数
	int nDivW = width%16;
	int nDivH = height%16;
	if (nDivW<8)
		width -= nDivW;
	else
		width += (16 - nDivW);
	if (nDivH<8)
		height -= nDivH;
	else
		height += (16 - nDivH);
	if (width>m_nMaxxScreen)
	{
		width = m_nMaxxScreen;
	}
	if (height>m_nMaxyScreen)
	{
		height = m_nMaxyScreen;
	}
	//设置捕获帧率
	int nFps = 25;

	////////////////////////////////////////////////
	// Set Up Flashing Rect
	////////////////////////////////////////////////
	if (m_bFlashingRect)
	{
		if (m_bAutopan) 	
			m_pFrame->SetUpRegion(left,top,width,height,1);	
		else
			m_pFrame->SetUpRegion(left,top,width,height,0);
		m_pFrame->ShowWindow(SW_SHOW);
	}

	/*LPBITMAPINFOHEADER*/VOID*  alpbi = NULL;
	RECT panrect_current;
	RECT panrect_dest;

	if (m_bAutopan) 
	{
		panrect_current.left = left;
		panrect_current.top = top;
		panrect_current.right = left + width - 1;
		panrect_current.bottom = top + height - 1;
	}

	//Into Capture Loop
	while (m_bCapturing)
	{
		//Autopan
		if ((m_bAutopan) && (width < m_nMaxxScreen) && (height < m_nMaxyScreen))
		{
			POINT xPoint;
			GetCursorPos(&xPoint);

			int extleft = ((panrect_current.right - panrect_current.left)*1)/4 + panrect_current.left;
			int extright = ((panrect_current.right - panrect_current.left)*3)/4 + panrect_current.left;
			int exttop = ((panrect_current.bottom - panrect_current.top)*1)/4 + panrect_current.top;
			int extbottom = ((panrect_current.bottom - panrect_current.top)*3)/4 + panrect_current.top;				

			if (xPoint.x  < extleft )  //need to pan left
			{
				panrect_dest.left = xPoint.x - width/2;
				panrect_dest.right = panrect_dest.left +  width - 1;
				if (panrect_dest.left < 0) 
				{
					panrect_dest.left = 0;
					panrect_dest.right = panrect_dest.left +  width - 1;
				}
			}
			else if (xPoint.x  > extright ) //need to pan right
			{ 
				panrect_dest.left = xPoint.x - width/2;						
				panrect_dest.right = panrect_dest.left +  width - 1;
				if (panrect_dest.right >= m_nMaxxScreen)
				{
					panrect_dest.right = m_nMaxxScreen - 1;
					panrect_dest.left  = panrect_dest.right - width + 1;	
				}
			}
			else 
			{
				panrect_dest.right = panrect_current.right;
				panrect_dest.left  = panrect_current.left;
			}

			if (xPoint.y  < exttop )  //need to pan up
			{
				panrect_dest.top = xPoint.y - height/2;
				panrect_dest.bottom = panrect_dest.top +  height - 1;
				if (panrect_dest.top < 0) 
				{
					panrect_dest.top = 0;
					panrect_dest.bottom = panrect_dest.top +  height - 1;
				}
			}
			else if (xPoint.y  > extbottom ) { //need to pan down

				panrect_dest.top = xPoint.y - height/2;						
				panrect_dest.bottom = panrect_dest.top +  height - 1;
				if (panrect_dest.bottom >= m_nMaxyScreen)
				{
					panrect_dest.bottom = m_nMaxyScreen - 1;
					panrect_dest.top  = panrect_dest.bottom - height + 1;	
				}
			}
			else 
			{
				panrect_dest.top = panrect_current.top;
				panrect_dest.bottom  = panrect_current.bottom;
			}

			//Determine Pan Values
			int xdiff,ydiff;
			xdiff = panrect_dest.left - panrect_current.left;
			ydiff = panrect_dest.top - panrect_current.top;

			if (abs(xdiff) < m_nAutopanSpeed) 
			{
				panrect_current.left += xdiff;
			}
			else
			{
				if (xdiff<0) 
					panrect_current.left -= m_nAutopanSpeed;
				else
					panrect_current.left += m_nAutopanSpeed;
			}

			if (abs(ydiff) < m_nAutopanSpeed)
			{
				panrect_current.top += ydiff;
			}
			else
			{
				if (ydiff<0) 
					panrect_current.top -= m_nAutopanSpeed;
				else
					panrect_current.top += m_nAutopanSpeed;
			}				

			panrect_current.right = panrect_current.left + width - 1;
			panrect_current.bottom =  panrect_current.top + height - 1;

			alpbi=CaptureScreenFrame(panrect_current.left,panrect_current.top,width, height,0);					
		}
		else
			alpbi=CaptureScreenFrame(left,top,width, height,0);	

		// Capture Data callBack
		//测试显示
#if 0
		HWND hWnd=m_hMainWnd;
		HDC hDC=::GetDC(hWnd);

		::SetTextColor( hDC, RGB( 255, 255, 255 ) );
		::SetBkMode( hDC, TRANSPARENT );
		::SetStretchBltMode( hDC, COLORONCOLOR );

		CRect rect;
		::GetClientRect(m_hMainWnd, &rect);

			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth       = width;
			bmi.bmiHeader.biHeight      = height; // top-down image 
			bmi.bmiHeader.biPlanes      = 1;
			bmi.bmiHeader.biBitCount    = 24;//m_nColorBits;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage   = 0;

			unsigned char * pData = (unsigned char *)alpbi ;//+ alpbi->biSize + alpbi->biClrUsed * sizeof(RGBQUAD);
			::SetDIBitsToDevice( hDC, 0, 0, /*rect.Width()*/640, /*rect.Height()*/480, 0, 0, 0, height, (LPBYTE)pData , &bmi, DIB_RGB_COLORS );
#endif
			if (m_pCallback&&m_pMaster)
			{
					ScreenCapDataInfo sCapScreenInfo;
					sCapScreenInfo.nWidth = width;
					sCapScreenInfo.nHeight = height;
					sCapScreenInfo.nDataType = 24;
					m_pCallback(m_nId, (unsigned char*)(alpbi), /*alpbi->biSizeImage*/width*height*3, 1, &sCapScreenInfo, m_pMaster);
			} 

		//Slowly thread By framerate
		//Sleep(30);//1000/nFps
	}
	//捕获结束，隐藏闪烁区域框
	if (m_bFlashingRect)
	{
		m_pFrame->ShowWindow(SW_HIDE);
	}
}


//设置捕获数据回调函数
void CCaptureScreen::SetCaptureScreenCallback(CaptureScreenCallback callBack, void * pMaster)
{
	m_pCallback = callBack;
	m_pMaster = pMaster;
}
