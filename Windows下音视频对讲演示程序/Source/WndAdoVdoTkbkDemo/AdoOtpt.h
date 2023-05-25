#include "MediaPocsThrd.h"

#ifndef __ADOOTPT_H__
#define __ADOOTPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;

typedef struct //音频输出。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUse; //存放是否使用音频输出，为0表示不使用，为非0表示要使用。
	int32_t m_IsInit; //存放是否初始化音频输出，为0表示未初始化，为非0表示已初始化。

	int32_t m_SmplRate; //存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
	size_t m_FrmLenMsec; //存放帧的长度，单位为毫秒，取值只能为10毫秒的倍数。
	size_t m_FrmLenUnit; //存放帧的长度，单位为采样单元，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	size_t m_FrmLenData; //存放帧的长度，单位为采样数据，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80、20毫秒为160、30毫秒为240，16000Hz的10毫秒为160、20毫秒为320、30毫秒为480，32000Hz的10毫秒为320、20毫秒为640、30毫秒为960，48000Hz的10毫秒为480、20毫秒为960、30毫秒为1440。
	size_t m_FrmLenByt; //存放帧的长度，单位为字节，取值只能为10毫秒的倍数。例如：8000Hz的10毫秒为80*2、20毫秒为160*2、30毫秒为240*2，16000Hz的10毫秒为160*2、20毫秒为320*2、30毫秒为480*2，32000Hz的10毫秒为320*2、20毫秒为640*2、30毫秒为960*2，48000Hz的10毫秒为480*2、20毫秒为960*2、30毫秒为1440*2。
	
	typedef struct //存放流。
	{
		int32_t m_Idx; //存放索引。

		int32_t m_IsUse; //存放是否使用流，为0表示不使用，为非0表示要使用。

		int32_t m_UseWhatDecd; //存放使用什么解码器，为0表示PCM原始数据，为1表示Speex解码器，为2表示Opus解码器。

		struct //存放Speex解码器。
		{
			SpeexDecd * m_Pt; //存放指针。
			int32_t m_IsUsePrcplEnhsmt; //存放是否使用知觉增强，为非0表示要使用，为0表示不使用。
		} m_SpeexDecd;
	} Strm;
	CQueueCls m_StrmCntnr; //存放流容器。
	int32_t m_StrmUseTotal; //存放流要使用的总数。
	
	struct //存放波形器。
	{
		int32_t m_IsDrawAdoWavfmToWnd; //存放是否绘制音频波形到窗口，为非0表示要绘制，为0表示不绘制。
		AdoWavfm * m_SrcWavfmPt; //存放原始波形器的指针。
		HWND m_SrcWavfmWndHdl; //存放原始波形窗口的句柄。
	} m_Wavfm;

	struct //存放Wave文件。
	{
		int32_t m_IsSaveAdoToWaveFile; //存放是否保存音频到Wave文件，非0表示要使用，0表示不使用。
		WaveFileWriter * m_SrcWaveFileWriterPt; //存放原始Wave文件写入器的指针。
		Vstr * m_SrcWaveFileFullPathVstrPt; //存放原始Wave文件完整路径动态字符串的指针。
		size_t m_WaveFileWrBufSzByt; //存放Wave文件写入缓冲区的大小，单位为字节。
	} m_WaveFile;
	
	struct //存放设备。
	{
		UINT m_ID; //存放标识符，取值范围为从1到音频输出设备的总数，为0表示使用默认设备。
		IMMDeviceEnumerator * m_EnumPt; //存放枚举器的指针。
		IMMDeviceCollection * m_ClctPt; //存放收集器的指针。
		IMMDevice * m_Pt; //存放指针。
		IAudioClient * m_ClntPt; //存放客户端的指针。
		WAVEFORMATEX * m_WaveFmtExPt; //存放格式的指针。
		UINT32 m_BufSzUnit; //存放缓冲区大小，单位为采样单元。
		IAudioRenderClient * m_RndrClntPt; //存放渲染客户端的指针。
		IGlobalInterfaceTable * m_GlblIntfcTablePt; //存放全局接口表的指针。
		DWORD m_RndrClntCookie; //存放渲染客户端的Cookie。
		int16_t * m_PcmBufFrmPt; //存放Pcm格式缓冲区帧的指针。
		size_t m_PcmBufFrmLenUnit; //存放Pcm格式缓冲区帧的长度，单位为采样单元。
		size_t m_PcmBufFrmLenByt; //存放Pcm格式缓冲区帧的长度，单位为字节。
		SpeexResamplerState * m_PcmBufFrmSpeexResamplerPt; //存放Pcm格式缓冲区帧Speex重采样器的指针。
		int32_t m_IsMute; //存放是否静音，为0表示有声音，为非0表示静音。
		int32_t m_IsClos; //存放是否关闭，为0表示正常，为非0表示关闭。
	} m_Dvc;

	CQueueCls m_PcmSrcFrmCntnr; //存放Pcm格式原始帧容器。
	CQueueCls m_PcmIdleFrmCntnr; //存放Pcm格式空闲帧容器。
	
	struct //存放线程。
	{
		int32_t m_IsInitThrdTmpVar; //存放是否初始化线程的临时变量。
		int16_t * m_PcmSrcFrmPt; //存放Pcm格式原始帧的指针。
		uint8_t * m_EncdSrcFrmPt; //存放已编码格式原始帧的指针。
		size_t m_EncdSrcFrmSzByt; //存放已编码格式原始帧的大小，单位为字节。
		size_t m_EncdSrcFrmLenByt; //存放已编码格式原始帧的长度，单位为字节。
		int32_t * m_PcmMixFrmPt; //存放Pcm格式混音帧的指针。
		size_t m_ElmTotal; //存放元素总数。
		uint64_t m_LastTickMsec; //存放上次的嘀嗒钟，单位为毫秒。
		uint64_t m_NowTickMsec; //存放本次的嘀嗒钟，单位为毫秒。

		ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
		int32_t m_ThrdIsStart; //存放线程是否开始，为0表示未开始，为1表示已开始。
		int32_t m_ExitFlag; //存放退出标记，0表示保持运行，1表示请求退出。
	} m_Thrd;
} AdoOtpt;

