#include "NVSource.h"
#include "trace.h"
#include "base64.h"
#include "sps_pps.h"
#include <time.h>
#include <liveMedia_version.hh>


#ifdef _WIN32
DWORD WINAPI __RtspClientProcThread(LPVOID lpParam);
#else
void __RtspClientProcThread(LPVOID lpParam);
#endif
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterTEARDOWN(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
  // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);


int AnalysisSPSPPS(RTSPClient* rtspClient, char *strSDP);


CLive555::CLive555(void)
{
	memset(&rtspObj, 0x00, sizeof(RTSP_OBJ_T));
}


CLive555::~CLive555(void)
{
	CloseStream();
}

int		CLive555::SetCallback(LPVOID lpCallback)
{
	rtspObj.pCallback = lpCallback;
	return 0;
}


int		CLive555::GetSPS(char *_sps, int *_spslen)
{
	if (NULL == rtspObj.rtspClient)		return -1;

	int *spslen = rtspObj.rtspClient->GetSpsLen();
	if (NULL == spslen)		return -1;
	if (*spslen < 1)		return -1;

	if (NULL != _sps)	memcpy(_sps, rtspObj.rtspClient->GetSps(), *spslen);
	if (NULL != _spslen)	*_spslen = *spslen;

	return 0;
}
int		CLive555::GetPPS(char *_pps, int *_ppslen)
{
	if (NULL == rtspObj.rtspClient)		return -1;

	int *ppslen = rtspObj.rtspClient->GetPpsLen();
	if (NULL == ppslen)		return -1;
	if (*ppslen < 1)		return -1;

	if (NULL != _pps)	memcpy(_pps, rtspObj.rtspClient->GetPps(), *ppslen);
	if (NULL != _ppslen)	*_ppslen = *ppslen;

	return 0;
}
void	CLive555::GetResolution(int *_width, int *_height)
{
	if (NULL == rtspObj.rtspClient)		return;

	rtspObj.rtspClient->GetResolution(_width, _height);
}

int		CLive555::OpenStream(int _channelid, char const* rtspURL, int _mediatype, RTP_CONNECT_TYPE _connecttype, char const *username, char const *password, void *userPtr, int _connecttimes, int rtpPacket)
{
	if (NULL == rtspObj.scheduler)		rtspObj.scheduler = BasicTaskScheduler::createNew();
	if (NULL == rtspObj.scheduler)		return -1;
	if (NULL == rtspObj.env)			rtspObj.env       = BasicUsageEnvironment::createNew(*rtspObj.scheduler);
	if (NULL == rtspObj.env)			return -1;

	CLIENT_PARAM_T	param;
	memset(&param, 0x00, sizeof(CLIENT_PARAM_T));
	param.mediainfo.channelId = _channelid;
	param.connectType	=	_connecttype;
	param.mediatype	=	_mediatype;
	if (NULL != username)	strcpy(param.username, username);
	if (NULL != password)	strcpy(param.password, password);
	param.connect_times = _connecttimes;
	strcpy(param.url, rtspURL);

	rtspObj.pUserPtr = userPtr;

	param.mediainfo.pCallback = rtspObj.pCallback;
	param.mediainfo.userPtr   = userPtr;
	param.mediainfo.outputRtpPacket = rtpPacket;

	param.mLastTimestamp = (unsigned int)time(NULL);

	rtspObj.rtspClient = CLiveRtspClient::createNew(*rtspObj.env, &param, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, RTSP_PROG_NAME);
	if (rtspObj.rtspClient == NULL) 
	{
		_TRACE("Failed to create a RTSP client for URL %s: %s\n", rtspURL, rtspObj.env->getResultMsg());
		//env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
		return -2;
	}
#if 1
	if (NULL != strstr(rtspURL, "playback_") || NULL != strstr(rtspURL, "export_"))
	{
		rtspObj.rtspClient->SetTimeFlag(1);
	}

	if (NULL == rtspObj.hThread)
	{
		rtspObj.flag = 0x01;
		rtspObj.pEx	 = this;
#ifdef _WIN32
		rtspObj.hThread = CreateThread(NULL, 0, __RtspClientProcThread, &rtspObj, 0, NULL);
#else
		 ret=pthread_create(&rtspObj.threadId, NULL, (void  *) __RtspClientProcThread, &rtspObj);
#endif
		if (NULL == rtspObj.hThread)		rtspObj.flag = 0x00;
		while (rtspObj.flag != 0x02 && rtspObj.flag!=0x00)	{Sleep(100);}
	}

	//回调连接状态
	if (NULL != rtspObj.rtspClient->GetClientParam())
	{
		NVSourceCallBack pNVSCallback = (NVSourceCallBack )rtspObj.rtspClient->GetClientParam()->mediainfo.pCallback;
		if (NULL != pNVSCallback)
		{
			pNVSCallback(rtspObj.rtspClient->GetClientParam()->mediainfo.channelId, (int *)rtspObj.rtspClient->GetClientParam()->mediainfo.userPtr, MEDIA_TYPE_EVENT, NULL, NULL);
		}
	}

	rtspObj.rtspClient->sendDescribeCommand(continueAfterDESCRIBE, rtspObj.rtspClient->GetAuthenticator()); 
#endif
	if (rtspObj.flag == 0x00)
	{
		if (NULL != rtspObj.rtspClient)
		{
			Medium::close(rtspObj.rtspClient);
			rtspObj.rtspClient = NULL;
		}
		return -1;
	}

	return 0;
}

int	CLive555::CloseStream()
{
	if (NULL != rtspObj.hThread)
	{
		if (rtspObj.flag == 0x02)	rtspObj.flag = 0x03;
		while (rtspObj.flag != 0x00)	{Sleep(100);}
		CloseHandle(rtspObj.hThread);
		rtspObj.hThread = NULL;
	}

	if (NULL != rtspObj.rtspClient)
	{
		//shutdownStream(rtspObj.rtspClient, 0x00);
		//Medium::close(rtspObj.rtspClient);
		rtspObj.rtspClient = NULL;
	}
	if (NULL != rtspObj.env)
	{
		rtspObj.env->reclaim();
		rtspObj.env = NULL;
	}
	if (NULL != rtspObj.scheduler)
	{
		delete rtspObj.scheduler;
		rtspObj.scheduler = NULL;
	}

	return 0;
}


void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
#ifdef _DEBUG
	_TRACE("continueAfterDESCRIBE\n");
#endif

  do 
  {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((CLiveRtspClient*)rtspClient)->scs; // alias
	CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;

    if (resultCode != 0) 
	{
		if (resultCode == -10057)	//unknown error
		{
			
		}
      _TRACE("Failed to get a SDP description: %s\n", resultString);
      delete[] resultString;
      break;
    }

    char* const sdpDescription = resultString;
	
    //env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";
	if (NULL != sdpDescription)
	{
		_TRACE("Got a SDP description: %s\n", sdpDescription);

		if (NULL != pClient)		pClient->SetSDP(sdpDescription, (int)strlen(sdpDescription));

		char *sprop_parameter_sets = strstr(sdpDescription, "sprop-parameter-sets=");
		if (NULL != sprop_parameter_sets)
		{
			AnalysisSPSPPS(pClient, sprop_parameter_sets+21);
		}



		// Create a media session object from this SDP description:
		scs.session = MediaSession::createNew(env, sdpDescription);
		delete[] sdpDescription; // because we don't need it anymore
		if (scs.session == NULL) 
		{
			_TRACE("Failed to create a MediaSession object from the SDP description: %s\n", env.getResultMsg());
		  //env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
		  break;
		}
		else if (!scs.session->hasSubsessions()) 
		{
			_TRACE("This session has no media subsessions (i.e., no \"m=\" lines)\n");
		  //env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
		  break;
		}
	}

    // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
    // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
    // (Each 'subsession' will have its own data source.)
    scs.iter = new MediaSubsessionIterator(*scs.session);
    setupNextSubsession(rtspClient);
    return;
  } while (0);

  // An unrecoverable error occurred with this stream.
  shutdownStream(rtspClient);
}



