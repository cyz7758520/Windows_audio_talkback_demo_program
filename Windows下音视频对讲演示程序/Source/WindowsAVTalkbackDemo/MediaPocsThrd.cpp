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

DWORD WINAPI AudioInputThrdRun( MediaPocsThrd * MediaPocsThrdPt );
DWORD WINAPI AudioOutputThrdRun( MediaPocsThrd * MediaPocsThrdPt );
DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdInit
 * 功能说明：创建并初始化媒体处理线程。
 * 参数说明：MediaPocsThrdPtPt：[输出]，存放用于存放媒体处理线程指针的指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的指针，不能为NULL。
			 UserPocsessFuncPt：[输入]，存放用户定义的处理函数的指针，不能为NULL。
			 UserDstoyFuncPt：[输入]，存放用户定义的销毁函数的指针，不能为NULL。
			 UserReadAudioVideoInputFrameFuncPt：[输入]，存放用户定义的读取音视频输入帧函数的指针，不能为NULL。
			 UserWriteAudioOutputFrameFuncPt：[输入]，存放用户定义的写入音频输出帧函数的指针，不能为NULL。
			 UserGetPcmAudioOutputFrameFuncPt：[输入]，存放用户定义的获取PCM格式音频输出帧函数的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
						 USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt,
						 USER_READ_AUDIO_VIDEO_INPUT_FRAME_FUNC_PT UserReadAudioVideoInputFrameFuncPt,
						 USER_WRITE_AUDIO_OUTPUT_FRAME_FUNC_PT UserWriteAudioOutputFrameFuncPt, USER_GET_PCM_AUDIO_OUTPUT_FRAME_FUNC_PT UserGetPcmAudioOutputFrameFuncPt,
						 USER_WRITE_VIDEO_OUTPUT_FRAME_FUNC_PT UserWriteVideoOutputFrameFuncPt, USER_GET_YU12_VIDEO_OUTPUT_FRAME_FUNC_PT UserGetYU12VideoOutputFrameFuncPt,
						 VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	MediaPocsThrd * p_MediaPocsThrdPt = NULL; //存放媒体处理线程的指针。

	//判断各个变量是否正确。
	if( MediaPocsThrdPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放媒体处理线程指针的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserInitFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的初始化函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserPocsFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的处理函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserDstoyFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的销毁函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserReadAudioVideoInputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的读取音视频输入帧函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserWriteAudioOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的写入音频输出帧函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserGetPcmAudioOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的获取PCM格式音频输出帧函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserWriteVideoOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的写入视频输出帧函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( UserGetYU12VideoOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的获取YU12格式视频输出帧函数的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	
	//创建并清空媒体处理线程的内存块。
	p_MediaPocsThrdPt = ( MediaPocsThrd * )calloc( 1, sizeof( MediaPocsThrd ) );
	if( p_MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并清空媒体处理线程的内存块失败。" );
		goto Out;
	}

	p_MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcID = WAVE_MAPPER; //设置音频输入设备的标识符。
	p_MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcID = WAVE_MAPPER; //设置音频输出设备的标识符。
	p_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcID = WAVE_MAPPER; //设置视频输入设备的标识符。
	
	//创建并初始化设置文件的完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_StngFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化设置文件的完整路径动态字符串失败。" );
		goto Out;
	}
	
	//创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
		goto Out;
	}
	
	//创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
		goto Out;
	}

	//创建并初始化音频输入文件完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输入文件完整路径动态字符串失败。" );
		goto Out;
	}

	//创建并初始化音频结果文件完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频结果文件完整路径动态字符串失败。" );
		goto Out;
	}

	//创建并初始化音频输出文件完整路径动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输出文件完整路径动态字符串失败。" );
		goto Out;
	}

	//创建并初始化错误信息动态字符串。
	if( VarStrInit( &p_MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化错误信息动态字符串失败。" );
		goto Out;
	}

	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VarStrCpy( ErrInfoVarStrPt, "设置最小计时器分辨率失败。" );
		goto Out;
	}

	p_MediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_MediaPocsThrdPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的指针。
	p_MediaPocsThrdPt->m_UserPocsFuncPt = UserPocsFuncPt; //设置用户定义的处理函数的指针。
	p_MediaPocsThrdPt->m_UserDstoyFuncPt = UserDstoyFuncPt; //设置用户定义的销毁函数的指针。
	p_MediaPocsThrdPt->m_UserReadAudioVideoInputFrameFuncPt = UserReadAudioVideoInputFrameFuncPt; //设置用户定义的读取音视频输入帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteAudioOutputFrameFuncPt = UserWriteAudioOutputFrameFuncPt; //设置用户定义的写入音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetPcmAudioOutputFrameFuncPt = UserGetPcmAudioOutputFrameFuncPt; //设置用户定义的获取PCM格式音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteVideoOutputFrameFuncPt = UserWriteVideoOutputFrameFuncPt; //设置用户定义的写入视频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetYU12VideoOutputFrameFuncPt = UserGetYU12VideoOutputFrameFuncPt; //设置用户定义的获取YU12格式视频输出帧函数的指针。

	*MediaPocsThrdPtPt = p_MediaPocsThrdPt; //设置媒体处理线程的指针。

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoy( p_MediaPocsThrdPt, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsSaveStngToFile
 * 功能说明：设置是否保存设置到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveStngToFile：[输入]，存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
			 StngFileFullPathStrPt：[输入]，存放设置文件完整路径字符串的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, int IsSaveStngToFile, const char * StngFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveStngToFile != 0 ) && ( StngFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "设置文件完整路径字符串的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_IsSaveStngToFile = IsSaveStngToFile;
	if( IsSaveStngToFile != 0 )
	{
		if( VarStrCpy( MediaPocsThrdPt->m_StngFileFullPathVarStrPt, StngFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置设置文件完整路径动态字符串失败。" );
			goto Out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_IsSaveStngToFile = 0;
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsPrintLogShowToast
 * 功能说明：设置是否打印Log日志、显示Toast。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int IsPrintLog, int IsShowToast, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_IsPrintLog = IsPrintLog;
	MediaPocsThrdPt->m_IsShowToast = IsShowToast;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAudioInputDvcName
 * 功能说明：获取音频输入设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AudioInputDvcNameArrPtPtPt：[输出]，存放用于存放音频输入设备名称数组指针的指针，不能为NULL。
			 AudioInputDvcTotalPt：[输出]，存放用于存放音频输入设备总数的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAudioInputDvcName( VarStr * * * AudioInputDvcNameArrPtPtPt, UINT * AudioInputDvcTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_AudioInputDvcNameArrPtPt = NULL; //存放音频输入设备名称数组的指针。
	UINT p_AudioInputDvcTotal = 0; //存放音频输入设备的总数。
	WAVEINCAPS p_WaveInCaps; //存放音频输入设备的功能。
	MMRESULT p_MMResult;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AudioInputDvcNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输入设备名称数组指针的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( AudioInputDvcTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输入设备总数的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	p_AudioInputDvcTotal = waveInGetNumDevs();
	*AudioInputDvcTotalPt = p_AudioInputDvcTotal; //设置音频输入设备的总数。

	//设置音频输入设备名称数组。
	{
		p_AudioInputDvcNameArrPtPt = ( VarStr * * )calloc( p_AudioInputDvcTotal, sizeof( VarStr * ) );
		if( p_AudioInputDvcNameArrPtPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "创建并清空音频输入设备名称数组的内存块失败。" );
			goto Out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AudioInputDvcTotal; p_TmpUint++ )
		{
			p_MMResult = waveInGetDevCaps( p_TmpUint, &p_WaveInCaps, sizeof( WAVEINCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrFmtCpy( ErrInfoVarStrPt, "获取音频输入设备功能失败。原因：%s", FuncGetErrInfo( p_MMResult, 'M' ) );
				goto Out; //本函数返回失败。
			}
			if( VarStrInitByStr( &p_AudioInputDvcNameArrPtPt[p_TmpUint], p_WaveInCaps.szPname ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建音频输入设备名称的动态字符串失败。" );
				goto Out; //本函数返回失败。
			}
		}

		*AudioInputDvcNameArrPtPtPt = p_AudioInputDvcNameArrPtPt; //设置音频输入设备名称数组的指针。
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AudioInputDvcNameArrPtPt, p_AudioInputDvcTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAudioOutputDvcName
 * 功能说明：获取音频输出设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AudioOutputDvcNameArrPtPtPt：[输出]，存放用于存放音频输出设备名称数组指针的指针，不能为NULL。
			 AudioOutputDvcTotalPt：[输出]，存放用于存放音频输出设备总数的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAudioOutputDvcName( VarStr * * * AudioOutputDvcNameArrPtPtPt, UINT * AudioOutputDvcTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_AudioOutputDvcNameArrPtPt = NULL; //存放音频输出设备名称数组的指针。
	UINT p_AudioOutputDvcTotal = 0; //存放音频输出设备的总数。
	WAVEOUTCAPS p_WaveOutCaps; //存放音频输出设备的功能。
	MMRESULT p_MMResult;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AudioOutputDvcNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输出设备名称数组指针的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( AudioOutputDvcTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输出设备总数的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	p_AudioOutputDvcTotal = waveOutGetNumDevs();
	*AudioOutputDvcTotalPt = p_AudioOutputDvcTotal; //设置音频输出设备的总数。

	//设置音频输出设备名称数组。
	{
		p_AudioOutputDvcNameArrPtPt = ( VarStr * * )calloc( p_AudioOutputDvcTotal, sizeof( VarStr * ) );
		if( p_AudioOutputDvcNameArrPtPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "创建并清空音频输出设备名称数组的内存块失败。" );
			goto Out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AudioOutputDvcTotal; p_TmpUint++ )
		{
			p_MMResult = waveOutGetDevCaps( p_TmpUint, &p_WaveOutCaps, sizeof( WAVEOUTCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrFmtCpy( ErrInfoVarStrPt, "获取音频输出设备功能失败。原因：%s", FuncGetErrInfo( p_MMResult, 'M' ) );
				goto Out; //本函数返回失败。
			}
			if( VarStrInitByStr( &p_AudioOutputDvcNameArrPtPt[p_TmpUint], p_WaveOutCaps.szPname ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建音频输出设备名称的动态字符串失败。" );
				goto Out; //本函数返回失败。
			}
		}

		*AudioOutputDvcNameArrPtPtPt = p_AudioOutputDvcNameArrPtPt; //设置音频输出设备名称数组的指针。
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AudioOutputDvcNameArrPtPt, p_AudioOutputDvcTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetVideoInputDvcName
 * 功能说明：获取视频输入设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：VideoInputDvcNameArrPtPtPt：[输出]，存放用于存放视频输入设备名称数组指针的指针，不能为NULL。
			 VideoInputDvcTotalPt：[输出]，存放用于存放视频输入设备总数的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetVideoInputDvcName( VarStr * * * VideoInputDvcNameArrPtPtPt, UINT * VideoInputDvcTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_VideoInputDvcNameArrPtPt = NULL; //存放视频输入设备名称数组的指针。
	VarStr * * p_TmpVarStrPtPt = NULL;
	UINT p_VideoInputDvcTotal = 0; //存放视频输入设备的总数。
	ICreateDevEnum * p_CreateDevEnumPt = NULL;
	IEnumMoniker * p_EnumMonikerPt = NULL;
	IMoniker * p_MonikerPt = NULL;
	IPropertyBag * p_PropertyBagPt = NULL;
	ULONG p_Ulong;
	
	//判断各个变量是否正确。
	if( VideoInputDvcNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放视频输入设备名称数组指针的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( VideoInputDvcTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放视频输入设备总数的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	//初始化COM库。
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY );

	//创建系统设备枚举器。
	if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建系统设备枚举器失败。" );
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
					p_VideoInputDvcTotal++;
					p_TmpVarStrPtPt = ( VarStr * * )realloc( p_VideoInputDvcNameArrPtPt, p_VideoInputDvcTotal * sizeof( VarStr * ) );
					if( p_TmpVarStrPtPt == NULL )
					{
						VarStrCpy( ErrInfoVarStrPt, "创建视频输入设备名称数组的内存块失败。" );
						goto Out; //本函数返回失败。
					}
					p_VideoInputDvcNameArrPtPt = p_TmpVarStrPtPt;
					p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1] = NULL;
					if( VarStrInit( &p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1], wcslen( varName.bstrVal ) + 1 ) != 0 )
					{
						VarStrCpy( ErrInfoVarStrPt, "创建视频输入设备名称的动态字符串失败。" );
						goto Out; //本函数返回失败。
					}
					WideCharToMultiByte( CP_OEMCP, 0, varName.bstrVal, -1, p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1]->m_StrPt, p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1]->m_StrSz, NULL, NULL );
					p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1]->m_StrLen = p_VideoInputDvcNameArrPtPt[p_VideoInputDvcTotal - 1]->m_StrSz - 1;
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

	*VideoInputDvcNameArrPtPtPt = p_VideoInputDvcNameArrPtPt; //设置视频输入设备名称数组的指针。
	*VideoInputDvcTotalPt = p_VideoInputDvcTotal; //设置视频输入设备的总数。

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_VideoInputDvcNameArrPtPt, p_VideoInputDvcTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdDstoyDvcName
 * 功能说明：销毁设备名称数组。
 * 参数说明：DvcNameArrPtPt：[输出]，存放设备名称数组的指针，不能为NULL。
			 DvcTotal：[输出]，存放设备的总数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDstoyDvcName( VarStr * * DvcNameArrPtPt, UINT DvcTotal, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( DvcNameArrPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "设备名称数组的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	//循环销毁所有的动态字符串。
	for( p_TmpUint = 0; p_TmpUint < DvcTotal; p_TmpUint++ )
	{
		if( DvcNameArrPtPt[p_TmpUint] != NULL )
		{
			VarStrDstoy( DvcNameArrPtPt[p_TmpUint] );
		}
	}

	free( DvcNameArrPtPt ); //销毁设备名称数组的内存块。

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseAudioInput
 * 功能说明：设置是否使用音频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseAudioInput：[输入]，存放是否使用音频输入，为非0表示要使用，为0表示不使用。
			 SamplingRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000、48000。
			 FrameLenMsec：[输入]，存放帧的长度，单位毫秒，取值只能为10、20、30。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseAudioInput( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAudioInput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAudioInput != 0 ) && ( SamplingRate != 8000 ) && ( SamplingRate != 16000 ) && ( SamplingRate != 32000 ) && ( SamplingRate != 48000 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "采样频率不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAudioInput != 0 ) && ( ( FrameLenMsec <= 0 ) || ( FrameLenMsec % 10 != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的长度不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput = IsUseAudioInput;
    MediaPocsThrdPt->m_AudioInput.m_SamplingRate = SamplingRate;
    MediaPocsThrdPt->m_AudioInput.m_FrameLen = FrameLenMsec * SamplingRate / 1000;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseNoAec
 * 功能说明：设置音频输入不使用声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseNoAec( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseSpeexAec
 * 功能说明：设置音频输入要使用Speex声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FilterLen：[输入]，存放Speex声学回音消除器滤波器的长度，单位毫秒。
			 IsSaveMemFile：[输入]，存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathStrPt：[输入]，存放Speex声学回音消除器的内存块文件完整路径字符串的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "Speex声学回音消除器的内存块文件完整路径字符串的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 1;
	MediaPocsThrdPt->m_AudioInput.m_SpeexAecFilterLen = FilterLen;
	MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsUseRec = IsUseRec;
    MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoMutp = EchoMutp;
    MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoCntu = EchoCntu;
    MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupes = EchoSupes;
    MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupesAct = EchoSupesAct;
	MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VarStrCpy( MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto Out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 0;
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseWebRtcAecm
 * 功能说明：设置音频输入要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 2;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode = IsUseCNGMode;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmEchoMode = EchoMode;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmDelay = Delay;
	
	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseWebRtcAec
 * 功能说明：设置音频输入要使用WebRtc浮点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             EchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             Delay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             IsUseDelayAgnosticMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             IsUseExtdFilterMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             IsUseRefinedFilterAdaptAecMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             IsUseAdaptAdjDelay：[输入]，存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
             IsSaveMemFile：[输入]，存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
             MemFileFullPathVarStrPt：[输入]，存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "WebRtc浮点版声学回音消除器的内存块文件完整路径字符串的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 3;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcAecEchoMode = EchoMode;
    MediaPocsThrdPt->m_AudioInput.m_WebRtcAecDelay = Delay;
    MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode = IsUseDelayAgnosticMode;
    MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode = IsUseExtdFilterMode;
    MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VarStrCpy( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto Out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 0;
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseSpeexWebRtcAec
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
             WebRtcAecIsUseDelayAgnosticMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseExtdFilterMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseRefinedFilterAdaptAecMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseAdaptAdjDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
			 IsUseSameRoomAec：[输入]，存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
			 SameRoomEchoMinDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位毫秒，取值区间为[1,2147483647]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgnosticMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatAec = 4;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWorkMode = WorkMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen = SpeexAecFilterLen;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec = SpeexAecIsUseRec;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp = SpeexAecEchoMutp;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu = SpeexAecEchoCntu;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes = SpeexAecEchoSupes;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode = WebRtcAecmEchoMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay = WebRtcAecmDelay;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode = WebRtcAecEchoMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay = WebRtcAecDelay;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode = WebRtcAecIsUseDelayAgnosticMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec = IsUseSameRoomAec;
	MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay = SameRoomEchoMinDelay;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseNoNs
 * 功能说明：设置音频输入不使用噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseNoNs( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatNs = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
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
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatNs = 1;
	MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseNs = IsUseNs;
	MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsNoiseSupes = NoiseSupes;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseDereverb = IsUseDereverb;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetUseWebRtcNsx
 * 功能说明：设置音频输入要使用WebRtc定点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatNs = 2;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseWebRtcNs
 * 功能说明：设置音频输入要使用WebRtc浮点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatNs = 3;
	MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseRNNoise
 * 功能说明：设置音频输入要使用RNNoise噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatNs = 4;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputIsUseSpeexPrpocsOther
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
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputIsUseSpeexPrpocsOther( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther = IsUseOther;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseVad = IsUseVad;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbStart = VadProbStart;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbCntu = VadProbCntu;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseAgc = IsUseAgc;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcLevel = AgcLevel;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcIncrement = AgcIncrement;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcDecrement = AgcDecrement;
    MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcMaxGain = AgcMaxGain;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetUsePcm
 * 功能说明：设置音频输入要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUsePcm( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseSpeexEncoder
 * 功能说明：设置音频输入要使用Speex编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             UseCbrOrVbr：[输入]，存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             Qualt：[输入]，存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             Cmplxt：[输入]，存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             PlcExpectedLossRate：[输入]，存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseSpeexEncoder( MediaPocsThrd * MediaPocsThrdPt, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExpectedLossRate, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder = 1;
	MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr = UseCbrOrVbr;
    MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderQualt = Qualt;
    MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderCmplxt = Cmplxt;
    MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate = PlcExpectedLossRate;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseOpusEncoder
 * 功能说明：设置音频输入要使用Opus编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseOpusEncoder( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder = 2;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputIsSaveAudioToFile
 * 功能说明：设置音频输入是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioInputFileFullPathStrPt：[输入]，存放音频输入文件完整路径字符串的指针。
			 AudioResultFileFullPathStrPt：[输入]，存放音频结果文件完整路径字符串的指针。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputIsSaveAudioToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAudioToFile, const char * AudioInputFileFullPathStrPt, const char * AudioResultFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioInputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入文件的完整路径字符串不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioResultFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频结果文件的完整路径字符串不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_IsSaveAudioToFile = IsSaveAudioToFile;
	if( IsSaveAudioToFile != 0 )
	{
		if( VarStrCpy( MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt, AudioInputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输入文件完整路径动态字符串失败。" );
			goto Out;
		}
		if( VarStrCpy( MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt, AudioResultFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频结果文件完整路径动态字符串失败。" );
			goto Out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioInput.m_IsSaveAudioToFile = 0;
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputIsDrawAudioOscilloToWnd
 * 功能说明：设置音频输入是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioInputOscilloWndHdl：[输入]，存放音频输入波形窗口的句柄。
			 AudioResultOscilloWndHdl：[输入]，存放音频结果波形窗口的句柄。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputIsDrawAudioOscilloToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAudioOscilloToWnd, HWND AudioInputOscilloWndHdl, HWND AudioResultOscilloWndHdl, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAudioOscilloToWnd != 0 ) && ( AudioInputOscilloWndHdl == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入波形窗口的句柄不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAudioOscilloToWnd != 0 ) && ( AudioResultOscilloWndHdl == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频结果波形窗口的句柄不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_IsDrawAudioOscilloToWnd = IsDrawAudioOscilloToWnd;
	if( IsDrawAudioOscilloToWnd != 0 )
	{
		MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloWndHdl = AudioInputOscilloWndHdl;
		MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloWndHdl = AudioResultOscilloWndHdl;
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioInput.m_IsDrawAudioOscilloToWnd = 0;
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputUseDvc
 * 功能说明：设置音频输入使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 AudioInputDvcID：[输入]，存放音频输入设备的标识符，从0到音频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AudioInputDvcID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( AudioInputDvcID != WAVE_MAPPER ) && ( AudioInputDvcID >= waveInGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入设备的标识符不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcID = AudioInputDvcID;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioInputIsMute
 * 功能说明：设置音频输入是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioInputIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioInput.m_AudioInputIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseAudioOutput
 * 功能说明：设置是否使用音频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseAudioOutput：[输入]，存放是否使用音频输出，为非0表示要使用，为0表示不使用。
			 SamplingRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000、48000。
			 FrameLenMsec：[输入]，存放帧的长度，单位毫秒，取值只能为10、20、30。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseAudioOutput( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAudioOutput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAudioOutput != 0 ) && ( SamplingRate != 8000 ) && ( SamplingRate != 16000 ) && ( SamplingRate != 32000 ) && ( SamplingRate != 48000 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "采样频率不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseAudioOutput != 0 ) && ( ( FrameLenMsec <= 0 ) || ( FrameLenMsec % 10 != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的长度不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput = IsUseAudioOutput;
    MediaPocsThrdPt->m_AudioOutput.m_SamplingRate = SamplingRate;
    MediaPocsThrdPt->m_AudioOutput.m_FrameLen = FrameLenMsec * SamplingRate / 1000;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputUsePcm
 * 功能说明：设置音频输出要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputUsePcm( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputUseSpeexDecoder
 * 功能说明：设置音频输出要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             IsUsePerceptualEnhancement：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputUseSpeexDecoder( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePerceptualEnhancement, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder = 1;
    MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement = IsUsePerceptualEnhancement;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputUseOpusDecoder
 * 功能说明：设置音频输出要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputUseOpusDecoder( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder = 2;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputIsSaveAudioToFile
 * 功能说明：设置音频输出是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioOutputFileFullPathStrPt：[输入]，存放音频输出文件完整路径字符串的指针。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputIsSaveAudioToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAudioToFile, const char * AudioOutputFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioOutputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出文件的完整路径字符串不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_IsSaveAudioToFile = IsSaveAudioToFile;
	if( IsSaveAudioToFile != 0 )
	{
		if( VarStrCpy( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt, AudioOutputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输出文件完整路径动态字符串失败。" );
			goto Out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioOutput.m_IsSaveAudioToFile = 0;
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputIsDrawAudioOscilloToWnd
 * 功能说明：设置音频输出是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsDrawAudioOscilloToWnd：[输出]，存放是否绘制音频波形到窗口，非0表示要绘制，0表示不绘制。
			 AudioOutputOscilloWndHdl：[输出]，存放音频输出波形窗口的句柄。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputIsDrawAudioOscilloToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAudioOscilloToWnd, HWND AudioOutputOscilloWndHdl, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsDrawAudioOscilloToWnd != 0 ) && ( AudioOutputOscilloWndHdl == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出波形窗口的句柄不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_IsDrawAudioOscilloToWnd = IsDrawAudioOscilloToWnd;
	if( IsDrawAudioOscilloToWnd != 0 )
	{
		MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloWndHdl = AudioOutputOscilloWndHdl;
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AudioOutput.m_IsDrawAudioOscilloToWnd = 0;
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputUseDvc
 * 功能说明：设置音频输出使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 AudioOutputDvcID：[输入]，存放音频输出设备的标识符，从0到音频输出设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT AudioOutputDvcID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( AudioOutputDvcID != WAVE_MAPPER ) && ( AudioOutputDvcID >= waveOutGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出设备的标识符不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcID = AudioOutputDvcID;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAudioOutputIsMute
 * 功能说明：设置音频输出是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAudioOutputIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseVideoInput
 * 功能说明：设置是否使用视频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVideoInput：[输入]，存放是否使用视频输入，为非0表示要使用，为0表示不使用。
			 MaxSamplingRate：[输入]，存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
			 FrameWidth：[输入]，存放帧的宽度，单位为像素。
			 FrameHeight：[输入]，存放帧的高度，单位为像素。
			 VideoInputPrvwWnd：[输入]，存放视频输入预览窗口的句柄，可以为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseVideoInput( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVideoInput, int32_t MaxSamplingRate, int32_t FrameWidth, int32_t FrameHeight, HWND VideoInputPrvwWnd, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( MaxSamplingRate < 1 ) || ( MaxSamplingRate > 60 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "最大采样频率不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( FrameWidth <= 0 ) || ( ( FrameWidth & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的宽度不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( FrameHeight <= 0 ) || ( ( FrameHeight & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的高度不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput = IsUseVideoInput;
    MediaPocsThrdPt->m_VideoInput.m_MaxSamplingRate = MaxSamplingRate;
    MediaPocsThrdPt->m_VideoInput.m_FrameWidth = FrameWidth;
	MediaPocsThrdPt->m_VideoInput.m_FrameHeight = FrameHeight;
	MediaPocsThrdPt->m_VideoInput.m_VideoInputPrvwWndHdl = VideoInputPrvwWnd;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoInputUseYU12
 * 功能说明：设置视频输入要使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoInputUseYU12( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoInputUseOpenH264Encoder
 * 功能说明：设置视频输入要使用OpenH264编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VideoType：[输入]，存放OpenH264编码器的视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
             EncodedBitrate：[输入]，存放OpenH264编码器的编码后比特率，单位为bps。
             BitrateControlMode：[输入]，存放OpenH264编码器的比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
             IDRFrameIntvl：[输入]，存放OpenH264编码器的IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
             Cmplxt：[输入]，存放OpenH264编码器的复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,2]。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoInputUseOpenH264Encoder( MediaPocsThrd * MediaPocsThrdPt, int32_t VideoType, int32_t EncodedBitrate, int32_t BitrateControlMode, int32_t IDRFrameIntvl, int32_t Cmplxt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder = 1;
	MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderVideoType = VideoType;
    MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderEncodedBitrate = EncodedBitrate;
    MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderBitrateControlMode = BitrateControlMode;
    MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderIDRFrameIntvl = IDRFrameIntvl;
	MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderCmplxt = Cmplxt;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoInputUseDvc
 * 功能说明：设置视频输入使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 VideoInputDvcID：[输入]，存放视频输入设备的标识符，从0到视频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoInputUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT VideoInputDvcID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcID = VideoInputDvcID;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoInputIsBlack
 * 功能说明：设置视频输入是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输入]，存放视频输入设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoInputIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VideoInput.m_VideoInputIsBlack = IsBlack;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseVideoOutput
 * 功能说明：设置是否使用视频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVideoOutput：[输入]，存放是否使用视频输出，为非0表示要使用，为0表示不使用。
			 FrameWidth：[输入]，存放帧的宽度，单位为像素。
			 FrameHeight：[输入]，存放帧的高度，单位为像素。
			 VideoOutputDspyWnd：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 VideoOutputDspyScale：[输入]，存放视频输出缩放的倍数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseVideoOutput( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseVideoOutput, HWND VideoOutputDspyWnd, float VideoOutputDspyScale, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput = IsUseVideoOutput;
	MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyWndHdl = VideoOutputDspyWnd;
	MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale = VideoOutputDspyScale;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoOutputUseYU12
 * 功能说明：设置视频输出要使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoOutputUseYU12( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder = 0;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoOutputUseOpenH264Decoder
 * 功能说明：设置视频输出要使用OpenH264解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             DecodeThrdNum：[输出]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoOutputUseOpenH264Decoder( MediaPocsThrd * MediaPocsThrdPt, int32_t DecodeThrdNum, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder = 1;
	MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderDecodeThrdNum = DecodeThrdNum;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVideoOutputIsBlack
 * 功能说明：设置视频输出是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVideoOutputIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	
	MediaPocsThrdPt->m_VideoOutput.m_VideoOutputIsBlack = IsBlack;

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdStart
* 功能说明：启动媒体处理线程。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdStart( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( MediaPocsThrdPt->m_MediaPocsThrdHdl != NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程已经启动。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdPt->m_MediaPocsThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )MediaPocsThrdRun, MediaPocsThrdPt, 0, NULL ); //创建并启动媒体处理线程。
	if( MediaPocsThrdPt->m_MediaPocsThrdHdl == NULL )
	{
		VarStrFmtCpy( ErrInfoVarStrPt, "创建并启动媒体处理线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
		goto Out; //本函数返回失败。
	}

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdRqirExit
* 功能说明：请求媒体处理线程退出。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
            ExitFlag：[输入]，存放退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
			IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示阻塞。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int ExitFlag, int IsBlockWait, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "退出标记不正确。" );
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

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaPocsThrdDstoy
* 功能说明：销毁媒体处理线程。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDstoy( MediaPocsThrd * MediaPocsThrdPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的指针不正确。" );
		goto Out; //本函数返回失败。
	}

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 1, NULL ); //请求媒体处理线程退出，并阻塞等待。

	//销毁设置文件的完整路径动态字符串。
	if( MediaPocsThrdPt->m_StngFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_StngFileFullPathVarStrPt );
		MediaPocsThrdPt->m_StngFileFullPathVarStrPt = NULL;
	}
	
	//销毁Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt );
		MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt = NULL;
	}
	
	//销毁WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt );
		MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt = NULL;
	}

	//销毁音频输入文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt );
		MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt = NULL;
	}

	//销毁音频结果文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt );
		MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt = NULL;
	}
	
	//销毁音频输出文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt );
		MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt = NULL;
	}

	//销毁错误信息动态字符串。
	if( MediaPocsThrdPt->m_ErrInfoVarStrPt != NULL )
	{
		VarStrDstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt );
		MediaPocsThrdPt->m_ErrInfoVarStrPt = NULL;
	}

	//销毁媒体处理线程的内存块。
	free( MediaPocsThrdPt );

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	p_Result = 0; //设置本函数执行成功。

	Out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioInputThrdRun
 * 功能说明：音频输入线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioInputThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	//开始音频输入循环。
	while( GetMessage( &MediaPocsThrdPt->m_AudioInput.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaPocsThrdPt->m_AudioInput.m_Msg.message == WIM_DATA ) //如果是音频输入缓冲区块读取完毕消息。
		{
			if( ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioInput.m_Msg.lParam )->dwBytesRecorded != MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) ) //如果本次音频输入帧的长度不等于帧的长度，就表示本次音频输入帧不完整，丢弃。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "本次音频输入帧的长度不等于帧的长度，就表示本次音频输入帧不完整，丢弃。" );
				waveInAddBuffer( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AudioInput.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。
				continue;
			}

			//获取一个音频输入空闲帧。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.GetTotal( &MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal, 1, NULL ); //获取音频输入空闲帧链表的元素总数。
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal > 0 ) //如果音频输入空闲帧链表中有音频输入空闲帧。
			{
				//从音频输入空闲帧链表中取出第一个音频输入空闲帧。
				{
					MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.Lock( NULL ); //音频输入空闲帧链表的互斥锁加锁。
					MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt, NULL, 0, NULL );
					MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.DelHead( 0, NULL );
					MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.Unlock( NULL ); //音频输入空闲帧链表的互斥锁解锁。
				}
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：从音频输入空闲帧链表中取出第一个音频输入空闲帧，音频输入空闲帧链表元素个数：%" PRIuPTR "。", MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal );
			}
			else //如果音频输入空闲帧链表中没有音频输入空闲帧。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "音频输入线程：音频输入空闲帧链表中没有音频输入空闲帧，本次音频输入帧丢弃。" );
			}

			//追加本次音频输入帧到音频输入帧链表。
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt != NULL ) //如果获取了一个音频输入空闲帧。
			{
				memcpy( MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt, ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioInput.m_Msg.lParam )->lpData, MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.PutTail( &MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt, 1, NULL );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt = NULL;
			}

			waveInAddBuffer( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AudioInput.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。

			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AudioInput.m_NowTimeMsec );
				LOGFI( "音频输入线程：本次音频输入帧读取完毕，耗时 %" PRIu64 " 毫秒。", MediaPocsThrdPt->m_AudioInput.m_NowTimeMsec - MediaPocsThrdPt->m_AudioInput.m_LastTimeMsec );
				MediaPocsThrdPt->m_AudioInput.m_LastTimeMsec = MediaPocsThrdPt->m_AudioInput.m_NowTimeMsec;
			}
		}
		else if( MediaPocsThrdPt->m_AudioInput.m_Msg.message == WIM_OPEN ) //如果是音频输入设备打开消息。
		{
			SetThreadPriority( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AudioInput.m_LastTimeMsec );
				LOGI( "音频输入线程：开始准备音频输入。" );
			}
		}
		else if( MediaPocsThrdPt->m_AudioInput.m_Msg.message == WIM_CLOSE ) //如果是音频输入设备关闭消息。
		{
			MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcID = WAVE_MAPPER; //设置音频输入设备的标识符为默认的音频输入设备。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输入设备。" );
			break;
		}
	}

	if( MediaPocsThrdPt->m_AudioInput.m_Msg.message == WM_QUIT )
	{
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioOutputThrdRun
 * 功能说明：音频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioOutputThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	//开始音频输出循环。
	while( GetMessage( &MediaPocsThrdPt->m_AudioOutput.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaPocsThrdPt->m_AudioOutput.m_Msg.message == WOM_DONE ) //如果是音频输出缓冲区块写入完毕消息。
		{
			//调用用户定义的写入音频输出帧函数，并解码成PCM原始数据。
			switch( MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder ) //使用什么解码器。
			{
				case 0: //如果使用PCM原始数据。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen = ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength;
					MediaPocsThrdPt->m_UserWriteAudioOutputFrameFuncPt( MediaPocsThrdPt, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->lpData, NULL, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen );
					break;
				}
				case 1: //如果使用Speex编解码器。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen = MediaPocsThrdPt->m_AudioOutput.m_FrameLen;
					MediaPocsThrdPt->m_UserWriteAudioOutputFrameFuncPt( MediaPocsThrdPt, NULL, MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen );

					//使用Speex解码器。
					if( SpeexDecoderPocs( MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderPt, MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->lpData ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：使用Speex解码器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：使用Speex解码器失败。" );
					}
					break;
				}
				case 2: //如果使用Opus编解码器。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：暂不支持使用Opus解码器。" );
				}
			}
			
			//判断音频输出是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
            if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIsMute != 0 )
            {
				memset( ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->lpData, 0, ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength );
            }

			//写入本次音频输出帧到音频输出设备。
			waveOutWrite( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl, ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam, sizeof( WAVEHDR ) );
			
			//调用用户定义的获取PCM格式音频输出帧函数。
			MediaPocsThrdPt->m_UserGetPcmAudioOutputFrameFuncPt( MediaPocsThrdPt, ( int16_t * )( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->lpData, ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength );

			//获取一个音频输出空闲帧。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.GetTotal( &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal, 1, NULL );
			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal > 0 ) //如果音频输出空闲帧链表中有音频输出空闲帧。
			{
				//从音频输出空闲帧链表中取出第一个音频输出空闲帧。
				{
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.Lock( NULL ); //音频输出空闲帧链表的互斥锁加锁。
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt, NULL, 0, NULL );
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.DelHead( 0, NULL );
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.Unlock( NULL ); //音频输出空闲帧链表的互斥锁解锁。
				}
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：从音频输出空闲帧链表中取出第一个音频输出空闲帧，音频输出空闲帧链表元素个数：%" PRIuPTR "。", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal );
			}
			else //如果音频输出空闲帧链表中没有音频输出空闲帧。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：音频输出空闲帧链表中没有音频输出空闲帧，本次音频输出帧丢弃。" );
			}

			//追加本次音频输出帧到音频输出帧链表。
			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt != NULL )
			{
				memcpy( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt, ( ( WAVEHDR * )MediaPocsThrdPt->m_AudioOutput.m_Msg.lParam )->lpData, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * sizeof( int16_t ) );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.PutTail( &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt, 1, NULL );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt = NULL;
			}

			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AudioOutput.m_NowTimeMsec );
				LOGFI( "音频输出线程：本次音频输出帧写入完毕，耗时 %" PRIu64 " 毫秒。", MediaPocsThrdPt->m_AudioOutput.m_NowTimeMsec - MediaPocsThrdPt->m_AudioOutput.m_LastTimeMsec );
				MediaPocsThrdPt->m_AudioOutput.m_LastTimeMsec = MediaPocsThrdPt->m_AudioOutput.m_NowTimeMsec;
			}
		}
		else if( MediaPocsThrdPt->m_AudioOutput.m_Msg.message == WOM_OPEN ) //如果是音频输出设备打开消息。
		{
			SetThreadPriority( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaPocsThrdPt->m_AudioOutput.m_LastTimeMsec );
				LOGI( "音频输出线程：开始准备音频输出。" );
			}
		}
		else if( MediaPocsThrdPt->m_AudioOutput.m_Msg.message == WOM_CLOSE ) //如果是音频输出设备关闭消息。
		{
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcID = WAVE_MAPPER; //设置音频输出设备的标识符为默认的音频输出设备。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输出设备。" );
			break;
		}
	}

	if( MediaPocsThrdPt->m_AudioOutput.m_Msg.message == WM_QUIT )
	{
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VideoInputThrd
 * 功能说明：视频输入线程主类。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct VideoInputThrd : public ISampleGrabberCB
{
public:
	MediaPocsThrd * m_MediaPocsThrdPt;

	VideoInputThrd( MediaPocsThrd * MediaPocsThrdPt )
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
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "视频输入线程：读取一个视频输入帧。" );

		//丢弃采样频率过快的视频输入设备帧。
		FuncGetTimeAsMsec( &m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec );
		if( m_MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec != 0 ) //如果已经设置过上一个视频输入设备帧的时间。
		{
			if( m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec - m_MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec >= m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameTimeStepMsec )
			{
				m_MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec += m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameTimeStepMsec;
			}
			else
			{
				LOGFI( "视频输入线程：采样频率过快，本次视频输入设备帧丢弃。" );
			}
		}
		else //如果没有设置过上一个视频输入设备帧的时间。
		{
			m_MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec = m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec;
		}

		//将视频输入帧复制到视频结果帧，方便处理。
		m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth * m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight * 4;
		memcpy( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, pBuffer, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen );

		//裁剪视频输入设备帧。
		if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop != 0 )
		{
			if( LibYUVPictrCrop( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight,
								 m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight,
								 m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								 NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：裁剪视频输入设备帧成功。" );
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：裁剪视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto Out;
			}
		}
		
		//垂直翻转镜像视频输入设备帧。因为视频输入设备帧是自底向上的。
		{
			if( LibYUVPictrMirror( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight,
								   1,
								   m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：垂直翻转镜像视频输入设备帧成功。" );
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：垂直翻转镜像视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto Out;
			}
		}

		//缩放视频输入设备帧。
		if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsScale != 0 )
		{
			if( LibYUVPictrScale( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight,
								  3,
								  m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight,
								  NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：缩放视频输入设备帧成功。" );
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：缩放视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto Out;
			}
		}
		
		//预览视频输入帧。
		if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputPrvwWndHdl != NULL )
		{
			//水平翻转镜像视频输入帧。
			if( LibYUVPictrMirror( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight,
								   0,
								   m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：水平翻转镜像视频输入帧成功。" );
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：水平翻转镜像视频输入帧失败，本次视频输入帧丢弃。" );
				goto Out;
			}

			//绘制视频输入帧到视频输入预览窗口。
			if( LibYUVPictrDrawToWnd( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight,
									  0,
									  m_MediaPocsThrdPt->m_VideoInput.m_VideoInputPrvwWndHdl,
									  NULL ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：绘制视频输入帧到视频输入预览窗口成功。" );
			}
			else
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：绘制视频输入帧到视频输入预览窗口失败，本次视频输入帧丢弃。" );
				goto Out;
			}
		}
		
		//获取一个视频输入空闲帧元素。
		m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.GetTotal( &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal, 1, NULL );
        if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal > 0 ) //如果视频输入空闲帧链表中有视频输入空闲帧元素。
        {
            //从视频输入空闲帧链表中取出第一个视频输入空闲帧元素。
            {
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.Lock( NULL ); //视频输入空闲帧链表的互斥锁加锁。
                m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.GetHead( NULL, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt, NULL, 0, NULL );
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.DelHead( 0, NULL );
				m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.Unlock( NULL ); //视频输入空闲帧链表的互斥锁解锁。
            }
            if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( "视频输入线程：从视频输入空闲帧链表中取出第一个视频输入空闲帧元素，视频输入空闲帧链表元素个数：%" PRIuPTR "。", m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal );
        }
        else //如果视频输入空闲帧链表中没有视频输入空闲帧元素。
        {
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGE( "视频输入线程：视频输入空闲帧链表中没有视频输入空闲帧元素，本次视频输入帧丢弃。" );
			goto Out;
        }

		//将BGRA格式视频输入帧转为YU12格式视频输入帧。
		if( LibYUVPictrFmtCnvrt( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight,
								 m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth * m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight * 3 / 2, NULL, PICTR_FMT_BT601F8_YU12_I420,
								 NULL ) == 0 )
		{
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧成功。" );
		}
		else
		{
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧失败，本次视频输入帧丢弃。" );
			goto Out;
		}
		/*FILE * p_File = fopen( "123.yuv", "wb+" );
		fwrite( p_VideoInputFrame.m_YU12VideoInputFramePt, 1, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth * m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight * 3 / 2, p_File );
		fclose( p_File );*/
		
        //判断视频输入是否黑屏。在视频输入处理完后再设置黑屏，这样可以保证视频输入处理器的连续性。
        if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIsBlack != 0 )
        {
            int p_TmpLen = m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth * m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight;
            memset( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, 0, p_TmpLen );
            memset( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt + p_TmpLen, 128, p_TmpLen / 2 );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：设置YU12格式视频输入帧为黑屏成功。" );
        }

		//使用编码器。
        switch( m_MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( OpenH264EncoderPocs( m_MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderPt,
										 m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight, m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec,
										 m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFramePt, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth * m_MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight * 3 / 2, &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFrameLen,
										 NULL ) == 0 )
                {
                    if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "视频输入线程：使用OpenH264编码器成功。H264格式视频输入帧的长度：%" PRIuPTR "，时间戳：%" PRIu64 "，类型：%hhd。", m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFrameLen, m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec, m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFramePt[4] );
                }
                else
                {
                    if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：使用OpenH264编码器失败，本次视频输入帧丢弃。" );
					goto Out;
                }
                break;
            }
        }

		//追加本次视频输入帧到视频输入帧链表。
		m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.PutTail( &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt, 1, NULL );
		m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt = NULL;

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &m_MediaPocsThrdPt->m_VideoInput.m_NowTimeMsec );
			LOGFI( "视频输入线程：本次视频输入帧处理完毕，耗时 %" PRIu64 " 毫秒。", m_MediaPocsThrdPt->m_VideoInput.m_NowTimeMsec - m_MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec );
		}

		Out:
		if( m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt != NULL ) //如果获取的视频输入空闲帧没有追加到视频输入帧链表。
		{
			m_MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.PutTail( &m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt, 1, NULL );
			m_MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt = NULL;
		}
		return 0;
	}
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VideoOutputThrdRun
 * 功能说明：视频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI VideoOutputThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	SetThreadPriority( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
	
    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输出线程：开始准备视频输出。" );

    //开始视频输出循环。
    while( true )
    {
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &MediaPocsThrdPt->m_VideoOutput.m_LastTimeMsec );

		//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
		switch( MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder ) //使用什么解码器。
		{
			case 0: //如果使用YU12原始数据。
			{
				//调用用户定义的写入视频输出帧函数。
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth = 0;
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight = 0;
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz;
				MediaPocsThrdPt->m_UserWriteVideoOutputFrameFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight, NULL, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen );
				
				if( ( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth > 0 ) && ( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight > 0 ) ) //如果本次写入了视频输出帧。
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：使用YU12原始数据成功。YU12格式帧宽度：%" PRId32 "，YU12格式帧高度：%" PRId32 "。", MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight );
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
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz;
				MediaPocsThrdPt->m_UserWriteVideoOutputFrameFuncPt( MediaPocsThrdPt, NULL, NULL, NULL, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen );

				if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen > 0 ) //如果本次写入了视频输出帧。
				{
					//使用OpenH264解码器。
					if( OpenH264DecoderPocs( MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderPt,
											 MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen,
											 MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight,
											 NULL ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：使用OpenH264解码器成功。已解码YU12格式帧宽度：%" PRId32 "，已解码YU12格式帧高度：%" PRId32 "。", MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight );
                        if( ( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth == 0 ) || ( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight == 0 ) ) goto Skip; //如果未解码出YU12格式帧，就把本次视频输出帧丢弃。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "视频输出线程：使用OpenH264解码器失败，本次视频输出帧丢弃。" );
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
		MediaPocsThrdPt->m_UserGetYU12VideoOutputFrameFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight );
		
        //判断视频输出设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
        if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputIsBlack != 0 )
        {
            int p_TmpLen = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth * MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight;
            memset( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, 0, p_TmpLen );
            memset( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt + p_TmpLen, 128, p_TmpLen / 2 );
        }

		//缩放视频输出帧。
		if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale != 1.0f )
		{
			if( LibYUVPictrScale( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_BT601F8_YU12_I420, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight,
								  3,
								  MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz, NULL, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth * MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight * MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale,
								  NULL ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输出线程：缩放视频输出帧失败，本次视频输出帧丢弃。" );
				goto Skip;
			}
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt; MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt; MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt; //交换视频结果帧和视频临时帧。

			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth *= MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale;
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight *= MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale;
		}

		//将本次YU12格式视频输出帧转为BGRA格式视频输出帧。
		if( LibYUVPictrFmtCnvrt( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_BT601F8_YU12_I420, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight,
								 MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz, NULL, PICTR_FMT_SRGBF8_BGRA8888,
								 NULL ) != 0 )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "视频输出线程：将本次YU12格式视频输出帧转为BGRA格式视频输出帧失败，本次视频输出帧丢弃。" );
			goto Skip;
		}
		MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt; MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt; MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt = MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt; //交换视频结果帧和视频临时帧。

        //显示视频输出帧。
		if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyWndHdl != NULL )
		{
			//绘制视频输出帧到视频输出显示窗口。
			if( LibYUVPictrDrawToWnd( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight,
									  0,
									  MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyWndHdl,
									  NULL ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "视频输出线程：绘制视频输出帧到视频输出显示窗口失败，本次视频输出帧丢弃。" );
				goto Skip;
			}
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &MediaPocsThrdPt->m_VideoOutput.m_NowTimeMsec );
			LOGFI( "视频输出线程：本次视频输出帧处理完毕，耗时 %" PRIu64 " 毫秒。", MediaPocsThrdPt->m_VideoOutput.m_NowTimeMsec - MediaPocsThrdPt->m_VideoOutput.m_LastTimeMsec );
		}

		Skip:
        if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "视频输出线程：本线程接收到退出请求，开始准备退出。" );
            goto Out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	Out:
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：本线程已退出。" );
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

	ReInit:
	MediaPocsThrdPt->m_RunFlag = RUN_FLAG_INIT; //设置本线程运行标记为刚开始运行正在初始化。

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本地代码的指令集名称为%s。退出标记：%d。", ( sizeof( size_t ) == 4 ) ? "x86" : "x64", MediaPocsThrdPt->m_ExitFlag );

	if( MediaPocsThrdPt->m_ExitFlag != 3 ) //如果需要执行用户定义的初始化函数。
	{
		MediaPocsThrdPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
		MediaPocsThrdPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。

		//调用用户定义的初始化函数。
		p_TmpInt321 = MediaPocsThrdPt->m_UserInitFuncPt( MediaPocsThrdPt );
		if( p_TmpInt321 == 0 )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的初始化函数成功。返回值：%d。", p_TmpInt321 );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：调用用户定义的初始化函数失败。返回值：%d。", p_TmpInt321 );
			goto Out;
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
		FILE * p_StngFileStreamPt;

		p_StngFileStreamPt = fopen( MediaPocsThrdPt->m_StngFileFullPathVarStrPt->m_StrPt, "wb+" );
		if( p_StngFileStreamPt == NULL )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开设置文件 %s 失败。原因：%s", MediaPocsThrdPt->m_StngFileFullPathVarStrPt->m_StrPt, FuncGetErrInfo( errno, 'E' ) );
			goto Out;
		}

		fprintf( p_StngFileStreamPt, "m_IsSaveStngToFile：%" PRId32 "\n", MediaPocsThrdPt->m_IsSaveStngToFile );
		fprintf( p_StngFileStreamPt, "m_StngFileFullPathVarStrPt->m_StrPt：%s\n", ( MediaPocsThrdPt->m_StngFileFullPathVarStrPt != NULL ) ? MediaPocsThrdPt->m_StngFileFullPathVarStrPt->m_StrPt : NULL );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_IsPrintLog：%" PRId32 "\n", MediaPocsThrdPt->m_IsPrintLog );
		fprintf( p_StngFileStreamPt, "m_IsShowToast：%" PRId32 "\n", MediaPocsThrdPt->m_IsShowToast );
		fprintf( p_StngFileStreamPt, "\n" );

		fprintf( p_StngFileStreamPt, "m_AudioInput.m_IsUseAudioInput：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SamplingRate：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SamplingRate );
        fprintf( p_StngFileStreamPt, "m_AudioInput.m_FrameLen：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_FrameLen );
        fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_UseWhatAec：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_UseWhatAec );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecFilterLen：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecFilterLen );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecIsUseRec：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsUseRec );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecEchoMutp：%f\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoMutp );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecEchoCntu：%f\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoCntu );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecEchoSupes：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupes );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecEchoSupesAct：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupesAct );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecIsSaveMemFile：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsSaveMemFile );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt：%s\n", MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecmIsUseCNGMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecmEchoMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmEchoMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecmDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmDelay );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecEchoMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecEchoMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecDelay );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseExtdFilterMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecIsSaveMemFile：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsSaveMemFile );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt：%s\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWorkMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWorkMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp：%f\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu：%f\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_UseWhatNs：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_UseWhatNs );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsIsUseNs：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseNs );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsNoiseSupes：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsNoiseSupes );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsIsUseDereverb：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseDereverb );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcNsxPolicyMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPolicyMode );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_WebRtcNsPolicyMode：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPolicyMode );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_IsUseSpeexPrpocsOther：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsIsUseVad：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseVad );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsVadProbStart：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbStart );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsVadProbCntu：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbCntu );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsIsUseAgc：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseAgc );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsAgcLevel：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcLevel );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsAgcIncrement：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcIncrement );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsAgcDecrement：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcDecrement );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexPrpocsAgcMaxGain：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcMaxGain );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_UseWhatEncoder：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexEncoderUseCbrOrVbr：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexEncoderQualt：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderQualt );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexEncoderCmplxt：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderCmplxt );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_SpeexEncoderPlcExpectedLossRate：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_IsSaveAudioToFile：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_IsSaveAudioToFile );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt：%s\n", MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt：%s\n", MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_IsDrawAudioOscilloToWnd：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_IsDrawAudioOscilloToWnd );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioInputOscilloWndHdl：%p\n", MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioResultOscilloWndHdl：%p\n", MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloWndHdl );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioInputDvcID：%u\n", MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcID );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioInputWaveHdrTotal：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal );
		fprintf( p_StngFileStreamPt, "m_AudioInput.m_AudioInputIsMute：%" PRId32 "\n", MediaPocsThrdPt->m_AudioInput.m_AudioInputIsMute );
		fprintf( p_StngFileStreamPt, "\n" );

		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_IsUseAudioOutput：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_SamplingRate：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_SamplingRate );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_FrameLen：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_FrameLen );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_UseWhatDecoder：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_IsSaveAudioToFile：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_IsSaveAudioToFile );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt：%s\n", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_IsDrawAudioOscilloToWnd：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_IsDrawAudioOscilloToWnd );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_AudioOutputOscilloWndHdl：%p\n", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloWndHdl );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_AudioOutputDvcID：%u\n", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcID );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_AudioOutputWaveHdrTotal：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal );
		fprintf( p_StngFileStreamPt, "m_AudioOutput.m_AudioOutputIsMute：%" PRId32 "\n", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIsMute );
		fprintf( p_StngFileStreamPt, "\n" );

		fprintf( p_StngFileStreamPt, "m_VideoInput.m_IsUseVideoInput：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_MaxSamplingRate：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_MaxSamplingRate );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_FrameWidth：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_FrameWidth );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_FrameHeight：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_FrameHeight );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_UseWhatEncoder：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_OpenH264EncoderVideoType：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderVideoType );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_OpenH264EncoderEncodedBitrate：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderEncodedBitrate );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_OpenH264EncoderBitrateControlMode：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderBitrateControlMode );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_OpenH264EncoderIDRFrameIntvl：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderIDRFrameIntvl );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_OpenH264EncoderCmplxt：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderCmplxt );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_VideoInputDvcID：%u\n", MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcID );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_VideoInputIsBlack：%" PRId32 "\n", MediaPocsThrdPt->m_VideoInput.m_VideoInputIsBlack );
		fprintf( p_StngFileStreamPt, "m_VideoInput.m_VideoInputPrvwWndHdl：%p\n", MediaPocsThrdPt->m_VideoInput.m_VideoInputPrvwWndHdl );
		fprintf( p_StngFileStreamPt, "\n" );

		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_IsUseVideoOutput：%" PRId32 "\n", MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_UseWhatDecoder：%" PRId32 "\n", MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_OpenH264DecoderDecodeThrdNum：%" PRId32 "\n", MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderDecodeThrdNum );
		fprintf( p_StngFileStreamPt, "\n" );
		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_VideoOutputDspyWndHdl：%p\n", MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyWndHdl );
		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_VideoOutputDspyScale：%p\n", MediaPocsThrdPt->m_VideoOutput.m_VideoOutputDspyScale );
		fprintf( p_StngFileStreamPt, "m_VideoOutput.m_VideoOutputIsBlack：%p\n", MediaPocsThrdPt->m_VideoOutput.m_VideoOutputIsBlack );
		fprintf( p_StngFileStreamPt, "\n" );

		fclose( p_StngFileStreamPt );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：保存设置到文件 %s 成功。", MediaPocsThrdPt->m_StngFileFullPathVarStrPt->m_StrPt );
	}

	//初始化音频输入。
	if( MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput != 0 )
	{
		//创建并初始化声学回音消除器。
		if( MediaPocsThrdPt->m_AudioInput.m_UseWhatAec != 0 ) //如果要使用声学回音消除器。
		{
			if( MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：不使用音频输出时，不能使用声学回音消除器。" );
				goto Out;
			}
			if( MediaPocsThrdPt->m_AudioOutput.m_SamplingRate != MediaPocsThrdPt->m_AudioInput.m_SamplingRate )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：音频输出的采样频率与音频输入不一致时，不能使用声学回音消除器。" );
				goto Out;
			}
			if( MediaPocsThrdPt->m_AudioOutput.m_FrameLen != MediaPocsThrdPt->m_AudioInput.m_FrameLen )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：音频输出帧的长度与音频输入不一致时，不能使用声学回音消除器。" );
				goto Out;
			}
		}
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatAec )
		{
			case 0: //如果不使用声学回音消除器。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用声学回音消除器。" );
				break;
			}
			case 1: //如果使用Speex声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
				{
					if( SpeexAecInitByMemFile( &MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器成功。", MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					}
				}
				if( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt == NULL )
				{
					if( SpeexAecInit( &MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						goto Out;
					}
				}
				break;
			}
			case 2: //如果使用WebRtc定点版声学回音消除器。
			{
				if( WebRtcAecmInit( &MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmEchoMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmDelay, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto Out;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
				{
					if( WebRtcAecInitByMemFile( &MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecDelay, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器成功。", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					}
				}
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt == NULL )
				{
					if( WebRtcAecInit( &MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecDelay, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						goto Out;
					}
				}
				break;
			}
			case 4: //如果使用SpeexWebRtc三重声学回音消除器。
			{
				if( SpeexWebRtcAecInit( &MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWorkMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec, MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto Out;
				}
				break;
			}
		}

		//创建并初始化噪音抑制器。
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs )
		{
			case 0: //如果不使用噪音抑制器。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用噪音抑制器。" );
				break;
			}
			case 1: //如果使用Speex预处理器的噪音抑制。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：稍后在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" );
				break;
			}
			case 2: //如果使用WebRtc定点版噪音抑制器。
			{
				if( WebRtcNsxInit( &MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPolicyMode, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto Out;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版噪音抑制器。
			{
				if( WebRtcNsInit( &MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPolicyMode, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto Out;
				}
				break;
			}
			case 4: //如果使用RNNoise噪音抑制器。
			{
				if( RNNoiseInit( &MediaPocsThrdPt->m_AudioInput.m_RNNoisePt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化RNNoise噪音抑制器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化RNNoise噪音抑制器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto Out;
				}
				break;
			}
		}

		//创建并初始化Speex预处理器。
		if( ( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs == 1 ) || ( MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther != 0 ) )
		{
			if( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs != 1 )
			{
				MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseNs = 0;
				MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseDereverb = 0;
			}
			if( MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther == 0 )
			{
				MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseVad = 0;
				MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseAgc = 0;
			}
			if( SpeexPrpocsInit( &MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseNs, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsNoiseSupes, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseDereverb, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseVad, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbStart, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsVadProbCntu, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseAgc, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcLevel, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcIncrement, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcDecrement, MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsAgcMaxGain, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex预处理器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex预处理器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out;
			}
		}

		//创建并初始化编码器。
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
				break;
			}
			case 1: //如果使用Speex编码器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_FrameLen != MediaPocsThrdPt->m_AudioInput.m_SamplingRate / 1000 * 20 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：帧的长度不为20毫秒不能使用Speex编码器。" );
					goto Out;
				}
				if( SpeexEncoderInit( &MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr, MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderQualt, MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderCmplxt, MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex编码器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex编码器失败。" );
					goto Out;
				}
				break;
			}
			case 2: //如果使用Opus编码器。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus编码器。" );
				goto Out;
			}
		}

		//创建并初始化音频输入Wave文件写入器、音频结果Wave文件写入器。
		if( MediaPocsThrdPt->m_AudioInput.m_IsSaveAudioToFile != 0 )
		{
			if( WaveFileWriterInit( &MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveFileWriterPt, MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt, 1, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器成功。", MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器失败。", MediaPocsThrdPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
				goto Out;
			}
			if( WaveFileWriterInit( &MediaPocsThrdPt->m_AudioInput.m_AudioResultWaveFileWriterPt, MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt, 1, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器成功。", MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器失败。", MediaPocsThrdPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
				goto Out;
			}
		}
		
        //创建并初始化音频输入波形器、音频结果波形器。
        if( MediaPocsThrdPt->m_AudioInput.m_IsDrawAudioOscilloToWnd != 0 )
        {
            if( AudioOscilloInit( &MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入波形器成功。" );
            }
            else
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入波形器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out;
            }
            if( AudioOscilloInit( &MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频结果波形器成功。" );
            }
            else
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频结果波形器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out;
            }
        }

		//创建并初始化音频输入帧链表、音频输入空闲帧链表。
		{
			MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal = 6; //音频输入帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输入帧，从而导致不同步。音频输入空闲帧链表最多存储总数与音频输入帧链表一致。
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			//创建并初始化音频输入空闲帧，并追加到音频输入空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameElmTotal; p_TmpInt322++ )
			{
				MediaPocsThrdPt->m_PcmAudioInputFramePt = ( int16_t * )malloc( MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) );
				if( MediaPocsThrdPt->m_PcmAudioInputFramePt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化音频输入空闲帧失败。" );
					goto Out; //本函数返回失败。
				}
				if( MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAudioInputFramePt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioInputFramePt );
					MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加音频输入空闲帧到音频输入空闲帧链表失败。" );
					goto Out; //本函数返回失败。
				}
				MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
			}
		}

		//初始化音频输入线程的临时变量。
		{
			MediaPocsThrdPt->m_AudioInput.m_Msg = { 0 }; //初始化消息。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt = NULL; //初始化音频输入帧的指针。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal = 0; //初始化音频输入帧链表的元数总数。
			MediaPocsThrdPt->m_AudioInput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaPocsThrdPt->m_AudioInput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化音频输入线程的临时变量成功。" );
		}

		//创建并启动音频输入线程。必须在初始化音频输入设备前初始化音频输入线程，因为音频输入设备会使用音频输入线程ID。
		{
			MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioInputThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdId );
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl != NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动音频输入线程成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动音频输入线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
				goto Out; //本函数返回失败。
			}
		}

		//创建并初始化音频输入缓冲区块数组。
		{
			MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal = 2; //音频输入缓冲区块数组的元素总数固定为2。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal * sizeof( WAVEHDR ) ); //创建并清空音频输入缓冲区块数组的内存块。
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输入缓冲区块数组的内存块失败。" );
				goto Out; //本函数返回失败。
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwUser = p_TmpInt322;
				MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwBufferLength = MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( int16_t );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwBufferLength );
				if( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输入缓冲区块数组的数据内存块失败。" );
					goto Out; //本函数返回失败。
				}
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入缓冲区块数组成功。" );
		}

		//创建并初始化音频输入设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = MediaPocsThrdPt->m_AudioInput.m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = MediaPocsThrdPt->m_AudioInput.m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt321 = waveInOpen( &MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcID, &WaveFormatEx, ( DWORD_PTR )MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdId, ( DWORD_PTR )MediaPocsThrdPt, CALLBACK_THREAD );
			if( p_TmpInt321 != MMSYSERR_NOERROR )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开音频输入设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：打开音频输入设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				goto Out;
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				p_TmpInt321 = waveInPrepareHeader( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, &MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：为音频输入设备准备音频输入缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto Out;
				}
				p_TmpInt321 = waveInAddBuffer( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, &MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将已准备的音频输入缓冲区块添加到音频输入设备上失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto Out;
				}
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入设备成功。音频输入缓冲区大小：%d * %d。", MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[0].dwBufferLength, MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal );
		}
	} //初始化音频输入完毕。

	//初始化音频输出。
	if( MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 )
	{
		//创建并初始化解码器。
		switch( MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
				break;
			}
			case 1: //如果使用Speex解码器。
			{
				if( MediaPocsThrdPt->m_AudioOutput.m_FrameLen != MediaPocsThrdPt->m_AudioOutput.m_SamplingRate / 1000 * 20 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：帧的长度不为20毫秒不能使用Speex解码器。" );
					goto Out;
				}
				if( SpeexDecoderInit( &MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderPt, MediaPocsThrdPt->m_AudioOutput.m_SamplingRate, MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex解码器成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex解码器失败。" );
					goto Out;
				}
				break;
			}
			case 2: //如果使用Opus解码器。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus解码器。" );
				goto Out;
			}
		}

		//创建并初始化音频输出Wave文件写入器。
		if( MediaPocsThrdPt->m_AudioOutput.m_IsSaveAudioToFile != 0 )
		{
			if( WaveFileWriterInit( &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt, 1, MediaPocsThrdPt->m_AudioOutput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器成功。", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器失败。", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
				goto Out;
			}
		}
		
        //创建并初始化音频输入波形器、音频结果波形器。
        if( MediaPocsThrdPt->m_AudioOutput.m_IsDrawAudioOscilloToWnd != 0 )
        {
            if( AudioOscilloInit( &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出波形器成功。" );
            }
            else
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出波形器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out;
            }
        }

		//创建并初始化音频输出帧链表、音频输出空闲帧链表。
		{
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal = 6; //音频输出帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输出帧，从而导致不同步。音频输出空闲帧链表最多存储总数与音频输出帧链表一致。
			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.Init( sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出空闲帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出空闲帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			//创建并初始化音频输出空闲帧，并追加到音频输出空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameElmTotal; p_TmpInt322++ )
			{
				MediaPocsThrdPt->m_PcmAudioOutputFramePt = ( int16_t * )malloc( MediaPocsThrdPt->m_AudioOutput.m_FrameLen * sizeof( int16_t ) );
				if( MediaPocsThrdPt->m_PcmAudioOutputFramePt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化音频输出空闲帧失败。" );
					goto Out; //本函数返回失败。
				}
				if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAudioOutputFramePt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioOutputFramePt );
					MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加音频输入空闲帧到音频输出空闲帧链表失败。" );
					goto Out; //本函数返回失败。
				}
				MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
			}
		}
		
		//初始化音频输出线程的临时变量。
		{
			MediaPocsThrdPt->m_AudioOutput.m_Msg = { 0 }; //初始化消息。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt = NULL; //初始化音频输出帧的指针。
			if( MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder != 0 )
			{
				MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AudioOutput.m_FrameLen * sizeof( uint8_t ) ); //初始化已编码格式音频输出帧的指针。
				if( MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建已编码格式音频输出帧的内存块失败。" );
					goto Out;
				}
				MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = MediaPocsThrdPt->m_AudioOutput.m_FrameLen * sizeof( uint8_t ); //初始化已编码格式音频输出帧的内存大小，单位字节。
			}
			else
			{
				MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt = NULL; //初始化已编码格式音频输出帧的指针。
				MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = 0; //初始化已编码格式音频输出帧的内存大小，单位字节。
			}
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen = 0; //初始化音频输出帧的长度，单位字节。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal = 0; //初始化音频输出帧链表的元数总数。
			MediaPocsThrdPt->m_AudioOutput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaPocsThrdPt->m_AudioOutput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化音频输出线程的临时变量成功。" );
		}

		//创建并启动音频输出线程。必须在初始化音频输出设备前初始化音频输出线程，因为音频输出设备会使用音频输出线程ID。
		MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioOutputThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdId );
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl != NULL )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动音频输出线程成功。" );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动音频输出线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto Out; //本函数返回失败。
		}

		//创建并初始化音频输出缓冲区块数组。
		{
			if( MediaPocsThrdPt->m_AudioOutput.m_FrameLen / ( MediaPocsThrdPt->m_AudioOutput.m_SamplingRate / 1000 ) == 10 ) //如果帧的长度为10毫秒。
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 4;
			}
			else if( MediaPocsThrdPt->m_AudioOutput.m_FrameLen / ( MediaPocsThrdPt->m_AudioOutput.m_SamplingRate / 1000 ) == 20 ) //如果帧的长度为20毫秒。
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 3;
			}
			else //如果帧的长度为大于等于30毫秒。
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 2;
			}
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal * sizeof( WAVEHDR ) );
			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输出缓冲区块数组的内存块失败。" );
				goto Out; //本函数返回失败。
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].dwBufferLength = MediaPocsThrdPt->m_AudioOutput.m_FrameLen * sizeof( int16_t );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].dwBufferLength );
				if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输出缓冲区块数组的数据内存块失败。" );
					goto Out; //本函数返回失败。
				}
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出缓冲区块成功。" );
		}

		//创建并初始化音频输出设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = MediaPocsThrdPt->m_AudioOutput.m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = MediaPocsThrdPt->m_AudioOutput.m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt321 = waveOutOpen( &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcID, &WaveFormatEx, ( DWORD_PTR )MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdId, ( DWORD_PTR )MediaPocsThrdPt, CALLBACK_THREAD );
			if( p_TmpInt321 != MMSYSERR_NOERROR )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开音频输出设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：打开音频输出设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				goto Out;
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				p_TmpInt321 = waveOutPrepareHeader( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：为音频输出设备准备音频输出缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto Out;
				}
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出设备成功。音频输出缓冲区大小：%d * %d。", MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[0].dwBufferLength, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal );
		}
	} //初始化音频输出完毕。

	//自适应设置回音的延迟。
	if( MediaPocsThrdPt->m_AudioInput.m_UseWhatAec != 0 ) //如果要使用音频输入的声学回音消除，就自适应计算声学回音的延迟，并设置到声学回音消除器。
	{
		int32_t p_Delay = ( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal + MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal ) * ( MediaPocsThrdPt->m_AudioInput.m_FrameLen / ( MediaPocsThrdPt->m_AudioInput.m_SamplingRate / 1000 ) );
		if( ( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmGetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			p_TmpInt321 = p_Delay;
			WebRtcAecmSetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt, p_TmpInt321 );
			WebRtcAecmGetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt, &p_TmpInt321 );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
		}
		if( ( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt != NULL ) && ( WebRtcAecGetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
        {
            if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt321 = p_Delay;
                WebRtcAecSetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, p_TmpInt321 );
                WebRtcAecGetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 );
            }
            else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                WebRtcAecSetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, 20 );
                WebRtcAecGetDelay( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 );
            }
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
		if( ( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
        {
			p_TmpInt321 = p_Delay;
            SpeexWebRtcAecSetWebRtcAecmDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, p_TmpInt321 );
            SpeexWebRtcAecGetWebRtcAecmDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
		if( ( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
        {
            if( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt321 = p_Delay;
                SpeexWebRtcAecSetWebRtcAecDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, p_TmpInt321 );
                SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            }
            else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                SpeexWebRtcAecSetWebRtcAecDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, 20 );
                SpeexWebRtcAecGetWebRtcAecDelay( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            }
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
	} //自适应设置回音的延迟完毕。

	//初始化视频输入。
	if( MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput != 0 )
	{
		//创建并初始化视频输入线程。必须在初始化视频输入设备前初始化视频输入线程，因为视频输入设备会使用视频输入线程的指针。
		{
			MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt = new VideoInputThrd( MediaPocsThrdPt );
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt != NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入线程成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入线程失败。" );
				goto Out;
			}
		}

		//创建并初始化视频输入设备。
		{
			int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
			ICreateDevEnum * p_CreateDevEnumPt = NULL;
			IEnumMoniker * p_EnumMonikerPt = NULL;
			IMoniker * p_MonikerPt = NULL;
			IEnumPins * p_EnumPinsPt = NULL;
			IPin * p_PinPt = NULL;
			PIN_DIRECTION p_PinDirection;
			IEnumMediaTypes * p_EnumMediaTypesPt = NULL;
			AM_MEDIA_TYPE * p_AmMediaTypePt = NULL;

			IBaseFilter * p_VideoInputDvcFilterPt = NULL; //存放视频输入设备过滤器的指针。
			IPin * p_SelPinPt = NULL;
			AM_MEDIA_TYPE * p_SelAmMediaTypePt = NULL;
			int32_t p_AvgTimePerFrame = 1000.0 / ( MediaPocsThrdPt->m_VideoInput.m_MaxSamplingRate / 10.0 / 1000.0 );
			double p_FrameWidthToHeightRatio = ( double )MediaPocsThrdPt->m_VideoInput.m_FrameWidth / ( double )MediaPocsThrdPt->m_VideoInput.m_FrameHeight; //存放指定帧的宽高比。
            double p_VideoInputDvcFrameWidthToHeightRatio = 0; //存放本次视频输入设备帧的宽高比。
            int32_t p_VideoInputDvcFrameCropWidth = 0; //存放本次视频输入设备帧的裁剪宽度。
            int32_t p_VideoInputDvcFrameCropHeight = 0; //存放本次视频输入设备帧的裁剪高度。

			IBaseFilter * p_DecFilterPt = NULL;
			IPin * p_DecFilterInputPinPt = NULL;
			IPin * p_DecFilterOutputPinPt = NULL;
			
			IBaseFilter * p_SampleGrabberFilterPt = NULL;
			ISampleGrabber * p_SampleGrabberInterfacePt = NULL;
			IPin * p_SampleGrabberFilterInputPinPt = NULL;
			IPin * p_SampleGrabberFilterOutputPinPt = NULL;
			AM_MEDIA_TYPE p_TmpAmMediaType = { 0 };

			//初始化COM库。
			CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY );

			//创建视频输入过滤器图管理器。
			if( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, ( void ** )&MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器失败。" );
				goto OutInitVideoInputDvc;
			}
			
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->QueryInterface( IID_IMediaEvent, ( LPVOID * )&MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器媒体事件器失败。" );
				goto OutInitVideoInputDvc;
			}

			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->QueryInterface( IID_IMediaControl, ( LPVOID * )&MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器媒体控制器失败。" );
				goto OutInitVideoInputDvc;
			}

			//创建系统设备枚举器。
			if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建系统设备枚举器失败。" );
				goto OutInitVideoInputDvc;
			}

			//查找并创建指定的视频输入设备过滤器。
			if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
			{
				for( p_TmpInt322 = 0; p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK; p_TmpInt322++ )
				{
					if( ( p_TmpInt322 == MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcID ) || ( ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcID == WAVE_MAPPER ) && ( p_TmpInt322 == 0 ) ) )
					{
						if( p_MonikerPt->BindToObject( NULL, NULL, IID_IBaseFilter, ( void * * )&p_VideoInputDvcFilterPt ) == S_OK )
						{
							p_TmpSz1 = p_MonikerPt->Release();
							p_MonikerPt = NULL;
							break;
						}
						else
						{
							p_TmpSz1 = p_MonikerPt->Release();
							p_MonikerPt = NULL;
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据Moniker创建视频输入设备过滤器失败。" );
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
			if( p_VideoInputDvcFilterPt == NULL ) //如果创建视频输入设备失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" );
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" );
				goto OutInitVideoInputDvc;
			}
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_VideoInputDvcFilterPt, L"Video Capture" ) != S_OK ) //如果添加视频输入设备过滤器到视频输入过滤器图失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加视频输入设备过滤器到视频输入过滤器图失败。" );
				goto OutInitVideoInputDvc;
			}

			//选择视频输入设备过滤器上合适的引脚和媒体类型。
			if( p_VideoInputDvcFilterPt->EnumPins( &p_EnumPinsPt ) == S_OK ) //创建引脚枚举器。
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
									#define VIDEOINFOHEADER_AvgTimePerFrame( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
									#define VIDEOINFOHEADER_AvgTimePerFrameCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER_AvgTimePerFrame( AmMediaType1Pt ) Com VIDEOINFOHEADER_AvgTimePerFrame( AmMediaType2Pt ) )

									#define VIDEOINFOHEADER_Width( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biWidth )
									#define VIDEOINFOHEADER_Height( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER * )AmMediaTypePt->pbFormat )->bmiHeader.biHeight )
									#define VIDEOINFOHEADER_WidthHeightCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( ( VIDEOINFOHEADER_Width( AmMediaType1Pt ) Com VIDEOINFOHEADER_Width( AmMediaType2Pt ) ) && ( VIDEOINFOHEADER_Height( AmMediaType1Pt ) Com VIDEOINFOHEADER_Height( AmMediaType2Pt ) ) )

									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备支持的媒体格式：subtype：%s  AvgTimePerFrame：%lld  %.1f  Width：%ld  Height：%ld。",
																					  ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																					  VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ) / 10.0 / 1000.0 ),
																					  VIDEOINFOHEADER_Width( p_AmMediaTypePt ), VIDEOINFOHEADER_Height( p_AmMediaTypePt ) );
									
									if( ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) || ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ) //如果媒体格式为MJPEG或YUY2才进行选择。
									{
										//设置本次视频输入设备帧的宽高比、裁剪宽度、裁剪高度。
										p_VideoInputDvcFrameWidthToHeightRatio = ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt );
										if( p_VideoInputDvcFrameWidthToHeightRatio >= p_FrameWidthToHeightRatio )
										{
											p_VideoInputDvcFrameCropWidth = ( int )( ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt ) * p_FrameWidthToHeightRatio );
											p_VideoInputDvcFrameCropWidth -= p_VideoInputDvcFrameCropWidth % 2;
											p_VideoInputDvcFrameCropHeight = VIDEOINFOHEADER_Height( p_AmMediaTypePt );
										}
										else
										{
											p_VideoInputDvcFrameCropWidth = VIDEOINFOHEADER_Width( p_AmMediaTypePt );
											p_VideoInputDvcFrameCropHeight = ( int )( ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / p_FrameWidthToHeightRatio );
											p_VideoInputDvcFrameCropHeight -= p_VideoInputDvcFrameCropHeight % 2;
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
										else if( ( ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth < MediaPocsThrdPt->m_VideoInput.m_FrameWidth ) || ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight < MediaPocsThrdPt->m_VideoInput.m_FrameHeight ) ) && ( ( p_VideoInputDvcFrameCropWidth > MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) || ( p_VideoInputDvcFrameCropHeight > MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) ) )
										{
											p_TmpInt321 = 1;
										}
										else if( ( ( p_VideoInputDvcFrameCropWidth >= MediaPocsThrdPt->m_VideoInput.m_FrameWidth ) || ( p_VideoInputDvcFrameCropHeight >= MediaPocsThrdPt->m_VideoInput.m_FrameHeight ) ) || ( ( p_VideoInputDvcFrameCropWidth == MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) && ( p_VideoInputDvcFrameCropHeight == MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) ) )
										{
											if( ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) > p_AvgTimePerFrame ) && ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
											{
												p_TmpInt321 = 1;
											}
											else if( ( VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ) <= p_AvgTimePerFrame ) || ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
											{
												if( ( ( p_VideoInputDvcFrameCropWidth < MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) || ( p_VideoInputDvcFrameCropHeight < MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) ) )
												{
													p_TmpInt321 = 1;
												}
												else if( ( ( p_VideoInputDvcFrameCropWidth == MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) && ( p_VideoInputDvcFrameCropHeight == MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) ) && ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
												{
													p_TmpInt321 = 1;
												}
											}
										}
									}
									else
									{
										if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：本次视频输入设备支持的媒体格式不是为MJPEG或YUY2，不能被选择。" );
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

										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcMaxSamplingRate = 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth = VIDEOINFOHEADER_Width( p_SelAmMediaTypePt );
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight = VIDEOINFOHEADER_Height( p_SelAmMediaTypePt );

										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth = p_VideoInputDvcFrameCropWidth;
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight = p_VideoInputDvcFrameCropHeight;
									}
									else //如果不要选择本次的。
									{
										DeleteMediaType( p_AmMediaTypePt );
										p_AmMediaTypePt = NULL;
									}
								}
								/*else if( p_AmMediaTypePt->majortype == MEDIATYPE_Video && p_AmMediaTypePt->formattype == FORMAT_VideoInfo2 ) //如果当前还没有FORMAT_VideoInfo格式的媒体类型。
								{
									#define VIDEOINFOHEADER2_AvgTimePerFrame( AmMediaTypePt ) ( ( ( VIDEOINFOHEADER2 * )AmMediaTypePt->pbFormat )->AvgTimePerFrame )
									#define VIDEOINFOHEADER2_AvgTimePerFrameCom( AmMediaType1Pt, Com, AmMediaType2Pt ) ( VIDEOINFOHEADER2_AvgTimePerFrame( AmMediaType1Pt ) Com VIDEOINFOHEADER2_AvgTimePerFrame( AmMediaType2Pt ) )

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
									else if( ( ( VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt ) < MediaPocsThrdPt->m_VideoInput.m_FrameWidth ) || ( VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt ) < MediaPocsThrdPt->m_VideoInput.m_FrameHeight ) ) && ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
									{
										p_TmpInt321 = 1;
									}
									else if( ( ( VIDEOINFOHEADER2_Width( p_AmMediaTypePt ) >= MediaPocsThrdPt->m_VideoInput.m_FrameWidth ) && ( VIDEOINFOHEADER2_Height( p_AmMediaTypePt ) >= MediaPocsThrdPt->m_VideoInput.m_FrameHeight ) ) || ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
									{
										if( ( VIDEOINFOHEADER2_AvgTimePerFrame( p_SelAmMediaTypePt ) > p_AvgTimePerFrame ) && ( VIDEOINFOHEADER2_AvgTimePerFrameCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
										{
											p_TmpInt321 = 1;
										}
										else if( ( VIDEOINFOHEADER2_AvgTimePerFrame( p_AmMediaTypePt ) <= p_AvgTimePerFrame ) || ( VIDEOINFOHEADER2_AvgTimePerFrameCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
										{
											if( ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) || ( VIDEOINFOHEADER2_AvgTimePerFrameCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
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
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcMaxSamplingRate = 1000.0 / ( VIDEOINFOHEADER2_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth = VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt );
										MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight = VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt );
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
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" );
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" );
				goto OutInitVideoInputDvc;
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备选择的媒体格式：subtype：%s  AvgTimePerFrame：%lld  %.1f  Width：%ld  Height：%ld。",
															  ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
															  VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 ),
															  VIDEOINFOHEADER_Width( p_SelAmMediaTypePt ), VIDEOINFOHEADER_Height( p_SelAmMediaTypePt ) );
			
            //判断视频输入设备帧是否裁剪。
            if( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth > MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) //如果视频输入设备帧的宽度比裁剪宽度大，就表示需要裁剪宽度。
            {
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop = 1; //设置视频输入设备帧要裁剪。

                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX = ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth - MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth ) / 2; //设置视频输入设备帧裁剪区域左上角的横坐标，使裁剪区域居中。
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
            }
            else if( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight > MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) //如果视频输入设备帧的高度比裁剪高度大，就表示需要裁剪高度。
            {
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop = 1; //设置视频输入设备帧要裁剪。

                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY = ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight - MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight ) / 2; //设置视频输入设备帧裁剪区域左上角的纵坐标，使裁剪区域居中。
            }
            else //如果视频输入设备帧的宽度和高度与裁剪宽度和高度一致，就表示不需要裁剪。
            {
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop = 0; //设置视频输入设备帧不裁剪。

                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
            }
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备帧是否裁剪：%" PRIi32 "  左上角的横坐标：%" PRIi32 "  纵坐标：%" PRIi32 "  裁剪区域的宽度：%" PRIi32 "  高度：%" PRIi32 "。", MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop, MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX, MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY, MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth, MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight );

            //判断视频输入设备帧是否缩放。
            if( ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth != MediaPocsThrdPt->m_VideoInput.m_FrameWidth ) || ( MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight != MediaPocsThrdPt->m_VideoInput.m_FrameHeight ) )
            {
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsScale = 1; //设置视频输入设备帧要缩放。
            }
            else
            {
                MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsScale = 0; //设置视频输入设备帧不缩放。
            }
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleWidth = MediaPocsThrdPt->m_VideoInput.m_FrameWidth; //设置视频输入设备帧缩放后的宽度。
            MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameScaleHeight = MediaPocsThrdPt->m_VideoInput.m_FrameHeight; //设置视频输入设备帧缩放后的高度。
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备帧是否缩放：%" PRIi32 "。", MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsScale );

			//创建视频输入设备过滤器上选择的引脚和媒体类型对应的解码过滤器，并连接引脚。
			if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG )
			{
				if( CoCreateInstance( CLSID_MjpegDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建MJPG解码过滤器失败。" );
					goto OutInitVideoInputDvc;
				}
			}
			else if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 )
			{
				if( CoCreateInstance( CLSID_AVIDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建AVI解码过滤器失败。" );
					goto OutInitVideoInputDvc;
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
							p_DecFilterInputPinPt = p_PinPt;
						}
						else if( p_PinDirection == PINDIR_OUTPUT )
						{
							p_DecFilterOutputPinPt = p_PinPt;
						}
					}
					p_PinPt = NULL;
				}
				p_TmpSz1 = p_EnumPinsPt->Release();
				p_EnumPinsPt = NULL;
			}
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_DecFilterPt, L"Decompressor" ) != S_OK ) //如果添加解码过滤器到视频输入过滤器图失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加解码过滤器到视频输入过滤器图失败。" );
				goto OutInitVideoInputDvc;
			}
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->ConnectDirect( p_SelPinPt, p_DecFilterInputPinPt, p_SelAmMediaTypePt ) != S_OK ) //如果连接引脚失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。" );
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。原因：可能是视频输入设备无法正常工作。" );
				goto OutInitVideoInputDvc;
			}

			//创建采样抓取过滤器，并连接引脚。
			if( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void * * )&p_SampleGrabberFilterPt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建采样抓取过滤器失败。" );
				goto OutInitVideoInputDvc;
			}
			{
				p_SampleGrabberFilterPt->EnumPins( &p_EnumPinsPt );
				while( p_EnumPinsPt->Next( 1, &p_PinPt, NULL ) == S_OK )
				{
					if( p_PinPt->QueryDirection( &p_PinDirection ) == S_OK )
					{
						if( p_PinDirection == PINDIR_INPUT )
						{
							p_SampleGrabberFilterInputPinPt = p_PinPt;
						}
						else if( p_PinDirection == PINDIR_OUTPUT )
						{
							p_SampleGrabberFilterOutputPinPt = p_PinPt;
						}
					}
					p_PinPt = NULL;
				}
				p_TmpSz1 = p_EnumPinsPt->Release();
				p_EnumPinsPt = NULL;
			}
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_SampleGrabberFilterPt, L"Sample Grabber" ) != S_OK ) //如果采样抓取过滤器到视频输入过滤器图失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加采样抓取过滤器到视频输入过滤器图失败。" );
				goto OutInitVideoInputDvc;
			}
			p_TmpAmMediaType.majortype = MEDIATYPE_Video;
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB555; //PICTR_FMT_SRGBF8_BGR555。
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB565; //PICTR_FMT_SRGBF8_BGR565。
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB24; //PICTR_FMT_SRGBF8_BGR888。
			p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB32; //PICTR_FMT_SRGBF8_BGRA8888。
			if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->ConnectDirect( p_DecFilterOutputPinPt, p_SampleGrabberFilterInputPinPt, &p_TmpAmMediaType ) != S_OK ) //如果连接引脚失败。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：连接解码过滤器与采样抓取过滤器的引脚失败。" );
				goto OutInitVideoInputDvc;
			}

			//设置采样抓取过滤器的回调函数。
			if( p_SampleGrabberFilterPt->QueryInterface( IID_ISampleGrabber, ( void * * )&p_SampleGrabberInterfacePt ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：获取采样抓取过滤器的回调接口失败。" );
				goto OutInitVideoInputDvc;
			}
			if( p_SampleGrabberInterfacePt->SetCallback( MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt, 1 ) != S_OK )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：设置采样抓取过滤器的回调函数失败。" );
				goto OutInitVideoInputDvc;
			}

			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入设备成功。" );

			p_Result = 0; //设置本函数执行成功。

			OutInitVideoInputDvc:
			if( p_VideoInputDvcFilterPt != NULL )
			{
				p_TmpSz1 = p_VideoInputDvcFilterPt->Release();
				p_VideoInputDvcFilterPt = NULL;
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
			if( p_DecFilterInputPinPt != NULL )
			{
				p_TmpSz1 = p_DecFilterInputPinPt->Release();
				p_DecFilterInputPinPt = NULL;
			}
			if( p_DecFilterOutputPinPt != NULL )
			{
				p_TmpSz1 = p_DecFilterOutputPinPt->Release();
				p_DecFilterOutputPinPt = NULL;
			}
			if( p_SampleGrabberFilterPt != NULL )
			{
				p_TmpSz1 = p_SampleGrabberFilterPt->Release();
				p_SampleGrabberFilterPt = NULL;
			}
			if( p_SampleGrabberInterfacePt != NULL )
			{
				p_TmpSz1 = p_SampleGrabberInterfacePt->Release();
				p_SampleGrabberInterfacePt = NULL;
			}
			if( p_SampleGrabberFilterInputPinPt != NULL )
			{
				p_TmpSz1 = p_SampleGrabberFilterInputPinPt->Release();
				p_SampleGrabberFilterInputPinPt = NULL;
			}
			if( p_SampleGrabberFilterOutputPinPt != NULL )
			{
				p_TmpSz1 = p_SampleGrabberFilterOutputPinPt->Release();
				p_SampleGrabberFilterOutputPinPt = NULL;
			}
			if( p_Result != 0 ) //如果本函数执行失败。
			{
				goto Out;
			}
		} //创建并初始化视频输入设备完毕。
		
		//初始化视频输入线程的临时变量。
		{
			MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec = 0; //初始化上一个视频输入设备帧的时间。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameTimeStepMsec = 1000 / MediaPocsThrdPt->m_VideoInput.m_MaxSamplingRate; //初始化视频输入帧的时间步进。
			if( MediaPocsThrdPt->m_VideoInput.m_FrameWidth * MediaPocsThrdPt->m_VideoInput.m_FrameHeight >= MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth * MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight ) //如果视频输入帧指定的尺寸大于等于视频输入设备帧的尺寸。
			{
				MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz = MediaPocsThrdPt->m_VideoInput.m_FrameWidth * MediaPocsThrdPt->m_VideoInput.m_FrameHeight * 4; //初始化视频输入结果帧的内存大小。
			}
			else //如果视频输入帧指定的尺寸小于视频输入设备帧的尺寸。
			{
				MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz = MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth * MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight * 4; //初始化视频输入结果帧的内存大小。
			}
			MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz ); //初始化视频输入结果帧的指针。
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频结果帧的内存块失败。" );
				goto Out;
			}
			MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz ); //初始化视频输入临时帧的指针。
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频临时帧的内存块失败。" );
				goto Out;
			}
			MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen = 0; //初始化视频输入结果帧的长度。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt = NULL; //初始化视频输入交换帧的指针。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt = NULL; //初始化视频输入帧元素的指针。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal = 0; //初始化视频输入帧链表的元数总数。
			MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaPocsThrdPt->m_VideoInput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化视频输入线程的临时变量成功。" );
		}

		//初始化编码器。
        switch( MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( OpenH264EncoderInit( &MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderPt, MediaPocsThrdPt->m_VideoInput.m_FrameWidth, MediaPocsThrdPt->m_VideoInput.m_FrameHeight, MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderVideoType, MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderEncodedBitrate, MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderBitrateControlMode, MediaPocsThrdPt->m_VideoInput.m_MaxSamplingRate, MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderIDRFrameIntvl, MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderCmplxt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：创建并初始化OpenH264编码器成功。" );
                }
                else
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化OpenH264编码器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
                    goto Out;
                }
                break;
            }
        }

		//创建并初始化视频输入帧链表、视频输入空闲帧链表。
		{
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal = 2; //视频输入帧链表最多只存储2帧，避免因为视频设备有变化导致卡顿并积累大量视频输入帧，从而导致不及时。视频输入空闲帧链表最多存储总数与视频输入帧链表一致。
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.Init( sizeof( MediaPocsThrd::VideoInput::VideoInputFrameElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.Init( sizeof( MediaPocsThrd::VideoInput::VideoInputFrameElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入空闲帧链表成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入空闲帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				goto Out; //本函数返回失败。
			}

			//创建并初始化视频输入空闲帧，并追加到视频输入空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmTotal; p_TmpInt322++ )
			{
				MediaPocsThrdPt->m_VideoInputFramePt = ( MediaPocsThrd::VideoInput::VideoInputFrameElm * )malloc( sizeof( MediaPocsThrd::VideoInput::VideoInputFrameElm ) );
				if( MediaPocsThrdPt->m_VideoInputFramePt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
					goto Out; //本函数返回失败。
				}
				MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VideoInput.m_FrameWidth * MediaPocsThrdPt->m_VideoInput.m_FrameHeight * 3 / 2 );
				if( MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt == NULL )
				{
					free( MediaPocsThrdPt->m_VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt = NULL;
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
					goto Out; //本函数返回失败。
				}
				if( MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder != 0 )
				{
					MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_VideoInput.m_FrameWidth * MediaPocsThrdPt->m_VideoInput.m_FrameHeight * 3 / 2 );
					if( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt == NULL )
					{
						free( MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
						MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
						free( MediaPocsThrdPt->m_VideoInputFramePt );
						MediaPocsThrdPt->m_VideoInputFramePt = NULL;
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
						goto Out; //本函数返回失败。
					}
					MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen = 0;
				}
				else
				{
					MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen = 0;
				}
				if( MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_VideoInputFramePt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaPocsThrdPt->m_VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt = NULL;
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加视频输入空闲帧到视频输入空闲帧链表失败。" );
					goto Out; //本函数返回失败。
				}
			}
		}
	} //初始化视频输入完毕。

    //初始化视频输出。
    if( MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput != 0 )
    {
        //初始化解码器。
        switch( MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264解码器。
            {
                if( OpenH264DecoderInit( &MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderPt, MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderDecodeThrdNum, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：创建并初始化OpenH264解码器成功。" );
                }
                else
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化OpenH264解码器失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
                    goto Out;
                }
                break;
            }
        }

		//初始化视频输出线程的临时变量。
		{
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出结果帧的指针。
			if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输出结果帧的内存块失败。" );
				goto Out;
			}
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出临时帧的指针。
			if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输出临时帧的内存块失败。" );
				goto Out;
			}
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt = NULL; //初始化视频输出交换帧的指针。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz = 960 * 1280 * 4 * 3; //初始化视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen = 0; //初始化视频输出结果帧的长度。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth = 0; //初始化视频输出帧的宽度。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight = 0; //初始化视频输出帧的高度。
			MediaPocsThrdPt->m_VideoOutput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaPocsThrdPt->m_VideoOutput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化视频输出线程的临时变量成功。" );
		}

		//创建并启动音频输出线程。
		MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdExitFlag = 0; //设置视频输出线程退出标记为0表示保持运行。
		MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )VideoOutputThrdRun, MediaPocsThrdPt, 0, &MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdId );
		if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl != NULL )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动视频输出线程成功。" );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动视频输出线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto Out; //本函数返回失败。
		}
    } //初始化视频输出完毕。
	
	//初始化媒体处理线程的临时变量。
	{
		MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL; //初始化PCM格式音频输入帧的指针。
		MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL; //初始化PCM格式音频输出帧的指针。
		if( MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput != 0 ) //初始化PCM格式音频结果帧的指针。
		{
			MediaPocsThrdPt->m_PcmAudioResultFramePt = ( int16_t * )malloc( sizeof( int16_t ) * MediaPocsThrdPt->m_AudioInput.m_FrameLen );
			if( MediaPocsThrdPt->m_PcmAudioResultFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建PCM格式音频结果帧的内存块失败。" );
				goto Out;
			}
		}
		else
		{
			MediaPocsThrdPt->m_PcmAudioResultFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput != 0 ) //初始化PCM格式音频临时帧的指针。
		{
			MediaPocsThrdPt->m_PcmAudioTmpFramePt = ( int16_t * )malloc( sizeof( int16_t ) * MediaPocsThrdPt->m_AudioInput.m_FrameLen );
			if( MediaPocsThrdPt->m_PcmAudioTmpFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建PCM格式音频临时帧的内存块失败。" );
				goto Out;
			}
		}
		else
		{
			MediaPocsThrdPt->m_PcmAudioTmpFramePt = NULL;
		}
		MediaPocsThrdPt->m_PcmAudioSwapFramePt = NULL; //初始化PCM格式音频交换帧的指针。
		MediaPocsThrdPt->m_VoiceActSts = 1; //初始化语音活动状态，预设为1是为了让在没有使用语音活动检测的情况下永远都有语音活动。
		if( ( MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput != 0 ) && ( MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder != 0 ) ) //初始化已编码格式音频输入帧的指针和内存大小。
		{
			MediaPocsThrdPt->m_EncoderAudioInputFramePt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( uint8_t ) );
			if( MediaPocsThrdPt->m_EncoderAudioInputFramePt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建已编码格式音频输入帧的内存块失败。" );
				goto Out;
			}
			MediaPocsThrdPt->m_EncoderAudioInputFrameSz = MediaPocsThrdPt->m_AudioInput.m_FrameLen * sizeof( uint8_t );
		}
		else
		{
			MediaPocsThrdPt->m_EncoderAudioInputFramePt = NULL;
			MediaPocsThrdPt->m_EncoderAudioInputFrameSz = 0;
		}
		MediaPocsThrdPt->m_EncoderAudioInputFrameLen = 0; //初始化已编码格式音频输入帧的长度，单位字节。
		MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans = 1; //初始化已编码格式音频输入帧是否需要传输预设为1，为了让在不使用非连续传输的情况下永远都是需要传输。
		MediaPocsThrdPt->m_VideoInputFramePt = NULL; //初始化视频输入帧的指针。
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：初始化媒体处理线程的临时变量成功。" );
	} //初始化媒体处理线程的临时变量完毕。

	if( MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput != 0 ) //如果要使用音频输入。
	{
		waveInStart( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl ); //让音频输入设备立即开始录音。
	}
	if( MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) //如果要使用音频输出。
	{
		for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ ) //让音频输出设备立即开始播放。
		{
			waveOutWrite( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322] ) );
		}
	}
	if( MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput != 0 ) //如果要使用视频输入。
	{
		MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Run(); //让视频输入过滤器图管理器开始运行。
	}
	
	if( MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec );
		LOGFI( "媒体处理线程：媒体处理线程初始化完毕，耗时：%" PRIu64 " 毫秒，正式开始处理帧。", p_NowMsec - p_LastMsec );
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
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的处理函数成功。返回值：%d", p_TmpInt321 );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：调用用户定义的处理函数失败。返回值：%d", p_TmpInt321 );
			goto Out;
		}
		
        if( MediaPocsThrdPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：调用用户定义的处理函数完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }

		//取出音频输入帧和音频输出帧。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取音频输入帧链表的元素总数。
		else p_TmpSz1 = 0;
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.GetTotal( &p_TmpSz2, 1, NULL ); //获取音频输出帧链表的元素总数。
		else p_TmpSz2 = 0;
		if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) ) //如果音频输入帧链表和音频输出帧链表中都有帧了，就开始取出。
		{
			//从音频输入帧链表中取出第一个音频输入帧。
			{
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Lock( NULL ); //音频输入帧链表的互斥锁加锁。
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioInputFramePt, NULL, 0, NULL );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.DelHead( 0, NULL );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Unlock( NULL ); //音频输入帧链表的互斥锁解锁。
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%" PRIi32 "。", p_TmpSz1 );

			//从音频输出帧链表中取出第一个音频输出帧。
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Lock( NULL ); //音频输出帧链表的互斥锁加锁。
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioOutputFramePt, NULL, 0, NULL );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.DelHead( 0, NULL );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Unlock( NULL ); //音频输出帧链表的互斥锁解锁。
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%" PRIi32 "。", p_TmpSz2 );

			//将音频输入帧复制到音频结果帧，方便处理。
			memcpy( MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioInputFramePt, sizeof( int16_t ) * MediaPocsThrdPt->m_AudioInput.m_FrameLen );
		}
		else if( ( p_TmpSz1 > 0 ) && ( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.m_ConstLenLnkLstPt == NULL ) ) //如果音频输入帧链表有帧了，且不使用音频输出帧链表，就开始取出。
		{
			//从音频输入帧链表中取出第一个音频输入帧。
			{
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Lock( NULL ); //音频输入帧链表的互斥锁加锁。
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.GetHead( NULL, MediaPocsThrdPt->m_PcmAudioInputFramePt, NULL, 0, NULL );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.DelHead( 0, NULL );
				MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Unlock( NULL ); //音频输入帧链表的互斥锁解锁。
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%" PRIi32 "。", p_TmpSz1 );

			//将音频输入帧复制到音频结果帧，方便处理。
			memcpy( MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioInputFramePt, sizeof( int16_t ) * MediaPocsThrdPt->m_AudioInput.m_FrameLen );
		}
		else if( ( p_TmpSz2 > 0 ) && ( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.m_ConstLenLnkLstPt == NULL ) ) //如果音频输出帧链表有帧了，且不使用音频输入帧链表，就开始取出。
		{
			//从音频输出帧链表中取出第一个音频输出帧。
			{
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Lock( NULL ); //音频输出帧链表的互斥锁加锁。
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioOutputFramePt, NULL, 0, NULL );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.DelHead( 0, NULL );
				MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Unlock( NULL ); //音频输出帧链表的互斥锁解锁。
			}
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%" PRIi32 "。", p_TmpSz2 );
		}
		
        //处理音频输入帧。
		if( MediaPocsThrdPt->m_PcmAudioInputFramePt != NULL )
		{
			//使用声学回音消除器。
			switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatAec )
			{
				case 0: //如果不使用声学回音消除器。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用声学回音消除器。" );
					break;
				}
				case 1: //如果使用Speex声学回音消除器。
				{
					if( ( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt != NULL ) && ( SpeexAecPocs( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex声学回音消除器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex声学回音消除器失败。" );
					}
					break;
				}
				case 2: //如果使用WebRtc定点版声学回音消除器。
				{
					if( ( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmPocs( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc定点版声学回音消除器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc定点版声学回音消除器失败。" );
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版声学回音消除器。
				{
					if( ( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt != NULL ) && ( WebRtcAecPocs( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc浮点版声学回音消除器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc浮点版声学回音消除器失败。" );
					}
					break;
				}
				case 4: //如果使用SpeexWebRtc三重声学回音消除器。
				{
					if( ( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecPocs( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器失败。" );
					}
					break;
				}
			}

			//使用噪音抑制器。
			switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs )
			{
				case 0: //如果不使用噪音抑制器。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用噪音抑制器。" );
					break;
				}
				case 1: //如果使用Speex预处理器的噪音抑制。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：稍后在使用Speex预处理器时一起使用噪音抑制。" );
					break;
				}
				case 2: //如果使用WebRtc定点版噪音抑制器。
				{
					if( WebRtcNsxPocs( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc定点版噪音抑制器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc定点版噪音抑制器失败。" );
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版噪音抑制器。
				{
					if( WebRtcNsPocs( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc浮点版噪音抑制器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc浮点版噪音抑制器失败。" );
					}
					break;
				}
				case 4: //如果使用RNNoise噪音抑制器。
				{
					if( RNNoisePocs( MediaPocsThrdPt->m_AudioInput.m_RNNoisePt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用RNNoise噪音抑制器成功。" );
						MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用RNNoise噪音抑制器失败。" );
					}
					break;
				}
			}

			//使用Speex预处理器。
			if( ( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs == 1 ) || ( MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther != 0 ) )
			{
				if( SpeexPrpocsPocs( MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_PcmAudioTmpFramePt, &MediaPocsThrdPt->m_VoiceActSts ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex预处理器成功。语音活动状态：%d", MediaPocsThrdPt->m_VoiceActSts );
					MediaPocsThrdPt->m_PcmAudioSwapFramePt = MediaPocsThrdPt->m_PcmAudioResultFramePt; MediaPocsThrdPt->m_PcmAudioResultFramePt = MediaPocsThrdPt->m_PcmAudioTmpFramePt; MediaPocsThrdPt->m_PcmAudioTmpFramePt = MediaPocsThrdPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex预处理器失败。" );
				}
			}

			//判断音频输入是否静音。在音频输入处理完后再设置静音，这样可以保证音频输入处理器的连续性。
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputIsMute != 0 )
			{
				memset( MediaPocsThrdPt->m_PcmAudioResultFramePt, 0, sizeof( int16_t ) * MediaPocsThrdPt->m_AudioInput.m_FrameLen );
				if( ( MediaPocsThrdPt->m_AudioInput.m_IsUseSpeexPrpocsOther != 0 ) && ( MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsIsUseVad != 0 ) ) //如果Speex预处理器要使用其他功能，且要使用语音活动检测。
				{
					MediaPocsThrdPt->m_VoiceActSts = 0;
				}
			}

			//使用编码器。
			switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder )
			{
				case 0: //如果使用PCM原始数据。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
					break;
				}
				case 1: //如果使用Speex编码器。
				{
					if( SpeexEncoderPocs( MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_EncoderAudioInputFramePt, MediaPocsThrdPt->m_EncoderAudioInputFrameSz, &MediaPocsThrdPt->m_EncoderAudioInputFrameLen, &MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex编码器成功。Speex格式输入帧的长度：%" PRIuPTR "，Speex格式输入帧是否需要传输：%" PRIi32, MediaPocsThrdPt->m_EncoderAudioInputFrameLen, MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex编码器失败。" );
					}
					break;
				}
				case 2: //如果使用Opus编码器。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus编码器。" );
					goto Out;
				}
			}

			//使用音频输入Wave文件写入器写入输入帧数据、音频结果Wave文件写入器写入结果帧数据。
			if( MediaPocsThrdPt->m_AudioInput.m_IsSaveAudioToFile != 0 )
			{
				if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAudioInputFramePt, MediaPocsThrdPt->m_AudioInput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据失败。" );
				}
				if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AudioInput.m_AudioResultWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_AudioInput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据失败。" );
				}
			}

            //使用音频输入波形器绘制音频输入波形到窗口、音频结果波形器绘制音频结果波形到窗口。
            if( MediaPocsThrdPt->m_AudioInput.m_IsDrawAudioOscilloToWnd != 0 )
            {
                if( AudioOscilloDrawToWnd( MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt, MediaPocsThrdPt->m_PcmAudioInputFramePt, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloWndHdl, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输入波形器绘制音频输入波形到窗口成功。" );
                }
                else
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输入波形器绘制音频输入波形到窗口失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
                }
                if( AudioOscilloDrawToWnd( MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloPt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloWndHdl, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频结果波形器绘制音频结果波形到窗口成功。" );
                }
                else
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频结果波形器绘制音频结果波形到窗口失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
                }
            }
		}
		
		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：处理音频输入帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }
		
        //处理音频输出帧。
		if( MediaPocsThrdPt->m_PcmAudioOutputFramePt != NULL )
		{
			//使用音频输出Wave文件写入器写入输出帧数据。
			if( MediaPocsThrdPt->m_AudioOutput.m_IsSaveAudioToFile != 0 )
			{
				if( WaveFileWriterWriteData( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据失败。" );
				}
			}

            //使用音频输出波形器绘制音频输出波形到窗口。
            if( MediaPocsThrdPt->m_AudioOutput.m_IsDrawAudioOscilloToWnd != 0 )
            {
                if( AudioOscilloDrawToWnd( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloPt, MediaPocsThrdPt->m_PcmAudioOutputFramePt, MediaPocsThrdPt->m_AudioOutput.m_FrameLen, MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloWndHdl, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输出波形器绘制音频输出波形到窗口成功。" );
                }
                else
                {
                    if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输出波形器绘制音频输出波形到窗口失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
                }
            }
		}
		
		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：处理音频输出帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }
		
        //处理视频输入帧。
		if( MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.m_ConstLenLnkLstPt != NULL )
		{
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输入帧链表的元素总数。
		}
		else p_TmpSz1 = 0;
        if( ( p_TmpSz1 > 0 ) && //如果视频输入帧链表中有帧了。
            ( ( MediaPocsThrdPt->m_PcmAudioInputFramePt != NULL ) || ( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.m_ConstLenLnkLstPt == NULL ) ) ) //且已经处理了音频输入帧或不使用音频输入帧链表。
        {
            //从视频输入帧链表中取出第一个视频输入帧。
            {
				MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.Lock( NULL ); //视频输入帧链表的互斥锁加锁。
				MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VideoInputFramePt, NULL, 0, NULL );
				MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.DelHead( 0, NULL );
				MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.Unlock( NULL ); //视频输入帧链表的互斥锁解锁。
            }
            if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从视频输入帧链表中取出第一个视频输入帧，视频输入帧链表元素个数：%" PRIuPTR "。", p_TmpSz1 );
        }

        if( MediaPocsThrdPt->m_IsPrintLog != 0 )
        {
            FuncGetTimeAsMsec( &p_NowMsec );
			LOGFI( "媒体处理线程：处理视频输入帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            FuncGetTimeAsMsec( &p_LastMsec );
        }

		//调用用户定义的读取音视频输入帧函数。
        if( ( MediaPocsThrdPt->m_PcmAudioInputFramePt != NULL ) || ( MediaPocsThrdPt->m_VideoInputFramePt != NULL ) ) //如果取出了音频输入帧或视频输入帧。
        {
			if( MediaPocsThrdPt->m_VideoInputFramePt != NULL ) //如果取出了视频输入帧。
                 MediaPocsThrdPt->m_UserReadAudioVideoInputFrameFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_PcmAudioInputFramePt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_VoiceActSts, MediaPocsThrdPt->m_EncoderAudioInputFramePt, MediaPocsThrdPt->m_EncoderAudioInputFrameLen, MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans, MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt, MediaPocsThrdPt->m_VideoInput.m_FrameWidth, MediaPocsThrdPt->m_VideoInput.m_FrameHeight, MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt, MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen );
            else
                MediaPocsThrdPt->m_UserReadAudioVideoInputFrameFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_PcmAudioInputFramePt, MediaPocsThrdPt->m_PcmAudioResultFramePt, MediaPocsThrdPt->m_VoiceActSts, MediaPocsThrdPt->m_EncoderAudioInputFramePt, MediaPocsThrdPt->m_EncoderAudioInputFrameLen, MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans, NULL, NULL, NULL, NULL, NULL );
		}
		
		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &p_NowMsec );
			LOGFI( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时：%" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
			p_LastMsec = p_NowMsec;
		}
		
		if( MediaPocsThrdPt->m_PcmAudioInputFramePt != NULL ) //如果取出了PCM格式音频输入帧，就追加到音频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAudioInputFramePt, 1, NULL );
			MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAudioOutputFramePt != NULL ) //如果取出了PCM格式音频输出帧，就追加到音频输出空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAudioOutputFramePt, 1, NULL );
			MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_VideoInputFramePt != NULL ) //如果取出了视频输入帧，就追加到视频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.PutTail( &MediaPocsThrdPt->m_VideoInputFramePt, 1, NULL );
			MediaPocsThrdPt->m_VideoInputFramePt = NULL;
		}

		//如果音频输入线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl != NULL )
		{
			GetExitCodeThread( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl, ( LPDWORD )&p_TmpInt321 );
			if( p_TmpInt321 != STILL_ACTIVE )
			{
				LOGFI( "媒体处理线程：音频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaPocsThrdPt->m_ExitFlag = 3;
			}
		}

		//如果音频输出线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl != NULL )
		{
			GetExitCodeThread( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl, ( LPDWORD )&p_TmpInt321 );
			if( p_TmpInt321 != STILL_ACTIVE )
			{
				LOGFI( "媒体处理线程：音频输出线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaPocsThrdPt->m_ExitFlag = 3;
			}
		}
		
		//如果视频输入线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt != NULL )
		{
			long p_EventCode;
			LONG_PTR p_EventParam1, p_EventParam2;
			MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
			if( p_EventCode == EC_DEVICE_LOST )
			{
				LOGFI( "媒体处理线程：视频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaPocsThrdPt->m_ExitFlag = 3;
			}
		}

		if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程退出标记为请求退出。
		{
			MediaPocsThrdPt->m_ExitCode = 0; //处理已经成功了，再将本线程退出代码设置为正常退出。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：接收到退出请求，开始准备退出。退出标记：%d。", MediaPocsThrdPt->m_ExitFlag );
			goto Out;
		}

		Sleep( 1 ); //暂停一下，避免CPU使用率过高。
	} //音视频输入输出帧处理循环完毕。

	Out:
	MediaPocsThrdPt->m_RunFlag = RUN_FLAG_DSTOY; //设置本线程运行标记为跳出循环处理帧正在销毁。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程开始退出。" );
	
	//请求音视频输入输出线程退出。必须在销毁音视频输入输出前退出，因为音视频输入输出线程会使用音视频输入输出相关变量。
	if( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl != NULL ) while( PostThreadMessage( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdId, WM_QUIT, NULL, NULL ) == 0 ) Sleep( 0 ); //循环发送请求音频输入线程退出，直到线程消息发送成功，因为线程刚启动时还来不及创建消息队列从而导致发送失败。不要使用退出标记，因为音频输入设备可能会打开失败，导致音频输入线程不会进入消息循环。
	if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl != NULL ) while( PostThreadMessage( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdId, WM_QUIT, NULL, NULL ) == 0 ) Sleep( 0 ); //循环发送请求音频输出线程退出，直到线程消息发送成功，因为线程刚启动时还来不及创建消息队列从而导致发送失败。不要使用退出标记，因为音频输出设备可能会打开失败，导致音频输出线程不会进入消息循环。
	if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl != NULL ) MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdExitFlag = 1; //请求视频输出线程退出。

	//销毁音频输入。
    {
		//销毁音频输入线程。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl != NULL )
		{
			WaitForSingleObject( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl, INFINITE ); //等待音频输入线程退出。
			CloseHandle( MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl );
			MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdHdl = NULL;
			MediaPocsThrdPt->m_AudioInput.m_AudioInputThrdId = NULL;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入线程成功。" );
		}
		
		//销毁音频输入线程线程的临时变量。
		{
			MediaPocsThrdPt->m_AudioInput.m_Msg = { 0 }; //销毁消息。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputFramePt = NULL; //销毁音频输入帧的指针。
			MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal = 0; //销毁音频输入帧链表的元数总数。
			MediaPocsThrdPt->m_AudioInput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaPocsThrdPt->m_AudioInput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁音频输入线程线程的临时变量成功。" );
		}

		//销毁音频输入帧链表、音频输入空闲帧链表。
		{
			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioInputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioInputFramePt );
					MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
					MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_AudioInput.m_AudioInputFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}

			if( MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioInputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioInputFramePt );
					MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
					MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_AudioInput.m_AudioInputIdleFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入空闲帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入空闲帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}
		}

		//销毁音频输入设备。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				waveInUnprepareHeader( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl, &MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
			}
			waveInReset( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl );
			waveInClose( MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl );
			MediaPocsThrdPt->m_AudioInput.m_AudioInputDvcHdl = NULL;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入设备成功。" );
		}
		
		//销毁音频输入缓冲区块数组。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				if( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData != NULL )
				{
					free( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData );
					MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData = NULL;
				}
			}
			free( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt );
			MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrPt = NULL;
			MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveHdrTotal = 0;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入缓冲区块数组成功。" );
		}
		
		//销毁音频输入波形器、音频结果波形器。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt != NULL )
		{
			if( AudioOscilloDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入波形器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入波形器失败。" );
			}
			MediaPocsThrdPt->m_AudioInput.m_AudioInputOscilloPt = NULL;
		}
		if( MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloPt != NULL )
		{
			if( AudioOscilloDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频结果波形器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频结果波形器失败。" );
			}
			MediaPocsThrdPt->m_AudioInput.m_AudioResultOscilloPt = NULL;
		}
		
		//销毁音频输入Wave文件写入器、音频结果Wave文件写入器。
		if( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveFileWriterPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入Wave文件写入器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入Wave文件写入器失败。" );
			}
			MediaPocsThrdPt->m_AudioInput.m_AudioInputWaveFileWriterPt = NULL;
		}
		if( MediaPocsThrdPt->m_AudioInput.m_AudioResultWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AudioInput.m_AudioResultWaveFileWriterPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频结果Wave文件写入器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频结果Wave文件写入器失败。" );
			}
			MediaPocsThrdPt->m_AudioInput.m_AudioResultWaveFileWriterPt = NULL;
		}
		
		//销毁编码器。
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatEncoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				break;
			}
			case 1: //如果使用Speex编码器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPt != NULL )
				{
					if( SpeexEncoderDstoy( MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex编码器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex编码器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_SpeexEncoderPt = NULL;
				}
				break;
			}
			case 2: //如果使用Opus编码器。
			{
				break;
			}
		}
		
		//销毁Speex预处理器。
		if( MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsPt != NULL )
		{
			if( SpeexPrpocsDstoy( MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex预处理器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex预处理器失败。" );
			}
			MediaPocsThrdPt->m_AudioInput.m_SpeexPrpocsPt = NULL;
		}
		
		//销毁噪音抑制器。
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatNs )
		{
			case 0: //如果不使用噪音抑制器。
			{
				break;
			}
			case 1: //如果使用Speex预处理器的噪音抑制。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：之前在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" );
				break;
			}
			case 2: //如果使用WebRtc定点版噪音抑制器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPt != NULL )
				{
					if( WebRtcNsxDstoy( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc定点版噪音抑制器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc定点版噪音抑制器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_WebRtcNsxPt = NULL;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版噪音抑制器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPt != NULL )
				{
					if( WebRtcNsDstoy( MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc浮点版噪音抑制器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc浮点版噪音抑制器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_WebRtcNsPt = NULL;
				}
				break;
			}
			case 4: //如果使用RNNoise噪音抑制器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_RNNoisePt != NULL )
				{
					if( RNNoiseDstoy( MediaPocsThrdPt->m_AudioInput.m_RNNoisePt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁RNNoise噪音抑制器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁RNNoise噪音抑制器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_RNNoisePt = NULL;
				}
				break;
			}
		}
		
		//销毁声学回音消除器。
		switch( MediaPocsThrdPt->m_AudioInput.m_UseWhatAec )
		{
			case 0: //如果不使用声学回音消除器。
			{
				break;
			}
			case 1: //如果使用Speex声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt != NULL )
				{
					if( MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
					{
						if( SpeexAecSaveMemFile( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecFilterLen, MediaPocsThrdPt->m_AudioInput.m_SpeexAecIsUseRec, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoMutp, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoCntu, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupes, MediaPocsThrdPt->m_AudioInput.m_SpeexAecEchoSupesAct, MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 成功。", MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", MediaPocsThrdPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( SpeexAecDstoy( MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex声学回音消除器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_SpeexAecPt = NULL;
				}
				break;
			}
			case 2: //如果使用WebRtc定点版声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt != NULL )
				{
					if( WebRtcAecmDstoy( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc定点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc定点版声学回音消除器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_WebRtcAecmPt = NULL;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt != NULL )
				{
					if( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
					{
						if( WebRtcAecSaveMemFile( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt, MediaPocsThrdPt->m_AudioInput.m_SamplingRate, MediaPocsThrdPt->m_AudioInput.m_FrameLen, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecEchoMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecDelay, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay, MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 成功。", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", MediaPocsThrdPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( WebRtcAecDstoy( MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc浮点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc浮点版声学回音消除器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_WebRtcAecPt = NULL;
				}
				break;
			}
			case 4: //如果使用SpeexWebRtc三重声学回音消除器。
			{
				if( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL )
				{
					if( SpeexWebRtcAecDstoy( MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器失败。" );
					}
					MediaPocsThrdPt->m_AudioInput.m_SpeexWebRtcAecPt = NULL;
				}
				break;
			}
		}
	} //销毁音频输入完毕。

	//销毁音频输出。
	{
		//销毁音频输出线程。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl != NULL )
		{
			WaitForSingleObject( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl, INFINITE ); //等待音频输出线程退出。
			CloseHandle( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl );
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdHdl = NULL;
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputThrdId = NULL;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出线程成功。" );
		}
		
		//销毁音频输出线程线程的临时变量。
		{
			MediaPocsThrdPt->m_AudioOutput.m_Msg = { 0 }; //销毁消息。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFramePt = NULL; //销毁音频输出帧的指针。
			if( MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt != NULL ) //销毁已编码格式音频输出帧的指针。
			{
				free( MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt );
				MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFramePt = NULL;
			}
			MediaPocsThrdPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = 0; //销毁已编码格式音频输出帧的内存大小，单位字节。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLen = 0; //销毁音频输出帧的长度，单位字节。
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal = 0; //销毁音频输出帧链表的元数总数。
			MediaPocsThrdPt->m_AudioOutput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaPocsThrdPt->m_AudioOutput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁音频输出线程线程的临时变量成功。" );
		}

		//销毁音频输出帧链表、音频输出空闲帧链表。
		{
			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioOutputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioOutputFramePt );
					MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}

			if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAudioOutputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_PcmAudioOutputFramePt );
					MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出空闲帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出空闲帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}
		}

		//销毁音频输出设备。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				waveOutUnprepareHeader( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl, &MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
			}
			waveOutReset( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl );
			waveOutClose( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl );
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputDvcHdl = NULL;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出设备成功。" );
		}

		//销毁音频输出缓冲区块数组。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData != NULL )
				{
					free( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData );
					MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData = NULL;
				}
			}
			free( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt );
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrPt = NULL;
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 0;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出缓冲区块数组成功。" );
		}
		
		//销毁音频输出波形器。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloPt != NULL )
		{
			if( AudioOscilloDstoy( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出波形器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出波形器失败。" );
			}
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputOscilloPt = NULL;
		}
		
		//销毁音频输出Wave文件写入器。
		if( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDstoy( MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出Wave文件写入器成功。" );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出Wave文件写入器失败。" );
			}
			MediaPocsThrdPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt = NULL;
		}

		//销毁解码器。
		switch( MediaPocsThrdPt->m_AudioOutput.m_UseWhatDecoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				break;
			}
			case 1: //如果使用Speex解码器。
			{
				if( MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderPt != NULL )
				{
					if( SpeexDecoderDstoy( MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderPt ) == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex解码器成功。" );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex解码器失败。" );
					}
					MediaPocsThrdPt->m_AudioOutput.m_SpeexDecoderPt = NULL;
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
		if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt != NULL )
		{
			MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Stop(); //让视频输入过滤器图管理器停止运行。
			p_TmpSz1 = MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Release();
			MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaControlPt = NULL;
		}
		if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt != NULL )
		{
			p_TmpSz1 = MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt->Release();
			MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerMediaEventPt = NULL;
		}
		if( MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt != NULL )
		{
			p_TmpSz1 = MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt->Release();
			MediaPocsThrdPt->m_VideoInput.m_FilterGraphManagerPt = NULL;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcMaxSamplingRate = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameWidth = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameHeight = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsCrop = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropX = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropY = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropWidth = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameCropHeight = 0;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputDvcFrameIsScale = 0;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入设备成功。" );
		}

		//销毁视频输入线程。
		if( MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt != NULL )
		{
			delete MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt;
			MediaPocsThrdPt->m_VideoInput.m_VideoInputThrdPt = NULL;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入线程成功。" );
		}
		
		//销毁视频输入线程的临时变量。
		{
			MediaPocsThrdPt->m_VideoInput.m_LastVideoInputDvcFrameTimeMsec = 0; //销毁上一个视频输入设备帧的时间。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameTimeStepMsec = 0; //销毁视频输入帧的时间步进。
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt != NULL ) //销毁视频结果帧的指针。
			{
				free( MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt );
				MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFramePt = NULL;
			}
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt != NULL ) //销毁视频临时帧的指针。
			{
				free( MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt );
				MediaPocsThrdPt->m_VideoInput.m_VideoInputTmpFramePt = NULL;
			}
			MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameSz = 0; //销毁视频输入结果帧的内存大小。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputResultFrameLen = 0; //销毁视频输入结果帧的长度。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputSwapFramePt = NULL; //销毁视频交换帧的指针。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameElmPt = NULL; //销毁视频输入帧元素的指针。
			MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal = 0; //销毁视频输入帧链表的元数总数。
			MediaPocsThrdPt->m_VideoInput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaPocsThrdPt->m_VideoInput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁视频输入线程的临时变量成功。" );
		}

		//销毁视频输入帧链表、视频输入空闲帧链表。
		{
			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VideoInputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaPocsThrdPt->m_VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt = NULL;
					MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_VideoInput.m_VideoInputFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁视频输入帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}

			if( MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.m_ConstLenLnkLstPt != NULL )
			{
				while( MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VideoInputFramePt, NULL, 0, NULL ) == 0 )
				{
					free( MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaPocsThrdPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaPocsThrdPt->m_VideoInputFramePt );
					MediaPocsThrdPt->m_VideoInputFramePt = NULL;
					MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.DelHead( 0, NULL );
				}
				if( MediaPocsThrdPt->m_VideoInput.m_VideoInputIdleFrameLnkLst.Dstoy( MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入空闲帧链表成功。" );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁视频输入空闲帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}
		}

        //销毁编码器。
        switch( MediaPocsThrdPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderPt != NULL )
                {
                    if( OpenH264EncoderDstoy( MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderPt, NULL ) == 0 )
                    {
                        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁OpenH264编码器成功。" );
                    }
                    else
                    {
                        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁OpenH264编码器失败。" );
                    }
                    MediaPocsThrdPt->m_VideoInput.m_OpenH264EncoderPt = NULL;
                }
                break;
            }
        }
	} //销毁视频输入完毕。

	//销毁视频输出。
    {
		//销毁视频输出线程。
		if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl != NULL )
		{
			WaitForSingleObject( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl, INFINITE ); //等待视频输出线程退出。
			CloseHandle( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl );
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdHdl = NULL;
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdId = NULL;
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputThrdExitFlag = 0;
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输出线程成功。" );
		}
		
		//销毁视频输出线程的临时变量。
		{
			if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt != NULL ) //销毁视频输出结果帧的指针。
			{
				free( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt );
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFramePt = NULL;
			}
			if( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt != NULL ) //销毁视频输出临时帧的指针。
			{
				free( MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt );
				MediaPocsThrdPt->m_VideoOutput.m_VideoOutputTmpFramePt = NULL;
			}
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputSwapFramePt = NULL; //销毁视频输出交换帧的指针。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameSz = 0; //销毁视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputResultFrameLen = 0; //销毁视频输出结果帧的长度。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameWidth = 0; //销毁视频输出帧的宽度。
			MediaPocsThrdPt->m_VideoOutput.m_VideoOutputFrameHeight = 0; //销毁视频输出帧的高度。
			MediaPocsThrdPt->m_VideoOutput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaPocsThrdPt->m_VideoOutput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁视频输出线程的临时变量成功。" );
		}

        //销毁解码器。
        switch( MediaPocsThrdPt->m_VideoOutput.m_UseWhatDecoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                break;
            }
            case 1: //如果要使用OpenH264解码器。
            {
                if( MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderPt != NULL )
                {
                    if( OpenH264DecoderDstoy( MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderPt, NULL ) == 0 )
                    {
                        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁OpenH264解码器成功。" );
                    }
                    else
                    {
                        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：销毁OpenH264解码器失败。" );
                    }
                    MediaPocsThrdPt->m_VideoOutput.m_OpenH264DecoderPt = NULL;
                }
                break;
            }
        }
    } //销毁视频输出完毕。

	//销毁媒体处理线程的临时变量。
	{
		if( MediaPocsThrdPt->m_PcmAudioInputFramePt != NULL ) //销毁PCM格式音频输入帧的指针。
		{
			free( MediaPocsThrdPt->m_PcmAudioInputFramePt );
			MediaPocsThrdPt->m_PcmAudioInputFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAudioOutputFramePt != NULL ) //销毁PCM格式音频输出帧的指针。
		{
			free( MediaPocsThrdPt->m_PcmAudioOutputFramePt );
			MediaPocsThrdPt->m_PcmAudioOutputFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAudioResultFramePt != NULL ) //销毁PCM格式结果帧的指针。
		{
			free( MediaPocsThrdPt->m_PcmAudioResultFramePt );
			MediaPocsThrdPt->m_PcmAudioResultFramePt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAudioTmpFramePt != NULL ) //销毁PCM格式临时帧的指针。
		{
			free( MediaPocsThrdPt->m_PcmAudioTmpFramePt );
			MediaPocsThrdPt->m_PcmAudioTmpFramePt = NULL;
		}
		MediaPocsThrdPt->m_PcmAudioSwapFramePt = NULL; //销毁PCM格式音频交换帧的指针。
		MediaPocsThrdPt->m_VoiceActSts = 0; //销毁语音活动状态。
		if( MediaPocsThrdPt->m_EncoderAudioInputFramePt != NULL ) //销毁已编码格式音频输入帧的指针。
		{
			free( MediaPocsThrdPt->m_EncoderAudioInputFramePt );
			MediaPocsThrdPt->m_EncoderAudioInputFramePt = NULL;
		}
		MediaPocsThrdPt->m_EncoderAudioInputFrameSz = 0; //销毁已编码格式音频输入帧的内存大小。
		MediaPocsThrdPt->m_EncoderAudioInputFrameLen = 0; //销毁已编码格式音频输入帧的长度。
		MediaPocsThrdPt->m_EncoderAudioInputFrameIsNeedTrans = 0; //销毁已编码格式音频输入帧是否需要传输。
		MediaPocsThrdPt->m_VideoInputFramePt = NULL; //销毁视频输入帧的指针。
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁媒体处理线程的临时变量成功。" );
	} //销毁媒体处理线程的临时变量完毕。

    if( MediaPocsThrdPt->m_ExitFlag != 3 ) //如果需要调用用户定义的销毁函数。
    {
        MediaPocsThrdPt->m_UserDstoyFuncPt( MediaPocsThrdPt ); //调用用户定义的销毁函数。
        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的销毁函数成功。" );
    }
	
    MediaPocsThrdPt->m_RunFlag = RUN_FLAG_END; //设置本线程运行标记为销毁完毕。

	if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) || ( MediaPocsThrdPt->m_ExitFlag == 1 ) ) //如果用户需要直接退出。
    {
        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程已退出。" );
    }
    else //如果用户需用重新初始化。
    {
        if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程重新初始化。" );
		goto ReInit;
    }
	
	//销毁媒体处理线程的句柄。
	if( MediaPocsThrdPt->m_MediaPocsThrdHdl != NULL )
	{
		CloseHandle( MediaPocsThrdPt->m_MediaPocsThrdHdl );
		MediaPocsThrdPt->m_MediaPocsThrdHdl = NULL;
	}

	return 0;
}
