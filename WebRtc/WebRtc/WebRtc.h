#ifndef __WEBRTC_H__
#define __WEBRTC_H__

#include "VarStr.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//WebRtc项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_WEBRTC__ ) //如果正在编译WebRtc项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WEBRTC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WEBRTC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WEBRTC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WEBRTC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WEBRTC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WEBRTC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __WEBRTC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __WEBRTC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __WEBRTC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __WEBRTC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __WEBRTC_DLLAPI__
	#endif
#endif
	
typedef struct WebRtcAecm WebRtcAecm;

__WEBRTC_DLLAPI__ int WebRtcAecmInit( WebRtcAecm * * WebRtcAecmPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay );
__WEBRTC_DLLAPI__ int WebRtcAecmSetDelay( WebRtcAecm * WebRtcAecmPt, int32_t Delay );
__WEBRTC_DLLAPI__ int WebRtcAecmGetDelay( WebRtcAecm * WebRtcAecmPt, int32_t * DelayPt );
__WEBRTC_DLLAPI__ int WebRtcAecmProc( WebRtcAecm * WebRtcAecmPt, int16_t * InputFramePt, int16_t * OutputFramePt, int16_t * ResultFramePt );
__WEBRTC_DLLAPI__ int WebRtcAecmDestroy( WebRtcAecm * WebRtcAecmPt );

typedef struct WebRtcAec WebRtcAec;

__WEBRTC_DLLAPI__ int WebRtcAecInit( WebRtcAec * * WebRtcAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay );
__WEBRTC_DLLAPI__ int WebRtcAecInitByMem( WebRtcAec * * WebRtcAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, char * WebRtcAecMemPt, size_t WebRtcAecMemLen );
__WEBRTC_DLLAPI__ int WebRtcAecInitByMemFile( WebRtcAec * * WebRtcAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, const char * WebRtcAecMemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetMemLen( WebRtcAec * WebRtcAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, size_t * WebRtcAecMemLenPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetMem( WebRtcAec * WebRtcAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, char * WebRtcAecMemPt, size_t WebRtcAecMemSz );
__WEBRTC_DLLAPI__ int WebRtcAecSaveMemFile( WebRtcAec * WebRtcAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, const char * WebRtcAecMemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
__WEBRTC_DLLAPI__ int WebRtcAecSetDelay( WebRtcAec * WebRtcAecPt, int32_t Delay );
__WEBRTC_DLLAPI__ int WebRtcAecGetDelay( WebRtcAec * WebRtcAecPt, int32_t * DelayPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetIsConvergence( WebRtcAec * WebRtcAecPt, int32_t * IsConvergencePt );
__WEBRTC_DLLAPI__ int WebRtcAecProc( WebRtcAec * WebRtcAecPt, int16_t * InputFramePt, int16_t * OutputFramePt, int16_t * ResultFramePt );
__WEBRTC_DLLAPI__ int WebRtcAecDestroy( WebRtcAec * WebRtcAecPt );

typedef struct WebRtcNsx WebRtcNsx;

__WEBRTC_DLLAPI__ int WebRtcNsxInit( WebRtcNsx * * WebRtcNsxPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t PolicyMode );
__WEBRTC_DLLAPI__ int WebRtcNsxProc( WebRtcNsx * WebRtcNsxPt, int16_t * FramePt, int16_t * ResultFramePt );
__WEBRTC_DLLAPI__ int WebRtcNsxDestroy( WebRtcNsx * WebRtcNsxPt );

typedef struct WebRtcNs WebRtcNs;

__WEBRTC_DLLAPI__ int WebRtcNsInit( WebRtcNs * * WebRtcNsPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t PolicyMode );
__WEBRTC_DLLAPI__ int WebRtcNsProc( WebRtcNs * WebRtcNsPt, int16_t * FramePt, int16_t * ResultFramePt );
__WEBRTC_DLLAPI__ int WebRtcNsDestroy( WebRtcNs * WebRtcNsPt );

__WEBRTC_DLLAPI__ int WebRtcResamplerInit( void * * WebRtcResamplerPtPt, int32_t BeforeSamplingRate, int32_t AfterSamplingRate );
__WEBRTC_DLLAPI__ int WebRtcResamplerProc( void * WebRtcResamplerPt, int16_t * BeforeFramePt, int32_t BeforeFrameLen, int16_t * AfterFramePt, size_t AfterFrameSz, int32_t * AfterFrameLenPt );
__WEBRTC_DLLAPI__ int WebRtcResamplerDestroy( void * WebRtcResamplerPt );

#ifdef __cplusplus
}
#endif

#endif