﻿#include "Func.h"

#ifndef __JAVANTVINTFC_H__
#define __JAVANTVINTFC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if( defined __ANDROID_GCC__ )
__FUNC_DLLAPI__ int GetJavaCls( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, jclass * ClsPt );

__FUNC_DLLAPI__ int GetJavaClsMbrVarFieldID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, jfieldID * FieldIDPt );
__FUNC_DLLAPI__ int GetJavaClsMbrFuncMethodID( JNIEnv * env, jclass Cls, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, jmethodID * MethodIDPt );

__FUNC_DLLAPI__ int SetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, const void * ValPt );
__FUNC_DLLAPI__ int GetJavaClsObjMbrVarVal( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrVarNameStrPt, const char * MbrVarDataTypeSignStrPt, void * ValPt );

__FUNC_DLLAPI__ int CallJavaClsObjMbrFunc( JNIEnv * env, jobject ClsObj, const char * PkgNameClsNameStrPt, int32_t IsStatic, const char * MbrFuncNameStrPt, const char * MbrFuncDataTypeSignStrPt, void * RtnValPt, ... );

__FUNC_DLLAPI__ int NewJavaStringClsObjByU8str( JNIEnv * env, const char * U8strPt, jstring * StringClsObjPtPt );

#define SetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) SetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", ShortValPt )
#define GetJavaHTShortClsObjVal( env, HTShortClsObj, ShortValPt ) GetJavaClsObjMbrVarVal( env, HTShortClsObj, NULL, 0, "m_Val", "S", ShortValPt )
#define SetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) SetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", IntValPt )
#define GetJavaHTIntClsObjVal( env, HTIntClsObj, IntValPt ) GetJavaClsObjMbrVarVal( env, HTIntClsObj, NULL, 0, "m_Val", "I", IntValPt )
#define SetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) SetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", LongValPt )
#define GetJavaHTLongClsObjVal( env, HTLongClsObj, LongValPt ) GetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "J", LongValPt )
#define SetJavaHTStringClsObjVal( env, HTLongClsObj, StringValPt ) SetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", StringValPt )
#define GetJavaHTStringClsObjVal( env, HTLongClsObj, StringValPt ) GetJavaClsObjMbrVarVal( env, HTLongClsObj, NULL, 0, "m_Val", "Ljava/lang/String;", StringValPt )

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
__FUNC_DLLAPI__ int GetJavaStringClsObjCharArr( JNIEnv * env, jstring StringClsObj, const jchar * * CharArrPtPt );
__FUNC_DLLAPI__ int DstoyJavaStringClsObjCharArr( JNIEnv * env, jstring StringClsObj, const jchar * CharArrPt );
__FUNC_DLLAPI__ int GetJavaStringClsObjU8str( JNIEnv * env, jstring StringClsObj, const char * * U8strPtPt );
__FUNC_DLLAPI__ int DstoyJavaStringClsObjU8str( JNIEnv * env, jstring StringClsObj, const char * U8strPt );

__FUNC_DLLAPI__ int GetAndrdPkgName( JNIEnv * env, char * PkgNameStrPt, size_t PkgNameStrSz, size_t * PkgNameStrLenPt );
#endif

#ifdef __cplusplus
}
#endif

#endif