#include "MediaPocsThrd.h"

#ifndef __VDOINPT_H__
#define __VDOINPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;
typedef struct VdoInptThrd VdoInptThrd;

typedef struct VdoInptFrmElm //视频输入帧链表元素。
{
	uint8_t * m_YU12VdoInptFrmPt; //存放YU12格式视频输入帧的指针，大小为m_FrmWidth * m_FrmHeight * 3 / 2。
	uint8_t * m_EncdVdoInptFrmPt; //存放已编码格式视频输入帧的指针，大小为m_FrmWidth * m_FrmHeight * 3 / 2。
	size_t m_EncdVdoInptFrmLen; //存放已编码格式视频输入帧的长度，单位为字节。
}VdoInptFrmElm;

typedef struct VdoInpt //视频输入。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUseVdoInpt; //存放是否使用视频输入，为0表示不使用，为非0表示要使用。
	int32_t m_IsInitVdoInpt; //存放是否初始化视频输入，为0表示未初始化，为非0表示已初始化。

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

	ConstLenLnkLstCls m_VdoInptFrmLnkLst; //存放视频输入帧链表。
	ConstLenLnkLstCls m_VdoInptIdleFrmLnkLst; //存放视频输入空闲帧链表。
	size_t m_VdoInptFrmElmTotal; //存放视频输入帧元素的总数。

	int32_t m_IsInitVdoInptThrdTmpVar; //存放是否初始化视频输入线程的临时变量。
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

#ifdef __cplusplus
extern "C"
{
#endif

int VdoInptInit( VdoInpt * VdoInptPt );
void VdoInptDstoy( VdoInpt * VdoInptPt );

#ifdef __cplusplus
}
#endif

#endif
