#include "MediaPocsThrd.h"

#ifndef __VDOOTPT_H__
#define __VDOOTPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;
typedef struct VdoOtptThrdParm VdoOtptThrdParm;

typedef struct VdoOtptStrm //视频输出流。
{
	int32_t m_VdoOtptStrmIdx; //存放视频输出流索引。

	int32_t m_IsUseVdoOtptStrm; //存放是否使用视频输出流，为0表示不使用，为非0表示要使用。

	int32_t m_UseWhatDecd; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264解码器。

	OpenH264Decd * m_OpenH264DecdPt; //存放OpenH264解码器的指针。
	int32_t m_OpenH264DecdDecdThrdNum; //存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。

	HWND m_VdoOtptDspyWndHdl; //存放视频输出显示窗口的句柄。
	float m_VdoOtptDspyScale; //存放视频输出显示缩放倍数。
	int32_t m_VdoOtptIsBlack; //存放视频输出是否黑屏，为0表示有图像，为非0表示黑屏。
	
	int32_t m_IsInitVdoOtptThrdTmpVar; //存放是否初始化视频输出线程的临时变量。
	uint8_t * m_VdoOtptRsltFrmPt; //存放视频输出结果帧的指针。
	uint8_t * m_VdoOtptTmpFrmPt; //存放视频输出临时帧的指针。
	uint8_t * m_VdoOtptSwapFrmPt; //存放视频输出交换帧的指针。
	size_t m_VdoOtptRsltFrmSz; //存放视频输出结果帧的大小，单位为字节。
	size_t m_VdoOtptRsltFrmLen; //存放视频输出结果帧的长度，单位为字节。
	int32_t m_VdoOtptFrmWidth; //存放视频输出帧的宽度，单位为像素。
	int32_t m_VdoOtptFrmHeight; //存放视频输出帧的高度，单位为像素。
	uint64_t m_LastTimeMsec; //存放上次时间的毫秒数。
	uint64_t m_NowTimeMsec; //存放本次时间的毫秒数。

	ThrdInfo * m_VdoOtptThrdInfoPt; //存放视频输出线程信息的指针。
	int m_VdoOtptThrdExitFlag; //视频输出线程退出标记，0表示保持运行，1表示请求退出。
}VdoOtptStrm;

typedef struct VdoOtpt //视频输出。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUseVdoOtpt; //存放是否使用视频输出，为0表示不使用，为非0表示要使用。
	int32_t m_IsInitVdoOtpt; //存放是否初始化视频输出，为0表示未初始化，为非0表示已初始化。

	ConstLenLnkLstCls m_VdoOtptStrmLnkLst; //存放视频输出流链表。
}VdoOtpt;

#ifdef __cplusplus
extern "C"
{
#endif

int VdoOtptVdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmDecdDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
int VdoOtptVdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmThrdDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
int VdoOtptVdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
void VdoOtptVdoOtptStrmDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt );
DWORD WINAPI VdoOtptThrdRun( VdoOtptThrdParm * VdoOtptThrdParmPt );
void VdoOtptAddVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx );
void VdoOtptDelVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx );
void VdoOtptSetVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, HWND VdoOtptDspyWndHdl, float VdoOtptDspyScale );
void VdoOtptSetVdoOtptStrmUseYU12( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx );
void VdoOtptSetVdoOtptStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t DecdThrdNum );
void VdoOtptSetVdoOtptStrmIsBlack( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t IsBlack );
void VdoOtptSetVdoOtptStrmIsUse( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm );
int VdoOtptInit( VdoOtpt * VdoOtptPt );
void VdoOtptDstoy( VdoOtpt * VdoOtptPt );

#ifdef __cplusplus
}
#endif

#endif
