#include "Func.h"

#ifndef __QUEUE_H__
#define __QUEUE_H__

#define QUEUE_BUF_AUTO_ADJ_METH_FREERATIO        1      //缓冲区自动调整方式为按空闲比率，自动调整参数为空闲元素的比率。
#define QUEUE_BUF_AUTO_ADJ_METH_FREENUMBER       2      //缓冲区自动调整方式为按空闲个数，自动调整参数为空闲元素的个数。

//DataStruct项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_DATASTRUCT__ ) //如果正在编译DataStruct项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __DATASTRUCT_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __DATASTRUCT_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __DATASTRUCT_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __DATASTRUCT_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __DATASTRUCT_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __DATASTRUCT_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __DATASTRUCT_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __DATASTRUCT_DLLAPI__
	#endif
#endif

//队列的优点：增删头尾快、改查任意快、遍历快。
//队列的缺点：不能增中间、删中间慢。

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct ConstLenQueue ConstLenQueue;

__DATASTRUCT_DLLAPI__ int ConstLenQueueInit( ConstLenQueue * * ConstLenQueuePtPt, size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueLocked( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueUnlock( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueuePutHead( ConstLenQueue * ConstLenQueuePt, const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueuePutTail( ConstLenQueue * ConstLenQueuePt, const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueGetHead( ConstLenQueue * ConstLenQueuePt, void * HeadElmDataPt, void * * HeadElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTail( ConstLenQueue * ConstLenQueuePt, void * TailElmDataPt, void * * TailElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTotal( ConstLenQueue * ConstLenQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDelHead( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelTail( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelAll( ConstLenQueue * ConstLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDstoy( ConstLenQueue * ConstLenQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ConstLenQueueCls
{
public:
	ConstLenQueue * m_ConstLenQueuePt;

	ConstLenQueueCls() { m_ConstLenQueuePt = NULL; }
	~ConstLenQueueCls() { Dstoy( NULL ); }

	int Init( size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt ) { return ConstLenQueueInit( &m_ConstLenQueuePt, ElmDataLen, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, ElmMinNum, ElmMaxNum, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return ConstLenQueueLocked( m_ConstLenQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return ConstLenQueueUnlock( m_ConstLenQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueuePutHead( m_ConstLenQueuePt, NewHeadElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueuePutTail( m_ConstLenQueuePt, NewTailElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, void * * HeadElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetHead( m_ConstLenQueuePt, HeadElmDataPt, HeadElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, void * * TailElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetTail( m_ConstLenQueuePt, TailElmDataPt, TailElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, void * * SpecElmDataBufPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetByNum( m_ConstLenQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataBufPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueGetTotal( m_ConstLenQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelHead( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelTail( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelByNum( m_ConstLenQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenQueueDelAll( m_ConstLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = ConstLenQueueDstoy( m_ConstLenQueuePt, ErrInfoVstrPt ); m_ConstLenQueuePt = NULL; return p_Rslt; }
};
#endif


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct VarLenQueue VarLenQueue;
	
__DATASTRUCT_DLLAPI__ int VarLenQueueInit( VarLenQueue * * VarLenQueuePtPt, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueLocked( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueUnlock( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueuePutHead( VarLenQueue * VarLenQueuePt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueuePutTail( VarLenQueue * VarLenQueuePt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueGetHead( VarLenQueue * VarLenQueuePt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTail( VarLenQueue * VarLenQueuePt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataBufPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTotal( VarLenQueue * VarLenQueuePt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDelHead( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelTail( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelAll( VarLenQueue * VarLenQueuePt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDstoy( VarLenQueue * VarLenQueuePt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarLenQueueCls
{
public:
	VarLenQueue * m_VarLenQueuePt;

	VarLenQueueCls() { m_VarLenQueuePt = NULL; }
	~VarLenQueueCls() { Dstoy( NULL ); }

	int Init( int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt ) { return VarLenQueueInit( &m_VarLenQueuePt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSz, BufMaxSz, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return VarLenQueueLocked( m_VarLenQueuePt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenQueueUnlock( m_VarLenQueuePt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutHead( m_VarLenQueuePt, NewHeadElmDataPt, NewHeadElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueuePutTail( m_VarLenQueuePt, NewTailElmDataPt, NewTailElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	
	int GetHead( void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataBufPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetHead( m_VarLenQueuePt, HeadElmDataPt, HeadElmDataSz, HeadElmDataBufPtPt, HeadElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataBufPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTail( m_VarLenQueuePt, TailElmDataPt, TailElmDataSz, TailElmDataBufPtPt, TailElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataBufPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetByNum( m_VarLenQueuePt, SpecElmNum, SpecElmDataPt, SpecElmDataSz, SpecElmDataBufPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueGetTotal( m_VarLenQueuePt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelHead( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelTail( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelByNum( m_VarLenQueuePt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenQueueDelAll( m_VarLenQueuePt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VarLenQueueDstoy( m_VarLenQueuePt, ErrInfoVstrPt ); m_VarLenQueuePt = NULL; return p_Rslt; }
};
#endif

#endif
