#include "MediaPocsThrd.h"

#ifndef __ADOINPT_H__
#define __ADOINPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;

typedef struct AdoInpt //音频输入。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUseAdoInpt; //存放是否使用音频输入，为0表示不使用，为非0表示要使用。
	int32_t m_IsInitAdoInpt; //存放是否初始化音频输入，为0表示未初始化，为非0表示已初始化。

	int32_t m_SmplRate; //存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
	int32_t m_FrmLenMsec; //存放帧的长度，单位为毫秒，取值只能为10毫秒的倍数。
	int32_t m_FrmLenUnit; //存放帧的长度，单位为采样单元，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	int32_t m_FrmLenData; //存放帧的长度，单位为采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	int32_t m_FrmLenByt; //存放帧的长度，单位为字节，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80*2、20毫秒为160*2、30毫秒为240*2，16000Hz的10毫秒为160*2、20毫秒为320*2、30毫秒为480*2，32000Hz的10毫秒为320*2、20毫秒为640*2、30毫秒为960*2，48000Hz的10毫秒为480*2、20毫秒为960*2、30毫秒为1440*2。

	int32_t m_UseWhatAec; //存放使用什么声学回音消除器，为0表示不使用，为1表示Speex声学回音消除器，为2表示WebRtc定点版声学回音消除器，为2表示WebRtc浮点版声学回音消除器，为4表示SpeexWebRtc三重声学回音消除器。
	int32_t m_IsCanUseAec; //存放是否可以使用声学回音消除器，为0表示不可以，为非0表示可以。

	SpeexAec * m_SpeexAecPt; //存放Speex声学回音消除器的指针。
	int32_t m_SpeexAecFilterLen; //存放Speex声学回音消除器滤波器的长度，单位为毫秒。
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
	int32_t m_WebRtcAecmDelay; //存放WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。

	WebRtcAec * m_WebRtcAecPt; //存放WebRtc浮点版声学回音消除器的指针。
	int32_t m_WebRtcAecEchoMode; //存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
	int32_t m_WebRtcAecDelay; //存放WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
	int32_t m_WebRtcAecIsUseDelayAgstcMode; //存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
	int32_t m_WebRtcAecIsUseExtdFilterMode; //存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
	int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode; //存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
	int32_t m_WebRtcAecIsUseAdaptAdjDelay; //存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
	int32_t m_WebRtcAecIsSaveMemFile; //存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
	Vstr * m_WebRtcAecMemFileFullPathVstrPt; //存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针。

	SpeexWebRtcAec * m_SpeexWebRtcAecPt; //存放SpeexWebRtc三重声学回音消除器的指针。
	int32_t m_SpeexWebRtcAecWorkMode; //存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
	int32_t m_SpeexWebRtcAecSpeexAecFilterLen; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器滤波器的长度，单位为毫秒。
	int32_t m_SpeexWebRtcAecSpeexAecIsUseRec; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
	float m_SpeexWebRtcAecSpeexAecEchoMutp; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
	float m_SpeexWebRtcAecSpeexAecEchoCntu; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
	int32_t m_SpeexWebRtcAecSpeexAecEchoSupes; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
	int32_t m_SpeexWebRtcAecSpeexAecEchoSupesAct; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
	int32_t m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecWebRtcAecmEchoMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
	int32_t m_SpeexWebRtcAecWebRtcAecmDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
	int32_t m_SpeexWebRtcAecWebRtcAecEchoMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
	int32_t m_SpeexWebRtcAecWebRtcAecDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
	int32_t m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecIsUseSameRoomAec; //存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
	int32_t m_SpeexWebRtcAecSameRoomEchoMinDelay; //存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位为毫秒，取值区间为[1,2147483647]。

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

    int32_t m_IsDrawAdoWavfmToWnd; //存放是否绘制音频波形到窗口，为非0表示要绘制，为0表示不绘制。
	HWND m_AdoInptSrcWavfmWndHdl; //存放音频输入原始波形窗口的句柄。
    AdoWavfm * m_AdoInptSrcWavfmPt; //存放音频输入原始波形器的指针。
    HWND m_AdoInptRsltWavfmWndHdl; //存放音频输入结果波形窗口的句柄。
    AdoWavfm * m_AdoInptRsltWavfmPt; //存放音频输入结果波形器的指针。
	
	int32_t m_IsSaveAdoToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
	WaveFileWriter * m_AdoInptSrcWaveFileWriterPt; //存放音频输入原始Wave文件写入器的指针。
	WaveFileWriter * m_AdoInptRsltWaveFileWriterPt; //存放音频输入结果Wave文件写入器的指针。
	Vstr * m_AdoInptSrcFileFullPathVstrPt; //存放音频输入原始文件完整路径动态字符串的指针。
	Vstr * m_AdoInptRsltFileFullPathVstrPt; //存放音频输入结果文件完整路径动态字符串的指针。
	size_t m_AdoInptFileWrBufSzByt; //存放音频输入文件写入缓冲区的大小，单位为字节。

	UINT m_AdoInptDvcID; //存放音频输入设备的标识符。
	IMMDeviceEnumerator * m_AdoInptDvcEnumPt; //存放音频输入设备枚举器的指针。
	IMMDeviceCollection * m_AdoInptDvcClctPt; //存放音频输入设备收集器的指针。
	IMMDevice * m_AdoInptDvcPt; //存放音频输入设备的指针。
	IAudioClient * m_AdoInptDvcClntPt; //存放音频输入设备客户端的指针。
	WAVEFORMATEX * m_AdoInptDvcWaveFmtExPt; //存放音频输入设备格式的指针。
	UINT32 m_AdoInptDvcBufSzUnit; //存放音频输入设备的缓冲区大小，单位为采样单元。
	IAudioCaptureClient * m_AdoInptDvcCptrClntPt; //存放音频输入设备捕获客户端的指针。
	IGlobalInterfaceTable * m_AdoInptDvcGlblIntfcTablePt; //存放音频输入设备全局接口表的指针。
	DWORD m_AdoInptDvcCptrClntCookie; //存放音频输入设备捕获客户端的Cookie。
	BufQueueCls m_PcmAdoInptDvcFrmBufQueue; //存放PCM格式音频输入设备帧的缓冲区队列。
	int16_t * m_PcmAdoInptDvcFrmPt; //存放PCM格式音频输入设备帧的指针。
	size_t m_PcmAdoInptDvcFrmLenUnit; //存放PCM格式音频输入设备帧的长度，单位为采样单元。
	size_t m_PcmAdoInptDvcFrmLenByt; //存放PCM格式音频输入设备帧的长度，单位为字节。
	SpeexResamplerState * m_PcmAdoInptSrcFrmSpeexResamplerPt; //存放PCM格式音频输入原始帧Speex重采样器的指针。
	int32_t m_AdoInptIsMute; //存放音频输入是否静音，为0表示有声音，为非0表示静音。
	int32_t m_AdoInptDvcIsClos; //存放音频输入设备是否关闭，为0表示正常，为非0表示关闭。

	ConstLenLnkLstCls m_PcmAdoInptSrcFrmLnkLst; //存放PCM格式音频输入原始帧链表。
	ConstLenLnkLstCls m_PcmAdoInptIdleFrmLnkLst; //存放PCM格式音频输入空闲帧链表。

	int32_t m_IsInitAdoInptThrdTmpVar; //存放是否初始化音频输入线程的临时变量。
	int16_t * m_PcmAdoInptSrcFrmPt; //存放PCM格式音频输入原始帧的指针。
	size_t m_FrmLnkLstElmTotal; //存放帧链表的元素总数。
	uint64_t m_LastTickMsec; //存放上次的嘀嗒钟。
	uint64_t m_NowTickMsec; //存放本次的嘀嗒钟。

	ThrdInfo * m_AdoInptThrdInfoPt; //存放音频输入线程信息的指针。
	int m_AdoInptThrdExitFlag; //存放音频输入线程退出标记，0表示保持运行，1表示请求退出。
} AdoInpt;

