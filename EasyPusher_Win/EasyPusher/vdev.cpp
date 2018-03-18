// 包含头文件
// 
#include "StdAfx.h"

#include "vdev.h"

bool vdev_convert(AVPixelFormat eInFormat, int iInWidth, int iInHeight, void* ptInData,
	AVPixelFormat eOutFormat, int iOutWidth, int iOutHeight, unsigned char** pOutData)
{

	// init ffmpeg
	//av_register_all();

	SwsContext* ptImgConvertCtx;    // Frame conversion context

	AVFrame video;
	int numBytesIn;
	//numBytesIn = av_image_get_buffer_size(eOutFormat, iOutWidth, iOutHeight, 1);

	//	uint8_t* out_buffer = (uint8_t *)av_malloc(numBytesIn * sizeof(uint8_t));

	av_image_fill_arrays(video.data, video.linesize, *pOutData, eOutFormat, iOutWidth, iOutHeight, 1);
	video.width = iOutWidth;
	video.height = iOutHeight;
	video.format = eOutFormat;

	AVFrame ptPictureIn= {};
	ptPictureIn.format = eInFormat;
	ptPictureIn.width  = iInWidth > 0 ? iInWidth : video.width;
	ptPictureIn.height = iInHeight > 0 ? iInHeight : video.height;
	// 	if (av_frame_get_buffer(&ptPictureIn, 32) < 0) {
	// 		//av_log(NULL, AV_LOG_ERROR, "failed to allocate picture !\n", file);
	// 	}
	av_image_fill_arrays(ptPictureIn.data, ptPictureIn.linesize, (const uint8_t*)ptInData, eInFormat, iInWidth, iInHeight, 1);


	//Initialize convert context
	//------------------
	ptImgConvertCtx = sws_getContext(iInWidth, iInHeight, eInFormat,     // (source format)
		iOutWidth, iOutHeight, eOutFormat,  // (dest format)
		SWS_FAST_BILINEAR, NULL, NULL, NULL);


	// Do conversion:
	//------------------
	int iRes = sws_scale(ptImgConvertCtx,
		ptPictureIn.data, //src
		ptPictureIn.linesize,
		0,
		iInHeight,
		video.data,//dst
		video.linesize);

	//Free memory
	sws_freeContext(ptImgConvertCtx);

	//Check result:
	if (iRes == iOutHeight)
		return true;

	return false;
}


