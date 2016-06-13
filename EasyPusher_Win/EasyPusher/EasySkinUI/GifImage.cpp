/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#include "Stdafx.h"
#include <OLECtl.h>
#include "GIFImage.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGIFImage::CGIFImage()
{
	//设置变量
	m_wImageWidth=0;
	m_wImageHeight=0;
	m_nCurrentFrame=0;
	m_crBackGroundColor=RGB(255,255,255);

	return;
}

//析构函数
CGIFImage::~CGIFImage()
{
	//释放图像
	DestroyImage();

	return;
}

//加载动画
bool CGIFImage::LoadFromFile(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//释放图像
	DestroyImage();

	//打开文件
	CFile GIFFile;
	if (GIFFile.Open(pszFileName,CFile::modeRead|CFile::shareDenyWrite)==FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建内存
	DWORD dwFileSize=(DWORD)GIFFile.GetLength();
	BYTE * pcbData=(BYTE *)GlobalAlloc(GMEM_FIXED,dwFileSize);

	//效验结果
	ASSERT(pcbData!=NULL);
	if (pcbData==NULL) return false;
	
	//读取文件
	GIFFile.Read(pcbData,dwFileSize);
	GIFFile.Close();

	//加载动画
	bool bSuccess=LoadFromMemroy(pcbData,dwFileSize);

	//释放内存
	GlobalFree(pcbData);

	return bSuccess;
}

//加载动画
bool CGIFImage::LoadFromMemroy(BYTE * pcbData, LONG lDataSize)
{
	//效验大小
	ASSERT(pcbData!=NULL);
	ASSERT(lDataSize>=sizeof(tagGIFHeader));
	if ((pcbData==NULL)||(lDataSize<sizeof(tagGIFHeader))) return false;

	//释放图像
	DestroyImage();

	//变量定义
	tagImageDataInfo ImageDataInfo;
	ZeroMemory(&ImageDataInfo,sizeof(ImageDataInfo));

	//设置变量
	ImageDataInfo.lDataSize=lDataSize;
	ImageDataInfo.cbImageDataBuffer=pcbData;

	//判断文件
	tagGIFHeader * pGIFHeader=(tagGIFHeader *)pcbData;
	if (memcmp(&pGIFHeader->cbSignature,"GIF",3)!= 0) return false;
	if ((memcmp(&pGIFHeader->cbVersion,"87a",3)!= 0)&&(memcmp(&pGIFHeader->cbVersion,"89a",3)!= 0)) return false;

	//逻辑屏幕
	tagGIFLSDescriptor * pGIFLSDescriptor=(tagGIFLSDescriptor *)(pcbData+sizeof(tagGIFHeader));
	if (GetPackedValue(pGIFLSDescriptor,LSD_PACKED_GLOBALCT)==1)
	{
		ImageDataInfo.lColorTableSize=3*(1<<(GetPackedValue(pGIFLSDescriptor,LSD_PACKED_GLOBALCTSIZE)+1));
		BYTE * pcbBkColor=pcbData+sizeof(tagGIFHeader)+sizeof(tagGIFLSDescriptor)+3*pGIFLSDescriptor->cbBkIndex;
		m_crBackGroundColor=RGB(pcbBkColor[0],pcbBkColor[1],pcbBkColor[2]);
	};

	//设置变量
	tagGIFFrameInfo GIFFrameInfo;
	enGIFBlockTypes GIFBlockTypes;
	m_GIFFrameInfoArray.RemoveAll();
	m_wImageWidth=pGIFLSDescriptor->wWidth;
	m_wImageHeight=pGIFLSDescriptor->wHeight;
	ImageDataInfo.lCurentPos=sizeof(tagGIFHeader)+sizeof(tagGIFLSDescriptor)+ImageDataInfo.lColorTableSize;

	//生成图帧
	do
	{
		//获取信息
		GIFBlockTypes=GetCurrentBlockType(ImageDataInfo);
		if ((GIFBlockTypes==BLOCK_UNKNOWN)||(GIFBlockTypes==BLOCK_TRAILER)) break;

		//变量定义
		LONG lStartPos=ImageDataInfo.lCurentPos;
		LONG lBlockSize=GetCurrentBlockSize(ImageDataInfo);

		//图形控制
		if (GIFBlockTypes==BLOCK_CONTROLEXT)
		{
			//变量定义
			tagGIFControlExt * pGIFControlExt=(tagGIFControlExt *)(pcbData+ImageDataInfo.lCurentPos);

			//获取信息
			GIFFrameInfo.nDelay=pGIFControlExt->wDelayTime;
			GIFFrameInfo.nDisposal=GetPackedValue(pGIFControlExt,GCX_PACKED_DISPOSAL);

			do
			{
				//切换区块
				if (SwitchNextBlock(ImageDataInfo)==false) return false;

				//获取类型
				GIFBlockTypes=GetCurrentBlockType(ImageDataInfo);
				if ((GIFBlockTypes==BLOCK_UNKNOWN)||(GIFBlockTypes==BLOCK_TRAILER)) return false;

				//类型判断
				lBlockSize+=GetCurrentBlockSize(ImageDataInfo);
				if ((GIFBlockTypes==BLOCK_IMAGE)||(GIFBlockTypes==BLOCK_PLAINTEXT)) break;

			} while (true);
		}
		else 
		{
			GIFFrameInfo.nDisposal=0;
			GIFFrameInfo.nDelay=0xFFFF;
		}

		//图形类型
		if (GIFBlockTypes==BLOCK_IMAGE)
		{
			//变量定义
			tagGIFImageDescriptor * pGIFImageDescriptor=(tagGIFImageDescriptor *)(pcbData+ImageDataInfo.lCurentPos);

			//获取信息
			GIFFrameInfo.wImageWidth=pGIFImageDescriptor->wWidth;
			GIFFrameInfo.wImageHeight=pGIFImageDescriptor->wHeight;
			GIFFrameInfo.wXExcursion=pGIFImageDescriptor->wXExcursion;
			GIFFrameInfo.wYExcursion=pGIFImageDescriptor->wYExcursion;

			//申请内存
			LONG lAllocSize=sizeof(tagGIFHeader)+sizeof(tagGIFLSDescriptor)+ImageDataInfo.lColorTableSize+lBlockSize+1;
			BYTE * pcbGlobal=(BYTE *)GlobalAlloc(GMEM_FIXED,lAllocSize);
			if (pcbGlobal==NULL) break;

			//拷贝内存
			LONG lOffset=0L;
			CopyMemory(pcbGlobal,pcbData,sizeof(tagGIFHeader)+sizeof(tagGIFLSDescriptor)+ImageDataInfo.lColorTableSize);
			lOffset+=sizeof(tagGIFHeader)+sizeof(tagGIFLSDescriptor)+ImageDataInfo.lColorTableSize;
			CopyMemory(pcbGlobal+lOffset,pcbData+lStartPos,lBlockSize);
			lOffset+=lBlockSize;
			pcbGlobal[lOffset++]=0x3B;

			//创建图像
			IStream * pIStream=NULL;
			if (CreateStreamOnHGlobal(pcbGlobal,TRUE,&pIStream)!=S_OK)
			{
				ASSERT(FALSE);
				GlobalFree(pcbGlobal);
				break;
			};

			//加载图片
			if (OleLoadPicture(pIStream,lOffset,FALSE,IID_IPicture,(VOID * *)&GIFFrameInfo.pIFramePicture)!=S_OK)
			{
				ASSERT(FALSE);
				SafeRelease(pIStream);
				break;
			};

			//释放资源
			SafeRelease(pIStream);

			//加入图帧
			m_GIFFrameInfoArray.Add(GIFFrameInfo);
		}

		//切换区块
		if (SwitchNextBlock(ImageDataInfo)==false) break;

	} while (true);

	return true;
}

//加载动画
bool CGIFImage::LoadFromResource(LPCTSTR pszResourceName, LPCTSTR pszResourceType)
{
	//效验参数
	ASSERT(pszResourceName!=NULL);
	ASSERT(pszResourceType!=NULL);
	if ((pszResourceName==NULL)||(pszResourceType==NULL)) return false;

	//释放图像
	DestroyImage();

	//获取资源
	HRSRC hPicture=FindResource(AfxGetResourceHandle(),pszResourceName,pszResourceType);
	if (hPicture==NULL) return false;
	HGLOBAL hResData=LoadResource(AfxGetResourceHandle(),hPicture);
	if (hResData=NULL) return false;

	//创建内存
	DWORD dwResourceSize=SizeofResource(AfxGetResourceHandle(),hPicture);
	BYTE * pcbData=(BYTE *)GlobalAlloc(GMEM_FIXED,dwResourceSize);
	if (pcbData==NULL) 
	{
		FreeResource(hResData);
		return false;
	}

	//锁定资源
	BYTE * pcbSrc=(BYTE *)LockResource(hResData);
	if (pcbSrc==NULL) 
	{
		GlobalFree(pcbData);
		FreeResource(hResData);
		return false;
	}

	//拷贝数据
	CopyMemory(pcbData,pcbSrc,dwResourceSize);
	FreeResource(hResData);

	//加载动画
	bool bSuccess=LoadFromMemroy(pcbData,dwResourceSize);

	//释放内存
	GlobalFree(pcbData);

	return bSuccess;
}

//是否加载
bool CGIFImage::IsNull()
{
	return (m_GIFFrameInfoArray.GetCount()==0);
}

//释放图像
VOID CGIFImage::DestroyImage()
{
	//设置变量
	m_wImageWidth=0;
	m_wImageHeight=0;
	m_nCurrentFrame=0;
	m_crBackGroundColor=RGB(255,255,255);

	//释放图帧
	if (m_GIFFrameInfoArray.GetCount()>0)
	{
		for (INT_PTR i=0;i<m_GIFFrameInfoArray.GetCount();i++)
		{
			//获取对象
			tagGIFFrameInfo * pGIFFrameInfo=&m_GIFFrameInfoArray[i];

			//释放接口
			SafeRelease(pGIFFrameInfo->pIFramePicture);
		}

		//删除数组
		m_GIFFrameInfoArray.RemoveAll();
	}

	return;
}

//当前图像
tagGIFFrameInfo * CGIFImage::GetCurrentFrame()
{
	//状态效验
	ASSERT(IsNull()==false);
	if (IsNull()==true) return NULL;

	return &m_GIFFrameInfoArray[m_nCurrentFrame];
}

//绘画图像
VOID CGIFImage::DrawImageFrame(CDC * pDC, INT nXPos, INT nYPos)
{
	//绘画图片
	DrawImageFrame(pDC,nXPos,nYPos,m_nCurrentFrame);

	//调整图帧
	m_nCurrentFrame=((m_nCurrentFrame+1)==m_GIFFrameInfoArray.GetCount())?0L:(m_nCurrentFrame+1L);

	return;
}

//绘画图像
VOID CGIFImage::DrawImageFrame(CDC * pDC, INT nXPos, INT nYPos, INT_PTR nFrameIndex)
{
	//获取数据
	ASSERT(nFrameIndex<m_GIFFrameInfoArray.GetCount());
	tagGIFFrameInfo * pGIFFrameInfo=&m_GIFFrameInfoArray[nFrameIndex];

	//获取大小
	OLE_XSIZE_HIMETRIC hmWidth=0L;
	OLE_YSIZE_HIMETRIC hmHeight=0L;
	pGIFFrameInfo->pIFramePicture->get_Width(&hmWidth);
	pGIFFrameInfo->pIFramePicture->get_Height(&hmHeight);

	//绘画图片
	pGIFFrameInfo->pIFramePicture->Render(pDC->m_hDC,nXPos+pGIFFrameInfo->wXExcursion,nYPos+pGIFFrameInfo->wYExcursion,
		pGIFFrameInfo->wImageWidth,pGIFFrameInfo->wImageHeight,0,hmHeight,hmWidth,-hmHeight,NULL);

	return;
}

//区块长度
LONG CGIFImage::GetCurrentBlockSize(tagImageDataInfo & ImageDataInfo)
{
	enGIFBlockTypes GIFBlockTypes=GetCurrentBlockType(ImageDataInfo);
	switch (GIFBlockTypes)
	{
	case BLOCK_UNKNOWN: { return -1; }
	case BLOCK_TRAILER: { return 1; }
	case BLOCK_APPEXT:
		{
			LONG lSubSize=GetSubBlockSize(ImageDataInfo,ImageDataInfo.lCurentPos+sizeof(tagGIFAppExtension));
			return sizeof(tagGIFAppExtension)+lSubSize;
		}
	case BLOCK_COMMEXT:
		{
			LONG lSubSize=GetSubBlockSize(ImageDataInfo,ImageDataInfo.lCurentPos+sizeof(tagGIFCommentExt));
			return sizeof(tagGIFCommentExt)+lSubSize;
		}
	case BLOCK_CONTROLEXT:
		{
			return sizeof(tagGIFControlExt);
		}
	case BLOCK_PLAINTEXT:
		{
			LONG lSubSize=GetSubBlockSize(ImageDataInfo,ImageDataInfo.lCurentPos+sizeof(tagGIFPlainTextExt));
			return sizeof(tagGIFPlainTextExt)+lSubSize;
		}
	case BLOCK_IMAGE:
		{
			INT nLCTSize=0;
			tagGIFImageDescriptor * pGIFImageDescriptor=(tagGIFImageDescriptor *)(ImageDataInfo.cbImageDataBuffer+ImageDataInfo.lCurentPos);
			if (GetPackedValue(pGIFImageDescriptor,ID_PACKED_LOCALCT)==1)
			{
				nLCTSize=(1<<(GetPackedValue(pGIFImageDescriptor,ID_PACKED_LOCALCTSIZE)+1))*3;
			}
			LONG lSubSize=GetSubBlockSize(ImageDataInfo,ImageDataInfo.lCurentPos+sizeof(tagGIFImageDescriptor)+nLCTSize+1);
			return sizeof(tagGIFImageDescriptor)+nLCTSize+lSubSize+1;
		}
	};

	return 0;
}

//获取类型
enGIFBlockTypes CGIFImage::GetCurrentBlockType(tagImageDataInfo & ImageDataInfo)
{
	switch (ImageDataInfo.cbImageDataBuffer[ImageDataInfo.lCurentPos])
	{
	case 0x21:
		{
			switch (ImageDataInfo.cbImageDataBuffer[ImageDataInfo.lCurentPos+1])
			{
			case 0x01: { return BLOCK_PLAINTEXT; }
			case 0xF9: { return BLOCK_CONTROLEXT; }
			case 0xFE: { return BLOCK_COMMEXT; }
			case 0xFF: { return BLOCK_APPEXT; }
			};
			break;
		}
	case 0x3B: { return BLOCK_TRAILER; }
	case 0x2C: { return BLOCK_IMAGE; }
	}

	return BLOCK_UNKNOWN;
}

//区块长度
LONG CGIFImage::GetSubBlockSize(tagImageDataInfo & ImageDataInfo,LONG lBlockPos)
{
	LONG lBlockLength=0;
	while (ImageDataInfo.cbImageDataBuffer[lBlockPos]!=0)
	{
		lBlockLength+=ImageDataInfo.cbImageDataBuffer[lBlockPos]+1;
		lBlockPos+=ImageDataInfo.cbImageDataBuffer[lBlockPos]+1;
	};
	return lBlockLength+1;
}

//切换区块
bool CGIFImage::SwitchNextBlock(tagImageDataInfo & ImageDataInfo)
{
	LONG lBlockSize=GetCurrentBlockSize(ImageDataInfo);
	if ((lBlockSize<=0)||((ImageDataInfo.lCurentPos+lBlockSize)>ImageDataInfo.lDataSize)) return false;
	ImageDataInfo.lCurentPos+=lBlockSize;
	return true;
}

//获取数值
INT CGIFImage::GetPackedValue(const tagGIFControlExt * pGIFControlExt, enum enControlExtValues Value)
{
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:		//处置方法
		{
			return (pGIFControlExt->cbPacked&28)>>2;
		}
	case GCX_PACKED_USERINPUT:		//输入标志
		{
			return (pGIFControlExt->cbPacked&2)>>1;
		}
	case GCX_PACKED_TRANSPCOLOR:	//透明标志
		{
			return pGIFControlExt->cbPacked&1;
		}
	};

	ASSERT(FALSE);

	return 0;
}

