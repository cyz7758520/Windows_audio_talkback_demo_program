#pragma once

#include "Func.h"

#ifdef __cplusplus
extern "C"
{
#endif

//XComponent控件信息。
typedef struct XComponentInfo
{
	char m_NativeXComponentIdU8strArr[ OH_XCOMPONENT_ID_LEN_MAX + 1 ];
	OH_NativeXComponent * m_NativeXComponentPt;
	OHNativeWindow * m_NativeWindowPt;
	uint64_t m_XComponentWidth;
	uint64_t m_XComponentHeight;
}XComponentInfo;

__FUNC_DLLAPI__ int XComponentInfoGetByIdStrVal( napi_env env, napi_value IdStrVal, XComponentInfo * * XComponentInfoPtPt, Vstr * ErrInfoVstrPt );

#ifdef __cplusplus
}
#endif
