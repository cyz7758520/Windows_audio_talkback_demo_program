#include "Func.h"

#ifndef __ADO_H__
#define __ADO_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
__FUNC_DLLAPI__ int AdoInt16IncrDecr( int16_t * SrcInt16AdoPt, int16_t * DstInt16AdoPt, int SmplCnt, int16_t Qntt );

__FUNC_DLLAPI__ int AdoInt16ToFlt32768( const int16_t * Int16AdoPt, float * Flt32768AdoPt, int SmplCnt, Drct_t CnvrtDrct );
__FUNC_DLLAPI__ int AdoInt16ToFlt1( const int16_t * Int16AdoPt, float * Flt1AdoPt, int SmplCnt, Drct_t CnvrtDrct );
__FUNC_DLLAPI__ int AdoFlt32768ToInt16( const float * Flt32768AdoPt, int16_t * Int16AdoPt, int SmplCnt, Drct_t CnvrtDrct );
__FUNC_DLLAPI__ int AdoFlt1ToInt16( const float * Flt1AdoPt, int16_t * Int16AdoPt, int SmplCnt, Drct_t CnvrtDrct );

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

//G711的A律和μ律编解码。
__FUNC_DLLAPI__ int G711aEncdPocs( int16_t * PcmFrmPt, size_t PcmFrmLen, uint8_t * G711aFrmPt, size_t G711aFrmSz, size_t * G711aFrmLenPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int G711aDecdPocs( uint8_t * G711aFrmPt, size_t G711aFrmLen, int16_t * PcmFrmPt, size_t PcmFrmSz, size_t * PcmFrmLenPt, Vstr * ErrInfoVstrPt );

__FUNC_DLLAPI__ int G711uEncdPocs( int16_t * PcmFrmPt, size_t PcmFrmLen, uint8_t * G711uFrmPt, size_t G711uFrmSz, size_t * G711uFrmLenPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int G711uDecdPocs( uint8_t * G711uFrmPt, size_t G711uFrmLen, int16_t * PcmFrmPt, size_t PcmFrmSz, size_t * PcmFrmLenPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#endif
