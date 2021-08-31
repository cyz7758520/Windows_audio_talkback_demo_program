#ifndef __FUNC_H__
#define __FUNC_H__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
#include <winsdkver.h>                    //包含Windows SDK支持的最高版本Windows的相关宏
#define _WIN32_WINNT  _WIN32_WINNT_MAXVER //设置_WIN32_WINNT宏为最高版本Windows
#define WINVER        WINVER_MAXVER       //设置WINVER宏为最高版本Windows
#define NTDDI_VERSION NTDDI_MAXVER        //设置NTDDI_VERSION宏为最高版本Windows
#define _WIN32_IE     _WIN32_IE_MAXVER    //设置_WIN32_IE宏为最高版本IE
#define WIN32_LEAN_AND_MEAN               //设置WIN32_LEAN_AND_MEAN宏，防止windows.h头文件包含winsock.h头文件，从而防止AF_IPX宏重定义
#include <windows.h>                      //包含整个Windows SDK支持的API
#include <psapi.h>
#include <Ws2tcpip.h>
#endif
#if( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#if( defined __ANDROID_GCC__ )
#include <jni.h>
#include <android/bitmap.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#endif
#include "VarStr.h"
#include "Log.h"
#include "ThreadLock.h"

//线程局部变量宏。
#if( defined __MS_VCXX__ )
#define __thread __declspec( thread )
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//Func项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_FUNC__ ) //如果正在编译Func项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __FUNC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#endif
	
//交换两个指针变量的值。
#define SWAPPT( type, pt1, pt2 ) ( pt1 = ( type )( ( size_t )pt1 ^ ( size_t )pt2 ), pt2 = ( type )( ( size_t )pt1 ^ ( size_t )pt2 ), pt1 = ( type )( ( size_t )pt1 ^ ( size_t )pt2 ) )

//计算最大值和最小值。
#ifndef __cplusplus
#ifndef max
#define max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef min
#define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#endif

