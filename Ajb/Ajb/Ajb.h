#ifndef __AJB_H__
#define __AJB_H__

#include "Func.h"

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

typedef struct AAjb AAjb;

__AJB_DLLAPI__ int AAjbInit( AAjb * * AAjbPtPt, int32_t SamplingRate, int32_t FrameLen, int32_t IsHaveTimeStamp, int32_t TimeStampStep, int32_t InactIsContPut, int32_t MinNeedBufFrameCnt, int32_t MaxNeedBufFrameCnt, float AdaptSensitivity, int32_t IsUseMutexLock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbPutOneFrame( AAjb * AAjbPt, uint32_t TimeStamp, const int8_t * FramePt, size_t FrameLen, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int AAjbGetOneFrame( AAjb * AAjbPt, uint32_t * TimeStampPt, int8_t * FramePt, size_t FrameSz, size_t * FrameLenPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbGetBufFrameCnt( AAjb * AAjbPt, int32_t * CurHaveBufActFrameCntPt, int32_t * CurHaveBufInactFrameCntPt, int32_t * CurHaveBufFrameCntPt, int32_t * MinNeedBufFrameCntPt, int32_t * MaxNeedBufFrameCntPt, int32_t * CurNeedBufFrameCntPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbClear( AAjb * AAjbPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbDestroy( AAjb * AAjbPt, VarStr * ErrInfoVarStrPt );

typedef struct VAjb VAjb;

__AJB_DLLAPI__ int VAjbInit( VAjb * * VAjbPtPt, int32_t IsHaveTimeStamp, int32_t MinNeedBufFrameCnt, int32_t MaxNeedBufFrameCnt, float AdaptSensitivity, int32_t IsUseMutexLock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbPutOneFrame( VAjb * VAjbPt, uint64_t CurTime, uint32_t TimeStamp, const int8_t * FramePt, size_t FrameLen, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int VAjbGetOneFrame( VAjb * VAjbPt, uint64_t CurTime, uint32_t * TimeStampPt, int8_t * FramePt, size_t FrameSz, size_t * FrameLenPt, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int VAjbGetOneFrameWantTimeStamp( VAjb * VAjbPt, uint64_t CurTime, uint32_t WantTimeStamp, uint32_t * TimeStampPt, int8_t * FramePt, size_t FrameSz, size_t * FrameLenPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbGetBufFrameCnt( VAjb * VAjbPt, int32_t * CurHaveBufFrameCntPt, int32_t * MinNeedBufFrameCntPt, int32_t * MaxNeedBufFrameCntPt, int32_t * CurNeedBufFrameCntPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbClear( VAjb * VAjbPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbDestroy( VAjb * VAjbPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif