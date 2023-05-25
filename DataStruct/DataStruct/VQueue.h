#include "DataStruct.h"

#ifndef __VQUEUE_H__
#define __VQUEUE_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VQueue VQueue;
	
__DATASTRUCT_DLLAPI__ int VQueueInit( VQueue * * VQueuePtPt, size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueueLocked( VQueue * VQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueUnlock( VQueue * VQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueuePutHead( VQueue * VQueuePt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueuePutTail( VQueue * VQueuePt, const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueueGetHead( VQueue * VQueuePt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueGetTail( VQueue * VQueuePt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueGetByNum( VQueue * VQueuePt, size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueueDelHead( VQueue * VQueuePt, void * * NewHeadElmDataPtPt, size_t * NewHeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueDelTail( VQueue * VQueuePt, void * * NewTailElmDataPtPt, size_t * NewTailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueDelByNum( VQueue * VQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueDelAll( VQueue * VQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueueGetTotal( VQueue * VQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueSetSz( VQueue * VQueuePt, size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VQueueGetSz( VQueue * VQueuePt, size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VQueueDstoy( VQueue * VQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VQueueCls
{
public:
	VQueue * m_VQueuePt;

	VQueueCls() { m_VQueuePt = NULL; }
	~VQueueCls() { Dstoy( NULL ); }

	int Init( size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt ) { return VQueueInit( &m_VQueuePt, BufInitSzByt, BufAutoExtdMeth, BufAutoExtdParm, BufMaxSzByt, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VQueueLocked( m_VQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VQueueUnlock( m_VQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueuePutHead( m_VQueuePt, NewHeadElmDataPt, NewHeadElmDataLenByt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueuePutTail( m_VQueuePt, NewTailElmDataPt, NewTailElmDataLenByt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueGetHead( m_VQueuePt, HeadElmDataPt, HeadElmDataSzByt, HeadElmDataPtPt, HeadElmDataLenBytPt, IsDelHeadElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueGetTail( m_VQueuePt, TailElmDataPt, TailElmDataSzByt, TailElmDataPtPt, TailElmDataLenBytPt, IsDelTailElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueGetByNum( m_VQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataPtPt, SpecElmDataLenBytPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( void * * NewHeadElmDataPtPt, size_t * NewHeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueDelHead( m_VQueuePt, NewHeadElmDataPtPt, NewHeadElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( void * * NewTailElmDataPtPt, size_t * NewTailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueDelTail( m_VQueuePt, NewTailElmDataPtPt, NewTailElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueDelByNum( m_VQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueDelAll( m_VQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueGetTotal( m_VQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }
	int SetSz( size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueSetSz( m_VQueuePt, SzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSz( size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VQueueGetSz( m_VQueuePt, SzBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VQueueDstoy( m_VQueuePt, ErrInfoVstrPt ); m_VQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
