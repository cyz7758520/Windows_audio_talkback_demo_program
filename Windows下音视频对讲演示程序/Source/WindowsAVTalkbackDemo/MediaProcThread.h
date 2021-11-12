#ifndef __MEDIAPROCTHREAD_H__
#define __MEDIAPROCTHREAD_H__

#include "Func.h"
#include "LnkLst.h"
#include "VarStr.h"
#include "SpeexDsp.h"
#include "WebRtc.h"
#include "SpeexWebRtcAec.h"
#include "RNNoise.h"
#include "Speex.h"
#include "WaveFile.h"
#include "LibYUV.h"
#include "OpenH264.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct IGraphBuilder IGraphBuilder;
typedef struct IMediaControl IMediaControl;
typedef struct IMediaEventEx IMediaEventEx;
typedef struct IBaseFilter IBaseFilter;
typedef class VideoInputThread VideoInputThread;
typedef struct MediaProcThread MediaProcThread;

typedef int( __cdecl *USER_INIT_FUNC_PT )( MediaProcThread * MediaProcThreadPt );

typedef int( __cdecl *USER_PROCESS_FUNC_PT )( MediaProcThread * MediaProcThreadPt );

typedef void( __cdecl *USER_DESTROY_FUNC_PT )( MediaProcThread * MediaProcThreadPt );

typedef void( __cdecl *USER_READ_AUDIO_VIDEO_INPUT_FRAME_FUNC_PT )( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioInputFramePt, int16_t * PcmAudioResultFramePt, int32_t VoiceActSts, uint8_t * EncoderAudioInputFramePt, size_t EncoderAudioInputFrameLen, int32_t EncoderAudioInputFrameIsNeedTrans,
																	uint8_t * YU12VideoInputFramePt, int32_t YU12VideoInputFrameWidth, int32_t YU12VideoInputFrameHeight, uint8_t * EncoderVideoInputFramePt, size_t EncoderVideoInputFrameLen );

typedef void( __cdecl *USER_WRITE_AUDIO_OUTPUT_FRAME_FUNC_PT )( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioOutputFramePt, uint8_t * EncoderAudioOutputFramePt, size_t * AudioOutputFrameLenPt );

typedef void( __cdecl *USER_GET_PCM_AUDIO_OUTPUT_FRAME_FUNC_PT )( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioOutputFramePt, size_t PcmAudioOutputFrameLen );

typedef void( __cdecl *USER_WRITE_VIDEO_OUTPUT_FRAME_FUNC_PT )( MediaProcThread * MediaProcThreadPt, uint8_t * YU12VideoOutputFramePt, int32_t * YU12VideoOutputFrameWidthPt, int32_t * YU12VideoOutputFrameHeightPt, uint8_t * EncoderVideoOutputFramePt, size_t * VideoOutputFrameLenPt );

typedef void( __cdecl *USER_GET_YU12_VIDEO_OUTPUT_FRAME_FUNC_PT )( MediaProcThread * MediaProcThreadPt, uint8_t * YU12VideoOutputFramePt, int32_t YU12VideoOutputFrameWidth, int32_t YU12VideoOutputFrameHeight );