//日期时间函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetTimeAsSec( uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncGetTimeAsMsec( uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncGetTimeAsLclDateTime( int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncSecToLclDateTime( uint64_t Sec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt );
__FUNC_DLLAPI__ int FuncMsecToLclDateTime( uint64_t Msec, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToSec( int Year, int Mon, int Day, int Hour, int Min, int Sec, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclDateTimeToMsec( int Year, int Mon, int Day, int Hour, int Min, int Sec, int Msec, uint64_t * MsecPt );
#endif
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncFileTimeToSec( const FILETIME * FileTimePt, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncFileTimeToMsec( const FILETIME * FileTimePt, uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncFileTimeToLclDateTime( const FILETIME * FileTimePt, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToSec( const SYSTEMTIME * LclSysTimePt, uint64_t * SecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToMsec( const SYSTEMTIME * LclSysTimePt, uint64_t * MsecPt );
__FUNC_DLLAPI__ int FuncLclSysTimeToLclDateTime( const SYSTEMTIME * LclSysTimePt, int * YearPt, int * MonPt, int * DayPt, int * HourPt, int * MinPt, int * SecPt, int * MsecPt );
#endif

//文件函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetFileSzByPath( const char * FullPathStrPt, uint64_t * FileSzPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int FuncGetFileSzByFileStream( FILE * FileStreamPt, uint64_t * FileSzPt, VarStr * ErrInfoVarStrPt );
#endif
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetFileSzByHdl( HANDLE FileHdl, uint64_t * FileSzPt, VarStr * ErrInfoVarStrPt );
#endif

//音频函数。
__FUNC_DLLAPI__ int FuncAudioDataInt16ToFlt32( const int16_t * AudioDataInt16Pt, float * AudioDataFlt32Pt, int SampleCnt, int FltRange, const char * Drct );
__FUNC_DLLAPI__ int FuncAudioDataFlt32ToInt16( const float * AudioDataFlt32Pt, int16_t * AudioDataInt16Pt, int SampleCnt, int FltRange, const char * Drct );

//字符、字符串函数。
__FUNC_DLLAPI__ int FuncIsBlank( char Chr );
__FUNC_DLLAPI__ int FuncIsNum( char Chr );
__FUNC_DLLAPI__ int FuncPathFindFileName( const char * PathStrPt, const char * * FileNameStrPtPt );
__FUNC_DLLAPI__ int FuncPathRmvFileName( char * PathStrPt );
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncStrToInt( const char * IntStrPt, void * IntValPt, size_t IntValSz, size_t * IntStrLenPt, size_t MaxReadFigure = SIZE_MAX, size_t MinReadFigure = 0 );
#else
__FUNC_DLLAPI__ int FuncStrToInt( const char * IntStrPt, void * IntValPt, size_t IntValSz, size_t * IntStrLenPt, size_t MaxReadFigure, size_t MinReadFigure );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncIntToStr( int64_t IntVal, char * IntStrPt, size_t IntStrSz, size_t * IntStrLenPt = NULL, int8_t IsSigned = 1, int8_t System = 10, size_t MinWriteLen = 0, char Aligned = 'R', char FillChr = ' ', int8_t IsWriteEnd = 1 );
#else
__FUNC_DLLAPI__ int FuncIntToStr( int64_t IntVal, char * IntStrPt, size_t IntStrSz, size_t * IntStrLenPt, int8_t IsSigned, int8_t System, size_t MinWriteLen, char Aligned, char FillChr, int8_t IsWriteEnd );
#endif
__FUNC_DLLAPI__ int FuncMemInsMem( char * DstMemPt, size_t DstMemLen, size_t DstMemSz, size_t InsPos, const char * InsMemPt, size_t InsMemLen, size_t * DstMemNewLenPt );
__FUNC_DLLAPI__ int FuncStrInsStr( char * DstStrPt, size_t DstStrSz, size_t InsPos, const char * InsStrPt, size_t * DstStrNewLenPt );
__FUNC_DLLAPI__ int FuncMemTrim( char * SrcMemPt, size_t SrcMemLen, const char * TrimMemPt, size_t TrimMemLen, char TrimPos, char * AfterMemPt, size_t AfterMemSz, size_t * AfterMemLenPt );
__FUNC_DLLAPI__ int FuncStrTrim( char * SrcStrPt, const char * TrimStrPt, char TrimPos, char * AfterStrPt, size_t AfterStrSz, size_t * AfterStrLenPt );
//__FUNC_DLLAPI__ int FuncMemCmp( const void * Buf1Pt, const void * Buf2Pt, size_t MemSz );
__FUNC_DLLAPI__ void * FuncMemCpy( void * DstPt, const void * SrcPt, size_t MemSz );
//__FUNC_DLLAPI__ void * FuncMemCpy_inline( void * DstPt, const void * SrcPt, size_t MemSz );

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#ifdef __cplusplus
__FUNC_DLLAPI__ const char * FuncGetErrInfo( uint32_t ErrNum, char ErrType = 'G' );
#else
__FUNC_DLLAPI__ const char * FuncGetErrInfo( uint32_t ErrNum, char ErrType );
#endif
#endif

#define FuncStrFmtRead( SrcStrPt, ReadCntPt, FmtStrPt, ... )  FuncStrLenFmtRead( SrcStrPt, SIZE_MAX, ReadCntPt, FmtStrPt, __VA_ARGS__ )
__FUNC_DLLAPI__ void FuncStrLenFmtRead( const char * SrcStrPt, size_t SrcStrLen, unsigned int * ReadCntPt, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int FuncGetFmtLen( size_t * FullStrLenPt, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int FuncGetVaFmtLen( size_t * FullStrLenPt, const char * FmtStrPt, va_list VaLst );
__FUNC_DLLAPI__ int FuncStrCpy( char * DstStrPt, size_t DstStrSz, int8_t IsWriteEnd, size_t * DstStrLenPt, const char * SrcStrPt );
__FUNC_DLLAPI__ int FuncStrFmtCpy( char * DstFullStrPt, int8_t IsWriteEnd, size_t * DstFullStrLenPt, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int FuncStrVaFmtCpy( char * DstFullStrPt, int8_t IsWriteEnd, size_t * DstFullStrLenPt, const char * FmtStrPt, va_list VaLst );
#define FuncStrFindChrs( StrPt, FindMemPt, FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt ) FuncMemFindChrs( StrPt, strlen( StrPt ), FindMemPt, strlen( FindMemPt ), FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt )
__FUNC_DLLAPI__ int FuncMemFindChrs( const char * MemPt, size_t MemLen, const char * FindMemPt, size_t FindMemLen, char FindPos, int IsBelong, const char * * MemFindedChrPtPt, size_t * FindedChrTotalPt );
#define FuncStrFindStr( StrPt, FindStrPt, FindPos, FindedChrTotalPt ) FuncMemFindMem( StrPt, strlen( StrPt ), FindStrPt, strlen( FindStrPt ), FindPos, FindedChrTotalPt )
__FUNC_DLLAPI__ int FuncMemFindMem( const char * MemPt, uint64_t MemSz, const char * FindMemPt, uint64_t FindMemSz, const char * FindPos, size_t * FindedMemPosPt );
__FUNC_DLLAPI__ int FuncStrFindMatch( const char * SrcStrPt, const char * PatternStrPt, size_t * PatternStrPos );
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncStrFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchCase = 1 );
#else
__FUNC_DLLAPI__ int FuncStrFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchCase );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncStrMultiFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchLogic = 1, int MatchCase = 1 );
#else
__FUNC_DLLAPI__ int FuncStrMultiFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchLogic, int MatchCase );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncStrUpperCase( char * SrcStrPt, char * DstStrPt = NULL, size_t SrcStrMaxLen = SIZE_MAX, size_t SrcStrMinLen = 0 );
#else
__FUNC_DLLAPI__ int FuncStrUpperCase( char * SrcStrPt, char * DstStrPt, size_t SrcStrMaxLen, size_t SrcStrMinLen );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int FuncStrLowerCase( char * SrcStrPt, char * DstStrPt = NULL, size_t SrcStrMaxLen = SIZE_MAX, size_t SrcStrMinLen = 0 );
#else
__FUNC_DLLAPI__ int FuncStrLowerCase( char * SrcStrPt, char * DstStrPt, size_t SrcStrMaxLen, size_t SrcStrMinLen );
#endif

//获取函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncGetPrivateProfileStringFromSectionMem( const char * KeyNameStrPt, const char * DefaultKeyStrValPt, char * KeyStrValPt, size_t KeyStrValSz, size_t * KeyStrValLenPt, const char * SectionMemPt );
#endif

//设置函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncSetCurActDir( const char * FullPathStrPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int FuncSetWndCenter( HWND SpecWndHdl, HWND OtherWndHdl, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int FuncToast( HWND SpecWndHdl, uint64_t TimeOutMesc, VarStr * ErrInfoVarStrPt, const char * TextPt );
__FUNC_DLLAPI__ int FuncToastFmt( HWND SpecWndHdl, uint64_t TimeOutMesc, VarStr * ErrInfoVarStrPt, const char * FmtStrPt, ... );
#endif

//判断函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncIsForegroundFullscreen();
#endif

//管道函数。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int FuncCreatePipe( HANDLE * PipeReadHdl, HANDLE * PipeWriteHdl, LPVOID * SecurityDescriptor, BOOL InheritHdl, DWORD BufSz );
__FUNC_DLLAPI__ int FuncReadPipe( HANDLE PipeReadHdl, char * * BufPtPt, size_t * BufSzPt, size_t * BufLenPt, const char * EndMemPt, size_t EndMemLen, int IsAllowRealloc, uint64_t TimeOutMsec );
__FUNC_DLLAPI__ void FuncClosePipe( HANDLE PipeReadHdl, HANDLE PipeWriteHdl );
#endif

//JNI函数。
#if( defined __ANDROID_GCC__ )
__FUNC_DLLAPI__ int FuncGetJavaCls( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, jclass * ClsPt );

__FUNC_DLLAPI__ int FuncGetJavaClsMbrVarFieldID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, jfieldID * FieldIDPt );
__FUNC_DLLAPI__ int FuncGetJavaClsMbrFuncMethodID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, jmethodID * MethodIDPt );

__FUNC_DLLAPI__ int FuncSetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, const void * ValPt );
__FUNC_DLLAPI__ int FuncGetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, void * ValPt );

__FUNC_DLLAPI__ int FuncCallJavaClsObjMbrFunc( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, void * RtnValPt, ... );

__FUNC_DLLAPI__ int FuncNewJavaStringClsObjByUTF8CharArr( JNIEnv * env, const char * UTF8CharArrPt, jstring * StringClsObjPtPt );

#define FuncSetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) FuncSetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", ShortValPt )
#define FuncGetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) FuncGetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", ShortValPt )
#define FuncSetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) FuncSetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", IntValPt )
#define FuncGetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) FuncGetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", IntValPt )
#define FuncSetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) FuncSetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", LongValPt )
#define FuncGetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) FuncGetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", LongValPt )
#define FuncSetJavaHTStringClsObjVal( env, HTLongClsObj, StringValPt ) FuncSetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", StringValPt )
#define FuncGetJavaHTStringClsObjVal( env, HTLongClsObj, StringValPt ) FuncGetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", StringValPt )

__FUNC_DLLAPI__ int FuncGetJavaByteArrClsObj( JNIEnv * env, jbyteArray ByteArrClsObj, jbyte * * ByteArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaByteArrClsObj( JNIEnv * env, jbyteArray ByteArrClsObj, jbyte * ByteArrPt );
__FUNC_DLLAPI__ int FuncGetJavaShortArrClsObj( JNIEnv * env, jshortArray ShortArrClsObj, jshort * * ShortArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaShortArrClsObj( JNIEnv * env, jshortArray ShortArrClsObj, jshort * ShortArrPt );
__FUNC_DLLAPI__ int FuncGetJavaIntArrClsObj( JNIEnv * env, jintArray IntArrClsObj, jint * * IntArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaIntArrClsObj( JNIEnv * env, jintArray IntArrClsObj, jint * IntArrPt );
__FUNC_DLLAPI__ int FuncGetJavaLongArrClsObj( JNIEnv * env, jlongArray LongArrClsObj, jlong * * LongArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaLongArrClsObj( JNIEnv * env, jlongArray LongArrClsObj, jlong * LongArrPt );
__FUNC_DLLAPI__ int FuncGetJavaFloatArrClsObj( JNIEnv * env, jfloatArray FloatArrClsObj, jfloat * * FloatArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaFloatArrClsObj( JNIEnv * env, jfloatArray FloatArrClsObj, jfloat * FloatArrPt );
__FUNC_DLLAPI__ int FuncGetJavaDoubleArrClsObj( JNIEnv * env, jdoubleArray DoubleArrClsObj, jdouble * * DoubleArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaDoubleArrClsObj( JNIEnv * env, jdoubleArray DoubleArrClsObj, jdouble * DoubleArrPt );
__FUNC_DLLAPI__ int FuncGetJavaBooleanArrClsObj( JNIEnv * env, jbooleanArray BooleanArrClsObj, jboolean * * BooleanArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaBooleanArrClsObj( JNIEnv * env, jbooleanArray BooleanArrClsObj, jboolean * BooleanArrPt );
__FUNC_DLLAPI__ int FuncGetJavaCharArrClsObj( JNIEnv * env, jcharArray CharArrClsObj, jchar * * CharArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaCharArrClsObj( JNIEnv * env, jcharArray CharArrClsObj, jchar * CharArrPt );
//__FUNC_DLLAPI__ int FuncGetJavaStringClsObjByteArr( JNIEnv * env, jstring StringClsObj, const jbyte * * ByteArrPtPt );
//__FUNC_DLLAPI__ int FuncDestroyJavaStringClsObjByteArr( JNIEnv * env, jstring StringClsObj, const jbyte * ByteArrPt );
__FUNC_DLLAPI__ int FuncGetJavaStringClsObjCharArr( JNIEnv * env, jstring StringClsObj, const jchar * * CharArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaStringClsObjCharArr( JNIEnv * env, jstring StringClsObj, const jchar * CharArrPt );
__FUNC_DLLAPI__ int FuncGetJavaStringClsObjUTF8CharArr( JNIEnv * env, jstring StringClsObj, const char * * UTF8CharArrPtPt );
__FUNC_DLLAPI__ int FuncDestroyJavaStringClsObjUTF8CharArr( JNIEnv * env, jstring StringClsObj, const char * UTF8CharArrPt );

__FUNC_DLLAPI__ int FuncGetAndroidPkgName( JNIEnv * env, char * PkgNameStrPt, size_t PkgNameStrSz, size_t * PkgNameStrLenPt );
#endif

#ifdef __cplusplus
}
#endif

#endif
