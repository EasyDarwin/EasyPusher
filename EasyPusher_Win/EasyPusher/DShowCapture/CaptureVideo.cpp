/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CaptureVideo.h"
#include "SourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <initGuid.h>
DEFINE_GUID(CLSID_DeinterlaceFilter,
			0x45FA9781,0xE904,0x11D6,0xA2,0xFE,0x00,0x80,0xAD,0x0B,0x2E,0xA7);
DEFINE_GUID(MEDIASUBTYPE_I420, 0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
CCaptureVideo::CCaptureVideo()
{
/*	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
//	{
//		AfxMessageBox("CoInitialize Failed!\r\n"); 
//		return ;
//	}
//	*/
	m_pGraphBuilder = NULL;

	m_pCaptureGraphBulid = NULL;
	m_pBaseFilter = NULL;
	m_pVideoWin = NULL; 
	m_pMediaCon = NULL;
	m_pMediaEvent=NULL;
	m_pSampleGrabberFilter = NULL;
	m_pSampleGrabber =NULL;
	m_pDataBuffer = NULL;
	m_nBufferSize = 0;

	m_pColorBuffer=NULL;
	m_pVideoDeinterlaceFilter=NULL;

	m_iDeviceId = -1;
	m_hWnd = 0;

	m_nFrameRate = 20;
	m_iWidth = 320;
	m_iHeight = 240;
	m_strRGBByte="RGB24";

	m_pWindowRender=NULL;
	m_nRenderType=0;
	m_nIndex=0;
	m_nDataType = 0;
	m_nPinType=1;
	m_nDeinterlace=0;
	m_nError=-2;

	m_hThreadEvent=NULL;
	m_bThread=FALSE;	
	m_bThreadExit = FALSE;
}

CCaptureVideo::~CCaptureVideo()
{
	if(m_bThread==TRUE)
	{
		m_bThreadExit = TRUE;
		if(m_hThreadEvent)
		{	
			Sleep(100);
			CloseHandle(m_hThreadEvent);
		}
	}

	if(m_pMediaCon)
	{
		m_pMediaCon->Stop();
		m_pMediaCon->Release();
		m_pMediaCon = NULL;
	}
	if(m_pVideoWin)
	{
		m_pVideoWin->put_Visible(OAFALSE);
		m_pVideoWin->put_Owner(NULL);
		m_pVideoWin->Release();
		m_pVideoWin = NULL;
	}
	TearDownGraph();
	SAFE_RELEASE(m_pGraphBuilder);
	SAFE_RELEASE(m_pCaptureGraphBulid);
	SAFE_RELEASE(m_pSampleGrabberFilter);
	SAFE_RELEASE(m_pSampleGrabber);
	SAFE_RELEASE(m_pWindowRender);
	SAFE_RELEASE(m_pMediaEvent);
	SAFE_RELEASE(m_pVideoDeinterlaceFilter);

	if(m_pDataBuffer)
	{
		delete []m_pDataBuffer;
		m_pDataBuffer=NULL;
	}
	if(m_pColorBuffer)
	{
		delete []m_pColorBuffer;
		m_pColorBuffer=NULL;
	}


//	CoUninitialize();
}
// void CCaptureVideo::ERR_DEBUG(CString strError)
// {
// 	AfxMessageBox(strError);
// }

//创建视频捕获Graph
HRESULT CCaptureVideo::CreateCaptureGraphBuilder()
{
	HRESULT hr=NOERROR;
	
	if(m_pGraphBuilder==NULL)
	{
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraphBuilder);
		if(FAILED(hr))
		{
			//ERR_DEBUG("CreateCaptureGraphBuilder Create m_pGraphBuilder Failed");
			return hr;
		}	
	}
	if(m_pCaptureGraphBulid==NULL)
	{
		//// 创建ICaptureGraphBuilder2接口，即创建视频捕获窗
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
							IID_ICaptureGraphBuilder2, (void **) &m_pCaptureGraphBulid);
		if (FAILED(hr))		
		{
			//ERR_DEBUG("CreateCaptureGraphBuilder CaptureGraphBuilder2 Failed");
			return hr;
		}	
		//将捕获窗的视频属性设为定义好的视频窗
		//给captureGrap builder指定一个图象filter,不能由于混合视频的呈现，video端口的管理
		hr = m_pCaptureGraphBulid->SetFiltergraph(m_pGraphBuilder);
	}

	//此处可能存在问题是否为一个链路就一个m_pMediaCon媒体控制//QueryInterface
	if(m_pMediaCon==NULL)
	{
		hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaCon);
		if (FAILED(hr))
		{
			//ERR_DEBUG("CreateCaptureGraphBuilder  QueryInterface m_pMediaCon Failed");
			return hr;
		}
	}

	if(m_pMediaEvent==NULL)
	{
		hr = m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (void **) &m_pMediaEvent);
		if (FAILED(hr))
		{
			//ERR_DEBUG("CreateCaptureGraphBuilder  QueryInterface m_pMediaEvent Failed");
			return hr;
		}
	}

	return hr;
}

