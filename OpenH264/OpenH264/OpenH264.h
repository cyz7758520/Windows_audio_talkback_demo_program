#ifndef __OPENH264_H__
#define __OPENH264_H__

#include <stdint.h>
#include "VarStr.h"

#ifdef __cplusplus
extern "C"
{
#endif

//OpenH264项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_OPENH264__ ) //如果正在编译OpenH264项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __OPENH264_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __OPENH264_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __OPENH264_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __OPENH264_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __OPENH264_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __OPENH264_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __OPENH264_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __OPENH264_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __OPENH264_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __OPENH264_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __OPENH264_DLLAPI__
	#endif
#endif

typedef struct OpenH264Encoder OpenH264Encoder;

__OPENH264_DLLAPI__ int OpenH264EncoderInit( OpenH264Encoder * * OpenH264EncoderPtPt, int32_t EncodedPictrWidth, int32_t EncodedPictrHeight, int32_t VideoType, int32_t EncodedBitrate, int32_t BitrateControlMode, int32_t MaxFrameRate, int32_t IDRFrameIntvlFrameCnt, int32_t Complexity, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncoderSetEncodedBitrate( OpenH264Encoder * OpenH264EncoderPt, int32_t EncodedBitrate, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncoderGetEncodedBitrate( OpenH264Encoder * OpenH264EncoderPt, int32_t * EncodedBitratePt, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncoderProc( OpenH264Encoder * OpenH264EncoderPt,
											 uint8_t * YU12FramePt, int32_t YU12FrameWidth, int32_t YU12FrameHeight, uint64_t YU12FrameTimeStampMsec,
											 uint8_t * H264FramePt, size_t H264FrameSz, size_t * H264FrameLenPt,
											 VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncoderDestroy( OpenH264Encoder * OpenH264EncoderPt, VarStr * ErrInfoVarStrPt );

typedef struct OpenH264Decoder OpenH264Decoder;

__OPENH264_DLLAPI__ int OpenH264DecoderInit( OpenH264Decoder * * OpenH264DecoderPtPt, int32_t DecodeThreadNum, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264DecoderProc( OpenH264Decoder * OpenH264DecoderPt,
											 uint8_t * H264FramePt, size_t H264FrameLen,
											 uint8_t * YU12FramePt, size_t YU12FrameSz, int32_t * YU12FrameWidthPt, int32_t * YU12FrameHeightPt,
											 VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264DecoderDestroy( OpenH264Decoder * OpenH264EncoderPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif