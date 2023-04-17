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

__SPEEXDSP_DLLAPI__ int SpeexAecInit( SpeexAec * * SpeexAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecInitByMem( SpeexAec * * SpeexAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const char * MemPt, size_t MemLenByt, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecInitByMemFile( SpeexAec * * SpeexAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const Vstr * MemFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetMemLen( SpeexAec * SpeexAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, size_t * MemLenBytPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetMem( SpeexAec * SpeexAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, char * MemPt, size_t MemSzByt );
__SPEEXDSP_DLLAPI__ int SpeexAecSaveMemFile( SpeexAec * SpeexAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, const Vstr * MemFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexAecGetIsCnvgnc( SpeexAec * SpeexAecPt, int32_t * IsCnvgncPt );
__SPEEXDSP_DLLAPI__ int SpeexAecPocs( SpeexAec * SpeexAecPt, int16_t * InptFrmPt, int16_t * OtptFrmPt, int16_t * RsltFrmPt );
__SPEEXDSP_DLLAPI__ int SpeexAecDstoy( SpeexAec * SpeexAecPt );

typedef struct SpeexPreprocessState_ SpeexPreprocessState;

__SPEEXDSP_DLLAPI__ int SpeexPrpocsInit( SpeexPreprocessState * * SpeexPrpocsPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexPrpocsPocs( SpeexPreprocessState * SpeexPrpocsPt, int16_t * FrmPt, int16_t * RsltFrmPt, int32_t * VoiceActStsPt );
__SPEEXDSP_DLLAPI__ int SpeexPrpocsDstoy( SpeexPreprocessState * SpeexPrpocsPt );

typedef struct SpeexResamplerState_ SpeexResamplerState;

__SPEEXDSP_DLLAPI__ int SpeexResamplerInit( SpeexResamplerState * * SpeexResamplerPtPt, int32_t BeforeSmplRate, int32_t AfterSmplRate, int32_t Quality, Vstr * ErrInfoVstrPt );
__SPEEXDSP_DLLAPI__ int SpeexResamplerPocs( SpeexResamplerState * SpeexResamplerPt, int16_t * BeforeFrmPt, size_t BeforeFrmLenUnit, int16_t * AfterFrmPt, size_t AfterFrmSzUnit, size_t * AfterFrmLenUnitPt );
__SPEEXDSP_DLLAPI__ int SpeexResamplerDstoy( SpeexResamplerState * SpeexResamplerPt );

#ifdef __cplusplus
}
#endif

#endif