//枚举设备并连接设备
BOOL CCaptureVideo::BindToVideoDev(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
	{
		return FALSE;
	}
	
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
									IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		//ERR_DEBUG("Instance DeviceEnum Failed");
		return FALSE;
	}
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR)
	{
		//ERR_DEBUG("Enum VideoInputDeviceCategory Failed");
		return FALSE;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM=NULL;

	int index = 0;
	while((( pEm->Next(1, &pM, &cFetched))==S_OK)&&( index <= deviceId))
	{
		IPropertyBag *pBag=NULL;
		if (pM==NULL)
		{
			break;
		}
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(pBag!=NULL) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				if (index == deviceId)
				{
					//将视频设备绑定到基础过滤器上
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
				pBag->Release();
			}
		
		}
		pM->Release();
		index++;
	}
	return TRUE;
}

/*
函数：InitVideoSize
功能：初始化设置视频格式，大小
参数说明：iWidth：宽度
		   iHeight：高度
		  nRGBByte：颜色的BYTES数，分为24和32,MEDIASUBTYPE_RGB32,MEDIASUBTYPE_RGB24
		  nPrewVideo：1--PIN_CATEGORY_CAPTURE，2--PIN_CATEGORY_PREVIEW
*/
HRESULT CCaptureVideo::SetVideoSize(int nPreview,CString strRGBBytes,int nFrameRate,int iWidth , int iHeight)
{
	HRESULT hr=E_FAIL;
	if(m_pCaptureGraphBulid==NULL)
		return hr;

	IAMStreamConfig *pAMStreamConfig=NULL;
	if(nPreview==0)
	{
		hr = m_pCaptureGraphBulid->FindInterface(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,
				m_pBaseFilter,IID_IAMStreamConfig,(void **)&pAMStreamConfig);
	}
	else
	{
		hr = m_pCaptureGraphBulid->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,
				m_pBaseFilter,IID_IAMStreamConfig,(void **)&pAMStreamConfig);
	}

	if(FAILED( hr ))
	{
		SAFE_RELEASE(pAMStreamConfig);
		return hr;
	}
		//得到视频格式大小
	AM_MEDIA_TYPE *pmt;
	pAMStreamConfig->GetFormat(&pmt);

	//设置视频格式
	pmt->majortype = MEDIATYPE_Video;

	GUID defaultSubType = pmt->subtype;
	pmt->subtype  = GetMediaTypeGuid(strRGBBytes);


	VIDEOINFOHEADER *pvih = reinterpret_cast<VIDEOINFOHEADER *>(pmt->pbFormat);
	//设置回去
	int nDefualWidth = pvih->bmiHeader.biWidth;
	int nDefualHeight = pvih->bmiHeader.biHeight;

	pvih->bmiHeader.biWidth = iWidth; 
	pvih->bmiHeader.biHeight = iHeight;
 	pvih->bmiHeader.biSizeImage = pmt->lSampleSize = iWidth*iHeight*pvih->bmiHeader.biPlanes*pvih->bmiHeader.biBitCount/8;
 	pvih->AvgTimePerFrame = (LONGLONG)(10000000/nFrameRate);

	hr = pAMStreamConfig->SetFormat(pmt);
	if(FAILED(hr))
	{
		//如果设置失败可以选用默认的,但运用之后,小屏幕初始化时会出现闪动的情况		
// 		pvih->bmiHeader.biWidth = nDefualWidth; 
// 		pvih->bmiHeader.biHeight = nDefualHeight;
// 		pvih->bmiHeader.biSizeImage = pmt->lSampleSize = nDefualWidth*nDefualHeight*pvih->bmiHeader.biPlanes*pvih->bmiHeader.biBitCount/8;
		
		pmt->subtype  = defaultSubType;

		hr = pAMStreamConfig->SetFormat(pmt);

		CSourceManager* pManager = CSourceManager::Instance();
		if (pManager)
		{
			pManager->LogErr(_T("使用默认参数。"));
		}
		//AfxMessageBox(_T("使用默认参数。"));

		if(FAILED(hr))
		{
			SAFE_RELEASE(pAMStreamConfig);
			FreeMediaType(*pmt);
			//ERR_DEBUG("初始化设置视频格式失败");
			return hr;
		}
	}
	SAFE_RELEASE(pAMStreamConfig);
	FreeMediaType(*pmt);//
	return hr;
}

