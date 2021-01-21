#ifndef __MAP_H__
#define __MAP_H__

#include "VarStr.h"

#define MAP_BUF_AUTO_ADJ_METH_FREERATIO        1      //�������Զ�������ʽΪ�����б��ʣ��Զ���������Ϊ����Ԫ�صı��ʡ�
#define MAP_BUF_AUTO_ADJ_METH_FREENUMBER       2      //�������Զ�������ʽΪ�����и������Զ���������Ϊ����Ԫ�صĸ�����

#ifdef __cplusplus
extern "C"
{
#endif

//DataStruct��Ŀ��DLL��̬���ļ����뵼�����źꡣ
#if( defined __NAME_DATASTRUCT__ ) //������ڱ���DataStruct��Ŀ��
	#if( ( defined __MS_VCXX__ ) ) //�������ʹ��MS VC++��������
		#if( defined __COMLIB__ ) //������ڱ���LIB��̬���ļ���
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __COMDLL__ ) //������ڱ���DLL��̬���ļ���
			#define __DATASTRUCT_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //������ڱ���EXE��ִ���ļ���
			#define __DATASTRUCT_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //�������ʹ��Cygwin GCC/G++��Linux GCC/G++��Android GCC/G++��KEIL ARMCLANG/ARMCC��������
		#if( defined __COMLIB__ ) //������ڱ���LIB��̬���ļ���
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __COMDLL__ ) //������ڱ���DLL��̬���ļ���
			#define __DATASTRUCT_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //������ڱ���EXE��ִ���ļ���
			#define __DATASTRUCT_DLLAPI__
		#endif
	#else //�������ʹ��δ֪��������
		#define __DATASTRUCT_DLLAPI__
	#endif
#else //������ڱ���������Ŀ��
	#if( ( defined __MS_VCXX__ ) ) //�������ʹ��MS VC++��������
		#if( defined __LNKLIB__ ) //�����������LIB��̬���ļ���
			#define __DATASTRUCT_DLLAPI__
		#elif( defined __LNKDLL__ ) //�����������DLL��̬���ļ���
			#define __DATASTRUCT_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //�������ʹ��Cygwin GCC/G++��Linux GCC/G++��Android GCC/G++��KEIL ARMCLANG/ARMCC��������
		#define __DATASTRUCT_DLLAPI__
	#else //�������ʹ��δ֪��������
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
