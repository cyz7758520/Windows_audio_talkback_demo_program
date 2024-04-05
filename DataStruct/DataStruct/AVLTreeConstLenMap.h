#pragma once

#include "DataStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AVLTreeConstLenMap AVLTreeConstLenMap;

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapInit( AVLTreeConstLenMap * * AVLTreeConstLenMapPtPt, size_t ElmKeySzByt, size_t ElmValSzByt, BufAutoAdjMeth BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDstoy( AVLTreeConstLenMap * AVLTreeConstLenMapPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapPut( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * NewElmKeyPt, const void * NewElmValPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetMin( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t * MinElmIdxPt, void * MinElmKeyPt, const void * * MinElmKeyBufPtPt, void * MinElmValPt, void * * MinElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetMax( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t * MaxElmIdxPt, void * MaxElmKeyPt, const void * * MaxElmKeyBufPtPt, void * MaxElmValPt, void * * MaxElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetSmlByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, size_t * SmlElmIdxPt, void * SmlElmKeyPt, void * SmlElmValPt, void * * SmlElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetSmlByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * SmlElmIdxPt, void * SmlElmKeyPt, void * SmlElmValPt, void * * SmlElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetBigByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, size_t * BigElmIdxPt, void * BigElmKeyPt, void * BigElmValPt, void * * BigElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetBigByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * BigElmIdxPt, void * BigElmKeyPt, void * BigElmValPt, void * * BigElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, void * SpecElmKeyPt, void * SpecElmValPt, void * * SpecElmValBufPtPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * SpecElmIdxPt, void * SpecElmValPt, void * * SpecElmValBufPtPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelMin( AVLTreeConstLenMap * AVLTreeConstLenMapPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelMax( AVLTreeConstLenMap * AVLTreeConstLenMapPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelAll( AVLTreeConstLenMap * AVLTreeConstLenMapPt, Vstr * ErrInfoVstrPt );

struct RBTreeMap;

__DATASTRUCT_DLLAPI__ int RBTreeMapInit();

struct HashMap;

__DATASTRUCT_DLLAPI__ int HashMapInit();

#ifdef __cplusplus
}
#endif
