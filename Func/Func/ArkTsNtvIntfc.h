#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

__FUNC_DLLAPI__ extern __thread napi_env g_NapiEnv;

#define GetArkTsVarVal( env, Var, ValPt ) _GetArkTsVarVal( env, Var, GetDataTyp( *ValPt ), ValPt )
__FUNC_DLLAPI__ int _GetArkTsVarVal( napi_env env, napi_value Var, DataTyp ValDt, const void * ValPt );
__FUNC_DLLAPI__ int GetArkTsVarStrToMem( napi_env env, napi_value Var, void * StrPt, ChrSet ChrSet_, size_t SzChr, size_t * LenChrPt );
__FUNC_DLLAPI__ int GetArkTsVarStrToMalloc( napi_env env, napi_value Var, void * * StrPtPt, ChrSet ChrSet_, size_t * LenChrPt );

#define SetArkTsObjMbrVarVal( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, ValType, ValPt ) _SetArkTsObjMbrVarVal( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, ValType, GetDataTyp( *ValPt ), ValPt )
__FUNC_DLLAPI__ int _SetArkTsObjMbrVarVal( napi_env env, napi_value Obj, const char * ModPathU8str, const char * MbrVarNameU8str, napi_value * MbrVarPt, napi_valuetype ValType, DataTyp ValDt, const void * ValPt );
#define SetArkTsObjMbrVarStr( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPt, ChrSet_, LenChr ) _SetArkTsObjMbrVarStr( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPt, ChrSet_, LenChr )
__FUNC_DLLAPI__ int _SetArkTsObjMbrVarStr( napi_env env, napi_value Obj, const char * ModPathU8str, const char * MbrVarNameU8str, napi_value * MbrVarPt, void * StrPt, ChrSet ChrSet_, size_t LenChr );

#define GetArkTsObjMbrVarVal( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, ValPt ) _GetArkTsObjMbrVarVal( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, GetDataTyp( *ValPt ), ValPt )
__FUNC_DLLAPI__ int _GetArkTsObjMbrVarVal( napi_env env, napi_value Obj, const char * ModPathU8str, const char * MbrVarNameU8str, napi_value * MbrVarPt, DataTyp ValDt, const void * ValPt );
#define GetArkTsObjMbrVarStrToMem( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPt, ChrSet_, SzChr, LenChrPt ) _GetArkTsObjMbrVarStrToMem( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPt, ChrSet_, SzChr, LenChrPt )
__FUNC_DLLAPI__ int _GetArkTsObjMbrVarStrToMem( napi_env env, napi_value Obj, const char * ModPathU8str, const char * MbrVarNameU8str, napi_value * MbrVarPt, void * StrPt, ChrSet ChrSet_, size_t SzChr, size_t * LenChrPt );
#define GetArkTsObjMbrVarStrToMalloc( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPtPt, ChrSet_, LenChrPt ) _GetArkTsObjMbrVarStrToMalloc( env, Obj, ModPathU8str, MbrVarNameU8str, MbrVarPt, StrPtPt, ChrSet_, LenChrPt )
__FUNC_DLLAPI__ int _GetArkTsObjMbrVarStrToMalloc( napi_env env, napi_value Obj, const char * ModPathU8str, const char * MbrVarNameU8str, napi_value * MbrVarPt, void * * StrPtPt, ChrSet ChrSet_, size_t * LenChrPt );

#define SetArkTsHTPtObjVal( env, HTPtObj, PtValPt ) SetArkTsObjMbrVarVal( env, HTPtObj, NULL, "m_Val", NULL, napi_external, PtValPt )
#define GetArkTsHTPtObjVal( env, HTPtObj, PtValPt ) GetArkTsObjMbrVarVal( env, HTPtObj, NULL, "m_Val", NULL, PtValPt )

#define SetArkTsHTNumberObjVal( env, HTNumberObj, NumberValPt ) SetArkTsObjMbrVarVal( env, HTNumberObj, NULL, "m_Val", NULL, napi_number, NumberValPt )
#define GetArkTsHTNumberObjVal( env, HTNumberObj, NumberValPt ) GetArkTsObjMbrVarVal( env, HTNumberObj, NULL, "m_Val", NULL, NumberValPt )

#define SetArkTsHTStringObjVal( env, HTStringObj, StrPt, ChrSet_, LenChr ) SetArkTsObjMbrVarStr( env, HTStringObj, NULL, "m_Val", NULL, StrPt, ChrSet_, LenChr )
#define GetArkTsHTStringObjValToMem( env, HTStringObj, StrPt, ChrSet_, SzChr, LenChrPt ) GetArkTsObjMbrVarStrToMem( env, HTStringObj, NULL, "m_Val", NULL, StrPt, ChrSet_, SzChr, LenChrPt )
#define GetArkTsHTStringObjValToMalloc( env, HTStringObj, StrPtPt, ChrSet_, LenChrPt ) GetArkTsObjMbrVarStrToMalloc( env, HTStringObj, NULL, "m_Val", NULL, StrPtPt, ChrSet_, LenChrPt )

__FUNC_DLLAPI__ int GetHamnyPkgName( napi_env env, uint8_t * PkgNameU8strPt, size_t PkgNameU8strSzChr, size_t * PkgNameU8strLenChrPt );

#ifdef __cplusplus
}
#endif
