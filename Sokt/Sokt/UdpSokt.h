#include "Sokt.h"

#ifndef __UDPSOKT_H__
#define __UDPSOKT_H__

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

__SOKT_DLLAPI__ int UdpSetSendBufSz( UdpSokt * UdpSoktPt, size_t SendBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetSendBufSz( UdpSokt * UdpSoktPt, size_t * SendBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSetRecvBufSz( UdpSokt * UdpSoktPt, size_t RecvBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufSz( UdpSokt * UdpSoktPt, size_t * RecvBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufLen( UdpSokt * UdpSoktPt, size_t * RecvBufLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSend( UdpSokt * UdpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpRecv( UdpSokt * UdpSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int UdpSendPkt( UdpSokt * UdpSoktPt, const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int UdpRecvPkt( UdpSokt * UdpSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

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
	
	int SetSendBufSz( size_t SendBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSetSendBufSz( m_UdpSoktPt, SendBufSz, IsAutoLock, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetSendBufSz( m_UdpSoktPt, SendBufSzPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSetRecvBufSz( m_UdpSoktPt, RecvBufSz, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetRecvBufSz( m_UdpSoktPt, RecvBufSzPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpGetRecvBufLen( m_UdpSoktPt, RecvBufLenPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int Send( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSend( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, DataBufPt, DataBufLen, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int Recv( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpRecv( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, DataBufPt, DataBufSz, DataBufLenPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int SendPkt( const int32_t RmtNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpSendPkt( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, DataBufPt, DataBufLen, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvPkt( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return UdpRecvPkt( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, DataBufPt, DataBufSz, DataBufLenPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = UdpDstoy( m_UdpSoktPt, ErrInfoVstrPt ); m_UdpSoktPt = NULL; return p_Rslt; }
};
#endif

#endif