//设置窗口句柄，并自动更新显示大小
HRESULT CCaptureVideo::SetupVideoWindow(HWND hWnd)
{
	HRESULT hr=NOERROR;

	if(m_pVideoWin==NULL)
	{
		return hr;
	}

	if(hWnd!=NULL)
		m_hWnd = hWnd;

	hr = m_pVideoWin->put_Owner((OAHWND)m_hWnd);
//	hr = m_pVideoWin->put_MessageDrain((OAHWND)m_hWnd);
	if (FAILED(hr))
	{
		//ERR_DEBUG("SetupVideoWindow put_Owner Error");
		return hr;
	}
	hr = m_pVideoWin->put_WindowStyle(WS_CHILD |SS_NOTIFY| WS_CLIPCHILDREN);
	if (FAILED(hr))
	{
		//ERR_DEBUG("SetupVideoWindow put_WindowStyle");
		return hr;
	}

	ResizeVideoWindow();
	hr = m_pVideoWin->put_Visible(OATRUE);

	return hr;
}
void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) 
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

//调整窗口大小
void CCaptureVideo::ResizeVideoWindow()
{
	if (m_pVideoWin)
	{
		CRect rc;
		::GetClientRect(m_hWnd,&rc);
		TRACE("%d %d %d %d %d \n",m_hWnd,rc.left,rc.top,rc.right,rc.bottom);
		m_pVideoWin->SetWindowPosition(0, 0, rc.right, rc.bottom);
	} 
}

HRESULT CCaptureVideo::CreateCaptureSampleGrabber(CString strRGBBytes)
{
	HRESULT hr=NOERROR;
	if(m_pSampleGrabberFilter==NULL)
	{
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
			(LPVOID *)&m_pSampleGrabberFilter);
		if(FAILED(hr))
		{
			SAFE_RELEASE(m_pSampleGrabberFilter);
			//ERR_DEBUG("CreateCaptureGraphBuilder  QueryInterface m_pSampleGrabberFilter Failed");
			return hr;
		}
	}
	if(m_pSampleGrabber==NULL)
	{
		hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pSampleGrabber);
		if(FAILED(hr))
		{
			SAFE_RELEASE(m_pSampleGrabberFilter);
			SAFE_RELEASE(m_pSampleGrabber);
			//ERR_DEBUG("CreateCaptureGraphBuilder  QueryInterface m_pSampleGrabber Failed");
			return hr;
		}
		AM_MEDIA_TYPE   mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		
		mt.subtype = GetMediaTypeGuid(strRGBBytes);
		
		mt.formattype = FORMAT_VideoInfo; 
		hr = m_pSampleGrabber->SetMediaType(&mt);
		
		hr = m_pGraphBuilder->AddFilter(m_pSampleGrabberFilter, L"Grabber");
	}
	return hr;
}

int  CCaptureVideo::CreateVideoFilter(int iDeviceID,HWND hWnd,int nFrameRate
									  ,int iWidth,int iHeight,CString  strRGBByte)
{

	//创建Capture
	int bFlag = 1;
	if(FAILED(CreateCaptureGraphBuilder()))
	{
		bFlag=-1;
		return bFlag;
	}

	HRESULT hr=NOERROR;
	hr = BindToVideoDev(iDeviceID,&m_pBaseFilter);
	if(FAILED(hr))
	{
		//ERR_DEBUG("BindVideoFilter Failed");
		return -1;
	}
	hr = m_pGraphBuilder->AddFilter(m_pBaseFilter, L"Capture");

	m_iDeviceId = iDeviceID;
	m_hWnd = hWnd;

	m_nFrameRate = m_nFrameRate;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_strRGBByte=strRGBByte;
	return bFlag;
	
}

