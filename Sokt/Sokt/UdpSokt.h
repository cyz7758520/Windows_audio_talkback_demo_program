#pragma once

#include "Sokt.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct UdpSokt UdpSokt;

__SOKT_DLLAPI__ int UdpInit( UdpSokt * * UdpSoktPtPt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpLocked( UdpSokt * UdpSoktPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpUnlock( UdpSokt * UdpSoktPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpCnct( UdpSokt * UdpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpDsct( UdpSokt * UdpSoktPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpGetLclAddr( UdpSokt * UdpSoktPt, int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetRmtAddr( UdpSokt * UdpSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSetSendBufSz( UdpSokt * UdpSoktPt, size_t SendBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetSendBufSz( UdpSokt * UdpSoktPt, size_t * SendBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSetRecvBufSz( UdpSokt * UdpSoktPt, size_t RecvBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufSz( UdpSokt * UdpSoktPt, size_t * RecvBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufLen( UdpSokt * UdpSoktPt, size_t * RecvBufLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSendPkt( UdpSokt * UdpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpRecvPkt( UdpSokt * UdpSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSendApkt( UdpSokt * UdpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpRecvApkt( UdpSokt * UdpSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpDstoy( UdpSokt * UdpSoktPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class UdpSoktCls
{
public:
	UdpSokt * m_UdpSoktPt;

	UdpSoktCls() { m_UdpSoktPt = NULL; }
	~UdpSoktCls() { Dstoy( NULL ); }

	int Init( const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, Vstr * ErrInfoVstrPt ) { return UdpInit( &m_UdpSoktPt, LclNodeAddrFmly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, ErrInfoVstrPt ); }

	int Locked( Vstr * ErrInfoVstrPt ) { return UdpLocked( m_UdpSoktPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return UdpUnlock( m_UdpSoktPt, ErrInfoVstrPt ); }
	
	int Cnct( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpCnct( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	int Dsct( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpDsct( m_UdpSoktPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetLclAddr( m_UdpSoktPt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRmtAddr( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetRmtAddr( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int SetSendBufSz( size_t SendBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSetSendBufSz( m_UdpSoktPt, SendBufSzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetSendBufSz( m_UdpSoktPt, SendBufSzBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSetRecvBufSz( m_UdpSoktPt, RecvBufSzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetRecvBufSz( m_UdpSoktPt, RecvBufSzBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetRecvBufLen( m_UdpSoktPt, RecvBufLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int SendPkt( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSendPkt( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, PktPt, PktLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvPkt( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpRecvPkt( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, PktPt, PktSzByt, PktLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int SendApkt( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSendApkt( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, PktPt, PktLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvApkt( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpRecvApkt( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, PktPt, PktSzByt, PktLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = UdpDstoy( m_UdpSoktPt, ErrInfoVstrPt ); m_UdpSoktPt = NULL; return p_Rslt; }
};
#endif
