#pragma once

#include "Func.h"

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
	
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WebRtcAecm WebRtcAecm;

__WEBRTC_DLLAPI__ int WebRtcAecmInit( WebRtcAecm * * WebRtcAecmPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcAecmSetDelay( WebRtcAecm * WebRtcAecmPt, int32_t Delay );
__WEBRTC_DLLAPI__ int WebRtcAecmGetDelay( WebRtcAecm * WebRtcAecmPt, int32_t * DelayPt );
__WEBRTC_DLLAPI__ int WebRtcAecmPocs( WebRtcAecm * WebRtcAecmPt, int16_t * InptFrmPt, int16_t * OtptFrmPt, int16_t * RsltFrmPt );
__WEBRTC_DLLAPI__ int WebRtcAecmDstoy( WebRtcAecm * WebRtcAecmPt );

typedef struct WebRtcAec WebRtcAec;

__WEBRTC_DLLAPI__ int WebRtcAecInit( WebRtcAec * * WebRtcAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcAecInitByMem( WebRtcAec * * WebRtcAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, char * MemPt, size_t MemLenByt, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcAecInitByMemFile( WebRtcAec * * WebRtcAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, const Vstr * MemFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetMemLen( WebRtcAec * WebRtcAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, size_t * MemLenBytPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetMem( WebRtcAec * WebRtcAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, char * MemPt, size_t MemSzByt );
__WEBRTC_DLLAPI__ int WebRtcAecSaveMemFile( WebRtcAec * WebRtcAecPt, int32_t SmplRate, size_t FrmLenUnit, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, const Vstr * MemFileFullPathVstrPt, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcAecSetDelay( WebRtcAec * WebRtcAecPt, int32_t Delay );
__WEBRTC_DLLAPI__ int WebRtcAecGetDelay( WebRtcAec * WebRtcAecPt, int32_t * DelayPt );
__WEBRTC_DLLAPI__ int WebRtcAecGetIsCnvgnc( WebRtcAec * WebRtcAecPt, int32_t * IsCnvgncPt );
__WEBRTC_DLLAPI__ int WebRtcAecPocs( WebRtcAec * WebRtcAecPt, int16_t * InptFrmPt, int16_t * OtptFrmPt, int16_t * RsltFrmPt );
__WEBRTC_DLLAPI__ int WebRtcAecDstoy( WebRtcAec * WebRtcAecPt );

typedef struct WebRtcNsx WebRtcNsx;

__WEBRTC_DLLAPI__ int WebRtcNsxInit( WebRtcNsx * * WebRtcNsxPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcNsxPocs( WebRtcNsx * WebRtcNsxPt, int16_t * FrmPt, int16_t * RsltFrmPt );
__WEBRTC_DLLAPI__ int WebRtcNsxDstoy( WebRtcNsx * WebRtcNsxPt );

typedef struct WebRtcNs WebRtcNs;

__WEBRTC_DLLAPI__ int WebRtcNsInit( WebRtcNs * * WebRtcNsPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t PolicyMode, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcNsPocs( WebRtcNs * WebRtcNsPt, int16_t * FrmPt, int16_t * RsltFrmPt );
__WEBRTC_DLLAPI__ int WebRtcNsDstoy( WebRtcNs * WebRtcNsPt );

__WEBRTC_DLLAPI__ int WebRtcResamplerInit( void * * WebRtcResamplerPtPt, int32_t BeforeSmplRate, int32_t AfterSmplRate, Vstr * ErrInfoVstrPt );
__WEBRTC_DLLAPI__ int WebRtcResamplerPocs( void * WebRtcResamplerPt, int16_t * BeforeFrmPt, size_t BeforeFrmLenUnit, int16_t * AfterFrmPt, size_t AfterFrmSzUnit, size_t * AfterFrmLenUnitPt );
__WEBRTC_DLLAPI__ int WebRtcResamplerDstoy( void * WebRtcResamplerPt );

#ifdef __cplusplus
}
#endif
