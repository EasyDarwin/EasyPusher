#pragma once

#ifdef _DEBUG
//#include <vld.h>
#endif



#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <liveMedia.hh>
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "libBasicUsageEnvironment.lib")
#pragma comment(lib, "libgroupsock.lib")
#pragma comment(lib, "libliveMedia.lib")
#pragma comment(lib, "libUsageEnvironment.lib")


#define		RTSP_CLIENT_VERBOSITY_LEVEL		1
#define		RTSP_PROG_NAME					"rtspClient"

typedef struct __RTSP_OBJ_T
{
	TaskScheduler* scheduler;
	UsageEnvironment* env;
	RTSPClient	*rtspClient;
	void		*pCallback;
	void		*pUserPtr;
#ifdef _WIN32
	HANDLE		hThread;
#else
	pthread_t	threadId;
#endif
	int			flag;
	void		*pEx;
}RTSP_OBJ_T;

class CLive555
{
public:
	CLive555(void);
	virtual ~CLive555(void);

	int		SetCallback(LPVOID lpCallback);

	int		OpenStream(int _channelid, char const* rtspURL, int _mediatype, RTP_CONNECT_TYPE _connecttype, char const *username, char const *password, void *userPtr, int _connecttimes=1000, int rtpPacket=0x00);
	int		CloseStream();

	int		GetSPS(char *sps, int *spslen);
	int		GetPPS(char *pps, int *ppslen);
	void	GetResolution(int *_width, int *_height);


protected:
	RTSP_OBJ_T	rtspObj;
};

