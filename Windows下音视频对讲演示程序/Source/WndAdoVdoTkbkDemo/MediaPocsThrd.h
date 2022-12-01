#include "Func.h"
#include "DataStruct.h"
#include "SpeexDsp.h"
#include "WebRtc.h"
#include "SpeexWebRtcAec.h"
#include "RNNoise.h"
#include "Speex.h"
#include "WaveFile.h"
#include "AdoWavfm.h"
#include "LibYUV.h"
#include "OpenH264.h"

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

#define CLSID_MMDeviceEnumerator __uuidof( MMDeviceEnumerator )
#define IID_IMMDeviceEnumerator __uuidof( IMMDeviceEnumerator )
#define IID_IAudioClient __uuidof( IAudioClient )
#define IID_IAudioCaptureClient __uuidof( IAudioCaptureClient )
#define IID_IAudioRenderClient __uuidof( IAudioRenderClient )

typedef int( __cdecl * USER_INIT_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef int( __cdecl * USER_POCS_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef void( __cdecl * USER_DSTOY_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef int( __cdecl * USER_MSG_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, void * MsgArgPt );

typedef void( __cdecl * USER_READ_ADO_VDO_INPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAdoInptFrmPt, int16_t * PcmAdoRsltFrmPt, int32_t VoiceActSts, uint8_t * EncdAdoInptFrmPt, size_t EncdAdoInptFrmLen, int32_t EncdAdoInptFrmIsNeedTrans,
															  uint8_t * YU12VdoInptFrmPt, int32_t YU12VdoInptFrmWidth, int32_t YU12VdoInptFrmHeight, uint8_t * EncdVdoInptFrmPt, size_t EncdVdoInptFrmLen );

typedef void( __cdecl * USER_WRITE_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, uint8_t * EncdAdoOtptFrmPt, size_t * AdoOtptFrmLenPt );

typedef void( __cdecl * USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, size_t PcmAdoOtptFrmLen );

typedef void( __cdecl * USER_WRITE_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, uint8_t * YU12VdoOtptFrmPt, int32_t * YU12VdoOtptFrmWidthPt, int32_t * YU12VdoOtptFrmHeightPt, uint8_t * EncdVdoOtptFrmPt, size_t * VdoOtptFrmLenPt );

typedef void( __cdecl * USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, uint8_t * YU12VdoOtptFrmPt, int32_t YU12VdoOtptFrmWidth, int32_t YU12VdoOtptFrmHeight );

//媒体处理线程。
typedef struct MediaPocsThrd
{
	enum
	{
		RunFlagNorun, //运行标记：未开始运行。
		RunFlagRun,   //运行标记：正在运行。
		RunFlagExit,  //运行标记：已经退出。
	} m_RunFlag; //存放本线程运行标记。

	enum
	{
		ExitCodeNormal,             //退出码：正常退出。
		ExitCodeUserInit,           //退出码：调用用户定义的初始化函数失败。
		ExitCodeAdoVdoInptOtptInit, //退出码：音视频输入输出初始化失败。
		ExitCodeMediaMsgPocs,       //退出码：媒体消息处理失败。
		ExitCodeAdoVdoInptOtptPocs, //退出码：音视频输入输出处理失败。
	} m_ExitCode; //存放退出码。
	
	int32_t m_LastCallUserInitOrDstoy; //存放上一次调用了用户定义的初始化函数或销毁函数，为0表示初始化函数，为1表示销毁函数。
	int32_t m_ReadyExitCnt; //存放准备退出计数，为0表示不准备退出，大于0表示要准备退出。
	
	VarLenLnkLstCls m_MediaMsgLnkLst; //存放媒体消息链表。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。
	HWND m_ShowToastWndHdl; //存放显示Toast窗口的句柄，为NULL表示桌面窗口。

	int32_t m_IsUsePrvntSysSleep; //存放是否使用阻止系统睡眠，为非0表示要使用，为0表示不使用。

	AdoInpt m_AdoInpt; //存放音频输入。
	AdoOtpt m_AdoOtpt; //存放音频输出。
	VdoInpt m_VdoInpt; //存放视频输入。
    VdoOtpt m_VdoOtpt; //存放视频输出。

	//媒体处理线程的临时变量。
	int16_t * m_PcmAdoInptFrmPt; //存放PCM格式音频输入帧的指针。
	int16_t * m_PcmAdoOtptFrmPt; //存放PCM格式音频输出帧的指针。
	int16_t * m_PcmAdoRsltFrmPt; //存放PCM格式音频结果帧的指针。
	int16_t * m_PcmAdoTmpFrmPt; //存放PCM格式音频临时帧的指针。
	int16_t * m_PcmAdoSwapFrmPt; //存放PCM格式音频交换帧的指针。
	int32_t m_VoiceActSts; //存放语音活动状态，为1表示有语音活动，为0表示无语音活动。
	uint8_t * m_EncdAdoInptFrmPt; //存放已编码格式音频输入帧的指针。
	size_t m_EncdAdoInptFrmSz; //存放已编码格式音频输入帧的大小，单位为字节。
    size_t m_EncdAdoInptFrmLen; //存放已编码格式音频输入帧的长度，单位为字节。
    int32_t m_EncdAdoInptFrmIsNeedTrans; //存放已编码格式音频输入帧是否需要传输，为1表示需要传输，为0表示不需要传输。
	VdoInptFrmElm * m_VdoInptFrmPt; //存放视频输入帧的指针。

	ThrdInfo * m_MediaPocsThrdInfoPt; //存放媒体处理线程信息的指针。

	Vstr * m_ErrInfoVstrPt; //存放错误信息动态字符串的指针。

	//用户定义的相关回调函数。

	//存放用户数据的指针。注意：在C++的MediaPocsThrdCls类中，本变量存放this指针，请勿修改。
	void * m_UserDataPt;
	
	//用户定义的初始化函数。
	USER_INIT_FUNC_PT m_UserInitFuncPt;

	//用户定义的处理函数。
	USER_POCS_FUNC_PT m_UserPocsFuncPt;

	//用户定义的销毁函数。
	USER_DSTOY_FUNC_PT m_UserDstoyFuncPt;
	
	//用户定义的消息函数。
	USER_MSG_FUNC_PT m_UserMsgFuncPt;

	//用户定义的读取音视频输入帧函数。
	USER_READ_ADO_VDO_INPT_FRM_FUNC_PT m_UserReadAdoVdoInptFrmFuncPt;

	//用户定义的写入音频输出帧函数。
	USER_WRITE_ADO_OTPT_FRM_FUNC_PT m_UserWriteAdoOtptFrmFuncPt;

	//用户定义的获取PCM格式音频输出帧函数。
	USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT m_UserGetPcmAdoOtptFrmFuncPt;

	//用户定义的写入视频输出帧函数。
	USER_WRITE_VDO_OTPT_FRM_FUNC_PT m_UserWriteVdoOtptFrmFuncPt;

	//用户定义的获取YU12格式视频输出帧函数。
	USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT m_UserGetYU12VdoOtptFrmFuncPt;
}MediaPocsThrd;

#ifdef __cplusplus
extern "C"
{
#endif

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
					   USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt, USER_MSG_FUNC_PT UserMsgFuncPt,
					   USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT UserGetPcmAdoOtptFrmFuncPt,
					   USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT UserGetYU12VdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt );
int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptUseNoAec( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptUseNoNs( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptIsUseSpeexPrpocsOther( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptUsePcm( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoInptFileFullPathVstrPt, const Vstr * AdoRsltFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoInptWavfmWndHdl, HWND AdoRsltWavfmWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoInptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoInptDvcID, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoInptIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAddAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDelAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoOtptFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptWavfmWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoOtptDvcID, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoInptUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT VdoInptDvcID, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAddVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDelVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, HWND VdoOtptDspyWnd, float VdoOtptDspyScale, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptStrmUseYU12( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsUseAdoVdoInptOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetIsUsePrvntSysSleep( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt );
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
	virtual void UserReadAdoVdoInptFrm( int16_t * PcmAdoInptFrmPt, int16_t * PcmAdoRsltFrmPt, int32_t VoiceActSts, uint8_t * EncdAdoInptFrmPt, size_t EncdAdoInptFrmLen, int32_t EncdAdoInptFrmIsNeedTrans,
										uint8_t * YU12VdoInptFrmPt, int32_t YU12VdoInptFrmWidth, int32_t YU12VdoInptFrmHeight, uint8_t * EncdVdoInptFrmPt, size_t EncdVdoInptFrmLen ) = 0;

	//用户定义的写入音频输出帧函数。
	virtual void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, uint8_t * EncdAdoOtptFrmPt, size_t * AdoOtptFrmLenPt ) = 0;

	//用户定义的获取PCM格式音频输出帧函数。
	virtual void UserGetPcmAdoOtptFrm( int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, size_t PcmAdoOtptFrmLen ) = 0;

	//用户定义的写入视频输出帧函数。
	virtual void UserWriteVdoOtptFrm( int32_t VdoOtptStrmIdx, uint8_t * YU12VdoOtptFrmPt, int32_t * YU12VdoOtptFrmWidthPt, int32_t * YU12VdoOtptFrmHeightPt, uint8_t * EncdVdoOtptFrmPt, size_t * VdoOtptFrmLenPt ) = 0;

	//用户定义的获取YU12格式视频输出帧函数。
	virtual void UserGetYU12VdoOtptFrm( int32_t VdoOtptStrmIdx, uint8_t * YU12VdoOtptFrmPt, int32_t YU12VdoOtptFrmWidth, int32_t YU12VdoOtptFrmHeight ) = 0;

	int Init( VstrCls * ErrInfoVstrPt );
	int Dstoy( VstrCls * ErrInfoVstrPt ) { int p_Rslt = MediaPocsThrdDstoy( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); m_MediaPocsThrdPt = NULL; return p_Rslt; }

	int GetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoInptDvcName( AdoInptDvcNameArrPtPtPt, AdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoOtptDvcName( AdoOtptDvcNameArrPtPtPt, AdoOtptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetVdoInptDvcName( VdoInptDvcNameArrPtPtPt, VdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdDstoyDvcName( DvcNameArrPtPt, DvcTotal, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInpt( int32_t SmplRate, int32_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInpt( m_MediaPocsThrdPt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptUseNoAec( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseNoAec( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseSpeexAec( int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseSpeexAec( m_MediaPocsThrdPt, FilterLen, IsUseRec, EchoMutp, EchoCntu, EchoSupes, EchoSupesAct, IsSaveMemFile, MemFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseWebRtcAecm( int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseWebRtcAecm( m_MediaPocsThrdPt, IsUseCNGMode, EchoMode, Delay, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseWebRtcAec( int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseWebRtcAec( m_MediaPocsThrdPt, EchoMode, Delay, IsUseDelayAgstcMode, IsUseExtdFilterMode, IsUseRefinedFilterAdaptAecMode, IsUseAdaptAdjDelay, IsSaveMemFile, MemFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseSpeexWebRtcAec( int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseSpeexWebRtcAec( m_MediaPocsThrdPt, WorkMode, SpeexAecFilterLen, SpeexAecIsUseRec, SpeexAecEchoMutp, SpeexAecEchoCntu, SpeexAecEchoSupes, SpeexAecEchoSupesAct, WebRtcAecmIsUseCNGMode, WebRtcAecmEchoMode, WebRtcAecmDelay, WebRtcAecEchoMode, WebRtcAecDelay, WebRtcAecIsUseDelayAgstcMode, WebRtcAecIsUseExtdFilterMode, WebRtcAecIsUseRefinedFilterAdaptAecMode, WebRtcAecIsUseAdaptAdjDelay, IsUseSameRoomAec, SameRoomEchoMinDelay, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptUseNoNs( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseNoNs( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseSpeexPrpocsNs( int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseSpeexPrpocsNs( m_MediaPocsThrdPt, IsUseNs, NoiseSupes, IsUseDereverb, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseWebRtcNsx( int32_t PolicyMode, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseWebRtcNsx( m_MediaPocsThrdPt, PolicyMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseWebRtcNs( int32_t PolicyMode, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseWebRtcNs( m_MediaPocsThrdPt, PolicyMode, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseRNNoise( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseRNNoise( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptIsUseSpeexPrpocsOther( int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptIsUseSpeexPrpocsOther( m_MediaPocsThrdPt, IsUseOther, IsUseVad, VadProbStart, VadProbCntu, IsUseAgc, AgcLevel, AgcIncrement, AgcDecrement, AgcMaxGain, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptUsePcm( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUsePcm( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseSpeexEncd( int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseSpeexEncd( m_MediaPocsThrdPt, UseCbrOrVbr, Qualt, Cmplxt, PlcExptLossRate, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptUseOpusEncd( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseOpusEncd( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptIsSaveAdoToFile( int32_t IsSaveAdoToFile, const Vstr * AdoInptFileFullPathVstrPt, const Vstr * AdoRsltFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptIsSaveAdoToFile( m_MediaPocsThrdPt, IsSaveAdoToFile, AdoInptFileFullPathVstrPt, AdoRsltFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptIsDrawAdoWavfmToWnd( int32_t IsDrawAdoWavfmToWnd, HWND AdoInptWavfmWndHdl, HWND AdoRsltWavfmWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsDrawAdoWavfmToWnd, AdoInptWavfmWndHdl, AdoRsltWavfmWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoInptUseDvc( UINT AdoInptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptUseDvc( m_MediaPocsThrdPt, AdoInptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoInptIsMute( int32_t IsMute, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoInptIsMute( m_MediaPocsThrdPt, IsMute, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtpt( int32_t SmplRate, int32_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtpt( m_MediaPocsThrdPt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int AddAdoOtptStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAddAdoOtptStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DelAdoOtptStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt )  { return MediaPocsThrdDelAdoOtptStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUsePcm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUsePcm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUseSpeexDecd( int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUseSpeexDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, IsUsePrcplEnhsmt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUseOpusDecd( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUseOpusDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmIsUse( int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmIsUse( m_MediaPocsThrdPt, AdoOtptStrmIdx, IsUseAdoOtptStrm, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtptIsSaveAdoToFile( int32_t IsSaveAdoToFile, const Vstr * AdoOtptFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsSaveAdoToFile( m_MediaPocsThrdPt, IsSaveAdoToFile, AdoOtptFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptIsDrawAdoWavfmToWnd( int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptWavfmWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsDrawAdoWavfmToWnd, AdoOtptWavfmWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtptUseDvc( UINT AdoOtptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptUseDvc( m_MediaPocsThrdPt, AdoOtptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptIsMute( int32_t IsMute, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsMute( m_MediaPocsThrdPt, IsMute, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInpt( int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInpt( m_MediaPocsThrdPt, MaxSmplRate, FrmWidth, FrmHeight, VdoInptPrvwWnd, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptUseYU12( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseYU12( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoInptUseOpenH264Encd( int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseOpenH264Encd( m_MediaPocsThrdPt, VdoType, EncdBitrate, BitrateCtrlMode, IDRFrmIntvl, Cmplxt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptUseDvc( UINT VdoInptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseDvc( m_MediaPocsThrdPt, VdoInptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptIsBlack( int IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptIsBlack( m_MediaPocsThrdPt, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int AddVdoOtptStrm( int32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAddVdoOtptStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DelVdoOtptStrm( int32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdDelVdoOtptStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptStrm( int32_t VdoOtptStrmIdx, HWND VdoOtptDspyWnd, float VdoOtptDspyScale, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptStrm( m_MediaPocsThrdPt, VdoOtptStrmIdx, VdoOtptDspyWnd, VdoOtptDspyScale, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptStrmUseYU12( int32_t VdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptStrmUseYU12( m_MediaPocsThrdPt, VdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptStrmUseOpenH264Decd( int32_t VdoOtptStrmIdx, int32_t DecdThrdNum, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptStrmUseOpenH264Decd( m_MediaPocsThrdPt, VdoOtptStrmIdx, DecdThrdNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptStrmIsBlack( int32_t VdoOtptStrmIdx, int32_t IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptStrmIsBlack( m_MediaPocsThrdPt, VdoOtptStrmIdx, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptStrmIsUse( int32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptStrmIsUse( m_MediaPocsThrdPt, VdoOtptStrmIdx, IsUseVdoOtptStrm, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int SetIsUseAdoVdoInptOtpt( int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseAdoVdoInptOtpt( m_MediaPocsThrdPt, IsUseAdoInpt, IsUseAdoOtpt, IsUseVdoInpt, IsUseVdoOtpt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsPrintLogShowToast( int IsPrintLog, int IsShowToast, HWND ShowToastWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsPrintLogShowToast( m_MediaPocsThrdPt, IsPrintLog, IsShowToast, ShowToastWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetIsUsePrvntSysSleep( int32_t IsUsePrvntSysSleep, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUsePrvntSysSleep( m_MediaPocsThrdPt, IsUsePrvntSysSleep, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SaveStngToFile( const Vstr * StngFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSaveStngToFile( m_MediaPocsThrdPt, StngFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int Start( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdStart( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int RqirExit( int ExitFlag, int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdRqirExit( m_MediaPocsThrdPt, ExitFlag, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SendUserMsg( void * MsgArgPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSendUserMsg( m_MediaPocsThrdPt, MsgArgPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
};
#endif

#endif
