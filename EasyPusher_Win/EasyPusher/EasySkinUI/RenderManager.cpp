/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "StdAfx.h"
#include "RenderManager.h"

CUIRenderManager::CUIRenderManager(void)
{
	m_hResInstance = NULL;

	SetDefaultFont(TEXT("宋体"),12,false,false,false);
}

CUIRenderManager::~CUIRenderManager(void)
{
}

//创建单例
CUIRenderManager* CUIRenderManager::GetInstance()
{
	static CUIRenderManager _Instance;

	return &_Instance;
}

static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
		GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
		GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
							   int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if (NULL == hTempDC)
		return FALSE;

	//Creates Source DIB
	LPBITMAPINFO lpbiSrc = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
	{
		::DeleteDC(hTempDC);
		return FALSE;
	}
	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = dwWidth;
	lpbiSrc->bmiHeader.biHeight = dwHeight;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	COLORREF* pSrcBits = NULL;
	HBITMAP hSrcDib = CreateDIBSection (
		hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
		NULL, NULL);

	if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
	{
		delete [] lpbiSrc;
		::DeleteDC(hTempDC);
		return FALSE;
	}

	HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
	::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	//Creates Destination DIB
	LPBITMAPINFO lpbiDest = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiDest->bmiHeader.biWidth = dwWidth;
	lpbiDest->bmiHeader.biHeight = dwHeight;
	lpbiDest->bmiHeader.biPlanes = 1;
	lpbiDest->bmiHeader.biBitCount = 32;
	lpbiDest->bmiHeader.biCompression = BI_RGB;
	lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiDest->bmiHeader.biXPelsPerMeter = 0;
	lpbiDest->bmiHeader.biYPelsPerMeter = 0;
	lpbiDest->bmiHeader.biClrUsed = 0;
	lpbiDest->bmiHeader.biClrImportant = 0;

	COLORREF* pDestBits = NULL;
	HBITMAP hDestDib = CreateDIBSection (
		hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
		NULL, NULL);

	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if( src_darken < 0.0 ) src_darken = 0.0;
		*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	} //for

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	delete [] lpbiDest;
	::DeleteObject(hDestDib);

	delete [] lpbiSrc;
	::DeleteObject(hSrcDib);

	::DeleteDC(hTempDC);
	return TRUE;
}

bool CUIRenderManager::GetWorkDirectory( TCHAR szWorkDirectory[], WORD wBufferCount )
{
	//模块路径
	TCHAR szModulePath[MAX_PATH<<1]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0]!=0);
	lstrcpyn(szWorkDirectory,szModulePath,wBufferCount);

	return true;
}

//区域色块
void CUIRenderManager::DrawColor(HDC hDC, const RECT& rc, COLORREF color)
{
	::SetBkColor(hDC, color);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}
