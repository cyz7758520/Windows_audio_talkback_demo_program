﻿#pragma once

#include "Func.h"

//SpeexWebRtcAec项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_SPEEXWEBRTCAEC__ ) //如果正在编译SpeexWebRtcAec项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __SPEEXWEBRTCAEC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __SPEEXWEBRTCAEC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __SPEEXWEBRTCAEC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __SPEEXWEBRTCAEC_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	
//SpeexWebRtc三重声学回音消除器工作模式。
enum SpeexWebRtcAecWorkMode
{
	SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm = 0b0011, //Speex声学回音消除器+WebRtc定点版声学回音消除器。
	SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec = 0b0110, //WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
	SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec = 0b0111, //Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
	SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec3 = 0b1010, //WebRtc定点版声学回音消除器+WebRtc第三版声学回音消除器。
	SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec3 = 0b1011, //Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc第三版声学回音消除器。
};

typedef struct SpeexWebRtcAec SpeexWebRtcAec;

__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecGetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecInit( const void * LicnCodePt, SpeexWebRtcAec * * SpeexWebRtcAecPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t WebRtcAec3Delay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecSetWebRtcAecmDelay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t WebRtcAecmDelay );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecGetWebRtcAecmDelay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t * WebRtcAecmDelayPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecSetWebRtcAecDelay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t WebRtcAecDelay );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecGetWebRtcAecDelay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t * WebRtcAecDelayPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecSetWebRtcAec3Delay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t WebRtcAec3Delay );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecGetWebRtcAec3Delay( SpeexWebRtcAec * SpeexWebRtcAecPt, int32_t * WebRtcAec3DelayPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecPocs( SpeexWebRtcAec * SpeexWebRtcAecPt, int16_t * InptFrmPt, int16_t * OtptFrmPt, int16_t * RsltFrmPt );
__SPEEXWEBRTCAEC_DLLAPI__ int SpeexWebRtcAecDstoy( SpeexWebRtcAec * SpeexWebRtcAecPt );

#ifdef __cplusplus
}
#endif
