#ifndef __AJB_H__
#define __AJB_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//Ajb项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_AJB__ ) //如果正在编译Ajb项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AJB_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AJB_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AJB_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AJB_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __AJB_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __AJB_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __AJB_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __AJB_DLLAPI__
	#endif
#endif

typedef struct Ajb Ajb;

__AJB_DLLAPI__ int AjbInit( Ajb * * AjbPtPt, int32_t SamplingRate, int32_t FrameLen, int8_t IsHaveTimeStamp, int32_t TimeStampStep, int8_t InactIsContPut, int32_t MinNeedBufFrameCnt, int32_t MaxNeedBufFrameCnt, uint8_t AdaptSensitivity );

__AJB_DLLAPI__ int AjbPutOneFrame( Ajb * AjbPt, uint32_t TimeStamp, const int8_t * FramePt, size_t FrameLen );
__AJB_DLLAPI__ int AjbGetOneFrame( Ajb * AjbPt, uint32_t * TimeStampPt, int8_t * FramePt, size_t FrameSz, size_t * FrameLenPt );

__AJB_DLLAPI__ int AjbGetBufFrameCnt( Ajb * AjbPt, int32_t * CurHaveBufActFrameCntPt, int32_t * CurHaveBufInactFrameCntPt, int32_t * CurHaveBufFrameCntPt, int32_t * MinNeedBufFrameCntPt, int32_t * MaxNeedBufFrameCntPt, int32_t * CurNeedBufFrameCntPt );

__AJB_DLLAPI__ int AjbClear( Ajb * AjbPt );

__AJB_DLLAPI__ int AjbDestroy( Ajb * AjbPt );

#ifdef __cplusplus
}
#endif

#endif