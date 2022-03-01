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

__SOKT_DLLAPI__ int TcpSrvrInit( TcpSrvrSokt * * TcpSrvrSoktPtPt, int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, int MaxWait, int IsReuseAddr, VarStr * ErrInfoVarStrPt );
//__SOKT_DLLAPI__ int TcpSrvrInitByHdl( TcpSrvrSokt * * TcpSrvrSoktPtPt, SOCKET TcpSrvrSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrLock( TcpSrvrSokt * TcpSrvrSoktPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpSrvrUnlock( TcpSrvrSokt * TcpSrvrSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrGetLclAddr( TcpSrvrSokt * TcpSrvrSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrAccept( TcpSrvrSokt * TcpSrvrSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, uint16_t TimeOutMsec, TcpClntSokt * * TcpClntSoktPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrDstoy( TcpSrvrSokt * TcpSrvrSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntInit( TcpClntSokt * * TcpClntSoktPtPt, int RmtLclNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const char * LclNodeNamePt, const char * LclNodeSrvcPt, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntInitByHdl( TcpClntSokt * * TcpClntSoktPtPt, SOCKET TcpClntSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntLock( TcpClntSokt * TcpClntSoktPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntUnlock( TcpClntSokt * TcpClntSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntGetLclAddr( TcpClntSokt * TcpClntSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRmtAddr( TcpClntSokt * TcpClntSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetNoDelay( TcpClntSokt * TcpClntSoktPt, int IsNoDelay, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetNoDelay( TcpClntSokt * TcpClntSoktPt, int * IsNoDelayPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t SendBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t * SendBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t RecvBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufLen( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSendPkt( TcpClntSokt * TcpClntSoktPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntRecvPkt( TcpClntSokt * TcpClntSoktPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntDstoy( TcpClntSokt * TcpClntSoktPt, uint16_t TimeOutSec, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//因为TcpSrvrSoktClass类中要使用TcpClntSoktClass类中的m_TcpClntSoktPt成员变量，所以将TcpSrvrSoktClass类定义在TcpClntSoktClass类之后，否则会报“使用了未定义类型”。
class TcpClntSoktCls
{
public:
	TcpClntSokt * m_TcpClntSoktPt;

	TcpClntSoktCls() { m_TcpClntSoktPt = NULL; }
	~TcpClntSoktCls() { Dstoy( UINT16_MAX, NULL ); }

	int Init( int RmtLclNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const char * LclNodeNamePt, const char * LclNodeSrvcPt, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt ) { return TcpClntInit( &m_TcpClntSoktPt, RmtLclNodeAddrFmly, RmtNodeNamePt, RmtNodeSrvcPt, LclNodeNamePt, LclNodeSrvcPt, TimeOutMsec, ErrInfoVarStrPt ); }
	int InitByHdl( SOCKET TcpClntSoktHdl, VarStr * ErrInfoVarStrPt ) { return TcpClntInitByHdl( &m_TcpClntSoktPt, TcpClntSoktHdl, ErrInfoVarStrPt ); }

