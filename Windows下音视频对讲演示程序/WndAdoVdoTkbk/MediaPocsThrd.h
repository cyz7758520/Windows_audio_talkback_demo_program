﻿#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "SpeexDsp.h"
#if IsIcludWebRtc
#include "WebRtc.h"
#endif
#if IsIcludWebRtc3
#include "WebRtc3.h"
#endif
#if IsIcludSpeexWebRtcAec
#include "SpeexWebRtcAec.h"
#endif
#if IsIcludRNNoise
#include "RNNoise.h"
#endif
#if IsIcludSpeex
#include "Speex.h"
#endif
#if IsIcludAdoWavfm
#include "AdoWavfm.h"
#endif
#include "LibYUV.h"
#if IsIcludOpenH264
#include "OpenH264.h"
#endif
#if IsIcludMediaFile
#include "MediaFile.h"
#endif

#include "AdoInpt.h"
#include "AdoOtpt.h"
#include "VdoInpt.h"
#include "VdoOtpt.h"

#include "WndAdoVdoTkbk.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct IGraphBuilder IGraphBuilder;
typedef struct IMediaControl IMediaControl;
typedef struct IMediaEventEx IMediaEventEx;
typedef struct IBaseFilter IBaseFilter;
typedef struct VdoInptThrd VdoInptThrd;
typedef struct MediaPocsThrd MediaPocsThrd;
typedef struct AviFileWriter AviFileWriter;

