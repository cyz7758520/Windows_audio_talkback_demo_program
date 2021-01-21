#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "VarStr.h"

#define QUEUE_BUF_AUTO_ADJ_METH_FREERATIO        1      //�������Զ�������ʽΪ�����б��ʣ��Զ���������Ϊ����Ԫ�صı��ʡ�
#define QUEUE_BUF_AUTO_ADJ_METH_FREENUMBER       2      //�������Զ�������ʽΪ�����и������Զ���������Ϊ����Ԫ�صĸ�����

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
