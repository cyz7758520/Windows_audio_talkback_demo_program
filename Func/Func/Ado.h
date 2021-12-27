#include "Func.h"

#ifndef __ADO_H__
#define __ADO_H__

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

__FUNC_DLLAPI__ int AdoInt16ToFlt32768( const int16_t * AdoInt16Pt, float * AdoFlt32768Pt, int SmplCnt, int Drct );
__FUNC_DLLAPI__ int AdoInt16ToFlt1( const int16_t * AdoInt16Pt, float * AdoFlt1Pt, int SmplCnt, int Drct );
__FUNC_DLLAPI__ int AdoFlt32768ToInt16( const float * AdoFlt32768Pt, int16_t * AdoInt16Pt, int SmplCnt, int Drct );
__FUNC_DLLAPI__ int AdoFlt1ToInt16( const float * AdoFlt1Pt, int16_t * AdoInt16Pt, int SmplCnt, int Drct );

__FUNC_DLLAPI__ void AdoOneBand32kInt16ToLHBand16kInt16( const int16_t OneBand32kInt16[320],
														 int16_t LowBand16kInt16[160], int16_t HighBand16kInt16[160],
														 int32_t AnalysisState1[6], int32_t AnalysisState2[6] );

__FUNC_DLLAPI__ void AdoOneBand32kInt16ToLHBand16kFlt32768( const int16_t OneBand32kInt16[320],
															float LowBand16kFlt32768[160], float HighBand16kFlt32768[160],
															int32_t AnalysisState1[6], int32_t AnalysisState2[6] );

__FUNC_DLLAPI__ void AdoLHBand16kInt16ToOneBand32kInt16( const int16_t LowBand16kInt16[160], const int16_t HighBand16kInt16[160],
														 int16_t OneBand32kInt16[320],
														 int32_t SynthesisState1[6], int32_t SynthesisState2[6] );

__FUNC_DLLAPI__ void AdoLHBand16kFlt32768ToOneBand32kInt16( const float LowBand16kFlt32768[160], const float HighBand16kFlt32768[160],
															int16_t OneBand32kInt16[320],
															int32_t SynthesisState1[6], int32_t SynthesisState2[6] );

__FUNC_DLLAPI__ void AdoOneBand48kInt16ToLMHBand16kInt16( const int16_t OneBand48kInt16[480],
														  int16_t LowBand16kInt16[160], int16_t MidBand16kInt16[160], int16_t HighBand16kInt16[160],
														  float AnalysisState[10][15] );

__FUNC_DLLAPI__ void AdoOneBand48kInt16ToLMHBand16kFlt32768( const int16_t OneBand48kInt16[480],
															 float LowBand16kFlt32768[160], float MidBand16kFlt32768[160], float HighBand16kFlt32768[160],
															 float AnalysisState[10][15] );

__FUNC_DLLAPI__ void AdoLMHBand16kInt16ToOneBand48kInt16( const int16_t LowBand16kInt16[160], const int16_t MidBand16kInt16[160], const int16_t HighBand16kInt16[160],
														  int16_t OneBand48kInt16[480],
														  float SynthesisState[10][15] );

__FUNC_DLLAPI__ void AdoLMHBand16kFlt32768ToOneBand48kInt16( const float LowBand16kFlt32768[160], const float MidBand16kFlt32768[160], const float HighBand16kFlt32768[160],
															 int16_t OneBand48kInt16[480],
															 float SynthesisState[10][15] );

//G711a编解码。
__FUNC_DLLAPI__ int G711aEncdPocs( int16_t * PcmFrmPt, size_t PcmFrmLen, uint8_t * G711aFrmPt, size_t G711aFrmSz, size_t * G711aFrmLenPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int G711aDecdPocs( uint8_t * G711aFrmPt, size_t G711aFrmLen, int16_t * PcmFrmPt, size_t PcmFrmSz, size_t * PcmFrmLenPt, VarStr * ErrInfoVarStrPt );

__FUNC_DLLAPI__ int G711uEncdPocs( int16_t * PcmFrmPt, size_t PcmFrmLen, uint8_t * G711uFrmPt, size_t G711uFrmSz, size_t * G711uFrmLenPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int G711uDecdPocs( uint8_t * G711uFrmPt, size_t G711uFrmLen, int16_t * PcmFrmPt, size_t PcmFrmSz, size_t * PcmFrmLenPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
