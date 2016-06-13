// H264EncoderManager.cpp: implementation of the H264Encoder class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "H264Encoder.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CH264Encoder::CH264Encoder()
{
	m_hx264=NULL;
	m_bIsworking=false;
	m_nEncoderIndex=0;
	m_x264_picin=NULL;
}

CH264Encoder::~CH264Encoder()
{
	Clean();
}

//品质
int CH264Encoder::Init(int width,int height,int fps,int keyframe,int bitrate,int level,int qp,int nUseQP)
{
	if (keyframe<5)
	{
		keyframe=10;
	}
	if (bitrate<100)
	{
		bitrate=1000;
	}
	if(qp<=0||qp>51)
		qp=26;
	if (fps<10)
	{
		fps=15;
	}
	Clean();

	m_nwidth=width;
	m_nheight=height;
	m_ncheckyuvsize=m_nwidth*m_nheight*3/2;
	x264_param_default(&m_x264_param);
	CH264Encoder::X264_CONFIG_SET(&m_x264_param,level);

	m_x264_param.i_width=m_nwidth;
	m_x264_param.i_height=m_nheight;
	// 增加输入色彩空间格式设置 [1/25/2016 Administrator]
	//m_x264_param.i_csp = X264_CSP_I420;//X264_CSP_RGB;
	

	m_x264_param.i_log_level=X264_LOG_NONE;
	m_x264_param.i_fps_num=fps;
	m_x264_param.i_fps_den = 1;
 	m_x264_param.i_keyint_min=X264_KEYINT_MIN_AUTO;
 	m_x264_param.i_keyint_max=keyframe;
 
 	m_x264_param.b_annexb=0;
 	m_x264_param.b_repeat_headers=0;

 //	m_x264_param.i_nal_hrd=X264_NAL_HRD_NONE;
 	
 	m_x264_param.i_timebase_num=m_x264_param.i_fps_den;
 	m_x264_param.i_timebase_den=m_x264_param.i_fps_num;
 	
	if(nUseQP==1)
	{
		if(qp>0&&qp<=51)
		{
			m_x264_param.rc.i_rc_method=X264_RC_CQP;//恒定质量	
			m_x264_param.rc.i_qp_constant=qp;
		}
	}
	else
	{
		if (bitrate>0)
		{
			if(nUseQP==0)
				m_x264_param.rc.i_rc_method=X264_RC_ABR;////平均码率//恒定码率
			else if(nUseQP==2)
				m_x264_param.rc.i_rc_method=X264_RC_CRF;//码率动态

			m_x264_param.rc.i_bitrate=bitrate;//单位kbps
			m_x264_param.rc.i_vbv_max_bitrate=bitrate;
		}
	}



 ////////////////////////////////////
//     m_x264_param.i_threads = 1;
//     m_x264_param.i_lookahead_threads = 1;
//     m_x264_param.b_deterministic = 1;
//     m_x264_param.i_sync_lookahead = 1;


	m_x264_param.analyse.intra = X264_ANALYSE_I4x4 ;//| X264_ANALYSE_I8x8 | X264_ANALYSE_PSUB8x8;
	m_x264_param.analyse.inter = X264_ANALYSE_I4x4 ;//| X264_ANALYSE_I8x8 | X264_ANALYSE_PSUB8x8;
//	m_x264_param.analyse.i_me_range = 24;//越大越好
	m_x264_param.analyse.b_transform_8x8 = 0;
    m_x264_param.i_cqm_preset = X264_CQM_FLAT;
	m_x264_param.analyse.i_me_method = X264_ME_UMH;
	m_x264_param.b_cabac=0;
	m_x264_param.rc.b_mb_tree = 0;//实时编码为0
	m_x264_param.rc.b_stat_write=0;   /* Enable stat writing in psz_stat_out */
	m_x264_param.rc.b_stat_read=0;    /* Read stat from psz_stat_in and use it */


/////////////////////////////////////////////
	m_hx264=x264_encoder_open(&m_x264_param);
	m_x264_picin=(x264_picture_t*)malloc(sizeof(x264_picture_t));
	//X264 Encoder just support I420 CSP
	int nRet = x264_picture_alloc(m_x264_picin,X264_CSP_I420/*X264_CSP_I422*/,m_nwidth,m_nheight);
	m_bIsworking=true;
	return 0;
}

int CH264Encoder::Clean()
{
	if (m_hx264!=NULL)
	{
		m_bIsworking=false;
		int m_inal=0;		
		x264_encoder_encode(m_hx264,&m_x264_nal,&m_inal,NULL,&m_x264_picout);
		x264_encoder_close(m_hx264);
		m_hx264=NULL;
		if (m_x264_picin!=NULL)
		{
			free(m_x264_picin);
			m_x264_picin=NULL;
		}		
	}
	return 0;
}

