#include "Func.h"

#ifndef __LNKLST_H__
#define __LNKLST_H__

#define LNKLST_BUF_AUTO_ADJ_METH_NONE             0      //缓冲区自动调整方式为空。
#define LNKLST_BUF_AUTO_ADJ_METH_FREERATIO        1      //缓冲区自动调整方式为按空闲比率，自动调整参数为空闲元素的比率。
#define LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER       2      //缓冲区自动调整方式为按空闲个数，自动调整参数为空闲元素的个数。

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
	
//链表的优点：增删改查头尾快、遍历快。
//链表的缺点：增删改查中间慢。

#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct ConstLenLnkLst ConstLenLnkLst;

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstInit( ConstLenLnkLst * * ConstLenLnkLstPtPt, size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt );

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

	int Init( size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t ElmMinNum, size_t ElmMaxNum, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstInit( &m_ConstLenLnkLstPt, ElmDataLen, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, ElmMinNum, ElmMaxNum, ErrInfoVstrPt ); }
	
	int Locked( Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstLocked( m_ConstLenLnkLstPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstUnlock( m_ConstLenLnkLstPt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutHead( m_ConstLenLnkLstPt, NewHeadElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutTail( m_ConstLenLnkLstPt, NewTailElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutPrevByNum( m_ConstLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstPutNextByNum( m_ConstLenLnkLstPt, SpecElmNum, NewNextElmDataPt, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetHead( m_ConstLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, void ** TailElmDataPtPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return ConstLenLnkLstGetTail( m_ConstLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataPtPt, IsAutoLock, ErrInfoVstrPt ); }
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


#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstLock( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstUnlock( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTotal( VarLenLnkLst * VarLenLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelHead( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelTail( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelAll( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDstoy( VarLenLnkLst * VarLenLnkLstPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarLenLnkLstCls
{
public:
	VarLenLnkLst * m_VarLenLnkLstPt;

	VarLenLnkLstCls() { m_VarLenLnkLstPt = NULL; }
	~VarLenLnkLstCls() { Dstoy( NULL ); }

	int Init( int BufAutoAdjMeth, float BufAutoAdjParm, int BufAutoAdjIsAllowShrink, size_t BufMinSz, size_t BufMaxSz, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstInit( &m_VarLenLnkLstPt, BufAutoAdjMeth, BufAutoAdjParm, BufAutoAdjIsAllowShrink, BufMinSz, BufMaxSz, ErrInfoVstrPt ); }
	
	int Lock( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstLock( m_VarLenLnkLstPt, ErrInfoVstrPt ); }
	int Unlock( Vstr * ErrInfoVstrPt ) { return VarLenLnkLstUnlock( m_VarLenLnkLstPt, ErrInfoVstrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutHead( m_VarLenLnkLstPt, NewHeadElmDataPt, NewHeadElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutTail( m_VarLenLnkLstPt, NewTailElmDataPt, NewTailElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutPrevByNum( m_VarLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, NewNextElmDataLen, IsAutoLock, ErrInfoVstrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstPutNextByNum( m_VarLenLnkLstPt, SpecElmNum, NewNextElmDataPt, NewNextElmDataLen, IsAutoLock, ErrInfoVstrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetHead( m_VarLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataSz, HeadElmDataPtPt, HeadElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetTail( m_VarLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataSz, TailElmDataPtPt, TailElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataSz, PrevElmDataPtPt, PrevElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataSz, NextElmDataPtPt, NextElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByIdx( m_VarLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetByNum( m_VarLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLock, ErrInfoVstrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstGetTotal( m_VarLenLnkLstPt, ElmTotalPt, IsAutoLock, ErrInfoVstrPt ); }

	int DelHead( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelHead( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelTail( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelTail( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByIdx( size_t SpecElmIdx, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLock, ErrInfoVstrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelByNum( m_VarLenLnkLstPt, SpecElmNum, IsAutoLock, ErrInfoVstrPt ); }
	int DelAll( int32_t IsAutoLock, Vstr * ErrInfoVstrPt ) { return VarLenLnkLstDelAll( m_VarLenLnkLstPt, IsAutoLock, ErrInfoVstrPt ); }

	int Dstoy( Vstr * ErrInfoVstrPt ) { int p_Rslt = VarLenLnkLstDstoy( m_VarLenLnkLstPt, ErrInfoVstrPt ); m_VarLenLnkLstPt = NULL; return p_Rslt; }
};
#endif

#endif
