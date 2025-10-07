#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

//互斥锁，内存初始时必须保证是全为0。
typedef uint8_t MutexLock;

__FUNC_DLLAPI__ void MutexLocked( MutexLock * MutexLockPt );
__FUNC_DLLAPI__ void MutexUnlock( MutexLock * MutexLockPt );

#ifdef __cplusplus
}
#endif
