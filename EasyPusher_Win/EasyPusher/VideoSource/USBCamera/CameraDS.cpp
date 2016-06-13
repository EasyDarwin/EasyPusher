//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//        HardyAI@OpenCV China
//        flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// 使用说明：
//   1. 将CameraDS.h CameraDS.cpp以及目录DirectShow复制到你的项目中
//   2. 菜单 Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      设置为 DirectShow/Include
//   3. 菜单 Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      设置为 DirectShow/Lib
//////////////////////////////////////////////////////////////////////

// CameraDS.cpp: implementation of the CCameraDS class.
//
//////////////////////////////////////////////////////////////////////

// 注意：这里添加预编译头文件stdafx.h在Realse版本下会出现找不到文件的情况
// 决解办法：在工程头文件添加中增加 "./"将其相对路径包含进来(很奇怪Debug下不会有这个问题！！！) [11/8/2015 Dingshuai]
#include "stdafx.h"
#include "CameraDS.h"
#include "convert.h"
#include <time.h>
#include "../../../libEasyPlayer/vstime.h"
#pragma comment(lib,"Strmiids.lib") 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD WINAPI __CameraProcDataThread(LPVOID lpParam);
CCameraDS::CCameraDS()
{
	memset(&cameraList, 0x00, sizeof(CAMERA_LIST_T));

	memset(&cameraSourceThread, 0x00, sizeof(CAMERA_SOURCE_THREAD_T));

    CoInitialize(NULL);
}

CCameraDS::~CCameraDS()
{
    CloseCamera();

	CAMERA_INFO_T *pCameraList = cameraList.pCamera;
	while (NULL != pCameraList)
	{
		CAMERA_INFO_T *pCamera = pCameraList;

		pCameraList = pCamera->pNext;

		delete pCamera;
	}
	cameraList.pCamera = NULL;

    CoUninitialize();
}

void CCameraDS::CloseCamera()
{
	if (cameraSourceThread.flag == 0x02)	cameraSourceThread.flag = 0x03;
	while (cameraSourceThread.flag != 0x00)	{Sleep(200);}
	if (NULL != cameraSourceThread.hThread)
	{
		CloseHandle(cameraSourceThread.hThread);
		cameraSourceThread.hThread = NULL;
	}

    cameraSourceThread.m_pGraph = NULL;
    cameraSourceThread.m_pDeviceFilter = NULL;
    cameraSourceThread.m_pMediaControl = NULL;
    cameraSourceThread.m_pSampleGrabberFilter = NULL;
    cameraSourceThread.m_pSampleGrabber = NULL;
    cameraSourceThread.m_pGrabberInput = NULL;
    cameraSourceThread.m_pGrabberOutput = NULL;
    cameraSourceThread.m_pCameraOutput = NULL;
    cameraSourceThread.m_pMediaEvent = NULL;
    cameraSourceThread.m_pNullFilter = NULL;
    cameraSourceThread.m_pNullInputPin = NULL;
}

CAMERA_LIST_T *CCameraDS::GetCameraList()
{
	if (NULL != cameraList.pCamera || cameraList.count > 0)
	{
		return &cameraList;
	}

	if (NULL == cameraList.pCamera)
	{
		cameraList.count = 0;

	   // enumerate all video capture devices
		CComPtr<ICreateDevEnum> pCreateDevEnum;
		HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
										IID_ICreateDevEnum, (void**)&pCreateDevEnum);

		CComPtr<IEnumMoniker> pEm;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
		if (hr != NOERROR) 
		{
			return &cameraList;
		}

		pEm->Reset();

		CAMERA_INFO_T	*pCameraList = cameraList.pCamera;
		CAMERA_INFO_T	*pCameraInfo = NULL;

		ULONG cFetched;
		IMoniker *pM = NULL;
		while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
		{
			pCameraInfo = new CAMERA_INFO_T;
			memset(pCameraInfo, 0x00, sizeof(CAMERA_INFO_T));

            IPropertyBag *pBag=0;
            hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
            if(SUCCEEDED(hr))
            {
                VARIANT var;
                var.vt = VT_BSTR;
                hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...
                if(hr == NOERROR)
                {
                    //获取设备名称
                    WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,pCameraInfo->friendlyName, sizeof(pCameraInfo->friendlyName) ,"",NULL);
                    SysFreeString(var.bstrVal);                
                }
                pBag->Release();
				cameraList.count++;

				{
					pCameraList = cameraList.pCamera;

					if (NULL == cameraList.pCamera)	cameraList.pCamera = pCameraInfo;
					else
					{
						while (NULL != pCameraList->pNext)
						{
							pCameraList = pCameraList->pNext;
						}
						pCameraList->pNext = pCameraInfo;
					}
				}
            }

			pM->Release();
		}

		pCreateDevEnum = NULL;
		pEm = NULL;
	}
	return &cameraList;
}

bool CCameraDS::OpenCamera(int nCamID, int nWidth, int nHeight, MediaSourceCallBack _callback, void *userPtr)
{
	if(nCamID >= cameraList.count)
    {
        return false;
    }

	if (cameraSourceThread.flag > 0)
	{
		return false;
	}

    bool bDisplayProperties = false;
    HRESULT hr = S_OK;

    // Create the Filter Graph Manager.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                            IID_IGraphBuilder, (void **)&cameraSourceThread.m_pGraph);

    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IBaseFilter, (LPVOID *)&cameraSourceThread.m_pSampleGrabberFilter);

    hr = cameraSourceThread.m_pGraph->QueryInterface(IID_IMediaControl, (void **) &cameraSourceThread.m_pMediaControl);
    hr = cameraSourceThread.m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &cameraSourceThread.m_pMediaEvent);

    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
                            IID_IBaseFilter, (LPVOID*) &cameraSourceThread.m_pNullFilter);


    hr = cameraSourceThread.m_pGraph->AddFilter(cameraSourceThread.m_pNullFilter, L"NullRenderer");

    hr = cameraSourceThread.m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&cameraSourceThread.m_pSampleGrabber);

    AM_MEDIA_TYPE   mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo; 
    hr = cameraSourceThread.m_pSampleGrabber->SetMediaType(&mt);
    MYFREEMEDIATYPE(mt);

    cameraSourceThread.m_pGraph->AddFilter(cameraSourceThread.m_pSampleGrabberFilter, L"Grabber");
 
    // Bind Device Filter.  We know the device because the id was passed in
    BindFilter(nCamID, &cameraSourceThread.m_pDeviceFilter);
    cameraSourceThread.m_pGraph->AddFilter(cameraSourceThread.m_pDeviceFilter, NULL);

    CComPtr<IEnumPins> pEnum;
    cameraSourceThread.m_pDeviceFilter->EnumPins(&pEnum);
 
    hr = pEnum->Reset();
    hr = pEnum->Next(1, &cameraSourceThread.m_pCameraOutput, NULL); 

    pEnum = NULL; 
    cameraSourceThread.m_pSampleGrabberFilter->EnumPins(&pEnum);
    pEnum->Reset();
    hr = pEnum->Next(1, &cameraSourceThread.m_pGrabberInput, NULL); 

    pEnum = NULL;
    cameraSourceThread.m_pSampleGrabberFilter->EnumPins(&pEnum);
    pEnum->Reset();
    pEnum->Skip(1);
    hr = pEnum->Next(1, &cameraSourceThread.m_pGrabberOutput, NULL); 

    pEnum = NULL;
    cameraSourceThread.m_pNullFilter->EnumPins(&pEnum);
    pEnum->Reset();
    hr = pEnum->Next(1, &cameraSourceThread.m_pNullInputPin, NULL);

    //SetCrossBar();

    if (bDisplayProperties) 
    {
        CComPtr<ISpecifyPropertyPages> pPages;

        HRESULT hr = cameraSourceThread.m_pCameraOutput->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPages);
        if (SUCCEEDED(hr))
        {
            PIN_INFO PinInfo;
            cameraSourceThread.m_pCameraOutput->QueryPinInfo(&PinInfo);

            CAUUID caGUID;
            pPages->GetPages(&caGUID);

            OleCreatePropertyFrame(NULL, 0, 0,
                        L"Property Sheet", 1,
                        (IUnknown **)&(cameraSourceThread.m_pCameraOutput.p),
                        caGUID.cElems,
                        caGUID.pElems,
                        0, 0, NULL);
            CoTaskMemFree(caGUID.pElems);
            PinInfo.pFilter->Release();
        }
        pPages = NULL;
    }
    else 
    {
        //////////////////////////////////////////////////////////////////////////////
        // 加入由 lWidth和lHeight设置的摄像头的宽和高 的功能，默认320*240
        // by flymanbox @2009-01-24
        //////////////////////////////////////////////////////////////////////////////
       int _Width = nWidth, _Height = nHeight;
       IAMStreamConfig*   iconfig; 
       iconfig = NULL;
       hr = cameraSourceThread.m_pCameraOutput->QueryInterface(IID_IAMStreamConfig,   (void**)&iconfig);   
      
       AM_MEDIA_TYPE* pmt;    
       if(iconfig->GetFormat(&pmt) !=S_OK) 
       {
          //printf("GetFormat Failed ! \n");
          return   false;   
       }
      
       VIDEOINFOHEADER*   phead;
       if ( pmt->formattype == FORMAT_VideoInfo)   
       {   
            phead=( VIDEOINFOHEADER*)pmt->pbFormat;   
            phead->bmiHeader.biWidth = _Width;   
            phead->bmiHeader.biHeight = _Height;   
            if(( hr=iconfig->SetFormat(pmt)) != S_OK )   
            {
                return   false;
            }
        }   
        
        iconfig->Release();   
        iconfig=NULL;   
        MYFREEMEDIATYPE(*pmt);
    }

    hr = cameraSourceThread.m_pGraph->Connect(cameraSourceThread.m_pCameraOutput, cameraSourceThread.m_pGrabberInput);
    hr = cameraSourceThread.m_pGraph->Connect(cameraSourceThread.m_pGrabberOutput, cameraSourceThread.m_pNullInputPin);

    if (FAILED(hr))
    {
        switch(hr)
        {
            case VFW_S_NOPREVIEWPIN :
                break;
            case E_FAIL :
                break;
            case E_INVALIDARG :
                break;
            case E_POINTER :
                break;
        }
    }

    cameraSourceThread.m_pSampleGrabber->SetBufferSamples(TRUE);
    cameraSourceThread.m_pSampleGrabber->SetOneShot(TRUE);
    
    hr = cameraSourceThread.m_pSampleGrabber->GetConnectedMediaType(&mt);
    if(FAILED(hr))
        return false;

    VIDEOINFOHEADER *videoHeader;
    videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
	cameraSourceThread.width = videoHeader->bmiHeader.biWidth;
	cameraSourceThread.height = videoHeader->bmiHeader.biHeight;

    pEnum = NULL;
    
	cameraSourceThread.camNo = nCamID;
	cameraSourceThread.pCallback = _callback;
	cameraSourceThread.userPtr = userPtr;

	cameraSourceThread.flag = 0x01;
	cameraSourceThread.hThread = CreateThread(NULL, 0, __CameraProcDataThread, &cameraSourceThread, 0, NULL);

	while (cameraSourceThread.flag!=0x02 && cameraSourceThread.flag!=0x00)
	{
		Sleep(200);
	}

    return true;
}


