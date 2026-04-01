#pragma once

#include "MediaFile.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileReader WaveFileReader;

__MEDIAFILE_DLLAPI__ int WaveFileReaderInit( WaveFileReader * * WaveFileReaderPtPt, const Vstr * WaveFileFullPathVstrPt, int32_t * ChanlNumPt, int32_t * SmplRatePt, int32_t * SmplBitPt, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int WaveFileReaderDstoy( WaveFileReader * WaveFileReaderPt, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int WaveFileReaderRead( WaveFileReader * WaveFileReaderPt, void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class WaveFileReaderCls
{
public:
	WaveFileReader * m_WaveFileReaderPt;

	WaveFileReaderCls() { m_WaveFileReaderPt = NULL; }
	~WaveFileReaderCls() { Dstoy( NULL ); }

	int Init( const Vstr * WaveFileFullPathVstrPt, int32_t * ChanlNumPt, int32_t * SmplRatePt, int32_t * SmplBitPt, Vstr * ErrInfoVstrPt ) { return WaveFileReaderInit( &m_WaveFileReaderPt, WaveFileFullPathVstrPt, ChanlNumPt, SmplRatePt, SmplBitPt, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = WaveFileReaderDstoy( m_WaveFileReaderPt, ErrInfoVstrPt ); m_WaveFileReaderPt = NULL; return p_Rslt; }
	
	int Read( void * DataPt, size_t DataSzByt, size_t * DataLenBytPt, Vstr * ErrInfoVstrPt ) { return WaveFileReaderRead( m_WaveFileReaderPt, DataPt, DataSzByt, DataLenBytPt, ErrInfoVstrPt ); }
};
#endif
