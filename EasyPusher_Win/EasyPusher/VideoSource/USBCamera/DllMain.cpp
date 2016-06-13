#if defined(HAS_DLL) && (defined(__WIN32__) || defined(_WIN32) || defined(WIN32) ||defined(_WIN32_WCE))

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD ulReason, LPVOID)
{
    switch (ulReason)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }
    return TRUE;
}

#endif

