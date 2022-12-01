#include "DataStruct.h"

#ifndef __CONSTLENLNKLST_H__
#define __CONSTLENLNKLST_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct ConstLenLnkLst ConstLenLnkLst;

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstInit( ConstLenLnkLst * * ConstLenLnkLstPtPt, size_t ElmDataLen, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstLocked( ConstLenLnkLst * ConstLenLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstUnlock( ConstLenLnkLst * ConstLenLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutHead( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutTail( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetHead( ConstLenLnkLst * ConstLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTail( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTotal( ConstLenLnkLst * ConstLenLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelHead( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelTail( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelAll( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDstoy( ConstLenLnkLst * ConstLenLnkLstPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ConstLenLnkLstCls
{
public:
	ConstLenLnkLst * m_ConstLenLnkLstPt;

	ConstLenLnkLstCls() { m_ConstLenLnkLstPt = NULL; }
	~ConstLenLnkLstCls() { Dstoy( NULL ); }

	int Init( size_t ElmDataLen, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstInit( &m_ConstLenLnkLstPt, ElmDataLen, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, ElmMinNum, ElmMaxNum, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstLocked( m_ConstLenLnkLstPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstUnlock( m_ConstLenLnkLstPt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutHead( m_ConstLenLnkLstPt, NewHeadElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutTail( m_ConstLenLnkLstPt, NewTailElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutPrevByNum( m_ConstLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutNextByNum( m_ConstLenLnkLstPt, SpecElmNum, NewNextElmDataPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetHead( m_ConstLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetTail( m_ConstLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetByIdx( m_ConstLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void ** SpecElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetByNum( m_ConstLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetTotal( m_ConstLenLnkLstPt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelHead( m_ConstLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelTail( m_ConstLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelByNum( m_ConstLenLnkLstPt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstDelAll( m_ConstLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = ConstLenLnkLstDstoy( m_ConstLenLnkLstPt, ErrInfoVstrPt ); m_ConstLenLnkLstPt = NULL; return p_Rslt; }
};
#endif

#endif
