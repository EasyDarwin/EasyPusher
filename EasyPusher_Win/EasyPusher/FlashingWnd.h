//********************************************************************************
//* FlashingWindow.CPP
//********************************************************************************

#if !defined(AFX_FLASHINGWND_H__INCLUDED_)
#define AFX_FLASHINGWND_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif


class CFlashingWnd : public CWnd
{

public:
	CFlashingWnd();

	void CreateFlashing(LPCTSTR pTitle, RECT &rect);
	void SetUpRegion(int x, int y, int width, int height, int type);
	void SetUpRect(int x, int y, int width, int height);
	void PaintBorder(COLORREF colorval);
	void PaintInvertedBorder(COLORREF colorval);


public:
	CRect cRect;
	HRGN oldregion;
	//HBRUSH newbrush;
	//HBRUSH newpen;
	//HBRUSH oldbrush;
	//HBRUSH oldpen;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlashingWnd)
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlashingWnd();

protected:
	

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlashingWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