void setupNextSubsession(RTSPClient* rtspClient) 
{
#ifdef _DEBUG
	_TRACE("setupNextSubsession\n");
#endif

  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((CLiveRtspClient*)rtspClient)->scs; // alias
  CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;
  
  scs.subsession = scs.iter->next();
  if (scs.subsession != NULL) {
	  if (!scs.subsession->initiate(pClient->GetMediaInfo()->outputRtpPacket==0x01?LIVEMEDIA_EASYDARWIN_RELAY_MARK:-1)) {
		_TRACE("Failed to initiate the %s/%s subsession: %s\n", scs.subsession->mediumName(), scs.subsession->codecName(), env.getResultMsg());
      //env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      //env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
      if (scs.subsession->rtcpIsMuxed()) {
	//env << "client port " << scs.subsession->clientPortNum();
      } else {
	//env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
      }
      //env << ")\n";

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
	  rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, pClient->GetConnectType()==RTP_OVER_TCP?True:False, False, pClient->GetAuthenticator());
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.session->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime(), 1.0f, pClient->GetAuthenticator());
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, 0.0, -1.0, 1.0f, pClient->GetAuthenticator());
  }
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {


#ifdef _DEBUG
	_TRACE("continueAfterSETUP\n");
#endif

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((CLiveRtspClient*)rtspClient)->scs; // alias
	CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;

    if (resultCode != 0) 
	{
		//_TRACE("Failed to set up the %s subsession: %s\n", *scs.subsession, resultString);
      //env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
      break;
    }

    //env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
    if (scs.subsession->rtcpIsMuxed()) {
      //env << "client port " << scs.subsession->clientPortNum();
    } else {
      //env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum()+1;
    }
    //env << ")\n";

    // Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
    // (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
    // after we've sent a RTSP "PLAY" command.)
	int time_flag = 0x00;
	if (NULL != pClient)	time_flag = pClient->GetTimeFlag();
	char *mediatype = (char*)scs.subsession->mediumName();
	
	scs.subsession->sink = CLiveRtspSink::createNew(env, *scs.subsession, pClient->GetMediaInfo(), pClient->GetLastTimestampPtr(), time_flag);
      // perhaps use your own custom "MediaSink" subclass instead
    if (scs.subsession->sink == NULL) 
	{
		_TRACE("Failed to create a data sink for the %s/%s subsession %s\n", scs.subsession->mediumName(), scs.subsession->codecName(), env.getResultMsg());
      //env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
	  //<< "\" subsession: " << env.getResultMsg() << "\n";
      break;
    }

	if( (0 == strcmp(mediatype, "video"))   &&
		(0 == strcmp(scs.subsession->codecName(),  "MP4V-ES")) &&
		(NULL != scs.subsession->fmtp_config()) ) {
		// For MPEG-4 video RTP streams, the 'config' information
		// from the SDP description contains useful VOL etc. headers.
		// Insert this data at the front of the output file:
		unsigned configLen;
		unsigned char* configData
				= parseGeneralConfigStr( scs.subsession->fmtp_config(), configLen );

		CLiveRtspSink *pRtspSink = (CLiveRtspSink *)scs.subsession->sink;
		pRtspSink->setupMPEG4VOLHeader( configData, configLen );
		delete[] configData;
	}

	_TRACE("Created a data sink for the %s/%s subsession\n", scs.subsession->mediumName(), scs.subsession->codecName());
    //env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";
    scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession 
    scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
				       subsessionAfterPlaying, scs.subsession);
    // Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
    if (scs.subsession->rtcpInstance() != NULL) {
      scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
    }
  } while (0);
  delete[] resultString;

  // Set up the next subsession, if any:
  setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {

#ifdef _DEBUG
	_TRACE("continueAfterPLAY\n");
#endif

  Boolean success = False;

  do {
    UsageEnvironment& env = rtspClient->envir(); // alias
    StreamClientState& scs = ((CLiveRtspClient*)rtspClient)->scs; // alias
	//CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;

    if (resultCode != 0) 
	{
      //env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
		_TRACE("Failed to start playing session: %s\n", resultString);
      break;
    }

    // Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
    // using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
    // 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
    // (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
    if (scs.duration > 0) {
      unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
      scs.duration += delaySlop;
      unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
      scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
    }


    //env << *rtspClient << "Started playing session";
	
    if (scs.duration > 0) 
	{
		_TRACE("Started playing session(for up to %d seconds)\n", scs.duration);
    }
	else
	{
		_TRACE("Started playing session\n");
	}
    //env << "...\n";

    success = True;
  } while (0);
  delete[] resultString;

  if (!success) 
  {
    // An unrecoverable error occurred with this stream.
    shutdownStream(rtspClient);
  }
}

