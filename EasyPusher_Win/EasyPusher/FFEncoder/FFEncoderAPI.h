/*
//==================================================================
//==================================================================
//音频编码
//当前仅支持AAC编码
//==================================================================
//==================================================================

	int channel = 2;
	int sampleFrequency = 44100;
	int bitrate = 150000;

	FFE_HANDLE	ffeAudioHandle = NULL;
	FFE_Init(&ffeAudioHandle);
	FFE_SetAudioEncodeParam(ffeAudioHandle, ENCODER_AAC, channel, sampleFrequency, bitrate);

	int enc_size = 0;
	int ret = FFE_EncodeAudio(ffeAudioHandle, (unsigned short*)raw_buf, 4096, (unsigned char*)enc_buf, &enc_size);
	if (ret == 0x00 && enc_size>0)
	{
		if (NULL != fOut)	fwrite(enc_buf, 1, enc_size, fOut);
	}

	while (1)
	{
		enc_size = 0;
		ret = FFE_EncodeAudio(ffeAudioHandle, NULL, 0, (unsigned char*)enc_buf, &enc_size);
		if (ret == 0x00 && enc_size>0)
		{
			if (NULL != fOut)	fwrite(enc_buf, 1, enc_size, fOut);
		}
		else	break;
	}

	FFE_Deinit(&ffeAudioHandle);



//==================================================================
//==================================================================
//视频编码
//当前仅支持H264编码
//==================================================================
//==================================================================
	int	width = 1920;
	int height = 1080;
	int fps = 25;
	int gop = 30;
	int bitrate = 15000000;
	int	intputformat = 3;		//3:rgb24  0:yv12

	FFE_HANDLE	ffeVideoHandle = NULL;		//声明
	FFE_Init(&ffeVideoHandle);	//初始化
	FFE_SetVideoEncodeParam(ffeVideoHandle, ENCODER_H264, width, height, fps, gop, bitrate, intputformat);		//设置编码参数

	char *enc_buf = new char[1920*1080];	//申请编码的内存空间
	int enc_size = 0;
	int ret = FFE_EncodeVideo(ffeVideoHandle, (unsigned char*)raw_buf, (unsigned char*)enc_buf, &enc_size, i+1);
	if (ret == 0x00 && enc_size>0)
	{
		if (NULL != fOut)	fwrite(enc_buf, 1, enc_size, fOut);
	}

	//编码结束,释放该句柄
	FFE_Deinit(&ffeVideoHandle);

	delete []enc_buf;
*/


#ifndef __FFENCODER_API_H__
#define __FFENCODER_API_H__



#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FFENCODER_API  __declspec(dllexport)
//=======================================================
//Encoder
#ifndef ENCODER_H264
#define ENCODER_H264			0x1C			//28
#endif

#ifndef ENCODER_AAC
#define ENCODER_AAC				0x15002			//86018
#endif



//=======================================================
typedef void *FFE_HANDLE;


//=======================================================
extern "C"
{
	int	FFENCODER_API	FFE_Init(FFE_HANDLE *_handle);
	int	FFENCODER_API	FFE_Deinit(FFE_HANDLE *_handle);

	int FFENCODER_API	FFE_SetVideoEncodeParam(FFE_HANDLE _handle, int _videoCodec, int _width, int _height, int _fps, int _gop, int _bitrate, int _inputFormat/*0:YV12  3:RGB24*/);
	int FFENCODER_API	FFE_SetAudioEncodeParam(FFE_HANDLE _handle, int _audioCodec, int _channel, unsigned int _sample_rate, int _bitrate);

	int FFENCODER_API	FFE_EncodeVideo(FFE_HANDLE _handle, unsigned char *rawbuf, unsigned char *encbuf, int *encsize, int pts, int rgbImageFlip=0x00);
	int	FFENCODER_API	FFE_EncodeAudio(FFE_HANDLE _handle, unsigned short *rawbuf, int rawsize, unsigned char *encbuf, int *encsize);


	//AAC编码
	int	FFENCODER_API	AAC_Init(FFE_HANDLE *_handle, int samplesPerSec, int channels);
	int	FFENCODER_API	AAC_Encode(FFE_HANDLE _handle, int *pcmData, int pcmDataSize, unsigned char **_outputAACData, int *_aacDataSize);
	int	FFENCODER_API	AAC_Deinit(FFE_HANDLE *_handle);
};


/*
	int codec = 28;		//H264
	int	fps = 25;
	int gop = 30;
	int bitrate = 1500000;
	int inputformat = 0;		/yuv420
	FFE_HANDLE	ffeHandle = NULL;
	FFE_Init(&ffeHandle);
	FFE_SetVideoEncodeParam(ffeHandle, codec, width, height, fps, gop, bitrate, inputformat);

	FILE *fOut = fopen("1.h264", "wb");

	int raw_buf_size = width*height*3+1;
	char *raw_buf = new char[raw_buf_size];
	char *enc_buf = new char[raw_buf_size/2];

	FILE *fIn = fopen("C:\\test\\352x288.yuv", "rb");
	if (NULL != fIn)
	{
		int pts = 0;
		while (! feof(fIn) )
		{
			fread(raw_buf, 1, width * height * 1.5, fIn);

			int enc_size = 0;
			int ret = FFE_EncodeVideo(ffeHandle, (unsigned char*)raw_buf, (unsigned char*)enc_buf, &enc_size, pts);
			if (ret == 0x00)
			{
				if (NULL != fOut)	fwrite(enc_buf, 1, enc_size, fOut);
			}

			pts += 3600;
		}
	}

	if (NULL != fOut)	fclose(fOut);
	delete []enc_buf;
	delete []raw_buf;

	FFE_Deinit(&ffeHandle);
*/



#endif
