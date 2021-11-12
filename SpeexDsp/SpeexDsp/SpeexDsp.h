#include "Func.h"

#ifndef __SPEEXDSP_H__
#define __SPEEXDSP_H__

//SpeexDsp项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_SPEEXDSP__ ) //如果正在编译SpeexDsp项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEXDSP_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEXDSP_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEXDSP_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEXDSP_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEXDSP_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEXDSP_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __SPEEXDSP_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __SPEEXDSP_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __SPEEXDSP_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __SPEEXDSP_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __SPEEXDSP_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct SpeexAec SpeexAec;

__SPEEXDSP_DLLAPI__ int SpeexAecInit( SpeexAec * * SpeexAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecInitByMem( SpeexAec * * SpeexAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const char * SpeexAecMemPt, size_t SpeexAecMemLen, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecInitByMemFile( SpeexAec * * SpeexAecPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const char * SpeexAecMemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetMemLen( SpeexAec * SpeexAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, size_t * SpeexAecMemLenPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetMem( SpeexAec * SpeexAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, char * SpeexAecMemPt, size_t SpeexAecMemSz );
__SPEEXDSP_DLLAPI__ int SpeexAecSaveMemFile( SpeexAec * SpeexAecPt, int32_t SamplingRate, int32_t FrameLen, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const char * SpeexAecMemFileFullPathStrPt, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetIsCnvgnc( SpeexAec * SpeexAecPt, int32_t * IsCnvgncPt );
__SPEEXDSP_DLLAPI__ int SpeexAecProc( SpeexAec * SpeexAecPt, int16_t * InputFramePt, int16_t * OutputFramePt, int16_t * ResultFramePt );
__SPEEXDSP_DLLAPI__ int SpeexAecDestroy( SpeexAec * SpeexAecPt );

typedef struct SpeexPreprocessState_ SpeexPreprocessState;

__SPEEXDSP_DLLAPI__ int SpeexPprocInit( SpeexPreprocessState * * SpeexPprocPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexPprocProc( SpeexPreprocessState * SpeexPprocPt, int16_t * FramePt, int16_t * ResultFramePt, int32_t * VoiceActStsPt );
__SPEEXDSP_DLLAPI__ int SpeexPprocDestroy( SpeexPreprocessState * SpeexPprocPt );

typedef struct SpeexResamplerState_ SpeexResamplerState;

__SPEEXDSP_DLLAPI__ int SpeexResamplerInit( SpeexResamplerState * * SpeexResamplerPtPt, int32_t BeforeSamplingRate, int32_t AfterSamplingRate, int32_t Quality, VarStr * ErrInfoVarStrPt );
__SPEEXDSP_DLLAPI__ int SpeexResamplerProc( SpeexResamplerState * SpeexResamplerPt, int16_t * BeforeFramePt, int32_t BeforeFrameLen, int16_t * AfterFramePt, size_t AfterFrameSz, int32_t * AfterFrameLenPt );
__SPEEXDSP_DLLAPI__ int SpeexResamplerDestroy( SpeexResamplerState * SpeexResamplerPt );

#ifdef __cplusplus
}
#endif

#endif