void continueAfterTEARDOWN(RTSPClient* rtspClient, int resultCode, char* resultString)
{
#ifdef _DEBUG
	_TRACE("continueAfterTEARDOWN\n");
#endif

	CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;

	Medium::close(rtspClient);
	pClient->Disconnect();
}

// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {

#ifdef _DEBUG
	_TRACE("subsessionAfterPlaying\n");
#endif

  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

  // Begin by closing this subsession's stream:
  Medium::close(subsession->sink);
  subsession->sink = NULL;

  // Next, check whether *all* subsessions' streams have now been closed:
  MediaSession& session = subsession->parentSession();
  MediaSubsessionIterator iter(session);
  while ((subsession = iter.next()) != NULL) {
    if (subsession->sink != NULL) return; // this subsession is still active
  }

  // All subsessions' streams have now been closed, so shutdown the client:
  shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) {
  MediaSubsession* subsession = (MediaSubsession*)clientData;
  RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
  UsageEnvironment& env = rtspClient->envir(); // alias

  _TRACE("Received RTCP 'BYE' on %s subsession\n", *subsession);
  //env << *rtspClient << "Received RTCP \"BYE\" on \"" << *subsession << "\" subsession\n";

  // Now act as if the subsession had closed:
  subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
  CLiveRtspClient* rtspClient = (CLiveRtspClient*)clientData;
  StreamClientState& scs = rtspClient->scs; // alias

  scs.streamTimerTask = NULL;

  // Shut down the stream:
  shutdownStream(rtspClient);
}

