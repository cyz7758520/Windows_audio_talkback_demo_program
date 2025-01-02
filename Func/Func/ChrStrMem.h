#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

//字符、字符串。
__FUNC_DLLAPI__ int IsBlankChr( uint32_t Chr );
__FUNC_DLLAPI__ int IsNumChr( uint32_t Chr );

//字符串转整数。
__FUNC_DLLAPI__ int _StrToInt( const void * StrPt, ChrSet StrChrSet, size_t StrSzChr, size_t MaxReadDigit, size_t MinReadDigit,  size_t * StrLenChrPt, void * IntValPt, size_t IntValSzByt );
#define StrToInt( StrPt, StrChrSet, StrSzChr, MaxReadDigit, MinReadDigit, StrLenChrPt, IntValPt, IntValSzByt ) \
       _StrToInt( StrPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, StrSzChr, SIZE_MAX ), DEFARG( size_t, MaxReadDigit, SIZE_MAX ), DEFARG( size_t, MinReadDigit, 0 ), DEFARG( size_t *, StrLenChrPt, NULL ), IntValPt, DEFARG( size_t, IntValSzByt, SIZE_MAX ) )

//整数转字符串。
__FUNC_DLLAPI__ int _IntToStr( int64_t IntVal, int32_t IsSigned, void * StrPt, ChrSet StrChrSet, size_t StrSzChr, int32_t System, size_t MaxWriteLenChr, size_t MinWriteLenChr, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * StrLenChrPt );
#define IntToStr( IntVal, IsSigned, StrPt, StrChrSet, StrSzChr, System, MaxWriteLenChr, MinWriteLenChr, Aligned, FillChr, IsWriteEnd, StrLenChrPt ) \
       _IntToStr( IntVal, DEFARG( int32_t, IsSigned, 1 ), StrPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, StrSzChr, SIZE_MAX ), DEFARG( int32_t, System, 10 ), DEFARG( size_t, MaxWriteLenChr, SIZE_MAX ), DEFARG( size_t, MinWriteLenChr, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, StrLenChrPt, NULL ) )

//浮点数转字符串。
__FUNC_DLLAPI__ int _FltToStr( double FltVal, void * StrPt, ChrSet StrChrSet, size_t StrSzChr, size_t DecimalDigit, size_t MaxWriteLenChr, size_t MinWriteLenChr, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * StrLenChrPt );
#define FltToStr( FltVal, StrPt, StrChrSet, StrSzChr, DecimalDigit, MaxWriteLenChr, MinWriteLenChr, Aligned, FillChr, IsWriteEnd, StrLenChrPt ) \
       _FltToStr( FltVal, StrPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, StrSzChr, SIZE_MAX ), DEFARG( size_t, DecimalDigit, 6 ), DEFARG( size_t, MaxWriteLenChr, SIZE_MAX ), DEFARG( size_t, MinWriteLenChr, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, StrLenChrPt, NULL ) ) 

__FUNC_DLLAPI__ int MemInsMem( char * DstMemPt, size_t DstMemLenByt, size_t DstMemSzByt, size_t InsPos, const char * InsMemPt, size_t InsMemLenByt, size_t * DstMemNewLenPt );

__FUNC_DLLAPI__ int StrInsStr( char * DstStrPt, size_t DstStrSz, size_t InsPos, const char * InsStrPt, size_t * DstStrNewLenPt );

__FUNC_DLLAPI__ int MemTrim( char * SrcMemPt, size_t SrcMemLenByt, const char * TrimMemPt, size_t TrimMemLenByt, char TrimPos, char * AfterMemPt, size_t AfterMemSzByt, size_t * AfterMemLenBytPt );

__FUNC_DLLAPI__ int StrTrim( char * SrcStrPt, const char * TrimStrPt, char TrimPos, char * AfterStrPt, size_t AfterStrSz, size_t * AfterStrLenPt );

//__FUNC_DLLAPI__ int MemCmp( const void * Buf1Pt, const void * Buf2Pt, size_t MemSzByt );

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
#if( ( defined __X86__ ) || ( defined __X64__ ) )
//__FUNC_DLLAPI__ void * __cdecl memcpy( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
//__FUNC_DLLAPI__ void * __cdecl memmove( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
__FUNC_DLLAPI__ void * __cdecl MemCpy( void * DstPt, void const * SrcPt, size_t SzByt );
__FUNC_DLLAPI__ void * __cdecl MemCpyAvx( void * DstPt, void const * SrcPt, size_t SzByt );
__FUNC_DLLAPI__ void * __cdecl MemCpySse2( void * DstPt, void const * SrcPt, size_t SzByt );
#elif( defined __ARMAT32__ )
//__FUNC_DLLAPI__ void * __cdecl memcpy( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
//__FUNC_DLLAPI__ void * __cdecl memmove( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
__FUNC_DLLAPI__ void * __cdecl MemCpy( void * DstPt, void const * SrcPt, size_t SzByt );
#endif
#endif

//获取错误信息。
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
typedef enum
{
    ErrTypeErrno,
    #if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
    ErrTypeWinSys,
    ErrTypeWinNetMsg,
    ErrTypeWinINet,
    ErrTypeWinPdh,
    ErrTypeWinNtDll,
    ErrTypeWinMM,
    #endif
    ErrTypeEnd
}ErrType;
__FUNC_DLLAPI__ Vstr * GetErrInfo( uint32_t ErrNum, Vstr * InfoVstrPt, ... );

#define GetErrnoInfo( ErrNum, InfoVstrPt ) GetErrInfo( DEFARG( uint32_t, ErrNum, errno ), InfoVstrPt, ErrTypeErrno, ErrTypeEnd )
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
#define GetWinLastErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( DEFARG( uint32_t, ErrNum, GetLastError() ), InfoVstrPt, ErrTypeWinSys, ErrTypeEnd )
#define GetWinSysErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinSys, ErrTypeEnd )
#define GetWinNetMsgErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinNetMsg, ErrTypeEnd )
#define GetWinINetErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinINet, ErrTypeEnd )
#define GetWinPdhErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinPdh, ErrTypeEnd )
#define GetWinNtDllErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinNtDll, ErrTypeEnd )
#define GetWinMMErrInfo( ErrNum, InfoVstrPt ) GetErrInfo( ErrNum, InfoVstrPt, ErrTypeWinMM, ErrTypeEnd )
#endif
#endif

//获取窗口消息信息。
#if( defined __MS_VCXX__ )
__FUNC_DLLAPI__ Vstr * GetWndMsgInfo( HWND WndHdl, UINT MsgNum, WPARAM WParam, LPARAM LParam, Vstr * InfoVstrPt );
#endif

//获取字符串的长度。
__FUNC_DLLAPI__ size_t _StrLen( const void * StrPt, ChrSet StrChrSet, size_t StrSzChr, size_t * StrLenChrPt );
#define StrLen( StrPt, StrChrSet, StrSzChr, StrLenChrPt ) \
       _StrLen( StrPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, StrSzChr, SIZE_MAX ), DEFARG( size_t *, StrLenChrPt, NULL ) )

//字符串复制到字符串。
__FUNC_DLLAPI__ int _StrCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSzChr, int32_t IsWriteEnd, size_t * DstStrLenChrPt, const void * SrcStrPt, ChrSet SrcStrChrSet, size_t SrcStrSzChr, size_t * SrcStrLenChrPt );
#define StrCpy( DstStrPt, DstStrChrSet, DstStrSzChr, IsWriteEnd, DstStrLenChrPt, SrcStrPt, SrcStrChrSet, SrcStrSzChr, SrcStrLenChrPt ) \
       _StrCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenChrPt, NULL ), SrcStrPt, DEFARG( ChrSet, SrcStrChrSet, Utf8 ), DEFARG( size_t, SrcStrSzChr, SIZE_MAX ), DEFARG( size_t *, SrcStrLenChrPt, NULL ) )

//字符串复制并创建栈字符串。
#define AstrCpy( AstrPtType, AstrPt, AstrChrSet, IsWriteEnd, AstrLenChrPt, SrcStrPt, SrcStrChrSet, SrcStrSzChr, SrcStrLenChrPt ) \
{ \
    StrCpy( , AstrChrSet, , IsWriteEnd, AstrLenChrPt, SrcStrPt, SrcStrChrSet, SrcStrSzChr, SrcStrLenChrPt ); /*设置栈字符串的长度。*/ \
	AstrPt = ( AstrPtType )alloca( ( *AstrLenChrPt + IsWriteEnd ) * AstrChrSet ); /*设置栈字符串缓冲区的指针。*/ \
	StrCpy( AstrPt, AstrChrSet, , IsWriteEnd, , SrcStrPt, SrcStrChrSet, SrcStrSzChr, SrcStrLenChrPt ); /*设置栈字符串。*/ \
}