HRESULT CCaptureVideo::CreateVideoRender(int nType)
{
	HRESULT hr = NOERROR;
	if(m_pWindowRender)
	{
		SAFE_RELEASE(m_pWindowRender);
	}
	//创建解码器filter,CLSID_VideoRendererDefault,//
	if(nType==0)
	{
		SAFE_RELEASE(m_pWindowRender);
		return NOERROR;
	}
	else if(nType==1)
	{
		
		hr = CoCreateInstance(CLSID_VideoRendererDefault,0,CLSCTX_INPROC_SERVER,
			IID_IBaseFilter,(void **)&m_pWindowRender);
		
	}
	else if(nType==2)
	{
		hr = CoCreateInstance(CLSID_VideoRenderer,0,CLSCTX_INPROC_SERVER,
			IID_IBaseFilter,(void **)&m_pWindowRender);
		
	}
	else if(nType==3)//不显示
	{
		hr = CoCreateInstance(CLSID_NullRenderer,0,CLSCTX_INPROC_SERVER,
			IID_IBaseFilter,(void **)&m_pWindowRender);
	}
	else
	{
		SAFE_RELEASE(m_pWindowRender);
		return NOERROR;
	}	
	
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pWindowRender);
		//ERR_DEBUG("接收创建呈现器失败");
		return hr; 
	}
	if(m_pGraphBuilder)
	{
		hr = m_pGraphBuilder->AddFilter(m_pWindowRender,L"recv render");
		if(FAILED(hr))
		{
			SAFE_RELEASE(m_pWindowRender);
			//ERR_DEBUG("加入接收创建呈现器失败");
			return hr; 
		}
	}
	return hr;
}

HRESULT CCaptureVideo::CreateDeinterlaceFilter()
{
	HRESULT hr =CoCreateInstance(CLSID_DeinterlaceFilter,NULL,
		CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void **)&m_pVideoDeinterlaceFilter);
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pVideoDeinterlaceFilter);
		return hr;
	}
	if(m_pGraphBuilder)
	{
		m_pGraphBuilder->AddFilter((IBaseFilter *)m_pVideoDeinterlaceFilter,L"Video Deinterlace");
		if(FAILED(hr))
		{
			SAFE_RELEASE(m_pVideoDeinterlaceFilter);
			return hr;
		}
	}
	return hr;
}


int  CCaptureVideo::CreateCaptureGraph()
{

	//创建Capture
	int bFlag = 1;
	if(FAILED(CreateCaptureGraphBuilder()))
	{
		bFlag=-1;
		return bFlag;
	}

	HRESULT hr=NOERROR;
	hr = BindToVideoDev(m_iDeviceId,&m_pBaseFilter);
	if(FAILED(hr))
	{
		//ERR_DEBUG("BindVideoFilter Failed");
		return -1;
	}
	hr = m_pGraphBuilder->AddFilter(m_pBaseFilter, L"Capture");
	return bFlag;
	
}

