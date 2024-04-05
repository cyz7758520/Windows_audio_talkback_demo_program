#pragma once

#include "MediaFile.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileReader WaveFileReader;

__MEDIAFILE_DLLAPI__ int WaveFileReaderInit( WaveFileReader * * WaveFileReaderPtPt, const Vstr * WaveFileFullPathVstrPt, int32_t * ChanlNumPt, int32_t * SmplRatePt, int32_t * SmplBitPt );
__MEDIAFILE_DLLAPI__ int WaveFileReaderRead( WaveFileReader * WaveFileReaderPt, char * DataPt, size_t DataSzByt, size_t * DataLenBytPt );
__MEDIAFILE_DLLAPI__ int WaveFileReaderDstoy( WaveFileReader * WaveFileReaderPt );

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

	int Init( const Vstr * WaveFileFullPathVstrPt, int32_t * ChanlNumPt, int32_t * SmplRatePt, int32_t * SmplBitPt ) { return WaveFileReaderInit( &m_WaveFileReaderPt, WaveFileFullPathVstrPt, ChanlNumPt, SmplRatePt, SmplBitPt ); }
	
	int Read( char * DataPt, size_t DataSzByt, size_t * DataLenBytPt ) { return WaveFileReaderRead( m_WaveFileReaderPt, DataPt, DataSzByt, DataLenBytPt ); }

	int Dstoy() { int p_Rslt = WaveFileReaderDstoy( m_WaveFileReaderPt ); m_WaveFileReaderPt = NULL; return p_Rslt; }
};
#endif
