#ifndef _ICAMERACAPTUER_H_
#define _ICAMERACAPTUER_H_

#include "DllManager.h"

class DLL_EXPORT ICameraCaptuer
{
public:
    ICameraCaptuer() {}
    virtual ~ICameraCaptuer() {}

    virtual bool OpenCamera(int nCamID, int nWidth, int nHeight) = 0;

    virtual void CloseCamera() = 0;

    virtual unsigned  char * QueryFrame() = 0;
};

class DLL_EXPORT CamCaptuerMgr
{
public:
    static ICameraCaptuer* GetCamCaptuer();

    static void Destory(ICameraCaptuer* pCamCaptuer);
};

#endif

