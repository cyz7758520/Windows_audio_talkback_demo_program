#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "VarStr.h"

#define QUEUE_BUF_AUTO_ADJ_METH_FREERATIO        1      //缓冲区自动调整方式为按空闲比率，自动调整参数为空闲元素的比率。
#define QUEUE_BUF_AUTO_ADJ_METH_FREENUMBER       2      //缓冲区自动调整方式为按空闲个数，自动调整参数为空闲元素的个数。

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


typedef struct ConstLenQueue ConstLenQueue;

__DATASTRUCT_DLLAPI__ int ConstLenQueueInit( ConstLenQueue * * ConstLenQueuePtPt, size_t ElmLen, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueuePutHead( ConstLenQueue * ConstLenQueuePt, const void * NewHeadElmPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueuePutTail( ConstLenQueue * ConstLenQueuePt, const void * NewTailElmPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueGetHead( ConstLenQueue * ConstLenQueuePt, void * HeadElmPt, void * * HeadElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTail( ConstLenQueue * ConstLenQueuePt, void * TailElmPt, void * * TailElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, void * SpecElmPt, void * * SpecElmBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueGetTotal( ConstLenQueue * ConstLenQueuePt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDelHead( ConstLenQueue * ConstLenQueuePt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelTail( ConstLenQueue * ConstLenQueuePt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelByNum( ConstLenQueue * ConstLenQueuePt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenQueueDelAll( ConstLenQueue * ConstLenQueuePt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueAdjBufSz( ConstLenQueue * ConstLenQueuePt, size_t AdjSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenQueueDestroy( ConstLenQueue * ConstLenQueuePt, VarStr * ErrInfoVarStrPt );


typedef struct VarLenQueue VarLenQueue;

__DATASTRUCT_DLLAPI__ int VarLenQueueInit( VarLenQueue * * VarLenQueuePtPt, int BufAutoAdjMeth, float BufAutoAdjParm, size_t BufMinSz, size_t BufMaxSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueuePutHead( VarLenQueue * VarLenQueuePt, const void * NewHeadElmPt, size_t NewHeadElmLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueuePutTail( VarLenQueue * VarLenQueuePt, const void * NewTailElmPt, size_t NewTailElmLen, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueGetHead( VarLenQueue * VarLenQueuePt, void * HeadElmPt, size_t HeadElmSz, void * * HeadElmBufPtPt, size_t * HeadElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTail( VarLenQueue * VarLenQueuePt, void * TailElmPt, size_t TailElmSz, void * * TailElmBufPtPt, size_t * TailElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, void * SpecElmPt, size_t SpecElmSz, void * * SpecElmBufPtPt, size_t * SpecElmLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueGetTotal( VarLenQueue * VarLenQueuePt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDelHead( VarLenQueue * VarLenQueuePt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelTail( VarLenQueue * VarLenQueuePt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelByNum( VarLenQueue * VarLenQueuePt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenQueueDelAll( VarLenQueue * VarLenQueuePt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueAdjBufSz( VarLenQueue * VarLenQueuePt, size_t AdjSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenQueueDestroy( VarLenQueue * VarLenQueuePt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
