#include "Sokt.h"

#ifndef __AUDPSOKT_H__
#define __AUDPSOKT_H__

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

__SOKT_DLLAPI__ int AudpInit( AudpSokt * * AudpSoktPtPt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t NewCnctMaxWaitCnt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

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

__SOKT_DLLAPI__ int AudpSendApkt( AudpSokt * AudpSoktPt, size_t CnctIdx, const void * PktPt, size_t PktLenByt, uint32_t Times, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpRecvApkt( AudpSokt * AudpSoktPt, size_t CnctIdx, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpDstoy( AudpSokt * AudpSoktPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class AudpSoktCls
{
public:
	AudpSokt * m_AudpSoktPt;

	AudpSoktCls() { m_AudpSoktPt = NULL; }
	~AudpSoktCls() { Dstoy( NULL ); }

	int Init( const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t NewCnctMaxWaitCnt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpInit( &m_AudpSoktPt, LclNodeAddrFmly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, NewCnctMaxWaitCnt, TmotMsec, ErrInfoVstrPt ); }

	int Acpt( size_t * CnctIdxPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpAcpt( m_AudpSoktPt, CnctIdxPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, TmotMsec, ErrInfoVstrPt ); }
	
	int Cnct( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, size_t * CnctIdxPt, Vstr * ErrInfoVstrPt ) { return AudpCnct( m_AudpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, CnctIdxPt, ErrInfoVstrPt ); }
	int WaitCnct( size_t CnctIdx, uint16_t TmotMsec, AudpCnctSts * CnctStsPt, Vstr * ErrInfoVstrPt ) { return AudpWaitCnct( m_AudpSoktPt, CnctIdx, TmotMsec, CnctStsPt, ErrInfoVstrPt ); }
	int ClosCnct( size_t CnctIdx, Vstr * ErrInfoVstrPt ) { return AudpClosCnct( m_AudpSoktPt, CnctIdx, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, Vstr * ErrInfoVstrPt ) { return AudpGetLclAddr( m_AudpSoktPt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, ErrInfoVstrPt ); }
	int GetRmtAddr( size_t CnctIdx, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, Vstr * ErrInfoVstrPt ) { return AudpGetRmtAddr( m_AudpSoktPt, CnctIdx, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, ErrInfoVstrPt ); }
	
	int SetSendBufSz( size_t SendBufSzByt, Vstr * ErrInfoVstrPt ) { return AudpSetSendBufSz( m_AudpSoktPt, SendBufSzByt, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetSendBufSz( m_AudpSoktPt, SendBufSzBytPt, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSzByt, Vstr * ErrInfoVstrPt ) { return AudpSetRecvBufSz( m_AudpSoktPt, RecvBufSzByt, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufSz( m_AudpSoktPt, RecvBufSzBytPt, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenBytPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufLen( m_AudpSoktPt, RecvBufLenBytPt, ErrInfoVstrPt ); }
	
	int SendApkt( size_t CnctIdx, const void * PktPt, size_t PktLenByt, uint32_t Times, Vstr * ErrInfoVstrPt ) { return AudpSendApkt( m_AudpSoktPt, CnctIdx, PktPt, PktLenByt, Times, ErrInfoVstrPt ); }
	int RecvApkt( size_t CnctIdx, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpRecvApkt( m_AudpSoktPt, CnctIdx, PktPt, PktSzByt, PktLenBytPt, TmotMsec, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = AudpDstoy( m_AudpSoktPt, ErrInfoVstrPt ); m_AudpSoktPt = NULL; return p_Rslt; }
};
#endif

#endif
