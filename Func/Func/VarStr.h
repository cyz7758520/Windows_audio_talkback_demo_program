#include "Func.h"

#ifndef __VARSTR_H__
#define __VARSTR_H__

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

//动态字符串。
typedef struct VarStr
{
	char * m_StrPt; //存放字符串的内存指针。
	size_t m_StrSz; //存放字符串的内存大小。
	size_t m_StrLen; //存放字符串的数据长度。
}VarStr;

#ifdef __cplusplus
extern "C"
{
#endif
	
#ifdef __cplusplus
__FUNC_DLLAPI__ int VarStrInit( VarStr * * VarStrPtPt, size_t StrSz = sizeof( size_t ) );
#else
__FUNC_DLLAPI__ int VarStrInit( VarStr * * VarStrPtPt, size_t StrSz );
#endif
#ifdef __cplusplus
__FUNC_DLLAPI__ int VarStrInitByStr( VarStr * * VarStrPtPt, const char * StrPt, size_t MaxCpyLen = SIZE_MAX );
#else
__FUNC_DLLAPI__ int VarStrInitByStr( VarStr * * VarStrPtPt, const char * StrPt, size_t MaxCpyLen );
#endif
#define VarStrInitByVarStr( VarStrPtPt, VarStrPt ) VarStrInitByStr( VarStrPtPt, VarStrPt->m_StrPt, VarStrPt->m_StrLen )

#ifdef __cplusplus
__FUNC_DLLAPI__ int VarStrCpy( VarStr * VarStrPt, const char * StrPt, size_t MaxCpyLen = SIZE_MAX );
#else
__FUNC_DLLAPI__ int VarStrCpy( VarStr * VarStrPt, const char * StrPt, size_t MaxCpyLen );
#endif
__FUNC_DLLAPI__ int VarStrFmtCpy( VarStr * VarStrPt, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int VarStrVaFmtCpy( VarStr * VarStrPt, const char * FmtStrPt, va_list VaLst );

#ifdef __cplusplus
__FUNC_DLLAPI__ int VarStrIns( VarStr * VarStrPt, size_t Pos, const char * StrPt, size_t MaxInsLen = SIZE_MAX );
#else
__FUNC_DLLAPI__ int VarStrIns( VarStr * VarStrPt, size_t Pos, const char * StrPt, size_t MaxInsLen );
#endif
__FUNC_DLLAPI__ int VarStrFmtIns( VarStr * VarStrPt, size_t Pos, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int VarStrVaFmtIns( VarStr * VarStrPt, size_t Pos, const char * FmtStrPt, va_list VaLst );

#ifdef __cplusplus
__FUNC_DLLAPI__ int VarStrCat( VarStr * VarStrPt, const char * StrPt, size_t MaxCatLen = SIZE_MAX );
#else
__FUNC_DLLAPI__ int VarStrCat( VarStr * VarStrPt, const char * StrPt, size_t MaxCatLen );
#endif
__FUNC_DLLAPI__ int VarStrFmtCat( VarStr * VarStrPt, const char * FmtStrPt, ... );
__FUNC_DLLAPI__ int VarStrVaFmtCat( VarStr * VarStrPt, const char * FmtStrPt, va_list VaLst );

#define VarStrSetEmpty( VarStrPt ) VarStrCpy( VarStrPt, "", 0 )
__FUNC_DLLAPI__ int VarStrSetSz( VarStr * VarStrPt, size_t StrSz );
__FUNC_DLLAPI__ int VarStrGetSz( VarStr * VarStrPt, size_t * StrSzPt );

__FUNC_DLLAPI__ int VarStrReSetLen( VarStr * VarStrPt );

__FUNC_DLLAPI__ int VarStrDstoy( VarStr * VarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarStrCls
{
public:
	VarStr * m_VarStrPt;

	VarStrCls() { m_VarStrPt = NULL; }
	~VarStrCls() { Dstoy(); }

	int Init( size_t StrSz = sizeof( size_t ) ) { return VarStrInit( &m_VarStrPt, StrSz ); }
	int InitByStr( const char * StrPt, size_t MaxCpyLen = SIZE_MAX ) { return VarStrInitByStr( &m_VarStrPt, StrPt, MaxCpyLen ); }
	int InitByVarStr( VarStrCls * VarStrPt ) { return VarStrInitByVarStr( &m_VarStrPt, VarStrPt->m_VarStrPt ); }

	int Cpy( const char * StrPt, size_t MaxCpyLen = SIZE_MAX ) { return VarStrCpy( m_VarStrPt, StrPt, MaxCpyLen ); }
	int FmtCpy( const char * FmtStrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtStrPt ); int p_Rslt = VarStrVaFmtCpy( m_VarStrPt, FmtStrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCpy( const char * FmtStrPt, va_list VaLst ) { return VarStrVaFmtCpy( m_VarStrPt, FmtStrPt, VaLst ); }

	int Ins( size_t Pos, const char * StrPt, size_t MaxInsLen = SIZE_MAX ) { return VarStrIns( m_VarStrPt, Pos, StrPt, MaxInsLen ); }
	int FmtIns( size_t Pos, const char * FmtStrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtStrPt ); int p_Rslt = VarStrVaFmtIns( m_VarStrPt, Pos, FmtStrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtIns( size_t Pos, const char * FmtStrPt, va_list VaLst ) { return VarStrVaFmtIns( m_VarStrPt, Pos, FmtStrPt, VaLst ); }

	int Cat( const char * StrPt, size_t MaxCatLen = SIZE_MAX ) { return VarStrCat( m_VarStrPt, StrPt, MaxCatLen ); }
	int FmtCat( const char * FmtStrPt, ... ) { va_list p_VaLst; va_start( p_VaLst, FmtStrPt ); int p_Rslt = VarStrVaFmtCat( m_VarStrPt, FmtStrPt, p_VaLst ); va_end( p_VaLst ); return p_Rslt; }
	int VaFmtCat( const char * FmtStrPt, va_list VaLst ) { return VarStrVaFmtCat( m_VarStrPt, FmtStrPt, VaLst ); }

	int SetEmpty() { return VarStrSetEmpty( m_VarStrPt ); }
	int SetSz( size_t StrSz ) { return VarStrSetSz( m_VarStrPt, StrSz ); }
	int GetSz( size_t * StrSzPt ) { return VarStrGetSz( m_VarStrPt, StrSzPt ); }

	int ReSetLen() { return VarStrReSetLen( m_VarStrPt ); }

	int Dstoy() { int p_Rslt = VarStrDstoy( m_VarStrPt ); m_VarStrPt = NULL; return p_Rslt; }
};
#endif

#endif
