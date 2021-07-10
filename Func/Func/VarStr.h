#ifndef __VARSTR_H__
#define __VARSTR_H__

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

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

//动态字符串。
typedef struct VarStr
{
	char * m_StrPt; //存放字符串的内存指针。
	size_t m_StrSz; //存放字符串的内存大小。
	size_t m_StrLen; //存放字符串的数据长度。
}VarStr;

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

__FUNC_DLLAPI__ int VarStrDestroy( VarStr * VarStrPt );

#ifdef __cplusplus
}
#endif

#endif