void shutdownStream(RTSPClient* rtspClient, int exitCode) 
{

#ifdef _DEBUG
	_TRACE("shutdownStream\n");
#endif

	if (NULL == rtspClient)		return;

	if (exitCode == 0x01)		return;

	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((CLiveRtspClient*)rtspClient)->scs; // alias

	CLiveRtspClient *pClient = (CLiveRtspClient *)rtspClient;



  // First, check whether any subsessions have still to be closed:
  if (scs.session != NULL) 
  { 
    Boolean someSubsessionsWereActive = False;
    MediaSubsessionIterator iter(*scs.session);
    MediaSubsession* subsession;

    while ((subsession = iter.next()) != NULL) 
	{
		if (subsession->sink != NULL) 
		{
			Medium::close(subsession->sink);
			subsession->sink = NULL;

			if (subsession->rtcpInstance() != NULL) 
			{
				subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
			}

			someSubsessionsWereActive = True;
		}
    }

    if (someSubsessionsWereActive) 
	{
      // Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
      // Don't bother handling the response to the "TEARDOWN".
      //rtspClient->sendTeardownCommand(*scs.session, continueAfterTEARDOWN, pClient->GetAuthenticator());
		rtspClient->sendTeardownCommand(*scs.session, NULL, pClient->GetAuthenticator());
    }
  }
  

  _TRACE("Closing the stream.\n");
  Medium::close(rtspClient);
  rtspClient = NULL;
  return;

  //env << *rtspClient << "Closing the stream.\n";
  if (exitCode == 0x00)
  {
	_TRACE("Closing the stream.\n");
	Medium::close(rtspClient);
  }
  else
  {
	//if (NULL != pClient->GetClientParam())
	{
		//if (pClient->GetClientParam()->connect_times > 0)
		{
			rtspClient->sendDescribeCommand(continueAfterDESCRIBE, pClient->GetAuthenticator());
		}
	}
  }
}



