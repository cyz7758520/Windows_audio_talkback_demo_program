#include "Func.h"

#ifndef __UTF_H__
#define __UTF_H__

#ifdef __cplusplus
extern "C"
{
#endif

//字符串复制函数应该从后面的参数向前面的参数复制的，因为这样可以满足根据动态参数复制的函数，同时也是strcpy函数、汇编mov指令的格式。

__FUNC_DLLAPI__ int _StrU8CpyU16( uint8_t * DstU8strPt, size_t DstU8strSz, int32_t IsWriteEnd, size_t * DstU8strLenPt, const uint16_t * SrcU16strPt, size_t SrcU16strSz, size_t * SrcU16strLenPt );
#define StrU8CpyU16( DstU8strPt, DstU8strSz, IsWriteEnd, DstU8strLenPt, SrcU16strPt, SrcU16strSz, SrcU16strLenPt ) \
       _StrU8CpyU16( DEFARG( uint8_t *, DstU8strPt, NULL ), DEFARG( size_t, DstU8strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU8strLenPt, NULL ), ( const uint16_t * )( SrcU16strPt ), DEFARG( size_t, SrcU16strSz, SIZE_MAX ), DEFARG( size_t *, SrcU16strLenPt, NULL ) )

__FUNC_DLLAPI__ int _StrU8CpyU32( uint8_t * DstU8strPt, size_t DstU8strSz, int32_t IsWriteEnd, size_t * DstU8strLenPt, const uint32_t * SrcU32strPt, size_t SrcU32strSz, size_t * SrcU32strLenPt );
#define StrU8CpyU32( DstU8strPt, DstU8strSz, IsWriteEnd, DstU8strLenPt, SrcU32strPt, SrcU32strSz, SrcU32strLenPt ) \
       _StrU8CpyU32( DEFARG( uint8_t *, DstU8strPt, NULL ), DEFARG( size_t, DstU8strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU8strLenPt, NULL ), ( const uint32_t * )( SrcU32strPt ), DEFARG( size_t, SrcU32strSz, SIZE_MAX ), DEFARG( size_t *, SrcU32strLenPt, NULL ) )

__FUNC_DLLAPI__ int _StrU16CpyU8( uint16_t * DstU16strPt, size_t DstU16strSz, int32_t IsWriteEnd, size_t * DstU16strLenPt, const uint8_t * SrcU8strPt, size_t SrcU8strSz, size_t * SrcU8strLenPt );
#define StrU16CpyU8( DstU16strPt, DstU16strSz, IsWriteEnd, DstU16strLenPt, SrcU8strPt, SrcU8strSz, SrcU8strLenPt ) \
       _StrU16CpyU8( DEFARG( uint16_t *, DstU16strPt, NULL ), DEFARG( size_t, DstU16strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU16strLenPt, NULL ), ( const uint8_t * )( SrcU8strPt ), DEFARG( size_t, SrcU8strSz, SIZE_MAX ), DEFARG( size_t *, SrcU8strLenPt, NULL ) )

__FUNC_DLLAPI__ int _StrU16CpyU32( uint16_t * DstU16strPt, size_t DstU16strSz, int32_t IsWriteEnd, size_t * DstU16strLenPt, const uint32_t * SrcU32strPt, size_t SrcU32strSz, size_t * SrcU32strLenPt );
#define StrU16CpyU32( DstU16strPt, DstU16strSz, IsWriteEnd, DstU16strLenPt, SrcU32strPt, SrcU32strSz, SrcU32strLenPt ) \
       _StrU16CpyU32( DEFARG( uint16_t *, DstU16strPt, NULL ), DEFARG( size_t, DstU16strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU16strLenPt, NULL ), ( const uint32_t * )( SrcU32strPt ), DEFARG( size_t, SrcU32strSz, SIZE_MAX ), DEFARG( size_t *, SrcU32strLenPt, NULL ) )

__FUNC_DLLAPI__ int _StrU32CpyU8( uint32_t * DstU32strPt, size_t DstU32strSz, int32_t IsWriteEnd, size_t * DstU32strLenPt, const uint8_t * SrcU8strPt, size_t SrcU8strSz, size_t * SrcU8strLenPt );
#define StrU32CpyU8( DstU32strPt, DstU32strSz, IsWriteEnd, DstU32strLenPt, SrcU8strPt, SrcU8strSz, SrcU8strLenPt ) \
       _StrU32CpyU8( DEFARG( uint32_t *, DstU32strPt, NULL ), DEFARG( size_t, DstU32strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU32strLenPt, NULL ), ( const uint8_t * )( SrcU8strPt ), DEFARG( size_t, SrcU8strSz, SIZE_MAX ), DEFARG( size_t *, SrcU8strLenPt, NULL ) )

__FUNC_DLLAPI__ int _StrU32CpyU16( uint32_t * DstU32strPt, size_t DstU32strSz, int32_t IsWriteEnd, size_t * DstU32strLenPt, const uint16_t * SrcU16strPt, size_t SrcU16strSz, size_t * SrcU16strLenPt );
#define StrU32CpyU16( DstU32strPt, DstU32strSz, IsWriteEnd, DstU32strLenPt, SrcU16strPt, SrcU16strSz, SrcU16strLenPt ) \
       _StrU32CpyU16( DEFARG( uint32_t *, DstU32strPt, NULL ), DEFARG( size_t, DstU32strSz, SIZE_MAX ), DEFARG( int32_t, IsWriteEnd, 1 ), DEFARG( size_t *, DstU32strLenPt, NULL ), ( const uint16_t * )( SrcU16strPt ), DEFARG( size_t, SrcU16strSz, SIZE_MAX ), DEFARG( size_t *, SrcU16strLenPt, NULL ) )

#define StrU8TmpCpy( U8TmpPt, SrcStrPt, SrcStrChrSet ) \
{ \
	size_t p_TmpSz_; \
	if( SrcStrChrSet == Utf8 ) \
	{ \
		U8TmpPt = ( uint8_t * )SrcStrPt; \
	} \
	else if( SrcStrChrSet == Utf16 ) \
	{ \
		StrU8CpyU16( , , , &p_TmpSz_, SrcStrPt, , ); \
		U8TmpPt = ( uint8_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint8_t ) ); \
		StrU8CpyU16( ( uint8_t * )U8TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
	else \
	{ \
		StrU8CpyU32( , , , &p_TmpSz_, SrcStrPt, , ); \
		U8TmpPt = ( uint8_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint8_t ) ); \
		StrU8CpyU32( ( uint8_t * )U8TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
}

#define StrU16TmpCpy( U16TmpPt, SrcStrPt, SrcStrChrSet ) \
{ \
	size_t p_TmpSz_; \
	if( SrcStrChrSet == Utf8 ) \
	{ \
		StrU16CpyU8( , , , &p_TmpSz_, SrcStrPt, , ); \
		U16TmpPt = ( uint16_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint16_t ) ); \
		StrU16CpyU8( ( uint16_t * )U16TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
	else if( SrcStrChrSet == Utf16 ) \
	{ \
		U16TmpPt = ( uint16_t * )SrcStrPt; \
	} \
	else \
	{ \
		StrU16CpyU32( , , , &p_TmpSz_, SrcStrPt, , ); \
		U16TmpPt = ( uint16_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint16_t ) ); \
		StrU16CpyU32( ( uint16_t * )U16TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
}

#define StrU32TmpCpy( U32TmpPt, SrcStrPt, SrcStrChrSet ) \
{ \
	size_t p_TmpSz_; \
	if( SrcStrChrSet == Utf8 ) \
	{ \
		StrU32CpyU8( , , , &p_TmpSz_, SrcStrPt, , ); \
		U32TmpPt = ( uint32_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint32_t ) ); \
		StrU32CpyU8( ( uint32_t * )U32TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
	else if( SrcStrChrSet == Utf16 ) \
	{ \
		StrU32CpyU16( , , , &p_TmpSz_, SrcStrPt, , ); \
		U32TmpPt = ( uint32_t * )alloca( ( p_TmpSz_ + 1 ) * sizeof( uint32_t ) ); \
		StrU32CpyU16( ( uint32_t * )U32TmpPt, p_TmpSz_ + 1, , , SrcStrPt, , ); \
	} \
	else \
	{ \
		U32TmpPt = ( uint32_t * )SrcStrPt; \
	} \
}

#ifdef __cplusplus
}
#endif

#endif