/*
函数: BulidPrivewGraph()
功能: 创建函数
*/
int CCaptureVideo::BulidPrivewGraph()
{
	HRESULT hr=NOERROR;
	if(m_iDeviceId <0)
	{
		return -1;
	}

	GUID pCategorySuc = PIN_CATEGORY_PREVIEW;
	GUID pCategoryFail = PIN_CATEGORY_CAPTURE;
	if(m_nPinType==0)
	{
		pCategorySuc = PIN_CATEGORY_PREVIEW;
		pCategoryFail = PIN_CATEGORY_CAPTURE;

	}
	else
	{
		pCategorySuc = PIN_CATEGORY_CAPTURE;
		pCategoryFail = PIN_CATEGORY_PREVIEW;	
	}
	
	hr = SetVideoSize(m_nPinType,m_strRGBByte,m_nFrameRate,m_iWidth,m_iHeight);
	if(FAILED(hr))
	{
		m_strRGBByte = "RGB24";
		hr = SetVideoSize(m_nPinType,m_strRGBByte,m_nFrameRate,m_iWidth,m_iHeight);
		if(FAILED(hr))
		{
			
			//ERR_DEBUG("设置视频格式失败");
			return -1;
		}
	}

	CreateVideoRender(m_nRenderType);

	hr = CreateCaptureSampleGrabber(m_strRGBByte);
	if(FAILED(hr))
	{
		SAFE_RELEASE(m_pSampleGrabberFilter);
		SAFE_RELEASE(m_pSampleGrabber);
		//ERR_DEBUG("CreateCaptureSampleGrabber Failed");
		return -1;
	}
	if(m_nDeinterlace==1)//m_iHeight/m_iWidth!=(1.5/4))
	{
		CreateDeinterlaceFilter();
	}
	if(m_pVideoDeinterlaceFilter)
	{
		hr = m_pCaptureGraphBulid->RenderStream(&pCategorySuc,&MEDIATYPE_Video,m_pBaseFilter,m_pVideoDeinterlaceFilter,m_pSampleGrabberFilter);
		hr = m_pCaptureGraphBulid->RenderStream(NULL,NULL,m_pSampleGrabberFilter,NULL,m_pWindowRender);
		if(FAILED(hr))
		{	
			hr = m_pCaptureGraphBulid->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,m_pBaseFilter,m_pSampleGrabberFilter,NULL);
			if(FAILED(hr))
			{
				//ERR_DEBUG("PrivewVideoDev RenderStream Failed ");
				return -1;
			}
		}
	}
	else
	{
		hr = m_pCaptureGraphBulid->RenderStream(&pCategorySuc,&MEDIATYPE_Video,m_pBaseFilter,m_pSampleGrabberFilter,m_pWindowRender);
		if(FAILED(hr))
		{	
			hr = m_pCaptureGraphBulid->RenderStream(&pCategoryFail,&MEDIATYPE_Video,m_pBaseFilter,m_pSampleGrabberFilter,m_pWindowRender);
			if(FAILED(hr))
			{
				//ERR_DEBUG("PrivewVideoDev RenderStream Failed ");
				return -1;
			}
		}
	}
	
	if(m_bThread)
	{
		m_pSampleGrabber->SetOneShot(FALSE);
		m_pSampleGrabber->SetBufferSamples(TRUE);
	//	m_pSampleGrabber->SetOneShot(TRUE);

	}
	else
	{
		m_pSampleGrabber->SetBufferSamples(TRUE);
		m_pSampleGrabber->SetOneShot(FALSE);
		//m_nDataType:数据类型1--音频，2--视频数据
		//nIndex：设备编号：音频-1，视频0---N
		m_cSampleGrabberCB.SetDataInfo(m_nIndex, m_nDataType);
		int nMode=1;//0--SampleCB,1--BufferCB
		m_pSampleGrabber->SetCallback(&m_cSampleGrabberCB, nMode);
	}

	if(m_pVideoWin==NULL&&m_nRenderType!=2)
	{
		hr = m_pCaptureGraphBulid->FindInterface(&pCategorySuc,&MEDIATYPE_Video,//CAPTURE
							m_pBaseFilter,IID_IVideoWindow,(void **)&m_pVideoWin);
		if (FAILED(hr))//建立失败则查找CAPTURE口
		{
			hr = m_pCaptureGraphBulid->FindInterface(&pCategoryFail,&MEDIATYPE_Video,//CAPTURE
							m_pBaseFilter,IID_IVideoWindow,(void **)&m_pVideoWin);
			if (FAILED(hr))
			{
				hr = m_pCaptureGraphBulid->FindInterface(&LOOK_UPSTREAM_ONLY,&MEDIATYPE_Video,//CAPTURE
					m_pBaseFilter,IID_IVideoWindow,(void **)&m_pVideoWin);

				//ERR_DEBUG("CreateCaptureGraphBuilder  QueryInterface m_pVideoWin Failed");
				//设置视频显示窗口
				SetupVideoWindow();
				hr = StartPreview();
				if(FAILED(hr))
				{
					return -1;
				}
				return 2;
			}
		}
	}
	
	//设置视频显示窗口
	SetupVideoWindow();
	hr = StartPreview();
	if(FAILED(hr))
	{
		return -1;
	}
	return 1;
}

