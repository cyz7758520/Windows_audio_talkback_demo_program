#include "Func.h"
#include "DataStruct.h"
#include "SpeexDsp.h"
#include "WebRtc.h"
#include "SpeexWebRtcAec.h"
#include "RNNoise.h"
#include "Speex.h"
#include "AdoWavfm.h"
#include "LibYUV.h"
#include "OpenH264.h"
#include "MediaFile.h"

#ifndef __MEDIAPROCTHRD_H__
#define __MEDIAPROCTHRD_H__

#include "AdoInpt.h"
#include "AdoOtpt.h"
#include "VdoInpt.h"
#include "VdoOtpt.h"

typedef struct IGraphBuilder IGraphBuilder;
typedef struct IMediaControl IMediaControl;
typedef struct IMediaEventEx IMediaEventEx;
typedef struct IBaseFilter IBaseFilter;
typedef struct VdoInptThrd VdoInptThrd;
typedef struct MediaPocsThrd MediaPocsThrd;

//媒体处理线程。
typedef struct MediaPocsThrd
{
	enum //存放本线程运行标记。
	{
		RunFlagNorun, //未开始运行。
		RunFlagRun, //正在运行。
		RunFlagExit, //已经退出。
	} m_RunFlag;

	enum //存放退出码。
	{
		ExitCodeNormal, //正常退出。
		ExitCodeUserInit, //调用用户定义的初始化函数失败。
		ExitCodeAdoVdoInptOtptInit, //音视频输入输出初始化失败。
		ExitCodeMediaMsgPocs, //媒体消息处理失败。
		ExitCodeAdoVdoInptOtptPocs, //音视频输入输出处理失败。
	} m_ExitCode;
	
	int32_t m_LastCallUserInitOrDstoy; //存放上一次调用了用户定义的初始化函数或销毁函数，为0表示初始化函数，为1表示销毁函数。
	int32_t m_ReadyExitCnt; //存放准备退出计数，为0表示不准备退出，大于0表示要准备退出。
	
	VarLenLnkLstCls m_MediaMsgLnkLst; //存放媒体消息链表。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。
	HWND m_ShowToastWndHdl; //存放显示Toast窗口的句柄，为NULL表示桌面窗口。

	int32_t m_IsUsePrvntSysSleep; //存放是否使用阻止系统睡眠，为非0表示要使用，为0表示不使用。
	
	struct //存放音视频输入输出Avi文件。
	{
		AviFileWriter * m_WriterPt; //存放写入器的指针。
		Vstr * m_FullPathVstrPt; //存放完整路径动态字符串的指针。
		size_t m_WrBufSzByt; //存放写入缓冲区的大小，单位为字节。
		int32_t m_IsSaveAdoInpt; //存放是否保存音频输入，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveAdoOtpt; //存放是否保存音频输出，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveVdoInpt; //存放是否保存视频输入，为非0表示要保存，为0表示不保存。
		int32_t m_IsSaveVdoOtpt; //存放是否保存视频输出，为非0表示要保存，为0表示不保存。
		uint32_t m_PcmAdoInptSrcStrmIdx; //存放Pcm格式音频输入原始流的索引。
		uint32_t m_PcmAdoInptRsltStrmIdx; //存放Pcm格式音频输入结果流的索引。
		int32_t m_AdoInptStrmTimeStampIsReset; //存放音频输入流时间戳是否重置，为非0表示要重置，为0表示不重置。
		uint32_t m_PcmAdoOtptSrcStrmIdx; //存放Pcm格式音频输出原始流的索引。
		int32_t m_AdoOtptStrmTimeStampIsReset; //存放音频输出流时间戳是否重置，为非0表示要重置，为0表示不重置。
		uint64_t m_AdoInptOtptStrmCurTimeStampMsec; //存放音频输入输出流的当前时间戳，单位为毫秒。
		uint32_t m_EncdVdoInptRsltStrmIdx; //存放已编码格式视频输入结果流的索引。
		std::map< uint32_t, uint32_t > * m_EncdVdoOtptSrcStrmIdxMapPt; //存放已编码格式视频输出原始流的索引映射的指针。
	} m_AdoVdoInptOtptAviFile;

	AdoInpt m_AdoInpt; //存放音频输入。
	AdoOtpt m_AdoOtpt; //存放音频输出。
	VdoInpt m_VdoInpt; //存放视频输入。
    VdoOtpt m_VdoOtpt; //存放视频输出。
	
	struct //存放线程。
	{
		int32_t m_IsInitThrdTmpVar; //存放是否初始化线程的临时变量。
		int16_t * m_PcmAdoInptSrcFrmPt; //存放Pcm格式音频输入原始帧的指针。
		int16_t * m_PcmAdoInptRsltFrmPt; //存放Pcm格式音频输入结果帧的指针。
		int16_t * m_PcmAdoInptTmpFrmPt; //存放Pcm格式音频输入临时帧的指针。
		int16_t * m_PcmAdoOtptSrcFrmPt; //存放Pcm格式音频输出原始帧的指针。
		int32_t m_PcmAdoInptRsltFrmVoiceActSts; //存放Pcm格式音频输入原始帧的语音活动状态，为非0表示有语音活动，为0表示无语音活动。
		uint8_t * m_EncdAdoInptRsltFrmPt; //存放已编码格式音频输入结果帧的指针，大小为 m_AdoInpt.m_FrmLenByt 字节。
		size_t m_EncdAdoInptRsltFrmLenByt; //存放已编码格式音频输入结果帧的长度，单位为字节。
		int32_t m_EncdAdoInptRsltFrmIsNeedTrans; //存放已编码格式音频输入结果帧是否需要传输，为非0表示需要传输，为0表示不要传输。
		VdoInpt::Frm * m_VdoInptFrmPt; //存放视频输入帧的指针。
		VdoOtpt::Frm * m_VdoOtptFrmPt; //存放视频输出帧的指针。

		ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
	} m_Thrd;

	Vstr * m_ErrInfoVstrPt; //存放错误信息动态字符串的指针。

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的MediaPocsThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的初始化函数。
	typedef int( __cdecl * USER_INIT_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );
	USER_INIT_FUNC_PT m_UserInitFuncPt;

	//用户定义的处理函数。
	typedef int( __cdecl * USER_POCS_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );
	USER_POCS_FUNC_PT m_UserPocsFuncPt;

	//用户定义的销毁函数。
	typedef void( __cdecl * USER_DSTOY_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );
	USER_DSTOY_FUNC_PT m_UserDstoyFuncPt;
	
	//用户定义的消息函数。
	typedef int( __cdecl * USER_MSG_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, void * MsgArgPt );
	USER_MSG_FUNC_PT m_UserMsgFuncPt;

	//用户定义的读取音视频输入帧函数。
	typedef void( __cdecl * USER_READ_ADO_VDO_INPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt,
																  int16_t * PcmAdoInptSrcFrmPt, int16_t * PcmAdoInptRsltFrmPt, size_t PcmAdoInptFrmLenUnit, int32_t PcmAdoInptRsltFrmVoiceActSts,
																  uint8_t * EncdAdoInptRsltFrmPt, size_t EncdAdoInptRsltFrmLenByt, int32_t EncdAdoInptRsltFrmIsNeedTrans,
																  uint8_t * BgraVdoInptSrcFrmPt, int32_t BgraVdoInptSrcFrmWidth, int32_t BgraVdoInptSrcFrmHeight, size_t BgraVdoInptSrcFrmLenByt,
																  uint8_t * YU12VdoInptRsltFrmPt, int32_t YU12VdoInptRsltFrmWidth, int32_t YU12VdoInptRsltFrmHeight, size_t YU12VdoInptRsltFrmLenByt,
																  uint8_t * EncdVdoInptRsltFrmPt, size_t EncdVdoInptRsltFrmLenByt );
	USER_READ_ADO_VDO_INPT_FRM_FUNC_PT m_UserReadAdoVdoInptFrmFuncPt;

	//用户定义的写入音频输出帧函数。
	typedef void( __cdecl * USER_WRITE_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
															   int16_t * PcmAdoOtptSrcFrmPt, size_t PcmAdoOtptFrmLenUnit,
															   uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmSzByt, size_t * EncdAdoOtptSrcFrmLenBytPt );
	USER_WRITE_ADO_OTPT_FRM_FUNC_PT m_UserWriteAdoOtptFrmFuncPt;

	//用户定义的获取音频输出帧函数。
	typedef void( __cdecl * USER_GET_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
															 int16_t * PcmAdoOtptSrcFrmPt, size_t PcmAdoOtptFrmLenUnit,
															 uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmLenByt );
	USER_GET_ADO_OTPT_FRM_FUNC_PT m_UserGetAdoOtptFrmFuncPt;

	//用户定义的写入视频输出帧函数。
	typedef void( __cdecl * USER_WRITE_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
															   uint8_t * YU12VdoOtptSrcFrmPt, int32_t * YU12VdoOtptSrcFrmWidthPt, int32_t * YU12VdoOtptSrcFrmHeightPt,
															   uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmSzByt, size_t * EncdVdoOtptSrcFrmLenBytPt );
	USER_WRITE_VDO_OTPT_FRM_FUNC_PT m_UserWriteVdoOtptFrmFuncPt;

	//用户定义的获取视频输出帧函数。
	typedef void( __cdecl * USER_GET_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
															 uint8_t * YU12VdoOtptSrcFrmPt, int32_t YU12VdoOtptSrcFrmWidth, int32_t YU12VdoOtptSrcFrmHeight,
															 uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmLenByt );

	USER_GET_VDO_OTPT_FRM_FUNC_PT m_UserGetVdoOtptFrmFuncPt;
}MediaPocsThrd;

