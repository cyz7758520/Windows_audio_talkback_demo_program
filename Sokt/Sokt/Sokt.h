#ifndef __SOKT_H__
#define __SOKT_H__

#include "VarStr.h"
#include <stdint.h>
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
#include <Ws2tcpip.h>
#elif( ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

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

typedef struct TcpSrvrSokt TcpSrvrSokt;
typedef struct TcpClntSokt TcpClntSokt;

__SOKT_DLLAPI__ int TcpSrvrInit( TcpSrvrSokt * * TcpSrvrSoktPtPt, int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, int MaxWait, int IsReuseAddr, VarStr * ErrInfoVarStrPt );
//__SOKT_DLLAPI__ int TcpSrvrInitByHdl( TcpSrvrSokt * * TcpSrvrSoktPtPt, SOCKET TcpSrvrSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrGetLclAddr( TcpSrvrSokt * TcpSrvrSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrAccept( TcpSrvrSokt * TcpSrvrSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, uint16_t TimeOutMsec, TcpClntSokt * * TcpClntSoktPtPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpSrvrDestroy( TcpSrvrSokt * TcpSrvrSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntInit( TcpClntSokt * * TcpClntSoktPtPt, int RmtLclNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const char * LclNodeNamePt, const char * LclNodeSrvcPt, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntInitByHdl( TcpClntSokt * * TcpClntSoktPtPt, SOCKET TcpClntSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntGetLclAddr( TcpClntSokt * TcpClntSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRmtAddr( TcpClntSokt * TcpClntSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSendPkt( TcpClntSokt * TcpClntSoktPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntRecvPkt( TcpClntSokt * TcpClntSoktPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetMultiThread( TcpClntSokt * TcpClntSoktPt, int IsMultiThread, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetMultiThread( TcpClntSokt * TcpClntSoktPt, int * IsMultiThreadPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetNoDelay( TcpClntSokt * TcpClntSoktPt, int IsNoDelay, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetNoDelay( TcpClntSokt * TcpClntSoktPt, int * IsNoDelayPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t SendBufSz, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetSendBufSz( TcpClntSokt * TcpClntSoktPt, size_t * SendBufSzPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntSetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t RecvBufSz, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufSz( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufSzPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int TcpClntGetRecvBufLen( TcpClntSokt * TcpClntSoktPt, size_t * RecvBufLenPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int TcpClntDestroy( TcpClntSokt * TcpClntSoktPt, uint16_t TimeOutSec, VarStr * ErrInfoVarStrPt );

typedef struct UdpSokt UdpSokt;

__SOKT_DLLAPI__ int UdpInit( UdpSokt * * UdpSoktPtPt, int LclNodeAddrFmly, const char * LclNodeNamePt, const char * LclNodeSrvcPt, VarStr * ErrInfoVarStrPt );
//__SOKT_DLLAPI__ int UdpInitByHdl( UdpSokt * * UdpSoktPtPt, SOCKET UdpSoktHdl, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpConnect( UdpSokt * UdpSoktPt, int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpDisconnect( UdpSokt * UdpSoktPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpGetLclAddr( UdpSokt * UdpSoktPt, int * LclNodeAddrFmlyPt, char * LclNodeAddrPt, char * LclNodePortPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRmtAddr( UdpSokt * UdpSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSendPkt( UdpSokt * UdpSoktPt, int RmtNodeAddrFmly, const char * RmtNodeNamePt, const char * RmtNodeSrvcPt, const void * DataBufPt, size_t DataBufLen, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpRecvPkt( UdpSokt * UdpSoktPt, int * RmtNodeAddrFmlyPt, char * RmtNodeAddrPt, char * RmtNodePortPt, void * DataBufPt, size_t DataBufSz, size_t * DataBufLenPt, uint16_t TimeOutMsec, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSetMultiThread( UdpSokt * UdpSoktPt, int IsMultiThread, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetMultiThread( UdpSokt * UdpSoktPt, int * IsMultiThreadPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSetSendBufSz( UdpSokt * UdpSoktPt, size_t SendBufSz, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetSendBufSz( UdpSokt * UdpSoktPt, size_t * SendBufSzPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpSetRecvBufSz( UdpSokt * UdpSoktPt, size_t RecvBufSz, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufSz( UdpSokt * UdpSoktPt, size_t * RecvBufSzPt, VarStr * ErrInfoVarStrPt );
__SOKT_DLLAPI__ int UdpGetRecvBufLen( UdpSokt * UdpSoktPt, size_t * RecvBufLenPt, VarStr * ErrInfoVarStrPt );

__SOKT_DLLAPI__ int UdpDestroy( UdpSokt * UdpSoktPt, VarStr * ErrInfoVarStrPt );

#endif