#ifdef __cplusplus
extern "C"
{
#endif

int AdoInptAecInit( AdoInpt * AdoInptPt );
void AdoInptAecDstoy( AdoInpt * AdoInptPt );
void AdoInptSetIsCanUseAec( AdoInpt * AdoInptPt );
int AdoInptNsInit( AdoInpt * AdoInptPt );
void AdoInptNsDstoy( AdoInpt * AdoInptPt );
int AdoInptSpeexPrpocsInit( AdoInpt * AdoInptPt );
void AdoInptSpeexPrpocsDstoy( AdoInpt * AdoInptPt );
int AdoInptEncdInit( AdoInpt * AdoInptPt );
void AdoInptEncdDstoy( AdoInpt * AdoInptPt );
int AdoInptWaveFileWriterInit( AdoInpt * AdoInptPt );
void AdoInptWaveFileWriterDstoy( AdoInpt * AdoInptPt );
int AdoInptWavfmInit( AdoInpt * AdoInptPt );
void AdoInptWavfmDstoy( AdoInpt * AdoInptPt );
int AdoInptDvcAndThrdInit( AdoInpt * AdoInptPt );
void AdoInptDvcAndThrdDstoy( AdoInpt * AdoInptPt );
int AdoInptInit( AdoInpt * AdoInptPt );
void AdoInptDstoy( AdoInpt * AdoInptPt );
DWORD WINAPI AdoInptThrdRun( AdoInpt * AdoInptPt );

#ifdef __cplusplus
}
#endif

#endif
