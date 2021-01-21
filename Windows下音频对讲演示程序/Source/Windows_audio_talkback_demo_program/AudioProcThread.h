#ifndef __AUDIOPROCTHREAD_H__
#define __AUDIOPROCTHREAD_H__

#include "LnkLst.h"
#include "VarStr.h"
#include "ThreadLock.h"
#include "SpeexDsp.h"
#include "WebRtc.h"
#include "SpeexWebRtcAec.h"
#include "RNNoise.h"
#include "Speex.h"
#include "WaveFile.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AudioProcThread AudioProcThread;
typedef int( __cdecl *USER_INIT_FUNC_PT )( AudioProcThread * AudioProcThreadPt );
typedef int( __cdecl *USER_PROCESS_FUNC_PT )( AudioProcThread * AudioProcThreadPt );
typedef void( __cdecl *USER_DESTROY_FUNC_PT )( AudioProcThread * AudioProcThreadPt );
typedef int( __cdecl *USER_READ_INPUT_FRAME_FUNC_PT )( AudioProcThread * AudioProcThreadPt, int16_t * PcmInputFramePt, int16_t * PcmResultFramePt, int32_t VoiceActSts, int8_t * SpeexInputFramePt, size_t SpeexInputFrameLen, int32_t SpeexInputFrameIsNeedTrans );
typedef void( __cdecl *USER_WRITE_OUTPUT_FRAME_FUNC_PT )( AudioProcThread * AudioProcThreadPt, int16_t * PcmOutputFramePt, int8_t * SpeexOutputFramePt, size_t * SpeexOutputFrameLenPt );
typedef void( __cdecl *USER_GET_PCM_OUTPUT_FRAME_FUNC_PT )( AudioProcThread * AudioProcThreadPt, int16_t * PcmOutputFramePt );

//音频处理线程。
typedef struct AudioProcThread
{
	int m_ExitFlag; //存放本线程退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDestroy销毁函数。
	int m_ExitCode; //存放本线程退出代码，为0表示正常退出，为-1表示初始化失败，为-2表示处理失败。

	int32_t m_SamplingRate; //存放采样频率，单位赫兹。
	int32_t m_FrameLen; //存放帧的数据长度，单位采样数据。
	
    int m_IsSaveSettingToFile; //存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
    VarStr * m_SettingFileFullPathVarStrPt; //存放设置文件的完整路径动态字符串的内存指针。

	int m_IsPrintLog; //存放是否打印Log日志，为非0表示要打印，为0表示不打印。

	ConstLenLnkLst * m_InputFrameLnkLstPt; //存放输入帧链表的内存指针。
	MutexLock * m_InputFrameLnkLstMutexLockPt; //存放输入帧链表互斥锁的内存指针。
	ConstLenLnkLst * m_OutputFrameLnkLstPt; //存放输出帧链表的内存指针。
	MutexLock * m_OutputFrameLnkLstMutexLockPt; //存放输出帧链表互斥锁的内存指针。

	int m_UseWhatAec; //存放使用什么声学回音消除器，为0表示不使用，为1表示Speex声学回音消除器，为2表示WebRtc定点版声学回音消除器，为2表示WebRtc浮点版声学回音消除器，为4表示SpeexWebRtc三重声学回音消除器。

	SpeexAec * m_SpeexAecPt; //存放Speex声学回音消除器的内存指针。
    int32_t m_SpeexAecFilterLen; //存放Speex声学回音消除器的滤波器数据长度，单位毫秒。
	int32_t m_SpeexAecIsUseRec; //存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
    float m_SpeexAecEchoMultiple; //存放Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
    float m_SpeexAecEchoCont; //存放Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
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
    int m_WebRtcAecIsSaveMemFile; //存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
    VarStr * m_WebRtcAecMemFileFullPathVarStrPt; //存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的内存指针。
	
    SpeexWebRtcAec * m_SpeexWebRtcAecPt; //存放SpeexWebRtc三重声学回音消除器的内存指针。
    int32_t m_SpeexWebRtcAecWorkMode; //存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
    int32_t m_SpeexWebRtcAecSpeexAecFilterLen; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器的滤波器数据长度，单位毫秒。
    int32_t m_SpeexWebRtcAecSpeexAecIsUseRec; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
	float m_SpeexWebRtcAecSpeexAecEchoMultiple; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
    float m_SpeexWebRtcAecSpeexAecEchoCont; //存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
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

	int m_UseWhatNs; //存放使用什么噪音抑制器，为0表示不使用，为1表示Speex预处理器的噪音抑制，为2表示WebRtc定点版噪音抑制器，为3表示WebRtc浮点版噪音抑制器，为4表示RNNoise噪音抑制器。
	
    SpeexPreprocessState * m_SpeexPprocPt; //存放Speex预处理器的内存指针。
    int32_t m_SpeexPprocIsUseNs; //存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
    int32_t m_SpeexPprocNoiseSupes; //存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
    int32_t m_SpeexPprocIsUseDereverb; //存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。

    WebRtcNsx * m_WebRtcNsxPt; //存放WebRtc定点版噪音抑制器的内存指针。
    int32_t m_WebRtcNsxPolicyMode; //存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

    WebRtcNs * m_WebRtcNsPt; //存放WebRtc浮点版噪音抑制器的内存指针。
    int32_t m_WebRtcNsPolicyMode; //存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。

    RNNoise * m_RNNoisePt; //存放RNNoise噪音抑制器的内存指针。

    int m_IsUseSpeexPprocOther; //存放Speex预处理器是否使用其他功能，为非0表示要使用，为0表示不使用。
    int32_t m_SpeexPprocIsUseVad; //存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
    int32_t m_SpeexPprocVadProbStart; //存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
    int32_t m_SpeexPprocVadProbCont; //存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
    int32_t m_SpeexPprocIsUseAgc; //存放Speex预处理器是否使用自动增益控制，为非0表示要使用，为0表示不使用。
    int32_t m_SpeexPprocAgcLevel; //存放Speex预处理器在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
    int32_t m_SpeexPprocAgcIncrement; //存放Speex预处理器在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
    int32_t m_SpeexPprocAgcDecrement; //存放Speex预处理器在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
    int32_t m_SpeexPprocAgcMaxGain; //存放Speex预处理器在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。

	int m_UseWhatCodec; //存放使用什么编解码器，为0表示PCM原始数据，为1表示Speex编解码器，为2表示Opus编解码器。

    SpeexEncoder * m_SpeexEncoderPt; //存放Speex编码器的内存指针。
    SpeexDecoder * m_SpeexDecoderPt; //存放Speex解码器的内存指针。
    int32_t m_SpeexCodecEncoderUseCbrOrVbr; //存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
    int32_t m_SpeexCodecEncoderQuality; //存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
    int32_t m_SpeexCodecEncoderComplexity; //存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
    int32_t m_SpeexCodecEncoderPlcExpectedLossRate; //存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
    int32_t m_SpeexCodecDecoderIsUsePerceptualEnhancement; //存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。

    int m_IsSaveAudioToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
    WaveFileWriter * m_AudioInputWaveFileWriterPt; //存放音频输入Wave文件写入器的内存指针。
    WaveFileWriter * m_AudioOutputWaveFileWriterPt; //存放音频输出Wave文件写入器的内存指针。
    WaveFileWriter * m_AudioResultWaveFileWriterPt; //存放音频结果Wave文件写入器的内存指针。
    VarStr * m_AudioInputFileFullPathVarStrPt; //存放音频输入文件完整路径动态字符串的内存指针。
    VarStr * m_AudioOutputFileFullPathVarStrPt; //存放音频输出文件完整路径动态字符串的内存指针。
    VarStr * m_AudioResultFileFullPathVarStrPt; //存放音频结果文件完整路径动态字符串的内存指针。

	HANDLE m_AudioInputThreadHdl; //存放音频输入线程的线程句柄。
	DWORD m_AudioInputThreadId; //存放音频输入线程的线程标识符。
	HANDLE m_AudioOutputThreadHdl; //存放音频输出线程的线程句柄。
	DWORD m_AudioOutputThreadId; //存放音频输出线程的线程标识符。

	UINT m_InputDeviceID; //存放音频输入设备的标识符。
	HWAVEIN m_AudioInputDeviceHdl; //存放音频输入设备的句柄。
	int m_AudioInputDeviceIsMute; //存放音频输入设备是否静音，为0表示有声音，为非0表示静音。
	UINT m_OutputDeviceID; //存放音频输出设备的标识符。
	HWAVEOUT m_AudioOutputDeviceHdl; //存放音频输出设备的句柄。
	WAVEHDR * m_AudioWaveHdrPt; //存放音频缓冲区块数组的内存指针。
	int m_AudioWaveHdrTotal; //存放音频缓冲区块数组的元素总数。
	int m_AudioOutputDeviceIsMute; //存放音频输出设备是否静音，为0表示有声音，为非0表示静音。

	HANDLE m_AudioProcThreadHdl; //存放音频处理线程的线程句柄。

	VarStr * m_ErrInfoVarStrPt; //存放错误信息动态字符串的内存指针。

	void * m_UserDataPt; //存放用户数据的内存指针。
	USER_INIT_FUNC_PT m_UserInitFuncPt; //存放用户定义的初始化函数的内存指针，在本线程刚启动时调用一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_PROCESS_FUNC_PT m_UserProcessFuncPt; //存放用户定义的处理函数的内存指针，在本线程运行时每隔1毫秒就调用一次，返回值表示是否成功，为0表示成功，为非0表示失败。
	USER_DESTROY_FUNC_PT m_UserDestroyFuncPt; //存放用户定义的销毁函数的内存指针，在本线程退出时调用一次，返回值表示是否重新初始化，为0表示直接退出，为非0表示重新初始化。
	USER_READ_INPUT_FRAME_FUNC_PT m_UserReadInputFrameFuncPt; //存放用户定义的读取输入帧函数的内存指针，在读取到一个输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
	USER_WRITE_OUTPUT_FRAME_FUNC_PT m_UserWriteOutputFrameFuncPt; //存放用户定义的写入输出帧函数的内存指针，在需要写入一个输出帧时回调一次。注意：本函数不是在音频处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致回音消除失败。
	USER_GET_PCM_OUTPUT_FRAME_FUNC_PT m_UserGetPcmOutputFrameFuncPt; //存放用户定义的获取PCM格式输出帧函数的内存指针，在解码完一个输出帧时回调一次。注意：本函数不是在音频处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致回音消除失败。
}AudioProcThread;

