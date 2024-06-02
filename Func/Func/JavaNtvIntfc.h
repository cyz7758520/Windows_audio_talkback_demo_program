#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

__FUNC_DLLAPI__ int GetJavaCls( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, jclass * ClsPt );

__FUNC_DLLAPI__ int GetJavaClsMbrVarFieldID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, jfieldID * FieldIDPt );
__FUNC_DLLAPI__ int GetJavaClsMbrFuncMethodID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, jmethodID * MethodIDPt );

__FUNC_DLLAPI__ int SetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, DataTyp ValDt, const void * ValPt );
__FUNC_DLLAPI__ int GetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, DataTyp ValDt, void * ValPt );

__FUNC_DLLAPI__ int CallJavaClsObjMbrFunc( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, void * RtnValPt, ... );

__FUNC_DLLAPI__ int NewJavaStringClsObjByU8str( JNIEnv * env, const uint8_t * U8strPt, jstring * StringClsObjPtPt );
__FUNC_DLLAPI__ int NewJavaStringClsObjByU16str( JNIEnv * env, const uint16_t * U16strPt, jstring * StringClsObjPtPt );

#define SetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) SetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", GetDataTyp( *ShortValPt ), ShortValPt )
#define GetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) GetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", ShortValPt )
#define SetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) SetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", GetDataTyp( *IntValPt ), IntValPt )
#define GetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) GetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", IntValPt )
#define SetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) SetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", GetDataTyp( *LongValPt ), LongValPt )
#define GetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) GetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", LongValPt )
#define SetJavaHTStringClsObjVal( env, HTStringClsObj, StringValPt ) SetJavaClsObjMbrVarVal( env, HTStringClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", GetDataTyp( *StringValPt ), StringValPt )
#define GetJavaHTStringClsObjVal( env, HTStringClsObj, StringValPt ) GetJavaClsObjMbrVarVal( env, HTStringClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", StringValPt )

__FUNC_DLLAPI__ int GetJavaByteArrClsObj( JNIEnv * env, jbyteArray ByteArrClsObj, jbyte * * ByteArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaByteArrClsObj( JNIEnv * env, jbyteArray ByteArrClsObj, jbyte * ByteArrPt );
__FUNC_DLLAPI__ int GetJavaShortArrClsObj( JNIEnv * env, jshortArray ShortArrClsObj, jshort * * ShortArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaShortArrClsObj( JNIEnv * env, jshortArray ShortArrClsObj, jshort * ShortArrPt );
__FUNC_DLLAPI__ int GetJavaIntArrClsObj( JNIEnv * env, jintArray IntArrClsObj, jint * * IntArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaIntArrClsObj( JNIEnv * env, jintArray IntArrClsObj, jint * IntArrPt );
__FUNC_DLLAPI__ int GetJavaLongArrClsObj( JNIEnv * env, jlongArray LongArrClsObj, jlong * * LongArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaLongArrClsObj( JNIEnv * env, jlongArray LongArrClsObj, jlong * LongArrPt );
__FUNC_DLLAPI__ int GetJavaFloatArrClsObj( JNIEnv * env, jfloatArray FloatArrClsObj, jfloat * * FloatArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaFloatArrClsObj( JNIEnv * env, jfloatArray FloatArrClsObj, jfloat * FloatArrPt );
__FUNC_DLLAPI__ int GetJavaDoubleArrClsObj( JNIEnv * env, jdoubleArray DoubleArrClsObj, jdouble * * DoubleArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaDoubleArrClsObj( JNIEnv * env, jdoubleArray DoubleArrClsObj, jdouble * DoubleArrPt );
__FUNC_DLLAPI__ int GetJavaBooleanArrClsObj( JNIEnv * env, jbooleanArray BooleanArrClsObj, jboolean * * BooleanArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaBooleanArrClsObj( JNIEnv * env, jbooleanArray BooleanArrClsObj, jboolean * BooleanArrPt );
__FUNC_DLLAPI__ int GetJavaCharArrClsObj( JNIEnv * env, jcharArray CharArrClsObj, jchar * * CharArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaCharArrClsObj( JNIEnv * env, jcharArray CharArrClsObj, jchar * CharArrPt );
__FUNC_DLLAPI__ int GetJavaStringClsObjU8str( JNIEnv * env, jstring StringClsObj, const uint8_t * * U8strPtPt );
__FUNC_DLLAPI__ int DstoyJavaStringClsObjU8str( JNIEnv * env, jstring StringClsObj, const uint8_t * U8strPt );
__FUNC_DLLAPI__ int GetJavaStringClsObjU16str( JNIEnv * env, jstring StringClsObj, const uint16_t * * CharArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaStringClsObjU16str( JNIEnv * env, jstring StringClsObj, const uint16_t * CharArrPt );

__FUNC_DLLAPI__ int GetAndrdPkgName( JNIEnv * env, uint8_t * PkgNameU8strPt, size_t PkgNameU8strSzChr, size_t * PkgNameU8strLenChrPt );

#ifdef __cplusplus
}
#endif
