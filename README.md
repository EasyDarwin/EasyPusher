# EasyPusher #

EasyPusher是一个推送流媒体音视频给开源流媒体服务器EasyDarwin的推送库，通过EasyPusher我们就可以避免接触到稍显复杂的RTSP/RTP推送流程，只需要调用EasyPusher的几个API接口，就能轻松、稳定地把流媒体音视频数据推送给EasyDarwin服务器进行转发和分发;

## 调用示例 ##

- **EasyPusher_File**：推送本地文件到EasyDarwin流媒体服务器；

- **EasyPusher_RTSP**：通过EasyRTSPClient库，将RTSP/RTP数据获取到本地，再推送到EasyDarwin；

- **EasyPusher_SDK**：通过调用私有SDK回调的音视频数据，进行RTSP/RTP直播推送，示例中的SDK是我们EasyDarwin开源摄像机的配套库，EasyDarwin开源摄像机硬件可以在：[https://easydarwin.taobao.com/](https://easydarwin.taobao.com/ "EasyCamera")购买，EasyCamera SDK及配套源码可在 [http://www.easydarwin.org](http://www.easydarwin.org "EasyDarwin") 或者 [https://github.com/EasyDarwin/EasyCamera](https://github.com/EasyDarwin/EasyCamera "EasyCamera") 获取到，您也可以用自己项目中用到的SDK获取音视频数据进行推送；

	Windows编译方法，

    	Visual Studio 2010 编译：./EasyPusher-master/win/EasyPusher_Demo.sln

	Linux编译方法，
		
		chmod +x ./Buildit
		./Buildit

	> 调用提示：目前的调用示例程序，还未做到接收参数，需要在代码内部修改具体的EasyDarwin流媒体服务IP、端口等信息，然后编译成可执行文件，EasyPusher_File示例需要将本地文件copy到可执行文件同目录！


- **我们同时提供Windows、Linux、ARM版本的libEasyPusher库**：arm版本请将交叉编译工具链发送至[support@easydarwin.org](mailto:support@easydarwin.org "EasyDarwin mail")，我们会帮您具体编译；

## 调用过程 ##
![](http://www.easydarwin.org/skin/easydarwin/images/easypusher20150802.png)


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

## 获取更多信息 ##

邮件：[support@easydarwin.org](mailto:support@easydarwin.org) 

WEB：[www.EasyDarwin.org](http://www.easydarwin.org)

Copyright &copy; EasyDarwin.org 2012-2015

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)
