#pragma once

#include "Func.h"

//Speex项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_SPEEX__ ) //如果正在编译Speex项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEX_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEX_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEX_DLLAPI__
		#endif
	#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_NDK__ ) || ( defined __HARMONY_NDK__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android NDK、Harmony NDK、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEX_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEX_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEX_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __SPEEX_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __SPEEX_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __SPEEX_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_NDK__ ) || ( defined __HARMONY_NDK__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android NDK、Harmony NDK、KEIL ARMCLANG/ARMCC编译器。
		#define __SPEEX_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __SPEEX_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct SpeexEncd SpeexEncd;

__SPEEX_DLLAPI__ int SpeexEncdInit( SpeexEncd * * SpeexEncdPtPt, int32_t SmplRate, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate );
__SPEEX_DLLAPI__ int SpeexEncdDstoy( SpeexEncd * SpeexEncdPt );
__SPEEX_DLLAPI__ int SpeexEncdPocs( SpeexEncd * SpeexEncdPt, int16_t * PcmFrmPt, uint8_t * SpeexFrmPt, size_t SpeexFrmSzByt, size_t * SpeexFrmLenBytPt, int32_t * IsNeedTransPt );

typedef struct SpeexDecd SpeexDecd;

__SPEEX_DLLAPI__ int SpeexDecdInit( SpeexDecd * * SpeexDecdPtPt, int32_t SmplRate, int32_t IsUsePrcplEnhsmt );
__SPEEX_DLLAPI__ int SpeexDecdDstoy( SpeexDecd * SpeexDecdPt );
__SPEEX_DLLAPI__ int SpeexDecdPocs( SpeexDecd * SpeexDecdPt, uint8_t * SpeexFrmPt, size_t SpeexFrmLenByt, int16_t * PcmFrmPt );

#ifdef __cplusplus
}
#endif
