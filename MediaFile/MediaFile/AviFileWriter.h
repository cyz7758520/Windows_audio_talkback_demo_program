#pragma once

#include "MediaFile.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AviFileWriter AviFileWriter;

__MEDIAFILE_DLLAPI__ int AviFileWriterGetAppLmtInfo( const void * LicnCodePt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int AviFileWriterInit( const void * LicnCodePt, AviFileWriter * * AviFileWriterPtPt, const Vstr * AviFileFullPathVstrPt, size_t AviFileWrBufSzByt, uint32_t MaxStrmNum, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterDstoy( AviFileWriter * AviFileWriterPt, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int AviFileWriterSetStartTimeStamp( AviFileWriter * AviFileWriterPt, uint64_t StartTimeStampMsec, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterGetStartTimeStamp( AviFileWriter * AviFileWriterPt, uint64_t * StartTimeStampMsecPt, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int AviFileWriterAddAdoStrm( AviFileWriter * AviFileWriterPt, uint32_t Fmt, uint32_t SampleRate, uint32_t ChanlNum, uint32_t * AdoStrmIdxPt, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterAdoStrmSetCurTimeStamp( AviFileWriter * AviFileWriterPt, uint32_t AdoStrmIdx, uint64_t CurTimeStampMsec, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterAdoStrmGetCurTimeStamp( AviFileWriter * AviFileWriterPt, uint32_t AdoStrmIdx, uint64_t * CurTimeStampMsecPt, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterAdoStrmWrite( AviFileWriter * AviFileWriterPt, uint32_t AdoStrmIdx, void * FrmPt, size_t FrmLenByt, uint64_t FrmLenMsec, Vstr * ErrInfoVstrPt );

__MEDIAFILE_DLLAPI__ int AviFileWriterAddVdoStrm( AviFileWriter * AviFileWriterPt, uint32_t Fmt, uint32_t MaxSampleRate, uint32_t * VdoStrmIdxPt, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterVdoStrmGetCurTimeStamp( AviFileWriter * AviFileWriterPt, uint32_t VdoStrmIdx, uint64_t * CurTimeStampMsecPt, Vstr * ErrInfoVstrPt );
__MEDIAFILE_DLLAPI__ int AviFileWriterVdoStrmWrite( AviFileWriter * AviFileWriterPt, uint32_t VdoStrmIdx, uint64_t FrmTimeStampMsec, void * FrmPt, size_t FrmLenByt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif
