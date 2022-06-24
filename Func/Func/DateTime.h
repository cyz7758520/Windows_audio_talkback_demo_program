#include "Func.h"

#ifndef __DATETIME_H__
#define __DATETIME_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
__FUNC_DLLAPI__ uint64_t FuncGetTimeAsSec( uint64_t * SecPt );
__FUNC_DLLAPI__ uint64_t FuncGetTimeAsMsec( uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncGetTimeAsLclDateTime( int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncSecToLclDateTime( uint64_t Sec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt );
__FUNC_DLLAPI__ int FuncMsecToLclDateTime( uint64_t Msec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToSec( int Year, int Mon, int Day, int Hour, int Min, int Sec, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToMsec( int Year, int Mon, int Day, int Hour, int Min, int Sec, int Msec, uint64_t * MsecPt );
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ uint64_t FuncFileTimeToSec( const FILETIME * FileTimePt, uint64_t * SecPt );
__FUNC_DLLAPI__ uint64_t FuncFileTimeToMsec( const FILETIME * FileTimePt, uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncFileTimeToLclDateTime( const FILETIME * FileTimePt, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToSec( const SYSTEMTIME * LclSysTimePt, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToMsec( const SYSTEMTIME * LclSysTimePt, uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToLclDateTime( const SYSTEMTIME * LclSysTimePt, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
#endif

#ifdef __cplusplus
}
#endif

#endif
