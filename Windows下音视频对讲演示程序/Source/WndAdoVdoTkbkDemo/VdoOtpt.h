#include "MediaPocsThrd.h"

#ifndef __VDOOTPT_H__
#define __VDOOTPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;
typedef struct VdoOtptThrdParm VdoOtptThrdParm;

typedef struct VdoOtpt //视频输出。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUseVdoOtpt; //存放是否使用视频输出，为0表示不使用，为非0表示要使用。
	int32_t m_IsInitVdoOtpt; //存放是否初始化视频输出，为0表示未初始化，为非0表示已初始化。
	
	typedef struct VdoOtptFrm //视频输出帧。
	{
		uint32_t m_VdoOtptStrmIdx; //存放视频输出流索引。
		uint8_t * m_YU12VdoOtptSrcFrmPt; //存放YU12格式视频输入结果帧的指针，大小为FrmMaxWidth * FrmMaxHeight * 3 / 2字节。
		int32_t m_YU12VdoOtptSrcFrmWidth; //存放YU12格式视频输出原始帧的宽度，单位为像素。
		int32_t m_YU12VdoOtptSrcFrmHeight; //存放YU12格式视频输出原始帧的高度，单位为像素。
		uint8_t * m_EncdVdoOtptSrcFrmPt; //存放已编码格式视频输出原始帧的指针，大小为FrmMaxWidth * FrmMaxHeight * 3 / 2字节。
		size_t m_EncdVdoOtptSrcFrmLenByt; //存放已编码格式视频输出原始帧的长度，单位为字节。
		uint64_t m_TimeStampMsec; //存放时间戳，单位为毫秒。
	}VdoOtptFrm;
	ConstLenLnkLstCls m_VdoOtptFrmLnkLst; //存放视频输出帧链表。
	ConstLenLnkLstCls m_VdoOtptIdleFrmLnkLst; //存放视频输出空闲帧链表。

	typedef struct VdoOtptStrm //视频输出流。
	{
		uint32_t m_VdoOtptStrmIdx; //存放视频输出流索引。

		int32_t m_IsUseVdoOtptStrm; //存放是否使用视频输出流，为0表示不使用，为非0表示要使用。

		int32_t m_UseWhatDecd; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264解码器。

		OpenH264Decd * m_OpenH264DecdPt; //存放OpenH264解码器的指针。
		int32_t m_OpenH264DecdDecdThrdNum; //存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
		
		HWND m_VdoOtptDspyWndHdl; //存放视频输出显示窗口的句柄。
		int32_t m_VdoOtptIsBlack; //存放视频输出是否黑屏，为0表示有图像，为非0表示黑屏。

		int32_t m_IsInitVdoOtptThrdTmpVar; //存放是否初始化视频输出线程的临时变量。
		uint8_t * m_BgraVdoOtptRlstFrmPt; //存放BGRA8888格式视频输出结果帧的指针，大小为FrmMaxWidth * FrmMaxHeight * 4字节。
		VdoOtpt::VdoOtptFrm * m_VdoOtptFrmPt; //存放视频输出帧的指针。
		size_t m_FrmLnkLstElmTotal; //存放帧链表的元素总数。
		uint64_t m_LastTickMsec; //存放上次的嘀嗒钟，单位为毫秒。
		uint64_t m_NowTickMsec; //存放本次的嘀嗒钟，单位为毫秒。

		ThrdInfo * m_VdoOtptThrdInfoPt; //存放视频输出线程信息的指针。
		int m_VdoOtptThrdExitFlag; //视频输出线程退出标记，0表示保持运行，1表示请求退出。
	}VdoOtptStrm;
	ConstLenLnkLstCls m_VdoOtptStrmLnkLst; //存放视频输出流链表。
}VdoOtpt;

#ifdef __cplusplus
extern "C"
{
#endif

int VdoOtptVdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmDecdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
int VdoOtptVdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmThrdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
int VdoOtptVdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::VdoOtptStrm * VdoOtptStrmPt );
DWORD WINAPI VdoOtptThrdRun( VdoOtptThrdParm * VdoOtptThrdParmPt );
void VdoOtptAddVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx );
void VdoOtptDelVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx );
void VdoOtptSetVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, HWND VdoOtptDspyWndHdl );
void VdoOtptSetVdoOtptStrmUseYU12( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx );
void VdoOtptSetVdoOtptStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum );
void VdoOtptSetVdoOtptStrmIsBlack( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t IsBlack );
void VdoOtptSetVdoOtptStrmIsUse( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm );
int VdoOtptInit( VdoOtpt * VdoOtptPt );
void VdoOtptDstoy( VdoOtpt * VdoOtptPt );

#ifdef __cplusplus
}
#endif

#endif
