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

typedef struct BdctClnt //广播客户端。
{
	ClntMediaPocsThrd * m_ClntMediaPocsThrdPt; //存放客户端媒体处理线程的指针。
	
	int32_t m_IsInit; //存放是否初始化广播客户端，为0表示未初始化，为非0表示已初始化。
	int32_t m_CnctNumIsDecr; //存放连接序号是否递减，为0表示不递减，为非0表示要递减。

	typedef struct CnctInfo //连接信息。
	{
		size_t m_Idx; //存放索引，从0开始，连接信息容器的唯一索引，连接中途不会改变。
        int32_t m_IsInit; //存放连接信息是否初始化，为0表示未初始化，为非0表示已初始化。
        int32_t m_Num; //存放序号，从0开始，随着前面的连接销毁而递减。

		int32_t m_IsTcpOrAudpPrtcl; //存放是否是Tcp或Udp协议，为0表示Tcp协议，为1表示高级Udp协议。
		Vstr * m_RmtNodeNameVstrPt; //存放远端套接字绑定的远端节点名称动态字符串的指针。
		Vstr * m_RmtNodeSrvcVstrPt; //存放远端套接字绑定的远端节点服务动态字符串的指针。
		TcpClntSokt * m_TcpClntSoktPt; //存放本端Tcp协议客户端套接字的指针。
		size_t m_AudpClntCnctIdx; //存放本端高级Udp协议客户端连接索引。
		int32_t m_CurCnctSts; //存放当前连接状态，为[-m_MaxCnctTimes,0]表示等待远端接受连接。
		int32_t m_IsRqstDstoy; //存放是否请求销毁，为0表示未请求，为1表示已请求。
		
		int32_t m_MyTkbkIdx; //存放我的对讲索引。
		int32_t m_IsRecvExitPkt; //存放是否接收退出包，为0表示未接收，为1表示已接收。
	} CnctInfo;
	CQueue * m_CnctInfoCntnrPt; //存放连接信息容器的指针。
    int m_CnctInfoCurMaxNum; //存放连接信息的当前最大序号。

	int m_LastSendAdoInptFrmIsAct; //存放最后发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
	uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后发送音频输入帧的时间戳。
} BdctClnt;

int BdctClntInit( BdctClnt * BdctClntPt, int32_t CnctNumIsDecr );
void BdctClntDstoy( BdctClnt * BdctClntPt );
BdctClnt::CnctInfo * BdctClntCnctInfoInit( BdctClnt * BdctClntPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
void BdctClntCnctInfoDstoy( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt );
int BdctClntCnctInfoSendPkt( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt );
int BdctClntCnctInfoSendTkbkModePkt( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt, int LclTkbkMode );
void BdctClntAllCnctSendAdoPkt( BdctClnt * BdctClntPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab );
void BdctClntCnctPocs( BdctClnt * BdctClntPt );
void BdctClntUserReadAdoVdoInptFrm( BdctClnt * BdctClntPt,
									short AdoInptPcmSrcFrmPt[], short AdoInptPcmRsltFrmPt[], long AdoInptPcmFrmLenUnit, int AdoInptPcmRsltFrmVoiceActSts,
									byte AdoInptEncdRsltFrmPt[], long AdoInptEncdRsltFrmLenByt, int AdoInptEncdRsltFrmIsNeedTrans,
									byte VdoInptNv21SrcFrmPt[], int VdoInptNv21SrcFrmWidthPt, int VdoInptNv21SrcFrmHeightPt, long VdoInptNv21SrcFrmLenByt,
									byte VdoInptYu12RsltFrmPt[], int VdoInptYu12RsltFrmWidth, int VdoInptYu12RsltFrmHeight, long VdoInptYu12RsltFrmLenByt,
									byte VdoInptEncdRsltFrmPt[], long VdoInptEncdRsltFrmLenByt );

#ifdef __cplusplus
}
#endif
