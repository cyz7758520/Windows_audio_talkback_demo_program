#include "Func.h"

#ifndef __WAVEFILE_H__
#define __WAVEFILE_H__

//WaveFile项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_WAVEFILE__ ) //如果正在编译WaveFile项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WAVEFILE_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WAVEFILE_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WAVEFILE_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __WAVEFILE_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __WAVEFILE_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __WAVEFILE_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __WAVEFILE_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __WAVEFILE_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __WAVEFILE_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __WAVEFILE_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __WAVEFILE_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileWriter WaveFileWriter;

__WAVEFILE_DLLAPI__ int WaveFileWriterInit( WaveFileWriter * * WaveFileWriterPtPt, const char * WaveFileFullPathStrPt, int16_t NumChanl, int32_t SmplRate, int32_t SmplBit );
__WAVEFILE_DLLAPI__ int WaveFileWriterWriteData( WaveFileWriter * WaveFileWriterPt, const char * DataPt, size_t DataLen );
__WAVEFILE_DLLAPI__ int WaveFileWriterDstoy( WaveFileWriter * WaveFileWriterPt );

typedef struct WaveFileReader WaveFileReader;

__WAVEFILE_DLLAPI__ int WaveFileReaderInit( WaveFileReader * * WaveFileReaderPtPt, const char * WaveFileFullPathStrPt, int16_t * NumChanlPt, int32_t * SmplRatePt, int32_t * SmplBitPt );
__WAVEFILE_DLLAPI__ int WaveFileReaderReadData( WaveFileReader * WaveFileReaderPt, char * DataPt, size_t DataSz, size_t * DataLenPt );
__WAVEFILE_DLLAPI__ int WaveFileReaderDstoy( WaveFileReader * WaveFileReaderPt );

#ifdef __cplusplus
}
#endif

#endif