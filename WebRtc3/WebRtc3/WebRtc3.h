#ifndef __WEBRTC3_H__
#define __WEBRTC3_H__

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

//WebRtc3项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_WEBRTC3__ ) //如果正在编译WebRtc3项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WEBRTC3_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WEBRTC3_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WEBRTC3_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WEBRTC3_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WEBRTC3_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WEBRTC3_DLLAPI__
		#endif
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( ( defined __NOLNK__ ) || ( defined __LNKLIB__ ) ) //如果不链接库文件，或正在链接LIB静态库文件。
			#define __WEBRTC3_DLLAPI__
		#elif( defined __LNKDLL__ )
			#define __WEBRTC3_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __WEBRTC3_DLLAPI__
	#endif
#endif

typedef struct WebRtcAec3 WebRtcAec3;

__WEBRTC3_DLLAPI__ int WebRtcAec3GetAppLmtInfo( Vstr * LmtAppNameVstrPt, Vstr * CurAppNameVstrPt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );
__WEBRTC3_DLLAPI__ int WebRtcAec3Init( WebRtcAec3 * * WebRtcAec3PtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t Delay, Vstr * ErrInfoVstrPt );
__WEBRTC3_DLLAPI__ int WebRtcAec3SetDelay( WebRtcAec3 * WebRtcAec3Pt, int32_t Delay );
__WEBRTC3_DLLAPI__ int WebRtcAec3GetDelay( WebRtcAec3 * WebRtcAec3Pt, int32_t * DelayPt );
__WEBRTC3_DLLAPI__ int WebRtcAec3GetIsCnvgnc( WebRtcAec3 * WebRtcAec3Pt, int32_t * IsCnvgncPt );
__WEBRTC3_DLLAPI__ int WebRtcAec3Pocs( WebRtcAec3 * WebRtcAec3Pt, int16_t * InputFramePt, int16_t * OutputFramePt, int16_t * ResultFramePt );
__WEBRTC3_DLLAPI__ int WebRtcAec3Dstoy( WebRtcAec3 * WebRtcAec3Pt );

#ifdef __cplusplus
}
#endif

#endif