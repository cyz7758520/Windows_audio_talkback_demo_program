#include "Func.h"

#ifndef __VARSTR_H__
#define __VARSTR_H__

//动态字符串。
typedef struct Vstr
{
#ifdef __cplusplus
public:
	Vstr( void * StrPt, ChrSet StrChrSet, size_t StrLen ) :m_StrPt( StrPt ), m_StrChrSet( StrChrSet ), m_StrSz( StrLen + 1 ), m_StrLen( StrLen ){}
#endif
	void * m_StrPt; //存放字符串的指针。
	ChrSet m_StrChrSet; //存放字符串的字符集。
	size_t m_StrSz; //存放字符串的大小，单位为字符。
	size_t m_StrLen; //存放字符串的长度，单位为字符。
}Vstr;

#ifdef __cplusplus
extern "C"
{
#endif
	
__FUNC_DLLAPI__ int _VstrInit( Vstr * * VstrPtPt, ChrSet VstrChrSet, size_t VstrSz, const Vstr * SrcVstrPt );
#define VstrInit( VstrPtPt, StrChrSet, VstrSz, SrcVstrPt ) \
       _VstrInit( VstrPtPt, DEFARG( ChrSet, StrChrSet, Utf8 ), DEFARG( size_t, VstrSz, sizeof( size_t ) ), DEFARG( const Vstr *, SrcVstrPt, NULL ) )


__FUNC_DLLAPI__ int VstrCpy( Vstr * VstrPt, const Vstr * SrcVstrPt );

__FUNC_DLLAPI__ int VstrFmtCpy( Vstr * VstrPt, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtCpy( Vstr * VstrPt, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrIns( Vstr * VstrPt, size_t Pos, const Vstr * SrcVstrPt );

__FUNC_DLLAPI__ int VstrFmtIns( Vstr * VstrPt, size_t Pos, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtIns( Vstr * VstrPt, size_t Pos, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrCat( Vstr * VstrPt, const Vstr * SrcVstrPt );

__FUNC_DLLAPI__ int VstrFmtCat( Vstr * VstrPt, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtCat( Vstr * VstrPt, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrSetEmpty( Vstr * VstrPt );

__FUNC_DLLAPI__ int VstrSetSz( Vstr * VstrPt, size_t StrSz );

__FUNC_DLLAPI__ int VstrGetSz( Vstr * VstrPt, size_t * StrSzPt );


__FUNC_DLLAPI__ int VstrDstoy( Vstr * VstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VstrCls
{
public:
	Vstr * m_VstrPt;

	VstrCls() { m_VstrPt = NULL; }
	~VstrCls() { Dstoy(); }

	int Init( ChrSet StrChrSet = Utf8, size_t StrSz = sizeof( size_t ), const Vstr * SrcVstrPt = NULL ) { return VstrInit( &m_VstrPt, StrChrSet, StrSz, SrcVstrPt ); }
	
	int Cpy( const Vstr * SrcVstrPt ) { return VstrCpy( m_VstrPt, SrcVstrPt ); }
	int FmtCpy( const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtCpy( m_VstrPt, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCpy( const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtCpy( m_VstrPt, FmtVstrPt, VaLst ); }

	int Ins( size_t Pos, const Vstr * SrcVstrPt ) { return VstrIns( m_VstrPt, Pos, SrcVstrPt ); }
	int FmtIns( size_t Pos, const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtIns( m_VstrPt, Pos, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtIns( size_t Pos, const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtIns( m_VstrPt, Pos, FmtVstrPt, VaLst ); }

	int Cat( const Vstr * SrcVstrPt ) { return VstrCat( m_VstrPt, SrcVstrPt ); }
	int FmtCat( const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtCat( m_VstrPt, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCat( const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtCat( m_VstrPt, FmtVstrPt, VaLst ); }

	int SetEmpty() { return VstrSetEmpty( m_VstrPt ); }
	int SetSz( size_t StrSz ) { return VstrSetSz( m_VstrPt, StrSz ); }
	int GetSz( size_t * StrSzPt ) { return VstrGetSz( m_VstrPt, StrSzPt ); }

	int Dstoy() { int p_Rslt = VstrDstoy( m_VstrPt ); m_VstrPt = NULL; return p_Rslt; }
};
#else
typedef Vstr * VstrCls;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//将普通字符串的指针转换为动态字符串常量，不是动态字符串指针。这里不要使用wcslen函数计算字符串的长度，因为Linux的GCC下计算出长度会有错误。
#ifndef __cplusplus
#define Cu8vstr( U8strPt ) &( ( Vstr ){ .m_StrPt = U8strPt, .m_StrChrSet = Utf8, .m_StrSz = SIZE_MAX / Utf8, .m_StrLen = U8strLen( U8strPt, , ) } )
#else
#define Cu8vstr( U8strPt ) VstrRefToPt( Vstr( ( void * )U8strPt, Utf8, U8strLen( U8strPt, , ) ) )
#endif

#ifndef __cplusplus
#define Cu16vstr( U16strPt ) &( ( Vstr ){ .m_StrPt = U16strPt, .m_StrChrSet = Utf16, .m_StrSz = SIZE_MAX / Utf16, .m_StrLen = U16strLen( U16strPt, , ) } )
#else
#define Cu16vstr( U16strPt ) VstrRefToPt( Vstr( ( void * )U16strPt, Utf16, U16strLen( U16strPt, , ) ) )
#endif

#ifndef __cplusplus
#define Cu32vstr( U32strPt ) &( ( Vstr ){ .m_StrPt = U32strPt, .m_StrChrSet = Utf32, .m_StrSz = SIZE_MAX / Utf32, .m_StrLen = U32strLen( U32strPt, , ) } )
#else
#define Cu32vstr( U32strPt ) VstrRefToPt( Vstr( ( void * )U32strPt, Utf32, U32strLen( U32strPt, , ) ) )
#endif

#ifdef __cplusplus
inline Vstr * VstrRefToPt( const Vstr & VstrRef )
{
	return ( Vstr * )&VstrRef;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
