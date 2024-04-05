#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BufQueue BufQueue;

__DATASTRUCT_DLLAPI__ int BufQueueInit( BufQueue * * BufQueuePtPt, size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueDstoy( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueLocked( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueUnlock( BufQueue * BufQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueuePutHead( BufQueue * BufQueuePt, const void * NewHeadDataPt, size_t NewHeadDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueuePutTail( BufQueue * BufQueuePt, const void * NewTailDataPt, size_t NewTailDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueGetHead( BufQueue * BufQueuePt, void * HeadDataPt, size_t HeadDataSzByt, size_t * HeadDataLenBytPt, int32_t IsDelHeadData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueGetTail( BufQueue * BufQueuePt, void * TailDataPt, size_t TailDataSzByt, size_t * TailDataLenBytPt, int32_t IsDelTailData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueDelHead( BufQueue * BufQueuePt, size_t HeadDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueDelTail( BufQueue * BufQueuePt, size_t TailDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueDelAll( BufQueue * BufQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int BufQueueGetLen( BufQueue * BufQueuePt, size_t * LenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueSetSz( BufQueue * BufQueuePt, size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int BufQueueGetSz( BufQueue * BufQueuePt, size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

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

	int Init( size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt ) { return BufQueueInit( &m_BufQueuePt, BufInitSzByt, BufAutoExtdMeth, BufAutoExtdParm, BufMaxSzByt, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = BufQueueDstoy( m_BufQueuePt, ErrInfoVstrPt ); m_BufQueuePt = NULL; return p_Rslt; }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return BufQueueLocked( m_BufQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return BufQueueUnlock( m_BufQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadDataPt, size_t NewHeadDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueuePutHead( m_BufQueuePt, NewHeadDataPt, NewHeadDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailDataPt, size_t NewTailDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueuePutTail( m_BufQueuePt, NewTailDataPt, NewTailDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadDataPt, size_t HeadDataSzByt, size_t * HeadDataLenBytPt, int32_t IsDelHeadData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetHead( m_BufQueuePt, HeadDataPt, HeadDataSzByt, HeadDataLenBytPt, IsDelHeadData, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailDataPt, size_t TailDataSzByt, size_t * TailDataLenBytPt, int32_t IsDelTailData, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetTail( m_BufQueuePt, TailDataPt, TailDataSzByt, TailDataLenBytPt, IsDelTailData, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( size_t HeadDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelHead( m_BufQueuePt, HeadDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( size_t TailDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelTail( m_BufQueuePt, TailDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueDelAll( m_BufQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetLen( size_t * LenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetLen( m_BufQueuePt, LenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int SetSz( size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueSetSz( m_BufQueuePt, SzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSz( size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return BufQueueGetSz( m_BufQueuePt, SzBytPt, IsAutoLock, ErrInfoVstrPt ); }
};
#endif
