#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaPocsThrd.h"

#ifndef __MYMEDIAPOCSTHRD_H__
#define __MYMEDIAPOCSTHRD_H__

typedef enum //主对话框窗口消息。
{
	MainDlgWndMsgMediaPocsThrdInit = WM_USER + 101, //初始化媒体处理线程消息。加下划线是为了与MediaPocsThrdInit函数区分，否则会报重定义。
	MainDlgWndMsgMediaPocsThrdDstoy, //销毁媒体处理线程消息。加下划线是为了与MediaPocsThrdDstoy函数区分，否则会报重定义。
	MainDlgWndMsgRqstCnctDlgInit, //初始化请求连接对话框消息。
	MainDlgWndMsgRqstCnctDlgDstoy, //销毁请求连接对话框消息。
	MainDlgWndMsgPttBtnInit, //初始化一键即按即通按钮消息。
	MainDlgWndMsgPttBtnDstoy, //销毁一键即按即通按钮消息。
	MainDlgWndMsgShowLog, //显示日志消息。
} MainDlgWndMsg;

typedef enum //对讲状态消息。
{
	TkbkStsMsgCreateSrvrStart = WM_USER + 101, //开始创建服务器。
	TkbkStsMsgCreateSrvrSucs, //创建服务器成功。
	TkbkStsMsgCreateSrvrFail, //创建服务器失败。
	TkbkStsMsgCnctSrvrStart, //开始连接服务器。
	TkbkStsMsgCnctSucs, //连接服务器或接受连接成功。
	TkbkStsMsgCnctFail, //连接服务器或接受连接失败。
	TkbkStsMsgAllowCnct, //允许连接。
	TkbkStsMsgRefuseCnct, //拒绝连接。
	TkbkStsMsgInterruptTkbk, //中断对讲。
} TkbkStsMsg;

//我的媒体处理线程。
class MyMediaPocsThrdCls : public MediaPocsThrdCls
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	HWND m_TkbkStsWndHdl; //存放对讲状态窗口的句柄。
	int m_IsInterrupt; //存放是否中断，为0表示未中断，为1表示已中断。

	struct //存放网络。
	{
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

		int m_IsAutoAllowCnct; //存放是否自动允许连接，为0表示手动，为1表示自动。
		int m_RqstCnctRslt; //存放请求连接的结果，为0表示没有选择，为1表示允许，为2表示拒绝。
	} m_Ntwk;
	typedef enum //数据包类型。
	{
		PktTypAllowCnct, //允许连接包。
		PktTypRefuseCnct, //拒绝连接包。
		PktTypTkbkMode, //对讲模式包。
		PktTypAdoFrm, //音频输入输出帧包。
		PktTypVdoFrm, //视频输入输出帧包。
		PktTypExit, //退出包。
	} PktTyp;
	
	typedef enum //对讲模式。
	{
		TkbkModeNone, //空。
		TkbkModeAdo, //音频。
		TkbkModeVdo, //视频。
		TkbkModeAdoVdo, //音视频。
		TkbkModeNoChg, //不变。
	} TkbkMode;
	TkbkMode m_LclTkbkMode; //存放本端对讲模式。
	TkbkMode m_RmtTkbkMode; //存放远端对讲模式。

    int m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
    uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
    uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOtptFrm; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。

    VLnkLstCls m_RecvAdoOtptFrmLnkLst; //存放接收音频输出帧链表。
	VLnkLstCls m_RecvVdoOtptFrmLnkLst; //存放接收视频输出帧链表。

	struct //存放音频自适应抖动缓冲器。
	{
		AAjb * m_Pt; //存放指针。
		int m_MinNeedBufFrmCnt; //存放最小需缓冲帧数量，单位个，必须大于0。
		int m_MaxNeedBufFrmCnt; //存放最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
		int m_MaxCntuLostFrmCnt; //最大连续丢失帧的数量，单位为个帧，取值区间为[1,2147483647]，当连续丢失帧的数量超过最大时，认为是对方中途暂停发送。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	} m_AAjb;
	struct //存放视频自适应抖动缓冲器。
	{
		VAjb * m_Pt; //存放指针。
		int m_MinNeedBufFrmCnt; //存放最小需缓冲帧数量，单位个，必须大于0。
		int m_MaxNeedBufFrmCnt; //存放最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	} m_VAjb;

	int8_t * m_TmpBytePt; //存放临时数据的指针。
	size_t m_TmpByteSz; //存放临时数据的大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的指针。
	size_t m_TmpByte2Sz; //存放临时数据的大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的指针。
	size_t m_TmpByte3Sz; //存放临时数据的大小。
	
	typedef enum //我的媒体处理线程消息。
	{
		UserMsgTypLclTkbkMode, //本端对讲模式。
		UserMsgTypRmtTkbkMode, //远端对讲模式。
		UserMsgTypPttBtnDown, //一键即按即通按钮按下。
		UserMsgTypPttBtnUp, //一键即按即通按钮弹起。
	} UserMsgTyp;
	typedef struct
	{
		UserMsgTyp m_UserMsgTyp = UserMsgTypLclTkbkMode;
		TkbkMode m_LclTkbkMode;
	} UserMsgLclTkbkMode;
	typedef struct
	{
		UserMsgTyp m_UserMsgTyp = UserMsgTypRmtTkbkMode;
		TkbkMode m_RmtTkbkMode;
	} UserMsgRmtTkbkMode;
	typedef struct
	{
		UserMsgTyp m_UserMsgTyp = UserMsgTypPttBtnDown;
	} UserMsgPttBtnDown;
	typedef struct
	{
		UserMsgTyp m_UserMsgTyp = UserMsgTypPttBtnUp;
	} UserMsgPttBtnUp;

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
	void UserReadAdoVdoInptFrm( int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
								uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
								uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
								uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
								uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );

	//用户定义的写入音频输出帧函数。
	void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
							  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
							  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );

	//用户定义的获取音频输出帧函数。
	void UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
							int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
							uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );

	//用户定义的写入视频输出帧函数。
	void UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
							  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );

	//用户定义的获取视频输出帧函数。
	void UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
							uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );
};

#endif
