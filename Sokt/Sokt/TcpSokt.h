#include "Sokt.h"

#ifndef __TCPSOKT_H__
#define __TCPSOKT_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TcpSrvrSokt TcpSrvrSokt;
typedef struct TcpClntSokt TcpClntSokt;

__SOKT_DLLAPI__ int TcpSrvrInit( TcpSrvrSokt * * TcpSrvrSoktPtPt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, int MaxWait, int IsReuseAddr, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrLocked( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpSrvrUnlock( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrGetLclAddr( TcpSrvrSokt * TcpSrvrSoktPt, int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrAcpt( TcpSrvrSokt * TcpSrvrSoktPt, TcpClntSokt * * TcpClntSoktPtPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrDstoy( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );


__SOKT_DLLAPI__ int TcpClntInit( TcpClntSokt * * TcpClntSoktPtPt, const int RmtLclNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntLocked( TcpClntSokt * TcpClntSoktPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntUnlock( TcpClntSokt * TcpClntSoktPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntGetLclAddr( TcpClntSokt * TcpClntSoktPt, int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetRmtAddr( TcpClntSokt * TcpClntSoktPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetNoDelay( TcpClntSokt * TcpClntSoktPt, int IsNoDelay, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetNoDelay( TcpClntSokt * TcpClntSoktPt, int * IsNoDelayPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t SendBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t * SendBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t RecvBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufLen( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetKeepAlive( TcpClntSokt * TcpClntSoktPt, int IsKeepAlive, int KeepIdle, int KeepIntvl, int KeepCnt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetKeepAlive( TcpClntSokt * TcpClntSoktPt, int * IsKeepAlivePt, int * KeepIdlePt, int * KeepIntvlPt, int * KeepCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSendPkt( TcpClntSokt * TcpClntSoktPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntRecvPkt( TcpClntSokt * TcpClntSoktPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSendApkt( TcpClntSokt * TcpClntSoktPt, const void * PktPt, size_t PktLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntRecvApkt( TcpClntSokt * TcpClntSoktPt, void * PktPt, size_t PktSzByt, size_t * PktLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntDstoy( TcpClntSokt * TcpClntSoktPt, uint16_t TmotSec, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//因为TcpSrvrSoktCls类中要使用TcpClntSoktCls类中的m_TcpClntSoktPt成员变量，所以要将TcpSrvrSoktCls类定义在TcpClntSoktCls类之后，否则会报“使用了未定义类型”。
class TcpClntSoktCls
{
public:
	TcpClntSokt * m_TcpClntSoktPt;

	TcpClntSoktCls() { m_TcpClntSoktPt = NULL; }
	~TcpClntSoktCls() { Dstoy( UINT16_MAX, NULL ); }

	int Init( const int RmtLclNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return TcpClntInit( &m_TcpClntSoktPt, RmtLclNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, LclNodeNameVstrPt, LclNodeSrvcVstrPt, TmotMsec, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return TcpClntLocked( m_TcpClntSoktPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return TcpClntUnlock( m_TcpClntSoktPt, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetLclAddr( m_TcpClntSoktPt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRmtAddr( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRmtAddr( m_TcpClntSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int SetNoDelay( int IsNoDelay, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetNoDelay( m_TcpClntSoktPt, IsNoDelay, IsAutoLock, ErrInfoVstrPt ); }
	int GetNoDelay( int * IsNoDelayPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetNoDelay( m_TcpClntSoktPt, IsNoDelayPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetSendBufSz( size_t SendBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetSendBufSz( m_TcpClntSoktPt, SendBufSzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetSendBufSz( m_TcpClntSoktPt, SendBufSzBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufBytSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetRecvBufSz( m_TcpClntSoktPt, RecvBufBytSz, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufBytSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufSz( m_TcpClntSoktPt, RecvBufBytSzPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufBytLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufLen( m_TcpClntSoktPt, RecvBufBytLenPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetKeepAlive( int IsUseKeepAlive, int KeepIdle, int KeepIntvl, int KeepCnt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetKeepAlive( m_TcpClntSoktPt, IsUseKeepAlive, KeepIdle, KeepIntvl, KeepCnt, IsAutoLock, ErrInfoVstrPt ); }
	int GetKeepAlive( int * IsUseKeepAlivePt, int * KeepIdlePt, int * KeepIntvlPt, int * KeepCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetKeepAlive( m_TcpClntSoktPt, IsUseKeepAlivePt, KeepIdlePt, KeepIntvlPt, KeepCntPt, IsAutoLock, ErrInfoVstrPt ); }

	int SendPkt( const void * DataPt, size_t DataLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSendPkt( m_TcpClntSoktPt, DataPt, DataLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvPkt( void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntRecvPkt( m_TcpClntSoktPt, DataPt, DataSzByt, DataLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }
	
	int SendApkt( const void * DataPt, size_t DataLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSendApkt( m_TcpClntSoktPt, DataPt, DataLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvApkt( void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntRecvApkt( m_TcpClntSoktPt, DataPt, DataSzByt, DataLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( uint16_t TmotSec, Vstr * ErrInfoVstrPt ) { int p_Rslt = TcpClntDstoy( m_TcpClntSoktPt, TmotSec, ErrInfoVstrPt ); m_TcpClntSoktPt = NULL; return p_Rslt; }
};

class TcpSrvrSoktCls
{
public:
	TcpSrvrSokt * m_TcpSrvrSoktPt;

	TcpSrvrSoktCls() { m_TcpSrvrSoktPt = NULL; }
	~TcpSrvrSoktCls() { Dstoy( NULL ); }

	int Init( const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, int MaxWait, int IsReuseAddr, Vstr * ErrInfoVstrPt ) { return TcpSrvrInit( &m_TcpSrvrSoktPt, LclNodeAddrFmly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, MaxWait, IsReuseAddr, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return TcpSrvrLocked( m_TcpSrvrSoktPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return TcpSrvrUnlock( m_TcpSrvrSoktPt, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpSrvrGetLclAddr( m_TcpSrvrSoktPt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int Acpt( TcpClntSoktCls * TcpClntSoktClsPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpSrvrAcpt( m_TcpSrvrSoktPt, &TcpClntSoktClsPt->m_TcpClntSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }
	
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = TcpSrvrDstoy( m_TcpSrvrSoktPt, ErrInfoVstrPt ); m_TcpSrvrSoktPt = NULL; return p_Rslt; }
};
#endif

#endif