//媒体处理线程。
typedef struct MediaProcThread
{
	int m_RunFlag; //存放本线程运行标记。
	#define RUN_FLAG_NORUN 0x00 //运行标记：未开始运行。
    #define RUN_FLAG_INIT 0x01 //运行标记：刚开始运行正在初始化。
    #define RUN_FLAG_PROC 0x02 //运行标记：初始化完毕正在循环处理帧。
    #define RUN_FLAG_DESTROY 0x03 //运行标记：跳出循环处理帧正在销毁。
    #define RUN_FLAG_END 0x04 //运行标记：销毁完毕。
    int m_ExitFlag; //存放本线程退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDestroy销毁函数。
    int m_ExitCode; //存放本线程退出代码，为0表示正常退出，为-1表示初始化失败，为-2表示处理失败。

    int32_t m_IsSaveSettingToFile; //存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
    VarStr * m_SettingFileFullPathVarStrPt; //存放设置文件的完整路径动态字符串的内存指针。

	int32_t m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。
	int32_t m_IsShowToast; //存放是否显示Toast，为非0表示要显示，为0表示不显示。

	typedef struct AudioInput //音频输入结构体。
	{
		int32_t m_IsUseAudioInput; //存放是否使用音频输入，为0表示不使用，为非0表示要使用。
		
		int32_t m_SamplingRate; //存放采样频率，单位赫兹，取值只能为8000、16000、32000。
		int32_t m_FrameLen; //存放帧的数据长度，单位采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960。

		int32_t m_UseWhatAec; //存放使用什么声学回音消除器，为0表示不使用，为1表示Speex声学回音消除器，为2表示WebRtc定点版声学回音消除器，为2表示WebRtc浮点版声学回音消除器，为4表示SpeexWebRtc三重声学回音消除器。

		SpeexAec * m_SpeexAecPt; //存放Speex声学回音消除器的内存指针。
		int32_t m_SpeexAecFilterLen; //存放Speex声学回音消除器的滤波器数据长度，单位毫秒。
		int32_t m_SpeexAecIsUseRec; //存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
		float m_SpeexAecEchoMutp; //存放Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
		float m_SpeexAecEchoCntu; //存放Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
		int32_t m_SpeexAecEchoSupes; //存放Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexAecEchoSupesAct; //存放Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexAecIsSaveMemFile; //存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		VarStr * m_SpeexAecMemFileFullPathVarStrPt; //存放Speex声学回音消除器的内存块文件完整路径动态字符串的内存指针。

		WebRtcAecm * m_WebRtcAecmPt; //存放WebRtc定点版声学回音消除器的内存指针。
		int32_t m_WebRtcAecmIsUseCNGMode; //存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecmEchoMode; //存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
		int32_t m_WebRtcAecmDelay; //存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。

		WebRtcAec * m_WebRtcAecPt; //存放WebRtc浮点版声学回音消除器的内存指针。
		int32_t m_WebRtcAecEchoMode; //存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
		int32_t m_WebRtcAecDelay; //存放WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
		int32_t m_WebRtcAecIsUseDelayAgnosticMode; //存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseExtdFilterMode; //存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode; //存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsUseAdaptAdjDelay; //存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
		int32_t m_WebRtcAecIsSaveMemFile; //存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
		VarStr * m_WebRtcAecMemFileFullPathVarStrPt; //存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的内存指针。

		SpeexWebRtcAec * m_SpeexWebRtcAecPt; //存放SpeexWebRtc三重声学回音消除器的内存指针。
		int32_t m_SpeexWebRtcAecWorkMode; //存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
		int32_t m_SpeexWebRtcAecSpeexAecFilterLen; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器的滤波器数据长度，单位毫秒。
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
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay; //存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecIsUseSameRoomAec; //存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexWebRtcAecSameRoomEchoMinDelay; //存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位毫秒，取值区间为[1,2147483647]。

		int32_t m_UseWhatNs; //存放使用什么噪音抑制器，为0表示不使用，为1表示Speex预处理器的噪音抑制，为2表示WebRtc定点版噪音抑制器，为3表示WebRtc浮点版噪音抑制器，为4表示RNNoise噪音抑制器。

		SpeexPreprocessState * m_SpeexPprocPt; //存放Speex预处理器的内存指针。
		int32_t m_SpeexPprocIsUseNs; //存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPprocNoiseSupes; //存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexPprocIsUseDereverb; //存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。

		WebRtcNsx * m_WebRtcNsxPt; //存放WebRtc定点版噪音抑制器的内存指针。
		int32_t m_WebRtcNsxPolicyMode; //存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

		WebRtcNs * m_WebRtcNsPt; //存放WebRtc浮点版噪音抑制器的内存指针。
		int32_t m_WebRtcNsPolicyMode; //存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

		RNNoise * m_RNNoisePt; //存放RNNoise噪音抑制器的内存指针。

		int32_t m_IsUseSpeexPprocOther; //存放Speex预处理器是否使用其他功能，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPprocIsUseVad; //存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPprocVadProbStart; //存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
		int32_t m_SpeexPprocVadProbCntu; //存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
		int32_t m_SpeexPprocIsUseAgc; //存放Speex预处理器是否使用自动增益控制，为非0表示要使用，为0表示不使用。
		int32_t m_SpeexPprocAgcLevel; //存放Speex预处理器在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
		int32_t m_SpeexPprocAgcIncrement; //存放Speex预处理器在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
		int32_t m_SpeexPprocAgcDecrement; //存放Speex预处理器在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
		int32_t m_SpeexPprocAgcMaxGain; //存放Speex预处理器在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。

		int32_t m_UseWhatEncoder; //存放使用什么编码器，为0表示PCM原始数据，为1表示Speex编码器，为2表示Opus编码器。

		SpeexEncoder * m_SpeexEncoderPt; //存放Speex编码器的内存指针。
		int32_t m_SpeexEncoderUseCbrOrVbr; //存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
		int32_t m_SpeexEncoderQuality; //存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
		int32_t m_SpeexEncoderComplexity; //存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
		int32_t m_SpeexEncoderPlcExpectedLossRate; //存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
		
		int32_t m_IsSaveAudioToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
		WaveFileWriter * m_AudioInputWaveFileWriterPt; //存放音频输入Wave文件写入器的内存指针。
		WaveFileWriter * m_AudioResultWaveFileWriterPt; //存放音频结果Wave文件写入器的内存指针。
		VarStr * m_AudioInputFileFullPathVarStrPt; //存放音频输入文件完整路径动态字符串的内存指针。
		VarStr * m_AudioResultFileFullPathVarStrPt; //存放音频结果文件完整路径动态字符串的内存指针。

		UINT m_AudioInputDeviceID; //存放音频输入设备的标识符。
		HWAVEIN m_AudioInputDeviceHdl; //存放音频输入设备的句柄。
		WAVEHDR * m_AudioInputWaveHdrPt; //存放音频输入缓冲区块数组的内存指针。
		int32_t m_AudioInputWaveHdrTotal; //存放音频输入缓冲区块数组的元素总数。
		int32_t m_AudioInputIsMute; //存放音频输入是否静音，为0表示有声音，为非0表示静音。

		ConstLenLnkLstCls m_AudioInputFrameLnkLst; //存放音频输入帧链表。
		ConstLenLnkLstCls m_AudioInputIdleFrameLnkLst; //存放音频输入空闲帧链表。
		size_t m_AudioInputFrameElmTotal; //存放音频输入帧元素的总数。

		//音频输入线程的临时变量。
		MSG m_Msg; //存放消息。
		int16_t * m_AudioInputFramePt; //存放音频输入帧的内存指针。
		size_t m_AudioInputFrameLnkLstElmTotal; //存放音频输入帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_AudioInputThreadHdl; //存放音频输入线程的线程句柄。
		DWORD m_AudioInputThreadId; //存放音频输入线程的线程标识符。
	} AudioInput;

	AudioInput m_AudioInput; //存放音频输入。

	typedef struct AudioOutput //音频输出结构体。
	{
		int32_t m_IsUseAudioOutput; //存放是否使用音频输出，为0表示不使用，为非0表示要使用。

		int32_t m_SamplingRate; //存放采样频率，单位赫兹，取值只能为8000、16000、32000。
		int32_t m_FrameLen; //存放帧的数据长度，单位采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960。

		int32_t m_UseWhatDecoder; //存放使用什么解码器，为0表示PCM原始数据，为1表示Speex解码器，为2表示Opus解码器。

		SpeexDecoder * m_SpeexDecoderPt; //存放Speex解码器的内存指针。
		int32_t m_SpeexDecoderIsUsePerceptualEnhancement; //存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
		
		int32_t m_IsSaveAudioToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
		WaveFileWriter * m_AudioOutputWaveFileWriterPt; //存放音频输出Wave文件写入器的内存指针。
		VarStr * m_AudioOutputFileFullPathVarStrPt; //存放音频输出文件完整路径动态字符串的内存指针。
		
		UINT m_AudioOutputDeviceID; //存放音频输出设备的标识符。
		HWAVEOUT m_AudioOutputDeviceHdl; //存放音频输出设备的句柄。
		WAVEHDR * m_AudioOutputWaveHdrPt; //存放音频缓冲区块数组的内存指针。
		int32_t m_AudioOutputWaveHdrTotal; //存放音频输出缓冲区块数组的元素总数。
		int32_t m_AudioOutputIsMute; //存放音频输出是否静音，为0表示有声音，为非0表示静音。

		ConstLenLnkLstCls m_AudioOutputFrameLnkLst; //存放音频输出帧链表。
		ConstLenLnkLstCls m_AudioOutputIdleFrameLnkLst; //存放音频输出空闲帧链表。
		size_t m_AudioOutputFrameElmTotal; //存放音频输出帧元素的总数。

		//音频输出线程的临时变量。
		MSG m_Msg; //存放消息。
		int16_t * m_AudioOutputFramePt; //存放音频输出帧的内存指针。
		uint8_t * m_EncoderAudioOutputFramePt; //存放已编码格式音频输出帧的内存指针。
		size_t m_EncoderAudioOutputFrameSz; //存放已编码格式音频输出帧的内存大小，单位字节。
		size_t m_AudioOutputFrameLen; //存放音频输出帧的数据长度，单位字节。
		size_t m_AudioOutputFrameLnkLstElmTotal; //存放音频输出帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_AudioOutputThreadHdl; //存放音频输出线程的线程句柄。
		DWORD m_AudioOutputThreadId; //存放音频输出线程的线程标识符。
	} AudioOutput;

	AudioOutput m_AudioOutput; //存放音频输出。

	typedef struct VideoInput
	{
		int32_t m_IsUseVideoInput; //存放是否使用视频输入，为0表示不使用，为非0表示要使用。
		
        int32_t m_MaxSamplingRate; //存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
        int32_t m_FrameWidth; //存放帧的宽度，单位为像素。
        int32_t m_FrameHeight; //存放帧的高度，单位为像素。
		
        int32_t m_UseWhatEncoder; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264编码器。

        OpenH264Encoder * m_OpenH264EncoderPt; //存放OpenH264编码器的内存指针。
        int32_t m_OpenH264EncoderVideoType;//存放OpenH264编码器的视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
        int32_t m_OpenH264EncoderEncodedBitrate; //存放OpenH264编码器的编码后比特率，单位为bps。
        int32_t m_OpenH264EncoderBitrateControlMode; //存放OpenH264编码器的比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
        int32_t m_OpenH264EncoderIDRFrameIntvl; //存放OpenH264编码器的IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
        int32_t m_OpenH264EncoderComplexity; //存放OpenH264编码器的复杂度，复杂度越高压缩率不变、CPU使用率越高、画质越好，取值区间为[0,2]。

		UINT m_VideoInputDeviceID; //存放视频输入设备的标识符。
		IGraphBuilder * m_FilterGraphManagerPt; //存放视频输入过滤器图管理器的内存指针。
		IMediaEventEx * m_FilterGraphManagerMediaEventPt; //存放视频输入过滤器图管理器媒体事件器的内存指针。
		IMediaControl * m_FilterGraphManagerMediaControlPt; //存放视频输入过滤器图管理器媒体控制器的内存指针。
		int32_t m_VideoInputDeviceMaxSamplingRate; //存放视频输入设备的最大采样频率。
		int32_t m_VideoInputDeviceFrameWidth; //存放视频输入设备帧的宽度，单位为像素。
        int32_t m_VideoInputDeviceFrameHeight; //存放视频输入设备帧的高度，单位为像素。
		int32_t m_VideoInputDeviceFrameIsCrop; //存放视频输入设备帧是否裁剪，为0表示不裁剪，为非0表示要裁剪。
		int32_t m_VideoInputDeviceFrameCropX; //存放视频输入设备帧裁剪区域左上角的横坐标，单位像素。
        int32_t m_VideoInputDeviceFrameCropY; //存放视频输入设备帧裁剪区域左上角的纵坐标，单位像素。
		int32_t m_VideoInputDeviceFrameCropWidth; //存放视频输入设备帧裁剪区域的宽度，单位像素。
        int32_t m_VideoInputDeviceFrameCropHeight; //存放视频输入设备帧裁剪区域的高度，单位像素。
		int32_t m_VideoInputDeviceFrameIsScale; //存放视频输入设备帧是否缩放，为0表示不缩放，为非0表示要缩放。
		int32_t m_VideoInputIsBlack; //存放视频输入是否黑屏，为0表示有图像，为非0表示黑屏。
		HWND m_VideoInputPreviewWndHdl; //存放视频输入预览窗口的句柄。
		
		typedef struct VideoInputFrameElm //视频输入帧链表元素。
		{
			uint8_t * m_YU12VideoInputFramePt; //存放YU12格式视频输入帧的内存指针，内存大小为m_FrameWidth * m_FrameHeight * 3 / 2。
			uint8_t * m_EncoderVideoInputFramePt; //存放已编码格式视频输入帧的内存指针，内存大小为m_FrameWidth * m_FrameHeight * 3 / 2。
			size_t m_EncoderVideoInputFrameLen; //存放已编码格式视频输入帧的数据长度，单位字节。
		}VideoInputFrameElm;
		ConstLenLnkLstCls m_VideoInputFrameLnkLst; //存放视频输入帧链表。
		ConstLenLnkLstCls m_VideoInputIdleFrameLnkLst; //存放视频输入空闲帧链表。
		size_t m_VideoInputFrameElmTotal; //存放视频输入帧元素的总数。

		//视频输入线程的临时变量。
		uint64_t m_LastVideoInputFrameTimeMsec; //存放上一个视频输入帧的时间，单位毫秒。
		uint64_t m_VideoInputFrameTimeStepMsec; //存放视频输入帧的时间步进，单位毫秒。
		uint8_t * m_VideoInputResultFramePt; //存放视频输入结果帧的内存指针。
		uint8_t * m_VideoInputTmpFramePt; //存放视频输入临时帧的内存指针。
		uint8_t * m_VideoInputSwapFramePt; //存放视频输入交换帧的内存指针。
		size_t m_VideoInputResultFrameSz; //存放视频输入结果帧的内存大小，单位字节。
		size_t m_VideoInputResultFrameLen; //存放视频输入结果帧的数据长度，单位字节。
		VideoInputFrameElm * m_VideoInputFrameElmPt; //存放视频输入帧元素的内存指针。
		size_t m_VideoInputFrameLnkLstElmTotal; //存放视频输入帧链表的元数总数。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		VideoInputThread * m_VideoInputThreadPt; //存放视频输入线程的内存指针。
	}VideoInput;
	VideoInput m_VideoInput; //存放视频输入。
	
    typedef struct VideoOutput //视频输出类。
    {
        int32_t m_IsUseVideoOutput; //存放是否使用视频输出，为0表示不使用，为非0表示要使用。

        int32_t m_FrameWidth; //存放帧的宽度，单位为像素。
        int32_t m_FrameHeight; //存放帧的高度，单位为像素。

        int32_t m_UseWhatDecoder; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264解码器。

        OpenH264Decoder * m_OpenH264DecoderPt; //存放OpenH264解码器类对象的内存指针。
        int32_t m_OpenH264DecoderDecodeThreadNum; //存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。

        HWND m_VideoOutputDisplayWndHdl; //存放视频输出显示窗口的句柄。
        float m_VideoOutputDisplayScale; //存放视频输出显示缩放倍数。
        int32_t m_VideoOutputIsBlack; //存放视频输出是否黑屏，为0表示有图像，为非0表示黑屏。
		
		//视频输出线程的临时变量。
		uint8_t * m_VideoOutputResultFramePt; //存放视频输出结果帧的内存指针。
		uint8_t * m_VideoOutputTmpFramePt; //存放视频输出临时帧的内存指针。
		uint8_t * m_VideoOutputSwapFramePt; //存放视频输出交换帧的内存指针。
		size_t m_VideoOutputResultFrameSz; //存放视频输出结果帧的内存大小，单位字节。
		size_t m_VideoOutputResultFrameLen; //存放视频输出结果帧的数据长度，单位字节。
		int32_t m_VideoOutputFrameWidth; //存放视频输出帧的宽度，单位为像素。
        int32_t m_VideoOutputFrameHeight; //存放视频输出帧的高度，单位为像素。
		uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
		uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

		HANDLE m_VideoOutputThreadHdl; //存放视频输出线程的线程句柄。
		DWORD m_VideoOutputThreadId; //存放视频输出线程的线程标识符。
		int m_VideoOutputThreadExitFlag; //视频输出线程退出标记，0表示保持运行，1表示请求退出。
	}VideoOutput;

    VideoOutput m_VideoOutput; //存放视频输出。

	//媒体处理线程的临时变量。
	int16_t * m_PcmAudioInputFramePt; //存放PCM格式音频输入帧的内存指针。
	int16_t * m_PcmAudioOutputFramePt; //存放PCM格式音频输出帧的内存指针。
	int16_t * m_PcmAudioResultFramePt; //存放PCM格式音频结果帧的内存指针。
	int16_t * m_PcmAudioTmpFramePt; //存放PCM格式音频临时帧的内存指针。
	int16_t * m_PcmAudioSwapFramePt; //存放PCM格式音频交换帧的内存指针。
	int32_t m_VoiceActSts; //存放语音活动状态，为1表示有语音活动，为0表示无语音活动。
	uint8_t * m_EncoderAudioInputFramePt; //存放已编码格式音频输入帧的内存指针。
	size_t m_EncoderAudioInputFrameSz; //存放已编码格式音频输入帧的内存大小，单位字节。
    size_t m_EncoderAudioInputFrameLen; //存放已编码格式音频输入帧的数据长度，单位字节。
    int32_t m_EncoderAudioInputFrameIsNeedTrans; //存放已编码格式音频输入帧是否需要传输，为1表示需要传输，为0表示不需要传输。
	VideoInput::VideoInputFrameElm * m_VideoInputFramePt; //存放视频输入帧的内存指针。

	HANDLE m_MediaProcThreadHdl; //存放媒体处理线程的句柄。

	VarStr * m_ErrInfoVarStrPt; //存放错误信息动态字符串的内存指针。

	//用户定义的相关回调函数。

	//存放用户数据的内存指针。
	void * m_UserDataPt;
	
	//用户定义的初始化函数，在本线程刚启动时回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_INIT_FUNC_PT m_UserInitFuncPt;

	//用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_PROCESS_FUNC_PT m_UserProcessFuncPt;

	//用户定义的销毁函数，在本线程退出时回调一次。
	USER_DESTROY_FUNC_PT m_UserDestroyFuncPt;

	//用户定义的读取音视频输入帧函数，在读取到一个音频输入帧或视频输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
	USER_READ_AUDIO_VIDEO_INPUT_FRAME_FUNC_PT m_UserReadAudioVideoInputFrameFuncPt;

	//用户定义的写入音频输出帧函数，在需要写入一个音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
	USER_WRITE_AUDIO_OUTPUT_FRAME_FUNC_PT m_UserWriteAudioOutputFrameFuncPt;

	//用户定义的获取PCM格式音频输出帧函数，在解码完一个已编码音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
	USER_GET_PCM_AUDIO_OUTPUT_FRAME_FUNC_PT m_UserGetPcmAudioOutputFrameFuncPt;

	//用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
	USER_WRITE_VIDEO_OUTPUT_FRAME_FUNC_PT m_UserWriteVideoOutputFrameFuncPt;

	//用户定义的获取YU12格式视频输出帧函数，在解码完一个已编码视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
	USER_GET_YU12_VIDEO_OUTPUT_FRAME_FUNC_PT m_UserGetYU12VideoOutputFrameFuncPt;
}MediaProcThread;

