#include "Func.h"

#ifndef __SOKT_H__
#define __SOKT_H__

//Sokt项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_SOKT__ ) //如果正在编译Sokt项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SOKT_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SOKT_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SOKT_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SOKT_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SOKT_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SOKT_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __SOKT_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __SOKT_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __SOKT_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __SOKT_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __SOKT_DLLAPI__
	#endif
#endif

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

__SOKT_DLLAPI__ int TcpClntSetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t SendBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t * SendBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t RecvBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufLen( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSetKeepAlive( TcpClntSokt * TcpClntSoktPt, int IsKeepAlive, int KeepIdle, int KeepIntvl, int KeepCnt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntGetKeepAlive( TcpClntSokt * TcpClntSoktPt, int * IsKeepAlivePt, int * KeepIdlePt, int * KeepIntvlPt, int * KeepCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int TcpClntSendPkt( TcpClntSokt * TcpClntSoktPt, const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int TcpClntRecvPkt( TcpClntSokt * TcpClntSoktPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

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

	int SetSendBufSz( size_t SendBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetSendBufSz( m_TcpClntSoktPt, SendBufSz, IsAutoLock, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetSendBufSz( m_TcpClntSoktPt, SendBufSzPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSz, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetRecvBufSz( m_TcpClntSoktPt, RecvBufSz, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufSz( m_TcpClntSoktPt, RecvBufSzPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetRecvBufLen( m_TcpClntSoktPt, RecvBufLenPt, IsAutoLock, ErrInfoVstrPt ); }

	int SetKeepAlive( int IsUseKeepAlive, int KeepIdle, int KeepIntvl, int KeepCnt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSetKeepAlive( m_TcpClntSoktPt, IsUseKeepAlive, KeepIdle, KeepIntvl, KeepCnt, IsAutoLock, ErrInfoVstrPt ); }
	int GetKeepAlive( int * IsUseKeepAlivePt, int * KeepIdlePt, int * KeepIntvlPt, int * KeepCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntGetKeepAlive( m_TcpClntSoktPt, IsUseKeepAlivePt, KeepIdlePt, KeepIntvlPt, KeepCntPt, IsAutoLock, ErrInfoVstrPt ); }

	int SendPkt( const void * DataBufPt, size_t DataBufLen, uint16_t TmotMsec, uint32_t Times, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntSendPkt( m_TcpClntSoktPt, DataBufPt, DataBufLen, TmotMsec, Times, IsAutoLock, ErrInfoVstrPt ); }
	int RecvPkt( void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return TcpClntRecvPkt( m_TcpClntSoktPt, DataBufPt, DataBufSz, DataBufLenPt, TmotMsec, IsAutoLock, ErrInfoVstrPt ); }

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

__SOKT_DLLAPI__ int AudpSetSendBufSz( AudpSokt * AudpSoktPt, size_t SendBufSz, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetSendBufSz( AudpSokt * AudpSoktPt, size_t * SendBufSzPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpSetRecvBufSz( AudpSokt * AudpSoktPt, size_t RecvBufSz, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetRecvBufSz( AudpSokt * AudpSoktPt, size_t * RecvBufSzPt, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpGetRecvBufLen( AudpSokt * AudpSoktPt, size_t * RecvBufLenPt, Vstr * ErrInfoVstrPt );

__SOKT_DLLAPI__ int AudpSendPkt( AudpSokt * AudpSoktPt, size_t CnctIdx, const void * DataBufPt, size_t DataBufLen, uint32_t Times, Vstr * ErrInfoVstrPt );
__SOKT_DLLAPI__ int AudpRecvPkt( AudpSokt * AudpSoktPt, size_t CnctIdx, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt );

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
	
	int SetSendBufSz( size_t SendBufSz, Vstr * ErrInfoVstrPt ) { return AudpSetSendBufSz( m_AudpSoktPt, SendBufSz, ErrInfoVstrPt ); }
	int GetSendBufSz( size_t * SendBufSzPt, Vstr * ErrInfoVstrPt ) { return AudpGetSendBufSz( m_AudpSoktPt, SendBufSzPt, ErrInfoVstrPt ); }

	int SetRecvBufSz( size_t RecvBufSz, Vstr * ErrInfoVstrPt ) { return AudpSetRecvBufSz( m_AudpSoktPt, RecvBufSz, ErrInfoVstrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufSz( m_AudpSoktPt, RecvBufSzPt, ErrInfoVstrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenPt, Vstr * ErrInfoVstrPt ) { return AudpGetRecvBufLen( m_AudpSoktPt, RecvBufLenPt, ErrInfoVstrPt ); }
	
	int SendPkt( size_t CnctIdx, const void * DataBufPt, size_t DataBufLen, uint32_t Times, Vstr * ErrInfoVstrPt ) { return AudpSendPkt( m_AudpSoktPt, CnctIdx, DataBufPt, DataBufLen, Times, ErrInfoVstrPt ); }
	int RecvPkt( size_t CnctIdx, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TmotMsec, Vstr * ErrInfoVstrPt ) { return AudpRecvPkt( m_AudpSoktPt, CnctIdx, DataBufPt, DataBufSz, DataBufLenPt, TmotMsec, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = AudpDstoy( m_AudpSoktPt, ErrInfoVstrPt ); m_AudpSoktPt = NULL; return p_Rslt; }
};
#endif

#endif
