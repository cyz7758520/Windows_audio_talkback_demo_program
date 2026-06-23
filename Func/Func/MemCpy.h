#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

//__FUNC_DLLAPI__ void * __cdecl memcpy( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
//__FUNC_DLLAPI__ void * __cdecl memmove( void * DstPt, void const * SrcPt, size_t SzByt ); //这里不需要声明，因为编译器会报重定义，不影响调用。
__FUNC_DLLAPI__ void * MemCpy( void * DstPt, void const * SrcPt, size_t SzByt );

#if( ( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) || ( defined __LINUX_GCC__ ) || ( defined __ANDROID_NDK__ ) || ( defined __HARMONY_NDK__ ) ) && ( ( defined __X86__ ) || ( defined __X64__ ) ) )
__FUNC_DLLAPI__ void * MemCpyAvx( void * DstPt, void const * SrcPt, size_t SzByt );
__FUNC_DLLAPI__ void * MemCpySse2( void * DstPt, void const * SrcPt, size_t SzByt );
#endif

#ifdef __cplusplus
}
#endif
