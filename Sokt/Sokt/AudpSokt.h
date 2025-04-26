#pragma once

#include "Sokt.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AudpSokt AudpSokt;
enum AudpCnctSts
{
	AudpCnctStsWait, //连接状态：等待远端接受连接。
	AudpCnctStsCnct, //连接状态：已连接。
	AudpCnctStsTmot, //连接状态：超时未接收到任何数据包。异常断开。
	AudpCnctStsDsct  //连接状态：已断开。本端或远端正常断开。
};

__SOKT_DLLAPI__ int AudpGetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpInit( const void * LicnCodePt, AudpSokt * * AudpSoktPtPt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t NewCnctMaxWaitCnt, uint16_t NtwkTmotMsec, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpDstoy( AudpSokt * AudpSoktPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpAcpt( AudpSokt * AudpSoktPt, size_t * CnctIdxPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpCnct( AudpSokt * AudpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, size_t * CnctIdxPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpWaitCnct( AudpSokt * AudpSoktPt, size_t CnctIdx, uint16_t TmotMsec, AudpCnctSts * CnctStsPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpClosCnct( AudpSokt * AudpSoktPt, size_t CnctIdx, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpGetLclAddr( AudpSokt * AudpSoktPt, int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetRmtAddr( AudpSokt * AudpSoktPt, size_t CnctIdx, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpSetSendBufSz( AudpSokt * AudpSoktPt, size_t SendBufSzByt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetSendBufSz( AudpSokt * AudpSoktPt, size_t * SendBufSzBytPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpSetRecvBufSz( AudpSokt * AudpSoktPt, size_t RecvBufSzByt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetRecvBufSz( AudpSokt * AudpSoktPt, size_t * RecvBufSzBytPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetRecvBufLen( AudpSokt * AudpSoktPt, size_t * RecvBufLenBytPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpSendApkt( AudpSokt * AudpSoktPt, size_t CnctIdx, const void * PktPt, size_t PktLenByt, uint32_t Times, int32_t IsRlab, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpRecvApkt( AudpSokt * AudpSoktPt, size_t CnctIdx, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, int32_t * IsRlabPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class AudpSoktCls
{
public:
	AudpSokt * m_Pt;

	AudpSoktCls() { m_Pt = NULL; }
	~AudpSoktCls() { Dstoy( NULL ); }

	static int GetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt ) { return AudpGetAppLmtInfo( LicnCodePt, LmtTimeSecPt, RmnTimeSecPt, ErrInfoVstrPt ); }

	int Init( const void * LicnCodePt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t NewCnctMaxWaitCnt, uint16_t NtwkTmotMsec, Vstr * ErrInfoVstrPt ) { return AudpInit( LicnCodePt, &m_Pt, LclNodeAddrFmly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, NewCnctMaxWaitCnt, NtwkTmotMsec, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = AudpDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }

	int Acpt( size_t * CnctIdxPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpAcpt( m_Pt, CnctIdxPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, TmotMsec, ErrInfoVstrPt ); }
	
	int Cnct( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, size_t * CnctIdxPt, Vstr * ErrInfoVstrPt ) { return AudpCnct( m_Pt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, CnctIdxPt, ErrInfoVstrPt ); }
	int WaitCnct( size_t CnctIdx, uint16_t TmotMsec, AudpCnctSts * CnctStsPt, Vstr * ErrInfoVstrPt ) { return AudpWaitCnct( m_Pt, CnctIdx, TmotMsec, CnctStsPt, ErrInfoVstrPt ); }
	int ClosCnct( size_t CnctIdx, Vstr * ErrInfoVstrPt ) { return AudpClosCnct( m_Pt, CnctIdx, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, Vstr * ErrInfoVstrPt ) { return AudpGetLclAddr( m_Pt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, ErrInfoVstrPt ); }
	int GetRmtAddr( size_t CnctIdx, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, Vstr * ErrInfoVstrPt ) { return AudpGetRmtAddr( m_Pt, CnctIdx, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, ErrInfoVstrPt ); }
	
	int SetSendBufSz( size_t SendBufSzByt, Vstr * ErrInfoVstrPt ) { return AudpSetSendBufSz( m_Pt, SendBufSzByt, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetSendBufSz( m_Pt, SendBufSzBytPt, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSzByt, Vstr * ErrInfoVstrPt ) { return AudpSetRecvBufSz( m_Pt, RecvBufSzByt, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufSz( m_Pt, RecvBufSzBytPt, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufLen( m_Pt, RecvBufLenBytPt, ErrInfoVstrPt ); }
	
	int SendApkt( size_t CnctIdx, const void * PktPt, size_t PktLenByt, uint32_t Times, int32_t IsRlab, Vstr * ErrInfoVstrPt ) { return AudpSendApkt( m_Pt, CnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ); }
	int RecvApkt( size_t CnctIdx, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, int32_t * IsRlabPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpRecvApkt( m_Pt, CnctIdx, PktPt, PktSzByt, PktLenBytPt, IsRlabPt, TmotMsec, ErrInfoVstrPt ); }
};
#endif
