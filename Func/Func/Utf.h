#include "Func.h"

#ifndef __UTF_H__
#define __UTF_H__

#ifdef __cplusplus
extern "C"
{
#endif

//字符转字符。
__FUNC_DLLAPI__ int ChrToChr( const void * SrcChrPt, ChrSet SrcChrSet, size_t SrcChrSzChr, size_t * SrcChrLenChrPt, void * DstChrPt, ChrSet DstChrSet, size_t DstChrSzChr, size_t * DstChrLenChrPt );

//源始字符串复制到目的字符串。应该是从后面的参数向前面的参数复制的，因为这样可以满足根据动态参数复制的函数，同时也是strcpy函数、汇编mov指令的格式。

__FUNC_DLLAPI__ int _StrU8CpyU16( uint8_t * DstU8strPt, size_t DstU8strSzChr, int32_t IsWriteEnd, size_t * DstU8strLenChrPt, const uint16_t * SrcU16strPt, size_t SrcU16strSzChr, size_t * SrcU16strLenChrPt );
#define StrU8CpyU16( DstU8strPt, DstU8strSzChr, IsWriteEnd, DstU8strLenChrPt, SrcU16strPt, SrcU16strSzChr, SrcU16strLenChrPt ) \
       _StrU8CpyU16( DEFARG( uint8_t *, DstU8strPt, NULL ), DEFARG( size_t, DstU8strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU8strLenChrPt, NULL ), ( const uint16_t * )( SrcU16strPt ), DEFARG( size_t, SrcU16strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU16strLenChrPt, NULL ) )

__FUNC_DLLAPI__ int _StrU8CpyU32( uint8_t * DstU8strPt, size_t DstU8strSzChr, int32_t IsWriteEnd, size_t * DstU8strLenChrPt, const uint32_t * SrcU32strPt, size_t SrcU32strSzChr, size_t * SrcU32strLenChrPt );
#define StrU8CpyU32( DstU8strPt, DstU8strSzChr, IsWriteEnd, DstU8strLenChrPt, SrcU32strPt, SrcU32strSzChr, SrcU32strLenChrPt ) \
       _StrU8CpyU32( DEFARG( uint8_t *, DstU8strPt, NULL ), DEFARG( size_t, DstU8strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU8strLenChrPt, NULL ), ( const uint32_t * )( SrcU32strPt ), DEFARG( size_t, SrcU32strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU32strLenChrPt, NULL ) )

__FUNC_DLLAPI__ int _StrU16CpyU8( uint16_t * DstU16strPt, size_t DstU16strSzChr, int32_t IsWriteEnd, size_t * DstU16strLenChrPt, const uint8_t * SrcU8strPt, size_t SrcU8strSzChr, size_t * SrcU8strLenChrPt );
#define StrU16CpyU8( DstU16strPt, DstU16strSzChr, IsWriteEnd, DstU16strLenChrPt, SrcU8strPt, SrcU8strSzChr, SrcU8strLenChrPt ) \
       _StrU16CpyU8( DEFARG( uint16_t *, DstU16strPt, NULL ), DEFARG( size_t, DstU16strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU16strLenChrPt, NULL ), ( const uint8_t * )( SrcU8strPt ), DEFARG( size_t, SrcU8strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU8strLenChrPt, NULL ) )

__FUNC_DLLAPI__ int _StrU16CpyU32( uint16_t * DstU16strPt, size_t DstU16strSzChr, int32_t IsWriteEnd, size_t * DstU16strLenChrPt, const uint32_t * SrcU32strPt, size_t SrcU32strSzChr, size_t * SrcU32strLenChrPt );
#define StrU16CpyU32( DstU16strPt, DstU16strSzChr, IsWriteEnd, DstU16strLenChrPt, SrcU32strPt, SrcU32strSzChr, SrcU32strLenChrPt ) \
       _StrU16CpyU32( DEFARG( uint16_t *, DstU16strPt, NULL ), DEFARG( size_t, DstU16strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU16strLenChrPt, NULL ), ( const uint32_t * )( SrcU32strPt ), DEFARG( size_t, SrcU32strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU32strLenChrPt, NULL ) )

__FUNC_DLLAPI__ int _StrU32CpyU8( uint32_t * DstU32strPt, size_t DstU32strSzChr, int32_t IsWriteEnd, size_t * DstU32strLenChrPt, const uint8_t * SrcU8strPt, size_t SrcU8strSzChr, size_t * SrcU8strLenChrPt );
#define StrU32CpyU8( DstU32strPt, DstU32strSzChr, IsWriteEnd, DstU32strLenChrPt, SrcU8strPt, SrcU8strSzChr, SrcU8strLenChrPt ) \
       _StrU32CpyU8( DEFARG( uint32_t *, DstU32strPt, NULL ), DEFARG( size_t, DstU32strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU32strLenChrPt, NULL ), ( const uint8_t * )( SrcU8strPt ), DEFARG( size_t, SrcU8strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU8strLenChrPt, NULL ) )

__FUNC_DLLAPI__ int _StrU32CpyU16( uint32_t * DstU32strPt, size_t DstU32strSzChr, int32_t IsWriteEnd, size_t * DstU32strLenChrPt, const uint16_t * SrcU16strPt, size_t SrcU16strSzChr, size_t * SrcU16strLenChrPt );
#define StrU32CpyU16( DstU32strPt, DstU32strSzChr, IsWriteEnd, DstU32strLenChrPt, SrcU16strPt, SrcU16strSzChr, SrcU16strLenChrPt ) \
       _StrU32CpyU16( DEFARG( uint32_t *, DstU32strPt, NULL ), DEFARG( size_t, DstU32strSzChr, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU32strLenChrPt, NULL ), ( const uint16_t * )( SrcU16strPt ), DEFARG( size_t, SrcU16strSzChr, SIZE_MAX ), DEFARG( size_t *, SrcU16strLenChrPt, NULL ) )

#ifdef __cplusplus
}
#endif

#endif
