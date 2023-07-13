#include "MediaPocsThrd.h"

#ifndef __VDOOTPT_H__
#define __VDOOTPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;

typedef struct //视频输出。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUse; //存放是否使用视频输出，为0表示不使用，为非0表示要使用。
	int32_t m_IsInit; //存放是否初始化视频输出，为0表示未初始化，为非0表示已初始化。
	
	static const int m_FrmMaxWidth = 960; //存放帧的最大宽度，单位为像素。
	static const int m_FrmMaxHeight = 1280; //存放帧的最大高度，单位为像素。

	typedef struct //存放帧。
	{
		uint32_t m_StrmIdx; //存放流索引。
		uint8_t * m_Yu12SrcFrmPt; //存放Yu12格式原始帧的指针，大小为m_FrmMaxWidth * m_FrmMaxHeight * 3 / 2字节。
		int32_t m_Yu12SrcFrmWidth; //存放Yu12格式原始帧的宽度，单位为像素。
		int32_t m_Yu12SrcFrmHeight; //存放Yu12格式原始帧的高度，单位为像素。
		uint8_t * m_EncdSrcFrmPt; //存放已编码格式原始帧的指针，大小为m_FrmMaxWidth * m_FrmMaxHeight * 3 / 2字节。
		size_t m_EncdSrcFrmLenByt; //存放已编码格式原始帧的长度，单位为字节。
		uint64_t m_TimeStampMsec; //存放时间戳，单位为毫秒。
	} Frm;
	CQueueCls m_FrmCntnr; //存放帧容器。
	CQueueCls m_IdleFrmCntnr; //存放空闲帧容器。

	typedef struct //存放流。
	{
		uint32_t m_Idx; //存放流索引。

		int32_t m_IsUse; //存放是否使用流，为0表示不使用，为非0表示要使用。

		int32_t m_UseWhatDecd; //存放使用什么编码器，为0表示Yu12原始数据，为1表示OpenH264解码器。

		struct //存放OpenH264解码器。
		{
			OpenH264Decd * m_Pt; //存放指针。
			int32_t m_DecdThrdNum; //存放解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
		} m_OpenH264Decd;
		
		struct //存放设备。
		{
			HWND m_DspyWndHdl; //存放显示窗口的句柄。
			int32_t m_IsBlack; //存放是否黑屏，为0表示有图像，为非0表示黑屏。
		} m_Dvc;

		struct //存放线程。
		{
			int32_t m_IsInitThrdTmpVar; //存放是否初始化线程的临时变量。
			uint8_t * m_BgraRlstFrmPt; //存放Bgra8888格式结果帧的指针，大小为m_FrmMaxWidth * m_FrmMaxHeight * 4字节。
			Frm * m_FrmPt; //存放帧的指针。
			size_t m_ElmTotal; //存放元素总数。
			uint64_t m_LastTickMsec; //存放上次的嘀嗒钟，单位为毫秒。
			uint64_t m_NowTickMsec; //存放本次的嘀嗒钟，单位为毫秒。

			ThrdInfo * m_ThrdInfoPt; //存放线程信息的指针。
			int32_t m_ExitFlag; //存放退出标记，0表示保持运行，1表示请求退出。
		} m_Thrd;
	} Strm;
	CQueueCls m_StrmCntnr; //存放流容器。
} VdoOtpt;

#ifdef __cplusplus
extern "C"
{
#endif

int VdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
void VdoOtptStrmDecdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
int VdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
void VdoOtptStrmThrdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
int VdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
void VdoOtptStrmDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt );
void VdoOtptAddStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx );
void VdoOtptDelStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx );
void VdoOtptSetStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, HWND DspyWndHdl );
void VdoOtptSetStrmUseYu12( VdoOtpt * VdoOtptPt, uint32_t StrmIdx );
void VdoOtptSetStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t DecdThrdNum );
void VdoOtptSetStrmIsBlack( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t IsBlack );
void VdoOtptSetStrmIsUse( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t IsUseStrm );
int VdoOtptInit( VdoOtpt * VdoOtptPt );
void VdoOtptDstoy( VdoOtpt * VdoOtptPt );

#ifdef __cplusplus
}
#endif

#endif
