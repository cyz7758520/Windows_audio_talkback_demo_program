#include "Func.h"
#include "LnkLst.h"
#include "SpeexDsp.h"
#include "WebRtc.h"
#include "SpeexWebRtcAec.h"
#include "RNNoise.h"
#include "Speex.h"
#include "WaveFile.h"
#include "AdoWavfm.h"
#include "LibYUV.h"
#include "OpenH264.h"

#ifndef __MEDIAPROCTHREAD_H__
#define __MEDIAPROCTHREAD_H__

typedef struct IGraphBuilder IGraphBuilder;
typedef struct IMediaControl IMediaControl;
typedef struct IMediaEventEx IMediaEventEx;
typedef struct IBaseFilter IBaseFilter;
typedef struct VdoInptThrd VdoInptThrd;
typedef struct MediaPocsThrd MediaPocsThrd;

typedef int( __cdecl * USER_INIT_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef int( __cdecl * USER_POCS_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef void( __cdecl * USER_DSTOY_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt );

typedef void( __cdecl * USER_READ_ADO_VDO_INPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAdoInptFrmPt, int16_t * PcmAdoRsltFrmPt, int32_t VoiceActSts, uint8_t * EncdAdoInptFrmPt, size_t EncdAdoInptFrmLen, int32_t EncdAdoInptFrmIsNeedTrans,
															  uint8_t * YU12VdoInptFrmPt, int32_t YU12VdoInptFrmWidth, int32_t YU12VdoInptFrmHeight, uint8_t * EncdVdoInptFrmPt, size_t EncdVdoInptFrmLen );

typedef void( __cdecl * USER_WRITE_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, uint8_t * EncdAdoOtptFrmPt, size_t * AdoOtptFrmLenPt );

typedef void( __cdecl * USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, size_t PcmAdoOtptFrmLen );

typedef void( __cdecl * USER_WRITE_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VdoOtptFrmPt, int32_t * YU12VdoOtptFrmWidthPt, int32_t * YU12VdoOtptFrmHeightPt, uint8_t * EncdVdoOtptFrmPt, size_t * VdoOtptFrmLenPt );

typedef void( __cdecl * USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT )( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VdoOtptFrmPt, int32_t YU12VdoOtptFrmWidth, int32_t YU12VdoOtptFrmHeight );

//媒体处理线程。
typedef struct MediaPocsThrd
{
	int32_t m_RunFlag; //存放本线程运行标记。
	#define RUN_FLAG_NORUN 0 //运行标记：未开始运行。
    #define RUN_FLAG_INIT  1 //运行标记：刚开始运行正在初始化。
    #define RUN_FLAG_PROC  2 //运行标记：初始化完毕正在循环处理帧。
    #define RUN_FLAG_DSTOY 3 //运行标记：跳出循环处理帧正在销毁。
    #define RUN_FLAG_END   4 //运行标记：销毁完毕。
    int32_t m_ExitFlag; //存放本线程退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
    int32_t m_ExitCode; //存放本线程退出代码，为0表示正常退出，为-1表示初始化失败，为-2表示处理失败。

    int32_t m_IsSaveStngToFile; //存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
    Vstr * m_StngFileFullPathVstrPt; //存放设置文件的完整路径动态字符串的指针。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。

	typedef struct AdoInpt //音频输入结构体。
	{
		int32_t m_IsUseAdoInpt; //存放是否使用音频输入，为0表示不使用，为非0表示要使用。
		
		int32_t m_SmplRate; //存放采样频率，单位赫兹，取值只能为8000、16000、32000。
		int32_t m_FrmLen; //存放帧的长度，单位采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。

		int32_t m_UseWhatAec; //存放使用什么声学回音消除器，为0表示不使用，为1表示Speex声学回音消除器，为2表示WebRtc定点版声学回音消除器，为2表示WebRtc浮点版声学回音消除器，为4表示SpeexWebRtc三重声学回音消除器。

		SpeexAec * m_SpeexAecPt; //存放Speex声学回音消除器的指针。
		int32_t m_SpeexAecFilterLen; //存放Speex声学回音消除器滤波器的长度，单位毫秒。
		int32_t m_SpeexAecIsUseRec; //存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
		float m_SpeexAecEchoMutp; //存放Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
		float m_SpeexAecEchoCntu; //存放Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
		int32_t m_SpeexAecEchoSupes; //存放Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexAecEchoSupesAct; //存放Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexAecIsSaveMemFile; //存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		Vstr * m_SpeexAecMemFileFullPathVstrPt; //存放Speex声学回音消除器的内存块文件完整路径动态字符串的指针。

		WebRtcAecm * m_WebRtcAecmPt; //存放WebRtc定点版声学回音消除器的指针。
		int32_t m_WebRtcAecmIsUseCNGMode; //存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecmEchoMode; //存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
		int32_t m_WebRtcAecmDelay; //存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。

		WebRtcAec * m_WebRtcAecPt; //存放WebRtc浮点版声学回音消除器的指针。
		int32_t m_WebRtcAecEchoMode; //存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
		int32_t m_WebRtcAecDelay; //存放WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_WebRtcAecIsUseDelayAgstcMode; //存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseExtdFilterMode; //存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode; //存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseAdaptAdjDelay; //存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsSaveMemFile; //存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		Vstr * m_WebRtcAecMemFileFullPathVstrPt; //存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针。

		SpeexWebRtcAec * m_SpeexWebRtcAecPt; //存放SpeexWebRtc三重声学回音消除器的指针。
		int32_t m_SpeexWebRtcAecWorkMode; //存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
		int32_t m_SpeexWebRtcAecSpeexAecFilterLen; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器滤波器的长度，单位毫秒。
		int32_t m_SpeexWebRtcAecSpeexAecIsUseRec; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
		float m_SpeexWebRtcAecSpeexAecEchoMutp; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
		float m_SpeexWebRtcAecSpeexAecEchoCntu; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
		int32_t m_SpeexWebRtcAecSpeexAecEchoSupes; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexWebRtcAecSpeexAecEchoSupesAct; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecmEchoMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
		int32_t m_SpeexWebRtcAecWebRtcAecmDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_SpeexWebRtcAecWebRtcAecEchoMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
		int32_t m_SpeexWebRtcAecWebRtcAecDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecIsUseSameRoomAec; //存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecSameRoomEchoMinDelay; //存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位毫秒，取值区间为[1,2147483647]。

		int32_t m_UseWhatNs; //存放使用什么噪音抑制器，为0表示不使用，为1表示Speex预处理器的噪音抑制，为2表示WebRtc定点版噪音抑制器，为3表示WebRtc浮点版噪音抑制器，为4表示RNNoise噪音抑制器。

		SpeexPreprocessState * m_SpeexPrpocsPt; //存放Speex预处理器的指针。
		int32_t m_SpeexPrpocsIsUseNs; //存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPrpocsNoiseSupes; //存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexPrpocsIsUseDereverb; //存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。

		WebRtcNsx * m_WebRtcNsxPt; //存放WebRtc定点版噪音抑制器的指针。
		int32_t m_WebRtcNsxPolicyMode; //存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

		WebRtcNs * m_WebRtcNsPt; //存放WebRtc浮点版噪音抑制器的指针。
		int32_t m_WebRtcNsPolicyMode; //存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

		RNNoise * m_RNNoisePt; //存放RNNoise噪音抑制器的指针。

		int32_t m_IsUseSpeexPrpocsOther; //存放是否使用Speex预处理器的其他功能，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPrpocsIsUseVad; //存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPrpocsVadProbStart; //存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
		int32_t m_SpeexPrpocsVadProbCntu; //存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
		int32_t m_SpeexPrpocsIsUseAgc; //存放Speex预处理器是否使用自动增益控制，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPrpocsAgcLevel; //存放Speex预处理器在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
		int32_t m_SpeexPrpocsAgcIncrement; //存放Speex预处理器在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
		int32_t m_SpeexPrpocsAgcDecrement; //存放Speex预处理器在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexPrpocsAgcMaxGain; //存放Speex预处理器在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。

		int32_t m_UseWhatEncd; //存放使用什么编码器，为0表示PCM原始数据，为1表示Speex编码器，为2表示Opus编码器。

		SpeexEncd * m_SpeexEncdPt; //存放Speex编码器的指针。
		int32_t m_SpeexEncdUseCbrOrVbr; //存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
		int32_t m_SpeexEncdQualt; //存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
		int32_t m_SpeexEncdCmplxt; //存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
		int32_t m_SpeexEncdPlcExptLossRate; //存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
		
		int32_t m_IsSaveAdoToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
		WaveFileWriter * m_AdoInptWaveFileWriterPt; //存放音频输入Wave文件写入器的指针。
		WaveFileWriter * m_AdoRsltWaveFileWriterPt; //存放音频结果Wave文件写入器的指针。
		Vstr * m_AdoInptFileFullPathVstrPt; //存放音频输入文件完整路径动态字符串的指针。
		Vstr * m_AdoRsltFileFullPathVstrPt; //存放音频结果文件完整路径动态字符串的指针。

        int32_t m_IsDrawAdoWavfmToWnd; //存放是否绘制音频波形到窗口，为非0表示要绘制，为0表示不绘制。
		HWND m_AdoInptWavfmWndHdl; //存放音频输入波形窗口的句柄。
        AdoWavfm * m_AdoInptWavfmPt; //存放音频输入波形器对象的指针。
        HWND m_AdoRsltWavfmWndHdl; //存放音频结果波形窗口的句柄。
        AdoWavfm * m_AdoRsltWavfmPt; //存放音频结果波形器对象的指针。

		UINT m_AdoInptDvcID; //存放音频输入设备的标识符。
		HWAVEIN m_AdoInptDvcHdl; //存放音频输入设备的句柄。
		int32_t m_AdoInptDvcIsClos; //存放音频输入设备是否关闭，为0表示正常，为非0表示关闭。
		WAVEHDR * m_AdoInptWaveHdrPt; //存放音频输入缓冲区块数组的指针。
		int32_t m_AdoInptWaveHdrTotal; //存放音频输入缓冲区块数组的元素总数。
		int32_t m_AdoInptIsMute; //存放音频输入是否静音，为0表示有声音，为非0表示静音。

		ConstLenLnkLstCls m_AdoInptFrmLnkLst; //存放音频输入帧链表。
		ConstLenLnkLstCls m_AdoInptIdleFrmLnkLst; //存放音频输入空闲帧链表。
		size_t m_AdoInptFrmElmTotal; //存放音频输入帧元素的总数。

		//音频输入线程的临时变量。
		MSG m_Msg; //存放消息。
		int16_t * m_AdoInptFrmPt; //存放音频输入帧的指针。
		size_t m_AdoInptFrmLnkLstElmTotal; //存放音频输入帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_AdoInptThrdHdl; //存放音频输入线程的句柄。
		DWORD m_AdoInptThrdId; //存放音频输入线程的标识符。
		int m_AdoInptThrdExitFlag; //存放音频输入线程退出标记，0表示保持运行，1表示请求退出。
	} AdoInpt;

	AdoInpt m_AdoInpt; //存放音频输入。

	typedef struct AdoOtpt //音频输出结构体。
	{
		int32_t m_IsUseAdoOtpt; //存放是否使用音频输出，为0表示不使用，为非0表示要使用。

		int32_t m_SmplRate; //存放采样频率，单位赫兹，取值只能为8000、16000、32000。
		int32_t m_FrmLen; //存放帧的长度，单位采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
		
		typedef struct AdoOtptStrmInfo //音频输出流信息。
		{
			int32_t m_AdoOtptStrmIdx; //存放音频输出流索引。

			int32_t m_UseWhatDecd; //存放使用什么解码器，为0表示PCM原始数据，为1表示Speex解码器，为2表示Opus解码器。

			int32_t m_SpeexDecdIsUsePrcplEnhsmt; //存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
		} AdoOtptStrmInfo;
		ConstLenLnkLstCls m_AdoOtptStrmInfoLnkLst; //存放音频输出流信息链表。
		typedef struct AdoOtptStrmData //音频输出流数据。
		{
			int32_t m_AdoOtptStrmIdx; //存放音频输出流索引。

			int32_t m_UseWhatDecd; //存放使用什么解码器，为0表示PCM原始数据，为1表示Speex解码器，为2表示Opus解码器。

			SpeexDecd * m_SpeexDecdPt; //存放Speex解码器的指针。
		} AdoOtptStrmData;
		ConstLenLnkLstCls m_AdoOtptStrmDataLnkLst; //存放音频输出流数据链表。

		int32_t m_IsSaveAdoToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
		WaveFileWriter * m_AdoOtptWaveFileWriterPt; //存放音频输出Wave文件写入器的指针。
		Vstr * m_AdoOtptFileFullPathVstrPt; //存放音频输出文件完整路径动态字符串的指针。
		
        int32_t m_IsDrawAdoWavfmToWnd; //存放是否绘制音频波形到窗口，为非0表示要绘制，为0表示不绘制。
        HWND m_AdoOtptWavfmWndHdl; //存放音频输出波形窗口的句柄。
        AdoWavfm * m_AdoOtptWavfmPt; //存放音频输出波形器对象的指针。

		UINT m_AdoOtptDvcID; //存放音频输出设备的标识符。
		HWAVEOUT m_AdoOtptDvcHdl; //存放音频输出设备的句柄。
		int32_t m_AdoOtptDvcIsClos; //存放音频输出设备是否关闭，为0表示正常，为非0表示关闭。
		WAVEHDR * m_AdoOtptWaveHdrPt; //存放音频缓冲区块数组的指针。
		int32_t m_AdoOtptWaveHdrTotal; //存放音频输出缓冲区块数组的元素总数。
		int32_t m_AdoOtptIsMute; //存放音频输出是否静音，为0表示有声音，为非0表示静音。

		ConstLenLnkLstCls m_AdoOtptFrmLnkLst; //存放音频输出帧链表。
		ConstLenLnkLstCls m_AdoOtptIdleFrmLnkLst; //存放音频输出空闲帧链表。
		size_t m_AdoOtptFrmElmTotal; //存放音频输出帧元素的总数。

		//音频输出线程的临时变量。
		MSG m_Msg; //存放消息。
		int16_t * m_AdoOtptFrmPt; //存放音频输出帧的指针。
		int32_t * m_AdoOtptMixFrmPt; //存放音频输出混音帧的指针。
		size_t m_AdoOtptMixFrmSz; //存放音频输出混音帧的大小，单位为字节。
		uint8_t * m_EncdAdoOtptFrmPt; //存放已编码格式音频输出帧的指针。
		size_t m_EncdAdoOtptFrmSz; //存放已编码格式音频输出帧的大小，单位为字节。
		size_t m_AdoOtptFrmLen; //存放音频输出帧的长度，单位为字节。
		size_t m_AdoOtptFrmLnkLstElmTotal; //存放音频输出帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_AdoOtptThrdHdl; //存放音频输出线程的句柄。
		DWORD m_AdoOtptThrdId; //存放音频输出线程的标识符。
		int m_AdoOtptThrdExitFlag; //存放音频输出线程退出标记，0表示保持运行，1表示请求退出。
	} AdoOtpt;

	AdoOtpt m_AdoOtpt; //存放音频输出。

	typedef struct VdoInpt
	{
		int32_t m_IsUseVdoInpt; //存放是否使用视频输入，为0表示不使用，为非0表示要使用。
		
        int32_t m_MaxSmplRate; //存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
        int32_t m_FrmWidth; //存放帧的宽度，单位为像素。
        int32_t m_FrmHeight; //存放帧的高度，单位为像素。
		
        int32_t m_UseWhatEncd; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264编码器。

        OpenH264Encd * m_OpenH264EncdPt; //存放OpenH264编码器的指针。
        int32_t m_OpenH264EncdVdoType;//存放OpenH264编码器的视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
        int32_t m_OpenH264EncdEncdBitrate; //存放OpenH264编码器的编码后比特率，单位为bps。
        int32_t m_OpenH264EncdBitrateCtrlMode; //存放OpenH264编码器的比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
        int32_t m_OpenH264EncdIDRFrmIntvl; //存放OpenH264编码器的IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
        int32_t m_OpenH264EncdCmplxt; //存放OpenH264编码器的复杂度，复杂度越高压缩率不变、CPU使用率越高、画质越好，取值区间为[0,2]。

		UINT m_VdoInptDvcID; //存放视频输入设备的标识符。
		IGraphBuilder * m_FilterGraphManagerPt; //存放视频输入过滤器图管理器的指针。
		IMediaEventEx * m_FilterGraphMediaEventPt; //存放视频输入过滤器图媒体事件器的指针。
		IMediaControl * m_FilterGraphMediaCtrlPt; //存放视频输入过滤器图媒体控制器的指针。
		int32_t m_VdoInptDvcMaxSmplRate; //存放视频输入设备的最大采样频率。
		int32_t m_VdoInptDvcFrmWidth; //存放视频输入设备帧的宽度，单位为像素。
        int32_t m_VdoInptDvcFrmHeight; //存放视频输入设备帧的高度，单位为像素。
		int32_t m_VdoInptDvcFrmIsCrop; //存放视频输入设备帧是否裁剪，为0表示不裁剪，为非0表示要裁剪。
		int32_t m_VdoInptDvcFrmCropX; //存放视频输入设备帧裁剪区域左上角的横坐标，单位像素。
        int32_t m_VdoInptDvcFrmCropY; //存放视频输入设备帧裁剪区域左上角的纵坐标，单位像素。
		int32_t m_VdoInptDvcFrmCropWidth; //存放视频输入设备帧裁剪区域的宽度，单位像素。
        int32_t m_VdoInptDvcFrmCropHeight; //存放视频输入设备帧裁剪区域的高度，单位像素。
		int32_t m_VdoInptDvcFrmIsScale; //存放视频输入设备帧是否缩放，为0表示不缩放，为非0表示要缩放。
		int32_t m_VdoInptDvcFrmScaleWidth; //存放视频输入帧缩放后的宽度，单位为像素。
        int32_t m_VdoInptDvcFrmScaleHeight; //存放视频输入帧缩放后的高度，单位为像素。
		int32_t m_VdoInptIsBlack; //存放视频输入是否黑屏，为0表示有图像，为非0表示黑屏。
		HWND m_VdoInptPrvwWndHdl; //存放视频输入预览窗口的句柄。
		
		typedef struct VdoInptFrmElm //视频输入帧链表元素。
		{
			uint8_t * m_YU12VdoInptFrmPt; //存放YU12格式视频输入帧的指针，大小为m_FrmWidth * m_FrmHeight * 3 / 2。
			uint8_t * m_EncdVdoInptFrmPt; //存放已编码格式视频输入帧的指针，大小为m_FrmWidth * m_FrmHeight * 3 / 2。
			size_t m_EncdVdoInptFrmLen; //存放已编码格式视频输入帧的长度，单位为字节。
		}VdoInptFrmElm;
		ConstLenLnkLstCls m_VdoInptFrmLnkLst; //存放视频输入帧链表。
		ConstLenLnkLstCls m_VdoInptIdleFrmLnkLst; //存放视频输入空闲帧链表。
		size_t m_VdoInptFrmElmTotal; //存放视频输入帧元素的总数。

		//视频输入线程的临时变量。
		uint64_t m_LastVdoInptFrmTimeMsec; //存放上一个视频输入帧的时间，单位毫秒。
		uint64_t m_VdoInptFrmTimeStepMsec; //存放视频输入帧的时间步进，单位毫秒。
		uint8_t * m_VdoInptRsltFrmPt; //存放视频输入结果帧的指针。
		uint8_t * m_VdoInptTmpFrmPt; //存放视频输入临时帧的指针。
		uint8_t * m_VdoInptSwapFrmPt; //存放视频输入交换帧的指针。
		size_t m_VdoInptRsltFrmSz; //存放视频输入结果帧的大小，单位为字节。
		size_t m_VdoInptRsltFrmLen; //存放视频输入结果帧的长度，单位为字节。
		VdoInptFrmElm * m_VdoInptFrmElmPt; //存放视频输入帧元素的指针。
		size_t m_VdoInptFrmLnkLstElmTotal; //存放视频输入帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		VdoInptThrd * m_VdoInptThrdPt; //存放视频输入线程的指针。
	}VdoInpt;
	VdoInpt m_VdoInpt; //存放视频输入。
	
    typedef struct VdoOtpt //视频输出类。
    {
        int32_t m_IsUseVdoOtpt; //存放是否使用视频输出，为0表示不使用，为非0表示要使用。

        int32_t m_UseWhatDecd; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264解码器。

        OpenH264Decd * m_OpenH264DecdPt; //存放OpenH264解码器的指针。
        int32_t m_OpenH264DecdDecdThrdNum; //存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。

        HWND m_VdoOtptDspyWndHdl; //存放视频输出显示窗口的句柄。
        float m_VdoOtptDspyScale; //存放视频输出显示缩放倍数。
        int32_t m_VdoOtptIsBlack; //存放视频输出是否黑屏，为0表示有图像，为非0表示黑屏。
		
		//视频输出线程的临时变量。
		uint8_t * m_VdoOtptRsltFrmPt; //存放视频输出结果帧的指针。
		uint8_t * m_VdoOtptTmpFrmPt; //存放视频输出临时帧的指针。
		uint8_t * m_VdoOtptSwapFrmPt; //存放视频输出交换帧的指针。
		size_t m_VdoOtptRsltFrmSz; //存放视频输出结果帧的大小，单位为字节。
		size_t m_VdoOtptRsltFrmLen; //存放视频输出结果帧的长度，单位为字节。
		int32_t m_VdoOtptFrmWidth; //存放视频输出帧的宽度，单位为像素。
        int32_t m_VdoOtptFrmHeight; //存放视频输出帧的高度，单位为像素。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_VdoOtptThrdHdl; //存放视频输出线程的句柄。
		DWORD m_VdoOtptThrdId; //存放视频输出线程的标识符。
		int m_VdoOtptThrdExitFlag; //视频输出线程退出标记，0表示保持运行，1表示请求退出。
	}VdoOtpt;

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
	VdoInpt::VdoInptFrmElm * m_VdoInptFrmPt; //存放视频输入帧的指针。

	HANDLE m_MediaPocsThrdHdl; //存放媒体处理线程的句柄。

	Vstr * m_ErrInfoVstrPt; //存放错误信息动态字符串的指针。

	//用户定义的相关回调函数。

	//存放用户数据的指针。
	void * m_UserDataPt;
	
	//用户定义的初始化函数，在本线程刚启动时回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_INIT_FUNC_PT m_UserInitFuncPt;

	//用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_POCS_FUNC_PT m_UserPocsFuncPt;

	//用户定义的销毁函数，在本线程退出时回调一次。
	USER_DSTOY_FUNC_PT m_UserDstoyFuncPt;

	//用户定义的读取音视频输入帧函数，在读取到一个音频输入帧或视频输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
	USER_READ_ADO_VDO_INPT_FRM_FUNC_PT m_UserReadAdoVdoInptFrmFuncPt;

	//用户定义的写入音频输出帧函数，在需要写入一个音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
	USER_WRITE_ADO_OTPT_FRM_FUNC_PT m_UserWriteAdoOtptFrmFuncPt;

	//用户定义的获取PCM格式音频输出帧函数，在解码完一个已编码音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
	USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT m_UserGetPcmAdoOtptFrmFuncPt;

	//用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
	USER_WRITE_VDO_OTPT_FRM_FUNC_PT m_UserWriteVdoOtptFrmFuncPt;

	//用户定义的获取YU12格式视频输出帧函数，在解码完一个已编码视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
	USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT m_UserGetYU12VdoOtptFrmFuncPt;
}MediaPocsThrd;

#ifdef __cplusplus
extern "C"
{
#endif

int MediaPocsThrdInit( MediaPocsThrd ** MediaPocsThrdPtPt, void * UserDataPt,
					   USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt,
					   USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT UserGetPcmAdoOtptFrmFuncPt,
					   USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT UserGetYU12VdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, int IsSaveStngToFile, const char * StngFileFullPathStrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int IsPrintLog, int IsShowToast, Vstr * ErrInfoVstrPt );

int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsUseAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoInpt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt );

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

int MediaPocsThrdSetIsUseAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoOtpt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt );

int MediaPocsThrdAddAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );
int MediaPocsThrdDelAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoOtptFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptWavfmWndHdl, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetAdoOtptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoOtptDvcID, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetAdoOtptIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsUseVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVdoInpt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoInptUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT VdoInptDvcID, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoInptIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetIsUseVdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVdoOtpt, HWND VdoOtptDspyWnd, float VdoOtptDspyScale, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoOtptUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdSetVdoOtptUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt );

int MediaPocsThrdSetVdoOtptIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt );

int MediaPocsThrdStart( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );
int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int ExitFlag, int IsBlockWait, Vstr * ErrInfoVstrPt );

int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt );

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

	int Init( void * UserDataPt,
			  USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt,
			  USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
			  USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT UserGetPcmAdoOtptFrmFuncPt,
			  USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT UserGetYU12VdoOtptFrmFuncPt,
			  VstrCls * ErrInfoVstrPt )
	{ return MediaPocsThrdInit( &m_MediaPocsThrdPt, UserDataPt,
								UserInitFuncPt, UserPocsFuncPt, UserDstoyFuncPt,
								UserReadAdoVdoInptFrmFuncPt,
								UserWriteAdoOtptFrmFuncPt, UserGetPcmAdoOtptFrmFuncPt,
								UserWriteVdoOtptFrmFuncPt, UserGetYU12VdoOtptFrmFuncPt,
								( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsSaveStngToFile( int IsSaveStngToFile, const char * StngFileFullPathStrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsSaveStngToFile( m_MediaPocsThrdPt, IsSaveStngToFile, StngFileFullPathStrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetIsPrintLogShowToast( int IsPrintLog, int IsShowToast, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsPrintLogShowToast( m_MediaPocsThrdPt, IsPrintLog, IsShowToast, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int GetAdoInptDvcName( Vstr * * * AdoInptDvcNameArrPtPtPt, UINT * AdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoInptDvcName( AdoInptDvcNameArrPtPtPt, AdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameArrPtPtPt, UINT * AdoOtptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetAdoOtptDvcName( AdoOtptDvcNameArrPtPtPt, AdoOtptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int GetVdoInptDvcName( Vstr * * * VdoInptDvcNameArrPtPtPt, UINT * VdoInptDvcTotalPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdGetVdoInptDvcName( VdoInptDvcNameArrPtPtPt, VdoInptDvcTotalPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdDstoyDvcName( DvcNameArrPtPt, DvcTotal, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsUseAdoInpt( int32_t IsUseAdoInpt, int32_t SmplRate, int32_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseAdoInpt( m_MediaPocsThrdPt, IsUseAdoInpt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

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

	int SetIsUseAdoOtpt( int32_t IsUseAdoOtpt, int32_t SmplRate, int32_t FrmLenMsec, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseAdoOtpt( m_MediaPocsThrdPt, IsUseAdoOtpt, SmplRate, FrmLenMsec, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	
	int AddAdoOtptStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdAddAdoOtptStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUsePcm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUsePcm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUseSpeexDecd( int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUseSpeexDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, IsUsePrcplEnhsmt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptStrmUseOpusDecd( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptStrmUseOpusDecd( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int DelAdoOtptStrm( int32_t AdoOtptStrmIdx, VstrCls * ErrInfoVstrPt )  { return MediaPocsThrdDelAdoOtptStrm( m_MediaPocsThrdPt, AdoOtptStrmIdx, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtptIsSaveAdoToFile( int32_t IsSaveAdoToFile, const Vstr * AdoOtptFileFullPathVstrPt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsSaveAdoToFile( m_MediaPocsThrdPt, IsSaveAdoToFile, AdoOtptFileFullPathVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptIsDrawAdoWavfmToWnd( int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptWavfmWndHdl, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( m_MediaPocsThrdPt, IsDrawAdoWavfmToWnd, AdoOtptWavfmWndHdl, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetAdoOtptUseDvc( UINT AdoOtptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptUseDvc( m_MediaPocsThrdPt, AdoOtptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetAdoOtptIsMute( int32_t IsMute, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetAdoOtptIsMute( m_MediaPocsThrdPt, IsMute, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsUseVdoInpt( int32_t IsUseVdoInpt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseVdoInpt( m_MediaPocsThrdPt, IsUseVdoInpt, MaxSmplRate, FrmWidth, FrmHeight, VdoInptPrvwWnd, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptUseYU12( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseYU12( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoInptUseOpenH264Encd( int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseOpenH264Encd( m_MediaPocsThrdPt, VdoType, EncdBitrate, BitrateCtrlMode, IDRFrmIntvl, Cmplxt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptUseDvc( UINT VdoInptDvcID, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptUseDvc( m_MediaPocsThrdPt, VdoInptDvcID, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoInptIsBlack( int IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoInptIsBlack( m_MediaPocsThrdPt, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetIsUseVdoOtpt( int32_t IsUseVdoOtpt, HWND VdoOtptDspyWnd, float VdoOtptDspyScale, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetIsUseVdoOtpt( m_MediaPocsThrdPt, IsUseVdoOtpt, VdoOtptDspyWnd, VdoOtptDspyScale, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoOtptUseYU12( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptUseYU12( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int SetVdoOtptUseOpenH264Decd( int32_t DecdThrdNum, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptUseOpenH264Decd( m_MediaPocsThrdPt, DecdThrdNum, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int SetVdoOtptIsBlack( int IsBlack, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdSetVdoOtptIsBlack( m_MediaPocsThrdPt, IsBlack, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int Start( VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdStart( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }
	int RqirExit( int ExitFlag, int IsBlockWait, VstrCls * ErrInfoVstrPt ) { return MediaPocsThrdRqirExit( m_MediaPocsThrdPt, ExitFlag, IsBlockWait, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

	int Dstoy( VstrCls * ErrInfoVstrPt ) { int p_Rslt = MediaPocsThrdDstoy( m_MediaPocsThrdPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); m_MediaPocsThrdPt = NULL; return p_Rslt; }
};
#endif

#endif