HRESULT CCaptureVideo::StartPreview()
{
	HRESULT hr=NOERROR;
	if(m_pMediaCon==NULL)
	{
		hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaCon);
		if(SUCCEEDED(hr))
		{
			hr = m_pMediaCon->Run();
			if(FAILED(hr))
			{
				m_pMediaCon->Stop();
			}
		}
	}
	else
	{
		hr = m_pMediaCon->Run();
		if(FAILED(hr))
		{
			m_pMediaCon->Stop();
		}
	}
	return hr;
}

HRESULT CCaptureVideo::StopPreview()
{
	HRESULT hr=NOERROR;
	if(m_pMediaCon==NULL)
	{
		hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaCon);
		if(SUCCEEDED(hr))
		{
		  hr = m_pMediaCon->Stop();
		}
		if(FAILED(hr))
		{
			return hr;
		}
	}
	else
	{
		hr = m_pMediaCon->Stop();
	}
	InvalidateRect(m_hWnd,NULL,TRUE);
	return hr;
}


//断掉Graph
void CCaptureVideo::TearDownGraph()
{
	//先释放掉没有使用的
   if(m_pVideoWin)
   {
       m_pVideoWin->put_Owner(NULL);
       m_pVideoWin->put_Visible(OAFALSE);
       m_pVideoWin->Release();
       m_pVideoWin = NULL;
   }
   if(m_pBaseFilter)
        NukeDownstream(m_pGraphBuilder,m_pBaseFilter);
   	SAFE_RELEASE(m_pGraphBuilder);
	SAFE_RELEASE(m_pCaptureGraphBulid);
	SAFE_RELEASE(m_pSampleGrabberFilter);
	SAFE_RELEASE(m_pSampleGrabber);
	SAFE_RELEASE(m_pWindowRender);
	SAFE_RELEASE(m_pMediaEvent);
}

