#include "Func.h"

#ifndef __OPENH264_H__
#define __OPENH264_H__

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

__OPENH264_DLLAPI__ int OpenH264EncdInit( OpenH264Encd * * OpenH264EncdPtPt, int32_t EncdPictrWidth, int32_t EncdPictrHeight, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t MaxFrmRate, int32_t IDRFrmIntvlFrmCnt, int32_t Cmplxt, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncdSetEncdBitrate( OpenH264Encd * OpenH264EncdPt, int32_t EncdBitrate, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncdGetEncdBitrate( OpenH264Encd * OpenH264EncdPt, int32_t * EncdBitratePt, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncdPocs( OpenH264Encd * OpenH264EncdPt,
										  uint8_t * YU12FrmPt, int32_t YU12FrmWidth, int32_t YU12FrmHeight, uint64_t YU12FrmTimeStampMsec,
										  uint8_t * H264FrmPt, size_t H264FrmSz, size_t * H264FrmLenPt,
										  VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264EncdDstoy( OpenH264Encd * OpenH264EncdPt, VarStr * ErrInfoVarStrPt );

typedef struct OpenH264Decd OpenH264Decd;

__OPENH264_DLLAPI__ int OpenH264DecdInit( OpenH264Decd ** OpenH264DecdPtPt, int32_t DecdThrdNum, VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264DecdPocs( OpenH264Decd * OpenH264DecdPt,
										  uint8_t * H264FrmPt, size_t H264FrmLen,
										  uint8_t * YU12FrmPt, size_t YU12FrmSz, int32_t * YU12FrmWidthPt, int32_t * YU12FrmHeightPt,
										  VarStr * ErrInfoVarStrPt );
__OPENH264_DLLAPI__ int OpenH264DecdDstoy( OpenH264Decd * OpenH264DecdPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif