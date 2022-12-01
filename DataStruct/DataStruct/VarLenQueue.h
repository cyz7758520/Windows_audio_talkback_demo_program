#include "DataStruct.h"

#ifndef __VARLENQUEUE_H__
#define __VARLENQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VarLenQueue VarLenQueue;
	
__DATASTRUCT_DLLAPI__ int VarLenQueueInit( VarLenQueue * * VarLenQueuePtPt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueLocked( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueUnlock( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueuePutHead( VarLenQueue * VarLenQueuePt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueuePutTail( VarLenQueue * VarLenQueuePt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueGetHead( VarLenQueue * VarLenQueuePt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTail( VarLenQueue * VarLenQueuePt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataBufPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
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

	int Init( BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt ) { return VarLenQueueInit( &m_VarLenQueuePt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSz, BufMaxSz, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VarLenQueueLocked( m_VarLenQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenQueueUnlock( m_VarLenQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutHead( m_VarLenQueuePt, NewHeadElmDataPt, NewHeadElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutTail( m_VarLenQueuePt, NewTailElmDataPt, NewTailElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetHead( m_VarLenQueuePt, HeadElmDataPt, HeadElmDataSz, HeadElmDataBufPtPt, HeadElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataBufPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTail( m_VarLenQueuePt, TailElmDataPt, TailElmDataSz, TailElmDataBufPtPt, TailElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetByNum( m_VarLenQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataSz, SpecElmDataBufPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTotal( m_VarLenQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelHead( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelTail( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelByNum( m_VarLenQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelAll( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VarLenQueueDstoy( m_VarLenQueuePt, ErrInfoVstrPt ); m_VarLenQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
