﻿#pragma once

#include "Func.h"

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

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct OpenH264Encd OpenH264Encd;

__OPENH264_DLLAPI__ int OpenH264EncdGetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264EncdInit( const void * LicnCodePt, OpenH264Encd * * OpenH264EncdPtPt, int32_t EncdPictrWidth, int32_t EncdPictrHeight, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t MaxFrmRate, int32_t IDRFrmIntvlFrmCnt, int32_t Cmplxt, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264EncdSetEncdBitrate( OpenH264Encd * OpenH264EncdPt, int32_t EncdBitrate, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264EncdGetEncdBitrate( OpenH264Encd * OpenH264EncdPt, int32_t * EncdBitratePt, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264EncdPocs( OpenH264Encd * OpenH264EncdPt,
										  uint8_t * Yu12FrmPt, int32_t Yu12FrmWidth, int32_t Yu12FrmHeight, uint64_t Yu12FrmTimeStampMsec,
										  uint8_t * H264FrmPt, size_t H264FrmSzByt, size_t * H264FrmLenBytPt,
										  Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264EncdDstoy( OpenH264Encd * OpenH264EncdPt, Vstr * ErrInfoVstrPt );

typedef struct OpenH264Decd OpenH264Decd;

__OPENH264_DLLAPI__ int OpenH264DecdGetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264DecdInit( const void * LicnCodePt, OpenH264Decd * * OpenH264DecdPtPt, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264DecdPocs( OpenH264Decd * OpenH264DecdPt,
										  uint8_t * H264FrmPt, size_t H264FrmLenByt,
										  uint8_t * Yu12FrmPt, size_t Yu12FrmSzByt, int32_t * Yu12FrmWidthPt, int32_t * Yu12FrmHeightPt,
										  Vstr * ErrInfoVstrPt );
__OPENH264_DLLAPI__ int OpenH264DecdDstoy( OpenH264Decd * OpenH264DecdPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif
