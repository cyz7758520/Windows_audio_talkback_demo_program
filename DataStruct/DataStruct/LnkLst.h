#ifndef __LNKLST_H__
#define __LNKLST_H__

#include "VarStr.h"

#define LNKLST_BUF_AUTO_ADJ_METH_NONE             0      //�������Զ�������ʽΪ�ա�
#define LNKLST_BUF_AUTO_ADJ_METH_FREERATIO        1      //�������Զ�������ʽΪ�����б��ʣ��Զ���������Ϊ����Ԫ�صı��ʡ�
#define LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER       2      //�������Զ�������ʽΪ�����и������Զ���������Ϊ����Ԫ�صĸ�����

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


typedef struct ConstLenLnkLst ConstLenLnkLst;

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstInit( ConstLenLnkLst * * ConstLenLnkLstPtPt, size_t ElmDataLen, int BufAutoAdjMeth, float BufAutoAdjParm, size_t ElmMinNum, size_t ElmMaxNum, int32_t IsUseMutexLock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutHead( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewHeadElmDataPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutTail( ConstLenLnkLst * ConstLenLnkLstPt, const void * NewTailElmDataPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutPrevByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstPutNextByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetHead( ConstLenLnkLst * ConstLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, void * * HeadElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTail( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, void * * TailElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, void * * PrevElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, void * * NextElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, void * * SpecElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, void * * SpecElmDataPtPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstGetTotal( ConstLenLnkLst * ConstLenLnkLstPt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelHead( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelTail( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelPrevByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelNextByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByIdx( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelByNum( ConstLenLnkLst * ConstLenLnkLstPt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDelAll( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int ConstLenLnkLstDestroy( ConstLenLnkLst * ConstLenLnkLstPt, VarStr * ErrInfoVarStrPt );


typedef struct VarLenLnkLst VarLenLnkLst;

__DATASTRUCT_DLLAPI__ int VarLenLnkLstInit( VarLenLnkLst * * VarLenLnkLstPtPt, int BufAutoAdjMeth, float BufAutoAdjParm, size_t BufMinSz, size_t BufMaxSz, int32_t IsUseMutexLock, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutHead( VarLenLnkLst * VarLenLnkLstPt, const void * NewHeadElmDataPt, size_t NewHeadElmDataLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutTail( VarLenLnkLst * VarLenLnkLstPt, const void * NewTailElmDataPt, size_t NewTailElmDataLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutPrevByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewPrevElmDataPt, size_t NewPrevElmDataLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, const void * NewNextElmDataPt, size_t NewNextElmDataLen, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstPutNextByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, const void * NewNextElmDataPt, size_t NewNextElmDataLen, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetHead( VarLenLnkLst * VarLenLnkLstPt, size_t * HeadElmIdxPt, void * HeadElmDataPt, size_t HeadElmDataSz, void * * HeadElmDataPtPt, size_t * HeadElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTail( VarLenLnkLst * VarLenLnkLstPt, size_t * TailElmIdxPt, void * TailElmDataPt, size_t TailElmDataSz, void * * TailElmDataPtPt, size_t * TailElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * PrevElmIdxPt, void * PrevElmDataPt, size_t PrevElmDataSz, void * * PrevElmDataPtPt, size_t * PrevElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, size_t * NextElmIdxPt, void * NextElmDataPt, size_t NextElmDataSz, void * * NextElmDataPtPt, size_t * NextElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, size_t * SpecElmIdxPt, void * SpecElmDataPt, size_t SpecElmDataSz, void * * SpecElmDataPtPt, size_t * SpecElmDataLenPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstGetTotal( VarLenLnkLst * VarLenLnkLstPt, size_t * TotalPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelHead( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelTail( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelPrevByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelNextByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByIdx( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmIdx, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelByNum( VarLenLnkLst * VarLenLnkLstPt, size_t SpecElmNum, VarStr * ErrInfoVarStrPt );
__DATASTRUCT_DLLAPI__ int VarLenLnkLstDelAll( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

__DATASTRUCT_DLLAPI__ int VarLenLnkLstDestroy( VarLenLnkLst * VarLenLnkLstPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#endif
