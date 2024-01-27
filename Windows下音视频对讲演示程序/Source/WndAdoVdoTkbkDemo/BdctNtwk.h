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

typedef struct BdctNtwk //广播网络。
{
	NtwkMediaPocsThrd * m_NtwkMediaPocsThrdPt; //存放网络媒体处理线程的指针。
	
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
		int m_RmtTkbkMode; //存放远端对讲模式。
		
		int m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。
	} CnctInfo;
	CQueue * m_CnctInfoCntnrPt; //存放连接信息容器的指针。

	int m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
	uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
	uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
} BdctNtwk;

BdctNtwk::CnctInfo * BdctNtwkCnctInfoInit( BdctNtwk * BdctNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int CurCnctSts );
int BdctNtwkCnctInfoSendPkt( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt );
int BdctNtwkCnctInfoSendTkbkModePkt( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode );
void BdctNtwkCnctInfoDstoy( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt );
void BdctNtwkCnctInfoAllDstoy( BdctNtwk * BdctNtwkPt );
void BdctNtwkCnctInit( BdctNtwk * BdctNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
void BdctNtwkAllCnctSendAdoPkt( BdctNtwk * BdctNtwkPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab );
void BdctNtwkCnctDstoy( BdctNtwk * BdctNtwkPt, int CnctNum );
void BdctNtwkCnctPocs( BdctNtwk * BdctNtwkPt );
void BdctNtwkUserReadAdoVdoInptFrm( BdctNtwk * BdctNtwkPt,
									short AdoInptPcmSrcFrmPt[], short AdoInptPcmRsltFrmPt[], long AdoInptPcmFrmLenUnit, int AdoInptPcmRsltFrmVoiceActSts,
									byte AdoInptEncdRsltFrmPt[], long AdoInptEncdRsltFrmLenByt, int AdoInptEncdRsltFrmIsNeedTrans,
									byte VdoInptNv21SrcFrmPt[], int VdoInptNv21SrcFrmWidthPt, int VdoInptNv21SrcFrmHeightPt, long VdoInptNv21SrcFrmLenByt,
									byte VdoInptYu12RsltFrmPt[], int VdoInptYu12RsltFrmWidth, int VdoInptYu12RsltFrmHeight, long VdoInptYu12RsltFrmLenByt,
									byte VdoInptEncdRsltFrmPt[], long VdoInptEncdRsltFrmLenByt );

#ifdef __cplusplus
}
#endif