int AnalysisSPSPPS(RTSPClient* rtspClient, char *strSDP)
{
	int iCnt=0, iIndexSPS=0, iIndexPPS=0, iStep=0;
	char strSPS[512], strPPS[512];

	memset(strSPS, 0x00, sizeof(strSPS));
	memset(strPPS, 0x00, sizeof(strPPS));

	if(strSDP == NULL)	return -1;

	while( (int)strlen(strSDP) >= iCnt){
		if(strSDP[iCnt] == ','){
			iStep++;
		}else if(strSDP[iCnt] == '\r'){
			if(strSDP[iCnt+1] == '\n')
				break;
		}else{
			if(iStep==0){
				strSPS[iIndexSPS++] = strSDP[iCnt];
			}else if(iStep==1){
				strPPS[iIndexPPS++] = strSDP[iCnt];
			}
		}
		iCnt++;
	}


	CLiveRtspClient	*pClient = (CLiveRtspClient *)rtspClient;

	pClient->ResetSpsPps();

	//if (NULL == m_ustrSPS)	m_ustrSPS = new unsigned char[512];

	decode(strSPS, (int)strlen(strSPS), (char *)pClient->GetSps(), pClient->GetSpsLen());
	decode(strPPS, (int)strlen(strPPS), (char *)pClient->GetPps(), pClient->GetPpsLen());

#ifdef _DEBUG
	FILE *fSps = fopen("sps.txt", "wb");
	if (NULL != fSps)
	{
		fwrite(pClient->GetSps(), 1, *pClient->GetSpsLen(), fSps);
		fclose(fSps);
	}
	FILE *fPps = fopen("pps.txt", "wb");
	if (NULL != fPps)
	{
		fwrite(pClient->GetPps(), 1, *pClient->GetPpsLen(), fPps);
		fclose(fPps);
	}
#endif

#if 1
	//int m_iSPSLen = 0;
	//decode(strSPS, (int)strlen(strSPS), (char *)strSPS, &m_iSPSLen);


	h264_sps_t sps_t;
	memset(&sps_t, 0, sizeof(h264_sps_t));
	int i_ret = 0;

	int nEexcursion = 0;
	if ( (unsigned char)pClient->GetSps()[0] == 0x00 &&
		 (unsigned char)pClient->GetSps()[1] == 0x00 &&
		 (unsigned char)pClient->GetSps()[2] == 0x01)
	{
		nEexcursion = 3;
	}
	else if ( (unsigned char)pClient->GetSps()[0] == 0x00 &&
			  (unsigned char)pClient->GetSps()[1] == 0x00 &&
			  (unsigned char)pClient->GetSps()[2] == 0x00 &&
		      (unsigned char)pClient->GetSps()[3] == 0x01)
	{
		nEexcursion = 4;
	}

    if( (  i_ret = h264_sps_read(  (unsigned char*)pClient->GetSps()+nEexcursion, *pClient->GetSpsLen()-nEexcursion, &sps_t ) ) < 0 )
    {
		//fprintf( stderr, "h264: h264_sps_read failed\n" );
    }
	else
	{
		pClient->SetResolution(sps_t.i_mb_width*16, sps_t.i_mb_height*16);
		if (sps_t.vui.i_num_units_in_tick>0 && sps_t.vui.i_time_scale>0)
		{
			pClient->SetFps(sps_t.vui.i_time_scale / sps_t.vui.i_num_units_in_tick / 2);
		}

		_TRACE("AnalysisSPS: %d x %d\t\tFixFrameRate:%d\n", sps_t.i_mb_width*16, sps_t.i_mb_height*16, sps_t.vui.b_fixed_frame_rate);
	}

#endif
 	return TRUE;
}


