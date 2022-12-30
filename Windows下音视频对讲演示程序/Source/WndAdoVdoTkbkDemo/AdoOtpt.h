#include "MediaPocsThrd.h"

#ifndef __ADOOTPT_H__
#define __ADOOTPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;

typedef struct AdoOtptStrm //音频输出流。
{
	int32_t m_AdoOtptStrmIdx; //存放音频输出流索引。

	int32_t m_IsUseAdoOtptStrm; //存放是否使用音频输出流，为0表示不使用，为非0表示要使用。

	int32_t m_UseWhatDecd; //存放使用什么解码器，为0表示PCM原始数据，为1表示Speex解码器，为2表示Opus解码器。

	SpeexDecd * m_SpeexDecdPt; //存放Speex解码器的指针。
	int32_t m_SpeexDecdIsUsePrcplEnhsmt; //存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
} AdoOtptStrm;

typedef struct AdoOtpt //音频输出。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUseAdoOtpt; //存放是否使用音频输出，为0表示不使用，为非0表示要使用。
	int32_t m_IsInitAdoOtpt; //存放是否初始化音频输入，为0表示未初始化，为非0表示已初始化。

	int32_t m_SmplRate; //存放采样频率，单位赫兹，取值只能为8000、16000、32000、48000。
	int32_t m_FrmLen; //存放帧的长度，单位采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。

	ConstLenLnkLstCls m_AdoOtptStrmLnkLst; //存放音频输出流链表。
	int32_t m_AdoOtptStrmUseTotal; //存放音频输出流要使用的总数。

	int32_t m_IsSaveAdoToFile; //存放是否保存音频到文件，非0表示要使用，0表示不使用。
	WaveFileWriter * m_AdoOtptWaveFileWriterPt; //存放音频输出Wave文件写入器的指针。
	Vstr * m_AdoOtptFileFullPathVstrPt; //存放音频输出文件完整路径动态字符串的指针。

    int32_t m_IsDrawAdoWavfmToWnd; //存放是否绘制音频波形到窗口，为非0表示要绘制，为0表示不绘制。
    HWND m_AdoOtptWavfmWndHdl; //存放音频输出波形窗口的句柄。
    AdoWavfm * m_AdoOtptWavfmPt; //存放音频输出波形器对象的指针。

	UINT m_AdoOtptDvcID; //存放音频输出设备的标识符。
	IMMDeviceEnumerator * m_AdoOtptDvcEnumPt; //存放音频输出设备枚举器的指针。
	IMMDeviceCollection * m_AdoOtptDvcClctPt; //存放音频输出设备收集器的指针。
	IMMDevice * m_AdoOtptDvcPt; //存放音频输出设备的指针。
	IAudioClient * m_AdoOtptDvcClntPt; //存放音频输出设备客户端的指针。
	WAVEFORMATEX * m_AdoOtptDvcWaveFmtExPt; //存放音频输出设备格式的指针。
	UINT32 m_AdoOtptDvcBufSz; //存放音频输出设备的缓冲区大小，单位为采样单元。
	IAudioRenderClient * m_AdoOtptDvcRndrClntPt; //存放音频输出设备渲染客户端的指针。
	IGlobalInterfaceTable * m_AdoOtptDvcGlblIntfcTablePt; //存放音频输出设备全局接口表的指针。
	DWORD m_AdoOtptDvcRndrClntCookie; //存放音频输出设备渲染客户端的Cookie。
	size_t m_AdoOtptDvcFrmSz; //存放音频输出设备帧的大小，单位为采样单元。
	SpeexResamplerState * m_AdoOtptDvcSpeexResamplerPt; //存放音频输出设备Speex重采样器的指针。
	int32_t m_AdoOtptIsMute; //存放音频输出是否静音，为0表示有声音，为非0表示静音。
	int32_t m_AdoOtptDvcIsClos; //存放音频输出设备是否关闭，为0表示正常，为非0表示关闭。

	ConstLenLnkLstCls m_AdoOtptFrmLnkLst; //存放音频输出帧链表。
	ConstLenLnkLstCls m_AdoOtptIdleFrmLnkLst; //存放音频输出空闲帧链表。
	
	int32_t m_IsInitAdoOtptThrdTmpVar; //存放是否初始化音频输出线程的临时变量。
	int16_t * m_AdoOtptFrmPt; //存放音频输出帧的指针。
	int32_t * m_AdoOtptMixFrmPt; //存放音频输出混音帧的指针。
	uint8_t * m_EncdAdoOtptFrmPt; //存放已编码格式音频输出帧的指针。
	size_t m_EncdAdoOtptFrmSz; //存放已编码格式音频输出帧的大小，单位为字节。
	size_t m_AdoOtptFrmLen; //存放音频输出帧的长度，单位为字节。
	size_t m_AdoOtptFrmLnkLstElmTotal; //存放音频输出帧链表的元数总数。
	uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
	uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

	ThrdInfo * m_AdoOtptThrdInfoPt; //存放音频输出线程信息的指针。
	int32_t m_AdoOtptThrdIsStart; //存放音频输出线程是否开始，为0表示未开始，为1表示已开始。
	int32_t m_AdoOtptThrdExitFlag; //存放音频输出线程退出标记，0表示保持运行，1表示请求退出。
} AdoOtpt;

#ifdef __cplusplus
extern "C"
{
#endif

int AdoOtptAdoOtptStrmInit( AdoOtpt * AdoOtptPt, AdoOtptStrm * AdoOtptStrmPt );
void AdoOtptAdoOtptStrmDstoy( AdoOtpt * AdoOtptPt, AdoOtptStrm * AdoOtptStrmPt );
void AdoOtptAddAdoOtptStrm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx );
void AdoOtptDelAdoOtptStrm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx );
void AdoOtptSetAdoOtptStrmUsePcm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx );
void AdoOtptSetAdoOtptStrmUseSpeexDecd( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt );
void AdoOtptSetAdoOtptStrmUseOpusDecd( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx );
void AdoOtptSetAdoOtptStrmIsUse( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm );
int AdoOtptAdoOtptStrmLnkLstInit( AdoOtpt * AdoOtptPt );
void AdoOtptAdoOtptStrmLnkLstDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptWaveFileWriterInit( AdoOtpt * AdoOtptPt );
void AdoOtptWaveFileWriterDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptWavfmInit( AdoOtpt * AdoOtptPt );
void AdoOtptWavfmDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptDvcAndThrdInit( AdoOtpt * AdoOtptPt );
void AdoOtptDvcAndThrdDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptInit( AdoOtpt * AdoOtptPt );
void AdoOtptDstoy( AdoOtpt * AdoOtptPt );
DWORD WINAPI AdoOtptThrdRun( AdoOtpt * AdoOtptPt );

#ifdef __cplusplus
}
#endif

#endif
