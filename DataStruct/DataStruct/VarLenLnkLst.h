#include "DataStruct.h"

#ifndef __VARLENLNKLST_H__
#define __VARLENLNKLST_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstLocked( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstUnlock( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
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

	int Init( BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstInit( &m_VarLenLnkLstPt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSz, BufMaxSz, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstLocked( m_VarLenLnkLstPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstUnlock( m_VarLenLnkLstPt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutHead( m_VarLenLnkLstPt, NewHeadElmDataPt, NewHeadElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutTail( m_VarLenLnkLstPt, NewTailElmDataPt, NewTailElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByNum( m_VarLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, NewNextElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByNum( m_VarLenLnkLstPt, SpecElmNum, NewNextElmDataPt, NewNextElmDataLen, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetHead( m_VarLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataSz, HeadElmDataPtPt, HeadElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetTail( m_VarLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataSz, TailElmDataPtPt, TailElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataSz, PrevElmDataPtPt, PrevElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataSz, NextElmDataPtPt, NextElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByIdx( m_VarLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByNum( m_VarLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
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
