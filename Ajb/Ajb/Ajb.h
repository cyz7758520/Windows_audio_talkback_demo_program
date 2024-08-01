#pragma once

#include "Func.h"

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

__AJB_DLLAPI__ int AAjbGetAppLmtInfo( Vstr * LmtAppNameVstrPt, Vstr * CurAppNameVstrPt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbInit( AAjb * * AAjbPtPt, int32_t SmplRate, size_t FrmLenUnit, int32_t IsHaveTimeStamp, int32_t TimeStampStep, int32_t InactIsContPut, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, int32_t MaxCntuLostFrmCnt, float AdaptSensitivity, int32_t IsDelObsltFrm, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbLocked( AAjb * AAjbPt, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int AAjbUnlock( AAjb * AAjbPt, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbPutFrm( AAjb * AAjbPt, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int AAjbGetFrm( AAjb * AAjbPt, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSzByt, size_t * FrmLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbGetBufFrmCnt( AAjb * AAjbPt, int32_t * CurHaveBufActFrmCntPt, int32_t * CurHaveBufInactFrmCntPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * MaxCntuLostFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbClear( AAjb * AAjbPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int AAjbReset( AAjb * AAjbPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int AAjbDstoy( AAjb * AAjbPt, Vstr * ErrInfoVstrPt );

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

	static int GetAppLmtInfo( Vstr * LmtAppNameVstrPt, Vstr * CurAppNameVstrPt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt ) { return AAjbGetAppLmtInfo( LmtAppNameVstrPt, CurAppNameVstrPt, LmtTimeSecPt, RmnTimeSecPt, ErrInfoVstrPt ); }

	int Init( int32_t SmplRate, size_t FrmLenUnit, int32_t IsHaveTimeStamp, int32_t TimeStampStep, int32_t InactIsContPut, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, int32_t MaxCntuLostFrmCnt, float AdaptSensitivity, int32_t IsDelObsltFrm, Vstr * ErrInfoVstrPt ) { return AAjbInit( &m_AAjbPt, SmplRate, FrmLenUnit, IsHaveTimeStamp, TimeStampStep, InactIsContPut, MinNeedBufFrmCnt, MaxNeedBufFrmCnt, MaxCntuLostFrmCnt, AdaptSensitivity, IsDelObsltFrm, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return AAjbLocked( m_AAjbPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return AAjbUnlock( m_AAjbPt, ErrInfoVstrPt ); }
	
	int PutFrm( uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return AAjbPutFrm( m_AAjbPt, TimeStamp, FrmPt, FrmLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetFrm( uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSzByt, size_t * FrmLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return AAjbGetFrm( m_AAjbPt, TimeStampPt, FrmPt, FrmSzByt, FrmLenBytPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetBufFrmCnt( int32_t * CurHaveBufActFrmCntPt, int32_t * CurHaveBufInactFrmCntPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * MaxCntuLostFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return AAjbGetBufFrmCnt( m_AAjbPt, CurHaveBufActFrmCntPt, CurHaveBufInactFrmCntPt, CurHaveBufFrmCntPt, MinNeedBufFrmCntPt, MaxNeedBufFrmCntPt, MaxCntuLostFrmCntPt, CurNeedBufFrmCntPt, IsAutoLock, ErrInfoVstrPt ); }

	int Clear( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return AAjbClear( m_AAjbPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = AAjbDstoy( m_AAjbPt, ErrInfoVstrPt ); m_AAjbPt = NULL; return p_Rslt; }
};
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VAjb VAjb;

__AJB_DLLAPI__ int VAjbGetAppLmtInfo( Vstr * LmtAppNameVstrPt, Vstr * CurAppNameVstrPt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbInit( VAjb * * VAjbPtPt, int32_t IsHaveTimeStamp, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, float AdaptSensitivity, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbLocked( VAjb * VAjbPt, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int VAjbUnlock( VAjb * VAjbPt, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbPutFrm( VAjb * VAjbPt, uint64_t CurTime, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int VAjbGetFrm( VAjb * VAjbPt, uint64_t CurTime, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSzByt, size_t * FrmLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbGetBufFrmCnt( VAjb * VAjbPt, int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbClear( VAjb * VAjbPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__AJB_DLLAPI__ int VAjbReset( VAjb * VAjbPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__AJB_DLLAPI__ int VAjbDstoy( VAjb * VAjbPt, Vstr * ErrInfoVstrPt );

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
	
	static int GetAppLmtInfo( Vstr * LmtAppNameVstrPt, Vstr * CurAppNameVstrPt, uint64_t * LmtTimeSecPt, uint64_t * RmnTimeSecPt, Vstr * ErrInfoVstrPt ) { return VAjbGetAppLmtInfo( LmtAppNameVstrPt, CurAppNameVstrPt, LmtTimeSecPt, RmnTimeSecPt, ErrInfoVstrPt ); }

	int Init( int32_t IsHaveTimeStamp, int32_t MinNeedBufFrmCnt, int32_t MaxNeedBufFrmCnt, float AdaptSensitivity, Vstr * ErrInfoVstrPt ) { return VAjbInit( &m_VAjbPt, IsHaveTimeStamp, MinNeedBufFrmCnt, MaxNeedBufFrmCnt, AdaptSensitivity, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VAjbLocked( m_VAjbPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VAjbUnlock( m_VAjbPt, ErrInfoVstrPt ); }
	
	int PutFrm( uint64_t CurTime, uint32_t TimeStamp, const int8_t * FrmPt, size_t FrmLenByt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VAjbPutFrm( m_VAjbPt, CurTime, TimeStamp, FrmPt, FrmLenByt, IsAutoLock, ErrInfoVstrPt ); }
	int GetFrm( uint64_t CurTime, uint32_t * TimeStampPt, int8_t * FrmPt, size_t FrmSzByt, size_t * FrmLenBytPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VAjbGetFrm( m_VAjbPt, CurTime, TimeStampPt, FrmPt, FrmSzByt, FrmLenBytPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetBufFrmCnt( int32_t * CurHaveBufFrmCntPt, int32_t * MinNeedBufFrmCntPt, int32_t * MaxNeedBufFrmCntPt, int32_t * CurNeedBufFrmCntPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VAjbGetBufFrmCnt( m_VAjbPt, CurHaveBufFrmCntPt, MinNeedBufFrmCntPt, MaxNeedBufFrmCntPt, CurNeedBufFrmCntPt, IsAutoLock, ErrInfoVstrPt ); }

	int Clear( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VAjbClear( m_VAjbPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VAjbDstoy( m_VAjbPt, ErrInfoVstrPt ); m_VAjbPt = NULL; return p_Rslt; }
};
#endif
