#include "WaveFile.h"

#ifndef __WAVEFILEREADER_H__
#define __WAVEFILEREADER_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileReader WaveFileReader;

__WAVEFILE_DLLAPI__ int WaveFileReaderInit( WaveFileReader * * WaveFileReaderPtPt, const Vstr * WaveFileFullPathVstrPt, int32_t * NumChanlPt, int32_t * SmplRatePt, int32_t * SmplBitPt );
__WAVEFILE_DLLAPI__ int WaveFileReaderRead( WaveFileReader * WaveFileReaderPt, char * DataPt, size_t DataSz, size_t * DataLenPt );
__WAVEFILE_DLLAPI__ int WaveFileReaderDstoy( WaveFileReader * WaveFileReaderPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class WaveFileReaderCls
{
public:
	WaveFileReader * m_WaveFileReaderPt;

	WaveFileReaderCls() { m_WaveFileReaderPt = NULL; }
	~WaveFileReaderCls() { Dstoy(); }

	int Init( const Vstr * WaveFileFullPathVstrPt, int32_t * NumChanlPt, int32_t * SmplRatePt, int32_t * SmplBitPt ) { return WaveFileReaderInit( &m_WaveFileReaderPt, WaveFileFullPathVstrPt, NumChanlPt, SmplRatePt, SmplBitPt ); }
	
	int Read( char * DataPt, size_t DataSz, size_t * DataLenPt ) { return WaveFileReaderRead( m_WaveFileReaderPt, DataPt, DataSz, DataLenPt ); }

	int Dstoy() { int p_Rslt = WaveFileReaderDstoy( m_WaveFileReaderPt ); m_WaveFileReaderPt = NULL; return p_Rslt; }
};
#endif

#endif