	int Lock( VarStr * ErrInfoVarStrPt ) { return TcpClntLock( m_TcpClntSoktPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return TcpClntLock( m_TcpClntSoktPt, ErrInfoVarStrPt ); }

	int GetLclAddr( int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetLclAddr( m_TcpClntSoktPt, LclNodeAddrFmlyPt, LclNodeAddrPt, LclNodePortPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRmtAddr( int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetRmtAddr( m_TcpClntSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrPt, RmtNodePortPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	
	int SetNoDelay( int IsNoDelay, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntSetNoDelay( m_TcpClntSoktPt, IsNoDelay, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetNoDelay( int * IsNoDelayPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetNoDelay( m_TcpClntSoktPt, IsNoDelayPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int SetSendBufSz( size_t SendBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntSetSendBufSz( m_TcpClntSoktPt, SendBufSz, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetSendBufSz( size_t * SendBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetSendBufSz( m_TcpClntSoktPt, SendBufSzPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int SetRecvBufSz( size_t RecvBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntSetRecvBufSz( m_TcpClntSoktPt, RecvBufSz, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetRecvBufSz( m_TcpClntSoktPt, RecvBufSzPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntGetRecvBufLen( m_TcpClntSoktPt, RecvBufLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int SendPkt( const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntSendPkt( m_TcpClntSoktPt, DataBufPt, DataBufLen, TimeOutMsec, Times, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int RecvPkt( void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpClntRecvPkt( m_TcpClntSoktPt, DataBufPt, DataBufSz, DataBufLenPt, TimeOutMsec, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Dstoy( uint16_t TimeOutSec, VarStr * ErrInfoVarStrPt ) { int p_Rslt = TcpClntDstoy( m_TcpClntSoktPt, TimeOutSec, ErrInfoVarStrPt ); m_TcpClntSoktPt = NULL; return p_Rslt; }
};

class TcpSrvrSoktCls
{
public:
	TcpSrvrSokt * m_TcpSrvrSoktPt;

	TcpSrvrSoktCls() { m_TcpSrvrSoktPt = NULL; }
	~TcpSrvrSoktCls() { Dstoy( NULL ); }

	int Init( int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, int MaxWait, int IsReuseAddr, VarStr * ErrInfoVarStrPt ) { return TcpSrvrInit( &m_TcpSrvrSoktPt, LclNodeAddrFmly, LclNodeNamePt, LclNodeSrvcPt, MaxWait, IsReuseAddr, ErrInfoVarStrPt ); }
	
	int Lock( VarStr * ErrInfoVarStrPt ) { return TcpSrvrLock( m_TcpSrvrSoktPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return TcpSrvrLock( m_TcpSrvrSoktPt, ErrInfoVarStrPt ); }

	int GetLclAddr( int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpSrvrGetLclAddr( m_TcpSrvrSoktPt, LclNodeAddrFmlyPt, LclNodeAddrPt, LclNodePortPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	
	int Accept( int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, uint16_t TimeOutMsec, TcpClntSoktCls * TcpClntSoktPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return TcpSrvrAccept( m_TcpSrvrSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrPt, RmtNodePortPt, TimeOutMsec, &TcpClntSoktPt->m_TcpClntSoktPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	
	int Dstoy( VarStr * ErrInfoVarStrPt ) { int p_Rslt = TcpSrvrDstoy( m_TcpSrvrSoktPt, ErrInfoVarStrPt ); m_TcpSrvrSoktPt = NULL; return p_Rslt; }
};
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct UdpSokt UdpSokt;

__SOKT_DLLAPI__ int UdpInit( UdpSokt * * UdpSoktPtPt, int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, VarStr * ErrInfoVarStrPt );
//__SOKT_DLLAPI__ int UdpInitByHdl( UdpSokt * * UdpSoktPtPt, SOCKET UdpSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpLock( UdpSokt * UdpSoktPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpUnlock( UdpSokt * UdpSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpConnect( UdpSokt * UdpSoktPt, int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpDisconnect( UdpSokt * UdpSoktPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpGetLclAddr( UdpSokt * UdpSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRmtAddr( UdpSokt * UdpSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSetSendBufSz( UdpSokt * UdpSoktPt, size_t SendBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetSendBufSz( UdpSokt * UdpSoktPt, size_t * SendBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSetRecvBufSz( UdpSokt * UdpSoktPt, size_t RecvBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufSz( UdpSokt * UdpSoktPt, size_t * RecvBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufLen( UdpSokt * UdpSoktPt, size_t * RecvBufLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSend( UdpSokt * UdpSoktPt, int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpRecv( UdpSokt * UdpSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSendPkt( UdpSokt * UdpSoktPt, int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpRecvPkt( UdpSokt * UdpSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpDstoy( UdpSokt * UdpSoktPt, VarStr * ErrInfoVarStrPt );

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

	int Init( int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, VarStr * ErrInfoVarStrPt ) { return UdpInit( &m_UdpSoktPt, LclNodeAddrFmly, LclNodeNamePt, LclNodeSrvcPt, ErrInfoVarStrPt ); }

	int Lock( VarStr * ErrInfoVarStrPt ) { return UdpLock( m_UdpSoktPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return UdpLock( m_UdpSoktPt, ErrInfoVarStrPt ); }
	
	int Connect( int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpConnect( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNamePt, RmtNodeSrvcPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int Disconnect( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpDisconnect( m_UdpSoktPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int GetLclAddr( int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpGetLclAddr( m_UdpSoktPt, LclNodeAddrFmlyPt, LclNodeAddrPt, LclNodePortPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRmtAddr( int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpGetRmtAddr( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrPt, RmtNodePortPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	
	int SetSendBufSz( size_t SendBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpSetSendBufSz( m_UdpSoktPt, SendBufSz, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetSendBufSz( size_t * SendBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpGetSendBufSz( m_UdpSoktPt, SendBufSzPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int SetRecvBufSz( size_t RecvBufSz, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpSetRecvBufSz( m_UdpSoktPt, RecvBufSz, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRecvBufSz( size_t * RecvBufSzPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpGetRecvBufSz( m_UdpSoktPt, RecvBufSzPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetRecvBufLen( size_t * RecvBufLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpGetRecvBufLen( m_UdpSoktPt, RecvBufLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	
	int Send( int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpSend( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNamePt, RmtNodeSrvcPt, DataBufPt, DataBufLen, TimeOutMsec, Times, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int Recv( int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpRecv( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrPt, RmtNodePortPt, DataBufPt, DataBufSz, DataBufLenPt, TimeOutMsec, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int SendPkt( int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, uint32_t Times, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpSendPkt( m_UdpSoktPt, RmtNodeAddrFmly, RmtNodeNamePt, RmtNodeSrvcPt, DataBufPt, DataBufLen, TimeOutMsec, Times, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int RecvPkt( int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return UdpRecvPkt( m_UdpSoktPt, RmtNodeAddrFmlyPt, RmtNodeAddrPt, RmtNodePortPt, DataBufPt, DataBufSz, DataBufLenPt, TimeOutMsec, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Dstoy( VarStr * ErrInfoVarStrPt ) { int p_Rslt = UdpDstoy( m_UdpSoktPt, ErrInfoVarStrPt ); m_UdpSoktPt = NULL; return p_Rslt; }
};
#endif

#endif
