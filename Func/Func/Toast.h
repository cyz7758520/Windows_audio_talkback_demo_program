﻿#include "Func.h"

#ifndef __TOAST_H__
#define __TOAST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if( ( defined __MS_VCXX__ ) || ( defined __CYGWIN_GCC__ ) )
__FUNC_DLLAPI__ int Toast( HWND SpecWndHdl, uint64_t TimeOutMesc, Vstr * ErrInfoVstrPt, const Vstr * TextVstrPt );
__FUNC_DLLAPI__ int ToastFmt( HWND SpecWndHdl, uint64_t TimeOutMesc, Vstr * ErrInfoVstrPt, const Vstr * FmtVstrPt, ... );
#endif

#ifdef __cplusplus
}
#endif

#endif