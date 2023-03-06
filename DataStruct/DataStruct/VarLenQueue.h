#include "DataStruct.h"

#ifndef __VARLENQUEUE_H__
#define __VARLENQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VarLenQueue VarLenQueue;
	
__DATASTRUCT_DLLAPI__ int VarLenQueueInit( VarLenQueue * * VarLenQueuePtPt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSzByt, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueLocked( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueUnlock( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueuePutHead( VarLenQueue * VarLenQueuePt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueuePutTail( VarLenQueue * VarLenQueuePt, const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueGetHead( VarLenQueue * VarLenQueuePt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTail( VarLenQueue * VarLenQueuePt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataBufPtPt, size_t * TailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTotal( VarLenQueue * VarLenQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDelHead( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelTail( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelAll( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDstoy( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarLenQueueCls
{
public:
	VarLenQueue * m_VarLenQueuePt;

	VarLenQueueCls() { m_VarLenQueuePt = NULL; }
	~VarLenQueueCls() { Dstoy( NULL ); }

	int Init( BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSzByt, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt ) { return VarLenQueueInit( &m_VarLenQueuePt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSzByt, BufMaxSzByt, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VarLenQueueLocked( m_VarLenQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenQueueUnlock( m_VarLenQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutHead( m_VarLenQueuePt, NewHeadElmDataPt, NewHeadElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutTail( m_VarLenQueuePt, NewTailElmDataPt, NewTailElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetHead( m_VarLenQueuePt, HeadElmDataPt, HeadElmDataSzByt, HeadElmDataBufPtPt, HeadElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataBufPtPt, size_t * TailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTail( m_VarLenQueuePt, TailElmDataPt, TailElmDataSzByt, TailElmDataBufPtPt, TailElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetByNum( m_VarLenQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataBufPtPt, SpecElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTotal( m_VarLenQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelHead( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelTail( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelByNum( m_VarLenQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelAll( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VarLenQueueDstoy( m_VarLenQueuePt, ErrInfoVstrPt ); m_VarLenQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
