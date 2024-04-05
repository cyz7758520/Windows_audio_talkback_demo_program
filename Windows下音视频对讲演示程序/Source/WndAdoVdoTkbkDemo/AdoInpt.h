#pragma once

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

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MediaPocsThrd MediaPocsThrd;

typedef struct AdoInpt //存放音频输入。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUse; //存放是否使用音频输入，为0表示不使用，为非0表示要使用。
	int32_t m_IsInit; //存放是否初始化音频输入，为0表示未初始化，为非0表示已初始化。

	int32_t m_SmplRate; //存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
	size_t m_FrmLenMsec; //存放帧的长度，单位为毫秒，取值只能为10毫秒的倍数。
	size_t m_FrmLenUnit; //存放帧的长度，单位为采样单元，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	size_t m_FrmLenData; //存放帧的长度，单位为采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	size_t m_FrmLenByt; //存放帧的长度，单位为字节，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80*2、20毫秒为160*2、30毫秒为240*2，16000Hz的10毫秒为160*2、20毫秒为320*2、30毫秒为480*2，32000Hz的10毫秒为320*2、20毫秒为640*2、30毫秒为960*2，48000Hz的10毫秒为480*2、20毫秒为960*2、30毫秒为1440*2。
	
	int32_t m_IsUseSystemAecNsAgc; //存放是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器，为0表示不使用，为非0表示要使用。

	int32_t m_UseWhatAec; //存放使用什么声学回音消除器，为0表示不使用，为1表示Speex声学回音消除器，为2表示WebRtc定点版声学回音消除器，为2表示WebRtc浮点版声学回音消除器，为4表示SpeexWebRtc三重声学回音消除器。
	int32_t m_IsCanUseAec; //存放是否可以使用声学回音消除器，为0表示不可以，为非0表示可以。

	struct //存放Speex声学回音消除器。
	{
		SpeexAec * m_Pt; //存放指针。
		int32_t m_FilterLenMsec; //存放滤波器的长度，单位为毫秒。
		int32_t m_IsUseRec; //存放是否使用残余回音消除，为非0表示要使用，为0表示不使用。
		float m_EchoMutp; //存放在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
		float m_EchoCntu; //存放在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
		int32_t m_EchoSupes; //存放在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_EchoSupesAct; //存放在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_IsSaveMemFile; //存放是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		Vstr * m_MemFileFullPathVstrPt; //存放内存块文件完整路径动态字符串的指针。
	} m_SpeexAec;

	struct //存放WebRtc定点版声学回音消除器。
	{
		WebRtcAecm * m_Pt; //存放指针。
		int32_t m_IsUseCNGMode; //存放是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
		int32_t m_EchoMode; //存放消除模式，消除模式越高消除越强，取值区间为[0,4]。
		int32_t m_Delay; //存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
	} m_WebRtcAecm;

	struct //存放WebRtc浮点版声学回音消除器。
	{
		WebRtcAec * m_Pt; //存放指针。
		int32_t m_EchoMode; //存放消除模式，消除模式越高消除越强，取值区间为[0,2]。
		int32_t m_Delay; //存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_IsUseDelayAgstcMode; //存放是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_IsUseExtdFilterMode; //存放是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_IsUseRefinedFilterAdaptAecMode; //存放是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_IsUseAdaptAdjDelay; //存放是否使用自适应调节回音延迟，为非0表示要使用，为0表示不使用。
		int32_t m_IsSaveMemFile; //存放是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		Vstr * m_MemFileFullPathVstrPt; //存放内存块文件完整路径动态字符串的指针。
	} m_WebRtcAec;

	struct //存放SpeexWebRtc三重声学回音消除器。
	{
		SpeexWebRtcAec * m_Pt; //存放指针。
		int32_t m_WorkMode; //存放工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
		int32_t m_SpeexAecFilterLenMsec; //存放Speex声学回音消除器的滤波器的长度，单位为毫秒。
		int32_t m_SpeexAecIsUseRec; //存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
		float m_SpeexAecEchoMutp; //存放Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
		float m_SpeexAecEchoCntu; //存放Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
		int32_t m_SpeexAecEchoSupes; //存放Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexAecEchoSupesAct; //存放Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_WebRtcAecmIsUseCNGMode; //存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecmEchoMode; //存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
		int32_t m_WebRtcAecmDelay; //存放WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_WebRtcAecEchoMode; //存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
		int32_t m_WebRtcAecDelay; //存放WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_WebRtcAecIsUseDelayAgstcMode; //存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseExtdFilterMode; //存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode; //存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseAdaptAdjDelay; //存放WebRtc浮点版声学回音消除器是否使用自适应调节回音延迟，为非0表示要使用，为0表示不使用。
		int32_t m_IsUseSameRoomAec; //存放是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
		int32_t m_SameRoomEchoMinDelay; //存放同一房间回音最小延迟，单位为毫秒，取值区间为[1,2147483647]。
	} m_SpeexWebRtcAec;

	int32_t m_UseWhatNs; //存放使用什么噪音抑制器，为0表示不使用，为1表示Speex预处理器的噪音抑制，为2表示WebRtc定点版噪音抑制器，为3表示WebRtc浮点版噪音抑制器，为4表示RNNoise噪音抑制器。

	struct //存放Speex预处理器的噪音抑制。
	{
		int32_t m_IsUseNs; //存放是否使用噪音抑制，为非0表示要使用，为0表示不使用。
		int32_t m_NoiseSupes; //存放在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_IsUseDereverb; //存放是否使用混响音消除，为非0表示要使用，为0表示不使用。
	} m_SpeexPrpocsNs;

	struct //存放WebRtc定点版噪音抑制器。
	{
		WebRtcNsx * m_Pt; //存放指针。
		int32_t m_PolicyMode; //存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
	} m_WebRtcNsx;

	struct //存放WebRtc浮点版噪音抑制器。
	{
		WebRtcNs * m_Pt; //存放指针。
		int32_t m_PolicyMode; //存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
	} m_WebRtcNs;

	struct //存放RNNoise噪音抑制器。
	{
		RNNoise * m_Pt; //存放指针。
	} m_RNNoise;
	
	struct //存放Speex预处理器。
	{
		int32_t m_IsUseSpeexPrpocs; //存放是否使用Speex预处理器，为非0表示要使用，为0表示不使用。
		SpeexPreprocessState * m_Pt; //存放指针。
		int32_t m_IsUseVad; //存放是否使用语音活动检测，为非0表示要使用，为0表示不使用。
		int32_t m_VadProbStart; //存放在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
		int32_t m_VadProbCntu; //存放在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
		int32_t m_IsUseAgc; //存放是否使用自动增益控制，为非0表示要使用，为0表示不使用。
		int32_t m_AgcLevel; //存放在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
		int32_t m_AgcIncrement; //存放在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
		int32_t m_AgcDecrement; //存放在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
		int32_t m_AgcMaxGain; //存放在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
	} m_SpeexPrpocs;

	int32_t m_UseWhatEncd; //存放使用什么编码器，为0表示Pcm原始数据，为1表示Speex编码器，为2表示Opus编码器。

	struct //存放Speex编码器。
	{
		SpeexEncd * m_Pt; //存放指针。
		int32_t m_UseCbrOrVbr; //存放使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
		int32_t m_Qualt; //存放编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
		int32_t m_Cmplxt; //存放编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
		int32_t m_PlcExptLossRate; //存放在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
	} m_SpeexEncd;

	struct //存放波形器。
	{
		int32_t m_IsDraw; //存放是否绘制，为非0表示要绘制，为0表示不绘制。
		AdoWavfm * m_SrcPt; //存放原始的指针。
		AdoWavfm * m_RsltPt; //存放结果的指针。
		HWND m_SrcWndHdl; //存放原始窗口的句柄。
		HWND m_RsltWndHdl; //存放结果窗口的句柄。
	} m_Wavfm;
	
	struct //存放Wave文件写入器。
	{
		int32_t m_IsSave; //存放是否保存，为非0表示要保存，为0表示不保存。
		WaveFileWriter * m_SrcWriterPt; //存放原始的指针。
		WaveFileWriter * m_RsltWriterPt; //存放结果的指针。
		Vstr * m_SrcFullPathVstrPt; //存放原始完整路径动态字符串的指针。
		Vstr * m_RsltFullPathVstrPt; //存放结果完整路径动态字符串的指针。
		size_t m_WrBufSzByt; //存放写入缓冲区的大小，单位为字节。
	} m_WaveFileWriter;

	struct //存放设备。
	{
		UINT m_ID; //存放标识符，取值范围为从1到音频输入设备的总数，为0表示使用默认设备。
		IMediaObject * m_SystemAecNsAgcMediaObjPt; //存放系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的指针。
		DMO_OUTPUT_DATA_BUFFER * m_SystemAecNsAgcOtptDataBufPt; //存放系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象的指针。
		IMMDeviceEnumerator * m_EnumPt; //存放枚举器的指针。
		IMMDeviceCollection * m_ClctPt; //存放收集器的指针。
		IMMDevice * m_Pt; //存放指针。
		IAudioClient * m_ClntPt; //存放客户端的指针。
		WAVEFORMATEX * m_WaveFmtExPt; //存放格式的指针。
		UINT32 m_BufSzUnit; //存放缓冲区大小，单位为采样单元。
		IAudioCaptureClient * m_CptrClntPt; //存放捕获客户端的指针。
		IGlobalInterfaceTable * m_GlblIntfcTablePt; //存放全局接口表的指针。
		DWORD m_SystemAecNsAgcMediaObjCookie; //存放系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的Cookie。
		DWORD m_CptrClntCookie; //存放捕获客户端的Cookie。
		BufQueueCls m_PcmBufFrmBufQueue; //存放Pcm格式缓冲区帧的缓冲区队列。
		int16_t * m_PcmBufFrmPt; //存放Pcm格式缓冲区帧的指针。
		size_t m_PcmBufFrmLenUnit; //存放Pcm格式缓冲区帧的长度，单位为采样单元。
		size_t m_PcmBufFrmLenByt; //存放Pcm格式缓冲区帧的长度，单位为字节。
		SpeexResamplerState * m_PcmSrcFrmSpeexResamplerPt; //存放Pcm格式原始帧Speex重采样器的指针。
		int32_t m_IsMute; //存放是否静音，为0表示有声音，为非0表示静音。
		int32_t m_IsClos; //存放是否关闭，为0表示正常，为非0表示关闭。
	} m_Dvc;

	CQueueCls m_PcmSrcFrmCntnr; //存放Pcm格式原始帧容器。
	CQueueCls m_PcmIdleFrmCntnr; //存放Pcm格式空闲帧容器。

	struct //存放线程。
	{
		int32_t m_IsInitThrdTmpVar; //存放是否初始化线程的临时变量。
		int16_t * m_PcmSrcFrmPt; //存放Pcm格式原始帧的指针。
		size_t m_ElmTotal; //存放元素总数。
		uint64_t m_LastTickMsec; //存放上次的嘀嗒钟，单位为毫秒。
		uint64_t m_NowTickMsec; //存放本次的嘀嗒钟，单位为毫秒。

		ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
		int32_t m_IsStartAdoOtptThrd; //存放是否开始音频输出线程，为0表示未开始，为1表示已开始。
		int32_t m_ExitFlag; //存放退出标记，为0表示保持运行，为1表示请求退出。
	} m_Thrd;
} AdoInpt;

int AdoInptAecInit( AdoInpt * AdoInptPt );
void AdoInptAecDstoy( AdoInpt * AdoInptPt );
void AdoInptSetIsCanUseAec( AdoInpt * AdoInptPt );
int AdoInptNsInit( AdoInpt * AdoInptPt );
void AdoInptNsDstoy( AdoInpt * AdoInptPt );
int AdoInptSpeexPrpocsInit( AdoInpt * AdoInptPt );
void AdoInptSpeexPrpocsDstoy( AdoInpt * AdoInptPt );
int AdoInptEncdInit( AdoInpt * AdoInptPt );
void AdoInptEncdDstoy( AdoInpt * AdoInptPt );
int AdoInptWavfmInit( AdoInpt * AdoInptPt );
void AdoInptWavfmDstoy( AdoInpt * AdoInptPt );
int AdoInptWaveFileWriterInit( AdoInpt * AdoInptPt );
void AdoInptWaveFileWriterDstoy( AdoInpt * AdoInptPt );
int AdoInptDvcAndThrdInit( AdoInpt * AdoInptPt );
void AdoInptDvcAndThrdDstoy( AdoInpt * AdoInptPt );
int AdoInptInit( AdoInpt * AdoInptPt );
void AdoInptDstoy( AdoInpt * AdoInptPt );

#ifdef __cplusplus
}
#endif
