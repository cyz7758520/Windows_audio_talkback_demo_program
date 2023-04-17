#include "MediaPocsThrd.h"

#ifndef __VDOINPT_H__
#define __VDOINPT_H__

typedef struct MediaPocsThrd MediaPocsThrd;
typedef struct VdoInptThrd VdoInptThrd;

typedef struct //视频输入。
{
	MediaPocsThrd * m_MediaPocsThrdPt; //存放媒体处理线程的指针。

	int32_t m_IsUse; //存放是否使用视频输入，为0表示不使用，为非0表示要使用。
	int32_t m_IsInit; //存放是否初始化视频输入，为0表示未初始化，为非0表示已初始化。

	int32_t m_MaxSmplRate; //存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
	int32_t m_FrmWidth; //存放帧的宽度，单位为像素。
	int32_t m_FrmHeight; //存放帧的高度，单位为像素。
	size_t m_YU12FrmLenByt; //存放YU12格式帧的长度，单位为字节，为m_FrmWidth * m_FrmHeight * 3 / 2。

	int32_t m_UseWhatEncd; //存放使用什么编码器，为0表示YU12原始数据，为1表示OpenH264编码器。

	struct //存放OpenH264编码器。
	{
		OpenH264Encd * m_Pt; //存放指针。
		int32_t m_VdoType; //存放视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
		int32_t m_EncdBitrate; //存放编码后比特率，单位为bps。
		int32_t m_BitrateCtrlMode; //存放比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
		int32_t m_IDRFrmIntvl; //存放IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
		int32_t m_Cmplxt; //存放复杂度，复杂度越高压缩率不变、CPU使用率越高、画质越好，取值区间为[0,2]。
	} m_OpenH264Encd;

	struct //存放设备。
	{
		UINT m_ID; //存放标识符，取值范围为从0到视频输入设备的总数减一。
		IGraphBuilder * m_FilterGraphManagerPt; //存放过滤器图管理器的指针。
		IMediaEventEx * m_FilterGraphMediaEventPt; //存放过滤器图媒体事件器的指针。
		IMediaControl * m_FilterGraphMediaCtrlPt; //存放过滤器图媒体控制器的指针。
		int32_t m_BgraSrcFrmWidth; //存放BGRA8888格式原始帧的宽度，单位为像素。
		int32_t m_BgraSrcFrmHeight; //存放BGRA8888格式原始帧的高度，单位为像素。
		size_t m_BgraSrcFrmLenByt; //存放BGRA8888格式原始帧的长度，单位为字节，为m_Dvc.m_BgraSrcFrmWidth * m_Dvc.m_BgraSrcFrmHeight * 4。
		int32_t m_BgraSrcFrmIsCrop; //存放BGRA8888格式原始帧是否裁剪，为0表示不裁剪，为非0表示要裁剪。
		int32_t m_BgraSrcFrmCropX; //存放BGRA8888格式原始帧裁剪区域左上角的横坐标，单位像素。
		int32_t m_BgraSrcFrmCropY; //存放BGRA8888格式原始帧裁剪区域左上角的纵坐标，单位像素。
		int32_t m_BgraSrcFrmCropWidth; //存放BGRA8888格式原始帧裁剪区域的宽度，单位像素。
		int32_t m_BgraSrcFrmCropHeight; //存放BGRA8888格式原始帧裁剪区域的高度，单位像素。
		int32_t m_BgraSrcFrmIsScale; //存放BGRA8888格式原始帧是否缩放，为0表示不缩放，为非0表示要缩放。
		int32_t m_BgraSrcFrmScaleWidth; //存放BGRA8888格式原始帧缩放后的宽度，单位为像素，为m_FrmWidth。
		int32_t m_BgraSrcFrmScaleHeight; //存放BGRA8888格式原始帧缩放后的高度，单位为像素，为m_FrmHeight。
        size_t m_BgraSrcFrmScaleLenByt; //存放BGRA8888格式原始帧缩放后的长度，单位为字节，为m_BgraSrcFrmScaleWidth * m_BgraSrcFrmScaleHeight * 4。
		HWND m_PrvwWndHdl; //存放预览窗口的句柄。
		int32_t m_IsBlack; //存放是否黑屏，为0表示有图像，为非0表示黑屏。
	} m_Dvc;

	typedef struct //存放帧。
	{
		uint8_t * m_BgraSrcFrmPt; //存放BGRA8888格式原始帧的指针，宽度为m_Dvc.m_BgraSrcFrmWidth，高度为m_Dvc.m_BgraSrcFrmHeight，大小为m_Dvc.m_BgraSrcFrmLenByt。
		uint8_t * m_YU12RsltFrmPt; //存放YU12格式结果帧的指针，宽度为m_FrmWidth，高度为m_FrmHeight，大小为m_YU12FrmLenByt。
		uint8_t * m_EncdRsltFrmPt; //存放已编码格式结果帧的指针，大小为m_YU12FrmLenByt。
		size_t m_EncdRsltFrmLenByt; //存放已编码格式结果帧的长度，单位为字节。
		uint64_t m_TimeStampMsec; //存放时间戳，单位为毫秒。
	} Frm;
	ConstLenLnkLstCls m_FrmLnkLst; //存放帧链表。
	ConstLenLnkLstCls m_IdleFrmLnkLst; //存放空闲帧链表。
	
	struct //存放线程。
	{
		int32_t m_IsInitThrdTmpVar; //存放是否初始化线程的临时变量。
		double m_LastFrmTickMsec; //存放上一个帧的嘀嗒钟，单位为毫秒。
		double m_FrmTimeStepMsec; //存放帧的时间步进，单位为毫秒。
		uint8_t * m_TmpFrm1Pt; //存放临时帧的指针。
		uint8_t * m_TmpFrm2Pt; //存放临时帧的指针。
		size_t m_TmpFrmSzByt; //存放临时帧的大小，单位为字节。
		size_t m_TmpFrmLenByt; //存放临时帧的长度，单位为字节。
		Frm * m_FrmPt; //存放帧的指针。
		size_t m_LnkLstElmTotal; //存放链表的元素总数。
		uint64_t m_LastTickMsec; //存放上次的嘀嗒钟，单位为毫秒。
		uint64_t m_NowTickMsec; //存放本次的嘀嗒钟，单位为毫秒。

		VdoInptThrd * m_ThrdPt; //存放线程的指针。
	} m_Thrd;
} VdoInpt;

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
