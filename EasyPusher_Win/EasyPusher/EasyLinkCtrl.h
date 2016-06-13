#pragma once


// CEasyLinkCtrl

class CEasyLinkCtrl : public CLinkCtrl
{
	DECLARE_DYNAMIC(CEasyLinkCtrl)

public:
	CEasyLinkCtrl();
	virtual ~CEasyLinkCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