//渐变色块
void CUIRenderManager::DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
{
	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
	if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
	typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

	BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
	if( bAlpha == 0 ) return;
	int cx = rc.right - rc.left;
	int cy = rc.bottom - rc.top;
	RECT rcPaint = rc;
	HDC hPaintDC = hDC;
	HBITMAP hPaintBitmap = NULL;
	HBITMAP hOldPaintBitmap = NULL;
	if( bAlpha < 255 )
	{
		rcPaint.left = rcPaint.top = 0;
		rcPaint.right = cx;
		rcPaint.bottom = cy;
		hPaintDC = ::CreateCompatibleDC(hDC);
		hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
		ASSERT(hPaintDC);
		ASSERT(hPaintBitmap);
		hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
	}

	if( lpGradientFill != NULL ) 
	{
		TRIVERTEX triv[2] = 
		{
			{ rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
			{ rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
		};
		GRADIENT_RECT grc = { 0, 1 };
		lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
	}
	else 
	{
		// Determine how many shades
		int nShift = 1;
		if( nSteps >= 64 ) nShift = 6;
		else if( nSteps >= 32 ) nShift = 5;
		else if( nSteps >= 16 ) nShift = 4;
		else if( nSteps >= 8 ) nShift = 3;
		else if( nSteps >= 4 ) nShift = 2;
		int nLines = 1 << nShift;
		
		for( int i = 0; i < nLines; i++ )
		{
			// Do a little alpha blending
			BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
			BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
			BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
			// ... then paint with the resulting color
			HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
			RECT r2 = rcPaint;
			if( bVertical ) 
			{
				r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
				r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
				if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
			}
			else 
			{
				r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
				r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
				if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
			}
			::DeleteObject(hBrush);
		}
	}

	if( bAlpha < 255 )
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
		lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
		::SelectObject(hPaintDC, hOldPaintBitmap);
		::DeleteObject(hPaintBitmap);
		::DeleteDC(hPaintDC);
	}
}

//绘制线条
void CUIRenderManager::DrawLine(HDC hDC, const RECT& rc, int nSize, COLORREF dwPenColor,int nStyle/* = PS_SOLID*/)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	LOGPEN lg;
	lg.lopnColor = dwPenColor;
	lg.lopnStyle = nStyle;
	lg.lopnWidth.x = nSize;
	HPEN hPen = CreatePenIndirect(&lg);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	POINT ptTemp = { 0 };
	::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
	::LineTo(hDC, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

//绘制矩形
void CUIRenderManager::DrawRect(HDC hDC, const RECT& rc, int nSize, COLORREF dwPenColor)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, dwPenColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

//圆角矩形
void CUIRenderManager::DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, COLORREF dwPenColor)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, dwPenColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

//文字尺寸
SIZE CUIRenderManager::GetTextSize(HDC hDC, LPCTSTR pstrText, HFONT hFont, UINT uStyle)
{
	SIZE size = {0,0};
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	if( pstrText == NULL ) return size;
	::SetBkMode(hDC, TRANSPARENT);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	GetTextExtentPoint32(hDC, pstrText, _tcslen(pstrText) , &size);
	::SelectObject(hDC, hOldFont);
	return size;
}

void CUIRenderManager::SetDefaultFont( LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic )
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy_s(lf.lfFaceName, pStrFontName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	m_hDefaultFont = ::CreateFontIndirect(&lf);

	if( m_hDefaultFont == NULL ) throw TEXT("创建字体失败");
}

//获取图片
CImageEx * CUIRenderManager::GetImage( LPCTSTR lpszFileName,LPCTSTR lpszResType/*=NULL*/ )
{
	map<LPCTSTR,tagImageInfo*>::iterator iter;
	pair<std::map<LPCTSTR, tagImageInfo *>::iterator, bool> pairInsert;
	
	tagImageInfo * pImageInfo = NULL;
	CImageEx * pImage=NULL;

	if (NULL == lpszFileName || NULL == *lpszFileName)
		return NULL;

	iter = m_ArrayImage.find(lpszFileName);
	if (iter != m_ArrayImage.end())
	{
		pImageInfo = iter->second;
		if (pImageInfo != NULL)
		{
			pImageInfo->nRef++;
			
			pImageInfo->pImage->DestroyImage();

			//路径加载资源
			if( lpszResType == NULL )
			{
				// MAX_PATH长度在多串情况下，汉字可能越界，故而长度*2 [7/20/2015-14:34:06 Dingshuai]
				TCHAR szWorkDirectory[MAX_PATH<<1]={0};
				GetWorkDirectory(szWorkDirectory,MAX_PATH<<1);
				StrCat(szWorkDirectory,TEXT("\\"));
				StrCat(szWorkDirectory,lpszFileName);

				CFileFind fileFind;

				//判断是否exe目录下的资源
				if ( fileFind.FindFile(szWorkDirectory) )
				{
					pImageInfo->pImage->LoadImage(szWorkDirectory);
				}
				else //lpszFileName为完整路径
				{
					pImageInfo->pImage->LoadImage(lpszFileName);
				}
				fileFind.Close();
			}
			else
			{
				pImageInfo->pImage->LoadImage(m_hResInstance,lpszFileName,lpszResType);
			}

			return pImageInfo->pImage;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		pImageInfo = new tagImageInfo;
		pImage = new CImageEx;
		if (NULL == pImageInfo || NULL == pImage)
		{
			SafeDelete(pImageInfo);
			SafeDelete(pImage);
			return NULL;
		}

		pairInsert = m_ArrayImage.insert(pair<LPCTSTR, tagImageInfo *>(lpszFileName, pImageInfo));
		if (!pairInsert.second)
		{
			SafeDelete(pImageInfo);
			SafeDelete(pImage);
			return NULL;
		}

		bool bReturn = false;

		//路径加载资源
		if( lpszResType == NULL )
		{
			TCHAR szWorkDirectory[MAX_PATH<<1]={0};
			GetWorkDirectory(szWorkDirectory,MAX_PATH<<1);
			StrCat(szWorkDirectory,TEXT("\\"));
			StrCat(szWorkDirectory,lpszFileName);
			//sprintf_s(szWorkDirectory,_TRUNCATE,TEXT("\\%s"),lpszFileName);

			CFileFind fileFind;

			//判断是否exe目录下的资源
			if ( fileFind.FindFile(szWorkDirectory) )
			{
				bReturn = pImage->LoadImage(szWorkDirectory);
			}
			else //lpszFileName为完整路径
			{
				bReturn = pImage->LoadImage(lpszFileName);
			}
			fileFind.Close();

		}
		else
		{
			bReturn = pImage->LoadImage(m_hResInstance,lpszFileName,lpszResType);
		}

		if (!bReturn)
		{
			m_ArrayImage.erase(pairInsert.first);
			SafeDelete(pImageInfo);
			SafeDelete(pImage);
			return NULL;
		}

		pImageInfo->pImage = pImage;
		pImageInfo->nRef = 1;

		return pImage;
	}
}

//删除图片
void CUIRenderManager::RemoveImage( CImageEx *&pImage )
{
	map<LPCTSTR, tagImageInfo *>::iterator iter;
	tagImageInfo * pImageInfo;

	if (NULL == pImage) return;

	for (iter = m_ArrayImage.begin(); iter != m_ArrayImage.end(); iter++)
	{
		pImageInfo = iter->second;
		if (pImageInfo != NULL)
		{
			if (pImageInfo->pImage == pImage)
			{
				pImageInfo->nRef--;
				if (pImageInfo->nRef <= 0)
				{
					pImageInfo->pImage->DestroyImage();

					SafeDelete(pImageInfo->pImage);
					SafeDelete(pImageInfo);

					m_ArrayImage.erase(iter);
				}

				pImage = NULL;
				break;
			}
		}
	}
}

//删除所有
void CUIRenderManager::ClearImage()
{
	map<LPCTSTR, tagImageInfo *>::iterator iter;
	tagImageInfo * pImageInfo;

	for (iter = m_ArrayImage.begin(); iter != m_ArrayImage.end(); iter++)
	{
		pImageInfo = iter->second;
		if (pImageInfo != NULL)
		{
			if (pImageInfo->pImage != NULL)
			{
				pImageInfo->pImage->DestroyImage();
				SafeDelete(pImageInfo->pImage);
			}

			SafeDelete(pImageInfo);
		}
	}

	m_ArrayImage.clear();
}

void CUIRenderManager::AddFont( LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic )
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcsncpy_s(lf.lfFaceName, pStrFontName, LF_FACESIZE);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);

	if( hFont != NULL )
		m_ArrayFont.push_back(hFont);
}

HFONT CUIRenderManager::GetFont( int nIndex )
{
	if ( nIndex>m_ArrayFont.size() ) return NULL;
	
	return m_ArrayFont.at(nIndex);
}

HFONT CUIRenderManager::GetEndFont()
{
	return *(m_ArrayFont.end()-1);
}

