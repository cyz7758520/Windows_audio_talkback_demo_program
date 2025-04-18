﻿#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct VLnkLst VLnkLst;

__DATASTRUCT_DLLAPI__ int VLnkLstInit( VLnkLst * * VLnkLstPtPt, size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDstoy( VLnkLst * VLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VLnkLstLocked( VLnkLst * VLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstUnlock( VLnkLst * VLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VLnkLstPutHead( VLnkLst * VLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstPutTail( VLnkLst * VLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstPutPrevByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstPutPrevByNum( VLnkLst * VLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstPutNextByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstPutNextByNum( VLnkLst * VLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VLnkLstGetHead( VLnkLst * VLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetTail( VLnkLst * VLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetPrevByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSzByt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, int32_t IsDelPrevElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetNextByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSzByt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsDelNextElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetByNum( VLnkLst * VLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VLnkLstDelHead( VLnkLst * VLnkLstPt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, size_t * NewHeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelTail( VLnkLst * VLnkLstPt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, size_t * NewTailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelPrevByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, size_t * NewPrevElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelNextByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, size_t * NewNextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelByIdx( VLnkLst * VLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelByNum( VLnkLst * VLnkLstPt, size_t SpecElmNum, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstDelAll( VLnkLst * VLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VLnkLstGetTotal( VLnkLst * VLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstSetSz( VLnkLst * VLnkLstPt, size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VLnkLstGetSz( VLnkLst * VLnkLstPt, size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VLnkLstCls
{
public:
	VLnkLst * m_Pt;

	VLnkLstCls() { m_Pt = NULL; }
	~VLnkLstCls() { Dstoy( NULL ); }

	int Init( size_t BufInitSzByt, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzByt, Vstr * ErrInfoVstrPt ) { return VLnkLstInit( &m_Pt, BufInitSzByt, BufAutoExtdMeth, BufAutoExtdParm, BufMaxSzByt, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VLnkLstDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VLnkLstLocked( m_Pt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VLnkLstUnlock( m_Pt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLenByt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutHead( m_Pt, NewHeadElmDataPt, NewHeadElmDataLenByt, NewHeadElmIdxPt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLenByt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutTail( m_Pt, NewTailElmDataPt, NewTailElmDataLenByt, NewTailElmIdxPt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutPrevByIdx( m_Pt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmDataLenByt, NewPrevElmIdxPt, NewPrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLenByt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutPrevByNum( m_Pt, SpecElmNum, NewPrevElmDataPt, NewPrevElmDataLenByt, NewPrevElmIdxPt, NewPrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutNextByIdx( m_Pt, SpecElmIdx, NewNextElmDataPt, NewNextElmDataLenByt, NewNextElmIdxPt, NewNextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLenByt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstPutNextByNum( m_Pt, SpecElmNum, NewNextElmDataPt, NewNextElmDataLenByt, NewNextElmIdxPt, NewNextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSzByt, void * * HeadElmDataPtPt, size_t * HeadElmDataLenBytPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetHead( m_Pt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataSzByt, HeadElmDataPtPt, HeadElmDataLenBytPt, IsDelHeadElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSzByt, void * * TailElmDataPtPt, size_t * TailElmDataLenBytPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetTail( m_Pt, TailElmIdxPt, TailElmDataPt, TailElmDataSzByt, TailElmDataPtPt, TailElmDataLenBytPt, IsDelTailElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSzByt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, int32_t IsDelPrevElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetPrevByIdx( m_Pt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataSzByt, PrevElmDataPtPt, PrevElmDataLenBytPt, IsDelPrevElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSzByt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsDelNextElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetNextByIdx( m_Pt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataSzByt, NextElmDataPtPt, NextElmDataLenBytPt, IsDelNextElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetByIdx( m_Pt, SpecElmIdx, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataPtPt, SpecElmDataLenBytPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSzByt, void * * SpecElmDataPtPt, size_t * SpecElmDataLenBytPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetByNum( m_Pt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataSzByt, SpecElmDataPtPt, SpecElmDataLenBytPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, size_t * NewHeadElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelHead( m_Pt, NewHeadElmIdxPt, NewHeadElmDataPtPt, NewHeadElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, size_t * NewTailElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelTail( m_Pt, NewTailElmIdxPt, NewTailElmDataPtPt, NewTailElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, size_t * NewPrevElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelPrevByIdx( m_Pt, SpecElmIdx, NewPrevElmIdxPt, NewPrevElmDataPtPt, NewPrevElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, size_t * NewNextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelNextByIdx( m_Pt, SpecElmIdx, NewNextElmIdxPt, NewNextElmDataPtPt, NewNextElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelByIdx( m_Pt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPtPt, PrevElmDataLenBytPt, NextElmIdxPt, NextElmDataPtPt, NextElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * PrevElmDataLenBytPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, size_t * NextElmDataLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelByNum( m_Pt, SpecElmNum, PrevElmIdxPt, PrevElmDataPtPt, PrevElmDataLenBytPt, NextElmIdxPt, NextElmDataPtPt, NextElmDataLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstDelAll( m_Pt, IsAutoLock, ErrInfoVstrPt ); }

	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetTotal( m_Pt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }
	int SetSz( size_t SzByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstSetSz( m_Pt, SzByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetSz( size_t * SzBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VLnkLstGetSz( m_Pt, SzBytPt, IsAutoLock, ErrInfoVstrPt ); }
};
#endif
