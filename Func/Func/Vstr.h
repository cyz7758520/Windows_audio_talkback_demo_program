#include "Func.h"

#ifndef __VARSTR_H__
#define __VARSTR_H__

//动态字符串。
typedef struct Vstr
{
#ifdef __cplusplus
public:
	Vstr( void * StrPt, ChrSet ChrSet_, size_t LenChr ) :m_Pt( StrPt ), m_ChrSet( ChrSet_ ), m_SzChr( LenChr + 1 ), m_LenChr( LenChr ){}
#endif
	void * m_Pt; //存放指针。
	ChrSet m_ChrSet; //存放字符集。
	size_t m_SzChr; //存放大小，单位为字符。
	size_t m_LenChr; //存放长度，单位为字符。
}Vstr;

#ifdef __cplusplus
extern "C"
{
#endif
	
__FUNC_DLLAPI__ int _VstrInit( Vstr * * VstrPtPt, ChrSet VstrChrSet, size_t VstrSzChr, const Vstr * SrcVstrPt );
#define VstrInit( VstrPtPt, VstrChrSet, VstrSzChr, SrcVstrPt ) \
       _VstrInit( VstrPtPt, DEFARG( ChrSet, VstrChrSet, Utf8 ), DEFARG( size_t, VstrSzChr, sizeof( size_t ) ), DEFARG( const Vstr *, SrcVstrPt, NULL ) )


__FUNC_DLLAPI__ int _VstrCpy( Vstr * VstrPt, const Vstr * SrcVstrPt, size_t SrcPosChr, size_t SrcMaxLenChr );
#define VstrCpy( VstrPt, SrcVstrPt, SrcPosChr, SrcMaxLenChr ) \
       _VstrCpy( VstrPt, SrcVstrPt, DEFARG( size_t, SrcPosChr, 0 ), DEFARG( size_t, SrcMaxLenChr, SIZE_MAX ) ) 

__FUNC_DLLAPI__ int VstrFmtCpy( Vstr * VstrPt, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtCpy( Vstr * VstrPt, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrIns( Vstr * VstrPt, size_t PosChr, const Vstr * SrcVstrPt );

__FUNC_DLLAPI__ int VstrFmtIns( Vstr * VstrPt, size_t PosChr, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtIns( Vstr * VstrPt, size_t PosChr, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrCat( Vstr * VstrPt, const Vstr * SrcVstrPt );

__FUNC_DLLAPI__ int VstrFmtCat( Vstr * VstrPt, const Vstr * FmtVstrPt, ... );

__FUNC_DLLAPI__ int VstrVaFmtCat( Vstr * VstrPt, const Vstr * FmtVstrPt, va_list VaLst );


__FUNC_DLLAPI__ int VstrSetEmpty( Vstr * VstrPt );

__FUNC_DLLAPI__ int VstrSetSz( Vstr * VstrPt, size_t VstrSzChr );

__FUNC_DLLAPI__ int VstrGetSz( Vstr * VstrPt, size_t * VstrSzChrPt );


__FUNC_DLLAPI__ int VstrCmp( const Vstr * VstrPt, const Vstr * CmpVstrPt, int * CmpRsltPt );


__FUNC_DLLAPI__ int VstrFindChr( const Vstr * VstrPt, size_t PosChr, const Vstr * FindChrVstrPt, size_t * FindPosChrPt );

__FUNC_DLLAPI__ int VstrIsBelongChr( const Vstr * VstrPt, size_t PosChr, const Vstr * FindChrVstrPt, int IsBelong, size_t * LenChrPt );

__FUNC_DLLAPI__ int VstrFindStr( const Vstr * VstrPt, size_t PosChr, const Vstr * FindStrVstrPt, size_t * FindPosChrPt );

__FUNC_DLLAPI__ int VstrSubStr( const Vstr * VstrPt, size_t PosChr, size_t LenChr, const Vstr * SubStrVstrPt );

__FUNC_DLLAPI__ int VstrUrlParse( const Vstr * VstrPt, Vstr * PrtclVstrPt, Vstr * UsernameVstrPt, Vstr * PasswordVstrPt, Vstr * HostnameVstrPt, Vstr * PortVstrPt, Vstr * PathVstrPt, Vstr * ParmVstrPt, Vstr * QueryVstrPt, Vstr * FragmentVstrPt, Vstr * ErrInfoVstrPt );


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

	int Init( ChrSet VstrChrSet = Utf8, size_t VstrSzChr = sizeof( size_t ), const Vstr * SrcVstrPt = NULL ) { return VstrInit( &m_VstrPt, VstrChrSet, VstrSzChr, SrcVstrPt ); }
	
	int Cpy( const Vstr * SrcVstrPt, size_t SrcPosChr = 0, size_t SrcMaxLenChr = SIZE_MAX ) { return VstrCpy( m_VstrPt, SrcVstrPt, SrcPosChr, SrcMaxLenChr ); }
	int FmtCpy( const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtCpy( m_VstrPt, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCpy( const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtCpy( m_VstrPt, FmtVstrPt, VaLst ); }

	int Ins( size_t Pos, const Vstr * SrcVstrPt ) { return VstrIns( m_VstrPt, Pos, SrcVstrPt ); }
	int FmtIns( size_t Pos, const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtIns( m_VstrPt, Pos, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtIns( size_t Pos, const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtIns( m_VstrPt, Pos, FmtVstrPt, VaLst ); }

	int Cat( const Vstr * SrcVstrPt ) { return VstrCat( m_VstrPt, SrcVstrPt ); }
	int FmtCat( const Vstr * FmtVstrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtVstrPt ); int p_Rslt = VstrVaFmtCat( m_VstrPt, FmtVstrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCat( const Vstr * FmtVstrPt, va_list VaLst ) { return VstrVaFmtCat( m_VstrPt, FmtVstrPt, VaLst ); }

	int SetEmpty() { return VstrSetEmpty( m_VstrPt ); }
	int SetSz( size_t VstrSzChr ) { return VstrSetSz( m_VstrPt, VstrSzChr ); }
	int GetSz( size_t * VstrSzChrPt ) { return VstrGetSz( m_VstrPt, VstrSzChrPt ); }

	int Cmp( const Vstr * CmpVstrPt, int * CmpRsltPt ) { return VstrCmp( m_VstrPt, CmpVstrPt, CmpRsltPt ); }

	int FindChr( size_t PosChr, const Vstr * FindChrVstrPt, size_t * FindPosChrPt ) { return VstrFindChr( m_VstrPt, PosChr, FindChrVstrPt, FindPosChrPt ); }
	int IsBelongChr( size_t PosChr, const Vstr * FindChrVstrPt, int IsBelong, size_t * LenChrPt ) { return VstrIsBelongChr( m_VstrPt, PosChr, FindChrVstrPt, IsBelong, LenChrPt ); }
	int FindStr( size_t PosChr, const Vstr * FindStrVstrPt, size_t * FindPosChrPt ) { return VstrFindStr( m_VstrPt, PosChr, FindStrVstrPt, FindPosChrPt ); }
	int SubStr( size_t PosChr, size_t LenChr, const Vstr * SubStrVstrPt ) { return VstrSubStr( m_VstrPt, PosChr, LenChr, SubStrVstrPt ); }
	int UrlParse( Vstr * PrtclVstrPt, Vstr * UsernameVstrPt, Vstr * PasswordVstrPt, Vstr * HostnameVstrPt, Vstr * PortVstrPt, Vstr * PathVstrPt, Vstr * ParmVstrPt, Vstr * QueryVstrPt, Vstr * FragmentVstrPt, VstrCls * ErrInfoVstrPt ) { return VstrUrlParse( m_VstrPt, PrtclVstrPt, UsernameVstrPt, PasswordVstrPt, HostnameVstrPt, PortVstrPt, PathVstrPt, ParmVstrPt, QueryVstrPt, FragmentVstrPt, ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL ); }

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
//使用Cu8vstr和Cu8avstr宏必须保证U8strPt以"\0"结尾。
//使用Cu16vstr和Cu16avstr宏必须保证U8strPt以"\0\0"结尾。
//使用Cu32vstr和Cu32avstr宏必须保证U8strPt以"\0\0\0\0"结尾。
#ifndef __cplusplus
#define Cu8vstr( U8strPt ) &( ( Vstr ){ .m_Pt = ( U8strPt ), .m_ChrSet = Utf8, .m_SzChr = SIZE_MAX / Utf8, .m_LenChr = StrLen( ( U8strPt ), Utf8, , ) } )
#else
#define Cu8vstr( U8strPt ) VstrRefToPt( Vstr( ( void * )( U8strPt ), Utf8, StrLen( ( U8strPt ), Utf8, , ) ) )
#define Cu8avstr( AvstrPt, U8strPt ) \
{ \
	AvstrPt = ( Vstr * )alloca( sizeof( Vstr ) ); \
	( ( Vstr * )AvstrPt )->m_Pt = ( void * )( U8strPt ); \
	( ( Vstr * )AvstrPt )->m_ChrSet = Utf8; \
	( ( Vstr * )AvstrPt )->m_LenChr = StrLen( ( U8strPt ), Utf8, , ); \
	( ( Vstr * )AvstrPt )->m_SzChr = ( ( Vstr * )AvstrPt )->m_LenChr + 1; \
}
#endif

#ifndef __cplusplus
#define Cu16vstr( U16strPt ) &( ( Vstr ){ .m_Pt = ( U16strPt ), .m_ChrSet = Utf16, .m_SzChr = SIZE_MAX / Utf16, .m_LenChr = StrLen( ( U16strPt ), Utf16, , ) } )
#else
#define Cu16vstr( U16strPt ) VstrRefToPt( Vstr( ( void * )( U16strPt ), Utf16, StrLen( ( U16strPt ), Utf16, , ) ) )
#define Cu16avstr( AvstrPt, U16strPt ) \
{ \
	AvstrPt = ( Vstr * )alloca( sizeof( Vstr ) ); \
	( ( Vstr * )AvstrPt )->m_Pt = ( void * )( U16strPt ); \
	( ( Vstr * )AvstrPt )->m_ChrSet = Utf16; \
	( ( Vstr * )AvstrPt )->m_LenChr = StrLen( ( U16strPt ), Utf16, , ); \
	( ( Vstr * )AvstrPt )->m_SzChr = ( ( Vstr * )AvstrPt )->m_LenChr + 1; \
}
#endif

#ifndef __cplusplus
#define Cu32vstr( U32strPt ) &( ( Vstr ){ .m_Pt = ( U32strPt ), .m_ChrSet = Utf32, .m_SzChr = SIZE_MAX / Utf32, .m_LenChr = StrLen( ( U32strPt ), Utf32, , ) } )
#else
#define Cu32vstr( U32strPt ) VstrRefToPt( Vstr( ( void * )( U32strPt ), Utf32, StrLen( ( U32strPt ), Utf32, , ) ) )
#define Cu32avstr( AvstrPt, U32strPt ) \
{ \
	AvstrPt = ( Vstr * )alloca( sizeof( Vstr ) ); \
	( ( Vstr * )AvstrPt )->m_Pt = ( void * )( U32strPt ); \
	( ( Vstr * )AvstrPt )->m_ChrSet = Utf32; \
	( ( Vstr * )AvstrPt )->m_LenChr = StrLen( ( U32strPt ), Utf32, , ); \
	( ( Vstr * )AvstrPt )->m_SzChr = ( ( Vstr * )AvstrPt )->m_LenChr + 1; \
}
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
