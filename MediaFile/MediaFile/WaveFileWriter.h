#pragma once

#include "MediaFile.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct WaveFileWriter WaveFileWriter;

__MEDIAFILE_DLLAPI__ int WaveFileWriterInit( WaveFileWriter * * WaveFileWriterPtPt, const Vstr * WaveFileFullPathVstrPt, size_t WaveFileWrBufSzByt, int32_t ChanlNum, int32_t SmplRate, int32_t SmplBit, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int WaveFileWriterDstoy( WaveFileWriter * WaveFileWriterPt, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int WaveFileWriterWrite( WaveFileWriter * WaveFileWriterPt, const void * DataPt, size_t DataLenByt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class WaveFileWriterCls
{
public:
	WaveFileWriter * m_WaveFileWriterPt;

	WaveFileWriterCls() { m_WaveFileWriterPt = NULL; }
	~WaveFileWriterCls() { Dstoy( NULL ); }

	int Init( const Vstr * WaveFileFullPathVstrPt, size_t WaveFileWrBufSzByt, int32_t ChanlNum, int32_t SmplRate, int32_t SmplBit, Vstr * ErrInfoVstrPt ) { return WaveFileWriterInit( &m_WaveFileWriterPt, WaveFileFullPathVstrPt, WaveFileWrBufSzByt, ChanlNum, SmplRate, SmplBit, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = WaveFileWriterDstoy( m_WaveFileWriterPt, ErrInfoVstrPt ); m_WaveFileWriterPt = NULL; return p_Rslt; }
	
	int Write( const void * DataPt, size_t DataLenByt, Vstr * ErrInfoVstrPt ) { return WaveFileWriterWrite( m_WaveFileWriterPt, DataPt, DataLenByt, ErrInfoVstrPt ); }
};
#endif