//动态字符串复制并创建栈动态字符串。
#define AvstrCpy( AvstrPt, AvstrChrSet, SrcVstrPt ) \
{ \
	const Vstr * _p_SrcVstrPt = SrcVstrPt; \
	AvstrPt = ( Vstr * )alloca( sizeof( Vstr ) ); /*设置栈动态字符串的指针。*/ \
	AvstrPt->m_ChrSet = AvstrChrSet; /*设置栈动态字符串的字符集。*/ \
	AvstrPt->m_SzChr = StrCpyMaxLenByt( AvstrPt->m_ChrSet, _p_SrcVstrPt->m_ChrSet, _p_SrcVstrPt->m_LenChr ); /*设置栈动态字符串的大小。*/ \
	AvstrPt->m_StrPt = alloca( AvstrPt->m_SzChr * AvstrPt->m_ChrSet ); /*设置栈动态字符串缓冲区的指针。*/ \
	StrCpy( ( uint8_t * )AvstrPt->m_StrPt, AvstrPt->m_ChrSet, AvstrPt->m_SzChr, 1, &AvstrPt->m_LenChr, _p_SrcVstrPt->m_StrPt, _p_SrcVstrPt->m_ChrSet, _p_SrcVstrPt->m_SzChr, ); /*源始动态字符串复制到栈动态字符串。*/ \
}

//将变量格式化复制到字符串。
__FUNC_DLLAPI__ int _StrFmtCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSzChr, int32_t IsWriteEnd, size_t * DstStrLenChrPt, const void * FmtStrPt, ChrSet FmtStrChrSet, ... );
#define StrFmtCpy( DstStrPt, DstStrChrSet, DstStrSzChr, IsWriteEnd, DstStrLenChrPt, FmtStrPt, FmtStrChrSet, ... ) \
       _StrFmtCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenChrPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), __VA_ARGS__ )