//获取数值
INT CGIFImage::GetPackedValue(const tagGIFImageDescriptor * pGIFImageDescriptor, enum enIDPackedValues Value)
{
	switch (Value)
	{
	case ID_PACKED_LOCALCT:		//颜色标志
		{
			return pGIFImageDescriptor->cbPacked>>7;
		}
	case ID_PACKED_INTERLACE:	//交织标志
		{
			return (pGIFImageDescriptor->cbPacked&0x40)>>6;
		}
	case ID_PACKED_SORT:		//分类标志
		{
			return (pGIFImageDescriptor->cbPacked&0x20)>>5;
		}
	case ID_PACKED_LOCALCTSIZE:	//颜色大小
		{
			return pGIFImageDescriptor->cbPacked&7;
		}
	};

	ASSERT(FALSE);

	return 0;
}

//获取数值
INT CGIFImage::GetPackedValue(const tagGIFLSDescriptor * pGIFLSDescriptor, enum enLSDPackedValues Value)
{
	switch (Value)
	{
	case LSD_PACKED_GLOBALCT:		//颜色标志
		{
			return pGIFLSDescriptor->cbPacked>>7;
		}
	case LSD_PACKED_CRESOLUTION:	//颜色深度
		{
			return ((pGIFLSDescriptor->cbPacked&0x70)>>4)+1;
		}
	case LSD_PACKED_SORT:			//分类标志
		{
			return (pGIFLSDescriptor->cbPacked&8)>>3;
		}
	case LSD_PACKED_GLOBALCTSIZE:	//全局颜色
		{
			return pGIFLSDescriptor->cbPacked & 0x07;
		}
	};

	ASSERT(FALSE);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
