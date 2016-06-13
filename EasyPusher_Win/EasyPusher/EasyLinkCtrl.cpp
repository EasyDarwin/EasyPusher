// EasyLinkCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EasyPusher.h"
#include "EasyLinkCtrl.h"


// CEasyLinkCtrl

IMPLEMENT_DYNAMIC(CEasyLinkCtrl, CLinkCtrl)

CEasyLinkCtrl::CEasyLinkCtrl()
{

}

CEasyLinkCtrl::~CEasyLinkCtrl()
{
}


BEGIN_MESSAGE_MAP(CEasyLinkCtrl, CLinkCtrl)
	//ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CEasyLinkCtrl message handlers




void CEasyLinkCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CLinkCtrl::OnPaint() for painting messages
// 		CBrush brushBkgnd; 
// 	CRect rcClient;
// 	brushBkgnd.CreateSolidBrush(COLOR_BACK);
// 	GetClientRect(&rcClient);
// 	dc.FillRect(&rcClient, &brushBkgnd);
// 	brushBkgnd.DeleteObject(); //ÊÍ·Å»­Ë¢ 

}


HBRUSH CEasyLinkCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CLinkCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
// 	pDC->SetBkMode(TRANSPARENT);   
// 	return (HBRUSH)GetStockObject(NULL_BRUSH);  

	return hbr;
}


BOOL CEasyLinkCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;//CLinkCtrl::OnEraseBkgnd(pDC);
}
