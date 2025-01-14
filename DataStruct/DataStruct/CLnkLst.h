﻿#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct CLnkLst CLnkLst;

__DATASTRUCT_DLLAPI__ int CLnkLstInit( CLnkLst * * CLnkLstPtPt, size_t ElmDataLenByt, size_t BufInitSzNum, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzNum, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDstoy( CLnkLst * CLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CLnkLstLocked( CLnkLst * CLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstUnlock( CLnkLst * CLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CLnkLstPutHead( CLnkLst * CLnkLstPt, const void * NewHeadElmDataPt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstPutTail( CLnkLst * CLnkLstPt, const void * NewTailElmDataPt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstPutPrevByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstPutPrevByNum( CLnkLst * CLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstPutNextByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstPutNextByNum( CLnkLst * CLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CLnkLstGetHead( CLnkLst * CLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetTail( CLnkLst * CLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetPrevByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsDelPrevElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetNextByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsDelNextElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetByNum( CLnkLst * CLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CLnkLstDelHead( CLnkLst * CLnkLstPt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelTail( CLnkLst * CLnkLstPt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelPrevByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelNextByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelByIdx( CLnkLst * CLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelByNum( CLnkLst * CLnkLstPt, size_t SpecElmNum, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstDelAll( CLnkLst * CLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int CLnkLstGetTotal( CLnkLst * CLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstSetSz( CLnkLst * CLnkLstPt, size_t SzNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int CLnkLstGetSz( CLnkLst * CLnkLstPt, size_t * SzNumPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class CLnkLstCls
{
public:
	CLnkLst * m_Pt;

	CLnkLstCls() { m_Pt = NULL; }
	~CLnkLstCls() { Dstoy( NULL ); }

	int Init( size_t ElmDataLenByt, size_t BufInitSzNum, BufAutoAdjMeth BufAutoExtdMeth, float BufAutoExtdParm, size_t BufMaxSzNum, Vstr * ErrInfoVstrPt ) { return CLnkLstInit( &m_Pt, ElmDataLenByt, BufInitSzNum, BufAutoExtdMeth, BufAutoExtdParm, BufMaxSzNum, ErrInfoVstrPt ); }
	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = CLnkLstDstoy( m_Pt, ErrInfoVstrPt ); m_Pt = NULL; return p_Rslt; }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return CLnkLstLocked( m_Pt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return CLnkLstUnlock( m_Pt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutHead( m_Pt, NewHeadElmDataPt, NewHeadElmIdxPt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutTail( m_Pt, NewTailElmDataPt, NewTailElmIdxPt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutPrevByIdx( m_Pt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmIdxPt, NewPrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutPrevByNum( m_Pt, SpecElmNum, NewPrevElmDataPt, NewPrevElmIdxPt, NewPrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutNextByIdx( m_Pt, SpecElmIdx, NewNextElmDataPt, NewNextElmIdxPt, NewNextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstPutNextByNum( m_Pt, SpecElmNum, NewNextElmDataPt, NewNextElmIdxPt, NewNextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsDelHeadElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetHead( m_Pt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataPtPt, IsDelHeadElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsDelTailElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetTail( m_Pt, TailElmIdxPt, TailElmDataPt, TailElmDataPtPt, IsDelTailElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsDelPrevElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetPrevByIdx( m_Pt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataPtPt, IsDelPrevElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsDelNextElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetNextByIdx( m_Pt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataPtPt, IsDelNextElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetByIdx( m_Pt, SpecElmIdx, SpecElmDataPt, SpecElmDataPtPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsDelSpecElm, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetByNum( m_Pt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataPtPt, IsDelSpecElm, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( size_t * NewHeadElmIdxPt, void * * NewHeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelHead( m_Pt, NewHeadElmIdxPt, NewHeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( size_t * NewTailElmIdxPt, void * * NewTailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelTail( m_Pt, NewTailElmIdxPt, NewTailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, size_t * NewPrevElmIdxPt, void * * NewPrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelPrevByIdx( m_Pt, SpecElmIdx, NewPrevElmIdxPt, NewPrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, size_t * NewNextElmIdxPt, void * * NewNextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelNextByIdx( m_Pt, SpecElmIdx, NewNextElmIdxPt, NewNextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelByIdx( m_Pt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPtPt, NextElmIdxPt, NextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, size_t * PrevElmIdxPt, void * * PrevElmDataPtPt, size_t * NextElmIdxPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelByNum( m_Pt, SpecElmNum, PrevElmIdxPt, PrevElmDataPtPt, NextElmIdxPt, NextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstDelAll( m_Pt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetTotal( m_Pt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }
	int SetSz( size_t SzNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstSetSz( m_Pt, SzNum, IsAutoLock, ErrInfoVstrPt ); }
	int GetSz( size_t * SzNumPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return CLnkLstGetSz( m_Pt, SzNumPt, IsAutoLock, ErrInfoVstrPt ); }
};
#endif
