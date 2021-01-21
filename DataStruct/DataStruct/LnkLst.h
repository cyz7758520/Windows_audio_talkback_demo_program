#ifndef __LNKLST_H__
#define __LNKLST_H__

#include "VarStr.h"

#define LNKLST_BUF_AUTO_ADJ_METH_FREERATIO        1      //缓冲区自动调整方式为按空闲比率，自动调整参数为空闲元素的比率。
#define LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER       2      //缓冲区自动调整方式为按空闲个数，自动调整参数为空闲元素的个数。

#ifdef __cplusplus
extern "C"
{
#endif
	
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


typedef struct ConstLenLnkLst ConstLenLnkLst;

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstInit( ConstLenLnkLst * * ConstLenLnkLstPtPt, size_t ElmSz, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutHead( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewHeadElmPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutTail( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewTailElmPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetHead( ConstLenLnkLst * ConstLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmPt, void * * HeadElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTail( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmPt, void * * TailElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmPt, void * * PrevElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmPt, void * * NextElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, void * SpecElmPt, void * * SpecElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmPt, void * * SpecElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTotal( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelHead( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelTail( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelAll( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstAdjBufSz( ConstLenLnkLst * ConstLenLnkLstPt, size_t AdjSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDestroy( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );


typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, int BufAutoAdjMeth, float BufAutoAdjParm, size_t BufMinSz, size_t BufMaxSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmPt, size_t NewHeadElmLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmPt, size_t NewTailElmLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmPt, size_t NewPrevElmLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmPt, size_t NewNextElmLen, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmPt, size_t HeadElmSz, void * * HeadElmBufPtPt, size_t * HeadElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmPt, size_t TailElmSz, void * * TailElmBufPtPt, size_t * TailElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmPt, size_t PrevElmSz, void * * PrevElmBufPtPt, size_t * PrevElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmPt, size_t NextElmSz, void * * NextElmBufPtPt, size_t * NextElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmPt, size_t SpecElmSz, void * * SpecElmBufPtPt, size_t * SpecElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmPt, size_t SpecElmSz, void * * SpecElmBufPtPt, size_t * SpecElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTotal( VarLenLnkLst * VarLenLnkLstPt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelHead( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelTail( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelAll( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstAdjBufSz( VarLenLnkLst * VarLenLnkLstPt, size_t AdjSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDestroy( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