#ifdef _WIN32
DWORD WINAPI __RtspClientProcThread(LPVOID lpParam)
#else
void *__RtspClientProcThread(void *lpParam)
#endif
{
	RTSP_OBJ_T	*pRtspObj = (RTSP_OBJ_T *)lpParam;
	CLive555   *pThis = (CLive555 *)pRtspObj->pEx;

	pRtspObj->flag = 0x02;


	CLIENT_PARAM_T	clientParam;
	memset(&clientParam, 0x00, sizeof(CLIENT_PARAM_T));
	memcpy(&clientParam, pRtspObj->rtspClient->GetClientParam(), sizeof(CLIENT_PARAM_T));
	if (clientParam.connect_times == 0x00)	clientParam.connect_times = 0x01;	//如果当前设置为不重连,则最小调度一次,直到此次断线


	while (1)
	{
		if (pRtspObj->flag == 0x03)		break;

		//pRtspObj->env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
		//continue;

		if (clientParam.connect_times < 1)	break;		//已达最大重连次数

		BasicTaskScheduler0 *pTaskScheduler0 = NULL;
		if (NULL != pRtspObj->env && (pTaskScheduler0 = (BasicTaskScheduler0 *)&pRtspObj->env->taskScheduler()))
		{
			pTaskScheduler0->SingleStep(0);
		}

		unsigned int uiCurrTime = (unsigned int)time(NULL);
		unsigned int lasttimestamp = 0;
		if (NULL != pRtspObj->rtspClient)	lasttimestamp = pRtspObj->rtspClient->GetLastTimestamp();
		if ( (lasttimestamp>0) && (uiCurrTime - lasttimestamp > 10) )
		{
			pRtspObj->rtspClient->SetLastTimestamp(uiCurrTime);
			if (NULL != pRtspObj->rtspClient && clientParam.connect_times>0)
			{
				shutdownStream(pRtspObj->rtspClient, 0x00);
			}

			if (clientParam.connect_times != 1000 && clientParam.connect_times>0)	clientParam.connect_times--;

			int nConnectingTimes = clientParam.connect_times;
			if (nConnectingTimes > 0)
			{
				pThis->OpenStream(clientParam.mediainfo.channelId, clientParam.url, clientParam.mediatype, clientParam.connectType, clientParam.username, clientParam.password, clientParam.mediainfo.userPtr, clientParam.connect_times, clientParam.mediainfo.outputRtpPacket);
			}
			else //if (nConnectingTimes < 1)
			{
#ifdef _DEBUG
				_TRACE("重连次数小于1. 连接线程退出.\n");
#endif
				//回调连接状态
				if (NULL != pRtspObj->pCallback)
				{
					NVSourceCallBack pNVSCallback = (NVSourceCallBack )pRtspObj->pCallback;
					if (NULL != pNVSCallback)
					{
						NVS_FRAME_INFO	frameinfo;
						memset(&frameinfo, 0x00, sizeof(NVS_FRAME_INFO));
						frameinfo.codec = 0x01;	//连接线程退出

						pNVSCallback(pRtspObj->rtspClient->GetClientParam()->mediainfo.channelId, (int *)pRtspObj->pUserPtr, MEDIA_TYPE_EVENT, NULL, &frameinfo);
					}
				}

				break;
			}
		}
	}

	if (NULL != pRtspObj->rtspClient && clientParam.connect_times>0)
	{
		shutdownStream(pRtspObj->rtspClient, 0x00);
	}

	pRtspObj->flag = 0x00;

	return 0;
}
