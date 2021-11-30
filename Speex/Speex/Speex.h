#include "Func.h"

#ifndef __SPEEX_H__
#define __SPEEX_H__

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
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
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
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __SPEEX_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __SPEEX_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct SpeexEncoder SpeexEncoder;

__SPEEX_DLLAPI__ int SpeexEncoderInit( SpeexEncoder * * SpeexEncoderPtPt, int32_t SamplingRate, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExpectedLossRate );
__SPEEX_DLLAPI__ int SpeexEncoderPocs( SpeexEncoder * SpeexEncoderPt, int16_t * PcmFramePt, uint8_t * SpeexFramePt, size_t SpeexFrameSz, size_t * SpeexFrameLenPt, int32_t * IsNeedTransPt );
__SPEEX_DLLAPI__ int SpeexEncoderDstoy( SpeexEncoder * SpeexEncoderPt );

typedef struct SpeexDecoder SpeexDecoder;

__SPEEX_DLLAPI__ int SpeexDecoderInit( SpeexDecoder * * SpeexDecoderPtPt, int32_t SamplingRate, int32_t IsUsePerceptualEnhancement );
__SPEEX_DLLAPI__ int SpeexDecoderPocs( SpeexDecoder * SpeexDecoderPt, uint8_t * SpeexFramePt, size_t SpeexFrameLen, int16_t * PcmFramePt );
__SPEEX_DLLAPI__ int SpeexDecoderDstoy( SpeexDecoder * SpeexDecoderPt );

#ifdef __cplusplus
}
#endif

#endif