#ifdef __cplusplus
extern "C"
{
#endif

int AdoOtptStrmInit( AdoOtpt * AdoOtptPt, AdoOtpt::Strm * StrmPt );
void AdoOtptStrmDstoy( AdoOtpt * AdoOtptPt, AdoOtpt::Strm * StrmPt );
void AdoOtptAddStrm( AdoOtpt * AdoOtptPt, int32_t StrmIdx );
void AdoOtptDelStrm( AdoOtpt * AdoOtptPt, int32_t StrmIdx );
void AdoOtptSetStrmUsePcm( AdoOtpt * AdoOtptPt, int32_t StrmIdx );
void AdoOtptSetStrmUseSpeexDecd( AdoOtpt * AdoOtptPt, int32_t StrmIdx, int32_t IsUsePrcplEnhsmt );
void AdoOtptSetStrmUseOpusDecd( AdoOtpt * AdoOtptPt, int32_t StrmIdx );
void AdoOtptSetStrmIsUse( AdoOtpt * AdoOtptPt, int32_t StrmIdx, int32_t IsUseStrm );
int AdoOtptStrmCntnrInit( AdoOtpt * AdoOtptPt );
void AdoOtptStrmCntnrDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptWaveFileWriterInit( AdoOtpt * AdoOtptPt );
void AdoOtptWaveFileWriterDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptWavfmInit( AdoOtpt * AdoOtptPt );
void AdoOtptWavfmDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptDvcAndThrdInit( AdoOtpt * AdoOtptPt );
void AdoOtptDvcAndThrdDstoy( AdoOtpt * AdoOtptPt );
int AdoOtptInit( AdoOtpt * AdoOtptPt );
void AdoOtptDstoy( AdoOtpt * AdoOtptPt );

#ifdef __cplusplus
}
#endif

#endif