typedef struct MediaPocsThrd //媒体处理线程。
{
	typedef enum RunFlag //运行标记。
	{
		RunFlagNorun, //未开始运行。
		RunFlagRun, //正在运行。
		RunFlagExit, //已经退出。
	} RunFlag;
	RunFlag m_RunFlag; //存放本线程运行标记。

	int32_t m_LastCallUserInitOrDstoy; //存放上一次调用了用户定义的初始化函数或销毁函数，为0表示初始化函数，为1表示销毁函数。

	typedef enum ThrdMsgTyp //线程消息。
	{
		ThrdMsgTypSetAdoInpt,
		ThrdMsgTypAdoInptSetIsUseSystemAecNsAgc,
		ThrdMsgTypAdoInptSetUseNoAec,
		ThrdMsgTypAdoInptSetUseSpeexAec,
		ThrdMsgTypAdoInptSetUseWebRtcAecm,
		ThrdMsgTypAdoInptSetUseWebRtcAec,
		ThrdMsgTypAdoInptSetUseWebRtcAec3,
		ThrdMsgTypAdoInptSetUseSpeexWebRtcAec,
		ThrdMsgTypAdoInptSetUseNoNs,
		ThrdMsgTypAdoInptSetUseSpeexPrpocsNs,
		ThrdMsgTypAdoInptSetUseWebRtcNsx,
		ThrdMsgTypAdoInptSetUseWebRtcNs,
		ThrdMsgTypAdoInptSetUseRNNoise,
		ThrdMsgTypAdoInptSetIsUseSpeexPrpocs,
		ThrdMsgTypAdoInptSetUsePcm,
		ThrdMsgTypAdoInptSetUseSpeexEncd,
		ThrdMsgTypAdoInptSetUseOpusEncd,
		ThrdMsgTypAdoInptSetIsSaveAdoToWaveFile,
		ThrdMsgTypAdoInptSetIsDrawAdoWavfmToWnd,
		ThrdMsgTypAdoInptSetUseDvc,
		ThrdMsgTypAdoInptSetIsMute,

		ThrdMsgTypSetAdoOtpt,
		ThrdMsgTypAdoOtptAddStrm,
		ThrdMsgTypAdoOtptDelStrm,
		ThrdMsgTypAdoOtptSetStrmUsePcm,
		ThrdMsgTypAdoOtptSetStrmUseSpeexDecd,
		ThrdMsgTypAdoOtptSetStrmUseOpusDecd,
		ThrdMsgTypAdoOtptSetStrmIsUse,
		ThrdMsgTypAdoOtptSetIsSaveAdoToWaveFile,
		ThrdMsgTypAdoOtptSetIsDrawAdoWavfmToWnd,
		ThrdMsgTypAdoOtptSetUseDvc,
		ThrdMsgTypAdoOtptSetIsMute,

		ThrdMsgTypSetVdoInpt,
		ThrdMsgTypVdoInptSetUseYu12,
		ThrdMsgTypVdoInptSetUseOpenH264Encd,
		ThrdMsgTypVdoInptSetUseDvc,
		ThrdMsgTypVdoInptSetIsBlack,

		ThrdMsgTypVdoOtptAddStrm,
		ThrdMsgTypVdoOtptDelStrm,
		ThrdMsgTypVdoOtptSetStrm,
		ThrdMsgTypVdoOtptSetStrmUseYu12,
		ThrdMsgTypVdoOtptSetStrmUseOpenH264Decd,
		ThrdMsgTypVdoOtptSetStrmIsBlack,
		ThrdMsgTypVdoOtptSetStrmIsUse,

		ThrdMsgTypSetIsUseAdoVdoInptOtpt,

		ThrdMsgTypSetIsUsePrvntSysSleep,
		ThrdMsgTypSetIsSaveAdoVdoInptOtptToAviFile,
		ThrdMsgTypSaveStsToTxtFile,

		ThrdMsgTypRqirExit,

		ThrdMsgTypUserInit,
		ThrdMsgTypUserDstoy,

		ThrdMsgTypAdoVdoInptOtptInit,
		ThrdMsgTypAdoVdoInptOtptDstoy,

		ThrdMsgTypAdoInptDvcChg,
		ThrdMsgTypAdoInptDvcClos,
		ThrdMsgTypAdoOtptDvcChg,
		ThrdMsgTypAdoOtptDvcClos,
		ThrdMsgTypVdoInptDvcChg,
		ThrdMsgTypVdoInptDvcClos,

		ThrdMsgTypUserMsgMinVal = 100, //用户消息的最小值。
	} ThrdMsgTyp;
	typedef struct
	{
		int32_t m_SmplRate;
		size_t m_FrmLenMsec;
	} ThrdMsgSetAdoInpt;
	typedef struct
	{
		int32_t m_IsUseSystemAecNsAgc;
	} ThrdMsgAdoInptSetIsUseSystemAecNsAgc;
	typedef struct
	{
	} ThrdMsgAdoInptSetUseNoAec;
	typedef struct
	{
		int32_t m_FilterLenMsec;
		int32_t m_IsUseRec;
		float m_EchoMutp;
		float m_EchoCntu;
		int32_t m_EchoSupes;
		int32_t m_EchoSupesAct;
	} ThrdMsgAdoInptSetUseSpeexAec;
	typedef struct
	{
		int32_t m_IsUseCNGMode;
		int32_t m_EchoMode;
		int32_t m_Delay;
	} ThrdMsgAdoInptSetUseWebRtcAecm;
	typedef struct
	{
		int32_t m_EchoMode;
		int32_t m_Delay;
		int32_t m_IsUseDelayAgstcMode;
		int32_t m_IsUseExtdFilterMode;
		int32_t m_IsUseRefinedFilterAdaptAecMode;
		int32_t m_IsUseAdaptAdjDelay;
	} ThrdMsgAdoInptSetUseWebRtcAec;
	typedef struct
	{
		int32_t m_Delay;
	} ThrdMsgAdoInptSetUseWebRtcAec3;
	typedef struct
	{
		int32_t m_WorkMode;
		int32_t m_SpeexAecFilterLenMsec;
		int32_t m_SpeexAecIsUseRec;
		float m_SpeexAecEchoMutp;
		float m_SpeexAecEchoCntu;
		int32_t m_SpeexAecEchoSupes;
		int32_t m_SpeexAecEchoSupesAct;
		int32_t m_WebRtcAecmIsUseCNGMode;
		int32_t m_WebRtcAecmEchoMode;
		int32_t m_WebRtcAecmDelay;
		int32_t m_WebRtcAecEchoMode;
		int32_t m_WebRtcAecDelay;
		int32_t m_WebRtcAecIsUseDelayAgstcMode;
		int32_t m_WebRtcAecIsUseExtdFilterMode;
		int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode;
		int32_t m_WebRtcAecIsUseAdaptAdjDelay;
		int32_t m_WebRtcAec3Delay;
		int32_t m_IsUseSameRoomAec;
		int32_t m_SameRoomEchoMinDelay;
	} ThrdMsgAdoInptSetUseSpeexWebRtcAec;
	typedef struct
	{
	} ThrdMsgAdoInptSetUseNoNs;
	typedef struct
	{
		int32_t m_IsUseNs;
		int32_t m_NoiseSupes;
		int32_t m_IsUseDereverb;
	} ThrdMsgAdoInptSetUseSpeexPrpocsNs;
	typedef struct
	{
		int32_t m_PolicyMode;
	} ThrdMsgAdoInptSetUseWebRtcNsx;
	typedef struct
	{
		int32_t m_PolicyMode;
	} ThrdMsgAdoInptSetUseWebRtcNs;
	typedef struct
	{
	} ThrdMsgAdoInptSetUseRNNoise;
	typedef struct
	{
		int32_t m_IsUseSpeexPrpocs;
		int32_t m_IsUseVad;
		int32_t m_VadProbStart;
		int32_t m_VadProbCntu;
		int32_t m_IsUseAgc;
		int32_t m_AgcLevel;
		int32_t m_AgcIncrement;
		int32_t m_AgcDecrement;
		int32_t m_AgcMaxGain;
	} ThrdMsgAdoInptSetIsUseSpeexPrpocs;
	typedef struct
	{
	} ThrdMsgAdoInptSetUsePcm;
	typedef struct
	{
		int32_t m_UseCbrOrVbr;
		int32_t m_Qualt;
		int32_t m_Cmplxt;
		int32_t m_PlcExptLossRate;
	} ThrdMsgAdoInptSetUseSpeexEncd;
	typedef struct
	{
	} ThrdMsgAdoInptSetUseOpusEncd;
	typedef struct
	{
		int32_t m_IsSave;
		Vstr * m_SrcFullPathVstrPt;
		Vstr * m_RsltFullPathVstrPt;
		size_t m_WrBufSzByt;
	} ThrdMsgAdoInptSetIsSaveAdoToWaveFile;
	typedef struct
	{
		int32_t m_IsDraw;
		HWND m_SrcWndHdl;
		HWND m_RsltWndHdl;
	} ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd;
	typedef struct
	{
		Vstr * m_NameVstrPt;
	} ThrdMsgAdoInptSetUseDvc;
	typedef struct
	{
		int32_t m_IsMute;
	} ThrdMsgAdoInptSetIsMute;
	typedef struct
	{
		int32_t m_SmplRate;
		size_t m_FrmLenMsec;
	} ThrdMsgSetAdoOtpt;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgAdoOtptAddStrm;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgAdoOtptDelStrm;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgAdoOtptSetStrmUsePcm;
	typedef struct
	{
		int32_t m_StrmIdx;
		int32_t m_IsUsePrcplEnhsmt;
	} ThrdMsgAdoOtptSetStrmUseSpeexDecd;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgAdoOtptSetStrmUseOpusDecd;
	typedef struct
	{
		int32_t m_StrmIdx;
		int32_t m_IsUse;
	} ThrdMsgAdoOtptSetStrmIsUse;
	typedef struct
	{
		int32_t m_IsSave;
		Vstr * m_SrcFullPathVstrPt;
		size_t m_WrBufSzByt;
	} ThrdMsgAdoOtptSetIsSaveAdoToWaveFile;
	typedef struct
	{
		int32_t m_IsDraw;
		HWND m_SrcWndHdl;
	} ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd;
	typedef struct
	{
		Vstr * m_NameVstrPt;
	} ThrdMsgAdoOtptSetUseDvc;
	typedef struct
	{
		int32_t m_IsMute;
	} ThrdMsgAdoOtptSetIsMute;
	typedef struct
	{
		int32_t m_MaxSmplRate;
		int32_t m_FrmWidth;
		int32_t m_FrmHeight;
		int32_t m_SrcMaxSmplRate;
		int32_t m_SrcFrmWidth;
		int32_t m_SrcFrmHeight;
		HWND m_PrvwWndHdl;
	} ThrdMsgSetVdoInpt;
	typedef struct
	{
	} ThrdMsgVdoInptSetUseYu12;
	typedef struct
	{
		int32_t m_VdoType;
		int32_t m_EncdBitrate;
		int32_t m_BitrateCtrlMode;
		int32_t m_IDRFrmIntvl;
		int32_t m_Cmplxt;
	} ThrdMsgVdoInptSetUseOpenH264Encd;
	typedef struct
	{
		Vstr * m_NameVstrPt;
	} ThrdMsgVdoInptSetUseDvc;
	typedef struct
	{
		int32_t m_IsBlack;
	} ThrdMsgSetVdoInptIsBlack;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgAddVdoOtptStrm;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgDelVdoOtptStrm;
	typedef struct
	{
		int32_t m_StrmIdx;
		HWND m_DspyWndHdl;
	} ThrdMsgSetVdoOtptStrm;
	typedef struct
	{
		int32_t m_StrmIdx;
	} ThrdMsgSetVdoOtptStrmUseYu12;
	typedef struct
	{
		int32_t m_StrmIdx;
		int32_t m_DecdThrdNum;
	} ThrdMsgSetVdoOtptStrmUseOpenH264Decd;
	typedef struct
	{
		int32_t m_StrmIdx;
		int32_t m_IsBlack;
	} ThrdMsgSetVdoOtptStrmIsBlack;
	typedef struct
	{
		int32_t m_StrmIdx;
		int32_t m_IsUse;
	} ThrdMsgSetVdoOtptStrmIsUse;
	typedef struct
	{
		int32_t m_IsUsePrvntSysSleep;
	} ThrdMsgSetIsUsePrvntSysSleep;
	typedef struct
	{
		Vstr * m_FullPathVstrPt;
		size_t m_WrBufSzByt;
		int32_t m_MaxStrmNum;
		int32_t m_IsSaveAdoInpt;
		int32_t m_IsSaveAdoOtpt;
		int32_t m_IsSaveVdoInpt;
		int32_t m_IsSaveVdoOtpt;
	} ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile;
	typedef struct
	{
		Vstr * m_FullPathVstrPt;
	} ThrdMsgSaveStsToTxtFile;
	typedef struct
	{
		int32_t m_IsUseAdoInpt;
		int32_t m_IsUseAdoOtpt;
		int32_t m_IsUseVdoInpt;
		int32_t m_IsUseVdoOtpt;
	} ThrdMsgSetIsUseAdoVdoInptOtpt;
	typedef struct
	{
		int32_t m_ExitFlag;
	} ThrdMsgRqirExit;
	typedef struct
	{
	} ThrdMsgUserInit;
	typedef struct
	{
	} ThrdMsgUserDstoy;
	typedef struct
	{
		uint8_t m_UserMsgArr[];
	} ThrdMsgUserMsg;
	typedef struct
	{
	} ThrdMsgAdoVdoInptOtptInit;
	typedef struct
	{
	} ThrdMsgAdoVdoInptOtptDstoy;
	typedef struct
	{
	} ThrdMsgAdoInptDvcChg;
	typedef struct
	{
	} ThrdMsgAdoInptDvcClos;
	typedef struct
	{
	} ThrdMsgAdoOtptDvcChg;
	typedef struct
	{
	} ThrdMsgAdoOtptDvcClos;
	typedef struct
	{
	} ThrdMsgVdoInptDvcChg;
	typedef struct
	{
	} ThrdMsgVdoInptDvcClos;
	MsgQueue * m_ThrdMsgQueuePt; //存放线程消息队列的指针。

	const void * m_LicnCodePt; //存放授权码的指针。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。
	HWND m_ShowToastWndHdl; //存放显示Toast窗口的句柄，为NULL表示桌面窗口。

	int32_t m_IsUsePrvntSysSleep; //存放是否使用阻止系统睡眠，为非0表示要使用，为0表示不使用。
	
	struct //存放音视频输入输出Avi文件。
	{
		AviFileWriter * m_WriterPt; //存放写入器的指针。
		Vstr * m_FullPathVstrPt; //存放完整路径动态字符串的指针。
		size_t m_WrBufSzByt; //存放写入缓冲区大小，单位为字节。
		int32_t m_MaxStrmNum; //存放最大流数量，取值区间为[1,100]。
		int32_t m_IsSaveAdoInpt; //存放是否保存音频输入，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveAdoOtpt; //存放是否保存音频输出，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveVdoInpt; //存放是否保存视频输入，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveVdoOtpt; //存放是否保存视频输出，为非0表示要保存，为0表示不保存。
		uint32_t m_AdoInptPcmSrcStrmIdx; //存放音频输入Pcm格式原始流索引。
		uint32_t m_AdoInptPcmRsltStrmIdx; //存放音频输入Pcm格式结果流索引。
		int32_t m_AdoInptStrmTimeStampIsReset; //存放音频输入流时间戳是否重置，为非0表示要重置，为0表示不重置。
		uint32_t m_AdoOtptPcmSrcStrmIdx; //存放音频输出Pcm格式原始流索引。
		int32_t m_AdoOtptStrmTimeStampIsReset; //存放音频输出流时间戳是否重置，为非0表示要重置，为0表示不重置。
		uint32_t m_VdoInptEncdRsltStrmIdx; //存放视频输入已编码格式结果流索引。
		std::map< uint32_t, uint32_t > * m_VdoOtptEncdSrcStrmIdxMapPt; //存放视频输出已编码格式原始流索引映射指针。
	} m_AdoVdoInptOtptAviFile;

	AdoInpt m_AdoInpt; //存放音频输入。
	AdoOtpt m_AdoOtpt; //存放音频输出。
	VdoInpt m_VdoInpt; //存放视频输入。
	VdoOtpt m_VdoOtpt; //存放视频输出。
	
	struct //存放线程。
	{
		int32_t m_IsInitThrdTmpVar; //存放是否初始化线程临时变量。
		int16_t * m_AdoInptPcmSrcFrmPt; //存放音频输入Pcm格式原始帧的指针。
		int16_t * m_AdoInptPcmRsltFrmPt; //存放音频输入Pcm格式结果帧的指针。
		int16_t * m_AdoInptPcmTmpFrmPt; //存放音频输入Pcm格式临时帧的指针。
		int16_t * m_AdoOtptPcmSrcFrmPt; //存放音频输出Pcm格式原始帧的指针。
		int32_t m_AdoInptPcmRsltFrmVoiceActSts; //存放音频输入Pcm格式结果帧语音活动状态，为非0表示有语音活动，为0表示无语音活动。
		uint8_t * m_AdoInptEncdRsltFrmPt; //存放音频输入已编码格式结果帧的指针，大小为 m_AdoInpt.m_FrmLenByt 字节。
		size_t m_AdoInptEncdRsltFrmLenByt; //存放音频输入已编码格式结果帧的长度，单位为字节。
		int32_t m_AdoInptEncdRsltFrmIsNeedTrans; //存放音频输入已编码格式结果帧是否需要传输，为非0表示需要传输，为0表示不要传输。
		VdoInpt::Frm * m_VdoInptFrmPt; //存放视频输入帧的指针。
		VdoOtpt::Frm * m_VdoOtptFrmPt; //存放视频输出帧的指针。

		ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
	} m_Thrd;

	Vstr * m_ErrInfoVstrPt; //存放错误信息动态字符串的指针。

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的MediaPocsThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;

	//用户定义的初始化函数。
	typedef void( __cdecl * MediaPocsThrdUserInitFuncPt )( MediaPocsThrd * MediaPocsThrdPt );
	MediaPocsThrdUserInitFuncPt m_UserInitFuncPt;

	//用户定义的销毁函数。
	typedef void( __cdecl * MediaPocsThrdUserDstoyFuncPt )( MediaPocsThrd * MediaPocsThrdPt );
	MediaPocsThrdUserDstoyFuncPt m_UserDstoyFuncPt;

	//用户定义的处理函数。
	typedef void( __cdecl * MediaPocsThrdUserPocsFuncPt )( MediaPocsThrd * MediaPocsThrdPt );
	MediaPocsThrdUserPocsFuncPt m_UserPocsFuncPt;

	//用户定义的消息函数。
	typedef int( __cdecl * MediaPocsThrdUserMsgFuncPt )( MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt );
	MediaPocsThrdUserMsgFuncPt m_UserMsgFuncPt;

	//用户定义的设备改变函数。
	typedef void( __cdecl * MediaPocsThrdUserDvcChgFuncPt )( MediaPocsThrd * MediaPocsThrdPt, Vstr * AdoInptDvcNameVstrPt, Vstr * AdoOtptDvcNameVstrPt, Vstr * VdoInptDvcNameVstrPt );
	MediaPocsThrdUserDvcChgFuncPt m_UserDvcChgFuncPt;

	//用户定义的读取音视频输入帧函数。
	typedef void( __cdecl * MediaPocsThrdUserReadAdoVdoInptFrmFuncPt )( MediaPocsThrd * MediaPocsThrdPt,
																		int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
																		uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
																		uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
																		uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
																		uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );
	MediaPocsThrdUserReadAdoVdoInptFrmFuncPt m_UserReadAdoVdoInptFrmFuncPt;

	//用户定义的写入音频输出帧函数。
	typedef void( __cdecl * MediaPocsThrdUserWriteAdoOtptFrmFuncPt )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
																	  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
																	  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );
	MediaPocsThrdUserWriteAdoOtptFrmFuncPt m_UserWriteAdoOtptFrmFuncPt;

	//用户定义的获取音频输出帧函数。
	typedef void( __cdecl * MediaPocsThrdUserGetAdoOtptFrmFuncPt )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
																	int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
																	uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );
	MediaPocsThrdUserGetAdoOtptFrmFuncPt m_UserGetAdoOtptFrmFuncPt;

	//用户定义的写入视频输出帧函数。
	typedef void( __cdecl * MediaPocsThrdUserWriteVdoOtptFrmFuncPt )( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
																	  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
																	  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );
	MediaPocsThrdUserWriteVdoOtptFrmFuncPt m_UserWriteVdoOtptFrmFuncPt;

	//用户定义的获取视频输出帧函数。
	typedef void( __cdecl * MediaPocsThrdUserGetVdoOtptFrmFuncPt )( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
																	uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
																	uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );
	MediaPocsThrdUserGetVdoOtptFrmFuncPt m_UserGetVdoOtptFrmFuncPt;
} MediaPocsThrd;

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, const void * LicnCodePt, void * UserDataPt,
												MediaPocsThrd::MediaPocsThrdUserInitFuncPt UserInitFuncPt, MediaPocsThrd::MediaPocsThrdUserDstoyFuncPt UserDstoyFuncPt,
												MediaPocsThrd::MediaPocsThrdUserPocsFuncPt UserPocsFuncPt, MediaPocsThrd::MediaPocsThrdUserMsgFuncPt UserMsgFuncPt, MediaPocsThrd::MediaPocsThrdUserDvcChgFuncPt UserDvcChgFuncPt,
												MediaPocsThrd::MediaPocsThrdUserReadAdoVdoInptFrmFuncPt UserReadAdoVdoInptFrmFuncPt,
												MediaPocsThrd::MediaPocsThrdUserWriteAdoOtptFrmFuncPt UserWriteAdoOtptFrmFuncPt, MediaPocsThrd::MediaPocsThrdUserGetAdoOtptFrmFuncPt UserGetAdoOtptFrmFuncPt,
												MediaPocsThrd::MediaPocsThrdUserWriteVdoOtptFrmFuncPt UserWriteVdoOtptFrmFuncPt, MediaPocsThrd::MediaPocsThrdUserGetVdoOtptFrmFuncPt UserGetVdoOtptFrmFuncPt,
												Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdDstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetAdoInptDvcId( Vstr * AdoInptDvcNameVstrPt, UINT * AdoInptDvcIdPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetAdoOtptDvcId( Vstr * AdoOtptDvcNameVstrPt, UINT * AdoOtptDvcIdPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdGetVdoInptDvcId( Vstr * VdoInptDvcNameVstrPt, UINT * VdoInptDvcIdPt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseSystemAecNsAgc, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseNoAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseWebRtcAec3( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t Delay, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t WebRtcAec3Delay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseNoNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUsePcm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, const Vstr * RsltFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsDrawAdoWavfmToWnd, HWND SrcWndHdl, HWND RsltWndHdl, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoInptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int AddFirstOrLast, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsDrawAdoWavfmToWnd, HWND SrcWndHdl, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdAdoOtptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, int32_t SrcMaxSmplRate, int32_t SrcFrmWidth, int32_t SrcFrmHeight, HWND PrvwWndHdl, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoInptSetUseYu12( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoInptSetUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoInptSetIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int IsBlack, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int AddFirstOrLast, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptSetStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, HWND DspyWndHdl, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptSetStrmUseYu12( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptSetStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdVdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetIsUseAdoVdoInptOtpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetIsUsePrvntSysSleep( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, const Vstr * FullPathVstrPt, size_t WrBufSzByt, int32_t MaxStrmNum, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSaveStsToTxtFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt );

__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdStart( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int ExitFlag, Vstr * ErrInfoVstrPt );
__WNDADOVDOTKBK_DLLAPI__ int MediaPocsThrdSendUserMsg( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, unsigned int MsgTyp, void * UserMsgParmPt, size_t UserMsgParmLenByt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
__WNDADOVDOTKBK_DLLAPI__ class MediaPocsThrdCls
{
public:
	MediaPocsThrd * m_MediaPocsThrdPt;

	MediaPocsThrdCls() { m_MediaPocsThrdPt = NULL; }
	~MediaPocsThrdCls() { Dstoy( NULL ); }
	
	//用户定义的初始化函数。
	virtual void UserInit() = 0;
	
	//用户定义的销毁函数。
	virtual void UserDstoy() = 0;
	
	//用户定义的处理函数。
	virtual void UserPocs() = 0;

	//用户定义的消息函数。
	virtual int UserMsg( unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt ) = 0;
	
	//用户定义的设备改变函数。
	virtual void UserDvcChg( Vstr * AdoInptDvcNameVstrPt, Vstr * AdoOtptDvcNameVstrPt, Vstr * VdoInptDvcNameVstrPt ) = 0;

	//用户定义的读取音视频输入帧函数。
	virtual void UserReadAdoVdoInptFrm( int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
										uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
										uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
										uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
										uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt ) = 0;

	//用户定义的写入音频输出帧函数。
	virtual void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
									  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
									  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt ) = 0;

	//用户定义的获取音频输出帧函数。
	virtual void UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
									int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
									uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt ) = 0;

	//用户定义的写入视频输出帧函数。
	virtual void UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
									  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
									  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt ) = 0;

	//用户定义的获取视频输出帧函数。
	virtual void UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
									uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
									uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt ) = 0;

	int Init( const void * LicnCodePt, Vstr * ErrInfoVstrPt );
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = MediaPocsThrdDstoy( m_MediaPocsThrdPt, ErrInfoVstrPt ); m_MediaPocsThrdPt = NULL; return p_Rslt; }

	static int GetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoInptDvcName( AdoInptDvcNameArrPtPtPt, AdoInptDvcTotalPt, ErrInfoVstrPt ); }
	static int GetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoOtptDvcName( AdoOtptDvcNameArrPtPtPt, AdoOtptDvcTotalPt, ErrInfoVstrPt ); }
	static int GetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdGetVdoInptDvcName( VdoInptDvcNameArrPtPtPt, VdoInptDvcTotalPt, ErrInfoVstrPt ); }
	static int DstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdDstoyDvcName( DvcNameArrPtPt, DvcTotal, ErrInfoVstrPt ); }

	int SetAdoInpt( int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInpt( m_MediaPocsThrdPt, IsBlockWait, SmplRate, FrmLenMsec, ErrInfoVstrPt ); }

	int AdoInptSetIsUseSystemAecNsAgc( int IsBlockWait, int32_t IsUseSystemAecNsAgc, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( m_MediaPocsThrdPt, IsBlockWait, IsUseSystemAecNsAgc, ErrInfoVstrPt ); }

	int AdoInptSetUseNoAec( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseNoAec( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int AdoInptSetUseSpeexAec( int IsBlockWait, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexAec( m_MediaPocsThrdPt, IsBlockWait, FilterLenMsec, IsUseRec, EchoMutp, EchoCntu, EchoSupes, EchoSupesAct, ErrInfoVstrPt ); }
	int AdoInptSetUseWebRtcAecm( int IsBlockWait, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcAecm( m_MediaPocsThrdPt, IsBlockWait, IsUseCNGMode, EchoMode, Delay, ErrInfoVstrPt ); }
	int AdoInptSetUseWebRtcAec( int IsBlockWait, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcAec( m_MediaPocsThrdPt, IsBlockWait, EchoMode, Delay, IsUseDelayAgstcMode, IsUseExtdFilterMode, IsUseRefinedFilterAdaptAecMode, IsUseAdaptAdjDelay, ErrInfoVstrPt ); }
	int AdoInptSetUseWebRtcAec3( int IsBlockWait, int32_t Delay, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcAec3( m_MediaPocsThrdPt, IsBlockWait, Delay, ErrInfoVstrPt ); }
	int AdoInptSetUseSpeexWebRtcAec( int IsBlockWait, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t WebRtcAec3Delay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( m_MediaPocsThrdPt, IsBlockWait, WorkMode, SpeexAecFilterLenMsec, SpeexAecIsUseRec, SpeexAecEchoMutp, SpeexAecEchoCntu, SpeexAecEchoSupes, SpeexAecEchoSupesAct, WebRtcAecmIsUseCNGMode, WebRtcAecmEchoMode, WebRtcAecmDelay, WebRtcAecEchoMode, WebRtcAecDelay, WebRtcAecIsUseDelayAgstcMode, WebRtcAecIsUseExtdFilterMode, WebRtcAecIsUseRefinedFilterAdaptAecMode, WebRtcAecIsUseAdaptAdjDelay, WebRtcAec3Delay, IsUseSameRoomAec, SameRoomEchoMinDelay, ErrInfoVstrPt ); }

	int AdoInptSetUseNoNs( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseNoNs( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int AdoInptSetUseSpeexPrpocsNs( int IsBlockWait, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( m_MediaPocsThrdPt, IsBlockWait, IsUseNs, NoiseSupes, IsUseDereverb, ErrInfoVstrPt ); }
	int AdoInptSetUseWebRtcNsx( int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcNsx( m_MediaPocsThrdPt, IsBlockWait, PolicyMode, ErrInfoVstrPt ); }
	int AdoInptSetUseWebRtcNs( int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcNs( m_MediaPocsThrdPt, IsBlockWait, PolicyMode, ErrInfoVstrPt ); }
	int AdoInptSetUseRNNoise( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseRNNoise( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }

	int AdoInptSetIsUseSpeexPrpocs( int IsBlockWait, int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( m_MediaPocsThrdPt, IsBlockWait, IsUseSpeexPrpocs, IsUseVad, VadProbStart, VadProbCntu, IsUseAgc, AgcLevel, AgcIncrement, AgcDecrement, AgcMaxGain, ErrInfoVstrPt ); }

	int AdoInptSetUsePcm( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUsePcm( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int AdoInptSetUseSpeexEncd( int IsBlockWait, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexEncd( m_MediaPocsThrdPt, IsBlockWait, UseCbrOrVbr, Qualt, Cmplxt, PlcExptLossRate, ErrInfoVstrPt ); }
	int AdoInptSetUseOpusEncd( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseOpusEncd( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }

	int AdoInptSetIsSaveAdoToWaveFile( int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, const Vstr * RsltFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( m_MediaPocsThrdPt, IsBlockWait, IsSave, SrcFullPathVstrPt, RsltFullPathVstrPt, WrBufSzByt, ErrInfoVstrPt ); }
	int AdoInptSetIsDrawAdoWavfmToWnd( int IsBlockWait, int32_t IsDraw, HWND SrcWndHdl, HWND RsltWndHdl, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsBlockWait, IsDraw, SrcWndHdl, RsltWndHdl, ErrInfoVstrPt ); }

	int AdoInptSetUseDvc( int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseDvc( m_MediaPocsThrdPt, IsBlockWait, NameVstrPt, ErrInfoVstrPt ); }
	int AdoInptSetIsMute( int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsMute( m_MediaPocsThrdPt, IsBlockWait, IsMute, ErrInfoVstrPt ); }

	int SetAdoOtpt( int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtpt( m_MediaPocsThrdPt, IsBlockWait, SmplRate, FrmLenMsec, ErrInfoVstrPt ); }
	
	int AdoOtptAddStrm( int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptAddStrm( m_MediaPocsThrdPt, IsBlockWait, AdoOtptStrmIdx, ErrInfoVstrPt ); }
	int AdoOtptDelStrm( int IsBlockWait, int AddFirstOrLast, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt )  { return MediaPocsThrdAdoOtptDelStrm( m_MediaPocsThrdPt, IsBlockWait, AddFirstOrLast, AdoOtptStrmIdx, ErrInfoVstrPt ); }
	int AdoOtptSetStrmUsePcm( int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUsePcm( m_MediaPocsThrdPt, IsBlockWait, AdoOtptStrmIdx, ErrInfoVstrPt ); }
	int AdoOtptSetStrmUseSpeexDecd( int IsBlockWait, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( m_MediaPocsThrdPt, IsBlockWait, AdoOtptStrmIdx, IsUsePrcplEnhsmt, ErrInfoVstrPt ); }
	int AdoOtptSetStrmUseOpusDecd( int IsBlockWait, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUseOpusDecd( m_MediaPocsThrdPt, IsBlockWait, AdoOtptStrmIdx, ErrInfoVstrPt ); }
	int AdoOtptSetStrmIsUse( int IsBlockWait, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmIsUse( m_MediaPocsThrdPt, IsBlockWait, AdoOtptStrmIdx, IsUseAdoOtptStrm, ErrInfoVstrPt ); }

	int AdoOtptSetIsSaveAdoToWaveFile( int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( m_MediaPocsThrdPt, IsBlockWait, IsSave, SrcFullPathVstrPt, WrBufSzByt, ErrInfoVstrPt ); }
	int AdoOtptSetIsDrawAdoWavfmToWnd( int IsBlockWait, int32_t IsDraw, HWND SrcWndHdl, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsBlockWait, IsDraw, SrcWndHdl, ErrInfoVstrPt ); }

	int AdoOtptSetUseDvc( int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetUseDvc( m_MediaPocsThrdPt, IsBlockWait, NameVstrPt, ErrInfoVstrPt ); }
	int AdoOtptSetIsMute( int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsMute( m_MediaPocsThrdPt, IsBlockWait, IsMute, ErrInfoVstrPt ); }

	int SetVdoInpt( int IsBlockWait, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, int32_t SrcMaxSmplRate, int32_t SrcFrmWidth, int32_t SrcFrmHeight, HWND PrvwWndHdl, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInpt( m_MediaPocsThrdPt, IsBlockWait, MaxSmplRate, FrmWidth, FrmHeight, SrcMaxSmplRate, SrcFrmWidth, SrcFrmHeight, PrvwWndHdl, ErrInfoVstrPt ); }

	int VdoInptSetUseYu12( int IsBlockWait, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseYu12( m_MediaPocsThrdPt, IsBlockWait, ErrInfoVstrPt ); }
	int VdoInptSetUseOpenH264Encd( int IsBlockWait, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseOpenH264Encd( m_MediaPocsThrdPt, IsBlockWait, VdoType, EncdBitrate, BitrateCtrlMode, IDRFrmIntvl, Cmplxt, ErrInfoVstrPt ); }

	int VdoInptSetUseDvc( int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseDvc( m_MediaPocsThrdPt, IsBlockWait, NameVstrPt, ErrInfoVstrPt ); }

	int VdoInptSetIsBlack( int IsBlockWait, int IsBlack, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetIsBlack( m_MediaPocsThrdPt, IsBlockWait, IsBlack, ErrInfoVstrPt ); }

	int VdoOtptAddStrm( int IsBlockWait, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptAddStrm( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, ErrInfoVstrPt ); }
	int VdoOtptDelStrm( int IsBlockWait, int AddFirstOrLast, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptDelStrm( m_MediaPocsThrdPt, IsBlockWait, AddFirstOrLast, VdoOtptStrmIdx, ErrInfoVstrPt ); }
	int VdoOtptSetStrm( int IsBlockWait, uint32_t VdoOtptStrmIdx, HWND DspyWndHdl, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrm( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, DspyWndHdl, ErrInfoVstrPt ); }
	int VdoOtptSetStrmUseYu12( int IsBlockWait, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmUseYu12( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, ErrInfoVstrPt ); }
	int VdoOtptSetStrmUseOpenH264Decd( int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, DecdThrdNum, ErrInfoVstrPt ); }
	int VdoOtptSetStrmIsBlack( int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmIsBlack( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, IsBlack, ErrInfoVstrPt ); }
	int VdoOtptSetStrmIsUse( int IsBlockWait, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmIsUse( m_MediaPocsThrdPt, IsBlockWait, VdoOtptStrmIdx, IsUseVdoOtptStrm, ErrInfoVstrPt ); }
	
	int SetIsUseAdoVdoInptOtpt( int IsBlockWait, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseAdoVdoInptOtpt( m_MediaPocsThrdPt, IsBlockWait, IsUseAdoInpt, IsUseAdoOtpt, IsUseVdoInpt, IsUseVdoOtpt, ErrInfoVstrPt ); }

	int SetIsPrintLogShowToast( int IsPrintLog, int IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetIsPrintLogShowToast( m_MediaPocsThrdPt, IsPrintLog, IsShowToast, ShowToastWndHdl, ErrInfoVstrPt ); }
	int SetIsUsePrvntSysSleep( int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUsePrvntSysSleep( m_MediaPocsThrdPt, IsBlockWait, IsUsePrvntSysSleep, ErrInfoVstrPt ); }
	int SetIsSaveAdoVdoInptOtptToAviFile( int IsBlockWait, const Vstr * FullPathVstrPt, size_t WrBufSzByt, int32_t MaxStrmNum, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( m_MediaPocsThrdPt, IsBlockWait, FullPathVstrPt, WrBufSzByt, MaxStrmNum, IsSaveAdoInpt, IsSaveAdoOtpt, IsSaveVdoInpt, IsSaveVdoOtpt, ErrInfoVstrPt ); }
	int SaveStsToTxtFile( int IsBlockWait, const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSaveStsToTxtFile( m_MediaPocsThrdPt, IsBlockWait, FullPathVstrPt, ErrInfoVstrPt ); }
	
	int Start( Vstr * ErrInfoVstrPt ) { return MediaPocsThrdStart( m_MediaPocsThrdPt, ErrInfoVstrPt ); }
	int RqirExit( int IsBlockWait, int ExitFlag, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdRqirExit( m_MediaPocsThrdPt, IsBlockWait, ExitFlag, ErrInfoVstrPt ); }
	int SendUserMsg( int IsBlockWait, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt ) { return MediaPocsThrdSendUserMsg( m_MediaPocsThrdPt, IsBlockWait, MsgTyp, MsgParmPt, MsgParmLenByt, ErrInfoVstrPt ); }
};
#endif
