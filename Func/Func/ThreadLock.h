#include "Func.h"

#ifndef __READWRITELOCK_H__
#define __READWRITELOCK_H__

//Func项目的DLL动态库文件导入导出符号宏。
#if( defined __NAME_FUNC__ ) //如果正在编译Func项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllexport )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#if( defined __COMLIB__ ) //如果正在编译LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __COMDLL__ ) //如果正在编译DLL动态库文件。
			#define __FUNC_DLLAPI__ __attribute__( ( visibility( "default" ) ) )
		#elif( defined __COMEXE__ ) //如果正在编译EXE可执行文件。
			#define __FUNC_DLLAPI__
		#endif
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#else //如果正在编译其他项目。
	#if( ( defined __MS_VCXX__ ) ) //如果正在使用MS VC++编译器。
		#if( defined __LNKLIB__ ) //如果正在链接LIB静态库文件。
			#define __FUNC_DLLAPI__
		#elif( defined __LNKDLL__ ) //如果正在链接DLL动态库文件。
			#define __FUNC_DLLAPI__ __declspec( dllimport )
		#endif
	#elif( ( defined __LINUX_GCC__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __ANDROID_GCC__ ) || ( defined __KEIL_ARMC__ ) ) //如果正在使用Cygwin GCC/G++、Linux GCC/G++、Android GCC/G++、KEIL ARMCLANG/ARMCC编译器。
		#define __FUNC_DLLAPI__
	#else //如果正在使用未知编译器。
		#define __FUNC_DLLAPI__
	#endif
#endif
	
#ifdef __cplusplus
//互斥锁。
struct MutexLock
{
	#if( defined __MS_VCXX__ )
	std::atomic_flag m_StdAtomicFlag = ATOMIC_FLAG_INIT;
	#elif( defined __CYGWIN_GCC__ )
	unsigned int m_ICE = 0;
	#elif( ( defined __LINUX_GCC__ ) || ( defined __ANDROID_GCC__ ) )
	std::atomic<int> m_StdAtomicInt;
	MutexLock() { m_StdAtomicInt = 0; }
	#endif
};
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MutexLock MutexLock;

__FUNC_DLLAPI__ int MutexLockInit( MutexLock * * MutexLockPtPt, VarStr * ErrInfoVarStrPt );

__FUNC_DLLAPI__ int MutexLockLock( MutexLock * MutexLockPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int MutexLockUnlock( MutexLock * MutexLockPt, VarStr * ErrInfoVarStrPt );

__FUNC_DLLAPI__ int MutexLockDstoy( MutexLock * MutexLockPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class MutexLockCls
{
public:
	MutexLock m_MutexLock;

	MutexLockCls() {}
	~MutexLockCls() {}

	int Lock( VarStr * ErrInfoVarStrPt ) { return MutexLockLock( &m_MutexLock, ErrInfoVarStrPt ); }
	int Unlock( VarStr * ErrInfoVarStrPt ) { return MutexLockUnlock( &m_MutexLock, ErrInfoVarStrPt ); }
};
#endif


#ifdef __cplusplus
//读写锁。
typedef struct RWLock
{
	std::atomic<int> g_ReadCount; //存放读线程个数。
	std::atomic<int> g_WriteCount; //存放写线程个数。
}RWLock;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RWLock RWLock;

__FUNC_DLLAPI__ int RWLockInit( RWLock * * RWLockPtPt, VarStr * ErrInfoVarStrPt );

__FUNC_DLLAPI__ int RWLockRLock( RWLock * RWLockPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int RWLockRUnlock( RWLock * RWLockPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int RWLockWLock( RWLock * RWLockPt, VarStr * ErrInfoVarStrPt );
__FUNC_DLLAPI__ int RWLockWUnlock( RWLock * RWLockPt, VarStr * ErrInfoVarStrPt );

__FUNC_DLLAPI__ int RWLockDstoy( RWLock * RWLockPt, VarStr * ErrInfoVarStrPt );

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class RWLockCls
{
public:
	RWLock m_RWLock;

	RWLockCls() {}
	~RWLockCls() {}

	int RLock( VarStr * ErrInfoVarStrPt ) { return RWLockRLock( &m_RWLock, ErrInfoVarStrPt ); }
	int RUnlock( VarStr * ErrInfoVarStrPt ) { return RWLockRUnlock( &m_RWLock, ErrInfoVarStrPt ); }
	int WLock( VarStr * ErrInfoVarStrPt ) { return RWLockWLock( &m_RWLock, ErrInfoVarStrPt ); }
	int WUnlock( VarStr * ErrInfoVarStrPt ) { return RWLockWUnlock( &m_RWLock, ErrInfoVarStrPt ); }
};
#endif

#endif
