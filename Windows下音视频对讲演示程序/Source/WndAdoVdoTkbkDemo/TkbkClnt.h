#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ClntMediaPocsThrd ClntMediaPocsThrd;

typedef struct TkbkClnt //对讲客户端。
{
	ClntMediaPocsThrd * m_ClntMediaPocsThrdPt; //存放客户端媒体处理线程的指针。
	
	struct TstNtwkDly //存放测试网络延迟。
	{
		int32_t m_IsTstNtwkDly; //存放是否测试网络延迟。
		uint64_t m_SendIntvlMsec; //存放发送间隔，单位为毫秒。
		uint64_t m_LastSendTickMsec; //存放最后发送的嘀嗒钟，单位为毫秒。
		int32_t m_IsRecvRplyPkt; //存放是否接收应答包，为0表示未接收，为1表示已接收。
	} m_TstNtwkDly;

	int32_t m_XfrMode; //存放传输模式，为0表示实时半双工（一键通），为1表示实时全双工。
	int32_t m_PttBtnIsDown; //存放一键即按即通按钮是否按下，为0表示弹起，为非0表示按下。

	typedef struct TkbkInfo //对讲信息。
	{
		int32_t m_TkbkIdx; //存放对讲索引，从0开始，对讲信息容器的唯一索引，连接中途不会改变。
		int32_t m_IsInit; //存放对讲信息是否初始化，为0表示未初始化，为非0表示已初始化。
		int32_t m_Num; //存放序号，从0开始，随着前面的连接销毁而递减。
		
		int32_t m_RmtTkbkMode; //存放远端对讲模式。

		VQueue * m_RecvAdoOtptFrmCntnrPt; //存放接收音频输出帧容器的指针。
		VQueue * m_RecvVdoOtptFrmCntnrPt; //存放接收视频输出帧容器的指针。

		AAjb * m_AAjbPt; //存放音频自适应抖动缓冲器的指针。
		VAjb * m_VAjbPt; //存放视频自适应抖动缓冲器的指针。
		
		struct AdoOtptTmpVar //存放音频输出临时变量。
		{
			int8_t * m_TmpBytePt; //存放临时数据的指针。
			size_t m_TmpByteSz; //存放临时数据大小。
		} m_AdoOtptTmpVar;
		
		struct VdoOtptTmpVar //存放视频输出临时变量。
		{
			int8_t * m_TmpBytePt; //存放临时数据的指针。
			size_t m_TmpByteSz; //存放临时数据大小。
		} m_VdoOtptTmpVar;
	} TkbkInfo;
	CQueue * m_TkbkInfoCntnrPt; //存放对讲信息容器的指针。
	int32_t m_TkbkInfoCurMaxNum; //存放对讲信息的当前最大序号。
	
	int32_t m_UseWhatRecvOtptFrm; //存放使用什么接收输出帧，为0表示容器，为1表示自适应抖动缓冲器。
	
	struct AAjbParm //存放音频自适应抖动缓冲器的参数。
	{
		int32_t m_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]。
		int32_t m_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]，必须大于等于最小需缓冲帧的数量。
		int32_t m_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量，单位为个帧，取值区间为[1,2147483647]，当连续丢失帧的数量超过最大时，认为是对方中途暂停发送。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大缓冲帧越多，取值区间为[0.0,127.0]。
	} m_AAjbParm;
	struct VAjbParm //存放视频自适应抖动缓冲器的参数。
	{
		int32_t m_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量，单位为个帧，取值区间为[0,2147483647]。
		int32_t m_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]，必须大于等于最小需缓冲帧数量。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	} m_VAjbParm;

	int32_t m_CnctIsInit; //存放连接是否初始化，为0表示未初始化，为非0表示已初始化。
	int32_t m_IsTcpOrAudpPrtcl; //存放是否是Tcp或Udp协议，为0表示Tcp协议，为1表示高级Udp协议。
	Vstr * m_RmtNodeNameVstrPt; //存放远端套接字绑定的远端节点名称动态字符串的指针。
	Vstr * m_RmtNodeSrvcVstrPt; //存放远端套接字绑定的远端节点服务动态字符串的指针。
	TcpClntSokt * m_TcpClntSoktPt; //存放本端Tcp协议客户端套接字的指针。
	size_t m_AudpClntCnctIdx; //存放本端高级Udp协议客户端连接索引。
	int32_t m_CurCnctSts; //存放当前连接状态，为[-m_MaxCnctTimes,0]表示等待远端接受连接。
	int32_t m_IsRqstDstoy; //存放是否请求销毁，为0表示未请求，为1表示已请求。

	int32_t m_MyTkbkIdx; //存放我的对讲索引。
	int32_t m_LclTkbkMode; //存放本端对讲模式。
	int32_t m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
	uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
	uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
	int32_t m_IsRecvExitPkt; //存放是否接收退出包，为0表示未接收，为1表示已接收。
} TkbkClnt;

int TkbkClntCnctInfoInit( TkbkClnt * TkbkClntPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
void TkbkClntCnctInfoDstoy( TkbkClnt * TkbkClntPt );
int TkbkClntCnctSendPkt( TkbkClnt * TkbkClntPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt );
int TkbkClntCnctSendTkbkModePkt( TkbkClnt * TkbkClntPt, int LclTkbkMode );
TkbkClnt::TkbkInfo * TkbkClntTkbkInfoInit( TkbkClnt * TkbkClntPt, int32_t TkbkIdx );
void TkbkClntTkbkInfoDstoy( TkbkClnt * TkbkClntPt, int32_t TkbkIdx );
void TkbkClntTkbkInfoAllDstoy( TkbkClnt * TkbkClntPt );
void TkbkClntCnctPocs( TkbkClnt * TkbkClntPt );

void TkbkClntUserReadAdoVdoInptFrm( TkbkClnt * TkbkClntPt,
									int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
									uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
									uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
									uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
									uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );
void TkbkClntUserWriteAdoOtptFrm( TkbkClnt * TkbkClntPt, int32_t AdoOtptStrmIdx,
								  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );
void TkbkClntUserGetAdoOtptFrm( TkbkClnt * TkbkClntPt, int32_t AdoOtptStrmIdx,
								int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );
void TkbkClntUserWriteVdoOtptFrm( TkbkClnt * TkbkClntPt, uint32_t VdoOtptStrmIdx,
								  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
								  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );
void TkbkClntUserGetVdoOtptFrm( TkbkClnt * TkbkClntPt, uint32_t VdoOtptStrmIdx,
								uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
								uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );

#ifdef __cplusplus
}
#endif