unsigned char* CH264Encoder::Encoder(unsigned char *indata, int inlen, int &outlen, bool &bIsKeyFrame)
{
	if (m_bIsworking&&(inlen==m_ncheckyuvsize))
	{	
		int m_inal=0;
		memcpy(m_x264_picin->img.plane[0],indata,inlen);
		outlen=x264_encoder_encode(m_hx264,&m_x264_nal,&m_inal,m_x264_picin,&m_x264_picout);
		m_x264_picin->i_pts++;
	}else
	{
		outlen=-1;
	}
	if (outlen>0)
	{
		bIsKeyFrame=m_x264_picout.b_keyframe==1;
		return m_x264_nal[0].p_payload;
	}else
	{
		return NULL;
	}
}

void CH264Encoder::X264_CONFIG_SET(x264_param_t*param,int mode)
{
	switch (mode)
	{
	case ZH264_DEFAULT:
	case ZH264_MEDIUM:
	case ZH264_ULTRAFAST:
		{
			param->i_frame_reference = 1;
			param->i_scenecut_threshold = 0;
			param->b_deblocking_filter = 0;
			param->b_cabac = 0;
			param->i_bframe = 0;
			param->analyse.intra = 0;
			param->analyse.inter = 0;
			param->analyse.b_transform_8x8 = 0;
			param->analyse.i_me_method = X264_ME_DIA;
			param->analyse.i_subpel_refine = 0;
			param->rc.i_aq_mode = 0;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->i_bframe_adaptive = X264_B_ADAPT_NONE;
			param->rc.b_mb_tree = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;
			param->analyse.b_weighted_bipred = 0;
			param->rc.i_lookahead = 0;
		}
		break;
	case ZH264_SUPERFAST:
		{
			param->analyse.inter = X264_ANALYSE_I8x8|X264_ANALYSE_I4x4;
			param->analyse.i_me_method = X264_ME_DIA;
			param->analyse.i_subpel_refine = 1;
			param->i_frame_reference = 1;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->rc.b_mb_tree = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 0;
		}
		break;
	case ZH264_VERYFAST:
		{
			param->analyse.i_me_method = X264_ME_HEX;
			param->analyse.i_subpel_refine = 2;
			param->i_frame_reference = 1;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 10;
		}
		break;
	case ZH264_FASTER:
		{
			param->analyse.b_mixed_references = 0;
			param->i_frame_reference = 2;
			param->analyse.i_subpel_refine = 4;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 20;			
		}
		break;
	case ZH264_FAST:
		{
			param->i_frame_reference = 2;
			param->analyse.i_subpel_refine = 6;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 30;
		}
		break;
	case ZH264_SLOW:
		{
			param->analyse.i_me_method = X264_ME_UMH;
			param->analyse.i_subpel_refine = 8;
			param->i_frame_reference = 5;
			param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
			param->analyse.i_direct_mv_pred = X264_DIRECT_PRED_AUTO;
			param->rc.i_lookahead = 50;
		}
		break;
	case ZH264_SLOWER:
		{
			param->analyse.i_me_method = X264_ME_UMH;
			param->analyse.i_subpel_refine = 9;
			param->i_frame_reference = 8;
			param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
			param->analyse.i_direct_mv_pred = X264_DIRECT_PRED_AUTO;
			param->analyse.inter |= X264_ANALYSE_PSUB8x8;
			param->analyse.i_trellis = 2;
			param->rc.i_lookahead = 60;
		}
		break;
	case ZH264_VERYSLOW:
		{
			param->analyse.i_me_method = X264_ME_UMH;
			param->analyse.i_subpel_refine = 10;
			param->analyse.i_me_range = 24;
			param->i_frame_reference = 16;
			param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
			param->analyse.i_direct_mv_pred = X264_DIRECT_PRED_AUTO;
			param->analyse.inter |= X264_ANALYSE_PSUB8x8;
			param->analyse.i_trellis = 2;
			param->i_bframe = 8;
			param->rc.i_lookahead = 60;
		}
		break;
	}
}

bool CH264Encoder::IsWorking(void)
{
	return m_hx264!=NULL;
}
void CH264Encoder::GetSPSAndPPS(unsigned char*sps,long&spslen,unsigned char*pps,long&ppslen)
{
	if (m_hx264!=NULL)
	{
		int i_nal=0;
		x264_nal_t*tnal=NULL;
		x264_encoder_headers(m_hx264,&tnal,&i_nal);
		spslen=tnal[0].i_payload-4;
		memcpy(sps,tnal[0].p_payload+4,spslen);
		ppslen=tnal[1].i_payload-4;
		memcpy(pps,tnal[1].p_payload+4,ppslen);
	}else
	{
		spslen=0;
		ppslen=0;
	}
}

void CH264Encoder::SetEncoderIndex(int nIndex)
{
	m_nEncoderIndex=nIndex;
}