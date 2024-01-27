#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct NtwkMediaPocsThrd NtwkMediaPocsThrd;

typedef struct TkbkNtwk //对讲网络。
{
	NtwkMediaPocsThrd * m_NtwkMediaPocsThrdPt; //存放网络媒体处理线程的指针。
	
    int m_SrvrIsInit; //存放服务端是否初始化，为0表示未初始化，为1表示已初始化。
    TcpSrvrSokt * m_TcpSrvrSoktPt; //存放本端Tcp协议服务端套接字。
    AudpSokt * m_AudpSrvrSoktPt; //存放本端高级Udp协议服务端套接字。
    int m_IsAutoActCnct; //存放是否自动激活连接，为0表示手动，为1表示自动。

    int m_XfrMode; //存放传输模式，为0表示实时半双工（一键通），为1表示实时全双工。
    int m_PttBtnIsDown; //存放一键即按即通按钮是否按下，为0表示弹起，为非0表示按下。

	typedef struct CnctInfo //连接信息。
	{
		int m_Num; //存放索引，从0开始。

		int m_IsSrvrOrClntCnct; //存放是否是服务端或客户端的连接，为0表示服务端，为1表示客户端。
		int m_IsTcpOrAudpPrtcl; //存放是否是Tcp或Udp协议，为0表示Tcp协议，为1表示高级Udp协议。
		Vstr * m_RmtNodeNameVstrPt; //存放远端套接字绑定的远端节点名称动态字符串的指针。
		Vstr * m_RmtNodeSrvcVstrPt; //存放远端套接字绑定的远端节点服务动态字符串的指针。
		TcpClntSokt * m_TcpClntSoktPt; //存放本端Tcp协议客户端套接字的指针。
		size_t m_AudpClntCnctIdx; //存放本端高级Udp协议客户端连接索引。
		int m_IsRqstDstoy; //存放是否请求销毁，为0表示未请求，为1表示已请求。
		
		int m_CurCnctSts; //存放当前连接状态，为[-m_MaxCnctTimes,0]表示等待远端接受连接。
		int m_LclTkbkMode; //存放本端对讲模式。
		int m_RmtTkbkMode; //存放远端对讲模式。
		
		int m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
		uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
		uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
		int m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。
	} CnctInfo;
	CQueue * m_CnctInfoCntnrPt; //存放连接信息容器的指针。
	CnctInfo * m_CurActCnctInfoPt; //存放当前激活的连接信息的指针。
	int m_MaxCnctNum; //存放最大连接数。

    int m_UseWhatRecvOtptFrm; //存放使用什么接收输出帧，为0表示容器，为1表示自适应抖动缓冲器。
    int m_RecvOtptFrmIsInit; //存放接收输出帧是否初始化，为0表示未初始化，为1表示已初始化。

    VQueue * m_RecvAdoOtptFrmCntnrPt; //存放接收音频输出帧容器的指针。
	VQueue * m_RecvVdoOtptFrmCntnrPt; //存放接收视频输出帧容器的指针。

	struct //存放音频自适应抖动缓冲器。
	{
		AAjb * m_Pt; //存放指针。
		int m_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]。
		int m_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]，必须大于等于最小需缓冲帧的数量。
		int m_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量，单位为个帧，取值区间为[1,2147483647]，当连续丢失帧的数量超过最大时，认为是对方中途暂停发送。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大缓冲帧越多，取值区间为[0.0,127.0]。
	} m_AAjb;
	struct //存放视频自适应抖动缓冲器。
	{
		VAjb * m_Pt; //存放指针。
		int m_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量，单位为个帧，取值区间为[0,2147483647]。
		int m_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量，单位为个帧，取值区间为[1,2147483647]，必须大于等于最小需缓冲帧数量。
		float m_AdaptSensitivity; //存放自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	} m_VAjb;
} TkbkNtwk;

TkbkNtwk::CnctInfo * TkbkNtwkCnctInfoInit( TkbkNtwk * TkbkNtwkPt, int IsSrvrOrClntCnct, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int CurCnctSts );
int TkbkNtwkCnctInfoSendPkt( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt );
int TkbkNtwkCnctInfoSendTkbkModePkt( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode );
void TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode );
void TkbkNtwkCnctInfoAllUpdtLclTkbkMode( TkbkNtwk * TkbkNtwkPt, int LclTkbkMode );
void TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt );
void TkbkNtwkCnctInfoAllUpdtRmtTkbkMode( TkbkNtwk * TkbkNtwkPt );
void TkbkNtwkCnctInfoAct( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt );
void TkbkNtwkCnctInfoPend( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt );
void TkbkNtwkCnctInfoDstoy( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt );
void TkbkNtwkCnctInfoAllDstoy( TkbkNtwk * TkbkNtwkPt );

int TkbkNtwkSrvrInit( TkbkNtwk * TkbkNtwkPt, Vstr * SrvrUrlVstrPt );
void TkbkNtwkSrvrDstoy( TkbkNtwk * TkbkNtwkPt );
void TkbkNtwkCnctInit( TkbkNtwk * TkbkNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
void TkbkNtwkCnctAct( TkbkNtwk * TkbkNtwkPt, int CnctNum );
void TkbkNtwkCnctDstoy( TkbkNtwk * TkbkNtwkPt, int CnctNum );
int TkbkNtwkRecvOtptFrmInit( TkbkNtwk * TkbkNtwkPt );
void TkbkNtwkRecvOtptFrmReset( TkbkNtwk * TkbkNtwkPt );
void TkbkNtwkRecvOtptFrmDstoy( TkbkNtwk * TkbkNtwkPt );

void TkbkNtwkCnctPocs( TkbkNtwk * TkbkNtwkPt );

void TkbkNtwkUserReadAdoVdoInptFrm( TkbkNtwk * TkbkNtwkPt,
									int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
									uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
									uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
									uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
									uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );
void TkbkNtwkUserWriteAdoOtptFrm( TkbkNtwk * TkbkNtwkPt, int32_t AdoOtptStrmIdx,
								  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );
void TkbkNtwkUserGetAdoOtptFrm( TkbkNtwk * TkbkNtwkPt, int32_t AdoOtptStrmIdx,
								int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );
void TkbkNtwkUserWriteVdoOtptFrm( TkbkNtwk * TkbkNtwkPt, uint32_t VdoOtptStrmIdx,
								  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
								  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );
void TkbkNtwkUserGetVdoOtptFrm( TkbkNtwk * TkbkNtwkPt, uint32_t VdoOtptStrmIdx,
								uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
								uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );

#ifdef __cplusplus
}
#endif
