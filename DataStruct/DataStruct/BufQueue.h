#include "DataStruct.h"

#ifndef __BUFQUEUE_H__
#define __BUFQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BufQueue BufQueue;

__DATASTRUCT_DLLAPI__ int BufQueueInit( BufQueue * * BufQueuePtPt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueLocked( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueUnlock( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueuePutHead( BufQueue * BufQueuePt, const void * NewHeadDataPt, size_t NewHeadDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueuePutTail( BufQueue * BufQueuePt, const void * NewTailDataPt, size_t NewTailDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueGetHead( BufQueue * BufQueuePt, void * HeadDataPt, size_t HeadDataSz, size_t * HeadDataLenPt, int32_t IsDelHeadData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueGetTail( BufQueue * BufQueuePt, void * TailDataPt, size_t TailDataSz, size_t * TailDataLenPt, int32_t IsDelTailData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueGetLen( BufQueue * BufQueuePt, size_t * LenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueDelHead( BufQueue * BufQueuePt, size_t HeadDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueDelTail( BufQueue * BufQueuePt, size_t TailDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueDelAll( BufQueue * BufQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueDstoy( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class BufQueueCls
{
public:
	BufQueue * m_BufQueuePt;

	BufQueueCls() { m_BufQueuePt = NULL; }
	~BufQueueCls() { Dstoy( NULL ); }

	int Init( BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinNum, size_t BufMaxNum, Vstr * ErrInfoVstrPt ) { return BufQueueInit( &m_BufQueuePt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinNum, BufMaxNum, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return BufQueueLocked( m_BufQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return BufQueueUnlock( m_BufQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadDataPt, size_t NewHeadDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueuePutHead( m_BufQueuePt, NewHeadDataPt, NewHeadDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailDataPt, size_t NewTailDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueuePutTail( m_BufQueuePt, NewTailDataPt, NewTailDataLen, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadDataPt, size_t HeadDataSz, size_t * HeadDataLenPt, int32_t IsDelHeadData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetHead( m_BufQueuePt, HeadDataPt, HeadDataSz, HeadDataLenPt, IsDelHeadData, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailDataPt, size_t TailDataSz, size_t * TailDataLenPt, int32_t IsDelTailData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetTail( m_BufQueuePt, TailDataPt, TailDataSz, TailDataLenPt, IsDelTailData, IsAutoLock, ErrInfoVstrPt ); }
	int GetLen( size_t * LenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetLen( m_BufQueuePt, LenPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( size_t HeadDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelHead( m_BufQueuePt, HeadDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( size_t TailDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelTail( m_BufQueuePt, TailDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelAll( m_BufQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = BufQueueDstoy( m_BufQueuePt, ErrInfoVstrPt ); m_BufQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
