//********************************************************************************
//* FlashingWindow.CPP
//*
//********************************************************************************

#include "stdafx.h"
#include "FlashingWnd.h"

#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define THICKNESS 8
#define SMALLTHICKNESS 4
#define SIDELEN 12
#define SIDELEN2 24


//********************************************************************************
//* Constructor
//********************************************************************************

CFlashingWnd::CFlashingWnd()
{
}


//********************************************************************************
//* Destructor
//********************************************************************************

CFlashingWnd::~CFlashingWnd()
{
}


BEGIN_MESSAGE_MAP(CFlashingWnd, CWnd)
	//{{AFX_MSG_MAP(CFlashingWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//********************************************************************************
//* CreateFlashing()
//*
//* Creates the main application window Flashing
//********************************************************************************

void CFlashingWnd::CreateFlashing(LPCTSTR pTitle, RECT &rect)
{	
	CreateEx(	WS_EX_TOPMOST  ,
						AfxRegisterWndClass(0,LoadCursor(NULL, IDC_ARROW)),						
						pTitle,
						WS_POPUP ,
						rect,
						NULL,
						NULL,
						NULL );

	oldregion = NULL;

}


//********************************************************************************
//* SetupRegion()
//*
//* Set the Window Region for transparancy outside the mask region
//********************************************************************************
void CFlashingWnd::SetUpRegion(int x, int y, int width, int height, int type)
{

	CRgn  wndRgn, rgnTemp, rgnTemp2,rgnTemp3;

		
	cRect.left= x;
	cRect.top= y;
	cRect.right = cRect.left + width -1;
	cRect.bottom = cRect.top + height -1;	

	if (type == 0)
	{
		wndRgn.CreateRectRgn(0,0, cRect.Width()+THICKNESS+THICKNESS, cRect.Height()+THICKNESS+THICKNESS);
		rgnTemp.CreateRectRgn(THICKNESS, THICKNESS, cRect.Width()+THICKNESS+1, cRect.Height()+THICKNESS+1);
		rgnTemp2.CreateRectRgn(0, SIDELEN2, cRect.Width()+THICKNESS+THICKNESS, cRect.Height()-SIDELEN+1);
		rgnTemp3.CreateRectRgn(SIDELEN2,0, cRect.Width()-SIDELEN+1, cRect.Height()+THICKNESS+THICKNESS);
	
		wndRgn.CombineRgn(&wndRgn,&rgnTemp,RGN_DIFF);
		wndRgn.CombineRgn(&wndRgn,&rgnTemp2,RGN_DIFF);
		wndRgn.CombineRgn(&wndRgn,&rgnTemp3,RGN_DIFF);

		wndRgn.OffsetRgn( cRect.left-THICKNESS, cRect.top-THICKNESS );	

	}
	else {

		wndRgn.CreateRectRgn(0,0, cRect.Width()+SMALLTHICKNESS+SMALLTHICKNESS, cRect.Height()+SMALLTHICKNESS+SMALLTHICKNESS);
		rgnTemp.CreateRectRgn(SMALLTHICKNESS, SMALLTHICKNESS, cRect.Width()+SMALLTHICKNESS+1, cRect.Height()+SMALLTHICKNESS+1);

		wndRgn.CombineRgn(&wndRgn,&rgnTemp,RGN_DIFF);

		wndRgn.OffsetRgn( cRect.left-SMALLTHICKNESS, cRect.top-SMALLTHICKNESS );	

	}		
	
	HRGN newregion = (HRGN) wndRgn.Detach();
	SetWindowRgn((HRGN) newregion, TRUE); 
	
	if (oldregion) DeleteObject(oldregion);
	oldregion = newregion;

}


void CFlashingWnd::SetUpRect(int x, int y, int width, int height)
{		
	cRect.left= x;
	cRect.top= y;
	cRect.right = cRect.left + width -1;
	cRect.bottom = cRect.top + height -1;
}

//********************************************************************************
//* CFlashingWnd message handlers
//********************************************************************************


void CFlashingWnd::PaintBorder(COLORREF colorval)
{

	// Add your drawing code here!
	HDC hdc = ::GetDC(m_hWnd);
	if ((cRect.right>cRect.left) && (cRect.bottom>cRect.top)) {	
		
		HBRUSH newbrush = (HBRUSH) CreateSolidBrush( colorval);
		HBRUSH newpen = (HBRUSH) CreatePen(PS_SOLID,1, colorval);
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc,newbrush);
		HBRUSH oldpen = (HBRUSH) SelectObject(hdc,newpen);

		
		Rectangle(hdc,cRect.left-THICKNESS,cRect.top-THICKNESS,cRect.right+THICKNESS,cRect.bottom+THICKNESS); 
		
		SelectObject(hdc,oldpen);
		SelectObject(hdc,oldbrush);
		DeleteObject(newpen);	
		DeleteObject(newbrush);	

	}
   
	::ReleaseDC(m_hWnd,hdc);
}


void CFlashingWnd::PaintInvertedBorder(COLORREF colorval)
{
	// Add your drawing code here!
	HDC hdc = ::GetDC(m_hWnd);
	if ((cRect.right>cRect.left) && (cRect.bottom>cRect.top)) {	
		
		HBRUSH newbrush = (HBRUSH) CreateSolidBrush( colorval);
		HBRUSH newpen = (HBRUSH) CreatePen(PS_SOLID,1, colorval);
		HBRUSH oldbrush = (HBRUSH) SelectObject(hdc,newbrush);
		HBRUSH oldpen = (HBRUSH) SelectObject(hdc,newpen);
		HDC hScreenDC=::GetDC(NULL);
		int maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
		int maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);
		::ReleaseDC(NULL,hScreenDC);		
		
		PatBlt(hdc, 0, 0, maxxScreen, maxyScreen, PATINVERT);		
		//PatBlt(hdc, cRect.left-THICKNESS, cRect.left-THICKNESS, cRect.right+THICKNESS - (cRect.left-THICKNESS) + 1, cRect.bottom+THICKNESS - (cRect.top-THICKNESS) + 1, PATINVERT);
		
		SelectObject(hdc,oldpen);
		SelectObject(hdc,oldbrush);
		DeleteObject(newpen);	
		DeleteObject(newbrush);	

	}
   
	::ReleaseDC(m_hWnd,hdc);
}


BOOL CFlashingWnd::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}