// Tear down everything downstream of a given filter
//关闭下游所有的连接
void CCaptureVideo::NukeDownstream(IGraphBuilder * inGraph, IBaseFilter * inFilter) 
{
	if (inGraph && inFilter)
	{
		IEnumPins * pinEnum = 0;
		if (SUCCEEDED(inFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();
			IPin * pin = 0;
			ULONG cFetched = 0;
			bool pass = true;
			while (pass && SUCCEEDED(pinEnum->Next(1, &pin, &cFetched)))
			{
				if (pin && cFetched)
				{
					IPin * connectedPin = 0;
					pin->ConnectedTo(&connectedPin);
					if(connectedPin) 
					{
						PIN_INFO pininfo;
						if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
						{
							if(pininfo.dir == PINDIR_INPUT) 
							{
								NukeDownstream(inGraph, pininfo.pFilter);
								inGraph->Disconnect(connectedPin);
								inGraph->Disconnect(pin);
								inGraph->RemoveFilter(pininfo.pFilter);
							}
							pininfo.pFilter->Release();
						}
						connectedPin->Release();
					}
					pin->Release();
				}
				else
				{
					pass = false;
				}
			}
			pinEnum->Release();
		}
	}
}

GUID CCaptureVideo::GetMediaTypeGuid(CString strMediaType)
{
	GUID subMediaType=MEDIASUBTYPE_RGB555;

	strMediaType.TrimLeft(_T(" "));
	strMediaType.TrimRight(_T(" "));
	if(strMediaType==_T("RGB555"))
	{
		subMediaType = MEDIASUBTYPE_RGB555;
	}
	else if(strMediaType==_T("RGB565"))
	{		
		subMediaType = MEDIASUBTYPE_RGB565;
	}
	else if(strMediaType==_T("RGB24"))
	{
		subMediaType = MEDIASUBTYPE_RGB24;
	}
	else if(strMediaType==_T("RGB32"))
	{
		subMediaType = MEDIASUBTYPE_RGB32;
	}
	else if(strMediaType==_T("YUY2"))
	{
		subMediaType = MEDIASUBTYPE_YUY2;
	}
	else if(strMediaType==_T("UYVY"))
	{
		subMediaType = MEDIASUBTYPE_UYVY;
	}
	else if(strMediaType==_T("Y41P"))
	{
		subMediaType = MEDIASUBTYPE_Y41P;
	}	

	else if(strMediaType==_T("YV12"))
	{
		subMediaType = MEDIASUBTYPE_YV12;
	}
	else if(strMediaType==_T("I420"))
	{
		subMediaType = MEDIASUBTYPE_I420;
	}
	else
	{	
		subMediaType = MEDIASUBTYPE_RGB24;
	}
	return subMediaType;
}

/*
函数: GetCurrentBuffer(BYTE *pData,long &lDatasize,CString strMediaType)
功能: 得到数据pData缓存
*/
BYTE* CCaptureVideo::GetCurrentBuffer(long &lDatasize,wchar_t* strMediaType)
{
	if(m_pSampleGrabber==NULL)
	{
		lDatasize=0;
		return NULL;
	}
	long  lSize = 0;
	HRESULT hr=NOERROR;
	hr = m_pSampleGrabber->GetCurrentBuffer(&lSize, NULL);
	if (FAILED(hr))
	{
		lDatasize=0;
		return NULL;
	}
	if(lSize<=0)
	{
		lDatasize=0;
		return NULL;
	}
	lDatasize=lSize;
	if(strMediaType==m_strRGBByte)
	{
		 if(lSize!=m_nBufferSize)
		{
			if (m_pDataBuffer)
			{
				delete[] m_pDataBuffer;
				m_pDataBuffer=NULL;
			}
			m_nBufferSize=lSize;
			m_pDataBuffer = new BYTE[m_nBufferSize];
		}
		hr = m_pSampleGrabber->GetCurrentBuffer(&lDatasize,/*(long*)*/reinterpret_cast<long*>(m_pDataBuffer));
		if (FAILED(hr))
		{
			lDatasize=0;
			return NULL;
		}
		return (BYTE*)m_pDataBuffer;
	}
	else
	{
		if(lSize!=m_nBufferSize)
		{
			if (m_pDataBuffer)
			{
				delete[] m_pDataBuffer;
				m_pDataBuffer=NULL;
			}
			m_nBufferSize=lSize;
			m_pDataBuffer = new BYTE[m_nBufferSize];
		}
		hr = m_pSampleGrabber->GetCurrentBuffer(&m_nBufferSize, (long*)m_pDataBuffer);
		if (FAILED(hr))
		{
			lDatasize=0;
			return NULL;
		}
		GUID src_type = GetMediaTypeGuid(m_strRGBByte);
		GUID dst_type = GetMediaTypeGuid(strMediaType);
		if(dst_type==MEDIASUBTYPE_I420)
		{

			if(src_type==MEDIASUBTYPE_YUY2)
			{
				if(m_pColorBuffer==NULL)
				{
					m_pColorBuffer=new BYTE[m_nBufferSize];
				}
				CoverYUY2TOI420(m_iWidth, m_iHeight,(BYTE*)m_pDataBuffer,m_pColorBuffer);
				return (BYTE*)m_pColorBuffer;
			}
			else if(src_type==MEDIASUBTYPE_YUY2)
			{

			}
		}
	}

	return NULL;
}
				   

/*
函数: SetVideoCaptureData(int nIndex,int iDeviceId,HWND hWnd,int nFrameRate,int iWidth,int iHeight,CString strRGBByte,int nRenderType,int nPinType)
功能: 外部接口
//nDataType:数据类型1--音频，2--视频数据
//nIndex：设备编号：音频-1，视频0---N
*/
void CCaptureVideo::SetVideoCaptureData(int nIndex,int iDeviceId,HWND hWnd,
										int nFrameRate,int iWidth,int iHeight,
										/*CString*/char* strRGBByte,int nRenderType,
										int nPinType, int nDataType, BOOL bIsThread)
{
	//nDataType:数据类型1--音频，2--视频数据
	//nIndex：设备编号：音频-1，视频0---N
	m_nIndex = nIndex;
	m_iDeviceId = iDeviceId;
	m_hWnd = hWnd;
	m_nFrameRate = nFrameRate;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_strRGBByte = strRGBByte;
	m_nRenderType= nRenderType;
	m_nPinType=nPinType;
	m_nDataType = nDataType;
	m_bThread		  = bIsThread;

}

void CCaptureVideo::SetCaptureVideoErr(int nError)
{
	m_nError=nError;
}

int CCaptureVideo::GetCaptureVideoErr()
{
	return m_nError;
}

BYTE CCaptureVideo::clip255(LONG v)
{
	 if(v<0) v=0;
	 else if(v>255) v=255;
	 return (BYTE)v;
}


void CCaptureVideo::CoverYUY2TOI420(int inWidth, int inHeight, BYTE *pSrc, BYTE *pDest)
{
	int i, j;
	BYTE *u = pDest + (inWidth * inHeight);
	BYTE *v = u + ((inWidth * inHeight)>>2);
	
	for (i = 0; i < (inHeight>>1); i++)
	{
		
		BYTE *src_l1 = pSrc + (inWidth<<2)*i;
		BYTE *src_l2 = src_l1 + (inWidth<<1);
		BYTE *y_l1 = pDest + (inWidth<<1)*i;
		BYTE *y_l2 = y_l1 + inWidth;
		for (j = 0; j < (inWidth>>1); j++)
		{
			// two pels in one go
			*y_l1++ = src_l1[0];
			*u++ = src_l1[1];
			*y_l1++ = src_l1[2];
			*v++ = src_l1[3];
			*y_l2++ = src_l2[0];
			*y_l2++ = src_l2[2];
			src_l1 += 4;
			src_l2 += 4;
		}
	}
}

//开始数据线程
void CCaptureVideo::BegineCaptureThread()
{
	if(m_bThread==FALSE) 
		return;
	
	BOOL bInnerThread = FALSE;
	if (!bInnerThread)
	{
		return;
	}
	m_bThreadExit = FALSE;
	//打开接收线程，当打开失败时也不报错退出
	m_hThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hThreadEvent == INVALID_HANDLE_VALUE)
	{
		return;
	}  
	AfxBeginThread(OnDataThread,this, THREAD_PRIORITY_NORMAL);//HIGHEST);//NORMAL);//
}