bool CCameraDS::BindFilter(int nCamID, IBaseFilter **pFilter)
{
    if (nCamID < 0)
        return false;
 
    // enumerate all video capture devices
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                                IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
    {
        return false;
    }

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
        &pEm, 0);
    if (hr != NOERROR) 
    {
        return false;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    int index = 0;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= nCamID)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr)) 
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR) 
            {
                if (index == nCamID)
                {
                    pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
                }
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        index++;
    }

    pCreateDevEnum = NULL;
    
    return true;
}



//将输入crossbar变成PhysConn_Video_Composite
void CCameraDS::SetCrossBar()
{
    int i;
    IAMCrossbar *pXBar1 = NULL;
    ICaptureGraphBuilder2 *pBuilder = NULL;

 
    HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
                    CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
                    (void **)&pBuilder);

    if (SUCCEEDED(hr))
    {
        hr = pBuilder->SetFiltergraph(cameraSourceThread.m_pGraph);
    }


    hr = pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, 
                                cameraSourceThread.m_pDeviceFilter,IID_IAMCrossbar, (void**)&pXBar1);

    if (SUCCEEDED(hr)) 
    {
          long OutputPinCount;
        long InputPinCount;
        long PinIndexRelated;
        long PhysicalType;
        long inPort = 0;
        long outPort = 0;

        pXBar1->get_PinCounts(&OutputPinCount,&InputPinCount);
        for( i =0;i<InputPinCount;i++)
        {
            pXBar1->get_CrossbarPinInfo(TRUE,i,&PinIndexRelated,&PhysicalType);
            if(PhysConn_Video_Composite==PhysicalType) 
            {
                inPort = i;
                break;
            }
        }
        for( i =0;i<OutputPinCount;i++)
        {
            pXBar1->get_CrossbarPinInfo(FALSE,i,&PinIndexRelated,&PhysicalType);
            if(PhysConn_Video_VideoDecoder==PhysicalType) 
            {
                outPort = i;
                break;
            }
        }
  
        if(S_OK==pXBar1->CanRoute(outPort,inPort))
        {
            pXBar1->Route(outPort,inPort);
        }
        pXBar1->Release();  
    }
    pBuilder->Release();
}


