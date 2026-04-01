#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
__FUNC_DLLAPI__ uint64_t FuncGetTickAsSec();
__FUNC_DLLAPI__ uint64_t FuncGetTickAsMsec();

__FUNC_DLLAPI__ uint64_t FuncGetTimeAsSec();
__FUNC_DLLAPI__ uint64_t FuncGetTimeAsMsec();
__FUNC_DLLAPI__ void FuncGetTimeAsLclDateTime( int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );

__FUNC_DLLAPI__ int FuncSecToLclDateTime( uint64_t Sec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt );
__FUNC_DLLAPI__ int FuncMsecToLclDateTime( uint64_t Msec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToSec( int Year, int Mon, int Day, int Hour, int Min, int Sec, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToMsec( int Year, int Mon, int Day, int Hour, int Min, int Sec, int Msec, uint64_t * MsecPt );

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ uint64_t FuncFileTimeToSec( FILETIME FileTime, uint64_t * SecPt );
__FUNC_DLLAPI__ uint64_t FuncFileTimeToMsec( FILETIME FileTime, uint64_t * MsecPt );
__FUNC_DLLAPI__ void FuncFileTimeToLclDateTime( FILETIME FileTime, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );

__FUNC_DLLAPI__ int FuncLclSysTimeToSec( const SYSTEMTIME * LclSysTimePt, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToMsec( const SYSTEMTIME * LclSysTimePt, uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToLclDateTime( const SYSTEMTIME * LclSysTimePt, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
#endif

//线程休眠函数。
#if( defined __MS_VCXX__ )
#define SleepSec( SleepTimeSec ) WaitForSingleObject( ( HANDLE )-1, ( SleepTimeSec ) * 1000 )
#define SleepMsec( SleepTimeMsec ) WaitForSingleObject( ( HANDLE )-1, ( SleepTimeMsec ) )
#define SleepUsec( SleepTimeUsec ) std::this_thread::sleep_for( std::chrono::microseconds( ( SleepTimeUsec ) ) )
#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_NDK__ ) || ( defined __HARMONY_NDK__ ) )
#define SleepSec( SleepTimeSec ) usleep( ( SleepTimeSec ) * 1000000 )
#define SleepMsec( SleepTimeMsec ) usleep( ( SleepTimeMsec ) * 1000 )
#define SleepUsec( SleepTimeUsec ) usleep( ( SleepTimeUsec ) )
#endif

#ifdef __cplusplus
}
#endif
