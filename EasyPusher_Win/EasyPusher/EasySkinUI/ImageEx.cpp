/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "StdAfx.h"
#include "ImageEx.h"
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CImageEx::CImageEx()
{
	::SetRect(&m_rcNinePart,0,0,0,0);

	return;
}

//析构函数
CImageEx::~CImageEx()
{
	//销毁图片
	DestroyImage(); 

	return;
}

//销毁图片
 bool CImageEx::DestroyImage()
 {
	Destroy();

 	return true;
 }

//加载图片
bool CImageEx::LoadImage(LPCTSTR pszFileName)
{
	HRESULT hr = CImage::Load(pszFileName);
	if (hr == S_OK)
	{
		return SetAlphaBit();
	}
	else
	{
		return FALSE;
	}
}

//加载图片
bool CImageEx::LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName,LPCTSTR pszResourceType/*=TEXT("IMAGE")*/)
{
	//查找资源
	HRSRC hResource=FindResource(hInstance,pszResourceName,pszResourceType);
	if (hResource==NULL) return false;

	//读取资源
	DWORD dwImageSize=SizeofResource(hInstance,hResource);
	LPVOID pImageBuffer=LoadResource(hInstance,hResource);

	//创建数据
	IStream * pIStream=NULL;
	if (CreateStreamOnHGlobal(NULL,TRUE,&pIStream)!=S_OK)
	{
		ASSERT(FALSE);
		return false;
	}

	//写入数据
	pIStream->Write(pImageBuffer,dwImageSize,NULL);

	HRESULT hr = CImage::Load(pIStream);

 	//释放资源
 	SafeRelease(pIStream);

	if (hr == S_OK)
	{
		return SetAlphaBit();
	}
	else
	{
		return FALSE;
	}
}

//绘画图像
BOOL CImageEx::DrawImage(CDC * pDC, INT nXPos, INT nYPos)
{
	pDC->SetStretchBltMode(HALFTONE);
	return CImage::Draw(pDC->GetSafeHdc(),nXPos,nYPos);
}

//绘画图像
BOOL CImageEx::DrawImage( CDC * pDC, INT nXPos, INT nYPos, INT nDestWidth, INT nDestHeight )
{
	pDC->SetStretchBltMode(HALFTONE);
	return CImage::Draw(pDC->GetSafeHdc(),nXPos,nYPos,nDestWidth,nDestHeight);
}

//绘画图像
BOOL CImageEx::DrawImage( CDC * pDC, RECT &rc )
{
	pDC->SetStretchBltMode(HALFTONE);
	return CImage::Draw(pDC->GetSafeHdc(),rc);
}

//绘画图像
BOOL CImageEx::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc)
{
	pDC->SetStretchBltMode(HALFTONE);
	return CImage::Draw(pDC->GetSafeHdc(),nXDest,nYDest,nDestWidth,nDestHeight,nXScr,nYSrc,nDestWidth,nDestHeight);
}

//绘画图像
BOOL CImageEx::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT nYSrc, INT nSrcWidth, INT nSrcHeight)
{
	pDC->SetStretchBltMode(HALFTONE);
	if( nSrcWidth == 0 || nSrcHeight == 0 ) return FALSE;

	return CImage::Draw(pDC->GetSafeHdc(),nXDest,nYDest,nDestWidth,nDestHeight,nXScr,nYSrc,nSrcWidth,nSrcHeight);
}