#ifdef __cplusplus
extern "C"
{
#endif

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
					   MediaPocsThrd::USER_INIT_FUNC_PT UserInitFuncPt, MediaPocsThrd::USER_POCS_FUNC_PT UserPocsFuncPt, MediaPocsThrd::USER_DSTOY_FUNC_PT UserDstoyFuncPt, MediaPocsThrd::USER_MSG_FUNC_PT UserMsgFuncPt,
					   MediaPocsThrd::USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   MediaPocsThrd::USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, MediaPocsThrd::USER_GET_ADO_OTPT_FRM_FUNC_PT UserGetAdoOtptFrmFuncPt,
					   MediaPocsThrd::USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, MediaPocsThrd::USER_GET_VDO_OTPT_FRM_FUNC_PT UserGetVdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt );
int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseSystemAecNsAgc, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetUseNoAec( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetUseNoNs( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetUsePcm( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToWaveFile, const Vstr * AdoInptSrcWaveFileFullPathVstrPt, const Vstr * AdoInptRsltWaveFileFullPathVstrPt, size_t AdoInptWaveFileWrBufSzByt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoInptSrcWavfmWndHdl, HWND AdoInptRsltWavfmWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoInptDvcID, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoInptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToWaveFile, const Vstr * AdoOtptSrcWaveFileFullPathVstrPt, size_t AdoOtptWaveFileWrBufSzByt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptSrcWavfmWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAdoOtptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoOtptDvcID, Vstr * ErrInfoVstrPt );
int MediaPocsThrdAdoOtptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND PrvwWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdVdoInptSetUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoInptSetUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdVdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT VdoInptDvcID, Vstr * ErrInfoVstrPt );

int MediaPocsThrdVdoInptSetIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt );

int MediaPocsThrdVdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptSetStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, HWND DspyWndHdl, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptSetStrmUseYU12( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptSetStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt );
int MediaPocsThrdVdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsUseAdoVdoInptOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetIsUsePrvntSysSleep( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * AdoVdoInptOtptAviFileFullPathVstrPt, size_t AdoVdoInptOtptAviFileWrBufSzByt, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * StngFileFullPathVstrPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdStart( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int ExitFlag, int IsBlockWait, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSendUserMsg( MediaPocsThrd * MediaPocsThrdPt, void * MsgArgPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class MediaPocsThrdCls
{
public:
	MediaPocsThrd * m_MediaPocsThrdPt;

	MediaPocsThrdCls() { m_MediaPocsThrdPt = NULL; }
	~MediaPocsThrdCls() { Dstoy( NULL ); }
	
	//用户定义的初始化函数。
	virtual int UserInit() = 0;
	
	//用户定义的处理函数。
	virtual int UserPocs() = 0;

	//用户定义的销毁函数。
	virtual void UserDstoy() = 0;
	
	//用户定义的消息函数。
	virtual int UserMsg( void * MsgArgPt ) = 0;

	//用户定义的读取音视频输入帧函数。
	virtual void UserReadAdoVdoInptFrm( int16_t * PcmAdoInptSrcFrmPt, int16_t * PcmAdoInptRsltFrmPt, size_t PcmAdoInptFrmLenUnit, int32_t PcmAdoInptRsltFrmVoiceActSts,
										uint8_t * EncdAdoInptRsltFrmPt, size_t EncdAdoInptRsltFrmLenByt, int32_t EncdAdoInptRsltFrmIsNeedTrans,
										uint8_t * BgraVdoInptSrcFrmPt, int32_t BgraVdoInptSrcFrmWidth, int32_t BgraVdoInptSrcFrmHeight, size_t BgraVdoInptSrcFrmLenByt,
										uint8_t * YU12VdoInptRsltFrmPt, int32_t YU12VdoInptRsltFrmWidth, int32_t YU12VdoInptRsltFrmHeight, size_t YU12VdoInptRsltFrmLenByt,
										uint8_t * EncdVdoInptRsltFrmPt, size_t EncdVdoInptRsltFrmLenByt ) = 0;

	//用户定义的写入音频输出帧函数。
	virtual void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
									  int16_t * PcmAdoOtptSrcFrmPt, size_t PcmAdoOtptFrmLenUnit,
									  uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmSzByt, size_t * EncdAdoOtptSrcFrmLenBytPt ) = 0;

	//用户定义的获取音频输出帧函数。
	virtual void UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
									int16_t * PcmAdoOtptSrcFrmPt, size_t PcmAdoOtptFrmLenUnit,
									uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmLenByt ) = 0;

	//用户定义的写入视频输出帧函数。
	virtual void UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
									  uint8_t * YU12VdoOtptSrcFrmPt, int32_t * YU12VdoOtptSrcFrmWidthPt, int32_t * YU12VdoOtptSrcFrmHeightPt,
									  uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmSzByt, size_t * EncdVdoOtptSrcFrmLenBytPt ) = 0;

	//用户定义的获取视频输出帧函数。
	virtual void UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
									uint8_t * YU12VdoOtptSrcFrmPt, int32_t YU12VdoOtptSrcFrmWidth, int32_t YU12VdoOtptSrcFrmHeight,
									uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmLenByt ) = 0;

	int Init( VstrCls * ErrInfoVstrPt );
	int Dstoy( VstrCls * ErrInfoVstrPt ) { int p_Rslt = MediaPocsThrdDstoy( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); m_MediaPocsThrdPt = NULL; return p_Rslt; }

	int GetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoInptDvcName( AdoInptDvcNameArrPtPtPt, AdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoOtptDvcName( AdoOtptDvcNameArrPtPtPt, AdoOtptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetVdoInptDvcName( VdoInptDvcNameArrPtPtPt, VdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdDstoyDvcName( DvcNameArrPtPt, DvcTotal, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInpt( int32_t SmplRate, size_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInpt( m_MediaPocsThrdPt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetIsUseSystemAecNsAgc( int32_t IsUseSystemAecNsAgc, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( m_MediaPocsThrdPt, IsUseSystemAecNsAgc, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetUseNoAec( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseNoAec( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseSpeexAec( int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexAec( m_MediaPocsThrdPt, FilterLenMsec, IsUseRec, EchoMutp, EchoCntu, EchoSupes, EchoSupesAct, IsSaveMemFile, MemFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseWebRtcAecm( int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcAecm( m_MediaPocsThrdPt, IsUseCNGMode, EchoMode, Delay, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseWebRtcAec( int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcAec( m_MediaPocsThrdPt, EchoMode, Delay, IsUseDelayAgstcMode, IsUseExtdFilterMode, IsUseRefinedFilterAdaptAecMode, IsUseAdaptAdjDelay, IsSaveMemFile, MemFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseSpeexWebRtcAec( int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( m_MediaPocsThrdPt, WorkMode, SpeexAecFilterLenMsec, SpeexAecIsUseRec, SpeexAecEchoMutp, SpeexAecEchoCntu, SpeexAecEchoSupes, SpeexAecEchoSupesAct, WebRtcAecmIsUseCNGMode, WebRtcAecmEchoMode, WebRtcAecmDelay, WebRtcAecEchoMode, WebRtcAecDelay, WebRtcAecIsUseDelayAgstcMode, WebRtcAecIsUseExtdFilterMode, WebRtcAecIsUseRefinedFilterAdaptAecMode, WebRtcAecIsUseAdaptAdjDelay, IsUseSameRoomAec, SameRoomEchoMinDelay, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetUseNoNs( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseNoNs( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseSpeexPrpocsNs( int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( m_MediaPocsThrdPt, IsUseNs, NoiseSupes, IsUseDereverb, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseWebRtcNsx( int32_t PolicyMode, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcNsx( m_MediaPocsThrdPt, PolicyMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseWebRtcNs( int32_t PolicyMode, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseWebRtcNs( m_MediaPocsThrdPt, PolicyMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseRNNoise( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseRNNoise( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetIsUseSpeexPrpocs( int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( m_MediaPocsThrdPt, IsUseSpeexPrpocs, IsUseVad, VadProbStart, VadProbCntu, IsUseAgc, AgcLevel, AgcIncrement, AgcDecrement, AgcMaxGain, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetUsePcm( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUsePcm( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseSpeexEncd( int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseSpeexEncd( m_MediaPocsThrdPt, UseCbrOrVbr, Qualt, Cmplxt, PlcExptLossRate, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetUseOpusEncd( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseOpusEncd( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetIsSaveAdoToWaveFile( int32_t IsSaveAdoToWaveFile, const Vstr * AdoInptSrcWaveFileFullPathVstrPt, const Vstr * AdoInptRsltWaveFileFullPathVstrPt, size_t AdoInptWaveFileWrBufSzByt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( m_MediaPocsThrdPt, IsSaveAdoToWaveFile, AdoInptSrcWaveFileFullPathVstrPt, AdoInptRsltWaveFileFullPathVstrPt, AdoInptWaveFileWrBufSzByt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetIsDrawAdoWavfmToWnd( int32_t IsDrawAdoWavfmToWnd, HWND AdoInptSrcWavfmWndHdl, HWND AdoInptRsltWavfmWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsDrawAdoWavfmToWnd, AdoInptSrcWavfmWndHdl, AdoInptRsltWavfmWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoInptSetUseDvc( UINT AdoInptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetUseDvc( m_MediaPocsThrdPt, AdoInptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoInptSetIsMute( int32_t IsMute, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoInptSetIsMute( m_MediaPocsThrdPt, IsMute, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtpt( int32_t SmplRate, size_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtpt( m_MediaPocsThrdPt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int AdoOtptAddStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptAddStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptDelStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt )  { return MediaPocsThrdAdoOtptDelStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetStrmUsePcm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUsePcm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetStrmUseSpeexDecd( int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, IsUsePrcplEnhsmt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetStrmUseOpusDecd( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmUseOpusDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetStrmIsUse( int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetStrmIsUse( m_MediaPocsThrdPt, AdoOtptStrmIdx, IsUseAdoOtptStrm, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoOtptSetIsSaveAdoToWaveFile( int32_t IsSaveAdoToWaveFile, const Vstr * AdoOtptSrcWaveFileFullPathVstrPt, size_t AdoOtptWaveFileWrBufSzByt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( m_MediaPocsThrdPt, IsSaveAdoToWaveFile, AdoOtptSrcWaveFileFullPathVstrPt, AdoOtptWaveFileWrBufSzByt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetIsDrawAdoWavfmToWnd( int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptSrcWavfmWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsDrawAdoWavfmToWnd, AdoOtptSrcWavfmWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AdoOtptSetUseDvc( UINT AdoOtptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetUseDvc( m_MediaPocsThrdPt, AdoOtptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int AdoOtptSetIsMute( int32_t IsMute, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAdoOtptSetIsMute( m_MediaPocsThrdPt, IsMute, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInpt( int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND PrvwWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInpt( m_MediaPocsThrdPt, MaxSmplRate, FrmWidth, FrmHeight, PrvwWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int VdoInptSetUseYU12( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseYU12( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoInptSetUseOpenH264Encd( int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseOpenH264Encd( m_MediaPocsThrdPt, VdoType, EncdBitrate, BitrateCtrlMode, IDRFrmIntvl, Cmplxt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int VdoInptSetUseDvc( UINT VdoInptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetUseDvc( m_MediaPocsThrdPt, VdoInptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int VdoInptSetIsBlack( int IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoInptSetIsBlack( m_MediaPocsThrdPt, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int VdoOtptAddStrm( uint32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptAddStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptDelStrm( uint32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptDelStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptSetStrm( uint32_t VdoOtptStrmIdx, HWND DspyWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, DspyWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptSetStrmUseYU12( uint32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmUseYU12( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptSetStrmUseOpenH264Decd( uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( m_MediaPocsThrdPt, VdoOtptStrmIdx, DecdThrdNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptSetStrmIsBlack( uint32_t VdoOtptStrmIdx, int32_t IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmIsBlack( m_MediaPocsThrdPt, VdoOtptStrmIdx, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int VdoOtptSetStrmIsUse( uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdVdoOtptSetStrmIsUse( m_MediaPocsThrdPt, VdoOtptStrmIdx, IsUseVdoOtptStrm, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int SetIsUseAdoVdoInptOtpt( int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseAdoVdoInptOtpt( m_MediaPocsThrdPt, IsUseAdoInpt, IsUseAdoOtpt, IsUseVdoInpt, IsUseVdoOtpt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsPrintLogShowToast( int IsPrintLog, int IsShowToast, HWND ShowToastWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsPrintLogShowToast( m_MediaPocsThrdPt, IsPrintLog, IsShowToast, ShowToastWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetIsUsePrvntSysSleep( int32_t IsUsePrvntSysSleep, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUsePrvntSysSleep( m_MediaPocsThrdPt, IsUsePrvntSysSleep, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetIsSaveAdoVdoInptOtptToAviFile( const Vstr * AdoVdoInptOtptAviFileFullPathVstrPt, size_t AdoVdoInptOtptAviFileWrBufSzByt, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( m_MediaPocsThrdPt, AdoVdoInptOtptAviFileFullPathVstrPt, AdoVdoInptOtptAviFileWrBufSzByt, IsSaveAdoInpt, IsSaveAdoOtpt, IsSaveVdoInpt, IsSaveVdoOtpt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SaveStngToFile( const Vstr * StngFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSaveStngToFile( m_MediaPocsThrdPt, StngFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int Start( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdStart( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int RqirExit( int ExitFlag, int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdRqirExit( m_MediaPocsThrdPt, ExitFlag, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendUserMsg( void * MsgArgPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSendUserMsg( m_MediaPocsThrdPt, MsgArgPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
};
#endif

#endif