UINT CCaptureVideo::OnDataThread(LPVOID lParam)//LPVOID* lParam)
{
// 	#define FLAG "{5DDA0840-6AE0-4c1b-9488-35F95396E4A8}"
// 	HANDLE handle=::CreateMutex(NULL,FALSE,FLAG);
// 	if(GetLastError()==ERROR_ALREADY_EXISTS)
// 	{	
// 		return 0;
// 	}
	
	CCaptureVideo* pControler=(CCaptureVideo *)lParam;
	if(!pControler)
	{
		return 0;
	}

	int nState=0;
	
	while(!pControler->m_bThreadExit)
	{
		// 检测是否有关闭本线程的信号
		DWORD dwEvent = WaitForSingleObject(pControler->m_hThreadEvent, 40);
		if (dwEvent == WAIT_OBJECT_0)
		{
			AfxEndThread(0,TRUE);
			return 0;
		}
		pControler->OnThreadDeal();
		
	}
	return 0;
}

void CCaptureVideo::OnThreadDeal()
{
	BYTE *pData=NULL;
	long lDatasize=0;
	wchar_t strMediaType[24]=_T("YUY2");
	if(!m_strRGBByte.IsEmpty())
	{
		wcscpy_s(strMediaType, 24, m_strRGBByte);
	}
	//读取缓冲区数据
	pData=GetCurrentBuffer(lDatasize,strMediaType);
	//TRACE("OnThreadDeal:%d\r\n",lDatasize);
	if (m_sThreadCalbackInfo.realDataCalFunc&&m_sThreadCalbackInfo.pMaster&&pData&&lDatasize>0)
	{
		//执行数据回调
		m_sThreadCalbackInfo.realDataCalFunc(m_nIndex, pData, lDatasize, 
			(RealDataStreamType)m_nDataType,NULL, m_sThreadCalbackInfo.pMaster);
	}
}

void CCaptureVideo::SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster)
{
	if(!m_bThread)//回调模式
	{
		m_cSampleGrabberCB.m_realDataCallback = realDataCalBack;
		m_cSampleGrabberCB.m_pMaster = pMaster;
	}
	else//线程缓冲模式
	{
		m_sThreadCalbackInfo.realDataCalFunc = realDataCalBack;
		m_sThreadCalbackInfo.pMaster = pMaster;
	}
}

