#pragma once

#include "Sokt.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TcpSrvrSokt TcpSrvrSokt;
typedef struct TcpClntSokt TcpClntSokt;
enum TcpCnctSts
{
	TcpCnctStsWait, //连接状态：等待远端接受连接。
	TcpCnctStsCnct, //连接状态：已连接。
	TcpCnctStsFail, //连接状态：连接失败。
};

__SOKT_DLLAPI__ int TcpSrvrInit( TcpSrvrSokt * * TcpSrvrSoktPtPt, const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, int MaxWait, int IsReuseAddr, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrLocked( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpSrvrUnlock( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrGetLclAddr( TcpSrvrSokt * TcpSrvrSoktPt, int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrAcpt( TcpSrvrSokt * TcpSrvrSoktPt, TcpClntSokt * * TcpClntSoktPtPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpSrvrDstoy( TcpSrvrSokt * TcpSrvrSoktPt, Vstr * ErrInfoVstrPt );


__SOKT_DLLAPI__ int TcpClntInit( TcpClntSokt * * TcpClntSoktPtPt, const int RmtLclNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntWaitCnct( TcpClntSokt * TcpClntSoktPt, uint16_t TmotMsec, TcpCnctSts * CnctStsPt, Vstr * ErrInfoVstrPt );

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
//因为TcpSrvrSoktCls类中要使用TcpClntSoktCls类中的m_Pt成员变量，所以要将TcpSrvrSoktCls类定义在TcpClntSoktCls类之后，否则会报“使用了未定义类型”。
class TcpClntSoktCls
{
public:
	TcpClntSokt * m_Pt;

	TcpClntSoktCls() { m_Pt = NULL; }
	~TcpClntSoktCls() { Dstoy( UINT16_MAX, NULL ); }

	int Init( const int RmtLclNodeAddrFmly, const Vstr * RmtNodeNameVstrPt, const Vstr * RmtNodeSrvcVstrPt, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, Vstr * ErrInfoVstrPt ) { return TcpClntInit( &m_Pt, RmtLclNodeAddrFmly, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, LclNodeNameVstrPt, LclNodeSrvcVstrPt, ErrInfoVstrPt ); }
	
	int WaitCnct( uint16_t TmotMsec, TcpCnctSts * CnctStsPt, Vstr * ErrInfoVstrPt ) { return TcpClntWaitCnct( m_Pt, TmotMsec, CnctStsPt, ErrInfoVstrPt ); }

	int Locked( Vstr * ErrInfoVstrPt ) { return TcpClntLocked( m_Pt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return TcpClntUnlock( m_Pt, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetLclAddr( m_Pt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRmtAddr( int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRmtAddr( m_Pt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int SetNoDelay( int IsNoDelay, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetNoDelay( m_Pt, IsNoDelay, IsAutoLock, ErrInfoVstrPt ); }
	int GetNoDelay( int * IsNoDelayPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetNoDelay( m_Pt, IsNoDelayPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetSendBufSz( size_t SendBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetSendBufSz( m_Pt, SendBufSzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetSendBufSz( m_Pt, SendBufSzBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetRecvBufSz( m_Pt, RecvBufSzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufSz( m_Pt, RecvBufSzBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufLen( m_Pt, RecvBufLenBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetKeepAlive( int IsUseKeepAlive, int KeepIdle, int KeepIntvl, int KeepCnt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetKeepAlive( m_Pt, IsUseKeepAlive, KeepIdle, KeepIntvl, KeepCnt, IsAutoLock, ErrInfoVstrPt ); }
	int GetKeepAlive( int * IsUseKeepAlivePt, int * KeepIdlePt, int * KeepIntvlPt, int * KeepCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetKeepAlive( m_Pt, IsUseKeepAlivePt, KeepIdlePt, KeepIntvlPt, KeepCntPt, IsAutoLock, ErrInfoVstrPt ); }

	int SendPkt( const void * DataPt, size_t DataLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSendPkt( m_Pt, DataPt, DataLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvPkt( void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntRecvPkt( m_Pt, DataPt, DataSzByt, DataLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }
	
	int SendApkt( const void * DataPt, size_t DataLenByt, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSendApkt( m_Pt, DataPt, DataLenByt, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvApkt( void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntRecvApkt( m_Pt, DataPt, DataSzByt, DataLenBytPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( uint16_t TmotSec, Vstr * ErrInfoVstrPt ) { int p_Rslt = TcpClntDstoy( m_Pt, TmotSec, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
};

class TcpSrvrSoktCls
{
public:
	TcpSrvrSokt * m_Pt;

	TcpSrvrSoktCls() { m_Pt = NULL; }
	~TcpSrvrSoktCls() { Dstoy( NULL ); }

	int Init( const int32_t LclNodeAddrFmly, const Vstr * LclNodeNameVstrPt, const Vstr * LclNodeSrvcVstrPt, int MaxWait, int IsReuseAddr, Vstr * ErrInfoVstrPt ) { return TcpSrvrInit( &m_Pt, LclNodeAddrFmly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, MaxWait, IsReuseAddr, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return TcpSrvrLocked( m_Pt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return TcpSrvrUnlock( m_Pt, ErrInfoVstrPt ); }

	int GetLclAddr( int32_t * LclNodeAddrFmlyPt, Vstr * LclNodeAddrVstrPt, Vstr * LclNodePortVstrPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpSrvrGetLclAddr( m_Pt, LclNodeAddrFmlyPt, LclNodeAddrVstrPt, LclNodePortVstrPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int Acpt( TcpClntSoktCls * TcpClntSoktClsPt, int32_t * RmtNodeAddrFmlyPt, Vstr * RmtNodeAddrVstrPt, Vstr * RmtNodePortVstrPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpSrvrAcpt( m_Pt, &TcpClntSoktClsPt->m_Pt, RmtNodeAddrFmlyPt, RmtNodeAddrVstrPt, RmtNodePortVstrPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }
	
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = TcpSrvrDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
};
#endif