bool CImageEx::Draw( CDC * pDC, INT x, INT y, INT cx, INT cy,INT nLeft,INT nTop,INT nRight,INT nBottom )
{
	int cxImage = GetWidth();
	int cyImage = GetHeight();

	// 左上
	{
		RECT rcDest = {x, y, x+nLeft, y+nTop};
		RECT rcSrc = {0, 0, nLeft, nTop};
		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 左边
	{
		RECT rcDest = {x, y+nTop, x+nLeft, (y+nTop)+(cy-nTop-nBottom)};
		RECT rcSrc = {0, nTop, nLeft, nTop+(cyImage-nTop-nBottom)};
		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 上边
	{
		RECT rcDest = {x+nLeft, y,x+(cx-nRight), y+nTop};
		RECT rcSrc = {nLeft, 0, (cxImage-nLeft-nRight), nTop};
 		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
 			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
 			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 右上
	{
		RECT rcDest = {x+(cx-nRight), y, (x+(cx-nRight))+nRight, y+nTop};
		RECT rcSrc = {cxImage-nRight, 0, (cxImage-nRight)+nRight, nTop};
  		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
  			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
  			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 右边
	{
		RECT rcDest = {x+(cx-nRight), y+nTop, (x+(cx-nRight))+nRight, (y+nTop)+(cy-nTop-nBottom)};
		RECT rcSrc = {cxImage-nRight, nTop, (cxImage-nRight)+nRight, nTop+(cyImage-nTop-nBottom)};
  		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
  			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
  			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 下边
	{
		RECT rcDest = {x+nLeft, y+(cy-nBottom), (x+nLeft)+(cx-nLeft-nRight), (y+(cy-nBottom))+nBottom};
		RECT rcSrc = {nLeft, cyImage-nBottom, nLeft+(cxImage-nLeft-nRight), (cyImage-nBottom)+nBottom};
 		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
 			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
 			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 右下
	{
		RECT rcDest = {x+(cx-nRight), y+(cy-nBottom), (x+(cx-nRight))+nRight, (y+(cy-nBottom))+nBottom};
		RECT rcSrc = {cxImage-nRight, cyImage-nBottom, (cxImage-nRight)+nRight, (cyImage-nBottom)+nBottom};
 		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
 			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
 			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 左下
	{
		RECT rcDest = {x, y+(cy-nBottom), x+nLeft, (y+(cy-nBottom))+nBottom};
		RECT rcSrc = {0, cyImage-nBottom, nLeft, (cyImage-nBottom)+nBottom};
		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	// 中间
	{
		RECT rcDest = {x+nLeft, y+nTop, x+(cx-nRight), y+(cy-nBottom)};
		RECT rcSrc = {nLeft, nTop, cxImage-nRight, cyImage-nBottom};

  		if (!::IsRectEmpty(&rcDest) && !::IsRectEmpty(&rcSrc))
  			DrawImage(pDC,rcDest.left, rcDest.top, rcDest.right-rcDest.left, rcDest.bottom-rcDest.top, 
  			rcSrc.left, rcSrc.top,rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top);
	}

	return true;
}

bool CImageEx::Draw( CDC * pDC, const RECT& rectDest, const RECT& rectSrc )
{
	return Draw(pDC,rectDest.left, rectDest.top, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, 
		rectSrc.left, rectSrc.top,rectSrc.right, rectSrc.bottom);
}

bool CImageEx::Draw( CDC * pDC, const RECT& rectDest )
{
	return Draw(pDC,rectDest,m_rcNinePart);
}

void CImageEx::SetNinePart( CONST LPRECT lprcNinePart )
{
	if( lprcNinePart == NULL ) return;

	::CopyRect(&m_rcNinePart,lprcNinePart);
}

HBITMAP CImageEx::ImageToBitmap() 
{ 
	return (HBITMAP)*this;
}

bool CImageEx::DrawExtrude( CDC*pDC,const RECT& rectDest,bool bLeft,int nPixel )
{
	//创建屏幕
	ASSERT(pDC!=NULL);

	int nWidth = rectDest.right - rectDest.left;
	int nHeight = rectDest.bottom - rectDest.top;

	if ( bLeft )
	{
		DrawImage(pDC,rectDest.left,rectDest.top);

		if(nWidth>GetWidth())
			DrawImage(pDC,GetWidth(),rectDest.top,(nWidth-GetWidth()),GetHeight(),GetWidth()-nPixel,0,nPixel,GetHeight());
	}
	else
	{
		if ( nWidth <=GetWidth() )
		{
			DrawImage(pDC,rectDest.left,rectDest.top,nWidth,GetHeight(),GetWidth()-nWidth,0,nWidth,GetHeight());
		}
		else
		{
			DrawImage(pDC,rectDest.left,rectDest.top,nWidth-GetWidth(),GetHeight(),nPixel,0,nPixel,GetHeight());
			DrawImage(pDC,nWidth-GetWidth(),rectDest.top);
		}
	}

	return true;
}

bool CImageEx::SetGray()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	BYTE* pArray = (BYTE*)GetBits();
	int nPitch = GetPitch();
	int nBitCount = GetBPP() / 8;

	for (int i = 0; i < nHeight; i++) 
	{
		for (int j = 0; j < nWidth; j++) 
		{
			int grayVal = (BYTE)(((*(pArray + nPitch * i + j * nBitCount) * 306)
				+ (*(pArray + nPitch * i + j * nBitCount + 1) * 601)
				+ (*(pArray + nPitch * i + j * nBitCount + 2) * 117) + 512 ) >> 10);	// 计算灰度值

			*(pArray + nPitch * i + j * nBitCount) = grayVal;							// 赋灰度值
			*(pArray + nPitch * i + j * nBitCount + 1) = grayVal;
			*(pArray + nPitch * i + j * nBitCount + 2) = grayVal;
		}
	}

	return true;
}

bool CImageEx::SetAlphaBit()
{
	ASSERT(IsNull() == false);
	if(IsNull())return FALSE;

	if ( GetBPP() == 32 )//png图像
	{
		LPVOID pBitsSrc = NULL;
		BYTE * psrc = NULL;
		BITMAP stBmpInfo;

		HBITMAP hBmp = (HBITMAP)*this;

		::GetObject(hBmp, sizeof(BITMAP), &stBmpInfo);

		if (32 != stBmpInfo.bmBitsPixel || NULL == stBmpInfo.bmBits)
			return FALSE;

		psrc = (BYTE *) stBmpInfo.bmBits;

		for (int nPosY = 0; nPosY < abs(stBmpInfo.bmHeight); nPosY++)
		{
			for (int nPosX = stBmpInfo.bmWidth; nPosX > 0; nPosX--)
			{
				BYTE alpha  = psrc[3];
				psrc[0] = (BYTE)((psrc[0] * alpha) / 255);
				psrc[1] = (BYTE)((psrc[1] * alpha) / 255);
				psrc[2] = (BYTE)((psrc[2] * alpha) / 255);
				psrc += 4;
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
