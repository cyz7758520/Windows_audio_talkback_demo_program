#include "Func.h"

#ifndef __CHRSTRMEM_H__
#define __CHRSTRMEM_H__

#ifdef __cplusplus
extern "C"
{
#endif

//字符、字符串。
__FUNC_DLLAPI__ int IsBlankChr( uint32_t Chr );

__FUNC_DLLAPI__ int IsNumChr( uint32_t Chr );

//字符串转整数。
__FUNC_DLLAPI__ int _U8strToInt( const uint8_t * IntStrPt, size_t IntStrSz, size_t MaxReadDigit, size_t MinReadDigit, size_t * IntStrLenPt, void * IntValPt, size_t IntValSz );
#define U8strToInt( IntStrPt, IntStrSz, MaxReadDigit, MinReadDigit, IntStrLenPt, IntValPt, IntValSz ) \
       _U8strToInt( ( const uint8_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( size_t, MaxReadDigit, SIZE_MAX ), DEFARG( size_t, MinReadDigit, 0 ), DEFARG( size_t *, IntStrLenPt, NULL ), IntValPt, DEFARG( size_t, IntValSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _U16strToInt( const uint16_t * IntStrPt, size_t IntStrSz, size_t MaxReadDigit, size_t MinReadDigit, size_t * IntStrLenPt, void * IntValPt, size_t IntValSz );
#define U16strToInt( IntStrPt, IntStrSz, MaxReadDigit, MinReadDigit, IntStrLenPt, IntValPt, IntValSz ) \
       _U16strToInt( ( const uint16_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( size_t, MaxReadDigit, SIZE_MAX ), DEFARG( size_t, MinReadDigit, 0 ), DEFARG( size_t *, IntStrLenPt, NULL ), IntValPt, DEFARG( size_t, IntValSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _U32strToInt( const uint32_t * IntStrPt, size_t IntStrSz, size_t MaxReadDigit, size_t MinReadDigit, size_t * IntStrLenPt, void * IntValPt, size_t IntValSz );
#define U32strToInt( IntStrPt, IntStrSz, MaxReadDigit, MinReadDigit, IntStrLenPt, IntValPt, IntValSz ) \
       _U32strToInt( ( const uint32_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( size_t, MaxReadDigit, SIZE_MAX ), DEFARG( size_t, MinReadDigit, 0 ), DEFARG( size_t *, IntStrLenPt, NULL ), IntValPt, DEFARG( size_t, IntValSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _StrToInt( const void * IntStrPt, ChrSet IntStrChrSet, size_t IntStrSz, size_t MaxReadDigit, size_t MinReadDigit, size_t * IntStrLenPt, void * IntValPt, size_t IntValSz );
#define StrToInt( IntStrPt, IntStrChrSet, IntStrSz, MaxReadDigit, MinReadDigit, IntStrLenPt, IntValPt, IntValSz ) \
       _StrToInt( IntStrPt, DEFARG( ChrSet, IntStrChrSet, Utf8 ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( size_t, MaxReadDigit, SIZE_MAX ), DEFARG( size_t, MinReadDigit, 0 ), DEFARG( size_t *, IntStrLenPt, NULL ), IntValPt, DEFARG( size_t, IntValSz, SIZE_MAX ) )

//整数转字符串。
__FUNC_DLLAPI__ int _IntToU8str( int64_t IntVal, int32_t IsSigned, uint8_t * IntStrPt, size_t IntStrSz, int32_t System, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint8_t FillChr, int32_t IsWriteEnd, size_t * IntStrLenPt );
#define IntToU8str( IntVal, IsSigned, IntStrPt, IntStrSz, System, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, IntStrLenPt ) \
       _IntToU8str( IntVal, DEFARG( int32_t, IsSigned, 1 ), ( uint8_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( int32_t, System, 10 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint8_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, IntStrLenPt, NULL ) )

__FUNC_DLLAPI__ int _IntToU16str( int64_t IntVal, int32_t IsSigned, uint16_t * IntStrPt, size_t IntStrSz, int32_t System, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint16_t FillChr, int32_t IsWriteEnd, size_t * IntStrLenPt );
#define IntToU16str( IntVal, IsSigned, IntStrPt, IntStrSz, System, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, IntStrLenPt ) \
       _IntToU16str( IntVal, DEFARG( int32_t, IsSigned, 1 ), ( uint16_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( int32_t, System, 10 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint16_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, IntStrLenPt, NULL ) )

__FUNC_DLLAPI__ int _IntToU32str( int64_t IntVal, int32_t IsSigned, uint32_t * IntStrPt, size_t IntStrSz, int32_t System, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * IntStrLenPt );
#define IntToU32str( IntVal, IsSigned, IntStrPt, IntStrSz, System, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, IntStrLenPt ) \
       _IntToU32str( IntVal, DEFARG( int32_t, IsSigned, 1 ), ( uint32_t * )( IntStrPt ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( int32_t, System, 10 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, IntStrLenPt, NULL ) )

__FUNC_DLLAPI__ int _IntToStr( int64_t IntVal, int32_t IsSigned, void * IntStrPt, ChrSet IntStrChrSet, size_t IntStrSz, int32_t System, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * IntStrLenPt );
#define IntToStr( IntVal, IsSigned, IntStrPt, IntStrChrSet, IntStrSz, System, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, IntStrLenPt ) \
       _IntToStr( IntVal, DEFARG( int32_t, IsSigned, 1 ), IntStrPt, DEFARG( ChrSet, IntStrChrSet, Utf8 ), DEFARG( size_t, IntStrSz, SIZE_MAX ), DEFARG( int32_t, System, 10 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, IntStrLenPt, NULL ) )

//浮点数转字符串。
__FUNC_DLLAPI__ int _FltToU8str( double FltVal, uint8_t * FltStrPt, size_t FltStrSz, size_t DecimalDigit, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint8_t FillChr, int32_t IsWriteEnd, size_t * FltStrLenPt );
#define FltToU8str( FltVal, FltStrPt, FltStrSz, DecimalDigit, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, FltStrLenPt ) \
       _FltToU8str( FltVal, ( uint8_t * )( FltStrPt ), DEFARG( size_t, FltStrSz, SIZE_MAX ), DEFARG( size_t, DecimalDigit, 6 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint8_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, FltStrLenPt, NULL ) ) 

__FUNC_DLLAPI__ int _FltToU16str( double FltVal, uint16_t * FltStrPt, size_t FltStrSz, size_t DecimalDigit, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint16_t FillChr, int32_t IsWriteEnd, size_t * FltStrLenPt );
#define FltToU16str( FltVal, FltStrPt, FltStrSz, DecimalDigit, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, FltStrLenPt ) \
       _FltToU16str( FltVal, ( uint16_t * )( FltStrPt ), DEFARG( size_t, FltStrSz, SIZE_MAX ), DEFARG( size_t, DecimalDigit, 6 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint16_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, FltStrLenPt, NULL ) ) 

__FUNC_DLLAPI__ int _FltToU32str( double FltVal, uint32_t * FltStrPt, size_t FltStrSz, size_t DecimalDigit, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * FltStrLenPt );
#define FltToU32str( FltVal, FltStrPt, FltStrSz, DecimalDigit, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, FltStrLenPt ) \
       _FltToU32str( FltVal, ( uint32_t * )( FltStrPt ), DEFARG( size_t, FltStrSz, SIZE_MAX ), DEFARG( size_t, DecimalDigit, 6 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, FltStrLenPt, NULL ) ) 

__FUNC_DLLAPI__ int _FltToStr( double FltVal, void * FltStrPt, ChrSet FltStrChrSet, size_t FltStrSz, size_t DecimalDigit, size_t MaxWriteLen, size_t MinWriteLen, Pos_t Aligned, uint32_t FillChr, int32_t IsWriteEnd, size_t * FltStrLenPt );
#define FltToStr( FltVal, FltStrPt, FltStrChrSet, FltStrSz, DecimalDigit, MaxWriteLen, MinWriteLen, Aligned, FillChr, IsWriteEnd, FltStrLenPt ) \
       _FltToStr( FltVal, FltStrPt, DEFARG( ChrSet, FltStrChrSet, Utf8 ), DEFARG( size_t, FltStrSz, SIZE_MAX ), DEFARG( size_t, DecimalDigit, 6 ), DEFARG( size_t, MaxWriteLen, SIZE_MAX ), DEFARG( size_t, MinWriteLen, 0 ), DEFARG( Pos_t, Aligned, Rit ), DEFARG( uint32_t, FillChr, ' ' ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, FltStrLenPt, NULL ) ) 

__FUNC_DLLAPI__ int MemInsMem( char * DstMemPt, size_t DstMemLen, size_t DstMemSz, size_t InsPos, const char * InsMemPt, size_t InsMemLen, size_t * DstMemNewLenPt );

__FUNC_DLLAPI__ int StrInsStr( char * DstStrPt, size_t DstStrSz, size_t InsPos, const char * InsStrPt, size_t * DstStrNewLenPt );

__FUNC_DLLAPI__ int MemTrim( char * SrcMemPt, size_t SrcMemLen, const char * TrimMemPt, size_t TrimMemLen, char TrimPos, char * AfterMemPt, size_t AfterMemSz, size_t * AfterMemLenPt );

__FUNC_DLLAPI__ int StrTrim( char * SrcStrPt, const char * TrimStrPt, char TrimPos, char * AfterStrPt, size_t AfterStrSz, size_t * AfterStrLenPt );

//__FUNC_DLLAPI__ int MemCmp( const void * Buf1Pt, const void * Buf2Pt, size_t MemSz );

__FUNC_DLLAPI__ void * MemCpy( void * DstPt, const void * SrcPt, size_t MemSz );
//__FUNC_DLLAPI__ void * MemCpy_inline( void * DstPt, const void * SrcPt, size_t MemSz );

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ Vstr * _GetLastErrInfo( uint32_t ErrNum, Vstr * InfoVstrPt );
#define GetLastErrInfo( ErrNum, InfoVstrPt ) _GetLastErrInfo( DEFARG( uint32_t, ErrNum, GetLastError() ), InfoVstrPt )
__FUNC_DLLAPI__ Vstr * GetNtStatuesInfo( uint32_t ErrNum, Vstr * InfoVstrPt );
__FUNC_DLLAPI__ Vstr * GetWinMMErrInfo( uint32_t ErrNum, Vstr * InfoVstrPt );
#endif
#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
__FUNC_DLLAPI__ Vstr * _GetErrnoInfo( uint32_t ErrNum, Vstr * InfoVstrPt );
#define GetErrnoInfo( ErrNum, InfoVstrPt ) _GetErrnoInfo( DEFARG( uint32_t, ErrNum, errno ), InfoVstrPt )
#endif

//获取字符串的长度。
__FUNC_DLLAPI__ size_t _U8strLen( const uint8_t * StrPt, size_t StrSz, size_t * StrLenPt );
#define U8strLen( StrPt, StrSz, StrLenPt ) \
       _U8strLen( ( const uint8_t * )( StrPt ), DEFARG( size_t, StrSz, SIZE_MAX ), DEFARG( size_t *, StrLenPt, NULL ) )

__FUNC_DLLAPI__ size_t _U16strLen( const uint16_t * StrPt, size_t StrSz, size_t * StrLenPt );
#define U16strLen( StrPt, StrSz, StrLenPt ) \
       _U16strLen( ( const uint16_t * )( StrPt ), DEFARG( size_t, StrSz, SIZE_MAX ), DEFARG( size_t *, StrLenPt, NULL ) )

__FUNC_DLLAPI__ size_t _U32strLen( const uint32_t * StrPt, size_t StrSz, size_t * StrLenPt );
#define U32strLen( StrPt, StrSz, StrLenPt ) \
       _U32strLen( ( const uint32_t * )( StrPt ), DEFARG( size_t, StrSz, SIZE_MAX ), DEFARG( size_t *, StrLenPt, NULL ) )

__FUNC_DLLAPI__ size_t _StrLen( const void * StrPt, ChrSet StrChrSet, size_t StrSz, size_t * StrLenPt );
#define StrLen( StrPt, StrChrSet, StrSz, StrLenPt ) \
       _StrLen( StrPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, StrSz, SIZE_MAX ), DEFARG( size_t *, StrLenPt, NULL ) )

//字符串复制到字符串。
__FUNC_DLLAPI__ int _U8strCpy( uint8_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const uint8_t * SrcStrPt, size_t SrcStrSz );
#define U8strCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, SrcStrPt, SrcStrSz ) \
       _U8strCpy( ( uint8_t * )( DstStrPt ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), ( const uint8_t * )( SrcStrPt ), DEFARG( size_t, SrcStrSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _U16strCpy( uint16_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const uint16_t * SrcStrPt, size_t SrcStrSz );
#define U16strCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, SrcStrPt, SrcStrSz ) \
       _U16strCpy( ( uint16_t * )( DstStrPt ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), ( const uint16_t * )( SrcStrPt ), DEFARG( size_t, SrcStrSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _U32strCpy( uint32_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const uint32_t * SrcStrPt, size_t SrcStrSz );
#define U32strCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, SrcStrPt, SrcStrSz ) \
       _U32strCpy( ( uint32_t * )( DstStrPt ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), ( const uint32_t * )( SrcStrPt ), DEFARG( size_t, SrcStrSz, SIZE_MAX ) )

__FUNC_DLLAPI__ int _StrCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * SrcStrPt, ChrSet SrcStrChrSet, size_t SrcStrSz, size_t * SrcStrLenPt );
#define StrCpy( DstStrPt, DstStrChrSet, DstStrSz, IsWriteEnd, DstStrLenPt, SrcStrPt, SrcStrChrSet, SrcStrSz, SrcStrLenPt ) \
       _StrCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), SrcStrPt, DEFARG( ChrSet, SrcStrChrSet, Utf8 ), DEFARG( size_t, SrcStrSz, SIZE_MAX ), DEFARG( size_t *, SrcStrLenPt, NULL ) )

//将变量格式化复制到字符串。
__FUNC_DLLAPI__ int _U8strFmtCpy( uint8_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, ... );
#define U8strFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, ... ) \
       _U8strFmtCpy( DEFARG( uint8_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), __VA_ARGS__ )

__FUNC_DLLAPI__ int _U16strFmtCpy( uint16_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, ... );
#define U16strFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, ... ) \
       _U16strFmtCpy( DEFARG( uint16_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), __VA_ARGS__ )

__FUNC_DLLAPI__ int _U32strFmtCpy( uint32_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, ... );
#define U32strFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, ... ) \
       _U32strFmtCpy( DEFARG( uint32_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), __VA_ARGS__ )

__FUNC_DLLAPI__ int _StrFmtCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, ... );
#define StrFmtCpy( DstStrPt, DstStrChrSet, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, ... ) \
       _StrFmtCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), __VA_ARGS__ )

#define AU8strFmtCpy( AU8strPtType, AU8strPt, IsWriteEnd, AU8strLen, FmtStrPt, FmtStrChrSet, ... ) \
{ \
    U8strFmtCpy( , , IsWriteEnd, &AU8strLen, FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串的长度。*/ \
	AU8strPt = ( AU8strPtType * )alloca( AU8strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U8strFmtCpy( AU8strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

#define AU16strFmtCpy( AU16strPtType, AU16strPt, IsWriteEnd, AU16strLen, FmtStrPt, FmtStrChrSet, ... ) \
{ \
    U16strFmtCpy( , , IsWriteEnd, &AU16strLen, FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串的长度。*/ \
	AU16strPt = ( AU16strPtType )alloca( AU16strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U16strFmtCpy( AU16strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

#define AU32strFmtCpy( AU32strPtType, AU32strPt, IsWriteEnd, AU32strLen, FmtStrPt, FmtStrChrSet, ... ) \
{ \
    U32strFmtCpy( , , IsWriteEnd, &AU32strLen, FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串的长度。*/ \
	AU32strPt = ( AU32strPtType )alloca( AU32strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U32strFmtCpy( AU32strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

#define AstrFmtCpy( AstrPtType, AstrPt, AstrChrSet, IsWriteEnd, AstrLen, FmtStrPt, FmtStrChrSet, ... ) \
{ \
    StrFmtCpy( , AstrChrSet, , IsWriteEnd, &AstrLen, FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串的长度。*/ \
	AstrPt = ( AstrPtType )alloca( AstrLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	StrFmtCpy( AstrPt, AstrChrSet, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, __VA_ARGS__ ); /*设置栈字符串。*/ \
}

//将可变参数列表格式化复制到字符串。
__FUNC_DLLAPI__ int _U8strVaFmtCpy( uint8_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, va_list VaLst );
#define U8strVaFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, VaLst ) \
       _U8strVaFmtCpy( DEFARG( uint8_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), VaLst )

__FUNC_DLLAPI__ int _U16strVaFmtCpy( uint16_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, va_list VaLst );
#define U16strVaFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, VaLst ) \
       _U16strVaFmtCpy( DEFARG( uint16_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), VaLst )

__FUNC_DLLAPI__ int _U32strVaFmtCpy( uint32_t * DstStrPt, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, va_list VaLst );
#define U32strVaFmtCpy( DstStrPt, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, VaLst ) \
       _U32strVaFmtCpy( DEFARG( uint32_t *, DstStrPt, NULL ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), VaLst )

__FUNC_DLLAPI__ int _StrVaFmtCpy( void * DstStrPt, ChrSet DstStrChrSet, size_t DstStrSz, int32_t IsWriteEnd, size_t * DstStrLenPt, const void * FmtStrPt, ChrSet FmtStrChrSet, va_list VaLst );
#define StrVaFmtCpy( DstStrPt, DstStrChrSet, DstStrSz, IsWriteEnd, DstStrLenPt, FmtStrPt, FmtStrChrSet, VaLst ) \
       _StrVaFmtCpy( DEFARG( void *, DstStrPt, NULL ), DEFARG( ChrSet, DstStrChrSet, Utf8 ), DEFARG( size_t, DstStrSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstStrLenPt, NULL ), FmtStrPt, DEFARG( ChrSet, FmtStrChrSet, Utf8 ), VaLst )

#define AU8strVaFmtCpy( AU8strPtType, AU8strPt, IsWriteEnd, AU8strLen, FmtStrPt, FmtStrChrSet, VaLst ) \
{ \
    U8strVaFmtCpy( , , IsWriteEnd, &AU8strLen, FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串的长度。*/ \
	AU8strPt = ( AU8strPtType )alloca( AU8strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U8strVaFmtCpy( AU8strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串。*/ \
}

#define AU16strVaFmtCpy( AU16strPtType, AU16strPt, IsWriteEnd, AU16strLen, FmtStrPt, FmtStrChrSet, VaLst ) \
{ \
    U16strVaFmtCpy( , , IsWriteEnd, &AU16strLen, FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串的长度。*/ \
	AU16strPt = ( AU16strPtType )alloca( AU16strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U16strVaFmtCpy( AU16strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串。*/ \
}

#define AU32strVaFmtCpy( AU32strPtType, AU32strPt, IsWriteEnd, AU32strLen, FmtStrPt, FmtStrChrSet, VaLst ) \
{ \
    U32strVaFmtCpy( , , IsWriteEnd, &AU32strLen, FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串的长度。*/ \
	AU32strPt = ( AU32strPtType )alloca( AU32strLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	U32strVaFmtCpy( AU32strPt, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串。*/ \
}

#define AstrVaFmtCpy( AstrPtType, AstrPt, AstrChrSet, IsWriteEnd, AstrLen, FmtStrPt, FmtStrChrSet, VaLst ) \
{ \
    StrVaFmtCpy( , AstrChrSet, , IsWriteEnd, &AstrLen, FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串的长度。*/ \
	AstrPt = ( AstrPtType )alloca( AstrLen + IsWriteEnd ); /*设置栈字符串缓冲区的指针。*/ \
	StrVaFmtCpy( AstrPt, AstrChrSet, , IsWriteEnd, , FmtStrPt, FmtStrChrSet, VaLst ); /*设置栈字符串。*/ \
}

//将字符串格式化读取到变量。
__FUNC_DLLAPI__ void _StrFmtRead( const char * SrcStrPt, size_t SrcStrMaxLen, unsigned int * ReadCntPt, const char * FmtStrPt, ... );
#define StrFmtRead( SrcStrPt, SrcStrMaxLen, ReadCntPt, FmtStrPt, ... ) \
       _StrFmtRead( SrcStrPt, DEFARG( size_t, SrcStrMaxLen, SIZE_MAX ), DEFARG( unsigned int *, ReadCntPt, NULL ), FmtStrPt, __VA_ARGS__ )

#define StrFindChrs( StrPt, FindMemPt, FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt ) MemFindChrs( StrPt, strlen( StrPt ), FindMemPt, strlen( FindMemPt ), FindPos, IsBelong, MemFindedChrPtPt, FindedChrTotalPt )
__FUNC_DLLAPI__ int MemFindChrs( const char * MemPt, size_t MemLen, const char * FindMemPt, size_t FindMemLen, char FindPos, int IsBelong, const char * * MemFindedChrPtPt, size_t * FindedChrTotalPt );
#define StrFindStr( StrPt, FindStrPt, FindPos, FindedChrTotalPt ) MemFindMem( StrPt, strlen( StrPt ), FindStrPt, strlen( FindStrPt ), FindPos, FindedChrTotalPt )
__FUNC_DLLAPI__ int MemFindMem( const char * MemPt, size_t MemSz, const char * FindMemPt, size_t FindMemSz, const char * FindPos, size_t * FindedMemPosPt );
__FUNC_DLLAPI__ int MemSum( const void * MemPt, size_t MemSz, uint64_t * SumPt );
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

#endif
