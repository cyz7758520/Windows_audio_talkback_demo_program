#include "Func.h"

#ifndef __AUDIOOSCILLO_H__
#define __AUDIOOSCILLO_H__

//AudioOscillo项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_AUDIOOSCILLO__ ) //如果正在编译AudioOscillo项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AUDIOOSCILLO_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AUDIOOSCILLO_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AUDIOOSCILLO_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AUDIOOSCILLO_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AUDIOOSCILLO_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AUDIOOSCILLO_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __AUDIOOSCILLO_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __AUDIOOSCILLO_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __AUDIOOSCILLO_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __AUDIOOSCILLO_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __AUDIOOSCILLO_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AudioOscillo AudioOscillo;

__AUDIOOSCILLO_DLLAPI__ int AudioOscilloInit( AudioOscillo * * AudioOscilloPtPt, VarStr * ErrInfoVarStrPt );
__AUDIOOSCILLO_DLLAPI__ int AudioOscilloDrawToWnd( AudioOscillo * AudioOscilloPt, int16_t * PcmFramePt, int FrameLen, HWND DstWndHdl, VarStr * ErrInfoVarStrPt );
__AUDIOOSCILLO_DLLAPI__ int AudioOscilloDstoy( AudioOscillo * AudioOscilloPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif