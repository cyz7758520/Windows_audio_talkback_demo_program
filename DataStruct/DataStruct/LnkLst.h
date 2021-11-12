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

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstInit( ConstLenLnkLst * * ConstLenLnkLstPtPt, size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstLock( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstUnlock( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutHead( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewHeadElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutTail( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewTailElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetHead( ConstLenLnkLst * ConstLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTail( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTotal( ConstLenLnkLst * ConstLenLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelHead( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelTail( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelAll( ConstLenLnkLst * ConstLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDestroy( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class ConstLenLnkLstCls
{
public:
	ConstLenLnkLst * m_ConstLenLnkLstPt;

	ConstLenLnkLstCls() { m_ConstLenLnkLstPt = NULL; }
	~ConstLenLnkLstCls() { Destroy( NULL ); }

	int Init( size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstInit( &m_ConstLenLnkLstPt, ElmDataLen, BufAutoAdjMeth, BufAutoAdjParm, ElmMinNum, ElmMaxNum, ErrInfoVarStrPt ); }
	
	int Lock( VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstLock( m_ConstLenLnkLstPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstUnlock( m_ConstLenLnkLstPt, ErrInfoVarStrPt ); }

	int PutHead( const void * NewHeadElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutHead( m_ConstLenLnkLstPt, NewHeadElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutTail( const void * NewTailElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutTail( m_ConstLenLnkLstPt, NewTailElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutPrevByNum( m_ConstLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstPutNextByNum( m_ConstLenLnkLstPt, SpecElmNum, NewNextElmDataPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetHead( m_ConstLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, void ** TailElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetTail( m_ConstLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetByIdx( m_ConstLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void ** SpecElmDataPtPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetByNum( m_ConstLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataPtPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstGetTotal( m_ConstLenLnkLstPt, ElmTotalPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int DelHead( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelHead( m_ConstLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelTail( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelTail( m_ConstLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelPrevByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelNextByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelByIdx( m_ConstLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelByNum( m_ConstLenLnkLstPt, SpecElmNum, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelAll( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return ConstLenLnkLstDelAll( m_ConstLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Destroy( VarStr * ErrInfoVarStrPt ) { int p_Result = ConstLenLnkLstDestroy( m_ConstLenLnkLstPt, ErrInfoVarStrPt ); m_ConstLenLnkLstPt = NULL; return p_Result; }
};
#endif


#ifdef __cplusplus
extern "C"
{
#endif
	
typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, int BufAutoAdjMeth, float BufAutoAdjParm, size_t BufMinSz, size_t BufMaxSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstLock( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstUnlock( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTotal( VarLenLnkLst * VarLenLnkLstPt, size_t * ElmTotalPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelHead( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelTail( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelAll( VarLenLnkLst * VarLenLnkLstPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDestroy( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class VarLenLnkLstCls
{
public:
	VarLenLnkLst * m_VarLenLnkLstPt;

	VarLenLnkLstCls() { m_VarLenLnkLstPt = NULL; }
	~VarLenLnkLstCls() { Destroy( NULL ); }

	int Init( int BufAutoAdjMeth, float BufAutoAdjParm, size_t BufMinSz, size_t BufMaxSz, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstInit( &m_VarLenLnkLstPt, BufAutoAdjMeth, BufAutoAdjParm, BufMinSz, BufMaxSz, ErrInfoVarStrPt ); }
	
	int Lock( VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstLock( m_VarLenLnkLstPt, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstUnlock( m_VarLenLnkLstPt, ErrInfoVarStrPt ); }

	int PutHead( const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutHead( m_VarLenLnkLstPt, NewHeadElmDataPt, NewHeadElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutTail( const void * NewTailElmDataPt, size_t NewTailElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutTail( m_VarLenLnkLstPt, NewTailElmDataPt, NewTailElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutPrevByIdx( size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutPrevByNum( size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutPrevByNum( m_VarLenLnkLstPt, SpecElmNum, NewPrevElmDataPt, NewPrevElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutNextByIdx( size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NewNextElmDataPt, NewNextElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int PutNextByNum( size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstPutNextByNum( m_VarLenLnkLstPt, SpecElmNum, NewNextElmDataPt, NewNextElmDataLen, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int GetHead( size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetHead( m_VarLenLnkLstPt, HeadElmIdxPt, HeadElmDataPt, HeadElmDataSz, HeadElmDataPtPt, HeadElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetTail( size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetTail( m_VarLenLnkLstPt, TailElmIdxPt, TailElmDataPt, TailElmDataSz, TailElmDataPtPt, TailElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetPrevByIdx( size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, PrevElmIdxPt, PrevElmDataPt, PrevElmDataSz, PrevElmDataPtPt, PrevElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetNextByIdx( size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, NextElmIdxPt, NextElmDataPt, NextElmDataSz, NextElmDataPtPt, NextElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetByIdx( size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetByIdx( m_VarLenLnkLstPt, SpecElmIdx, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetByNum( size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetByNum( m_VarLenLnkLstPt, SpecElmNum, SpecElmIdxPt, SpecElmDataPt, SpecElmDataSz, SpecElmDataPtPt, SpecElmDataLenPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int GetTotal( size_t * ElmTotalPt, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstGetTotal( m_VarLenLnkLstPt, ElmTotalPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int DelHead( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelHead( m_VarLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelTail( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelTail( m_VarLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelPrevByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelPrevByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelNextByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelNextByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelByIdx( size_t SpecElmIdx, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelByIdx( m_VarLenLnkLstPt, SpecElmIdx, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelByNum( size_t SpecElmNum, int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelByNum( m_VarLenLnkLstPt, SpecElmNum, IsAutoLockUnlock, ErrInfoVarStrPt ); }
	int DelAll( int32_t IsAutoLockUnlock, VarStr * ErrInfoVarStrPt ) { return VarLenLnkLstDelAll( m_VarLenLnkLstPt, IsAutoLockUnlock, ErrInfoVarStrPt ); }

	int Destroy( VarStr * ErrInfoVarStrPt ) { int p_Result = VarLenLnkLstDestroy( m_VarLenLnkLstPt, ErrInfoVarStrPt ); m_VarLenLnkLstPt = NULL; return p_Result; }
};
#endif

#endif
