#include "MediaPocsThrd.h"
#include "qedit.h"

void FreeMediaType( AM_MEDIA_TYPE& mt )
{
	if( mt.cbFormat != 0 )
	{
		CoTaskMemFree( ( PVOID )mt.pbFormat );
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if( mt.pUnk != NULL )
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

// Delete a media type structure that was allocated on the heap.
void DeleteMediaType( AM_MEDIA_TYPE * pmt )
{
	if( pmt != NULL )
	{
		FreeMediaType( *pmt );
		CoTaskMemFree( pmt );
	}
}

DWORD WINAPI AdoInptThrdRun( MediaPocsThrd * MediaPocsThrdPt );
DWORD WINAPI AdoOtptThrdRun( MediaPocsThrd * MediaPocsThrdPt );
DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdInit
 * 功能说明：创建并初始化媒体处理线程。
 * 参数说明：MediaPocsThrdPtPt：[输出]，存放用于存放媒体处理线程指针的指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的指针，不能为NULL。
			 UserPocsessFuncPt：[输入]，存放用户定义的处理函数的指针，不能为NULL。
			 UserDstoyFuncPt：[输入]，存放用户定义的销毁函数的指针，不能为NULL。
			 UserReadAdoVdoInptFrmFuncPt：[输入]，存放用户定义的读取音视频输入帧函数的指针，不能为NULL。
			 UserWriteAdoOtptFrmFuncPt：[输入]，存放用户定义的写入音频输出帧函数的指针，不能为NULL。
			 UserGetPcmAdoOtptFrmFuncPt：[输入]，存放用户定义的获取PCM格式音频输出帧函数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
					   USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt,
					   USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, USER_GET_PCM_ADO_OTPT_FRM_FUNC_PT UserGetPcmAdoOtptFrmFuncPt,
					   USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, USER_GET_YU12_VDO_OTPT_FRM_FUNC_PT UserGetYU12VdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	MediaPocsThrd * p_MediaPocsThrdPt = NULL; //存放媒体处理线程的指针。

	//判断各个变量是否正确。
	if( MediaPocsThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放媒体处理线程指针的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的初始化函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserPocsFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的处理函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的销毁函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserReadAdoVdoInptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的读取音视频输入帧函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserWriteAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入音频输出帧函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserGetPcmAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取PCM格式音频输出帧函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserWriteVdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入视频输出帧函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( UserGetYU12VdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取YU12格式视频输出帧函数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	
	//创建并清空媒体处理线程的内存块。
	p_MediaPocsThrdPt = ( MediaPocsThrd * )calloc( 1, sizeof( MediaPocsThrd ) );
	if( p_MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空媒体处理线程的内存块失败。" ) );
		goto Out;
	}

	p_MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID = WAVE_MAPPER; //设置音频输入设备的标识符。
	p_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID = WAVE_MAPPER; //设置音频输出设备的标识符。
	p_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID = WAVE_MAPPER; //设置视频输入设备的标识符。
	
	//创建并初始化设置文件的完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_StngFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化设置文件的完整路径动态字符串失败。" ) );
		goto Out;
	}
	
	//创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	
	//创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}

	//创建并初始化音频输入文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化音频输入文件完整路径动态字符串失败。" ) );
		goto Out;
	}

	//创建并初始化音频结果文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化音频结果文件完整路径动态字符串失败。" ) );
		goto Out;
	}

	//创建并初始化音频输出文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化音频输出文件完整路径动态字符串失败。" ) );
		goto Out;
	}

	//创建并初始化错误信息动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_ErrInfoVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并初始化错误信息动态字符串失败。" ) );
		goto Out;
	}

	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置最小计时器分辨率失败。" ) );
		goto Out;
	}

	p_MediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_MediaPocsThrdPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的指针。
	p_MediaPocsThrdPt->m_UserPocsFuncPt = UserPocsFuncPt; //设置用户定义的处理函数的指针。
	p_MediaPocsThrdPt->m_UserDstoyFuncPt = UserDstoyFuncPt; //设置用户定义的销毁函数的指针。
	p_MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt = UserReadAdoVdoInptFrmFuncPt; //设置用户定义的读取音视频输入帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt = UserWriteAdoOtptFrmFuncPt; //设置用户定义的写入音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetPcmAdoOtptFrmFuncPt = UserGetPcmAdoOtptFrmFuncPt; //设置用户定义的获取PCM格式音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt = UserWriteVdoOtptFrmFuncPt; //设置用户定义的写入视频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetYU12VdoOtptFrmFuncPt = UserGetYU12VdoOtptFrmFuncPt; //设置用户定义的获取YU12格式视频输出帧函数的指针。

	*MediaPocsThrdPtPt = p_MediaPocsThrdPt; //设置媒体处理线程的指针。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoy( p_MediaPocsThrdPt, NULL );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsSaveStngToFile
 * 功能说明：设置是否保存设置到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveStngToFile：[输入]，存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
			 StngFileFullPathStrPt：[输入]，存放设置文件完整路径字符串的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, int IsSaveStngToFile, const char * StngFileFullPathStrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveStngToFile != 0 ) && ( StngFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置文件完整路径字符串的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_IsSaveStngToFile = IsSaveStngToFile;
	if( IsSaveStngToFile != 0 )
	{
		if( VstrCpy( MediaPocsThrdPt->m_StngFileFullPathVstrPt, Cu8vstr( StngFileFullPathStrPt ) ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置设置文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_IsSaveStngToFile = 0;
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsPrintLogShowToast
 * 功能说明：设置是否打印Log日志、显示Toast。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int IsPrintLog, int IsShowToast, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_IsPrintLog = IsPrintLog;
	MediaPocsThrdPt->m_IsShowToast = IsShowToast;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoInptDvcName
 * 功能说明：获取音频输入设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AdoInptDvcNameVstrArrPtPtPt：[输出]，存放用于存放音频输入设备名称动态字符串数组指针的指针，不能为NULL。
			 AdoInptDvcTotalPt：[输出]，存放用于存放音频输入设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameVstrArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	Vstr * * p_AdoInptDvcNameArrPtPt = NULL; //存放音频输入设备名称动态字符串数组的指针。
	UINT p_AdoInptDvcTotal = 0; //存放音频输入设备的总数。
	WAVEINCAPSW p_WaveInCaps; //存放音频输入设备的功能。
	MMRESULT p_MMRslt;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放音频输入设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( AdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放音频输入设备总数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	p_AdoInptDvcTotal = waveInGetNumDevs();
	*AdoInptDvcTotalPt = p_AdoInptDvcTotal; //设置音频输入设备的总数。

	//设置音频输入设备名称动态字符串数组。
	{
		p_AdoInptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoInptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoInptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输入设备名称动态字符串数组的内存块失败。" ) );
			goto Out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AdoInptDvcTotal; p_TmpUint++ )
		{
			p_MMRslt = waveInGetDevCapsW( p_TmpUint, &p_WaveInCaps, sizeof( p_WaveInCaps ) );
			if( p_MMRslt != MMSYSERR_NOERROR )
			{
				GetWinMMErrInfo( p_MMRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输入设备功能失败。原因：" ) );
				goto Out; //本函数返回失败。
			}
			if( VstrInit( &p_AdoInptDvcNameArrPtPt[p_TmpUint], Utf16, , Cu16vstr( p_WaveInCaps.szPname ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称的动态字符串失败。" ) );
				goto Out; //本函数返回失败。
			}
		}

		*AdoInptDvcNameVstrArrPtPtPt = p_AdoInptDvcNameArrPtPt; //设置音频输入设备名称动态字符串数组的指针。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameArrPtPt, p_AdoInptDvcTotal, NULL );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoOtptDvcName
 * 功能说明：获取音频输出设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AdoOtptDvcNameVstrArrPtPtPt：[输出]，存放用于存放音频输出设备名称动态字符串数组指针的指针，不能为NULL。
			 AdoOtptDvcTotalPt：[输出]，存放用于存放音频输出设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameVstrArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	Vstr * * p_AdoOtptDvcNameArrPtPt = NULL; //存放音频输出设备名称动态字符串数组的指针。
	UINT p_AdoOtptDvcTotal = 0; //存放音频输出设备的总数。
	WAVEOUTCAPSW p_WaveOutCaps; //存放音频输出设备的功能。
	MMRESULT p_MMRslt;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AdoOtptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放音频输出设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( AdoOtptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放音频输出设备总数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	p_AdoOtptDvcTotal = waveOutGetNumDevs();
	*AdoOtptDvcTotalPt = p_AdoOtptDvcTotal; //设置音频输出设备的总数。

	//设置音频输出设备名称动态字符串数组。
	{
		p_AdoOtptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoOtptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoOtptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输出设备名称动态字符串数组的内存块失败。" ) );
			goto Out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AdoOtptDvcTotal; p_TmpUint++ )
		{
			p_MMRslt = waveOutGetDevCapsW( p_TmpUint, &p_WaveOutCaps, sizeof( p_WaveOutCaps ) );
			if( p_MMRslt != MMSYSERR_NOERROR )
			{
				GetWinMMErrInfo( p_MMRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输出设备功能失败。原因：" ) );
				goto Out; //本函数返回失败。
			}
			if( VstrInit( &p_AdoOtptDvcNameArrPtPt[p_TmpUint], Utf16, , Cu16vstr( p_WaveOutCaps.szPname ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输出设备名称的动态字符串失败。" ) );
				goto Out; //本函数返回失败。
			}
		}

		*AdoOtptDvcNameVstrArrPtPtPt = p_AdoOtptDvcNameArrPtPt; //设置音频输出设备名称动态字符串数组的指针。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameArrPtPt, p_AdoOtptDvcTotal, NULL );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetVdoInptDvcName
 * 功能说明：获取视频输入设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：VdoInptDvcNameVstrArrPtPtPt：[输出]，存放用于存放视频输入设备名称动态字符串数组指针的指针，不能为NULL。
			 VdoInptDvcTotalPt：[输出]，存放用于存放视频输入设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameVstrArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	Vstr * * p_VdoInptDvcNameArrPtPt = NULL; //存放视频输入设备名称动态字符串数组的指针。
	Vstr * * p_TmpVstrPtPt = NULL;
	UINT p_VdoInptDvcTotal = 0; //存放视频输入设备的总数。
	ICreateDevEnum * p_CreateDevEnumPt = NULL;
	IEnumMoniker * p_EnumMonikerPt = NULL;
	IMoniker * p_MonikerPt = NULL;
	IPropertyBag * p_PropertyBagPt = NULL;
	ULONG p_Ulong;
	
	//判断各个变量是否正确。
	if( VdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放视频输入设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( VdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用于存放视频输入设备总数的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	//初始化COM库。
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY );

	//创建系统设备枚举器。
	if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建系统设备枚举器失败。" ) );
		goto Out; //本函数返回失败。
	}
	
	if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
	{
		while( p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK )
		{
			if( SUCCEEDED( p_MonikerPt->BindToStorage( NULL, NULL, IID_IPropertyBag, ( void ** )&p_PropertyBagPt ) ) )
			{
				VARIANT varName;
				VariantInit( &varName );
				if( SUCCEEDED( p_PropertyBagPt->Read( L"FriendlyName", &varName, 0 ) ) )
				{
					p_VdoInptDvcTotal++;
					p_TmpVstrPtPt = ( Vstr * * )realloc( p_VdoInptDvcNameArrPtPt, p_VdoInptDvcTotal * sizeof( Vstr * ) );
					if( p_TmpVstrPtPt == NULL )
					{
						VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串数组的内存块失败。" ) );
						goto Out; //本函数返回失败。
					}
					p_VdoInptDvcNameArrPtPt = p_TmpVstrPtPt;
					p_VdoInptDvcNameArrPtPt[p_VdoInptDvcTotal - 1] = NULL;
					if( VstrInit( &p_VdoInptDvcNameArrPtPt[p_VdoInptDvcTotal - 1], Utf16, , Cu16vstr( varName.bstrVal ) ) != 0 )
					{
						VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称的动态字符串失败。" ) );
						goto Out; //本函数返回失败。
					}
				}
				VariantClear( &varName );
				p_Ulong = p_PropertyBagPt->Release();
				p_PropertyBagPt = NULL;
			}
			p_Ulong = p_MonikerPt->Release();
			p_MonikerPt = NULL;
		}
		p_Ulong = p_EnumMonikerPt->Release();
		p_EnumMonikerPt = NULL;
	}
	p_Ulong = p_CreateDevEnumPt->Release();
	p_CreateDevEnumPt = NULL;

	*VdoInptDvcNameVstrArrPtPtPt = p_VdoInptDvcNameArrPtPt; //设置视频输入设备名称动态字符串数组的指针。
	*VdoInptDvcTotalPt = p_VdoInptDvcTotal; //设置视频输入设备的总数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameArrPtPt, p_VdoInptDvcTotal, NULL );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdDstoyDvcName
 * 功能说明：销毁设备名称动态字符串数组。
 * 参数说明：DvcNameArrPtPt：[输出]，存放设备名称动态字符串数组的指针，不能为NULL。
			 DvcTotal：[输出]，存放设备的总数。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDstoyDvcName( Vstr * * DvcNameArrPtPt, UINT DvcTotal, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( DvcNameArrPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设备名称动态字符串数组的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	//循环销毁所有的动态字符串。
	for( p_TmpUint = 0; p_TmpUint < DvcTotal; p_TmpUint++ )
	{
		if( DvcNameArrPtPt[p_TmpUint] != NULL )
		{
			VstrDstoy( DvcNameArrPtPt[p_TmpUint] );
		}
	}

	free( DvcNameArrPtPt ); //销毁设备名称动态字符串数组的内存块。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseAdoInpt
 * 功能说明：设置是否使用音频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseAdoInpt：[输入]，存放是否使用音频输入，为非0表示要使用，为0表示不使用。
			 SmplRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoInpt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAdoInpt != 0 ) && ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAdoInpt != 0 ) && ( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = IsUseAdoInpt;
    MediaPocsThrdPt->m_AdoInpt.m_SmplRate = SmplRate;
    MediaPocsThrdPt->m_AdoInpt.m_FrmLen = FrmLenMsec * SmplRate / 1000;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseNoAec
 * 功能说明：设置音频输入不使用声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseNoAec( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseSpeexAec
 * 功能说明：设置音频输入要使用Speex声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FilterLen：[输入]，存放Speex声学回音消除器滤波器的长度，单位毫秒。
			 IsSaveMemFile：[输入]，存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathStrPt：[输入]，存放Speex声学回音消除器的内存块文件完整路径动态字符串的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "Speex声学回音消除器的内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 1;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen = FilterLen;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec = IsUseRec;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp = EchoMutp;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu = EchoCntu;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes = EchoSupes;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct = EchoSupesAct;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置Speex声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseWebRtcAecm
 * 功能说明：设置音频输入要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 2;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmIsUseCNGMode = IsUseCNGMode;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmEchoMode = EchoMode;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmDelay = Delay;
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseWebRtcAec
 * 功能说明：设置音频输入要使用WebRtc浮点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             EchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             Delay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             IsUseDelayAgstcMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             IsUseExtdFilterMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             IsUseRefinedFilterAdaptAecMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             IsUseAdaptAdjDelay：[输入]，存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
             IsSaveMemFile：[输入]，存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
             MemFileFullPathStrPt：[输入]，存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 3;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode = EchoMode;
    MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay = Delay;
    MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode = IsUseDelayAgstcMode;
    MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode = IsUseExtdFilterMode;
    MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseSpeexWebRtcAec
 * 功能说明：设置音频输入要使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             WorkMode：[输入]，存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
             SpeexAecFilterLen：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器滤波器的长度，单位毫秒。
             SpeexAecEchoMutp：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
             SpeexAecEchoCntu：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
             SpeexAecEchoSupes：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             SpeexAecEchoSupesAct：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             WebRtcAecmIsUseCNGMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
             WebRtcAecmEchoMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
             WebRtcAecmDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecEchoMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             WebRtcAecDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecIsUseDelayAgstcMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseExtdFilterMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseRefinedFilterAdaptAecMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseAdaptAdjDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
			 IsUseSameRoomAec：[输入]，存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
			 SameRoomEchoMinDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位毫秒，取值区间为[1,2147483647]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 4;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWorkMode = WorkMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecFilterLen = SpeexAecFilterLen;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecIsUseRec = SpeexAecIsUseRec;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoMutp = SpeexAecEchoMutp;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoCntu = SpeexAecEchoCntu;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupes = SpeexAecEchoSupes;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmEchoMode = WebRtcAecmEchoMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmDelay = WebRtcAecmDelay;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecEchoMode = WebRtcAecEchoMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecDelay = WebRtcAecDelay;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode = WebRtcAecIsUseDelayAgstcMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecIsUseSameRoomAec = IsUseSameRoomAec;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSameRoomEchoMinDelay = SameRoomEchoMinDelay;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseNoNs
 * 功能说明：设置音频输入不使用噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseNoNs( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetUseSpeexPrpocsNs
 * 功能说明：设置音频输入要使用Speex预处理器的噪音抑制。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseNs：[输入]，存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
			 NoiseSupes：[输入]，存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 IsUseDereverb：[输入]，存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。
			 IsUseRec：[输入]，存放Speex预处理器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
			 EchoMutp：[输入]，存放Speex预处理器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
			 EchoCntu：[输入]，存放Speex预处理器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
			 EchoSupes：[输入]，存放Speex预处理器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 EchoSupesAct：[输入]，存放Speex预处理器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 1;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseNs = IsUseNs;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNoiseSupes = NoiseSupes;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseDereverb = IsUseDereverb;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetUseWebRtcNsx
 * 功能说明：设置音频输入要使用WebRtc定点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 2;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPolicyMode = PolicyMode;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseWebRtcNs
 * 功能说明：设置音频输入要使用WebRtc浮点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 3;
	MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPolicyMode = PolicyMode;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseRNNoise
 * 功能说明：设置音频输入要使用RNNoise噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 4;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptIsUseSpeexPrpocsOther
 * 功能说明：设置音频输入是否使用Speex预处理器的其他功能。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             m_IsUseOther：[输入]，存放Speex预处理器是否使用其他功能，为非0表示要使用，为0表示不使用。
             IsUseVad：[输入]，存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
             VadProbStart：[输入]，存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
             VadProbCntu：[输入]，存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
             IsUseAgc：[输入]，存放Speex预处理器是否使用自动增益控制，为非0表示要使用，为0表示不使用。
             AgcLevel：[输入]，存放Speex预处理器在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
             AgcIncrement：[输入]，存放Speex预处理器在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
             AgcDecrement：[输入]，存放Speex预处理器在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
             AgcMaxGain：[输入]，存放Speex预处理器在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsUseSpeexPrpocsOther( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther = IsUseOther;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad = IsUseVad;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbStart = VadProbStart;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbCntu = VadProbCntu;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseAgc = IsUseAgc;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcLevel = AgcLevel;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcIncrement = AgcIncrement;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcDecrement = AgcDecrement;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcMaxGain = AgcMaxGain;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetUsePcm
 * 功能说明：设置音频输入要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUsePcm( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseSpeexEncd
 * 功能说明：设置音频输入要使用Speex编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             UseCbrOrVbr：[输入]，存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             Qualt：[输入]，存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             Cmplxt：[输入]，存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             PlcExptLossRate：[输入]，存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 1;
	MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdUseCbrOrVbr = UseCbrOrVbr;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdQualt = Qualt;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdCmplxt = Cmplxt;
    MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPlcExptLossRate = PlcExptLossRate;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseOpusEncd
 * 功能说明：设置音频输入要使用Opus编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 2;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptIsSaveAdoToFile
 * 功能说明：设置音频输入是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoInptFileFullPathVstrPt：[输入]，存放音频输入文件完整路径动态字符串的指针。
			 AdoRsltFileFullPathVstrPt：[输入]，存放音频结果文件完整路径动态字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoInptFileFullPathVstrPt, const Vstr * AdoRsltFileFullPathVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoInptFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入文件的完整路径字符串不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoRsltFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频结果文件的完整路径字符串不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile = IsSaveAdoToFile;
	if( IsSaveAdoToFile != 0 )
	{
		if( VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt, AdoInptFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频输入文件完整路径动态字符串失败。" ) );
			goto Out;
		}
		if( VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt, AdoRsltFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频结果文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile = 0;
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd
 * 功能说明：设置音频输入是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoInptWavfmWndHdl：[输入]，存放音频输入波形窗口的句柄。
			 AdoRsltWavfmWndHdl：[输入]，存放音频结果波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoInptWavfmWndHdl, HWND AdoRsltWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoInptWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入波形窗口的句柄不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoRsltWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频结果波形窗口的句柄不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	if( IsDrawAdoWavfmToWnd != 0 )
	{
		MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmWndHdl = AdoInptWavfmWndHdl;
		MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmWndHdl = AdoRsltWavfmWndHdl;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd = 0;
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseDvc
 * 功能说明：设置音频输入使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 AdoInptDvcID：[输入]，存放音频输入设备的标识符，从0到音频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoInptDvcID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( AdoInptDvcID != WAVE_MAPPER ) && ( AdoInptDvcID >= waveInGetNumDevs() ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备的标识符不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID = AdoInptDvcID;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptIsMute
 * 功能说明：设置音频输入是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute = IsMute;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseAdoOtpt
 * 功能说明：设置是否使用音频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseAdoOtpt：[输入]，存放是否使用音频输出，为非0表示要使用，为0表示不使用。
			 SmplRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoOtpt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAdoOtpt != 0 ) && ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAdoOtpt != 0 ) && ( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = IsUseAdoOtpt;
    MediaPocsThrdPt->m_AdoOtpt.m_SmplRate = SmplRate;
    MediaPocsThrdPt->m_AdoOtpt.m_FrmLen = FrmLenMsec * SmplRate / 1000;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptUsePcm
 * 功能说明：设置音频输出要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptUsePcm( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptUseSpeexDecd
 * 功能说明：设置音频输出要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             IsUsePrcplEnhsmt：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd = 1;
    MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdIsUsePrcplEnhsmt = IsUsePrcplEnhsmt;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptUseOpusDecd
 * 功能说明：设置音频输出要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd = 2;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptIsSaveAdoToFile
 * 功能说明：设置音频输出是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoOtptFileFullPathVstrPt：[输入]，存放音频输出文件完整路径字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoOtptFileFullPathVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoOtptFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出文件的完整路径字符串不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile = IsSaveAdoToFile;
	if( IsSaveAdoToFile != 0 )
	{
		if( VstrCpy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt, AdoOtptFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频输出文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile = 0;
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd
 * 功能说明：设置音频输出是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsDrawAdoWavfmToWnd：[输出]，存放是否绘制音频波形到窗口，非0表示要绘制，0表示不绘制。
			 AdoOtptWavfmWndHdl：[输出]，存放音频输出波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoOtptWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出波形窗口的句柄不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	if( IsDrawAdoWavfmToWnd != 0 )
	{
		MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmWndHdl = AdoOtptWavfmWndHdl;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd = 0;
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptUseDvc
 * 功能说明：设置音频输出使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 AdoOtptDvcID：[输入]，存放音频输出设备的标识符，从0到音频输出设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AdoOtptDvcID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( AdoOtptDvcID != WAVE_MAPPER ) && ( AdoOtptDvcID >= waveOutGetNumDevs() ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备的标识符不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID = AdoOtptDvcID;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptIsMute
 * 功能说明：设置音频输出是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIsMute = IsMute;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseVdoInpt
 * 功能说明：设置是否使用视频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVdoInpt：[输入]，存放是否使用视频输入，为非0表示要使用，为0表示不使用。
			 MaxSmplRate：[输入]，存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
			 FrmWidth：[输入]，存放帧的宽度，单位为像素。
			 FrmHeight：[输入]，存放帧的高度，单位为像素。
			 VdoInptPrvwWnd：[输入]，存放视频输入预览窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVdoInpt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVdoInpt != 0 ) && ( ( MaxSmplRate < 1 ) || ( MaxSmplRate > 60 ) ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "最大采样频率不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVdoInpt != 0 ) && ( ( FrmWidth <= 0 ) || ( ( FrmWidth & 1 ) != 0 ) ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的宽度不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVdoInpt != 0 ) && ( ( FrmHeight <= 0 ) || ( ( FrmHeight & 1 ) != 0 ) ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的高度不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = IsUseVdoInpt;
    MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate = MaxSmplRate;
    MediaPocsThrdPt->m_VdoInpt.m_FrmWidth = FrmWidth;
	MediaPocsThrdPt->m_VdoInpt.m_FrmHeight = FrmHeight;
	MediaPocsThrdPt->m_VdoInpt.m_VdoInptPrvwWndHdl = VdoInptPrvwWnd;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoInptUseYU12
 * 功能说明：设置视频输入要使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInptUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoInptUseOpenH264Encd
 * 功能说明：设置视频输入要使用OpenH264编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoType：[输入]，存放OpenH264编码器的视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
             EncdBitrate：[输入]，存放OpenH264编码器的编码后比特率，单位为bps。
             BitrateCtrlMode：[输入]，存放OpenH264编码器的比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
             IDRFrmIntvl：[输入]，存放OpenH264编码器的IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
             Cmplxt：[输入]，存放OpenH264编码器的复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,2]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInptUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 1;
	MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdVdoType = VdoType;
    MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdEncdBitrate = EncdBitrate;
    MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdBitrateCtrlMode = BitrateCtrlMode;
    MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdIDRFrmIntvl = IDRFrmIntvl;
	MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdCmplxt = Cmplxt;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoInptUseDvc
 * 功能说明：设置视频输入使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 VdoInptDvcID：[输入]，存放视频输入设备的标识符，从0到视频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInptUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT VdoInptDvcID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID = VdoInptDvcID;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoInptIsBlack
 * 功能说明：设置视频输入是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输入]，存放视频输入设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInptIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VdoInpt.m_VdoInptIsBlack = IsBlack;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseVdoOtpt
 * 功能说明：设置是否使用视频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVdoOtpt：[输入]，存放是否使用视频输出，为非0表示要使用，为0表示不使用。
			 FrmWidth：[输入]，存放帧的宽度，单位为像素。
			 FrmHeight：[输入]，存放帧的高度，单位为像素。
			 VdoOtptDspyWnd：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 VdoOtptDspyScale：[输入]，存放视频输出缩放的倍数。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseVdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVdoOtpt, HWND VdoOtptDspyWnd, float VdoOtptDspyScale, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = IsUseVdoOtpt;
	MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyWndHdl = VdoOtptDspyWnd;
	MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale = VdoOtptDspyScale;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoOtptUseYU12
 * 功能说明：设置视频输出要使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptUseYU12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd = 0;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoOtptUseOpenH264Decd
 * 功能说明：设置视频输出要使用OpenH264解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             DecdThrdNum：[输出]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd = 1;
	MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdDecdThrdNum = DecdThrdNum;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoOtptIsBlack
 * 功能说明：设置视频输出是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptIsBlack = IsBlack;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdStart
* 功能说明：启动媒体处理线程。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdStart( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( MediaPocsThrdPt->m_MediaPocsThrdHdl != NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程已经启动。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_MediaPocsThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )MediaPocsThrdRun, MediaPocsThrdPt, 0, NULL ); //创建并启动媒体处理线程。
	if( MediaPocsThrdPt->m_MediaPocsThrdHdl == NULL )
	{
		GetLastErrInfo( , ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "创建并启动媒体处理线程失败。原因：" ) );
		goto Out; //本函数返回失败。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdRqirExit
* 功能说明：请求媒体处理线程退出。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
            ExitFlag：[输入]，存放退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
			IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示阻塞。
			ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int ExitFlag, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "退出标记不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_ExitFlag = ExitFlag; //设置媒体处理线程的退出标记。

	if( IsBlockWait != 0 ) //如果需要阻塞等待。
    {
        if( ExitFlag == 1 ) //如果是请求退出。
		{
			do
			{
				if( MediaPocsThrdPt->m_MediaPocsThrdHdl == NULL ) //如果媒体处理线程的句柄为空，表示媒体处理线程已经退出。
				{
					break;
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			} while( true );
		}
		else //如果是请求重启。
        {
            //等待重启完毕。
            do
            {
				if( MediaPocsThrdPt->m_MediaPocsThrdHdl == NULL ) //如果媒体处理线程的句柄为空，表示媒体处理线程已经退出。
				{
					break;
				}
				if( MediaPocsThrdPt->m_ExitFlag == 0 ) //如果退出标记为0保持运行，表示重启完毕。
				{
					break;
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			} while( true );
        }
    }

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdDstoy
* 功能说明：销毁媒体处理线程。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 1, NULL ); //请求媒体处理线程退出，并阻塞等待。

	//销毁设置文件的完整路径动态字符串。
	if( MediaPocsThrdPt->m_StngFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_StngFileFullPathVstrPt );
		MediaPocsThrdPt->m_StngFileFullPathVstrPt = NULL;
	}
	
	//销毁Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt = NULL;
	}
	
	//销毁WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt = NULL;
	}

	//销毁音频输入文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt = NULL;
	}

	//销毁音频结果文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt = NULL;
	}
	
	//销毁音频输出文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt = NULL;
	}

	//销毁错误信息动态字符串。
	if( MediaPocsThrdPt->m_ErrInfoVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_ErrInfoVstrPt );
		MediaPocsThrdPt->m_ErrInfoVstrPt = NULL;
	}

	//销毁媒体处理线程的内存块。
	free( MediaPocsThrdPt );

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptThrdRun
 * 功能说明：音频输入线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AdoInptThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	LOGI( Cu8vstr( "音频输入线程：开始准备音频输入。" ) );

	//开始音频输入循环。
	while( GetMessage( &MediaPocsThrdPt->m_AdoInpt.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaPocsThrdPt->m_AdoInpt.m_Msg.message == WIM_DATA ) //如果是音频输入缓冲区块读取完毕消息。
		{
			if( ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoInpt.m_Msg.lParam )->dwBytesRecorded != MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( int16_t ) ) //如果本次音频输入帧的长度不等于帧的长度，就表示本次音频输入帧不完整，丢弃。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "本次音频输入帧的长度不等于帧的长度，就表示本次音频输入帧不完整，丢弃。" ) );
				waveInAddBuffer( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AdoInpt.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。
				continue;
			}

			//获取一个音频输入空闲帧。
			MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.GetTotal( &MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLstElmTotal, 1, NULL ); //获取音频输入空闲帧链表的元素总数。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLstElmTotal > 0 ) //如果音频输入空闲帧链表中有音频输入空闲帧。
			{
				//从音频输入空闲帧链表中取出第一个音频输入空闲帧。
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.Locked( NULL ); //音频输入空闲帧链表的互斥锁加锁。
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt, NULL, 0, NULL );
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.DelHead( 0, NULL );
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.Unlock( NULL ); //音频输入空闲帧链表的互斥锁解锁。
				}
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：从音频输入空闲帧链表中取出第一个音频输入空闲帧，音频输入空闲帧链表元素个数：%uzd。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLstElmTotal );
			}
			else //如果音频输入空闲帧链表中没有音频输入空闲帧。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：音频输入空闲帧链表中没有音频输入空闲帧，本次音频输入帧丢弃。" ) );
			}

			//追加本次音频输入帧到音频输入帧链表。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt != NULL ) //如果获取了一个音频输入空闲帧。
			{
				memcpy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt, ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoInpt.m_Msg.lParam )->lpData, MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( int16_t ) );
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.PutTail( &MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt, 1, NULL );
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt = NULL;
			}

			waveInAddBuffer( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AdoInpt.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。

			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AdoInpt.m_NowTimeMsec );
				LOGFI( Cu8vstr( "音频输入线程：本次音频输入帧读取完毕，耗时 %uz64d 毫秒。" ), MediaPocsThrdPt->m_AdoInpt.m_NowTimeMsec - MediaPocsThrdPt->m_AdoInpt.m_LastTimeMsec );
				MediaPocsThrdPt->m_AdoInpt.m_LastTimeMsec = MediaPocsThrdPt->m_AdoInpt.m_NowTimeMsec;
			}
		}
		else if( MediaPocsThrdPt->m_AdoInpt.m_Msg.message == WIM_OPEN ) //如果是音频输入设备打开消息。
		{
			SetThreadPriority( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AdoInpt.m_LastTimeMsec );
				LOGI( Cu8vstr( "音频输入线程：本线程接收到音频输入设备打开消息。" ) );
			}
		}
		else if( MediaPocsThrdPt->m_AdoInpt.m_Msg.message == WIM_CLOSE ) //如果是音频输入设备关闭消息。
		{
			MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID = WAVE_MAPPER; //设置音频输入设备的标识符为默认的音频输入设备。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输入设备。" ) );
			break;
		}
		else //如果是其他消息。
		{
			LOGFI( Cu8vstr( "音频输入线程：获取到其他消息 %#uX。" ), MediaPocsThrdPt->m_AdoInpt.m_Msg.message );
		}
	}

	if( MediaPocsThrdPt->m_AdoInpt.m_Msg.message == WM_QUIT )
	{
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到退出请求，开始准备退出。" ) );
	}

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：本线程已退出。" ) );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptThrdRun
 * 功能说明：音频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AdoOtptThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	LOGI( Cu8vstr( "音频输出线程：开始准备音频输出。" ) );

	//开始音频输出循环。
	while( GetMessage( &MediaPocsThrdPt->m_AdoOtpt.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaPocsThrdPt->m_AdoOtpt.m_Msg.message == WOM_DONE ) //如果是音频输出缓冲区块写入完毕消息。
		{
			//调用用户定义的写入音频输出帧函数，并解码成PCM原始数据。
			switch( MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd ) //使用什么解码器。
			{
				case 0: //如果使用PCM原始数据。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen = ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->dwBufferLength;
					MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( MediaPocsThrdPt, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->lpData, NULL, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen );
					break;
				}
				case 1: //如果使用Speex编解码器。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen = MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmSz;
					MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( MediaPocsThrdPt, NULL, MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen );

					//使用Speex解码器。
					if( SpeexDecdPocs( MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdPt, MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->lpData ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：使用Speex解码器成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：使用Speex解码器失败。" ) );
					}
					break;
				}
				case 2: //如果使用Opus编解码器。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：暂不支持使用Opus解码器。" ) );
				}
			}
			
			//判断音频输出是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
            if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIsMute != 0 )
            {
				memset( ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->lpData, 0, ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->dwBufferLength );
            }

			//写入本次音频输出帧到音频输出设备。
			waveOutWrite( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam, sizeof( WAVEHDR ) );
			
			//调用用户定义的获取PCM格式音频输出帧函数。
			MediaPocsThrdPt->m_UserGetPcmAdoOtptFrmFuncPt( MediaPocsThrdPt, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->lpData, ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->dwBufferLength );

			//获取一个音频输出空闲帧。
			MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.GetTotal( &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLstElmTotal, 1, NULL );
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLstElmTotal > 0 ) //如果音频输出空闲帧链表中有音频输出空闲帧。
			{
				//从音频输出空闲帧链表中取出第一个音频输出空闲帧。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.Locked( NULL ); //音频输出空闲帧链表的互斥锁加锁。
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt, NULL, 0, NULL );
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.Unlock( NULL ); //音频输出空闲帧链表的互斥锁解锁。
				}
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：从音频输出空闲帧链表中取出第一个音频输出空闲帧，音频输出空闲帧链表元素个数：%uzd。" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLstElmTotal );
			}
			else //如果音频输出空闲帧链表中没有音频输出空闲帧。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出空闲帧链表中没有音频输出空闲帧，本次音频输出帧丢弃。" ) );
			}

			//追加本次音频输出帧到音频输出帧链表。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt != NULL )
			{
				memcpy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt, ( ( WAVEHDR * )MediaPocsThrdPt->m_AdoOtpt.m_Msg.lParam )->lpData, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * sizeof( int16_t ) );
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.PutTail( &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt, 1, NULL );
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt = NULL;
			}

			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AdoOtpt.m_NowTimeMsec );
				LOGFI( Cu8vstr( "音频输出线程：本次音频输出帧写入完毕，耗时 %uz64d 毫秒。" ), MediaPocsThrdPt->m_AdoOtpt.m_NowTimeMsec - MediaPocsThrdPt->m_AdoOtpt.m_LastTimeMsec );
				MediaPocsThrdPt->m_AdoOtpt.m_LastTimeMsec = MediaPocsThrdPt->m_AdoOtpt.m_NowTimeMsec;
			}
		}
		else if( MediaPocsThrdPt->m_AdoOtpt.m_Msg.message == WOM_OPEN ) //如果是音频输出设备打开消息。
		{
			SetThreadPriority( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AdoOtpt.m_LastTimeMsec );
				LOGI( Cu8vstr( "音频输出线程：本线程接收到音频输出设备打开消息。" ) );
			}
		}
		else if( MediaPocsThrdPt->m_AdoOtpt.m_Msg.message == WOM_CLOSE ) //如果是音频输出设备关闭消息。
		{
			MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID = WAVE_MAPPER; //设置音频输出设备的标识符为默认的音频输出设备。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输出设备。" ) );
			break;
		}
		else //如果是其他消息。
		{
			LOGFI( Cu8vstr( "音频输出线程：获取到其他消息 %#uX。" ), MediaPocsThrdPt->m_AdoInpt.m_Msg.message );
		}
	}

	if( MediaPocsThrdPt->m_AdoOtpt.m_Msg.message == WM_QUIT )
	{
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到退出请求，开始准备退出。" ) );
	}

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：本线程已退出。" ) );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoInptThrd
 * 功能说明：视频输入线程主类。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct VdoInptThrd : public ISampleGrabberCB
{
public:
	MediaPocsThrd * m_MediaPocsThrdPt;

	VdoInptThrd( MediaPocsThrd * MediaPocsThrdPt )
	{
		m_MediaPocsThrdPt = MediaPocsThrdPt;
	}

	STDMETHODIMP_( ULONG ) AddRef()
	{
		return 1;
	}

	STDMETHODIMP_( ULONG ) Release()
	{
		return 1;
	}

	STDMETHODIMP QueryInterface( REFIID riid, void ** ppv )
	{
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
		{
			*ppv = ( void * ) static_cast< ISampleGrabberCB* > ( this );
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
	{
		return 0;
	}

	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	{
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：读取一个视频输入帧。" ) );

		//丢弃采样频率过快的视频输入帧。
		FuncGetTimeAsMsec( &m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec );
		if( m_MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec != 0 ) //如果已经设置过上一个视频输入帧的时间。
		{
			if( m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec - m_MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec >= m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmTimeStepMsec )
			{
				m_MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec += m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmTimeStepMsec;
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：采样频率过快，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}
		else //如果没有设置过上一个视频输入帧的时间。
		{
			m_MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec = m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec;
		}

		//将视频输入帧复制到视频结果帧，方便处理。
		m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth * m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight * 4;
		memcpy( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, pBuffer, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen );

		//裁剪视频输入设备帧。
		if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop != 0 )
		{
			if( LibYUVPictrCrop( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight,
								 m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight,
								 m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen, NULL, NULL,
								 NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：裁剪视频输入设备帧成功。" ) );
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：裁剪视频输入设备帧失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}
		
		//垂直翻转镜像视频输入设备帧。因为视频输入设备帧是自底向上的。
		{
			if( LibYUVPictrMirror( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight,
								   1,
								   m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：垂直翻转镜像视频输入设备帧成功。" ) );
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：垂直翻转镜像视频输入设备帧失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}

		//缩放视频输入设备帧。
		if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsScale != 0 )
		{
			if( LibYUVPictrScale( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight,
								  3,
								  m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight,
								  NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：缩放视频输入设备帧成功。" ) );
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt; m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：缩放视频输入设备帧失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}
		
		//预览视频输入帧。
		if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptPrvwWndHdl != NULL )
		{
			//水平翻转镜像视频输入帧。
			if( LibYUVPictrMirror( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight,
								   0,
								   m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：水平翻转镜像视频输入帧成功。" ) );
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：水平翻转镜像视频输入帧失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}

			//绘制视频输入帧到视频输入预览窗口。
			if( LibYUVPictrDrawToWnd( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight,
									  0,
									  m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptPrvwWndHdl,
									  NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：绘制视频输入帧到视频输入预览窗口成功。" ) );
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：绘制视频输入帧到视频输入预览窗口失败，本次视频输入帧丢弃。" ) );
				goto Out;
			}
		}
		
		//获取一个视频输入空闲帧元素。
		m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.GetTotal( &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLstElmTotal, 1, NULL );
        if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLstElmTotal > 0 ) //如果视频输入空闲帧链表中有视频输入空闲帧元素。
        {
            //从视频输入空闲帧链表中取出第一个视频输入空闲帧元素。
            {
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.Locked( NULL ); //视频输入空闲帧链表的互斥锁加锁。
                m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.GetHead( NULL, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt, NULL, 0, NULL );
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.DelHead( 0, NULL );
				m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.Unlock( NULL ); //视频输入空闲帧链表的互斥锁解锁。
            }
            if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( Cu8vstr( "视频输入线程：从视频输入空闲帧链表中取出第一个视频输入空闲帧元素，视频输入空闲帧链表元素个数：%uzd。" ), m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLstElmTotal );
        }
        else //如果视频输入空闲帧链表中没有视频输入空闲帧元素。
        {
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGE( Cu8vstr( "视频输入线程：视频输入空闲帧链表中没有视频输入空闲帧元素，本次视频输入帧丢弃。" ) );
			goto Out;
        }

		//将BGRA格式视频输入帧转为YU12格式视频输入帧。
		if( LibYUVPictrFmtCnvrt( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight,
								 m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_YU12VdoInptFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth * m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight * 3 / 2, NULL, PICTR_FMT_BT601F8_YU12_I420,
								 NULL ) == 0 )
		{
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧成功。" ) );
		}
		else
		{
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧失败，本次视频输入帧丢弃。" ) );
			goto Out;
		}
		/*FILE * p_File = fopen( "123.yuv", "wb+" );
		fwrite( p_VdoInptFrm.m_YU12VdoInptFrmPt, 1, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth * m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight * 3 / 2, p_File );
		fclose( p_File );*/
		
        //判断视频输入是否黑屏。在视频输入处理完后再设置黑屏，这样可以保证视频输入处理器的连续性。
        if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIsBlack != 0 )
        {
            int p_TmpLen = m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth * m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight;
            memset( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_YU12VdoInptFrmPt, 0, p_TmpLen );
            memset( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_YU12VdoInptFrmPt + p_TmpLen, 128, p_TmpLen / 2 );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：设置YU12格式视频输入帧为黑屏成功。" ) );
        }

		//使用编码器。
        switch( m_MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输入线程：使用YU12原始数据。" ) );
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( OpenH264EncdPocs( m_MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdPt,
										 m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_YU12VdoInptFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight, m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec,
										 m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_EncdVdoInptFrmPt, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth * m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight * 3 / 2, &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_EncdVdoInptFrmLen,
										 NULL ) == 0 )
                {
                    if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输入线程：使用OpenH264编码器成功。H264格式视频输入帧的长度：%uzd，时间戳：%uz64d，类型：%hhd。" ), m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_EncdVdoInptFrmLen, m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec, m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt->m_EncdVdoInptFrmPt[4] );
                }
                else
                {
                    if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输入线程：使用OpenH264编码器失败，本次视频输入帧丢弃。" ) );
					goto Out;
                }
                break;
            }
        }

		//追加本次视频输入帧到视频输入帧链表。
		m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.PutTail( &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt, 1, NULL );
		m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt = NULL;

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &m_MediaPocsThrdPt->m_VdoInpt.m_NowTimeMsec );
			LOGFI( Cu8vstr( "视频输入线程：本次视频输入帧处理完毕，耗时 %uz64d 毫秒。" ), m_MediaPocsThrdPt->m_VdoInpt.m_NowTimeMsec - m_MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec );
		}

		Out:
		if( m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt != NULL ) //如果获取的视频输入空闲帧没有追加到视频输入帧链表。
		{
			m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.PutTail( &m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt, 1, NULL );
			m_MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt = NULL;
		}
		return 0;
	}
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptThrdRun
 * 功能说明：视频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI VdoOtptThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	SetThreadPriority( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
	
    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输出线程：开始准备视频输出。" ) );

    //开始视频输出循环。
    while( true )
    {
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &MediaPocsThrdPt->m_VdoOtpt.m_LastTimeMsec );

		//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
		switch( MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd ) //使用什么解码器。
		{
			case 0: //如果使用YU12原始数据。
			{
				//调用用户定义的写入视频输出帧函数。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth = 0;
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight = 0;
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz;
				MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight, NULL, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen );
				
				if( ( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth > 0 ) && ( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight > 0 ) ) //如果本次写入了视频输出帧。
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：使用YU12原始数据成功。YU12格式帧宽度：%z32d，YU12格式帧高度：%z32d。" ), MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight );
                }
                else //如果本次没写入视频输出帧。
                {
                    goto Skip;
                }
				break;
			}
			case 1: //如果使用OpenH264解码器。
			{
				//调用用户定义的写入视频输出帧函数。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz;
				MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( MediaPocsThrdPt, NULL, NULL, NULL, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen );

				if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen > 0 ) //如果本次写入了视频输出帧。
				{
					//使用OpenH264解码器。
					if( OpenH264DecdPocs( MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdPt,
											 MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen,
											 MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight,
											 NULL ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：使用OpenH264解码器成功。已解码YU12格式帧宽度：%z32d，已解码YU12格式帧高度：%z32d。" ), MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight );
                        if( ( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth == 0 ) || ( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight == 0 ) ) goto Skip; //如果未解码出YU12格式帧，就把本次视频输出帧丢弃。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：使用OpenH264解码器失败，本次视频输出帧丢弃。" ) );
						goto Skip;
					}
				}
				else
				{
					goto Skip;
				}
				break;
			}
		}

		//用户定义的获取YU12格式视频输出帧函数。
		MediaPocsThrdPt->m_UserGetYU12VdoOtptFrmFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight );
		
        //判断视频输出设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
        if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptIsBlack != 0 )
        {
            int p_TmpLen = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth * MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight;
            memset( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, 0, p_TmpLen );
            memset( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt + p_TmpLen, 128, p_TmpLen / 2 );
        }

		//缩放视频输出帧。
		if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale != 1.0f )
		{
			if( LibYUVPictrScale( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, PICTR_FMT_BT601F8_YU12_I420, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight,
								  3,
								  MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz, NULL, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth * MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight * MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale,
								  NULL ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输出线程：缩放视频输出帧失败，本次视频输出帧丢弃。" ) );
				goto Skip;
			}
			MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt; MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt; MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt; //交换视频结果帧和视频临时帧。

			MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth *= MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale;
			MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight *= MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale;
		}

		//将本次YU12格式视频输出帧转为BGRA格式视频输出帧。
		if( LibYUVPictrFmtCnvrt( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, PICTR_FMT_BT601F8_YU12_I420, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight,
								 MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz, NULL, PICTR_FMT_SRGBF8_BGRA8888,
								 NULL ) != 0 )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：将本次YU12格式视频输出帧转为BGRA格式视频输出帧失败，本次视频输出帧丢弃。" ) );
			goto Skip;
		}
		MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt; MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt; MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt = MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt; //交换视频结果帧和视频临时帧。

        //显示视频输出帧。
		if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyWndHdl != NULL )
		{
			//绘制视频输出帧到视频输出显示窗口。
			if( LibYUVPictrDrawToWnd( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth, MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight,
									  0,
									  MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyWndHdl,
									  NULL ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输出线程：绘制视频输出帧到视频输出显示窗口失败，本次视频输出帧丢弃。" ) );
				goto Skip;
			}
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &MediaPocsThrdPt->m_VdoOtpt.m_NowTimeMsec );
			LOGFI( Cu8vstr( "视频输出线程：本次视频输出帧处理完毕，耗时 %uz64d 毫秒。" ), MediaPocsThrdPt->m_VdoOtpt.m_NowTimeMsec - MediaPocsThrdPt->m_VdoOtpt.m_LastTimeMsec );
		}

		Skip:
        if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输出线程：本线程接收到退出请求，开始准备退出。" ) );
            goto Out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	Out:
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：本线程已退出。" ) );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdRun
 * 功能说明：媒体处理线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	SetThreadPriority( MediaPocsThrdPt->m_MediaPocsThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。

	int32_t p_TmpInt321;
	int32_t p_TmpInt322;
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;

	while( true )
	{
		{
			MediaPocsThrdPt->m_RunFlag = RUN_FLAG_INIT; //设置本线程运行标记为刚开始运行正在初始化。

			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本地代码的指令集名称为%s。退出标记：%d。" ), ( sizeof( size_t ) == 4 ) ? "x86" : "x64", MediaPocsThrdPt->m_ExitFlag );

			if( MediaPocsThrdPt->m_ExitFlag != 3 ) //如果需要执行用户定义的初始化函数。
			{
				MediaPocsThrdPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
				MediaPocsThrdPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。

				//调用用户定义的初始化函数。
				p_TmpInt321 = MediaPocsThrdPt->m_UserInitFuncPt( MediaPocsThrdPt );
				if( p_TmpInt321 == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数成功。返回值：%d。" ), p_TmpInt321 );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数失败。返回值：%d。" ), p_TmpInt321 );
					goto OutMediaInitAndPocs;
				}
			}
			else
			{
				MediaPocsThrdPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
				MediaPocsThrdPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。
			}

			//保存设置到文件。
			if( MediaPocsThrdPt->m_IsSaveStngToFile != 0 )
			{
				File * p_StngFilePt;

				if( FileInitByPath( &p_StngFilePt, MediaPocsThrdPt->m_StngFileFullPathVstrPt, NoRd_Wr, Create_AlExist_Clr, 4096, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化设置文件 %vs 失败。原因：%vs" ), MediaPocsThrdPt->m_StngFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto OutMediaInitAndPocs;
				}

				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsSaveStngToFile：%z32d\n" ), MediaPocsThrdPt->m_IsSaveStngToFile );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_StngFileFullPathVstrPt：%vs\n" ), ( MediaPocsThrdPt->m_StngFileFullPathVstrPt != NULL ) ? MediaPocsThrdPt->m_StngFileFullPathVstrPt : NULL );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsPrintLog：%z32d\n" ), MediaPocsThrdPt->m_IsPrintLog );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsShowToast：%z32d\n" ), MediaPocsThrdPt->m_IsShowToast );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsUseAdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SmplRate );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLen：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecFilterLen：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecIsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecEchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecEchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecEchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecEchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecIsSaveMemFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsSaveMemFile );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecmIsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmIsUseCNGMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecmEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmEchoMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecmDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecIsSaveMemFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsSaveMemFile );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWorkMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWorkMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecFilterLen：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecFilterLen );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecIsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecIsUseRec );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoMutp );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoCntu );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupes );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupesAct );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmEchoMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecEchoMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecIsUseSameRoomAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecIsUseSameRoomAec );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAecSameRoomEchoMinDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSameRoomEchoMinDelay );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsIsUseNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseNs );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNoiseSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNoiseSupes );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsIsUseDereverb：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseDereverb );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNsxPolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPolicyMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNsPolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPolicyMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsUseSpeexPrpocsOther：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsIsUseVad：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsVadProbStart：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbStart );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsVadProbCntu：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbCntu );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsIsUseAgc：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseAgc );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsAgcLevel：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcLevel );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsAgcIncrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcIncrement );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsAgcDecrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcDecrement );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsAgcMaxGain：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcMaxGain );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncdUseCbrOrVbr：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdUseCbrOrVbr );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncdQualt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdQualt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncdCmplxt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdCmplxt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncdPlcExptLossRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPlcExptLossRate );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsSaveAdoToFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoRsltFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmWndHdl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoRsltWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmWndHdl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptDvcID：%ud\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptWaveHdrTotal：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptIsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsUseAdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_SmplRate );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLen：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLen );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_UseWhatDecd：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SpeexDecdIsUsePrcplEnhsmt：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdIsUsePrcplEnhsmt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsSaveAdoToFile：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmWndHdl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptDvcID：%ud\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptWaveHdrTotal：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptIsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIsMute );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsUseVdoInpt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_MaxSmplRate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmWidth：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmWidth );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmHeight：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmHeight );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264EncdVdoType：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdVdoType );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264EncdEncdBitrate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdEncdBitrate );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264EncdBitrateCtrlMode：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdBitrateCtrlMode );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264EncdIDRFrmIntvl：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdIDRFrmIntvl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264EncdCmplxt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdCmplxt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_VdoInptDvcID：%ud\n" ), MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_VdoInptIsBlack：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_VdoInptIsBlack );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_VdoInptPrvwWndHdl：%#P\n" ), MediaPocsThrdPt->m_VdoInpt.m_VdoInptPrvwWndHdl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsUseVdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_UseWhatDecd：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_OpenH264DecdDecdThrdNum：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdDecdThrdNum );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptDspyWndHdl：%#P\n" ), MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyWndHdl );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptDspyScale：%f\n" ), MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptDspyScale );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptIsBlack：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptIsBlack );
				FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

				FileDstoy( p_StngFilePt, 1, NULL );
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：保存设置到文件 %vs 成功。" ), MediaPocsThrdPt->m_StngFileFullPathVstrPt );
			}

			//初始化音频输入。
			if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 )
			{
				//创建并初始化声学回音消除器。
				if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec != 0 ) //如果要使用声学回音消除器。
				{
					if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：不使用音频输出时，不能使用声学回音消除器。" ) );
						goto OutMediaInitAndPocs;
					}
					if( MediaPocsThrdPt->m_AdoOtpt.m_SmplRate != MediaPocsThrdPt->m_AdoInpt.m_SmplRate )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出的采样频率与音频输入不一致时，不能使用声学回音消除器。" ) );
						goto OutMediaInitAndPocs;
					}
					if( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen != MediaPocsThrdPt->m_AdoInpt.m_FrmLen )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出帧的长度与音频输入不一致时，不能使用声学回音消除器。" ) );
						goto OutMediaInitAndPocs;
					}
				}
				switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec )
				{
					case 0: //如果不使用声学回音消除器。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用声学回音消除器。" ) );
						break;
					}
					case 1: //如果使用Speex声学回音消除器。
					{
						if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
						{
							if( SpeexAecInitByMemFile( &MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：根据Speex声学回音消除器内存块文件 %vs 来创建并初始化Speex声学回音消除器成功。" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据Speex声学回音消除器内存块文件 %vs 来创建并初始化Speex声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt == NULL )
						{
							if( SpeexAecInit( &MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化Speex声学回音消除器成功。" ) );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化Speex声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto OutMediaInitAndPocs;
							}
						}
						break;
					}
					case 2: //如果使用WebRtc定点版声学回音消除器。
					{
						if( WebRtcAecmInit( &MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmIsUseCNGMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmEchoMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmDelay, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
					case 3: //如果使用WebRtc浮点版声学回音消除器。
					{
						if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
						{
							if( WebRtcAecInitByMemFile( &MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来创建并初始化WebRtc浮点版声学回音消除器成功。" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来创建并初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt == NULL )
						{
							if( WebRtcAecInit( &MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器成功。" ) );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto OutMediaInitAndPocs;
							}
						}
						break;
					}
					case 4: //如果使用SpeexWebRtc三重声学回音消除器。
					{
						if( SpeexWebRtcAecInit( &MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWorkMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecFilterLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecIsUseRec, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoMutp, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoCntu, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupes, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupesAct, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmEchoMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmDelay, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecEchoMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecDelay, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecIsUseSameRoomAec, MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSameRoomEchoMinDelay, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
				}

				//创建并初始化噪音抑制器。
				switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs )
				{
					case 0: //如果不使用噪音抑制器。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用噪音抑制器。" ) );
						break;
					}
					case 1: //如果使用Speex预处理器的噪音抑制。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：稍后在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" ) );
						break;
					}
					case 2: //如果使用WebRtc定点版噪音抑制器。
					{
						if( WebRtcNsxInit( &MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPolicyMode, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
					case 3: //如果使用WebRtc浮点版噪音抑制器。
					{
						if( WebRtcNsInit( &MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPolicyMode, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
					case 4: //如果使用RNNoise噪音抑制器。
					{
						if( RNNoiseInit( &MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化RNNoise噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化RNNoise噪音抑制器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
				}

				//创建并初始化Speex预处理器。
				if( ( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs == 1 ) || ( MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther != 0 ) )
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs != 1 )
					{
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseNs = 0;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseDereverb = 0;
					}
					if( MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther == 0 )
					{
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad = 0;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseAgc = 0;
					}
					if( SpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseNs, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNoiseSupes, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseDereverb, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbStart, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbCntu, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseAgc, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcLevel, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcIncrement, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcDecrement, MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcMaxGain, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化Speex预处理器成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化Speex预处理器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化编码器。
				switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd )
				{
					case 0: //如果使用PCM原始数据。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用PCM原始数据。" ) );
						break;
					}
					case 1: //如果使用Speex编码器。
					{
						if( MediaPocsThrdPt->m_AdoInpt.m_FrmLen != MediaPocsThrdPt->m_AdoInpt.m_SmplRate / 1000 * 20 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：帧的长度不为20毫秒不能使用Speex编码器。" ) );
							goto OutMediaInitAndPocs;
						}
						if( SpeexEncdInit( &MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdUseCbrOrVbr, MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdQualt, MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdCmplxt, MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPlcExptLossRate ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化Speex编码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化Speex编码器失败。" ) );
							goto OutMediaInitAndPocs;
						}
						break;
					}
					case 2: //如果使用Opus编码器。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：暂不支持使用Opus编码器。" ) );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输入Wave文件写入器、音频结果Wave文件写入器。
				if( MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile != 0 )
				{
					if( WaveFileWriterInit( &MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveFileWriterPt, MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt, 1, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 16 ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入文件 %vs 的Wave文件写入器成功。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输入文件 %vs 的Wave文件写入器失败。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileFullPathVstrPt );
						goto OutMediaInitAndPocs;
					}
					if( WaveFileWriterInit( &MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWaveFileWriterPt, MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt, 1, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 16 ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频结果文件 %vs 的Wave文件写入器成功。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频结果文件 %vs 的Wave文件写入器失败。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoRsltFileFullPathVstrPt );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输入波形器、音频结果波形器。
				if( MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd != 0 )
				{
					if( AdoWavfmInit( &MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入波形器成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输入波形器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs;
					}
					if( AdoWavfmInit( &MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频结果波形器成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频结果波形器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输入帧链表、音频输入空闲帧链表。
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal = 6; //音频输入帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输入帧，从而导致不同步。音频输入空闲帧链表最多存储总数与音频输入帧链表一致。
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal, MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal, MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					//创建并初始化音频输入空闲帧，并追加到音频输入空闲帧链表。
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmElmTotal; p_TmpInt322++ )
					{
						MediaPocsThrdPt->m_PcmAdoInptFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( int16_t ) );
						if( MediaPocsThrdPt->m_PcmAdoInptFrmPt == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：创建并初始化音频输入空闲帧失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoInptFrmPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							free( MediaPocsThrdPt->m_PcmAdoInptFrmPt );
							MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：追加音频输入空闲帧到音频输入空闲帧链表失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
					}
				}

				//初始化音频输入线程的临时变量。
				{
					MediaPocsThrdPt->m_AdoInpt.m_Msg = { 0 }; //初始化消息。
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt = NULL; //初始化音频输入帧的指针。
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLstElmTotal = 0; //初始化音频输入帧链表的元数总数。
					MediaPocsThrdPt->m_AdoInpt.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
					MediaPocsThrdPt->m_AdoInpt.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化音频输入线程的临时变量成功。" ) );
				}

				//创建并启动音频输入线程。必须在初始化音频输入设备前初始化音频输入线程，因为音频输入设备会使用音频输入线程ID。
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AdoInptThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdId );
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl != NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并启动音频输入线程成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：创建并启动音频输入线程失败。原因：%vs" ), GetLastErrInfo( , MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}
				}

				//创建并初始化音频输入缓冲区块数组。
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal = 2; //音频输入缓冲区块数组的元素总数固定为2。
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal * sizeof( WAVEHDR ) ); //创建并清空音频输入缓冲区块数组的内存块。
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并清空音频输入缓冲区块数组的内存块失败。" ) );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal; p_TmpInt322++ )
					{
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].dwUser = p_TmpInt322;
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].dwBufferLength = MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( int16_t );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].dwBufferLength );
						if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].lpData == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并清空音频输入缓冲区块数组的数据内存块失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入缓冲区块数组成功。" ) );
				}

				//创建并初始化音频输入设备。
				{
					WAVEFORMATEX WaveFormatEx;
					WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
					WaveFormatEx.nChannels = 1;
					WaveFormatEx.nSamplesPerSec = MediaPocsThrdPt->m_AdoInpt.m_SmplRate;
					WaveFormatEx.nAvgBytesPerSec = MediaPocsThrdPt->m_AdoInpt.m_SmplRate * 1 * sizeof( int16_t );
					WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
					WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
					WaveFormatEx.cbSize = 0;
					p_TmpInt321 = waveInOpen( &MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID, &WaveFormatEx, ( DWORD_PTR )MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdId, ( DWORD_PTR )MediaPocsThrdPt, CALLBACK_THREAD );
					if( p_TmpInt321 != MMSYSERR_NOERROR )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：打开音频输入设备失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, Cu8vstr( "媒体处理线程：打开音频输入设备失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutMediaInitAndPocs;
					}
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal; p_TmpInt322++ )
					{
						p_TmpInt321 = waveInPrepareHeader( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, &MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322] ) );
						if( p_TmpInt321 != MMSYSERR_NOERROR )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：为音频输入设备准备音频输入缓冲区块失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
							goto OutMediaInitAndPocs;
						}
						p_TmpInt321 = waveInAddBuffer( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, &MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322] ) );
						if( p_TmpInt321 != MMSYSERR_NOERROR )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：将已准备的音频输入缓冲区块添加到音频输入设备上失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
							goto OutMediaInitAndPocs;
						}
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输入设备成功。音频输入缓冲区大小：%d * %d。" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[0].dwBufferLength, MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal );
				}
			} //初始化音频输入完毕。

			//初始化音频输出。
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 )
			{
				//创建并初始化解码器。
				switch( MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd )
				{
					case 0: //如果使用PCM原始数据。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用PCM原始数据。" ) );
						break;
					}
					case 1: //如果使用Speex解码器。
					{
						if( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen != MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000 * 20 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：帧的长度不为20毫秒不能使用Speex解码器。" ) );
							goto OutMediaInitAndPocs;
						}
						if( SpeexDecdInit( &MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdPt, MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdIsUsePrcplEnhsmt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化Speex解码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化Speex解码器失败。" ) );
							goto OutMediaInitAndPocs;
						}
						break;
					}
					case 2: //如果使用Opus解码器。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：暂不支持使用Opus解码器。" ) );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输出Wave文件写入器。
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile != 0 )
				{
					if( WaveFileWriterInit( &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveFileWriterPt, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt, 1, MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, 16 ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出文件 %vs 的Wave文件写入器成功。" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输出文件 %vs 的Wave文件写入器失败。" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileFullPathVstrPt );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输入波形器、音频结果波形器。
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd != 0 )
				{
					if( AdoWavfmInit( &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出波形器成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输出波形器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化音频输出帧链表、音频输出空闲帧链表。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal = 6; //音频输出帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输出帧，从而导致不同步。音频输出空闲帧链表最多存储总数与音频输出帧链表一致。
					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输出帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出空闲帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化音频输出空闲帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					//创建并初始化音频输出空闲帧，并追加到音频输出空闲帧链表。
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmElmTotal; p_TmpInt322++ )
					{
						MediaPocsThrdPt->m_PcmAdoOtptFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * sizeof( int16_t ) );
						if( MediaPocsThrdPt->m_PcmAdoOtptFrmPt == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：创建并初始化音频输出空闲帧失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							free( MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
							MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：追加音频输入空闲帧到音频输出空闲帧链表失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
					}
				}
		
				//初始化音频输出线程的临时变量。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_Msg = { 0 }; //初始化消息。
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt = NULL; //初始化音频输出帧的指针。
					if( MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd != 0 )
					{
						MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * sizeof( int16_t ) ); //初始化已编码格式音频输出帧的指针。
						if( MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建已编码格式音频输出帧的内存块失败。" ) );
							goto OutMediaInitAndPocs;
						}
						MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmSz = MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * sizeof( int16_t ); //初始化已编码格式音频输出帧的内存大小，单位字节。
					}
					else
					{
						MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt = NULL; //初始化已编码格式音频输出帧的指针。
						MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmSz = 0; //初始化已编码格式音频输出帧的内存大小，单位字节。
					}
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen = 0; //初始化音频输出帧的长度，单位字节。
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLstElmTotal = 0; //初始化音频输出帧链表的元数总数。
					MediaPocsThrdPt->m_AdoOtpt.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
					MediaPocsThrdPt->m_AdoOtpt.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化音频输出线程的临时变量成功。" ) );
				}

				//创建并启动音频输出线程。必须在初始化音频输出设备前初始化音频输出线程，因为音频输出设备会使用音频输出线程ID。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AdoOtptThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdId );
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl != NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并启动音频输出线程成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：创建并启动音频输出线程失败。原因：%vs" ), GetLastErrInfo( , MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutMediaInitAndPocs; //本函数返回失败。
				}

				//创建并初始化音频输出缓冲区块数组。
				{
					if( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen / ( MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000 ) == 10 ) //如果帧的长度为10毫秒。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal = 4;
					}
					else if( MediaPocsThrdPt->m_AdoOtpt.m_FrmLen / ( MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000 ) == 20 ) //如果帧的长度为20毫秒。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal = 3;
					}
					else //如果帧的长度为大于等于30毫秒。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal = 2;
					}
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal * sizeof( WAVEHDR ) );
					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并清空音频输出缓冲区块数组的内存块失败。" ) );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal; p_TmpInt322++ )
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].dwBufferLength = MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * sizeof( int16_t );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].dwBufferLength );
						if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].lpData == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并清空音频输出缓冲区块数组的数据内存块失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出缓冲区块成功。" ) );
				}

				//创建并初始化音频输出设备。
				{
					WAVEFORMATEX WaveFormatEx;
					WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
					WaveFormatEx.nChannels = 1;
					WaveFormatEx.nSamplesPerSec = MediaPocsThrdPt->m_AdoOtpt.m_SmplRate;
					WaveFormatEx.nAvgBytesPerSec = MediaPocsThrdPt->m_AdoOtpt.m_SmplRate * 1 * sizeof( int16_t );
					WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
					WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
					WaveFormatEx.cbSize = 0;
					p_TmpInt321 = waveOutOpen( &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID, &WaveFormatEx, ( DWORD_PTR )MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdId, ( DWORD_PTR )MediaPocsThrdPt, CALLBACK_THREAD );
					if( p_TmpInt321 != MMSYSERR_NOERROR )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：打开音频输出设备失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, Cu8vstr( "媒体处理线程：打开音频输出设备失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutMediaInitAndPocs;
					}
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal; p_TmpInt322++ )
					{
						p_TmpInt321 = waveOutPrepareHeader( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322] ) );
						if( p_TmpInt321 != MMSYSERR_NOERROR )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：为音频输出设备准备音频输出缓冲区块失败。原因：%vs" ), GetWinMMErrInfo( p_TmpInt321, MediaPocsThrdPt->m_ErrInfoVstrPt ) );
							goto OutMediaInitAndPocs;
						}
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化音频输出设备成功。音频输出缓冲区大小：%d * %d。" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[0].dwBufferLength, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal );
				}
			} //初始化音频输出完毕。

			//自适应设置声学回音的延迟。
			if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec != 0 ) //如果要使用音频输入的声学回音消除，就自适应计算声学回音的延迟，并设置到声学回音消除器。
			{
				int32_t p_Delay;
				p_TmpInt321 = MediaPocsThrdPt->m_AdoInpt.m_FrmLen / ( MediaPocsThrdPt->m_AdoInpt.m_SmplRate / 1000 );
				p_TmpInt322 = MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal * p_TmpInt321;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入延迟 %z32d 毫秒。" ), p_TmpInt322 );
				p_Delay = p_TmpInt322;
				p_TmpInt322 = MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal * p_TmpInt321;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出延迟 %z32d 毫秒。" ), p_TmpInt322 );
				p_Delay += p_TmpInt322;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：声学回音延迟 %z32d 毫秒。" ), p_Delay );
				
				if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmGetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
				{
					WebRtcAecmSetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
					WebRtcAecmGetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, &p_TmpInt321 );
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt321 );
				}
				if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt != NULL ) && ( WebRtcAecGetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
					{
						WebRtcAecSetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, p_Delay );
						WebRtcAecGetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt321 );
					}
					else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
					{
						WebRtcAecSetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, 20 );
						WebRtcAecGetDelay( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt321 );
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt321 );
				}
				if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
				{
					SpeexWebRtcAecSetWebRtcAecmDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
					SpeexWebRtcAecGetWebRtcAecmDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt321 );
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt321 );
				}
				if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
					{
						SpeexWebRtcAecSetWebRtcAecDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, p_Delay );
						SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt321 );
					}
					else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
					{
						SpeexWebRtcAecSetWebRtcAecDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, 20 );
						SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt321 );
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt321 );
				}
			} //自适应设置声学回音的延迟完毕。

			//初始化视频输入。
			if( MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt != 0 )
			{
				//创建并初始化视频输入线程。必须在初始化视频输入设备前初始化视频输入线程，因为视频输入设备会使用视频输入线程的指针。
				{
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt = new VdoInptThrd( MediaPocsThrdPt );
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt != NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化视频输入线程成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化视频输入线程失败。" ) );
						goto OutMediaInitAndPocs;
					}
				}

				//创建并初始化视频输入设备。
				{
					int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
					ICreateDevEnum * p_CreateDevEnumPt = NULL;
					IEnumMoniker * p_EnumMonikerPt = NULL;
					IMoniker * p_MonikerPt = NULL;
					IEnumPins * p_EnumPinsPt = NULL;
					IPin * p_PinPt = NULL;
					PIN_DIRECTION p_PinDirection;
					IEnumMediaTypes * p_EnumMediaTypesPt = NULL;
					AM_MEDIA_TYPE * p_AmMediaTypePt = NULL;

					IBaseFilter * p_VdoInptDvcFilterPt = NULL; //存放视频输入设备过滤器的指针。
					IPin * p_SelPinPt = NULL;
					AM_MEDIA_TYPE * p_SelAmMediaTypePt = NULL;
					int32_t p_AvgTimePerFrm = 1000.0 / ( MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate / 10.0 / 1000.0 );
					double p_FrmWidthToHeightRatio = ( double )MediaPocsThrdPt->m_VdoInpt.m_FrmWidth / ( double )MediaPocsThrdPt->m_VdoInpt.m_FrmHeight; //存放指定帧的宽高比。
					double p_VdoInptDvcFrmWidthToHeightRatio = 0; //存放本次视频输入设备帧的宽高比。
					int32_t p_VdoInptDvcFrmCropWidth = 0; //存放本次视频输入设备帧的裁剪宽度。
					int32_t p_VdoInptDvcFrmCropHeight = 0; //存放本次视频输入设备帧的裁剪高度。

					IBaseFilter * p_DecFilterPt = NULL;
					IPin * p_DecFilterInptPinPt = NULL;
					IPin * p_DecFilterOtptPinPt = NULL;
			
					IBaseFilter * p_SmplGrabberFilterPt = NULL;
					ISampleGrabber * p_SmplGrabberInterfacePt = NULL;
					IPin * p_SmplGrabberFilterInptPinPt = NULL;
					IPin * p_SmplGrabberFilterOtptPinPt = NULL;
					AM_MEDIA_TYPE p_TmpAmMediaType = { 0 };

					//初始化COM库。
					CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY );

					//创建视频输入过滤器图管理器。
					if( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, ( void ** )&MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入过滤器图管理器失败。" ) );
						goto OutInitVdoInptDvc;
					}
			
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->QueryInterface( IID_IMediaEvent, ( LPVOID * )&MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入过滤器图管理器媒体事件器失败。" ) );
						goto OutInitVdoInptDvc;
					}

					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->QueryInterface( IID_IMediaControl, ( LPVOID * )&MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入过滤器图管理器媒体控制器失败。" ) );
						goto OutInitVdoInptDvc;
					}

					//创建系统设备枚举器。
					if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建系统设备枚举器失败。" ) );
						goto OutInitVdoInptDvc;
					}

					//查找并创建指定的视频输入设备过滤器。
					if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
					{
						for( p_TmpInt322 = 0; p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK; p_TmpInt322++ )
						{
							if( ( p_TmpInt322 == MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID ) || ( ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID == WAVE_MAPPER ) && ( p_TmpInt322 == 0 ) ) )
							{
								if( p_MonikerPt->BindToObject( NULL, NULL, IID_IBaseFilter, ( void * * )&p_VdoInptDvcFilterPt ) == S_OK )
								{
									p_TmpSz1 = p_MonikerPt->Release();
									p_MonikerPt = NULL;
									break;
								}
								else
								{
									p_TmpSz1 = p_MonikerPt->Release();
									p_MonikerPt = NULL;
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据Moniker创建视频输入设备过滤器失败。" ) );
									break;
								}
							}
							else
							{
								p_TmpSz1 = p_MonikerPt->Release();
								p_MonikerPt = NULL;
							}
						}
						p_TmpSz1 = p_EnumMonikerPt->Release();
						p_EnumMonikerPt = NULL;
					}
					p_TmpSz1 = p_CreateDevEnumPt->Release();
					p_CreateDevEnumPt = NULL;
					if( p_VdoInptDvcFilterPt == NULL ) //如果创建视频输入设备失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" ) );
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, Cu8vstr( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" ) );
						goto OutInitVdoInptDvc;
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->AddFilter( p_VdoInptDvcFilterPt, L"Video Capture" ) != S_OK ) //如果添加视频输入设备过滤器到视频输入过滤器图失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加视频输入设备过滤器到视频输入过滤器图失败。" ) );
						goto OutInitVdoInptDvc;
					}

					//选择视频输入设备过滤器上合适的引脚和媒体类型。
					if( p_VdoInptDvcFilterPt->EnumPins( &p_EnumPinsPt ) == S_OK ) //创建引脚枚举器。
					{
						while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK ) //遍历引脚。
						{
							if( ( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK ) && ( p_PinDirection == PINDIR_OUTPUT ) ) //查询引脚方向是否为输出。
							{
								if( p_PinPt->EnumMediaTypes( &p_EnumMediaTypesPt ) == S_OK ) //创建引脚的媒体类型枚举器。
								{
									while( p_EnumMediaTypesPt->Next( 1, &p_AmMediaTypePt, NULL ) == S_OK ) //遍历媒体类型，并智能选择满足目标的媒体类型。
									{
										p_TmpInt321 = 0;
										if( p_AmMediaTypePt->majortype == MEDIATYPE_Video && p_AmMediaTypePt->formattype == FORMAT_VideoInfo )
										{
											#define VIDEOINFOHEADER_AvgTimePerFrm( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
											#define VIDEOINFOHEADER_AvgTimePerFrmCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER_AvgTimePerFrm( AmMediaType1Pt ) Com VIDEOINFOHEADER_AvgTimePerFrm( AmMediaType2Pt ) )

											#define VIDEOINFOHEADER_Width( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biWidth )
											#define VIDEOINFOHEADER_Height( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biHeight )
											#define VIDEOINFOHEADER_WidthHeightCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( ( VIDEOINFOHEADER_Width( AmMediaType1Pt ) Com VIDEOINFOHEADER_Width( AmMediaType2Pt ) ) && ( VIDEOINFOHEADER_Height( AmMediaType1Pt ) Com VIDEOINFOHEADER_Height( AmMediaType2Pt ) ) )

											if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备支持的媒体格式：subtype：%s  AvgTimePerFrm：%lld  %.1f  Width：%ld  Height：%ld。" ),
																							( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																							VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) / 10.0 / 1000.0 ),
																							VIDEOINFOHEADER_Width( p_AmMediaTypePt ), VIDEOINFOHEADER_Height( p_AmMediaTypePt ) );
									
											if( ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) || ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ) //如果媒体格式为MJPEG或YUY2才进行选择。
											{
												//设置本次视频输入设备帧的宽高比、裁剪宽度、裁剪高度。
												p_VdoInptDvcFrmWidthToHeightRatio = ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt );
												if( p_VdoInptDvcFrmWidthToHeightRatio >= p_FrmWidthToHeightRatio )
												{
													p_VdoInptDvcFrmCropWidth = ( int )( ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt ) * p_FrmWidthToHeightRatio );
													p_VdoInptDvcFrmCropWidth -= p_VdoInptDvcFrmCropWidth % 2;
													p_VdoInptDvcFrmCropHeight = VIDEOINFOHEADER_Height( p_AmMediaTypePt );
												}
												else
												{
													p_VdoInptDvcFrmCropWidth = VIDEOINFOHEADER_Width( p_AmMediaTypePt );
													p_VdoInptDvcFrmCropHeight = ( int )( ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / p_FrmWidthToHeightRatio );
													p_VdoInptDvcFrmCropHeight -= p_VdoInptDvcFrmCropHeight % 2;
												}

												//如果选择的为空，就设置选择的为本次的。
												//如果选择的裁剪帧大小不满足指定的，但是本次的裁剪帧大小比选择的高，就设置选择的为本次的。
												//如果本次的裁剪帧大小满足指定的（选择的裁剪帧大小肯定也满足指定的，如果选择的裁剪帧大小不满足指定的，那么就会走上一条判断），或本次的裁剪帧大小和选择的相同，且：
													//如果选择的帧率不满足指定的，但是本次的帧率比选择的高，就设置选择的为本次的。
													//如果本次的帧率满足指定的（选择的帧率肯定也满足指定的，如果选择的帧率不满足指定的，那么就会走上一条判断），或本次的帧率和选择的相同，且：
														//如果本次的裁剪帧大小比选择的低，就设置选择的为本次的。
														//如果本次的裁剪帧大小与选择的相同，但本次的帧率比选择的低，就设置选择的为本次的。
												if( p_SelAmMediaTypePt == NULL )
												{
													p_TmpInt321 = 1;
												}
												else if( ( ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth < MediaPocsThrdPt->m_VdoInpt.m_FrmWidth ) || ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight < MediaPocsThrdPt->m_VdoInpt.m_FrmHeight ) ) && ( ( p_VdoInptDvcFrmCropWidth > MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) || ( p_VdoInptDvcFrmCropHeight > MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) ) )
												{
													p_TmpInt321 = 1;
												}
												else if( ( ( p_VdoInptDvcFrmCropWidth >= MediaPocsThrdPt->m_VdoInpt.m_FrmWidth ) || ( p_VdoInptDvcFrmCropHeight >= MediaPocsThrdPt->m_VdoInpt.m_FrmHeight ) ) || ( ( p_VdoInptDvcFrmCropWidth == MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) && ( p_VdoInptDvcFrmCropHeight == MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) ) )
												{
													if( ( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) > p_AvgTimePerFrm ) && ( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
													{
														p_TmpInt321 = 1;
													}
													else if( ( VIDEOINFOHEADER_AvgTimePerFrm( p_AmMediaTypePt ) <= p_AvgTimePerFrm ) || ( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
													{
														if( ( ( p_VdoInptDvcFrmCropWidth < MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) || ( p_VdoInptDvcFrmCropHeight < MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) ) )
														{
															p_TmpInt321 = 1;
														}
														else if( ( ( p_VdoInptDvcFrmCropWidth == MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) && ( p_VdoInptDvcFrmCropHeight == MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) ) && ( VIDEOINFOHEADER_AvgTimePerFrmCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
														{
															p_TmpInt321 = 1;
														}
													}
												}
											}
											else
											{
												if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：本次视频输入设备支持的媒体格式不是为MJPEG或YUY2，不能被选择。" ) );
											}

											if( p_TmpInt321 != 0 ) //如果需要选择本次的。
											{
												if( p_SelPinPt != p_PinPt )
												{
													if( p_SelPinPt != NULL ) p_SelPinPt->Release();
													p_SelPinPt = p_PinPt;
												}
												DeleteMediaType( p_SelAmMediaTypePt );
												p_SelAmMediaTypePt = p_AmMediaTypePt;
												p_AmMediaTypePt = NULL;

												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcMaxSmplRate = 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth = VIDEOINFOHEADER_Width( p_SelAmMediaTypePt );
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight = VIDEOINFOHEADER_Height( p_SelAmMediaTypePt );

												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth = p_VdoInptDvcFrmCropWidth;
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight = p_VdoInptDvcFrmCropHeight;
											}
											else //如果不要选择本次的。
											{
												DeleteMediaType( p_AmMediaTypePt );
												p_AmMediaTypePt = NULL;
											}
										}
										/*else if( p_AmMediaTypePt->majortype == MEDIATYPE_Video && p_AmMediaTypePt->formattype == FORMAT_VideoInfo2 ) //如果当前还没有FORMAT_VideoInfo格式的媒体类型。
										{
											#define VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
											#define VIDEOINFOHEADER2_AvgTimePerFrmCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaType1Pt ) Com VIDEOINFOHEADER2_AvgTimePerFrm( AmMediaType2Pt ) )

											#define VIDEOINFOHEADER2_Width( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->bmiHeader.biWidth )
											#define VIDEOINFOHEADER2_Height( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->bmiHeader.biHeight )
											#define VIDEOINFOHEADER2_WidthHeightCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( ( VIDEOINFOHEADER2_Width( AmMediaType1Pt ) Com VIDEOINFOHEADER2_Width( AmMediaType2Pt ) ) && ( VIDEOINFOHEADER2_Height( AmMediaType1Pt ) Com VIDEOINFOHEADER2_Height( AmMediaType2Pt ) ) )

											//如果选择的为空，就设置选择的为本次的。
											//如果选择的帧大小不满足指定的，但是本次的帧大小比选择的高，就设置选择的为本次的。
											//如果本次的帧大小满足指定的（选择的帧大小肯定也满足指定的，如果选择的帧大小不满足指定的，那么就会走上一条判断），或本次的帧大小和选择的相同，且：
												//如果选择的帧率不满足指定的，但是本次的帧率比选择的高，就设置选择的为本次的。
												//如果本次的帧率满足指定的（选择的帧率肯定也满足指定的，如果选择的帧率不满足指定的，那么就会走上一条判断），或本次的帧率和选择的相同，且：
													//如果本次的帧大小比选择的低，或本次的帧率比选择的低，就设置选择的为本次的。
											if( p_SelAmMediaTypePt == NULL )
											{
												p_TmpInt321 = 1;
											}
											else if( ( ( VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt ) < MediaPocsThrdPt->m_VdoInpt.m_FrmWidth ) || ( VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt ) < MediaPocsThrdPt->m_VdoInpt.m_FrmHeight ) ) && ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
											{
												p_TmpInt321 = 1;
											}
											else if( ( ( VIDEOINFOHEADER2_Width( p_AmMediaTypePt ) >= MediaPocsThrdPt->m_VdoInpt.m_FrmWidth ) && ( VIDEOINFOHEADER2_Height( p_AmMediaTypePt ) >= MediaPocsThrdPt->m_VdoInpt.m_FrmHeight ) ) || ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
											{
												if( ( VIDEOINFOHEADER2_AvgTimePerFrm( p_SelAmMediaTypePt ) > p_AvgTimePerFrm ) && ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
												{
													p_TmpInt321 = 1;
												}
												else if( ( VIDEOINFOHEADER2_AvgTimePerFrm( p_AmMediaTypePt ) <= p_AvgTimePerFrm ) || ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
												{
													if( ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) || ( VIDEOINFOHEADER2_AvgTimePerFrmCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
													{
														p_TmpInt321 = 1;
													}
												}
											}

											if( p_TmpInt321 != 0 )
											{
												if( p_SelPinPt != p_PinPt )
												{
													if( p_SelPinPt != NULL ) p_SelPinPt->Release();
													p_SelPinPt = p_PinPt;
												}
												DeleteMediaType( p_SelAmMediaTypePt );
												p_SelAmMediaTypePt = p_AmMediaTypePt;
												p_AmMediaTypePt = NULL;
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcMaxSmplRate = 1000.0 / ( VIDEOINFOHEADER2_AvgTimePerFrm( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth = VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt );
												MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight = VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt );
											}
											else
											{
												DeleteMediaType( p_AmMediaTypePt );
												p_AmMediaTypePt = NULL;
											}
										}*/
										else
										{
											DeleteMediaType( p_AmMediaTypePt );
											p_AmMediaTypePt = NULL;
										}
									}
									p_TmpSz1 = p_EnumMediaTypesPt->Release();
									p_EnumMediaTypesPt = NULL;
								}
							}
					
							if( p_PinPt != p_SelPinPt )
							{
								p_TmpSz1 = p_PinPt->Release();
							}
							p_PinPt = NULL;
						}
						p_TmpSz1 = p_EnumPinsPt->Release();
						p_EnumPinsPt = NULL;
					}
					if( p_SelPinPt == NULL || p_SelAmMediaTypePt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" ) );
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, Cu8vstr( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" ) );
						goto OutInitVdoInptDvc;
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备选择的媒体格式：subtype：%s  AvgTimePerFrm：%lld  %.1f  Width：%ld  Height：%ld。" ),
																	( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																	VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrm( p_SelAmMediaTypePt ) / 10.0 / 1000.0 ),
																	VIDEOINFOHEADER_Width( p_SelAmMediaTypePt ), VIDEOINFOHEADER_Height( p_SelAmMediaTypePt ) );
			
					//判断视频输入设备帧是否裁剪。
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth > MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) //如果视频输入设备帧的宽度比裁剪宽度大，就表示需要裁剪宽度。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop = 1; //设置视频输入设备帧要裁剪。

						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX = ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth - MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth ) / 2; //设置视频输入设备帧裁剪区域左上角的横坐标，使裁剪区域居中。
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
					}
					else if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight > MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) //如果视频输入设备帧的高度比裁剪高度大，就表示需要裁剪高度。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop = 1; //设置视频输入设备帧要裁剪。

						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY = ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight - MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight ) / 2; //设置视频输入设备帧裁剪区域左上角的纵坐标，使裁剪区域居中。
					}
					else //如果视频输入设备帧的宽度和高度与裁剪宽度和高度一致，就表示不需要裁剪。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop = 0; //设置视频输入设备帧不裁剪。

						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备帧是否裁剪：%z32d  左上角的横坐标：%z32d  纵坐标：%z32d  裁剪区域的宽度：%z32d  高度：%z32d。" ), MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop, MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX, MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY, MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth, MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight );

					//判断视频输入设备帧是否缩放。
					if( ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth != MediaPocsThrdPt->m_VdoInpt.m_FrmWidth ) || ( MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight != MediaPocsThrdPt->m_VdoInpt.m_FrmHeight ) )
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsScale = 1; //设置视频输入设备帧要缩放。
					}
					else
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsScale = 0; //设置视频输入设备帧不缩放。
					}
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleWidth = MediaPocsThrdPt->m_VdoInpt.m_FrmWidth; //设置视频输入设备帧缩放后的宽度。
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmScaleHeight = MediaPocsThrdPt->m_VdoInpt.m_FrmHeight; //设置视频输入设备帧缩放后的高度。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备帧是否缩放：%z32d。" ), MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsScale );

					//创建视频输入设备过滤器上选择的引脚和媒体类型对应的解码过滤器，并连接引脚。
					if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG )
					{
						if( CoCreateInstance( CLSID_MjpegDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建MJPG解码过滤器失败。" ) );
							goto OutInitVdoInptDvc;
						}
					}
					else if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 )
					{
						if( CoCreateInstance( CLSID_AVIDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建AVI解码过滤器失败。" ) );
							goto OutInitVdoInptDvc;
						}
					}
					{
						p_DecFilterPt->EnumPins( &p_EnumPinsPt );
						while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK )
						{
							if( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK )
							{
								if( p_PinDirection == PINDIR_INPUT )
								{
									p_DecFilterInptPinPt = p_PinPt;
								}
								else if( p_PinDirection == PINDIR_OUTPUT )
								{
									p_DecFilterOtptPinPt = p_PinPt;
								}
							}
							p_PinPt = NULL;
						}
						p_TmpSz1 = p_EnumPinsPt->Release();
						p_EnumPinsPt = NULL;
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->AddFilter( p_DecFilterPt, L"Decompressor" ) != S_OK ) //如果添加解码过滤器到视频输入过滤器图失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加解码过滤器到视频输入过滤器图失败。" ) );
						goto OutInitVdoInptDvc;
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->ConnectDirect( p_SelPinPt, p_DecFilterInptPinPt, p_SelAmMediaTypePt ) != S_OK ) //如果连接引脚失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。" ) );
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, Cu8vstr( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。原因：可能是视频输入设备无法正常工作。" ) );
						goto OutInitVdoInptDvc;
					}

					//创建采样抓取过滤器，并连接引脚。
					if( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void * * )&p_SmplGrabberFilterPt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建采样抓取过滤器失败。" ) );
						goto OutInitVdoInptDvc;
					}
					{
						p_SmplGrabberFilterPt->EnumPins( &p_EnumPinsPt );
						while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK )
						{
							if( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK )
							{
								if( p_PinDirection == PINDIR_INPUT )
								{
									p_SmplGrabberFilterInptPinPt = p_PinPt;
								}
								else if( p_PinDirection == PINDIR_OUTPUT )
								{
									p_SmplGrabberFilterOtptPinPt = p_PinPt;
								}
							}
							p_PinPt = NULL;
						}
						p_TmpSz1 = p_EnumPinsPt->Release();
						p_EnumPinsPt = NULL;
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->AddFilter( p_SmplGrabberFilterPt, L"Sample Grabber" ) != S_OK ) //如果采样抓取过滤器到视频输入过滤器图失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：添加采样抓取过滤器到视频输入过滤器图失败。" ) );
						goto OutInitVdoInptDvc;
					}
					p_TmpAmMediaType.majortype = MEDIATYPE_Video;
					//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB555; //PICTR_FMT_SRGBF8_BGR555。
					//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB565; //PICTR_FMT_SRGBF8_BGR565。
					//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB24; //PICTR_FMT_SRGBF8_BGR888。
					p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB32; //PICTR_FMT_SRGBF8_BGRA8888。
					if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->ConnectDirect( p_DecFilterOtptPinPt, p_SmplGrabberFilterInptPinPt, &p_TmpAmMediaType ) != S_OK ) //如果连接引脚失败。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：连接解码过滤器与采样抓取过滤器的引脚失败。" ) );
						goto OutInitVdoInptDvc;
					}

					//设置采样抓取过滤器的回调函数。
					if( p_SmplGrabberFilterPt->QueryInterface( IID_ISampleGrabber, ( void * * )&p_SmplGrabberInterfacePt ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：获取采样抓取过滤器的回调接口失败。" ) );
						goto OutInitVdoInptDvc;
					}
					if( p_SmplGrabberInterfacePt->SetCallback( MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt, 1 ) != S_OK )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：设置采样抓取过滤器的回调函数失败。" ) );
						goto OutInitVdoInptDvc;
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化视频输入设备成功。" ) );

					p_Rslt = 0; //设置本函数执行成功。

					OutInitVdoInptDvc:
					if( p_VdoInptDvcFilterPt != NULL )
					{
						p_TmpSz1 = p_VdoInptDvcFilterPt->Release();
						p_VdoInptDvcFilterPt = NULL;
					}
					if( p_SelPinPt != NULL )
					{
						p_TmpSz1 = p_SelPinPt->Release();
						p_SelPinPt = NULL;
					}
					if( p_SelAmMediaTypePt != NULL )
					{
						DeleteMediaType( p_SelAmMediaTypePt );
						p_SelAmMediaTypePt = NULL;
					}
					if( p_DecFilterPt != NULL )
					{
						p_TmpSz1 = p_DecFilterPt->Release();
						p_DecFilterPt = NULL;
					}
					if( p_DecFilterInptPinPt != NULL )
					{
						p_TmpSz1 = p_DecFilterInptPinPt->Release();
						p_DecFilterInptPinPt = NULL;
					}
					if( p_DecFilterOtptPinPt != NULL )
					{
						p_TmpSz1 = p_DecFilterOtptPinPt->Release();
						p_DecFilterOtptPinPt = NULL;
					}
					if( p_SmplGrabberFilterPt != NULL )
					{
						p_TmpSz1 = p_SmplGrabberFilterPt->Release();
						p_SmplGrabberFilterPt = NULL;
					}
					if( p_SmplGrabberInterfacePt != NULL )
					{
						p_TmpSz1 = p_SmplGrabberInterfacePt->Release();
						p_SmplGrabberInterfacePt = NULL;
					}
					if( p_SmplGrabberFilterInptPinPt != NULL )
					{
						p_TmpSz1 = p_SmplGrabberFilterInptPinPt->Release();
						p_SmplGrabberFilterInptPinPt = NULL;
					}
					if( p_SmplGrabberFilterOtptPinPt != NULL )
					{
						p_TmpSz1 = p_SmplGrabberFilterOtptPinPt->Release();
						p_SmplGrabberFilterOtptPinPt = NULL;
					}
					if( p_Rslt != 0 ) //如果本函数执行失败。
					{
						goto OutMediaInitAndPocs;
					}
				} //创建并初始化视频输入设备完毕。
		
				//初始化视频输入线程的临时变量。
				{
					MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec = 0; //初始化上一个视频输入帧的时间。
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmTimeStepMsec = 1000 / MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate; //初始化视频输入帧的时间步进。
					if( MediaPocsThrdPt->m_VdoInpt.m_FrmWidth * MediaPocsThrdPt->m_VdoInpt.m_FrmHeight >= MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth * MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight ) //如果视频输入帧指定的尺寸大于等于视频输入设备帧的尺寸。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz = MediaPocsThrdPt->m_VdoInpt.m_FrmWidth * MediaPocsThrdPt->m_VdoInpt.m_FrmHeight * 4; //初始化视频输入结果帧的内存大小。
					}
					else //如果视频输入帧指定的尺寸小于视频输入设备帧的尺寸。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz = MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth * MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight * 4; //初始化视频输入结果帧的内存大小。
					}
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz ); //初始化视频输入结果帧的指针。
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频结果帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz ); //初始化视频输入临时帧的指针。
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频临时帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen = 0; //初始化视频输入结果帧的长度。
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt = NULL; //初始化视频输入交换帧的指针。
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt = NULL; //初始化视频输入帧元素的指针。
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLstElmTotal = 0; //初始化视频输入帧链表的元数总数。
					MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
					MediaPocsThrdPt->m_VdoInpt.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化视频输入线程的临时变量成功。" ) );
				}

				//初始化编码器。
				switch( MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd )
				{
					case 0: //如果要使用YU12原始数据。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：使用YU12原始数据。" ) );
						break;
					}
					case 1: //如果要使用OpenH264编码器。
					{
						if( OpenH264EncdInit( &MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdPt, MediaPocsThrdPt->m_VdoInpt.m_FrmWidth, MediaPocsThrdPt->m_VdoInpt.m_FrmHeight, MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdVdoType, MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdEncdBitrate, MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdBitrateCtrlMode, MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate, MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdIDRFrmIntvl, MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdCmplxt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：创建并初始化OpenH264编码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化OpenH264编码器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
				}

				//创建并初始化视频输入帧链表、视频输入空闲帧链表。
				{
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal = 2; //视频输入帧链表最多只存储2帧，避免因为视频设备有变化导致卡顿并积累大量视频输入帧，从而导致不及时。视频输入空闲帧链表最多存储总数与视频输入帧链表一致。
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Init( sizeof( MediaPocsThrd::VdoInpt::VdoInptFrmElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal, MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化视频输入帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化视频输入帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.Init( sizeof( MediaPocsThrd::VdoInpt::VdoInptFrmElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal, MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并初始化视频输入空闲帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化视频输入空闲帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutMediaInitAndPocs; //本函数返回失败。
					}

					//创建并初始化视频输入空闲帧，并追加到视频输入空闲帧链表。
					for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmTotal; p_TmpInt322++ )
					{
						MediaPocsThrdPt->m_VdoInptFrmPt = ( MediaPocsThrd::VdoInpt::VdoInptFrmElm * )malloc( sizeof( MediaPocsThrd::VdoInpt::VdoInptFrmElm ) );
						if( MediaPocsThrdPt->m_VdoInptFrmPt == NULL )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：创建并初始化视频输入空闲帧失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VdoInpt.m_FrmWidth * MediaPocsThrdPt->m_VdoInpt.m_FrmHeight * 3 / 2 );
						if( MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt == NULL )
						{
							free( MediaPocsThrdPt->m_VdoInptFrmPt );
							MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：创建并初始化视频输入空闲帧失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
						if( MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd != 0 )
						{
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VdoInpt.m_FrmWidth * MediaPocsThrdPt->m_VdoInpt.m_FrmHeight * 3 / 2 );
							if( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt == NULL )
							{
								free( MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt );
								MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt = NULL;
								free( MediaPocsThrdPt->m_VdoInptFrmPt );
								MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：创建并初始化视频输入空闲帧失败。" ) );
								goto OutMediaInitAndPocs; //本函数返回失败。
							}
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmLen = 0;
						}
						else
						{
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt = NULL;
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmLen = 0;
						}
						if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_VdoInptFrmPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							free( MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt );
							MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt = NULL;
							if( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt != NULL )
							{
								free( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt );
								MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt = NULL;
							}
							free( MediaPocsThrdPt->m_VdoInptFrmPt );
							MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：追加视频输入空闲帧到视频输入空闲帧链表失败。" ) );
							goto OutMediaInitAndPocs; //本函数返回失败。
						}
					}
				}
			} //初始化视频输入完毕。

			//初始化视频输出。
			if( MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt != 0 )
			{
				//初始化解码器。
				switch( MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd )
				{
					case 0: //如果要使用YU12原始数据。
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：使用YU12原始数据。" ) );
						break;
					}
					case 1: //如果要使用OpenH264解码器。
					{
						if( OpenH264DecdInit( &MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdPt, MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdDecdThrdNum, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：创建并初始化OpenH264解码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建并初始化OpenH264解码器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaInitAndPocs;
						}
						break;
					}
				}

				//初始化视频输出线程的临时变量。
				{
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出结果帧的指针。
					if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输出结果帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出临时帧的指针。
					if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建视频输出临时帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt = NULL; //初始化视频输出交换帧的指针。
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz = 960 * 1280 * 4 * 3; //初始化视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen = 0; //初始化视频输出结果帧的长度。
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth = 0; //初始化视频输出帧的宽度。
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight = 0; //初始化视频输出帧的高度。
					MediaPocsThrdPt->m_VdoOtpt.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
					MediaPocsThrdPt->m_VdoOtpt.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化视频输出线程的临时变量成功。" ) );
				}

				//创建并启动音频输出线程。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdExitFlag = 0; //设置视频输出线程退出标记为0表示保持运行。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )VdoOtptThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdId );
				if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl != NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：创建并启动视频输出线程成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：创建并启动视频输出线程失败。原因：%vs" ), GetLastErrInfo( , MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutMediaInitAndPocs; //本函数返回失败。
				}
			} //初始化视频输出完毕。
	
			//初始化媒体处理线程的临时变量。
			{
				MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL; //初始化PCM格式音频输入帧的指针。
				MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL; //初始化PCM格式音频输出帧的指针。
				if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) //初始化PCM格式音频结果帧的指针。
				{
					MediaPocsThrdPt->m_PcmAdoRsltFrmPt = ( int16_t * )malloc( sizeof( int16_t ) * MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
					if( MediaPocsThrdPt->m_PcmAdoRsltFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建PCM格式音频结果帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
				}
				else
				{
					MediaPocsThrdPt->m_PcmAdoRsltFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) //初始化PCM格式音频临时帧的指针。
				{
					MediaPocsThrdPt->m_PcmAdoTmpFrmPt = ( int16_t * )malloc( sizeof( int16_t ) * MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
					if( MediaPocsThrdPt->m_PcmAdoTmpFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建PCM格式音频临时帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
				}
				else
				{
					MediaPocsThrdPt->m_PcmAdoTmpFrmPt = NULL;
				}
				MediaPocsThrdPt->m_PcmAdoSwapFrmPt = NULL; //初始化PCM格式音频交换帧的指针。
				MediaPocsThrdPt->m_VoiceActSts = 1; //初始化语音活动状态，预设为1是为了让在没有使用语音活动检测的情况下永远都有语音活动。
				if( ( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd != 0 ) ) //初始化已编码格式音频输入帧的指针和内存大小。
				{
					MediaPocsThrdPt->m_EncdAdoInptFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( uint8_t ) );
					if( MediaPocsThrdPt->m_EncdAdoInptFrmPt == NULL )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建已编码格式音频输入帧的内存块失败。" ) );
						goto OutMediaInitAndPocs;
					}
					MediaPocsThrdPt->m_EncdAdoInptFrmSz = MediaPocsThrdPt->m_AdoInpt.m_FrmLen * sizeof( uint8_t );
				}
				else
				{
					MediaPocsThrdPt->m_EncdAdoInptFrmPt = NULL;
					MediaPocsThrdPt->m_EncdAdoInptFrmSz = 0;
				}
				MediaPocsThrdPt->m_EncdAdoInptFrmLen = 0; //初始化已编码格式音频输入帧的长度，单位字节。
				MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans = 1; //初始化已编码格式音频输入帧是否需要传输预设为1，为了让在不使用非连续传输的情况下永远都是需要传输。
				MediaPocsThrdPt->m_VdoInptFrmPt = NULL; //初始化视频输入帧的指针。
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化媒体处理线程的临时变量成功。" ) );
			} //初始化媒体处理线程的临时变量完毕。

			if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) //如果要使用音频输入。
			{
				waveInStart( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl ); //让音频输入设备立即开始录音。
			}
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) //如果要使用音频输出。
			{
				for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal; p_TmpInt322++ ) //让音频输出设备立即开始播放。
				{
					waveOutWrite( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322] ) );
				}
			}
			if( MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt != 0 ) //如果要使用视频输入。
			{
				MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt->Run(); //让视频输入过滤器图管理器开始运行。
			}

			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &p_NowMsec );
				LOGFI( Cu8vstr( "媒体处理线程：媒体处理线程初始化完毕，耗时：%uz64d 毫秒，正式开始处理帧。" ), p_NowMsec - p_LastMsec );
			}

			MediaPocsThrdPt->m_ExitCode = -2; //初始化已经成功了，再将本线程退出代码预设为处理失败，如果处理失败，这个退出代码就不用再设置了，如果处理成功，再设置为成功的退出代码。
			MediaPocsThrdPt->m_RunFlag = RUN_FLAG_PROC; //设置本线程运行标记为初始化完毕正在循环处理帧。

			//开始音视频输入输出帧处理循环。
			while( true )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec );

				//调用用户定义的处理函数。
				p_TmpInt321 = MediaPocsThrdPt->m_UserPocsFuncPt( MediaPocsThrdPt );
				if( p_TmpInt321 == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数成功。返回值：%d" ), p_TmpInt321 );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的处理函数失败。返回值：%d" ), p_TmpInt321 );
					goto OutMediaInitAndPocs;
				}
		
				if( MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &p_NowMsec );
					LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数完毕，耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
					p_LastMsec = p_NowMsec;
				}

				//取出音频输入帧和音频输出帧。
				if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取音频输入帧链表的元素总数。
				else p_TmpSz1 = 0;
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.GetTotal( &p_TmpSz2, 1, NULL ); //获取音频输出帧链表的元素总数。
				else p_TmpSz2 = 0;
				if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) ) //如果音频输入帧链表和音频输出帧链表中都有帧了，就开始取出。
				{
					//从音频输入帧链表中取出第一个音频输入帧。
					{
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Locked( NULL ); //音频输入帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Unlock( NULL ); //音频输入帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%z32d。" ), p_TmpSz1 );

					//从音频输出帧链表中取出第一个音频输出帧。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Locked( NULL ); //音频输出帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Unlock( NULL ); //音频输出帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%z32d。" ), p_TmpSz2 );

					//将音频输入帧复制到音频结果帧，方便处理。
					memcpy( MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptFrmPt, sizeof( int16_t ) * MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
				}
				else if( ( p_TmpSz1 > 0 ) && ( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.m_ConstLenLnkLstPt == NULL ) ) //如果音频输入帧链表有帧了，且不使用音频输出帧链表，就开始取出。
				{
					//从音频输入帧链表中取出第一个音频输入帧。
					{
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Locked( NULL ); //音频输入帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Unlock( NULL ); //音频输入帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%z32d。" ), p_TmpSz1 );

					//将音频输入帧复制到音频结果帧，方便处理。
					memcpy( MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptFrmPt, sizeof( int16_t ) * MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
				}
				else if( ( p_TmpSz2 > 0 ) && ( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.m_ConstLenLnkLstPt == NULL ) ) //如果音频输出帧链表有帧了，且不使用音频输入帧链表，就开始取出。
				{
					//从音频输出帧链表中取出第一个音频输出帧。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Locked( NULL ); //音频输出帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Unlock( NULL ); //音频输出帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%z32d。" ), p_TmpSz2 );
				}
		
				//处理音频输入帧。
				if( MediaPocsThrdPt->m_PcmAdoInptFrmPt != NULL )
				{
					//使用声学回音消除器。
					switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec )
					{
						case 0: //如果不使用声学回音消除器。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用声学回音消除器。" ) );
							break;
						}
						case 1: //如果使用Speex声学回音消除器。
						{
							if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt != NULL ) && ( SpeexAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 ) )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器失败。" ) );
							}
							break;
						}
						case 2: //如果使用WebRtc定点版声学回音消除器。
						{
							if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 ) )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器失败。" ) );
							}
							break;
						}
						case 3: //如果使用WebRtc浮点版声学回音消除器。
						{
							if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt != NULL ) && ( WebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 ) )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器失败。" ) );
							}
							break;
						}
						case 4: //如果使用SpeexWebRtc三重声学回音消除器。
						{
							if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 ) )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器失败。" ) );
							}
							break;
						}
					}

					//使用噪音抑制器。
					switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs )
					{
						case 0: //如果不使用噪音抑制器。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用噪音抑制器。" ) );
							break;
						}
						case 1: //如果使用Speex预处理器的噪音抑制。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：稍后在使用Speex预处理器时一起使用噪音抑制。" ) );
							break;
						}
						case 2: //如果使用WebRtc定点版噪音抑制器。
						{
							if( WebRtcNsxPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 3: //如果使用WebRtc浮点版噪音抑制器。
						{
							if( WebRtcNsPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 4: //如果使用RNNoise噪音抑制器。
						{
							if( RNNoisePocs( MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用RNNoise噪音抑制器成功。" ) );
								MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用RNNoise噪音抑制器失败。" ) );
							}
							break;
						}
					}

					//使用Speex预处理器。
					if( ( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs == 1 ) || ( MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther != 0 ) )
					{
						if( SpeexPrpocsPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_PcmAdoTmpFrmPt, &MediaPocsThrdPt->m_VoiceActSts ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex预处理器成功。语音活动状态：%d" ), MediaPocsThrdPt->m_VoiceActSts );
							MediaPocsThrdPt->m_PcmAdoSwapFrmPt = MediaPocsThrdPt->m_PcmAdoRsltFrmPt; MediaPocsThrdPt->m_PcmAdoRsltFrmPt = MediaPocsThrdPt->m_PcmAdoTmpFrmPt; MediaPocsThrdPt->m_PcmAdoTmpFrmPt = MediaPocsThrdPt->m_PcmAdoSwapFrmPt; //交换音频结果帧和音频临时帧。
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex预处理器失败。" ) );
						}
					}

					//判断音频输入是否静音。在音频输入处理完后再设置静音，这样可以保证音频输入处理器的连续性。
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute != 0 )
					{
						memset( MediaPocsThrdPt->m_PcmAdoRsltFrmPt, 0, sizeof( int16_t ) * MediaPocsThrdPt->m_AdoInpt.m_FrmLen );
						if( ( MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther != 0 ) && ( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad != 0 ) ) //如果Speex预处理器要使用其他功能，且要使用语音活动检测。
						{
							MediaPocsThrdPt->m_VoiceActSts = 0;
						}
					}

					//使用编码器。
					switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd )
					{
						case 0: //如果使用PCM原始数据。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用PCM原始数据。" ) );
							break;
						}
						case 1: //如果使用Speex编码器。
						{
							if( SpeexEncdPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_EncdAdoInptFrmPt, MediaPocsThrdPt->m_EncdAdoInptFrmSz, &MediaPocsThrdPt->m_EncdAdoInptFrmLen, &MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex编码器成功。Speex格式输入帧的长度：%uzd，Speex格式输入帧是否需要传输：%z32d" ), MediaPocsThrdPt->m_EncdAdoInptFrmLen, MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex编码器失败。" ) );
							}
							break;
						}
						case 2: //如果使用Opus编码器。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：暂不支持使用Opus编码器。" ) );
							goto OutMediaInitAndPocs;
						}
					}

					//使用音频输入Wave文件写入器写入输入帧数据、音频结果Wave文件写入器写入结果帧数据。
					if( MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile != 0 )
					{
						if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoInptFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLen * 2 ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据失败。" ) );
						}
						if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLen * 2 ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据失败。" ) );
						}
					}

					//使用音频输入波形器绘制音频输入波形到窗口、音频结果波形器绘制音频结果波形到窗口。
					if( MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmPt, MediaPocsThrdPt->m_PcmAdoInptFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入波形器绘制音频输入波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入波形器绘制音频输入波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频结果波形器绘制音频结果波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频结果波形器绘制音频结果波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}
				}
		
				if( MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &p_NowMsec );
					LOGFI( Cu8vstr( "媒体处理线程：处理音频输入帧完毕，耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
					p_LastMsec = p_NowMsec;
				}
		
				//处理音频输出帧。
				if( MediaPocsThrdPt->m_PcmAdoOtptFrmPt != NULL )
				{
					//使用音频输出Wave文件写入器写入输出帧数据。
					if( MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile != 0 )
					{
						if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * 2 ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据失败。" ) );
						}
					}

					//使用音频输出波形器绘制音频输出波形到窗口。
					if( MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmPt, MediaPocsThrdPt->m_PcmAdoOtptFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出波形器绘制音频输出波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出波形器绘制音频输出波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}
				}
		
				if( MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &p_NowMsec );
					LOGFI( Cu8vstr( "媒体处理线程：处理音频输出帧完毕，耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
					p_LastMsec = p_NowMsec;
				}

				//处理视频输入帧。
				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输入帧链表的元素总数。
				}
				else p_TmpSz1 = 0;
				if( ( p_TmpSz1 > 0 ) && //如果视频输入帧链表中有帧了。
					( ( MediaPocsThrdPt->m_PcmAdoInptFrmPt != NULL ) || ( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.m_ConstLenLnkLstPt == NULL ) ) ) //且已经处理了音频输入帧或不使用音频输入帧链表。
				{
					//从视频输入帧链表中取出第一个视频输入帧。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Locked( NULL ); //视频输入帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VdoInptFrmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Unlock( NULL ); //视频输入帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输入帧链表中取出第一个视频输入帧，视频输入帧链表元素个数：%uzd。" ), p_TmpSz1 );
				}

				if( MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &p_NowMsec );
					LOGFI( Cu8vstr( "媒体处理线程：处理视频输入帧完毕，耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
					FuncGetTimeAsMsec( &p_LastMsec );
				}

				//调用用户定义的读取音视频输入帧函数。
				if( ( MediaPocsThrdPt->m_PcmAdoInptFrmPt != NULL ) || ( MediaPocsThrdPt->m_VdoInptFrmPt != NULL ) ) //如果取出了音频输入帧或视频输入帧。
				{
					if( MediaPocsThrdPt->m_VdoInptFrmPt != NULL ) //如果取出了视频输入帧。
						 MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_PcmAdoInptFrmPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_VoiceActSts, MediaPocsThrdPt->m_EncdAdoInptFrmPt, MediaPocsThrdPt->m_EncdAdoInptFrmLen, MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans, MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt, MediaPocsThrdPt->m_VdoInpt.m_FrmWidth, MediaPocsThrdPt->m_VdoInpt.m_FrmHeight, MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt, MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmLen );
					else
						MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_PcmAdoInptFrmPt, MediaPocsThrdPt->m_PcmAdoRsltFrmPt, MediaPocsThrdPt->m_VoiceActSts, MediaPocsThrdPt->m_EncdAdoInptFrmPt, MediaPocsThrdPt->m_EncdAdoInptFrmLen, MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans, NULL, NULL, NULL, NULL, NULL );
				}

				if( MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &p_NowMsec );
					LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时：%uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
					p_LastMsec = p_NowMsec;
				}
		
				if( MediaPocsThrdPt->m_PcmAdoInptFrmPt != NULL ) //如果取出了PCM格式音频输入帧，就追加到音频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoInptFrmPt, 1, NULL );
					MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_PcmAdoOtptFrmPt != NULL ) //如果取出了PCM格式音频输出帧，就追加到音频输出空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, 1, NULL );
					MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_VdoInptFrmPt != NULL ) //如果取出了视频输入帧，就追加到视频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_VdoInptFrmPt, 1, NULL );
					MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
				}

				//如果音频输入线程因为设备关闭而退出了，就重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl != NULL )
				{
					GetExitCodeThread( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl, ( LPDWORD )&p_TmpInt321 );
					if( p_TmpInt321 != STILL_ACTIVE )
					{
						LOGFI( Cu8vstr( "媒体处理线程：音频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" ) );
						MediaPocsThrdPt->m_ExitFlag = 3;
					}
				}

				//如果音频输出线程因为设备关闭而退出了，就重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl != NULL )
				{
					GetExitCodeThread( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl, ( LPDWORD )&p_TmpInt321 );
					if( p_TmpInt321 != STILL_ACTIVE )
					{
						LOGFI( Cu8vstr( "媒体处理线程：音频输出线程因为设备关闭而退出了，准备重启媒体处理线程。" ) );
						MediaPocsThrdPt->m_ExitFlag = 3;
					}
				}

				//如果视频输入线程因为设备关闭而退出了，就重启媒体处理线程。
				if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt != NULL )
				{
					long p_EventCode;
					LONG_PTR p_EventParam1, p_EventParam2;
					MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
					if( p_EventCode == EC_DEVICE_LOST )
					{
						LOGFI( Cu8vstr( "媒体处理线程：视频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" ) );
						MediaPocsThrdPt->m_ExitFlag = 3;
					}
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程退出标记为请求退出。
				{
					MediaPocsThrdPt->m_ExitCode = 0; //处理已经成功了，再将本线程退出代码设置为正常退出。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：接收到退出请求，开始准备退出。退出标记：%d。" ), MediaPocsThrdPt->m_ExitFlag );
					goto OutMediaInitAndPocs;
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			} //音视频输入输出帧处理循环完毕。
		}
		OutMediaInitAndPocs:;
		
		MediaPocsThrdPt->m_RunFlag = RUN_FLAG_DSTOY; //设置本线程运行标记为跳出循环处理帧正在销毁。
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：本线程开始退出。" ) );

		//请求音视频输入输出线程退出。必须在销毁音视频输入输出前退出，因为音视频输入输出线程会使用音视频输入输出相关变量。
		if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl != NULL )
			while( PostThreadMessage( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdId, WM_QUIT, NULL, NULL ) == 0 ) //循环发送请求音频输入线程退出，直到线程消息发送成功。不要使用退出标记，因为音频输入设备可能会打开失败，导致音频输入线程不会进入消息循环。
			{
				GetExitCodeThread( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl, ( LPDWORD )&p_TmpInt321 );
				if( p_TmpInt321 == STILL_ACTIVE ) Sleep( 0 ); //如果音频输入线程还在运行，说明是音频输入线程刚开始运行时还来不及创建消息队列导致发送请求失败，就重新发送请求。
				else break; //如果音频输入线程已经退出，则不需要再发送请求。
			}
		if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl != NULL )
			while( PostThreadMessage( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdId, WM_QUIT, NULL, NULL ) == 0 ) //循环发送请求音频输出线程退出，直到线程消息发送成功。不要使用退出标记，因为音频输出设备可能会打开失败，导致音频输出线程不会进入消息循环。
			{
				GetExitCodeThread( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl, ( LPDWORD )&p_TmpInt321 );
				if( p_TmpInt321 == STILL_ACTIVE ) Sleep( 0 ); //如果音频输出线程还在运行，说明是音频输出线程刚开始运行时还来不及创建消息队列导致发送请求失败，就重新发送请求。
				else break; //如果音频输出线程已经退出，则不需要再发送请求。
			}
		if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl != NULL ) MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdExitFlag = 1; //请求视频输出线程退出。

		//销毁音频输入。
		{
			//销毁音频输入线程。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl != NULL )
			{
				WaitForSingleObject( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl, INFINITE ); //等待音频输入线程退出。
				CloseHandle( MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl );
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdHdl = NULL;
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptThrdId = NULL;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入线程成功。" ) );
			}

			//销毁音频输入线程线程的临时变量。
			{
				MediaPocsThrdPt->m_AdoInpt.m_Msg = { 0 }; //销毁消息。
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmPt = NULL; //销毁音频输入帧的指针。
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLstElmTotal = 0; //销毁音频输入帧链表的元数总数。
				MediaPocsThrdPt->m_AdoInpt.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
				MediaPocsThrdPt->m_AdoInpt.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入线程线程的临时变量成功。" ) );
			}

			//销毁音频输入帧链表、音频输入空闲帧链表。
			{
				if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_PcmAdoInptFrmPt );
						MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}

				if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_PcmAdoInptFrmPt );
						MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIdleFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入空闲帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入空闲帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
			}

			//销毁音频输入设备。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl != NULL )
			{
				for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal; p_TmpInt322++ )
				{
					waveInUnprepareHeader( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl, &MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
				}
				waveInReset( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl );
				waveInClose( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl );
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcHdl = NULL;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入设备成功。" ) );
			}

			//销毁音频输入缓冲区块数组。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt != NULL )
			{
				for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal; p_TmpInt322++ )
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].lpData != NULL )
					{
						free( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].lpData );
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt[p_TmpInt322].lpData = NULL;
					}
				}
				free( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt );
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrPt = NULL;
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveHdrTotal = 0;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入缓冲区块数组成功。" ) );
			}

			//销毁音频输入波形器、音频结果波形器。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmPt != NULL )
			{
				if( AdoWavfmDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入波形器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入波形器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptWavfmPt = NULL;
			}
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmPt != NULL )
			{
				if( AdoWavfmDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频结果波形器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频结果波形器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWavfmPt = NULL;
			}
		
			//销毁音频输入Wave文件写入器、音频结果Wave文件写入器。
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveFileWriterPt != NULL )
			{
				if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveFileWriterPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入Wave文件写入器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入Wave文件写入器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoInpt.m_AdoInptWaveFileWriterPt = NULL;
			}
			if( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWaveFileWriterPt != NULL )
			{
				if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWaveFileWriterPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频结果Wave文件写入器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频结果Wave文件写入器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoInpt.m_AdoRsltWaveFileWriterPt = NULL;
			}
		
			//销毁编码器。
			switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd )
			{
				case 0: //如果使用PCM原始数据。
				{
					break;
				}
				case 1: //如果使用Speex编码器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt != NULL )
					{
						if( SpeexEncdDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex编码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex编码器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt = NULL;
					}
					break;
				}
				case 2: //如果使用Opus编码器。
				{
					break;
				}
			}

			//销毁Speex预处理器。
			if( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt != NULL )
			{
				if( SpeexPrpocsDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex预处理器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex预处理器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt = NULL;
			}

			//销毁噪音抑制器。
			switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs )
			{
				case 0: //如果不使用噪音抑制器。
				{
					break;
				}
				case 1: //如果使用Speex预处理器的噪音抑制。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：之前在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" ) );
					break;
				}
				case 2: //如果使用WebRtc定点版噪音抑制器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt != NULL )
					{
						if( WebRtcNsxDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc定点版噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc定点版噪音抑制器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt = NULL;
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版噪音抑制器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt != NULL )
					{
						if( WebRtcNsDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版噪音抑制器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt = NULL;
					}
					break;
				}
				case 4: //如果使用RNNoise噪音抑制器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt != NULL )
					{
						if( RNNoiseDstoy( MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁RNNoise噪音抑制器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁RNNoise噪音抑制器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt = NULL;
					}
					break;
				}
			}

			//销毁声学回音消除器。
			switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec )
			{
				case 0: //如果不使用声学回音消除器。
				{
					break;
				}
				case 1: //如果使用Speex声学回音消除器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt != NULL )
					{
						if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
						{
							if( SpeexAecSaveMemFile( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %vs 成功。" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						if( SpeexAecDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex声学回音消除器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt = NULL;
					}
					break;
				}
				case 2: //如果使用WebRtc定点版声学回音消除器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt != NULL )
					{
						if( WebRtcAecmDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc定点版声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc定点版声学回音消除器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt = NULL;
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版声学回音消除器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt != NULL )
					{
						if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
						{
							if( WebRtcAecSaveMemFile( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, MediaPocsThrdPt->m_AdoInpt.m_FrmLen, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 成功。" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						if( WebRtcAecDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版声学回音消除器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt = NULL;
					}
					break;
				}
				case 4: //如果使用SpeexWebRtc三重声学回音消除器。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL )
					{
						if( SpeexWebRtcAecDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt = NULL;
					}
					break;
				}
			}
		} //销毁音频输入完毕。

		//销毁音频输出。
		{
			//销毁音频输出线程。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl != NULL )
			{
				WaitForSingleObject( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl, INFINITE ); //等待音频输出线程退出。
				CloseHandle( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl );
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdHdl = NULL;
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdId = NULL;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出线程成功。" ) );
			}

			//销毁音频输出线程线程的临时变量。
			{
				MediaPocsThrdPt->m_AdoOtpt.m_Msg = { 0 }; //销毁消息。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmPt = NULL; //销毁音频输出帧的指针。
				if( MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt != NULL ) //销毁已编码格式音频输出帧的指针。
				{
					free( MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt );
					MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmPt = NULL;
				}
				MediaPocsThrdPt->m_AdoOtpt.m_EncdAdoOtptFrmSz = 0; //销毁已编码格式音频输出帧的内存大小，单位字节。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLen = 0; //销毁音频输出帧的长度，单位字节。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLstElmTotal = 0; //销毁音频输出帧链表的元数总数。
				MediaPocsThrdPt->m_AdoOtpt.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
				MediaPocsThrdPt->m_AdoOtpt.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出线程线程的临时变量成功。" ) );
			}

			//销毁音频输出帧链表、音频输出空闲帧链表。
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
						MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}

				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
						MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIdleFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出空闲帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出空闲帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
			}

			//销毁音频输出设备。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl != NULL )
			{
				for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal; p_TmpInt322++ )
				{
					waveOutUnprepareHeader( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl, &MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
				}
				waveOutReset( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl );
				waveOutClose( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl );
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcHdl = NULL;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出设备成功。" ) );
			}

			//销毁音频输出缓冲区块数组。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt != NULL )
			{
				for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal; p_TmpInt322++ )
				{
					if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].lpData != NULL )
					{
						free( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].lpData );
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt[p_TmpInt322].lpData = NULL;
					}
				}
				free( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt );
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrPt = NULL;
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveHdrTotal = 0;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出缓冲区块数组成功。" ) );
			}

			//销毁音频输出波形器。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmPt != NULL )
			{
				if( AdoWavfmDstoy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出波形器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出波形器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWavfmPt = NULL;
			}

			//销毁音频输出Wave文件写入器。
			if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveFileWriterPt != NULL )
			{
				if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveFileWriterPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出Wave文件写入器成功。" ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出Wave文件写入器失败。" ) );
				}
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptWaveFileWriterPt = NULL;
			}

			//销毁解码器。
			switch( MediaPocsThrdPt->m_AdoOtpt.m_UseWhatDecd )
			{
				case 0: //如果使用PCM原始数据。
				{
					break;
				}
				case 1: //如果使用Speex解码器。
				{
					if( MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdPt != NULL )
					{
						if( SpeexDecdDstoy( MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex解码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex解码器失败。" ) );
						}
						MediaPocsThrdPt->m_AdoOtpt.m_SpeexDecdPt = NULL;
					}
					break;
				}
				case 2: //如果使用Opus编码器。
				{
					break;
				}
			}
		} //销毁音频输出完毕。

		//销毁视频输入。
		{
			//销毁视频输入设备。
			if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt != NULL )
			{
				MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt->Stop(); //让视频输入过滤器图管理器停止运行。
				p_TmpSz1 = MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt->Release();
				MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaCtrlPt = NULL;
			}
			if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt != NULL )
			{
				p_TmpSz1 = MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt->Release();
				MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerMediaEventPt = NULL;
			}
			if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt != NULL )
			{
				p_TmpSz1 = MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt->Release();
				MediaPocsThrdPt->m_VdoInpt.m_FilterGraphManagerPt = NULL;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcMaxSmplRate = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmWidth = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmHeight = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsCrop = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropX = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropY = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropWidth = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmCropHeight = 0;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcFrmIsScale = 0;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入设备成功。" ) );
			}

			//销毁视频输入线程。
			if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt != NULL )
			{
				delete MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt;
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptThrdPt = NULL;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入线程成功。" ) );
			}

			//销毁视频输入线程的临时变量。
			{
				MediaPocsThrdPt->m_VdoInpt.m_LastVdoInptFrmTimeMsec = 0; //销毁上一个视频输入帧的时间。
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmTimeStepMsec = 0; //销毁视频输入帧的时间步进。
				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt != NULL ) //销毁视频结果帧的指针。
				{
					free( MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt );
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt != NULL ) //销毁视频临时帧的指针。
				{
					free( MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt );
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptTmpFrmPt = NULL;
				}
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmSz = 0; //销毁视频输入结果帧的内存大小。
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptRsltFrmLen = 0; //销毁视频输入结果帧的长度。
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptSwapFrmPt = NULL; //销毁视频交换帧的指针。
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmElmPt = NULL; //销毁视频输入帧元素的指针。
				MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLstElmTotal = 0; //销毁视频输入帧链表的元数总数。
				MediaPocsThrdPt->m_VdoInpt.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
				MediaPocsThrdPt->m_VdoInpt.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输入线程的临时变量成功。" ) );
			}

			//销毁视频输入帧链表、视频输入空闲帧链表。
			{
				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VdoInptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt );
						MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt = NULL;
						if( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt != NULL )
						{
							free( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt );
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt = NULL;
						}
						free( MediaPocsThrdPt->m_VdoInptFrmPt );
						MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输入帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输入帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}

				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
				{
					while( MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VdoInptFrmPt, NULL, 0, NULL ) == 0 )
					{
						free( MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt );
						MediaPocsThrdPt->m_VdoInptFrmPt->m_YU12VdoInptFrmPt = NULL;
						if( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt != NULL )
						{
							free( MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt );
							MediaPocsThrdPt->m_VdoInptFrmPt->m_EncdVdoInptFrmPt = NULL;
						}
						free( MediaPocsThrdPt->m_VdoInptFrmPt );
						MediaPocsThrdPt->m_VdoInptFrmPt = NULL;
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.DelHead( 0, NULL );
					}
					if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输入空闲帧链表成功。" ) );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输入空闲帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
			}

			//销毁编码器。
			switch( MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd )
			{
				case 0: //如果要使用YU12原始数据。
				{
					break;
				}
				case 1: //如果要使用OpenH264编码器。
				{
					if( MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdPt != NULL )
					{
						if( OpenH264EncdDstoy( MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdPt, NULL ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁OpenH264编码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁OpenH264编码器失败。" ) );
						}
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdPt = NULL;
					}
					break;
				}
			}
		} //销毁视频输入完毕。

		//销毁视频输出。
		{
			//销毁视频输出线程。
			if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl != NULL )
			{
				WaitForSingleObject( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl, INFINITE ); //等待视频输出线程退出。
				CloseHandle( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl );
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdHdl = NULL;
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdId = NULL;
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptThrdExitFlag = 0;
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输出线程成功。" ) );
			}

			//销毁视频输出线程的临时变量。
			{
				if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt != NULL ) //销毁视频输出结果帧的指针。
				{
					free( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt );
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt != NULL ) //销毁视频输出临时帧的指针。
				{
					free( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt );
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptTmpFrmPt = NULL;
				}
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptSwapFrmPt = NULL; //销毁视频输出交换帧的指针。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmSz = 0; //销毁视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptRsltFrmLen = 0; //销毁视频输出结果帧的长度。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmWidth = 0; //销毁视频输出帧的宽度。
				MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmHeight = 0; //销毁视频输出帧的高度。
				MediaPocsThrdPt->m_VdoOtpt.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
				MediaPocsThrdPt->m_VdoOtpt.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输出线程的临时变量成功。" ) );
			}

			//销毁解码器。
			switch( MediaPocsThrdPt->m_VdoOtpt.m_UseWhatDecd )
			{
				case 0: //如果要使用YU12原始数据。
				{
					break;
				}
				case 1: //如果要使用OpenH264解码器。
				{
					if( MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdPt != NULL )
					{
						if( OpenH264DecdDstoy( MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdPt, NULL ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁OpenH264解码器成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：销毁OpenH264解码器失败。" ) );
						}
						MediaPocsThrdPt->m_VdoOtpt.m_OpenH264DecdPt = NULL;
					}
					break;
				}
			}
		} //销毁视频输出完毕。

		//销毁媒体处理线程的临时变量。
		{
			if( MediaPocsThrdPt->m_PcmAdoInptFrmPt != NULL ) //销毁PCM格式音频输入帧的指针。
			{
				free( MediaPocsThrdPt->m_PcmAdoInptFrmPt );
				MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
			}
			if( MediaPocsThrdPt->m_PcmAdoOtptFrmPt != NULL ) //销毁PCM格式音频输出帧的指针。
			{
				free( MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
				MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
			}
			if( MediaPocsThrdPt->m_PcmAdoRsltFrmPt != NULL ) //销毁PCM格式结果帧的指针。
			{
				free( MediaPocsThrdPt->m_PcmAdoRsltFrmPt );
				MediaPocsThrdPt->m_PcmAdoRsltFrmPt = NULL;
			}
			if( MediaPocsThrdPt->m_PcmAdoTmpFrmPt != NULL ) //销毁PCM格式临时帧的指针。
			{
				free( MediaPocsThrdPt->m_PcmAdoTmpFrmPt );
				MediaPocsThrdPt->m_PcmAdoTmpFrmPt = NULL;
			}
			MediaPocsThrdPt->m_PcmAdoSwapFrmPt = NULL; //销毁PCM格式音频交换帧的指针。
			MediaPocsThrdPt->m_VoiceActSts = 0; //销毁语音活动状态。
			if( MediaPocsThrdPt->m_EncdAdoInptFrmPt != NULL ) //销毁已编码格式音频输入帧的指针。
			{
				free( MediaPocsThrdPt->m_EncdAdoInptFrmPt );
				MediaPocsThrdPt->m_EncdAdoInptFrmPt = NULL;
			}
			MediaPocsThrdPt->m_EncdAdoInptFrmSz = 0; //销毁已编码格式音频输入帧的内存大小。
			MediaPocsThrdPt->m_EncdAdoInptFrmLen = 0; //销毁已编码格式音频输入帧的长度。
			MediaPocsThrdPt->m_EncdAdoInptFrmIsNeedTrans = 0; //销毁已编码格式音频输入帧是否需要传输。
			MediaPocsThrdPt->m_VdoInptFrmPt = NULL; //销毁视频输入帧的指针。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁媒体处理线程的临时变量成功。" ) );
		} //销毁媒体处理线程的临时变量完毕。

		if( MediaPocsThrdPt->m_ExitFlag != 3 ) //如果需要调用用户定义的销毁函数。
		{
			MediaPocsThrdPt->m_UserDstoyFuncPt( MediaPocsThrdPt ); //调用用户定义的销毁函数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的销毁函数成功。" ) );
		}
	
		MediaPocsThrdPt->m_RunFlag = RUN_FLAG_END; //设置本线程运行标记为销毁完毕。

		if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) || ( MediaPocsThrdPt->m_ExitFlag == 1 ) ) //如果用户需要直接退出。
		{
			//销毁媒体处理线程的句柄。
			if( MediaPocsThrdPt->m_MediaPocsThrdHdl != NULL )
			{
				CloseHandle( MediaPocsThrdPt->m_MediaPocsThrdHdl );
				MediaPocsThrdPt->m_MediaPocsThrdHdl = NULL;
			}

			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本线程已退出。" ) );
			goto OutMediaPocsThrdLoop;
		}
		else //如果用户需用重新初始化。
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本线程重新初始化。" ) );
		}
	}
	OutMediaPocsThrdLoop:;
	
	return 0;
}