//将变量格式化复制并创建栈字符串。
#define AstrFmtCpy( AstrPtType, AstrPt, AstrChrSet, IsWriteEnd, AstrLenChrPt, FmtStrPt, FmtStrChrSet, ... ) \
{ \
    StrFmtCpy( , AstrChrSet, , IsWriteEnd, AstrLenChrPt, FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串的长度。*/ \
	AstrPt = ( AstrPtType )alloca( ( *AstrLenChrPt + IsWriteEnd ) * AstrChrSet ); /*设置栈字符串缓冲区的指针。*/ \
	StrFmtCpy( AstrPt, AstrChrSet, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

//将变量格式化复制并创建栈动态字符串。
#define AvstrFmtCpy( AvstrPt, AvstrChrSet, FmtVstrPt, ... ) \
{ \
    const Vstr * _p_FmtVstrPt = FmtVstrPt; \
	AvstrPt = ( Vstr * )alloca( sizeof( Vstr ) ); /*设置栈动态字符串的指针。*/ \
    AvstrPt->m_ChrSet = AvstrChrSet; /*设置栈动态字符串的字符集。*/ \
	StrFmtCpy( , AvstrPt->m_ChrSet, , 1, &AvstrPt->m_SzChr, _p_FmtVstrPt->m_StrPt, _p_FmtVstrPt->m_ChrSet, __VA_ARGS__ ), AvstrPt->m_SzChr++; /*设置栈动态字符串的大小。*/ \
	AvstrPt->m_StrPt = alloca( AvstrPt->m_SzChr * AvstrPt->m_ChrSet ); /*设置栈动态字符串缓冲区的指针。*/ \
	StrFmtCpy( ( uint8_t * )AvstrPt->m_StrPt, AvstrPt->m_ChrSet, , 1, , _p_FmtVstrPt->m_StrPt, _p_FmtVstrPt->m_ChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

//将可变参数列表格式化复制到字符串。
__FUNC_DLLAPI__ int _StrVaFmtCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSzChr, int32_t IsWriteEnd, size_t * DstStrLenChrPt, const void * FmtStrPt, ChrSet FmtStrChrSet, va_list VaLst );
#define StrVaFmtCpy( DstStrPt, DstStrChrSet, DstStrSzChr, IsWriteEnd, DstStrLenChrPt, FmtStrPt, FmtStrChrSet, VaLst ) \
       _StrVaFmtCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenChrPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), VaLst )

//将可变参数列表格式化复制并创建栈字符串。
#define AstrVaFmtCpy( AstrPtType, AstrPt, AstrChrSet, IsWriteEnd, AstrLenChrPt, FmtStrPt, FmtStrChrSet, VaLst ) \
{ \
    StrVaFmtCpy( , AstrChrSet, , IsWriteEnd, AstrLenChrPt, FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串的长度。*/ \
	AstrPt = ( AstrPtType )alloca( ( *AstrLenChrPt + IsWriteEnd ) * AstrChrSet ); /*设置栈字符串缓冲区的指针。*/ \
	StrVaFmtCpy( AstrPt, AstrChrSet, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串。*/ \
}

//将字符串格式化读取到变量。
__FUNC_DLLAPI__ void _StrFmtRead( const char * SrcStrPt, size_t SrcStrMaxLen, unsigned int * ReadCntPt, const char * FmtStrPt, ... );
#define StrFmtRead( SrcStrPt, SrcStrMaxLen, ReadCntPt, FmtStrPt, ... ) \
       _StrFmtRead( SrcStrPt, DEFARG( size_t, SrcStrMaxLen, SIZE_MAX ), DEFARG( unsigned int *, ReadCntPt, NULL ), FmtStrPt, __VA_ARGS__ )

#define StrFindChrs( StrPt, FindMemPt, FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt ) MemFindChrs( StrPt, strlen( StrPt ), FindMemPt, strlen( FindMemPt ), FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt )
__FUNC_DLLAPI__ int MemFindChrs( const char * MemPt, size_t MemLenByt, const char * FindMemPt, size_t FindMemLenByt, char FindPos, int IsBelong, const char * * MemFindedChrPtPt, size_t * FindedChrTotalPt );
#define StrFindStr( StrPt, FindStrPt, FindPos, FindedChrTotalPt ) MemFindMem( StrPt, strlen( StrPt ), FindStrPt, strlen( FindStrPt ), FindPos, FindedChrTotalPt )
__FUNC_DLLAPI__ int MemFindMem( const char * MemPt, size_t MemSzByt, const char * FindMemPt, size_t FindMemSzByt, const char * FindPos, size_t * FindedMemPosPt );
__FUNC_DLLAPI__ int MemSum( const void * MemPt, size_t MemSzByt, uint64_t * SumPt );
__FUNC_DLLAPI__ int StrFindMatch( const char * SrcStrPt, const char * PatternStrPt, size_t * PatternStrPos );
#ifdef __cplusplus
__FUNC_DLLAPI__ int StrFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchCase = 1 );
#else
__FUNC_DLLAPI__ int StrFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchCase );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int StrMultiFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchLogic = 1, int MatchCase = 1 );
#else
__FUNC_DLLAPI__ int StrMultiFullMatch( const char * SrcStrPt, const char * PatternStrPt, int MatchLogic, int MatchCase );
#endif
__FUNC_DLLAPI__ int _StrUpperCase( char * SrcStrPt, char * DstStrPt, size_t MaxLen, size_t MinLen );
#define StrUpperCase( SrcStrPt, DstStrPt, MaxLen, MinLen ) _StrUpperCase( SrcStrPt, DEFARG( char *, DstStrPt, NULL ), DEFARG( size_t, MaxLen, SIZE_MAX ), DEFARG( size_t, MinLen, 0 ) )
__FUNC_DLLAPI__ int _StrLowerCase( char * SrcStrPt, char * DstStrPt, size_t MaxLen, size_t MinLen );
#define StrLowerCase( SrcStrPt, DstStrPt, MaxLen, MinLen ) StrLowerCase( SrcStrPt, DEFARG( char *, DstStrPt, NULL ), DEFARG( size_t, MaxLen, SIZE_MAX ), DEFARG( size_t, MinLen, 0 ) )

#ifdef __cplusplus
}
#endif
