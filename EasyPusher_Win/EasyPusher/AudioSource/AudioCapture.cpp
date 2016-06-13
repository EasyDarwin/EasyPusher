/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "StdAfx.h"
#include "AudioCapture.h"


CAudioCapture::CAudioCapture(void)
{
}


CAudioCapture::~CAudioCapture(void)
{
}



HRESULT CAudioCapture::CreateInputDevice()
{
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	IEnumMoniker *pEnumCat = NULL;
	IMoniker *pMoniker=NULL;
	
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, 
		CLSCTX_INPROC, IID_ICreateDevEnum, 
		(void **)&pSysDevEnum);
	if(hr!=S_OK)
	{
		return hr;
	}
	
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEnumCat, 0);
	if(hr!=S_OK)
	{
		return hr;
	}
	
	ULONG cFetched=0;
	hr = pEnumCat->Next(1,&pMoniker,&cFetched);
	if(hr!=S_OK)
	{
		return hr;
	}
	hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pInputDevice);   
	if(hr!=S_OK)
	{
		return hr;
	}
	pMoniker->Release();
	pEnumCat->Release();
	pSysDevEnum->Release();

	IEnumPins  *pEnum = NULL;
	IPin *pPin = NULL;

	hr = m_pInputDevice->EnumPins(&pEnum);
	if(hr!=S_OK)
	{
		return hr;
	}
	while((hr = pEnum->Next(1, &pPin, 0)) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
		
        hr = pPin->QueryDirection(&PinDirThis);
		if(hr!=S_OK)
		{
			return hr;
		}             // Does the pin's direction match the requested direction?
       if (PINDIR_INPUT == PinDirThis)
        {
            PIN_INFO pininfo={0};
			
            // Direction matches, so add pin name to listbox
            hr = pPin->QueryPinInfo(&pininfo);
			if(hr!=S_OK)
			{
				return hr;
			}
            if (SUCCEEDED(hr))
            {
				//char achName[64];
				WCHAR wName[] = L"Microphone";
				WCHAR wName2[]=L"Âó¿Ë·ç";
				WCHAR wName3[]=L"microphone";
			//	CString str(pininfo.achName);
				IAMAudioInputMixer *pPinMixer;
				hr = pPin->QueryInterface(IID_IAMAudioInputMixer, (void **)&pPinMixer);
				if(hr!=S_OK)
				{
					return hr;
				}
		
				if(memicmp(wName,pininfo.achName,sizeof(wName))==0 || 
					memicmp(wName2,pininfo.achName,sizeof(wName2))==0||
					memicmp(wName3,pininfo.achName,sizeof(wName3))==0)
				{
							
                    hr = pPinMixer->put_Enable(TRUE);
					if(hr!=S_OK)
					{
						return hr;
					}
					
				}
			
				pPinMixer->Release();
				            
              }
			pininfo.pFilter->Release();
            // The pininfo structure contains a reference to an IBaseFilter,
            // so you must release its reference to prevent resource a leak.
            
        }
		else 
		
		if(PINDIR_OUTPUT == PinDirThis)
		{	
			IAMStreamConfig *pCfg;
			hr = pPin->QueryInterface(IID_IAMStreamConfig,(void**)&pCfg);
			if(hr!=S_OK)
			{
				return hr;
			}
			AM_MEDIA_TYPE *pmt={0};
			hr = pCfg->GetFormat(&pmt);
			if(hr!=S_OK)
			{
				return hr;
			}
		
			if(SUCCEEDED(hr))
			{
				WAVEFORMATEX *pWF = (WAVEFORMATEX *)pmt->pbFormat;
				pWF->wFormatTag = WAVE_FORMAT_PCM;
				pWF->nChannels = 1;
				pWF->nSamplesPerSec = 8000;
				pWF->nAvgBytesPerSec = 16000;
				pWF->wBitsPerSample = 16;
				pWF->nBlockAlign = 2;
				hr = pCfg->SetFormat(pmt);
//				DeleteMediaType(pmt);
			}
			pCfg->Release();


			IAMBufferNegotiation *pBuffCfg;
			hr = pPin->QueryInterface(IID_IAMBufferNegotiation,(void**)&pBuffCfg);
			if(hr!=S_OK)
			{
				return hr;
			}
			ALLOCATOR_PROPERTIES prop={0};
	
			prop.cbBuffer = 3840;
			prop.cBuffers = 5;
			prop.cbAlign = 3840;
			
			hr = pBuffCfg->SuggestAllocatorProperties(&prop);
			if(hr!=S_OK)
			{
				return hr;
			}
			pBuffCfg->Release();
		}
		
		pPin->Release();
        
    }
    hr = pEnum->Release();



	return hr;
}