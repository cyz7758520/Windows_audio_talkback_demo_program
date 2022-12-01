#include "WaveFile.h"

#ifndef __WAVEFILEWRITER_H__
#define __WAVEFILEWRITER_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileWriter WaveFileWriter;

__WAVEFILE_DLLAPI__ int WaveFileWriterInit( WaveFileWriter * * WaveFileWriterPtPt, const Vstr * WaveFileFullPathVstrPt, int32_t NumChanl, int32_t SmplRate, int32_t SmplBit );
__WAVEFILE_DLLAPI__ int WaveFileWriterWrite( WaveFileWriter * WaveFileWriterPt, const char * DataPt, size_t DataLen );
__WAVEFILE_DLLAPI__ int WaveFileWriterDstoy( WaveFileWriter * WaveFileWriterPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class WaveFileWriterCls
{
public:
	WaveFileWriter * m_WaveFileWriterPt;

	WaveFileWriterCls() { m_WaveFileWriterPt = NULL; }
	~WaveFileWriterCls() { Dstoy(); }

	int Init( const Vstr * WaveFileFullPathVstrPt, int32_t NumChanl, int32_t SmplRate, int32_t SmplBit ) { return WaveFileWriterInit( &m_WaveFileWriterPt, WaveFileFullPathVstrPt, NumChanl, SmplRate, SmplBit ); }
	
	int Write( const char * DataPt, size_t DataLen ) { return WaveFileWriterWrite( m_WaveFileWriterPt, DataPt, DataLen ); }

	int Dstoy() { int p_Rslt = WaveFileWriterDstoy( m_WaveFileWriterPt ); m_WaveFileWriterPt = NULL; return p_Rslt; }
};
#endif

#endif