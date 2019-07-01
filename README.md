# EasyPusher v2.0

EasyPusher RTSP推流是EasyDarwin开源流媒体团队开发的一款推送流媒体音/视频流给标准RTSP流媒体服务器（如EasyDarwin、Wowza）的流媒体推送库，全平台支持(包括Windows、Linux、ARM、Android、iOS)，通过EasyPusher我们就可以避免接触到稍显复杂的RTSP/RTP/RTCP推送流程，只需要调用EasyPusher的几个API接口，就能轻松、稳定地把流媒体音视频数据推送给RTSP流媒体服务器进行处理和转发，EasyPusher经过长时间的企业用户体验，稳定性非常高；更重要的是EasyPusher是完全免费使用的！

## 工作流程

![EasyPusher Work Flow](http://www.easydarwin.org/github/images/easypusher/easypusher_android_workfolw.png?20190701)


## 功能版本

- **EasyPusher-Android**：实时采集安卓摄像头音视频（Android 5.0+支持采集手机桌面屏幕进行直播），进行H.264/AAC编码后，调用EasyPusher进行直播推送，项目地址：[https://github.com/EasyDarwin/EasyPusher-Android](https://github.com/EasyDarwin/EasyPusher-Android "EasyPusher-Android") ；

- **EasyPusher-iOS**：实时采集iOS摄像头音视频进行H.264/AAC编码，调用EasyPusher推送到RTSP流媒体服务器，项目地址：[https://github.com/EasyDarwin/EasyPusher-iOS](https://github.com/EasyDarwin/EasyPusher-iOS "EasyPusher-iOS") ；

- **EasyPusher_File**：推送本地文件到RTSP流媒体服务器进行文件直播；

- **EasyPusher_RTSP**：通过EasyRTSPClient库，将RTSP/RTP数据获取到本地，再通过EasyPusher推送到RTSP流媒体服务器；

- **EasyPusher_Win**：支持本地摄像头和声卡、RTSP流、屏幕捕获、MP4文件通过EasyPusher推送到RTSP流媒体服务器；


	Windows编译方法，

    	Visual Studio 2010 编译：./EasyPusher-master/win/EasyPusher.sln

	Linux编译方法，
		
		chmod +x ./Buildit
		./Buildit

	> 调用提示：目前的调用示例程序，可以接收参数，具体参数的使用，请在调用时增加**-h**命令查阅，EasyPusher_File示例需要将本地文件copy到可执行文件同目录！


	<table>
	<tr><td><b>支持平台</b></td><td><b>芯片</b></td><td><b>位置名称</b></td></tr>
	<tr><td>Windows</td><td>x86</td><td>./Lib/x86/</td></tr>
	<tr><td>Windows</td><td>x64</td><td>./Lib/x64/</td></tr>
	<tr><td>Linux</td><td>x86</td><td>./Lib/x86/</td></tr>
	<tr><td>Linux</td><td>x64</td><td>./Lib/x64/</td></tr>
	<tr><td>海思</td><td>arm-hisiv100-linux</td><td>./Lib/hisiv100/</td></tr>
	<tr><td>海思</td><td>arm-hisiv200-linux</td><td>./Lib/hisiv200/</td></tr>
	<tr><td>海思</td><td>arm-hisiv300-linux</td><td>./Lib/hisiv300/</td></tr>
	<tr><td>海思</td><td>arm-hisiv400-linux</td><td>./Lib/hisiv400/</td></tr>
	<tr><td>海思</td><td>arm-hisiv500-linux</td><td>./Lib/hisiv500/</td></tr>
	<tr><td>海思</td><td>arm-hisiv600-linux</td><td>./Lib/hisiv600/</td></tr>
	<tr><td>Android</td><td>arm</td><td>./Lib/Android/arm/libeasypusher.a</td></tr>
	<tr><td>Android</td><td>arm64</td><td>./Lib/Android/arm64/libeasypusher.a</td></tr>
	<tr><td>Android</td><td>x86</td><td>./Lib/Android/x86/libeasypusher.a</td></tr>
	<tr><td>Android</td><td>x86_64</td><td>./Lib/Android/x86_64/libeasypusher.a</td></tr>
	<tr><td>Android</td><td>mips</td><td>./Lib/Android/mips/libeasypusher.a</td></tr>
	<tr><td>Android</td><td>mips64</td><td>./Lib/Android/mips64/libeasypusher.a</td></tr>
	</table>


## 调用过程 ##
![](http://www.easydarwin.org/skin/easydarwin/images/easypusher20190415.gif)


## 特殊说明 ##
EasyPusher目前支持的音视频格式：

	/* 视频编码 */
	#define EASY_SDK_VIDEO_CODEC_H264	0x01000001		/* H264  */
	#define	EASY_SDK_VIDEO_CODEC_MJPEG	0x01000002		/* MJPEG */
	#define	EASY_SDK_VIDEO_CODEC_MPEG4	0x01000004		/* MPEG4 */
	
	/* 音频编码 */
	#define EASY_SDK_AUDIO_CODEC_AAC	0x01000011		/* AAC */
	#define EASY_SDK_AUDIO_CODEC_G711A	0x01000012		/* G711 alaw*/
	#define EASY_SDK_AUDIO_CODEC_G711U	0x01000014		/* G711 ulaw*/

EasyPusher回调事件定义：

	typedef enum __EASY_PUSH_STATE_T
	{
	    EASY_PUSH_STATE_CONNECTING   =   1,     /* 连接中 */
	    EASY_PUSH_STATE_CONNECTED,              /* 连接成功 */
	    EASY_PUSH_STATE_CONNECT_FAILED,         /* 连接失败 */
	    EASY_PUSH_STATE_CONNECT_ABORT,          /* 连接异常中断 */
	    EASY_PUSH_STATE_PUSHING,                /* 推流中 */
	    EASY_PUSH_STATE_DISCONNECTED,           /* 断开连接 */
	    EASY_PUSH_STATE_ERROR
	}EASY_PUSH_STATE_T;


## 版本下载 ##

- Windows：[https://github.com/EasyDarwin/EasyPusher/releases](https://github.com/EasyDarwin/EasyPusher/releases "EasyPusher")

- Android：[https://fir.im/EasyPusher ](https://fir.im/EasyPusher "EasyPusher_Android")

![EasyPusher_Android](http://www.easydarwin.org/skin/bs/images/app/EasyPusher_AN.png)

- iOS：[https://itunes.apple.com/us/app/easypusher/id1211967057](https://itunes.apple.com/us/app/easypusher/id1211967057 "EasyPusher_iOS")

![EasyPusher_iOS](http://www.easydarwin.org/skin/bs/images/app/EasyPusher_iOS.png)


## 技术支持 ##

- EasyPusher完全免费使用的，技术交流直接到QQ群：465901074，交流意见！


## 获取更多信息 ##

**EasyDarwin**开源流媒体服务器：[www.EasyDarwin.org](http://www.easydarwin.org)

Copyright &copy; EasyDarwin Team 2012-2019

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)

