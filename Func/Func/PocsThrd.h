#include "Func.h"

#ifndef __POCSTHRD_H__
#define __POCSTHRD_H__

#ifdef __cplusplus
extern "C"
{
#endif

//进程函数。
#if( defined __MS_VCXX__ )
#define PocsGetCurHdl() GetModuleHandle( NULL )
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
#define PocsGetCurId() GetCurrentProcessId()
#elif( ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#define PocsGetCurId() getpid() //Cygwin下调用getpid返回当前进程在Cygwin环境里的ID，但Cygwin下又没有gettid，为了统一，Cygwin下调用GetCurrentProcessId返回当前进程在Windows环境里的ID。
#endif

//线程函数。
#if( defined __MS_VCXX__ )
#define ThrdGetCurHdl() GetCurrentThread()
#elif( ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __CYGWIN_GCC__ ) )
#define ThrdGetCurHdl() pthread_self()
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
#define ThrdGetCurId() GetCurrentThreadId()
#elif( ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#define ThrdGetCurId() gettid() //Cygwin下没有gettid。
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
typedef struct ThrdInfo ThrdInfo;

#if( defined __MS_VCXX__ )

__FUNC_DLLAPI__ int ThrdInit( ThrdInfo * * ThrdInfoPtPt, DWORD ( WINAPI *ThrdFuncPt )( LPVOID Parm ), LPVOID ThrdFuncParm, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdGetHdl( ThrdInfo * ThrdInfoPt, HANDLE * ThrdHdlPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdGetId( ThrdInfo * ThrdInfoPt, DWORD * ThrdIdPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdWaitGetExitCode( ThrdInfo * ThrdInfoPt, DWORD * ThrdExitCodePt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdWaitDstoy( ThrdInfo * ThrdInfoPt, Vstr * ErrInfoVstrPt );

#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )

__FUNC_DLLAPI__ int ThrdInit( ThrdInfo * * ThrdInfoPtPt, void * ( *ThrdFuncPt )( void * Parm ), void * ThrdFuncParm, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdGetHdl( ThrdInfo * ThrdInfoPt, pthread_t * ThrdHdlPt, Vstr * ErrInfoVstrPt );
//__FUNC_DLLAPI__ int ThrdGetId( ThrdInfo * ThrdInfoPt, pid_t * ThrdIdPt, Vstr * ErrInfoVstrPt ); pthread系列函数无法获取线程ID。
__FUNC_DLLAPI__ int ThrdWaitGetExitCode( ThrdInfo * ThrdInfoPt, void * * ThrdExitCodePt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int ThrdWaitDstoy( ThrdInfo * ThrdInfoPt, Vstr * ErrInfoVstrPt );

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