int AudioProcThreadInit( AudioProcThread * * AudioProcThreadPtPt, void * UserDataPt, USER_INIT_FUNC_PT UserInitFuncPt, USER_PROCESS_FUNC_PT UserProcessFuncPt, USER_DESTROY_FUNC_PT UserDestroyFuncPt, USER_READ_INPUT_FRAME_FUNC_PT UserReadInputFrameFuncPt, USER_WRITE_OUTPUT_FRAME_FUNC_PT UserWriteOutputFrameFuncPt, USER_GET_PCM_OUTPUT_FRAME_FUNC_PT UserGetPcmOutputFrameFuncPt, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetSaveSettingToFile( AudioProcThread * AudioProcThreadPt, int IsSaveSettingToFile, const char * SettingFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetPrintLog( AudioProcThread * AudioProcThreadPt, int IsPrintLog, VarStr * ErrInfoVarStrPt );
int AudioProcThreadGetDeviceCaps( WAVEINCAPS * * InputCapsArrPtPt, UINT * InputCapsArrTotalPt, WAVEOUTCAPS * * OutputCapsArrPtPt, UINT * OutputCapsArrTotalPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseDevice( AudioProcThread * AudioProcThreadPt, UINT InputDeviceID, UINT OutputDeviceID, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetAudioInputDeviceMute( AudioProcThread * AudioProcThreadPt, int IsMute, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetAudioOutputDeviceMute( AudioProcThread * AudioProcThreadPt, int IsMute, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetUseNoAec( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseSpeexAec( AudioProcThread * AudioProcThreadPt, int32_t FilterLen, int32_t IsUseRec, float EchoMultiple, float EchoCont, int32_t EchoSupes, int32_t EchoSupesAct, int IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseWebRtcAecm( AudioProcThread * AudioProcThreadPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseWebRtcAec( AudioProcThread * AudioProcThreadPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptiveAdjustDelay, int IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseSpeexWebRtcAec( AudioProcThread * AudioProcThreadPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMultiple, float SpeexAecEchoCont, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgnosticMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetUseNoNs( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseSpeexPprocNs( AudioProcThread * AudioProcThreadPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseWebRtcNsx( AudioProcThread * AudioProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseWebRtcNs( AudioProcThread * AudioProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseRNNoise( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetSpeexPprocOther( AudioProcThread * AudioProcThreadPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCont, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetUsePcm( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseSpeexCodec( AudioProcThread * AudioProcThreadPt, int32_t EncoderUseCbrOrVbr, int32_t EncoderQuality, int32_t EncoderComplexity, int32_t EncoderPlcExpectedLossRate, int32_t DecoderIsUsePerceptualEnhancement, VarStr * ErrInfoVarStrPt );
int AudioProcThreadSetUseOpus( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );

int AudioProcThreadSetSaveAudioToFile( AudioProcThread * AudioProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioInputFileFullPathStrPt, const char * AudioOutputFileFullPathStrPt, const char * AudioResultFileFullPathStrPt, VarStr * ErrInfoVarStrPt );

int AudioProcThreadStart( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );
int AudioProcThreadRequireExit( AudioProcThread * AudioProcThreadPt, int ExitFlag, int IsBlockWait, VarStr * ErrInfoVarStrPt );
int AudioProcThreadDestroy( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
