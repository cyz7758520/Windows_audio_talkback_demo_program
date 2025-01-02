#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CQueue CQueue;

__DATASTRUCT_DLLAPI__ int CQueueInit( CQueue * * CQueuePtPt, size_t ElmDataLenByt, size_t ElmInitNum, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueDstoy( CQueue * CQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CQueueLocked( CQueue * CQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueUnlock( CQueue * CQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CQueuePutHead( CQueue * CQueuePt, const void * NewHeadElmDataPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueuePutTail( CQueue * CQueuePt, const void * NewTailElmDataPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CQueueGetHead( CQueue * CQueuePt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueGetTail( CQueue * CQueuePt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueGetByNum( CQueue * CQueuePt, size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CQueueDelHead( CQueue * CQueuePt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueDelTail( CQueue * CQueuePt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueDelByNum( CQueue * CQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueDelAll( CQueue * CQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CQueueGetTotal( CQueue * CQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueSetSz( CQueue * CQueuePt, size_t SzNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CQueueGetSz( CQueue * CQueuePt, size_t * SzNumPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class CQueueCls
{
public:
	CQueue * m_Pt;

	CQueueCls() { m_Pt = NULL; }
	~CQueueCls() { Dstoy( NULL ); }

	int Init( size_t ElmDataLenByt, size_t ElmInitNum, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t ElmMaxNum, Vstr * ErrInfoVstrPt ) { return CQueueInit( &m_Pt, ElmDataLenByt, ElmInitNum, BufAutoExtdMeth, BufAutoExtdParm, ElmMaxNum, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = CQueueDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return CQueueLocked( m_Pt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return CQueueUnlock( m_Pt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueuePutHead( m_Pt, NewHeadElmDataPt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueuePutTail( m_Pt, NewTailElmDataPt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueGetHead( m_Pt, HeadElmDataPt, HeadElmDataPtPt, IsDelHeadElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueGetTail( m_Pt, TailElmDataPt, TailElmDataPtPt, IsDelTailElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueGetByNum( m_Pt, SpecElmNum, SpecElmDataPt, SpecElmDataPtPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueDelHead( m_Pt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueDelTail( m_Pt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueDelByNum( m_Pt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueDelAll( m_Pt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueGetTotal( m_Pt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }
	int SetSz( size_t SzNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueSetSz( m_Pt, SzNum, IsAutoLock, ErrInfoVstrPt ); }
	int GetSz( size_t * SzNumPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CQueueGetSz( m_Pt, SzNumPt, IsAutoLock, ErrInfoVstrPt ); }
};
#endif
