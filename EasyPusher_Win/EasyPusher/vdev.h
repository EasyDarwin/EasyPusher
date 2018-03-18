#ifndef __FFPLAYER_VDEV_H__
#define __FFPLAYER_VDEV_H__

// 包含头文件
#include <stdint.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	//ffmpeg support
#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

	// 预编译开关
#define CLEAR_VDEV_WHEN_DESTROYED  TRUE

	// 常量定义
#define VDEV_CLOSE      (1 << 0)
#define VDEV_PAUSE      (1 << 1)
#define VDEV_COMPLETED  (1 << 2)

	// vdev render type
	enum {
		VDEV_RENDER_TYPE_GDI,
		VDEV_RENDER_TYPE_D3D,
		VDEV_RENDER_TYPE_MAX_NUM,
	};

	enum {
		VDEV_PIX_FMT_RGB24,
		VDEV_PIX_FMT_RGB32,
		VDEV_PIX_FMT_YUY2,
		VDEV_PIX_FMT_UYVY,
	};


	//++ vdev context common members
#define VDEV_COMMON_MEMBERS \
	int      type;   \
	int      bufnum; \
	int      pixfmt; \
	int      x;   /* video display rect x */ \
	int      y;   /* video display rect y */ \
	int      w;   /* video display rect w */ \
	int      h;   /* video display rect h */ \
	int      sw;  /* surface width        */ \
	int      sh;  /* surface height       */ \
	\
	HWND     hwnd;                           \
	int64_t *ppts;                           \
	int64_t  apts;                           \
	int64_t  vpts;                           \
	\
	int      head;                           \
	int      tail;                           \
	HANDLE   semr;                           \
	HANDLE   semw;                           \
	\
	int		 framerate;						 \
	int      tickavdiff;                     \
	int      tickframe;                      \
	int      ticksleep;                      \
	int      ticklast;                       \
	\
	int      status;                         \
	HANDLE   thread;                         \
	\
	int      completed_counter;              \
	int64_t  completed_apts;                 \
	int64_t  completed_vpts;                 \
	int      refresh_flag;                   \
	int		 pause;	 /*视频暂停标识*/	     \
	int		 step;	 /*视频单步标识*/	     \
	int		 veffect_type;					 \
	int		 play_speed;/*单视频播放速度调节*/\
	//-- vdev context common members

	// 类型定义
	typedef struct {
		VDEV_COMMON_MEMBERS
	} VDEV_COMMON_CTXT;

	bool vdev_convert(AVPixelFormat eInFormat, int iInWidth, int iInHeight, void* ptInData,
		AVPixelFormat eOutFormat, int iOutWidth, int iOutHeight, unsigned char** pOutData);


#ifdef __cplusplus
}
#endif

#endif
