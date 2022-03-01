#include "Func.h"

#ifndef __AJB_H__
#define __AJB_H__

//Ajb项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_AJB__ ) //如果正在编译Ajb项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AJB_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AJB_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __AJB_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __AJB_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __AJB_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __AJB_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __AJB_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __AJB_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __AJB_DLLAPI__
	#endif
#endif
	
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct AAjb AAjb;

__AJB_DLLAPI__ int AAjbInit( AAjb * * AAjbPtPt, int32_t SmplRate, int32_t FrmLen, int32_t IsHaveTimeStamp, int32_t TimeStampStep, int32_t InactIsContPut, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, int32_t MaxCntuLostFrmCnt, float AdaptSensitivity, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbLock( AAjb * AAjbPt, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int AAjbUnlock( AAjb * AAjbPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbPutOneFrm( AAjb * AAjbPt, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int AAjbGetOneFrm( AAjb * AAjbPt, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbGetBufFrmCnt( AAjb * AAjbPt, int32_t * CurHaveBufActFrmCntPt, int32_t * CurHaveBufInactFrmCntPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * MaxCntuLostFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbClear( AAjb * AAjbPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int AAjbDstoy( AAjb * AAjbPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class AAjbCls
{
public:
	AAjb * m_AAjbPt;

	AAjbCls() { m_AAjbPt = NULL; }
	~AAjbCls() { Dstoy( NULL ); }

	int Init( int32_t SmplRate, int32_t FrmLen, int32_t IsHaveTimeStamp, int32_t TimeStampStep, int32_t InactIsContPut, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, int32_t MaxCntuLostFrmCnt, float AdaptSensitivity, VarStr * ErrInfoVarStrPt ) { return AAjbInit( &m_AAjbPt, SmplRate, FrmLen, IsHaveTimeStamp, TimeStampStep, InactIsContPut, MinNeedBufFrmCnt, MaxNeedBufFrmCnt, MaxCntuLostFrmCnt, AdaptSensitivity, ErrInfoVarStrPt ); }
	
	int Lock( VarStr * ErrInfoVarStrPt ) { return AAjbLock( m_AAjbPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return AAjbUnlock( m_AAjbPt, ErrInfoVarStrPt ); }
	
	int PutOneFrm( uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return AAjbPutOneFrm( m_AAjbPt, TimeStamp, FrmPt, FrmLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetOneFrm( uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return AAjbGetOneFrm( m_AAjbPt, TimeStampPt, FrmPt, FrmSz, FrmLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int GetBufFrmCnt( int32_t * CurHaveBufActFrmCntPt, int32_t * CurHaveBufInactFrmCntPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * MaxCntuLostFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return AAjbGetBufFrmCnt( m_AAjbPt, CurHaveBufActFrmCntPt, CurHaveBufInactFrmCntPt, CurHaveBufFrmCntPt, MinNeedBufFrmCntPt, MaxNeedBufFrmCntPt, MaxCntuLostFrmCntPt, CurNeedBufFrmCntPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Clear( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return AAjbClear( m_AAjbPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Dstoy( VarStr * ErrInfoVarStrPt ) { int p_Rslt = AAjbDstoy( m_AAjbPt, ErrInfoVarStrPt ); m_AAjbPt = NULL; return p_Rslt; }
};
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VAjb VAjb;

__AJB_DLLAPI__ int VAjbInit( VAjb * * VAjbPtPt, int32_t IsHaveTimeStamp, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, float AdaptSensitivity, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbLock( VAjb * VAjbPt, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int VAjbUnlock( VAjb * VAjbPt, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbPutOneFrm( VAjb * VAjbPt, uint64_t CurTime, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int VAjbGetOneFrm( VAjb * VAjbPt, uint64_t CurTime, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__AJB_DLLAPI__ int VAjbGetOneFrmWantTimeStamp( VAjb * VAjbPt, uint64_t CurTime, uint32_t WantTimeStamp, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbGetBufFrmCnt( VAjb * VAjbPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbClear( VAjb * VAjbPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__AJB_DLLAPI__ int VAjbDstoy( VAjb * VAjbPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VAjbCls
{
public:
	VAjb * m_VAjbPt;

	VAjbCls() { m_VAjbPt = NULL; }
	~VAjbCls() { Dstoy( NULL ); }

	int Init( int32_t IsHaveTimeStamp, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, float AdaptSensitivity, VarStr * ErrInfoVarStrPt ) { return VAjbInit( &m_VAjbPt, IsHaveTimeStamp, MinNeedBufFrmCnt, MaxNeedBufFrmCnt, AdaptSensitivity, ErrInfoVarStrPt ); }
	
	int Lock( VarStr * ErrInfoVarStrPt ) { return VAjbLock( m_VAjbPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return VAjbUnlock( m_VAjbPt, ErrInfoVarStrPt ); }
	
	int PutOneFrm( uint64_t CurTime, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VAjbPutOneFrm( m_VAjbPt, CurTime, TimeStamp, FrmPt, FrmLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetOneFrm( uint64_t CurTime, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VAjbGetOneFrm( m_VAjbPt, CurTime, TimeStampPt, FrmPt, FrmSz, FrmLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetOneFrmWantTimeStamp( uint64_t CurTime, uint32_t WantTimeStamp, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSz, size_t * FrmLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VAjbGetOneFrmWantTimeStamp( m_VAjbPt, CurTime, WantTimeStamp, TimeStampPt, FrmPt, FrmSz, FrmLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int GetBufFrmCnt( int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VAjbGetBufFrmCnt( m_VAjbPt, CurHaveBufFrmCntPt, MinNeedBufFrmCntPt, MaxNeedBufFrmCntPt, CurNeedBufFrmCntPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Clear( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VAjbClear( m_VAjbPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Dstoy( VarStr * ErrInfoVarStrPt ) { int p_Rslt = VAjbDstoy( m_VAjbPt, ErrInfoVarStrPt ); m_VAjbPt = NULL; return p_Rslt; }
};
#endif

#endif