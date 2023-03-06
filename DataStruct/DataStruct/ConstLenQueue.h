#include "DataStruct.h"

#ifndef __CONSTLENQUEUE_H__
#define __CONSTLENQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ConstLenQueue ConstLenQueue;

__DATASTRUCT_DLLAPI__ int ConstLenQueueInit( ConstLenQueue * * ConstLenQueuePtPt, size_t ElmDataLenByt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueLocked( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueUnlock( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueuePutHead( ConstLenQueue * ConstLenQueuePt, const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueuePutTail( ConstLenQueue * ConstLenQueuePt, const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueGetHead( ConstLenQueue * ConstLenQueuePt, void * HeadElmDataPt, void * * HeadElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTail( ConstLenQueue * ConstLenQueuePt, void * TailElmDataPt, void * * TailElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTotal( ConstLenQueue * ConstLenQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDelHead( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelTail( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelAll( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDstoy( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ConstLenQueueCls
{
public:
	ConstLenQueue * m_ConstLenQueuePt;

	ConstLenQueueCls() { m_ConstLenQueuePt = NULL; }
	~ConstLenQueueCls() { Dstoy( NULL ); }

	int Init( size_t ElmDataLenByt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt ) { return ConstLenQueueInit( &m_ConstLenQueuePt, ElmDataLenByt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, ElmMinNum, ElmMaxNum, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return ConstLenQueueLocked( m_ConstLenQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return ConstLenQueueUnlock( m_ConstLenQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueuePutHead( m_ConstLenQueuePt, NewHeadElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueuePutTail( m_ConstLenQueuePt, NewTailElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, void * * HeadElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetHead( m_ConstLenQueuePt, HeadElmDataPt, HeadElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, void * * TailElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetTail( m_ConstLenQueuePt, TailElmDataPt, TailElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetByNum( m_ConstLenQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetTotal( m_ConstLenQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelHead( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelTail( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelByNum( m_ConstLenQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelAll( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = ConstLenQueueDstoy( m_ConstLenQueuePt, ErrInfoVstrPt ); m_ConstLenQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
