/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
	Author: Gavin@easydarwin.org
*/
#ifndef MACRO_HEAD_FILE
#define MACRO_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//常用常量

//无效数值
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值

//////////////////////////////////////////////////////////////////////////////////

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效地址
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////////////

//存储长度
#ifdef _UNICODE
#define CountStringBuffer CountStringBufferW
#else
#define CountStringBuffer CountStringBufferA
#endif

//存储长度
#define CountStringBufferA(String) ((UINT)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((UINT)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//接口释放
#define SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//删除指针
#define SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//关闭句柄
#define SafeCloseHandle(hHandle) { if (hHandle!=NULL) { CloseHandle(hHandle); hHandle=NULL; } }

//删除数组
#define SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//////////////////////////////////////////////////////////////////////////////////
//打开链接
#define OpenLink(String)	ShellExecute(NULL, TEXT("open"), String, NULL, NULL, SW_SHOWNORMAL);

#endif