int MediaProcThreadInit( MediaProcThread * * MediaProcThreadPtPt, void * UserDataPt,
						 USER_INIT_FUNC_PT UserInitFuncPt, USER_PROCESS_FUNC_PT UserProcessFuncPt, USER_DESTROY_FUNC_PT UserDestroyFuncPt,
						 USER_READ_AUDIO_VIDEO_INPUT_FRAME_FUNC_PT UserReadAudioVideoInputFrameFuncPt,
						 USER_WRITE_AUDIO_OUTPUT_FRAME_FUNC_PT UserWriteAudioOutputFrameFuncPt, USER_GET_PCM_AUDIO_OUTPUT_FRAME_FUNC_PT UserGetPcmAudioOutputFrameFuncPt,
						 USER_WRITE_VIDEO_OUTPUT_FRAME_FUNC_PT UserWriteVideoOutputFrameFuncPt, USER_GET_YU12_VIDEO_OUTPUT_FRAME_FUNC_PT UserGetYU12VideoOutputFrameFuncPt,
						 VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetIsSaveSettingToFile( MediaProcThread * MediaProcThreadPt, int IsSaveSettingToFile, const char * SettingFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetIsPrintLogShowToast( MediaProcThread * MediaProcThreadPt, int IsPrintLog, int IsShowToast, VarStr * ErrInfoVarStrPt );

int MediaProcThreadGetAudioInputDeviceName( VarStr * * * AudioInputDeviceNameArrPtPtPt, UINT * AudioInputDeviceTotalPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadGetAudioOutputDeviceName( VarStr * * * AudioOutputDeviceNameArrPtPtPt, UINT * AudioOutputDeviceTotalPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadGetVideoInputDeviceName( VarStr * * * VideoInputDeviceNameArrPtPtPt, UINT * VideoInputDeviceTotalPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadDestroyDeviceName( VarStr * * DeviceNameArrPtPt, UINT DeviceTotal, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetIsUseAudioInput( MediaProcThread * MediaProcThreadPt, int32_t IsUseAudioInput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputUseNoAec( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseSpeexAec( MediaProcThread * MediaProcThreadPt, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseWebRtcAecm( MediaProcThread * MediaProcThreadPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseWebRtcAec( MediaProcThread * MediaProcThreadPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseSpeexWebRtcAec( MediaProcThread * MediaProcThreadPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgnosticMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputUseNoNs( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseSpeexPprocNs( MediaProcThread * MediaProcThreadPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseWebRtcNsx( MediaProcThread * MediaProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseWebRtcNs( MediaProcThread * MediaProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseRNNoise( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputIsUseSpeexPprocOther( MediaProcThread * MediaProcThreadPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputUsePcm( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseSpeexEncoder( MediaProcThread * MediaProcThreadPt, int32_t UseCbrOrVbr, int32_t Quality, int32_t Complexity, int32_t PlcExpectedLossRate, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputUseOpusEncoder( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputIsSaveAudioToFile( MediaProcThread * MediaProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioInputFileFullPathStrPt, const char * AudioResultFileFullPathStrPt, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioInputUseDevice( MediaProcThread * MediaProcThreadPt, UINT AudioInputDeviceID, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioInputIsMute( MediaProcThread * MediaProcThreadPt, int32_t IsMute, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetIsUseAudioOutput( MediaProcThread * MediaProcThreadPt, int32_t IsUseAudioOutput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioOutputUsePcm( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioOutputUseSpeexDecoder( MediaProcThread * MediaProcThreadPt, int32_t IsUsePerceptualEnhancement, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioOutputUseOpusDecoder( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioOutputIsSaveAudioToFile( MediaProcThread * MediaProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioOutputFileFullPathStrPt, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetAudioOutputUseDevice( MediaProcThread * MediaProcThreadPt, UINT AudioOutputDeviceID, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetAudioOutputIsMute( MediaProcThread * MediaProcThreadPt, int32_t IsMute, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetIsUseVideoInput( MediaProcThread * MediaProcThreadPt, int32_t IsUseVideoInput, int32_t MaxSamplingRate, int32_t FrameWidth, int32_t FrameHeight, HWND VideoInputPreviewWnd, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetVideoInputUseYU12( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetVideoInputUseOpenH264Encoder( MediaProcThread * MediaProcThreadPt, int32_t VideoType, int32_t EncodedBitrate, int32_t BitrateControlMode, int32_t IDRFrameIntvl, int32_t Complexity, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetVideoInputUseDevice( MediaProcThread * MediaProcThreadPt, UINT VideoInputDeviceID, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetVideoInputIsBlack( MediaProcThread * MediaProcThreadPt, int IsBlack, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetIsUseVideoOutput( MediaProcThread * MediaProcThreadPt, int32_t IsUseVideoOutput, int32_t FrameWidth, int32_t FrameHeight, HWND VideoOutputDisplayWnd, float VideoOutputDisplayScale, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetVideoOutputUseYU12( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadSetVideoOutputUseOpenH264Decoder( MediaProcThread * MediaProcThreadPt, int32_t DecodeThreadNum, VarStr * ErrInfoVarStrPt );

int MediaProcThreadSetVideoOutputIsBlack( MediaProcThread * MediaProcThreadPt, int IsBlack, VarStr * ErrInfoVarStrPt );

int MediaProcThreadStart( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );
int MediaProcThreadRequireExit( MediaProcThread * MediaProcThreadPt, int ExitFlag, int IsBlockWait, VarStr * ErrInfoVarStrPt );

int MediaProcThreadDestroy( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
