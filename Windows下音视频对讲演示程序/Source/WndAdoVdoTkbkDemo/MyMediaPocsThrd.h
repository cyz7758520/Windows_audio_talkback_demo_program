#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaPocsThrd.h"

#ifndef __MYMEDIAPOCSTHRD_H__
#define __MYMEDIAPOCSTHRD_H__

//窗口消息。
typedef enum
{
	MediaPocsThrd_Init = WM_USER + 101, //主窗口消息：初始化媒体处理线程。
	MediaPocsThrd_Dstoy, //主窗口消息：销毁媒体处理线程。
	RqstCnctDlgInit, //主窗口消息：初始化请求连接对话框。
	RqstCnctDlgDstoy, //主窗口消息：销毁请求连接对话框。
	PttBtnInit, //主窗口消息：初始化一键即按即通按钮。
	PttBtnDstoy, //主窗口消息：销毁一键即按即通按钮。
	ShowLog, //主窗口消息：显示日志。
}WndMsg;

//我的媒体处理线程。
class MyMediaPocsThrdCls : public MediaPocsThrdCls
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	int m_IsInterrupt; //存放是否中断，为0表示未中断，为1表示已中断。

	typedef enum
	{
		LclTkbkMode, //我的媒体处理线程消息：本端对讲模式。
		RmtTkbkMode, //我的媒体处理线程消息：远端对讲模式。
		PttBtnDown, //我的媒体处理线程消息：一键即按即通按钮按下。
		PttBtnUp, //我的媒体处理线程消息：一键即按即通按钮弹起。
	}UserMsg;

	Vstr * m_IPAddrVstrPt; //存放IP地址动态字符串的指针。
    Vstr * m_PortVstrPt; //存放端口动态字符串的指针。
	int m_XfrMode; //存放传输模式，为0表示实时半双工（一键通），为1表示实时全双工。
	int m_PttBtnIsDown; //存放一键即按即通按钮是否按下，为0表示弹起，为非0表示按下。
	int m_MaxCnctTimes; //存放最大连接次数，取值区间为[1,2147483647]。
    int m_UseWhatXfrPrtcl; //存放使用什么传输协议，为0表示TCP协议，为1表示UDP协议。
    int m_IsCreateSrvrOrClnt; //存放创建服务端或者客户端标记，为1表示创建服务端，为0表示创建客户端。
    TcpSrvrSoktCls m_TcpSrvrSokt; //存放本端TCP协议服务端套接字。
    TcpClntSoktCls m_TcpClntSokt; //存放本端TCP协议客户端套接字。
	AudpSoktCls m_AudpSokt; //存放本端高级UDP协议套接字。
	size_t m_AudpCnctIdx; //存放本端高级UDP协议连接索引。
	#define PKT_TYP_ALLOW_CNCT  1 //数据包类型：允许连接包。
	#define PKT_TYP_REFUSE_CNCT 2 //数据包类型：拒绝连接包。
	#define PKT_TYP_TKBK_MODE   3 //数据包类型：对讲模式。
	#define PKT_TYP_ADO_FRM     4 //数据包类型：音频输入输出帧。
	#define PKT_TYP_VDO_FRM     5 //数据包类型：视频输入输出帧。
	#define PKT_TYP_EXIT        6 //数据包类型：退出包。
	
	int m_IsAutoAllowCnct; //存放是否自动允许连接，为0表示手动，为1表示自动。
	int m_RqstCnctRslt; //存放请求连接的结果，为0表示没有选择，为1表示允许，为2表示拒绝。
	
	typedef enum
	{
		None, //对讲模式：空。
		Ado, //对讲模式：音频。
		Vdo, //对讲模式：视频。
		AdoVdo, //对讲模式：音视频。
		NoChg, //对讲模式：不变。
	}TkbkMode;
	TkbkMode m_LclTkbkMode; //存放本端对讲模式。
	TkbkMode m_RmtTkbkMode; //存放远端对讲模式。

    int m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
    uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
    uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOtptFrm; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。

    VarLenLnkLstCls m_RecvAdoOtptFrmLnkLst; //存放接收音频输出帧链表。
	VarLenLnkLstCls m_RecvVdoOtptFrmLnkLst; //存放接收视频输出帧链表。

	AAjb * m_AAjbPt; //存放音频自适应抖动缓冲器的指针。
	int m_AAjbMinNeedBufFrmCnt; //存放音频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
	int m_AAjbMaxNeedBufFrmCnt; //存放音频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
	int m_AAjbMaxCntuLostFrmCnt; //音频自适应抖动缓冲器的最大连续丢失帧的数量，单位为个帧，取值区间为[1,2147483647]，当连续丢失帧的数量超过最大时，认为是对方中途暂停发送。
	float m_AAjbAdaptSensitivity; //存放音频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	VAjb * m_VAjbPt; //存放视频自适应抖动缓冲器的指针。
	int m_VAjbMinNeedBufFrmCnt; //存放视频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
	int m_VAjbMaxNeedBufFrmCnt; //存放视频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
	float m_VAjbAdaptSensitivity; //存放视频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。

	int8_t * m_TmpBytePt; //存放临时数据的指针。
	size_t m_TmpByteSz; //存放临时数据的大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的指针。
	size_t m_TmpByte2Sz; //存放临时数据的大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的指针。
	size_t m_TmpByte3Sz; //存放临时数据的大小。
	
	typedef struct
	{
		UserMsg m_MsgTyp = LclTkbkMode;
		TkbkMode m_LclTkbkMode;
	}UserMsgLclTkbkMode;
	typedef struct
	{
		UserMsg m_MsgTyp = RmtTkbkMode;
		TkbkMode m_RmtTkbkMode;
	}UserMsgRmtTkbkMode;
	typedef struct
	{
		UserMsg m_MsgTyp = PttBtnDown;
	}UserMsgPttBtnDown;
	typedef struct
	{
		UserMsg m_MsgTyp = PttBtnUp;
	}UserMsgPttBtnUp;

	//构造函数。
	MyMediaPocsThrdCls( HWND MainDlgWndHdl );

	//析构函数。
	~MyMediaPocsThrdCls();

	//用户定义的初始化函数。
	int UserInit();

	//用户定义的处理函数。
	int UserPocs();

	//用户定义的销毁函数。
	void UserDstoy();
	
	//设置对讲模式。
	void SetTkbkMode();
	
	//用户定义的消息函数。
	int UserMsg( void * MsgArgPt );

	//用户定义的读取音视频输入帧函数。
	void UserReadAdoVdoInptFrm( int16_t * PcmAdoInptSrcFrmPt, int16_t * PcmAdoInptRsltFrmPt, int32_t PcmAdoInptFrmLenUnit, int32_t PcmAdoInptRsltFrmVoiceActSts,
								uint8_t * EncdAdoInptRsltFrmPt, size_t EncdAdoInptRsltFrmLenByt, int32_t EncdAdoInptRsltFrmIsNeedTrans,
								uint8_t * BgraVdoInptSrcFrmPt, int32_t BgraVdoInptSrcFrmWidth, int32_t BgraVdoInptSrcFrmHeight, size_t BgraVdoInptSrcFrmLenByt,
								uint8_t * YU12VdoInptRsltFrmPt, int32_t YU12VdoInptRsltFrmWidth, int32_t YU12VdoInptRsltFrmHeight, size_t YU12VdoInptRsltFrmLenByt,
								uint8_t * EncdVdoInptRsltFrmPt, size_t EncdVdoInptRsltFrmLenByt );

	//用户定义的写入音频输出帧函数。
	void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
							  int16_t * PcmAdoOtptSrcFrmPt, int32_t PcmAdoOtptFrmLenUnit,
							  uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmSzByt, size_t * EncdAdoOtptSrcFrmLenBytPt );

	//用户定义的获取音频输出帧函数。
	void UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
							int16_t * PcmAdoOtptSrcFrmPt, int32_t PcmAdoOtptFrmLenUnit,
							uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmLenByt );

	//用户定义的写入视频输出帧函数。
	void UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							  uint8_t * YU12VdoOtptSrcFrmPt, int32_t * YU12VdoOtptSrcFrmWidthPt, int32_t * YU12VdoOtptSrcFrmHeightPt,
							  uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmSzByt, size_t * EncdVdoOtptSrcFrmLenBytPt );

	//用户定义的获取视频输出帧函数。
	void UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							uint8_t * YU12VdoOtptSrcFrmPt, int32_t YU12VdoOtptSrcFrmWidth, int32_t YU12VdoOtptSrcFrmHeight,
							uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmLenByt );
};

#endif
