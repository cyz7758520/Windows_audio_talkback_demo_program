#pragma once

#include "Func.h"

//互斥锁，内存初始时必须保证是全为0。
#ifndef __cplusplus
struct MutexLock
{
	long m_StdAtomicFlag;
};
#else
struct MutexLock
{
	#if( defined __MS_VCXX__ )

	std::atomic_flag m_StdAtomicFlag = ATOMIC_FLAG_INIT;

	#elif( ( defined __CYGWIN_GCC__ ) )

	std::atomic_flag m_StdAtomicFlag = ATOMIC_FLAG_INIT;

	#elif( ( defined __LINUX_GCC__ ) )

	std::atomic_flag m_StdAtomicFlag = ATOMIC_FLAG_INIT;

	#elif( defined __ANDROID_GCC__ )

	std::atomic<int> m_StdAtomicInt = {};

	#endif
};
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MutexLock MutexLock;

__FUNC_DLLAPI__ void MutexLocked( MutexLock * MutexLockPt );
__FUNC_DLLAPI__ void MutexUnlock( MutexLock * MutexLockPt );

#ifdef __cplusplus
}
#endif


//读写锁，内存初始时必须保证是全为0。
#ifndef __cplusplus
typedef struct RdWrLock
{
	long m_ReadCount; //存放读线程个数。
	long m_WriteCount; //存放写线程个数。
}RdWrLock;
#else
typedef struct RdWrLock
{
	std::atomic<int> m_ReadCount = {}; //存放读线程个数。
	std::atomic<int> m_WriteCount = {}; //存放写线程个数。
}RdWrLock;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct RdWrLock RdWrLock;

__FUNC_DLLAPI__ int RdWrRdLocked( RdWrLock * RdWrLockPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int RdWrRdUnlock( RdWrLock * RdWrLockPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int RdWrWrLocked( RdWrLock * RdWrLockPt, Vstr * ErrInfoVstrPt );
__FUNC_DLLAPI__ int RdWrWrUnlock( RdWrLock * RdWrLockPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif
