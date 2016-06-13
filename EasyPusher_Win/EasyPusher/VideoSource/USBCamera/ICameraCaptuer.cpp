#include "ICameraCaptuer.h"
#include "CameraDS.h"

ICameraCaptuer* CamCaptuerMgr::GetCamCaptuer() 
{ 
    //return new CCameraDS; 
	return NULL;
}

void CamCaptuerMgr::Destory(ICameraCaptuer* pCamCaptuer) 
{ 
    delete  pCamCaptuer; 
    pCamCaptuer = 0;
}


