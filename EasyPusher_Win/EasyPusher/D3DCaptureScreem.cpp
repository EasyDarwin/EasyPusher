#include "StdAfx.h"
#include "D3DCaptureScreem.h"
//#include <d3d.h>

typedef struct tagScreenCapDataInfo
{
	int nWidth;
	int nHeight;
	int nDataType;
}ScreenCapDataInfo;

CD3DCaptureScreem::CD3DCaptureScreem(void)
{
	m_pD3DScreenCapture=NULL;
	m_pd3dDevice=NULL;
	m_pSurface=NULL;
	m_pScreenCaptureData=NULL;
	memset(&m_ScreenRect, 0x00, sizeof(RECT)) ;
	m_hMainWnd = NULL;
	m_nCapWidth = 0;
	m_nCapHeight = 0;
	m_bCaptureScreen = FALSE;
	m_pCallback = NULL;
	m_pMaster = NULL;
}


CD3DCaptureScreem::~CD3DCaptureScreem(void)
{
}

HRESULT   CD3DCaptureScreem:: InitD3DCapture(HWND hShowWnd)
{
	m_hMainWnd  = hShowWnd;

	BITMAPINFO    bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biBitCount=32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biWidth=GetSystemMetrics(SM_CXSCREEN);
	bmpInfo.bmiHeader.biHeight=GetSystemMetrics(SM_CYSCREEN);
	bmpInfo.bmiHeader.biPlanes=1;
	bmpInfo.bmiHeader.biSizeImage=abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biBitCount/8;

	HDC    hdc=GetDC(GetDesktopWindow());
	HDC        hBackDC=NULL;
	HBITMAP    hBackBitmap=NULL;
	hBackDC=CreateCompatibleDC(hdc);
	hBackBitmap=CreateDIBSection(hdc,&bmpInfo,DIB_RGB_COLORS,&m_pScreenCaptureData,NULL,0);
	if(hBackBitmap==NULL)
	{
		return 0 ;
	}
	ReleaseDC(GetDesktopWindow(),hdc);

	HWND hWnd = hShowWnd;
	D3DDISPLAYMODE    ddm;
	D3DPRESENT_PARAMETERS    d3dpp;

	if((m_pD3DScreenCapture=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		return E_FAIL;
	}

	if(FAILED(m_pD3DScreenCapture->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
	{
		return E_FAIL;
	}

	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.Windowed=true;
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat=ddm.Format;
	d3dpp.BackBufferHeight=m_ScreenRect.bottom =ddm.Height;
	d3dpp.BackBufferWidth=m_ScreenRect.right =ddm.Width;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hWnd;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

	if(FAILED(m_pD3DScreenCapture->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING ,&d3dpp,&m_pd3dDevice)))
	{
		return E_FAIL;
	}
	m_nCapWidth = ddm.Width;
	m_nCapHeight = ddm.Height;
	if(FAILED(m_pd3dDevice->CreateOffscreenPlainSurface(m_nCapWidth, m_nCapHeight, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &m_pSurface, NULL)))
	{
		return E_FAIL;
	}
	CreateCaptureScreenThread();
	return S_OK;
}

void CD3DCaptureScreem::StopD3DScreenCapture()
{
	m_bCaptureScreen = 0;
	//等待线程结束
	//Sleep(300);
	//获取线程结束代码 ,如果线程还在运行就等她结束
	while (1)
	{
		DWORD dwExitCode ;
		::GetExitCodeThread(m_hScreenCaptureThread,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
		{
			WaitForSingleObject(m_hScreenCaptureThread, 100); 
		}
		else
		{
			break;
		}
	}
	CloseHandle(m_hScreenCaptureThread);
	m_hScreenCaptureThread = INVALID_HANDLE_VALUE;

	m_pSurface->Release();
	m_pd3dDevice->Release();
	m_pD3DScreenCapture->Release();

}

//direct实现的截图
void CD3DCaptureScreem::DirectScreenCapture(LPVOID screenCaptureData)
{
	m_pd3dDevice->GetFrontBufferData(0, m_pSurface);
	D3DLOCKED_RECT    lockedRect;
	if(FAILED(m_pSurface->LockRect(&lockedRect,&m_ScreenRect,D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY)))
	{
		return;
	}
// 	for(int i=0;i<m_ScreenRect.bottom;i++)
// 	{
// 	}
	memcpy((BYTE*)screenCaptureData/*+(i)*m_ScreenRect.right*32/8*/,    (BYTE*)lockedRect.pBits/*+i*lockedRect.Pitch*/,    m_ScreenRect.bottom*m_ScreenRect.right<<2);
	m_pSurface->UnlockRect();
	//printf("%d\n" , l3);
}

//创建线程进行屏幕捕获
int CD3DCaptureScreem::CreateCaptureScreenThread()
{
	//创建线程
	m_bCaptureScreen = TRUE;
	m_hScreenCaptureThread=(HANDLE)_beginthreadex(NULL,0,(&CD3DCaptureScreem::CaptureScreenThread),
		this,THREAD_PRIORITY_NORMAL,NULL);
	return 1;
}
UINT WINAPI CD3DCaptureScreem::CaptureScreenThread(LPVOID pParam)
{
	if (pParam)
	{
		CD3DCaptureScreem* pMaster = (CD3DCaptureScreem*)pParam;
		if (pMaster)
		{
			pMaster->CaptureVideoProcess();
		}
	}
	return 0;
}

void CD3DCaptureScreem::CaptureVideoProcess()
{
	unsigned char * screenData = new unsigned char[1920*1080*4];
	unsigned char * screenData24 = new unsigned char[1920*1080*3];

	while (m_bCaptureScreen)
	{
		DirectScreenCapture(screenData);
		Convert24Image(screenData, screenData24, 1920*1080*4);
#if 0
		//显示图像
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
		bmi.bmiHeader.biWidth       = 1920;
		bmi.bmiHeader.biHeight      = -1080; // top-down image 
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 32;//m_nColorBits;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage   = 0;

		unsigned char * pData = (unsigned char *)screenData ;
		::SetDIBitsToDevice( hDC, 0, 0, /*rect.Width()*/640, /*rect.Height()*/480, 0, 0, 0, 1080, (LPBYTE)pData , &bmi, DIB_RGB_COLORS );

#endif
		if (m_pCallback&&m_pMaster)
		{
			ScreenCapDataInfo sCapScreenInfo;
			sCapScreenInfo.nWidth = m_nCapWidth;
			sCapScreenInfo.nHeight = m_nCapHeight;
			sCapScreenInfo.nDataType = 24;
			m_pCallback(0, (unsigned char*)(screenData24), /*alpbi->biSizeImage*/m_nCapWidth*m_nCapHeight*3, 1, &sCapScreenInfo, m_pMaster);

		} 
		//Sleep(30);
	}

	if (screenData)
	{
		delete screenData;
		screenData = NULL;
	}
	if(screenData24)
	{
		delete screenData24;
		screenData24 = NULL;
	}
}

bool CD3DCaptureScreem::Convert24Image(BYTE *p32Img, BYTE *p24Img,DWORD dwSize32)  
{  
    if(p32Img != NULL && p24Img != NULL && dwSize32>0)  
    {  
  
        DWORD dwSize24;  
  
        dwSize24=(dwSize32 * 3)/4;  
  
        BYTE *pTemp,*ptr;  
  
        pTemp=p32Img;  
        ptr = p24Img;  
  
        int ival=0;  
        for (DWORD index = 0; index < dwSize32/4 ; index++)  
        {                                     
            unsigned char r = *(pTemp++);  
            unsigned char g = *(pTemp++);  
            unsigned char b = *(pTemp++);  
            (pTemp++);//skip alpha  
  
            *(ptr++) = r;  
            *(ptr++) = g;  
            *(ptr++) = b;  
        }     
    }  
    else  
    {  
        return false;  
    }  
  
    return true;  
}  

//设置捕获数据回调函数
void CD3DCaptureScreem::SetCaptureScreenCallback(CaptureScreenCallback callBack, void * pMaster)
{
	m_pCallback = callBack;
	m_pMaster = pMaster;
}

void CD3DCaptureScreem::GetCaptureScreenSize(int& nWidth, int& nHeight )
{
	nWidth = m_nCapWidth;
	nHeight = m_nCapHeight;
}

// void dump_buffer()
// {
// 	IDirect3DSurface9* pRenderTarget=NULL;
// 	IDirect3DSurface9* pDestTarget=NULL;
// 	const char file[] = "Pickture.bmp";
// 	// sanity checks.
// 	if (Device == NULL)
// 		return;
// 	// get the render target surface.
// 	HRESULT hr = Device->GetRenderTarget(0, &pRenderTarget);
// 	// get the current adapter display mode.
// 	//hr = pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddisplaymode);
// 	// create a destination surface.
// 	hr = Device->CreateOffscreenPlainSurface(DisplayMde.Width,
// 		DisplayMde.Height,
// 		DisplayMde.Format,
// 		D3DPOOL_SYSTEMMEM,
// 		&pDestTarget,
// 		NULL);
// 	//copy the render target to the destination surface.
// 	hr = Device->GetRenderTargetData(pRenderTarget, pDestTarget);
// 	//save its contents to a bitmap file.
// 	hr = D3DXSaveSurfaceToFile(file,
// 		D3DXIFF_BMP,
// 		pDestTarget,
// 		NULL,
// 		NULL);
// 	// clean up.
// 	pRenderTarget->Release();
// 	pDestTarget->Release();
// }