
#pragma once

#include <d3d9.h>
#include <WinError.h>

typedef struct IDirect3D9* LPDIRECT3D9, *PDIRECT3D9;
typedef struct IDirect3DDevice9* LPDIRECT3DDevice9, *PDIRECT3DDevice9;
typedef struct IDirect3DSurface9* LPDIRECT3DSurface9, *PDIRECT3DSurface9;



typedef int (WINAPI *CaptureScreenCallback)(int nDevId, unsigned char *pBuffer, int nBufSize, int nRealDatatYPE, void* realDataInfo, void* pMaster);

	class CD3DCaptureScreem
	{
	public:
		CD3DCaptureScreem(void);
		~CD3DCaptureScreem(void);

		//接口函数
		// 初始化
		HRESULT    InitD3DCapture(HWND hShowWnd);
		//direct实现的截图
		void DirectScreenCapture(LPVOID screenCaptureData);

		//创建线程进行屏幕捕获
		int CreateCaptureScreenThread();
		static UINT WINAPI CaptureScreenThread(LPVOID pParam);
		void CaptureVideoProcess();
		//设置捕获数据回调函数
		void SetCaptureScreenCallback(CaptureScreenCallback callBack, void * pMaster);

		BOOL IsInCapturing()
		{
			return m_bCaptureScreen;
		}
		void GetCaptureScreenSize(int& nWidth, int& nHeight );
		bool Convert24Image(BYTE *p32Img, BYTE *p24Img,DWORD dwSize32);
		void StopD3DScreenCapture();

	private:
		HWND m_hMainWnd;
		/*IDirect3D9**/  LPDIRECT3D9            m_pD3DScreenCapture;
		/*IDirect3DDevice9* */LPDIRECT3DDevice9   m_pd3dDevice;
		/*IDirect3DSurface9**/ LPDIRECT3DSurface9  m_pSurface;
		CaptureScreenCallback m_pCallback;
		void* m_pMaster;

		int m_nCapWidth;
		int m_nCapHeight;
		LPVOID					m_pScreenCaptureData;
		RECT							m_ScreenRect;
		HANDLE m_hScreenCaptureThread;
		BOOL m_bCaptureScreen;
	};

