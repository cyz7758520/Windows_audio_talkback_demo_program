#ifndef __MAP_H__
#define __MAP_H__

#include "VarStr.h"

#define MAP_BUF_AUTO_ADJ_METH_FREERATIO        1      //缓冲区自动调整方式为按空闲比率，自动调整参数为空闲元素的比率。
#define MAP_BUF_AUTO_ADJ_METH_FREENUMBER       2      //缓冲区自动调整方式为按空闲个数，自动调整参数为空闲元素的个数。

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

typedef struct AVLTreeConstLenMap AVLTreeConstLenMap;

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapInit( AVLTreeConstLenMap * * AVLTreeConstLenMapPtPt, size_t ElmKeySz, size_t ElmValSz, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapPut( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * NewElmKeyPt, const void * NewElmValPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetMin( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t * MinElmIdxPt, void * MinElmKeyPt, const void * * MinElmKeyBufPtPt, void * MinElmValPt, void * * MinElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetMax( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t * MaxElmIdxPt, void * MaxElmKeyPt, const void * * MaxElmKeyBufPtPt, void * MaxElmValPt, void * * MaxElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetSmlByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, size_t * SmlElmIdxPt, void * SmlElmKeyPt, void * SmlElmValPt, void * * SmlElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetSmlByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * SmlElmIdxPt, void * SmlElmKeyPt, void * SmlElmValPt, void * * SmlElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetBigByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, size_t * BigElmIdxPt, void * BigElmKeyPt, void * BigElmValPt, void * * BigElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetBigByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * BigElmIdxPt, void * BigElmKeyPt, void * BigElmValPt, void * * BigElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, void * SpecElmKeyPt, void * SpecElmValPt, void * * SpecElmValBufPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapGetByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, size_t * SpecElmIdxPt, void * SpecElmValPt, void * * SpecElmValBufPtPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelMin( AVLTreeConstLenMap * AVLTreeConstLenMapPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelMax( AVLTreeConstLenMap * AVLTreeConstLenMapPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelByIdx( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelByKey( AVLTreeConstLenMap * AVLTreeConstLenMapPt, const void * SpecElmKeyPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDelAll( AVLTreeConstLenMap * AVLTreeConstLenMapPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapAdjBufSz( AVLTreeConstLenMap * AVLTreeConstLenMapPt, size_t AdjSz, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int AVLTreeConstLenMapDestroy( AVLTreeConstLenMap * AVLTreeConstLenMapPt, VarStr * ErrInfoVarStrPt );

struct RBTreeMap;

__DATASTRUCT_DLLAPI__ int RBTreeMapInit();

struct HashMap;

__DATASTRUCT_DLLAPI__ int HashMapInit();

#ifdef __cplusplus
}
#endif

#endif