int	CCameraDS::InRunning()
{
	if (cameraSourceThread.flag == 0x00)		
		return -1;
	return 1;
}

#ifdef _WIN32
unsigned int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst    = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

DWORD WINAPI __CameraProcDataThread(LPVOID lpParam)
{
	CAMERA_SOURCE_THREAD_T	*pThread = (CAMERA_SOURCE_THREAD_T *)lpParam;
	pThread->flag = 0x02;

	EASY_MEDIA_INFO_T	mediaInfo;
	memset(&mediaInfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
	mediaInfo.u32VideoCodec	=	EASY_SDK_VIDEO_CODEC_H264;
	mediaInfo.u32VideoFps	=	25;
	mediaInfo.u32AudioCodec	=	EASY_SDK_AUDIO_CODEC_AAC;
	mediaInfo.u32AudioSamplerate = 16000;
	mediaInfo.u32AudioChannel	=	2;
	pThread->pCallback(pThread->camNo, (int *)pThread->userPtr, EASY_SDK_MEDIA_INFO_FLAG, (char *)&mediaInfo, NULL);

    long evCode;
    long size = 0;

	int image_size = pThread->width * pThread->height * 3;
	char	*raw_data = new char[image_size+1];
	char *enc_buf = new char[1920*1080];	//申请编码的内存空间
	int enc_size = 0;

	int	width = pThread->width;
	int height = pThread->height;
	int fps = 25;
	int gop = 30;
	int bitrate = 512000;
	int	intputformat = 3;		//3:rgb24  0:yv12
	FFE_Init(&pThread->ffeVideoHandle);	//初始化
	FFE_SetVideoEncodeParam(pThread->ffeVideoHandle, ENCODER_H264, width, height, fps, gop, bitrate, intputformat);		//设置编码参数

	int	frameNum = 0;
	pThread->m_pMediaControl->Run();

	unsigned int uiTime = (unsigned int)time(NULL);
	fps = 25;//30;
	int		fps_total = 0;

	LARGE_INTEGER	cpuFreq;		//cpu频率
	_LARGE_INTEGER	lastGetTime;
	_VS_BEGIN_TIME_PERIOD(1);
	QueryPerformanceFrequency(&cpuFreq);
	QueryPerformanceCounter(&lastGetTime);
	_VS_END_TIME_PERIOD(1);

	while (1)
	{
		if (pThread->flag == 0x03)		break;

		evCode = 0L;
		size = 0L;

		pThread->m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);
 
		if (FAILED(pThread->m_pSampleGrabber->GetCurrentBuffer(&size, NULL)))
		{
			Sleep(1);
			continue;
		}

		if (FAILED(pThread->m_pSampleGrabber->GetCurrentBuffer(&size, (long*)raw_data)))
		{
			Sleep(1);
			continue;
		}

		unsigned int uiCurrTime = (unsigned int)time(NULL);
		if (uiTime == uiCurrTime)
		{
			fps_total ++;
		}
		else
		{
			uiTime = uiCurrTime;
			fps = fps_total;
			fps_total = 1;

			TRACE("FPS: %d\n", fps);
		}

		//convert to YUV
		//RGBYUVConvert::ConvertRGB2YUV(m_nWidth, m_nHeight, m_pImgData, m_pYUVData);
		//回调给界面做显示
		{
			RTSP_FRAME_INFO	frameinfo;
			memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
			frameinfo.codec = EASY_SDK_VIDEO_RAW_RGB;
			frameinfo.length = width * height * 3;
			frameinfo.width	 = width;
			frameinfo.height = height;

			pThread->pCallback(pThread->camNo, (int *)pThread->userPtr, EASY_SDK_VIDEO_RAW_RGB, raw_data, &frameinfo);
		}

		//编码
		enc_size = 0;
		int ret = FFE_EncodeVideo(pThread->ffeVideoHandle, (unsigned char*)raw_data, (unsigned char*)enc_buf, &enc_size, ++frameNum, 1);
		if (ret == 0x00 && enc_size>0)
		{
			RTSP_FRAME_INFO	frameinfo;
			memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
			frameinfo.codec = EASY_SDK_VIDEO_CODEC_H264;
			frameinfo.length = enc_size;
			frameinfo.width	 = width;
			frameinfo.height = height;
			frameinfo.fps    = fps;
			frameinfo.type	 = ( (unsigned char)enc_buf[4]==0x67?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P);

#if 0
			static int iii = 0;
			char sztmp[128] = {0,};
			sprintf(sztmp, "C:\\test\\264\\%d.txt", iii++);
			if (iii < 10)
			{
				FILE *f = fopen(sztmp, "wb");
				if (NULL != f)
				{
					fwrite(enc_buf, 1, enc_size, f);
					fclose(f);
				}
			}
#endif
			struct timeval tv;
			_VS_BEGIN_TIME_PERIOD(1);
			gettimeofday(&tv, NULL);
			_VS_END_TIME_PERIOD(1);
			frameinfo.timestamp_sec = tv.tv_sec;
			frameinfo.timestamp_usec = tv.tv_usec;

// 			static unsigned int uiPts = 0;
// 			frameinfo.timestamp_sec = uiPts / 1000;
// 			frameinfo.timestamp_usec = (uiPts%1000) * 1000;
// 			uiPts += (1000 / (fps>0?fps:30));
// 
			int uiPts = tv.tv_sec*1000+tv.tv_usec/1000;
			TRACE("Video PTS: %d\n", uiPts);

			pThread->pCallback(pThread->camNo, (int *)pThread->userPtr, EASY_SDK_VIDEO_FRAME_FLAG, enc_buf, &frameinfo);
		}


		_LARGE_INTEGER	nowTime;
		
		_VS_BEGIN_TIME_PERIOD(1);
		QueryPerformanceCounter(&nowTime);

		if (cpuFreq.QuadPart < 1)	cpuFreq.QuadPart = 1;
		LONGLONG lInterval = (LONGLONG)(((nowTime.QuadPart - lastGetTime.QuadPart) / (double)cpuFreq.QuadPart * (double)1000));
		int iInterval = 33 - (int)lInterval;
		if (iInterval > 0)
		{
			__VS_Delay(iInterval);
			TRACE("delay: %d\n", iInterval);
		}
		QueryPerformanceCounter(&lastGetTime);
		_VS_END_TIME_PERIOD(1);
	}
	pThread->m_pMediaControl->Stop();

	FFE_Deinit(&pThread->ffeVideoHandle);

	delete []enc_buf;
	enc_buf = NULL;
	delete []raw_data;
	raw_data = NULL;
	pThread->flag = 0x00;

	return 0;
}
