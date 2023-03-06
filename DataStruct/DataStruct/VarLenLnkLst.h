#include "DataStruct.h"

#ifndef __VARLENLNKLST_H__
#define __VARLENLNKLST_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSzByt, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstLocked( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstUnlock( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSzByt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSzByt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTotal( VarLenLnkLst * VarLenLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelHead( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelTail( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelAll( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDstoy( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarLenLnkLstCls
{
public:
	VarLenLnkLst * m_VarLenLnkLstPt;

	VarLenLnkLstCls() { m_VarLenLnkLstPt = NULL; }
	~VarLenLnkLstCls() { Dstoy( NULL ); }

	int Init( BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSzByt, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstInit( &m_VarLenLnkLstPt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSzByt, BufMaxSzByt, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstLocked( m_VarLenLnkLstPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstUnlock( m_VarLenLnkLstPt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutHead( m_VarLenLnkLstPt, NewHeadElmDataPt, NewHeadElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutTail( m_VarLenLnkLstPt, NewTailElmDataPt, NewTailElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByNum( m_VarLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, NewPrevElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, NewNextElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByNum( m_VarLenLnkLstPt, SpecElmNum, NewNextElmDataPt, NewNextElmDataLenByt, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetHead( m_VarLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataSzByt, HeadElmDataPtPt, HeadElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetTail( m_VarLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataSzByt, TailElmDataPtPt, TailElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSzByt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataSzByt, PrevElmDataPtPt, PrevElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSzByt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataSzByt, NextElmDataPtPt, NextElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByIdx( m_VarLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataPtPt, SpecElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByNum( m_VarLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataPtPt, SpecElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetTotal( m_VarLenLnkLstPt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelHead( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelTail( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelByNum( m_VarLenLnkLstPt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelAll( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VarLenLnkLstDstoy( m_VarLenLnkLstPt, ErrInfoVstrPt ); m_VarLenLnkLstPt = NULL; return p_Rslt; }
};
#endif

#endif
