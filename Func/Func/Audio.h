﻿#include "Func.h"

#ifndef __AUDIO_H__
#define __AUDIO_H__

//如果是库的源文件就加入__FUNC_DLLAPI__宏的定义代码，如果不是就不加。
//Func项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_FUNC__ ) //如果正在编译Func项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __FUNC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

__FUNC_DLLAPI__ int AudioInt16ToFlt32768( const int16_t * AudioInt16Pt, float * AudioFlt32768Pt, int SampleCnt, int Drct );
__FUNC_DLLAPI__ int AudioInt16ToFlt1( const int16_t * AudioInt16Pt, float * AudioFlt1Pt, int SampleCnt, int Drct );
__FUNC_DLLAPI__ int AudioFlt32768ToInt16( const float * AudioFlt32768Pt, int16_t * AudioInt16Pt, int SampleCnt, int Drct );
__FUNC_DLLAPI__ int AudioFlt1ToInt16( const float * AudioFlt1Pt, int16_t * AudioInt16Pt, int SampleCnt, int Drct );

__FUNC_DLLAPI__ void AudioOneBand32kInt16ToLHBand16kInt16( const int16_t OneBand32kInt16[320],
														   int16_t LowBand16kInt16[160], int16_t HighBand16kInt16[160],
														   int32_t AnalysisState1[6], int32_t AnalysisState2[6] );

__FUNC_DLLAPI__ void AudioOneBand32kInt16ToLHBand16kFlt32768( const int16_t OneBand32kInt16[320],
															  float LowBand16kFlt32768[160], float HighBand16kFlt32768[160],
															  int32_t AnalysisState1[6], int32_t AnalysisState2[6] );

__FUNC_DLLAPI__ void AudioLHBand16kInt16ToOneBand32kInt16( const int16_t LowBand16kInt16[160], const int16_t HighBand16kInt16[160],
														   int16_t OneBand32kInt16[320],
														   int32_t SynthesisState1[6], int32_t SynthesisState2[6] );

__FUNC_DLLAPI__ void AudioLHBand16kFlt32768ToOneBand32kInt16( const float LowBand16kFlt32768[160], const float HighBand16kFlt32768[160],
															  int16_t OneBand32kInt16[320],
															  int32_t SynthesisState1[6], int32_t SynthesisState2[6] );

__FUNC_DLLAPI__ void AudioOneBand48kInt16ToLMHBand16kInt16( const int16_t OneBand48kInt16[480],
															int16_t LowBand16kInt16[160], int16_t MidBand16kInt16[160], int16_t HighBand16kInt16[160],
															float AnalysisState[10][15] );

__FUNC_DLLAPI__ void AudioOneBand48kInt16ToLMHBand16kFlt32768( const int16_t OneBand48kInt16[480],
															   float LowBand16kFlt32768[160], float MidBand16kFlt32768[160], float HighBand16kFlt32768[160],
															   float AnalysisState[10][15] );

__FUNC_DLLAPI__ void AudioLMHBand16kInt16ToOneBand48kInt16( const int16_t LowBand16kInt16[160], const int16_t MidBand16kInt16[160], const int16_t HighBand16kInt16[160],
															int16_t OneBand48kInt16[480],
															float SynthesisState[10][15] );

__FUNC_DLLAPI__ void AudioLMHBand16kFlt32768ToOneBand48kInt16( const float LowBand16kFlt32768[160], const float MidBand16kFlt32768[160], const float HighBand16kFlt32768[160],
															   int16_t OneBand48kInt16[480],
															   float SynthesisState[10][15] );

#ifdef __cplusplus
}
#endif

#endif