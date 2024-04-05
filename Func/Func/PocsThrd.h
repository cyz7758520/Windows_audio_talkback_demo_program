#pragma once

#include "Func.h"

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
typedef enum ThrdRunSts //线程运行状态。
{
	ThrdRunStsNorun, //未开始运行。
	ThrdRunStsRun, //正在运行。
	ThrdRunStsExit, //已经退出。
} ThrdRunSts;

#if( defined __MS_VCXX__ )

__FUNC_DLLAPI__ int ThrdInit( ThrdInfo * * ThrdInfoPtPt, DWORD( WINAPI * ThrdFuncPt )( LPVOID Parm ), LPVOID ThrdFuncParm, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ HANDLE ThrdGetHdl( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ DWORD ThrdGetId( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ DWORD ThrdGetExitCode( ThrdInfo * ThrdInfoPt, int IsBlockWait );
__FUNC_DLLAPI__ ThrdRunSts ThrdGetRunSts( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ int ThrdWaitDstoy( ThrdInfo * ThrdInfoPt, Vstr * ErrInfoVstrPt );

#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )

__FUNC_DLLAPI__ int ThrdInit( ThrdInfo * * ThrdInfoPtPt, void * ( *ThrdFuncPt )( void * Parm ), void * ThrdFuncParm, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ pthread_t ThrdGetHdl( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ pid_t ThrdGetId( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ void * ThrdGetExitCode( ThrdInfo * ThrdInfoPt, int IsBlockWait );
__FUNC_DLLAPI__ ThrdRunSts ThrdGetRunSts( ThrdInfo * ThrdInfoPt );
__FUNC_DLLAPI__ int ThrdWaitDstoy( ThrdInfo * ThrdInfoPt, Vstr * ErrInfoVstrPt );

#endif

#endif

#ifdef __cplusplus
}
#endif
