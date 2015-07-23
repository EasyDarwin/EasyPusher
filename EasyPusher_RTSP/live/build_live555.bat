echo 编译为DEBUG:  在.mak文件中，修改NODEBUG=1为DEBUG=1, 编译为RELEASE, 修改DEBUG=1为NODEBUG=1


call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
cd liveMedia
nmake /B -f liveMedia.mak
cd ../groupsock
nmake /B -f groupsock.mak
cd ../UsageEnvironment
nmake /B -f UsageEnvironment.mak
cd ../BasicUsageEnvironment
nmake /B -f BasicUsageEnvironment.mak
cd ../testProgs
nmake /B -f testProgs.mak
cd ../mediaServer
nmake /B -f mediaServer.mak
cd ../