#include "MediaProcThread.h"
#include "LibYUV.h"
#include <mmeapi.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <mmddk.h>
#include <Dshow.h>
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

DWORD WINAPI AudioInputThreadRun( MediaProcThread * MediaProcThreadPt );
DWORD WINAPI AudioOutputThreadRun( MediaProcThread * MediaProcThreadPt );
DWORD WINAPI MediaProcThreadRun( MediaProcThread * MediaProcThreadPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadInit
 * 功能说明：创建并初始化媒体处理线程。
 * 参数说明：MediaProcThreadPtPt：[输出]，存放用于存放媒体处理线程内存指针的内存指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的内存指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的内存指针，不能为NULL。
			 UserProcessFuncPt：[输入]，存放用户定义的处理函数的内存指针，不能为NULL。
			 UserDestroyFuncPt：[输入]，存放用户定义的销毁函数的内存指针，不能为NULL。
			 UserReadAudioVideoInputFrameFuncPt：[输入]，存放用户定义的读取音视频输入帧函数的内存指针，不能为NULL。
			 UserWriteAudioOutputFrameFuncPt：[输入]，存放用户定义的写入音频输出帧函数的内存指针，不能为NULL。
			 UserGetPcmAudioOutputFrameFuncPt：[输入]，存放用户定义的获取PCM格式音频输出帧函数的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadInit( MediaProcThread * * MediaProcThreadPtPt, void * UserDataPt,
						 USER_INIT_FUNC_PT UserInitFuncPt, USER_PROCESS_FUNC_PT UserProcessFuncPt, USER_DESTROY_FUNC_PT UserDestroyFuncPt,
						 USER_READ_AUDIO_VIDEO_INPUT_FRAME_FUNC_PT UserReadAudioVideoInputFrameFuncPt,
						 USER_WRITE_AUDIO_OUTPUT_FRAME_FUNC_PT UserWriteAudioOutputFrameFuncPt, USER_GET_PCM_AUDIO_OUTPUT_FRAME_FUNC_PT UserGetPcmAudioOutputFrameFuncPt,
						 USER_WRITE_VIDEO_OUTPUT_FRAME_FUNC_PT UserWriteVideoOutputFrameFuncPt, USER_GET_YU12_VIDEO_OUTPUT_FRAME_FUNC_PT UserGetYU12VideoOutputFrameFuncPt,
						 VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	MediaProcThread * p_MediaProcThreadPt = NULL; //存放媒体处理线程的内存指针。

	//判断各个变量是否正确。
	if( MediaProcThreadPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放媒体处理线程内存指针的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserInitFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的初始化函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserProcessFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的处理函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserDestroyFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的销毁函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserReadAudioVideoInputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的读取音视频输入帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserWriteAudioOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的写入音频输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserGetPcmAudioOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的获取PCM格式音频输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserWriteVideoOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的写入视频输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserGetYU12VideoOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的获取YU12格式视频输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	//创建并清空媒体处理线程的内存块。
	p_MediaProcThreadPt = ( MediaProcThread * )calloc( 1, sizeof( MediaProcThread ) );
	if( p_MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "calloc 创建并清空媒体处理线程的内存块失败。" );
		goto out;
	}

	p_MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceID = WAVE_MAPPER; //设置音频输入设备的标识符。
	p_MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceID = WAVE_MAPPER; //设置音频输出设备的标识符。
	p_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceID = WAVE_MAPPER; //设置视频输入设备的标识符。
	
	//创建并初始化设置文件的完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_SettingFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化设置文件的完整路径动态字符串失败。" );
		goto out;
	}
	
	//创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
		goto out;
	}
	
	//创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
		goto out;
	}

	//创建并初始化音频输入文件完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输入文件完整路径动态字符串失败。" );
		goto out;
	}

	//创建并初始化音频结果文件完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频结果文件完整路径动态字符串失败。" );
		goto out;
	}

	//创建并初始化音频输出文件完整路径动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输出文件完整路径动态字符串失败。" );
		goto out;
	}

	//创建并初始化错误信息动态字符串。
	if( VarStrInit( &p_MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
	{
		VarStrCpy( ErrInfoVarStrPt, "创建并初始化错误信息动态字符串失败。" );
		goto out;
	}

	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VarStrCpy( ErrInfoVarStrPt, "设置最小计时器分辨率失败。" );
		goto out;
	}

	p_MediaProcThreadPt->m_UserDataPt = UserDataPt; //设置用户数据的内存指针。
	p_MediaProcThreadPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的内存指针。
	p_MediaProcThreadPt->m_UserProcessFuncPt = UserProcessFuncPt; //设置用户定义的处理函数的内存指针。
	p_MediaProcThreadPt->m_UserDestroyFuncPt = UserDestroyFuncPt; //设置用户定义的销毁函数的内存指针。
	p_MediaProcThreadPt->m_UserReadAudioVideoInputFrameFuncPt = UserReadAudioVideoInputFrameFuncPt; //设置用户定义的读取音视频输入帧函数的内存指针。
	p_MediaProcThreadPt->m_UserWriteAudioOutputFrameFuncPt = UserWriteAudioOutputFrameFuncPt; //设置用户定义的写入音频输出帧函数的内存指针。
	p_MediaProcThreadPt->m_UserGetPcmAudioOutputFrameFuncPt = UserGetPcmAudioOutputFrameFuncPt; //设置用户定义的获取PCM格式音频输出帧函数的内存指针。
	p_MediaProcThreadPt->m_UserWriteVideoOutputFrameFuncPt = UserWriteVideoOutputFrameFuncPt; //设置用户定义的写入视频输出帧函数的内存指针。
	p_MediaProcThreadPt->m_UserGetYU12VideoOutputFrameFuncPt = UserGetYU12VideoOutputFrameFuncPt; //设置用户定义的获取YU12格式视频输出帧函数的内存指针。

	*MediaProcThreadPtPt = p_MediaProcThreadPt; //设置媒体处理线程的内存指针。

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadDestroy( p_MediaProcThreadPt, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsSaveSettingToFile
 * 功能说明：设置是否保存设置到文件。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsSaveSettingToFile：[输入]，存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
			 SettingFileFullPathStrPt：[输入]，存放设置文件完整路径字符串的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsSaveSettingToFile( MediaProcThread * MediaProcThreadPt, int IsSaveSettingToFile, const char * SettingFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveSettingToFile != 0 ) && ( SettingFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "设置文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_IsSaveSettingToFile = IsSaveSettingToFile;
	if( IsSaveSettingToFile != 0 )
	{
		if( MediaProcThreadPt->m_SettingFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &MediaProcThreadPt->m_SettingFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化设置文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( MediaProcThreadPt->m_SettingFileFullPathVarStrPt, SettingFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置设置文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadPt->m_IsSaveSettingToFile = 0;
		if( MediaProcThreadPt->m_SettingFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( MediaProcThreadPt->m_SettingFileFullPathVarStrPt );
			MediaProcThreadPt->m_SettingFileFullPathVarStrPt = NULL;
		}
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsPrintLogShowToast
 * 功能说明：设置是否打印Log日志。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsPrintLogShowToast( MediaProcThread * MediaProcThreadPt, int IsPrintLog, int IsShowToast, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_IsPrintLog = IsPrintLog;
	MediaProcThreadPt->m_IsShowToast = IsShowToast;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadGetAudioInputDeviceName
 * 功能说明：获取音频输入设备的名称。调用本函数成功后，必须手动调用MediaProcThreadDestroyDeviceName函数销毁。
 * 参数说明：AudioInputDeviceNameArrPtPtPt：[输出]，存放用于存放音频输入设备名称数组内存指针的内存指针，不能为NULL。
			 AudioInputDeviceTotalPt：[输出]，存放用于存放音频输入设备总数的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadGetAudioInputDeviceName( VarStr * * * AudioInputDeviceNameArrPtPtPt, UINT * AudioInputDeviceTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_AudioInputDeviceNameArrPtPt = NULL; //存放音频输入设备名称数组的内存指针。
	UINT p_AudioInputDeviceTotal = 0; //存放音频输入设备的总数。
	WAVEINCAPS p_WaveInCaps; //存放音频输入设备的功能。
	MMRESULT p_MMResult;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AudioInputDeviceNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输入设备名称数组内存指针的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( AudioInputDeviceTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输入设备总数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	p_AudioInputDeviceTotal = waveInGetNumDevs();
	*AudioInputDeviceTotalPt = p_AudioInputDeviceTotal; //设置音频输入设备的总数。

	//设置音频输入设备名称数组。
	{
		p_AudioInputDeviceNameArrPtPt = ( VarStr * * )calloc( p_AudioInputDeviceTotal, sizeof( VarStr * ) );
		if( p_AudioInputDeviceNameArrPtPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "calloc 创建并清空音频输入设备名称数组的内存块失败。" );
			goto out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AudioInputDeviceTotal; p_TmpUint++ )
		{
			p_MMResult = waveInGetDevCaps( p_TmpUint, &p_WaveInCaps, sizeof( WAVEINCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrCpy( ErrInfoVarStrPt, "waveInGetDevCaps 获取音频输入设备功能失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( p_MMResult, 'M' ) );
				goto out; //本函数返回失败。
			}
			if( VarStrInitByStr( &p_AudioInputDeviceNameArrPtPt[p_TmpUint], p_WaveInCaps.szPname ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "VarStrInit 创建音频输入设备名称的动态字符串失败。" );
				goto out; //本函数返回失败。
			}
		}

		*AudioInputDeviceNameArrPtPtPt = p_AudioInputDeviceNameArrPtPt; //设置音频输入设备名称数组的内存指针。
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadDestroyDeviceName( p_AudioInputDeviceNameArrPtPt, p_AudioInputDeviceTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadGetAudioOutputDeviceName
 * 功能说明：获取音频输出设备的名称。调用本函数成功后，必须手动调用MediaProcThreadDestroyDeviceName函数销毁。
 * 参数说明：AudioOutputDeviceNameArrPtPtPt：[输出]，存放用于存放音频输出设备名称数组内存指针的内存指针，不能为NULL。
			 AudioOutputDeviceTotalPt：[输出]，存放用于存放音频输出设备总数的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadGetAudioOutputDeviceName( VarStr * * * AudioOutputDeviceNameArrPtPtPt, UINT * AudioOutputDeviceTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_AudioOutputDeviceNameArrPtPt = NULL; //存放音频输出设备名称数组的内存指针。
	UINT p_AudioOutputDeviceTotal = 0; //存放音频输出设备的总数。
	WAVEOUTCAPS p_WaveOutCaps; //存放音频输出设备的功能。
	MMRESULT p_MMResult;
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( AudioOutputDeviceNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输出设备名称数组内存指针的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( AudioOutputDeviceTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频输出设备总数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	p_AudioOutputDeviceTotal = waveOutGetNumDevs();
	*AudioOutputDeviceTotalPt = p_AudioOutputDeviceTotal; //设置音频输出设备的总数。

	//设置音频输出设备名称数组。
	{
		p_AudioOutputDeviceNameArrPtPt = ( VarStr * * )calloc( p_AudioOutputDeviceTotal, sizeof( VarStr * ) );
		if( p_AudioOutputDeviceNameArrPtPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "calloc 创建并清空音频输出设备名称数组的内存块失败。" );
			goto out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_AudioOutputDeviceTotal; p_TmpUint++ )
		{
			p_MMResult = waveOutGetDevCaps( p_TmpUint, &p_WaveOutCaps, sizeof( WAVEOUTCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrCpy( ErrInfoVarStrPt, "waveInGetDevCaps 获取音频输出设备功能失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( p_MMResult, 'M' ) );
				goto out; //本函数返回失败。
			}
			if( VarStrInitByStr( &p_AudioOutputDeviceNameArrPtPt[p_TmpUint], p_WaveOutCaps.szPname ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "VarStrInit 创建音频输出设备名称的动态字符串失败。" );
				goto out; //本函数返回失败。
			}
		}

		*AudioOutputDeviceNameArrPtPtPt = p_AudioOutputDeviceNameArrPtPt; //设置音频输出设备名称数组的内存指针。
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadDestroyDeviceName( p_AudioOutputDeviceNameArrPtPt, p_AudioOutputDeviceTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadGetVideoInputDeviceName
 * 功能说明：获取视频输入设备的名称。调用本函数成功后，必须手动调用MediaProcThreadDestroyDeviceName函数销毁。
 * 参数说明：VideoInputDeviceNameArrPtPtPt：[输出]，存放用于存放视频输入设备名称数组内存指针的内存指针，不能为NULL。
			 VideoInputDeviceTotalPt：[输出]，存放用于存放视频输入设备总数的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadGetVideoInputDeviceName( VarStr * * * VideoInputDeviceNameArrPtPtPt, UINT * VideoInputDeviceTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VarStr * * p_VideoInputDeviceNameArrPtPt = NULL; //存放视频输入设备名称数组的内存指针。
	VarStr * * p_TmpVarStrPtPt = NULL;
	UINT p_VideoInputDeviceTotal = 0; //存放视频输入设备的总数。
	ICreateDevEnum * p_CreateDevEnumPt = NULL;
	IEnumMoniker * p_EnumMonikerPt = NULL;
	IMoniker * p_MonikerPt = NULL;
	IPropertyBag * p_PropertyBagPt = NULL;
	ULONG p_Ulong;
	
	//判断各个变量是否正确。
	if( VideoInputDeviceNameArrPtPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放视频输入设备名称数组内存指针的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( VideoInputDeviceTotalPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放视频输入设备总数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	//初始化COM库。
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY );

	//创建系统设备枚举器对象。
	if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
	{
		VarStrCpy( ErrInfoVarStrPt, "CoCreateInstance 创建系统设备枚举器对象失败。" );
		goto out; //本函数返回失败。
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
					p_VideoInputDeviceTotal++;
					p_TmpVarStrPtPt = ( VarStr * * )realloc( p_VideoInputDeviceNameArrPtPt, p_VideoInputDeviceTotal * sizeof( VarStr * ) );
					if( p_TmpVarStrPtPt == NULL )
					{
						VarStrCpy( ErrInfoVarStrPt, "realloc 创建并清空视频输入设备名称数组的内存块失败。" );
						goto out; //本函数返回失败。
					}
					p_VideoInputDeviceNameArrPtPt = p_TmpVarStrPtPt;
					if( VarStrInit( &p_VideoInputDeviceNameArrPtPt[p_VideoInputDeviceTotal - 1], wcslen( varName.bstrVal ) + 1 ) != 0 )
					{
						VarStrCpy( ErrInfoVarStrPt, "VarStrInit 创建视频输入设备名称的动态字符串失败。" );
						goto out; //本函数返回失败。
					}
					WideCharToMultiByte( CP_OEMCP, 0, varName.bstrVal, -1, p_VideoInputDeviceNameArrPtPt[p_VideoInputDeviceTotal - 1]->m_StrPt, p_VideoInputDeviceNameArrPtPt[p_VideoInputDeviceTotal - 1]->m_StrSz, NULL, NULL );
					p_VideoInputDeviceNameArrPtPt[p_VideoInputDeviceTotal - 1]->m_StrLen = p_VideoInputDeviceNameArrPtPt[p_VideoInputDeviceTotal - 1]->m_StrSz - 1;
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

	*VideoInputDeviceNameArrPtPtPt = p_VideoInputDeviceNameArrPtPt; //设置视频输入设备名称数组的内存指针。
	*VideoInputDeviceTotalPt = p_VideoInputDeviceTotal; //设置视频输入设备的总数。

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadDestroyDeviceName( p_VideoInputDeviceNameArrPtPt, p_VideoInputDeviceTotal, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadDestroyDeviceName
 * 功能说明：销毁设备名称数组。
 * 参数说明：DeviceNameArrPtPt：[输出]，存放设备名称数组的内存指针，不能为NULL。
			 DeviceTotal：[输出]，存放设备的总数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadDestroyDeviceName( VarStr * * DeviceNameArrPtPt, UINT DeviceTotal, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( DeviceNameArrPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "设备名称数组的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	//循环销毁所有的动态字符串。
	for( p_TmpUint = 0; p_TmpUint < DeviceTotal; p_TmpUint++ )
	{
		if( DeviceNameArrPtPt[p_TmpUint] != NULL )
		{
			VarStrDestroy( DeviceNameArrPtPt[p_TmpUint] );
		}
	}

	free( DeviceNameArrPtPt ); //销毁设备名称数组的内存块。

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsUseAudioInput
 * 功能说明：设置是否使用音频输入。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             IsUseAudioInput：[输入]，存放是否使用音频输入，为非0表示要使用，为0表示不使用。
			 InputDeviceID：[输入]，存放音频输入设备的标识符，从0到音频输入设备的总数减一。
			 SamplingRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000。
			 FrameLenMsec：[输入]，存放帧的毫秒长度，取值只能为10毫秒的倍数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsUseAudioInput( MediaProcThread * MediaProcThreadPt, int32_t IsUseAudioInput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseAudioInput != 0 ) && ( SamplingRate != 8000 ) && ( SamplingRate != 16000 ) && ( SamplingRate != 32000 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "采样频率不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseAudioInput != 0 ) && ( ( FrameLenMsec <= 0 ) || ( FrameLenMsec % 10 != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的毫秒长度不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput = IsUseAudioInput;
    MediaProcThreadPt->m_AudioInput.m_SamplingRate = SamplingRate;
    MediaProcThreadPt->m_AudioInput.m_FrameLen = FrameLenMsec * SamplingRate / 1000;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseNoAec
 * 功能说明：设置音频输入不使用声学回音消除器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseNoAec( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseSpeexAec
 * 功能说明：设置音频输入要使用Speex声学回音消除器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 FilterLen：[输入]，存放Speex声学回音消除器的滤波器数据长度，单位毫秒。
			 IsSaveMemFile：[输入]，存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathStrPt：[输入]，存放Speex声学回音消除器的内存块文件完整路径字符串的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseSpeexAec( MediaProcThread * MediaProcThreadPt, int32_t FilterLen, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "Speex声学回音消除器的内存块文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 1;
	MediaProcThreadPt->m_AudioInput.m_SpeexAecFilterLen = FilterLen;
	MediaProcThreadPt->m_AudioInput.m_SpeexAecIsUseRec = IsUseRec;
    MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoMutp = EchoMutp;
    MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoCntu = EchoCntu;
    MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupes = EchoSupes;
    MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupesAct = EchoSupesAct;
	MediaProcThreadPt->m_AudioInput.m_SpeexAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VarStrCpy( MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 0;
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseWebRtcAecm
 * 功能说明：设置音频输入要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseWebRtcAecm( MediaProcThread * MediaProcThreadPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 2;
	MediaProcThreadPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode = IsUseCNGMode;
	MediaProcThreadPt->m_AudioInput.m_WebRtcAecmEchoMode = EchoMode;
	MediaProcThreadPt->m_AudioInput.m_WebRtcAecmDelay = Delay;
	
	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseWebRtcAec
 * 功能说明：设置音频输入要使用WebRtc浮点版声学回音消除器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             EchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             Delay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             IsUseDelayAgnosticMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             IsUseExtdFilterMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             IsUseRefinedFilterAdaptAecMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             IsUseAdaptAdjDelay：[输入]，存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
             IsSaveMemFile：[输入]，存放WebRtc浮点版声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
             MemFileFullPathVarStrPt：[输入]，存放WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseWebRtcAec( MediaProcThread * MediaProcThreadPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "WebRtc浮点版声学回音消除器的内存块文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 3;
	MediaProcThreadPt->m_AudioInput.m_WebRtcAecEchoMode = EchoMode;
    MediaProcThreadPt->m_AudioInput.m_WebRtcAecDelay = Delay;
    MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode = IsUseDelayAgnosticMode;
    MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode = IsUseExtdFilterMode;
    MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( VarStrCpy( MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 0;
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseSpeexWebRtcAec
 * 功能说明：设置音频输入要使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             WorkMode：[输入]，存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
             SpeexAecFilterLen：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器的滤波器数据长度，单位毫秒。
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
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseSpeexWebRtcAec( MediaProcThread * MediaProcThreadPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgnosticMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatAec = 4;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWorkMode = WorkMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen = SpeexAecFilterLen;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec = SpeexAecIsUseRec;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp = SpeexAecEchoMutp;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu = SpeexAecEchoCntu;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes = SpeexAecEchoSupes;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode = WebRtcAecmEchoMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay = WebRtcAecmDelay;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode = WebRtcAecEchoMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay = WebRtcAecDelay;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode = WebRtcAecIsUseDelayAgnosticMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec = IsUseSameRoomAec;
	MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay = SameRoomEchoMinDelay;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseNoNs
 * 功能说明：设置音频输入不使用噪音抑制器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseNoNs( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatNs = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetUseSpeexPprocNs
 * 功能说明：设置音频输入要使用Speex预处理器的噪音抑制。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             IsUseNs：[输入]，存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
			 NoiseSupes：[输入]，存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 IsUseDereverb：[输入]，存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。
			 IsUseRec：[输入]，存放Speex预处理器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
			 EchoMutp：[输入]，存放Speex预处理器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
			 EchoCntu：[输入]，存放Speex预处理器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
			 EchoSupes：[输入]，存放Speex预处理器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 EchoSupesAct：[输入]，存放Speex预处理器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseSpeexPprocNs( MediaProcThread * MediaProcThreadPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatNs = 1;
	MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseNs = IsUseNs;
	MediaProcThreadPt->m_AudioInput.m_SpeexPprocNoiseSupes = NoiseSupes;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseDereverb = IsUseDereverb;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetUseWebRtcNsx
 * 功能说明：设置音频输入要使用WebRtc定点版噪音抑制器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseWebRtcNsx( MediaProcThread * MediaProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatNs = 2;
	MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseWebRtcNs
 * 功能说明：设置音频输入要使用WebRtc浮点版噪音抑制器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseWebRtcNs( MediaProcThread * MediaProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatNs = 3;
	MediaProcThreadPt->m_AudioInput.m_WebRtcNsPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseRNNoise
 * 功能说明：设置音频输入要使用RNNoise噪音抑制器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseRNNoise( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatNs = 4;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputIsUseSpeexPprocOther
 * 功能说明：设置音频输入是否使用Speex预处理器的其他功能。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             m_IsUseOther：[输入]，存放Speex预处理器是否使用其他功能，为非0表示要使用，为0表示不使用。
             IsUseVad：[输入]，存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
             VadProbStart：[输入]，存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
             VadProbCntu：[输入]，存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
             IsUseAgc：[输入]，存放Speex预处理器是否使用自动增益控制，为非0表示要使用，为0表示不使用。
             AgcLevel：[输入]，存放Speex预处理器在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
             AgcIncrement：[输入]，存放Speex预处理器在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
             AgcDecrement：[输入]，存放Speex预处理器在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
             AgcMaxGain：[输入]，存放Speex预处理器在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputIsUseSpeexPprocOther( MediaProcThread * MediaProcThreadPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther = IsUseOther;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseVad = IsUseVad;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbStart = VadProbStart;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbCntu = VadProbCntu;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseAgc = IsUseAgc;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcLevel = AgcLevel;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcIncrement = AgcIncrement;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcDecrement = AgcDecrement;
    MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcMaxGain = AgcMaxGain;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetUsePcm
 * 功能说明：设置音频输入要使用PCM原始数据。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUsePcm( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseSpeexEncoder
 * 功能说明：设置音频输入要使用Speex编码器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             UseCbrOrVbr：[输入]，存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             Quality：[输入]，存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             Complexity：[输入]，存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             PlcExpectedLossRate：[输入]，存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseSpeexEncoder( MediaProcThread * MediaProcThreadPt, int32_t UseCbrOrVbr, int32_t Quality, int32_t Complexity, int32_t PlcExpectedLossRate, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder = 1;
	MediaProcThreadPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr = UseCbrOrVbr;
    MediaProcThreadPt->m_AudioInput.m_SpeexEncoderQuality = Quality;
    MediaProcThreadPt->m_AudioInput.m_SpeexEncoderComplexity = Complexity;
    MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate = PlcExpectedLossRate;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseOpusEncoder
 * 功能说明：设置音频输入要使用Opus编码器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseOpusEncoder( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder = 2;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputIsSaveAudioToFile
 * 功能说明：设置音频输入是否保存音频到文件。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioInputFileFullPathStrPt：[输入]，存放音频输入文件完整路径字符串的内存指针。
			 AudioResultFileFullPathStrPt：[输入]，存放音频结果文件完整路径字符串的内存指针。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputIsSaveAudioToFile( MediaProcThread * MediaProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioInputFileFullPathStrPt, const char * AudioResultFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioInputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioResultFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频结果文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile = IsSaveAudioToFile;
	if( IsSaveAudioToFile != 0 )
	{
		if( VarStrCpy( MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt, AudioInputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输入文件完整路径动态字符串失败。" );
			goto out;
		}
		if( VarStrCpy( MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt, AudioResultFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频结果文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile = 0;
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputUseDevice
 * 功能说明：设置音频输入使用的设备。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 AudioInputDeviceID：[输入]，存放音频输入设备的标识符，从0到音频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputUseDevice( MediaProcThread * MediaProcThreadPt, UINT AudioInputDeviceID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( AudioInputDeviceID != WAVE_MAPPER ) && ( AudioInputDeviceID >= waveInGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入设备的标识符不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceID = AudioInputDeviceID;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioInputIsMute
 * 功能说明：设置音频输入是否静音。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioInputIsMute( MediaProcThread * MediaProcThreadPt, int32_t IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioInput.m_AudioInputIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsUseAudioOutput
 * 功能说明：设置是否使用音频输出。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             IsUseAudioOutput：[输入]，存放是否使用音频输出，为非0表示要使用，为0表示不使用。
			 SamplingRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000。
			 FrameLenMsec：[输入]，存放帧的毫秒长度，取值只能为10毫秒的倍数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsUseAudioOutput( MediaProcThread * MediaProcThreadPt, int32_t IsUseAudioOutput, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseAudioOutput != 0 ) && ( SamplingRate != 8000 ) && ( SamplingRate != 16000 ) && ( SamplingRate != 32000 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "采样频率不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseAudioOutput != 0 ) && ( ( FrameLenMsec <= 0 ) || ( FrameLenMsec % 10 != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的毫秒长度不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput = IsUseAudioOutput;
    MediaProcThreadPt->m_AudioOutput.m_SamplingRate = SamplingRate;
    MediaProcThreadPt->m_AudioOutput.m_FrameLen = FrameLenMsec * SamplingRate / 1000;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputUsePcm
 * 功能说明：设置音频输出要使用PCM原始数据。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputUsePcm( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputUseSpeexDecoder
 * 功能说明：设置音频输出要使用Speex解码器。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
             IsUsePerceptualEnhancement：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputUseSpeexDecoder( MediaProcThread * MediaProcThreadPt, int32_t IsUsePerceptualEnhancement, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder = 1;
    MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement = IsUsePerceptualEnhancement;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputUseOpusDecoder
 * 功能说明：设置音频输出要使用Opus解码器。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputUseOpusDecoder( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder = 2;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputIsSaveAudioToFile
 * 功能说明：设置音频输出是否保存音频到文件。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioOutputFileFullPathStrPt：[输入]，存放音频输出文件完整路径字符串的内存指针。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputIsSaveAudioToFile( MediaProcThread * MediaProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioOutputFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioOutputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_IsSaveAudioToFile = IsSaveAudioToFile;
	if( IsSaveAudioToFile != 0 )
	{
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输出文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt, AudioOutputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输出文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		MediaProcThreadPt->m_AudioOutput.m_IsSaveAudioToFile = 0;
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt );
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt = NULL;
		}
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputUseDevice
 * 功能说明：设置音频输出使用的设备。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 AudioOutputDeviceID：[输入]，存放音频输出设备的标识符，从0到音频输出设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputUseDevice( MediaProcThread * MediaProcThreadPt, UINT AudioOutputDeviceID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( AudioOutputDeviceID != WAVE_MAPPER ) && ( AudioOutputDeviceID >= waveOutGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出设备的标识符不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceID = AudioOutputDeviceID;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetAudioOutputIsMute
 * 功能说明：设置音频输出是否静音。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetAudioOutputIsMute( MediaProcThread * MediaProcThreadPt, int32_t IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_AudioOutput.m_AudioOutputIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsUseVideoInput
 * 功能说明：设置是否使用视频输入。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             IsUseVideoInput：[输入]，存放是否使用视频输入，为非0表示要使用，为0表示不使用。
			 MaxSamplingRate：[输入]，存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
			 FrameWidth：[输入]，存放帧的宽度，单位为像素。
			 FrameHeight：[输入]，存放帧的高度，单位为像素。
			 VideoInputPreviewWnd：[输入]，存放视频输入预览窗口的句柄，可以为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsUseVideoInput( MediaProcThread * MediaProcThreadPt, int32_t IsUseVideoInput, int32_t MaxSamplingRate, int32_t FrameWidth, int32_t FrameHeight, HWND VideoInputPreviewWnd, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( MaxSamplingRate < 1 ) || ( MaxSamplingRate > 60 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "最大采样频率不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( FrameWidth <= 0 ) || ( ( FrameWidth & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的宽度不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseVideoInput != 0 ) && ( ( FrameHeight <= 0 ) || ( ( FrameHeight & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的高度不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput = IsUseVideoInput;
    MediaProcThreadPt->m_VideoInput.m_MaxSamplingRate = MaxSamplingRate;
    MediaProcThreadPt->m_VideoInput.m_FrameWidth = FrameWidth;
	MediaProcThreadPt->m_VideoInput.m_FrameHeight = FrameHeight;
	MediaProcThreadPt->m_VideoInput.m_VideoInputPreviewWndHdl = VideoInputPreviewWnd;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoInputUseYU12
 * 功能说明：设置视频输入要使用YU12原始数据。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoInputUseYU12( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoInputUseOpenH264Encoder
 * 功能说明：设置视频输入要使用OpenH264编码器。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             VideoType：[输入]，存放OpenH264编码器的视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
             EncodedBitrate：[输入]，存放OpenH264编码器的编码后比特率，单位为bps。
             BitrateControlMode：[输入]，存放OpenH264编码器的比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
             IDRFrameIntvl：[输入]，存放OpenH264编码器的IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
             Complexity：[输入]，存放OpenH264编码器的复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,2]。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoInputUseOpenH264Encoder( MediaProcThread * MediaProcThreadPt, int32_t VideoType, int32_t EncodedBitrate, int32_t BitrateControlMode, int32_t IDRFrameIntvl, int32_t Complexity, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder = 1;
	MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderVideoType = VideoType;
    MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderEncodedBitrate = EncodedBitrate;
    MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderBitrateControlMode = BitrateControlMode;
    MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderIDRFrameIntvl = IDRFrameIntvl;
	MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderComplexity = Complexity;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoInputUseDevice
 * 功能说明：设置视频输入使用的设备。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 VideoInputDeviceID：[输入]，存放视频输入设备的标识符，从0到视频输入设备的总数减一，为-1表示使用默认设备。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoInputUseDevice( MediaProcThread * MediaProcThreadPt, UINT VideoInputDeviceID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceID = VideoInputDeviceID;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoInputIsBlack
 * 功能说明：设置视频输入是否黑屏。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			 IsBlack：[输入]，存放视频输入设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoInputIsBlack( MediaProcThread * MediaProcThreadPt, int IsBlack, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	MediaProcThreadPt->m_VideoInput.m_VideoInputIsBlack = IsBlack;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetIsUseVideoOutput
 * 功能说明：设置是否使用视频输出。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
             IsUseVideoOutput：[输入]，存放是否使用视频输出，为非0表示要使用，为0表示不使用。
			 FrameWidth：[输入]，存放帧的宽度，单位为像素。
			 FrameHeight：[输入]，存放帧的高度，单位为像素。
			 VideoOutputDisplayWnd：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 VideoOutputDisplayScale：[输入]，存放视频输出缩放的倍数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetIsUseVideoOutput( MediaProcThread * MediaProcThreadPt, int32_t IsUseVideoOutput, int32_t FrameWidth, int32_t FrameHeight, HWND VideoOutputDisplayWnd, float VideoOutputDisplayScale, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseVideoOutput != 0 ) && ( ( FrameWidth <= 0 ) || ( ( FrameWidth & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的宽度不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsUseVideoOutput != 0 ) && ( ( FrameHeight <= 0 ) || ( ( FrameHeight & 1 ) != 0 ) ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的高度不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput = IsUseVideoOutput;
    MediaProcThreadPt->m_VideoOutput.m_FrameWidth = FrameWidth;
	MediaProcThreadPt->m_VideoOutput.m_FrameHeight = FrameHeight;
	MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayWndHdl = VideoOutputDisplayWnd;
	MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale = VideoOutputDisplayScale;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoOutputUseYU12
 * 功能说明：设置视频输出要使用YU12原始数据。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoOutputUseYU12( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoOutput.m_UseWhatDecoder = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoOutputUseOpenH264Decoder
 * 功能说明：设置视频输出要使用OpenH264解码器。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
             DecodeThreadNum：[输出]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoOutputUseOpenH264Decoder( MediaProcThread * MediaProcThreadPt, int32_t DecodeThreadNum, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_VideoOutput.m_UseWhatDecoder = 1;
	MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderDecodeThreadNum = DecodeThreadNum;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadSetVideoOutputIsBlack
 * 功能说明：设置视频输出是否黑屏。
 * 参数说明：MediaProcThreadPt：[输出]，存放媒体处理线程的内存指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadSetVideoOutputIsBlack( MediaProcThread * MediaProcThreadPt, int IsBlack, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	MediaProcThreadPt->m_VideoOutput.m_VideoOutputIsBlack = IsBlack;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaProcThreadStart
* 功能说明：启动媒体处理线程。
* 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadStart( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( MediaProcThreadPt->m_MediaProcThreadHdl != NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程已经启动。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_MediaProcThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )MediaProcThreadRun, MediaProcThreadPt, 0, NULL ); //创建并启动媒体处理线程。
	if( MediaProcThreadPt->m_MediaProcThreadHdl == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "CreateThread 创建并启动媒体处理线程失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( GetLastError(), 'G' ) );
		goto out; //本函数返回失败。
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaProcThreadRequireExit
* 功能说明：请求媒体处理线程退出。
* 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
            ExitFlag：[输入]，存放退出标记的值，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDestroy销毁函数。
			IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示阻塞。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadRequireExit( MediaProcThread * MediaProcThreadPt, int ExitFlag, int IsBlockWait, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "退出标记的值不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadPt->m_ExitFlag = ExitFlag; //设置媒体处理线程的退出标记。

	if( IsBlockWait != 0 ) //如果需要阻塞等待。
    {
        if( ExitFlag == 1 ) //如果是请求退出。
		{
			do
			{
				if( MediaProcThreadPt->m_MediaProcThreadHdl == NULL ) //如果媒体处理线程的句柄为空，表示媒体处理线程已经退出。
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
				if( MediaProcThreadPt->m_MediaProcThreadHdl == NULL ) //如果媒体处理线程的句柄为空，表示媒体处理线程已经退出。
				{
					break;
				}
				if( MediaProcThreadPt->m_ExitFlag == 0 ) //如果退出标记为0保持运行，表示重启完毕。
				{
					break;
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			} while( true );
        }
    }

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：MediaProcThreadDestroy
* 功能说明：销毁媒体处理线程。
* 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaProcThreadDestroy( MediaProcThread * MediaProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "媒体处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	MediaProcThreadRequireExit( MediaProcThreadPt, 1, 1, NULL ); //请求媒体处理线程退出，并阻塞等待。

	//销毁设置文件的完整路径动态字符串。
	if( MediaProcThreadPt->m_SettingFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_SettingFileFullPathVarStrPt );
		MediaProcThreadPt->m_SettingFileFullPathVarStrPt = NULL;
	}
	
	//销毁Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt );
		MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt = NULL;
	}
	
	//销毁WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt );
		MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt = NULL;
	}

	//销毁音频输入文件完整路径动态字符串。
	if( MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt );
		MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt = NULL;
	}

	//销毁音频结果文件完整路径动态字符串。
	if( MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt );
		MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt = NULL;
	}
	
	//销毁音频输出文件完整路径动态字符串。
	if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt );
		MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt = NULL;
	}

	//销毁错误信息动态字符串。
	if( MediaProcThreadPt->m_ErrInfoVarStrPt != NULL )
	{
		VarStrDestroy( MediaProcThreadPt->m_ErrInfoVarStrPt );
		MediaProcThreadPt->m_ErrInfoVarStrPt = NULL;
	}

	//销毁媒体处理线程的内存块。
	free( MediaProcThreadPt );

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioInputThreadRun
 * 功能说明：音频输入线程主函数。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioInputThreadRun( MediaProcThread * MediaProcThreadPt )
{
	//开始音频输入循环。
	while( GetMessage( &MediaProcThreadPt->m_AudioInput.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaProcThreadPt->m_AudioInput.m_Msg.message == WIM_DATA ) //如果是音频输入缓冲区块读取完毕消息。
		{
			if( ( ( WAVEHDR * )MediaProcThreadPt->m_AudioInput.m_Msg.lParam )->dwBytesRecorded != MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) ) //如果本次音频输入帧的数据长度不等于帧的数据长度，就表示本次音频输入帧不完整，丢弃。
			{
				waveInAddBuffer( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, ( WAVEHDR * )MediaProcThreadPt->m_AudioInput.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。
				continue;
			}

			//获取一个音频输入空闲帧。
			ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, &MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal, NULL );
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal > 0 ) //如果音频输入空闲帧链表中有音频输入空闲帧。
			{
				//从音频输入空闲帧链表中取出第一个音频输入空闲帧。
				{
					ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, NULL, &MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt, NULL, NULL );
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, NULL );
				}
				if( MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFI( "音频输入线程：从音频输入空闲帧链表中取出第一个音频输入空闲帧，音频输入空闲帧链表元素个数：%" PRIuPTR "。", MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal );
			}
			else //如果音频输入空闲帧链表中没有音频输入空闲帧。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFE( "音频输入线程：音频输入空闲帧链表中没有音频输入空闲帧，本次音频输入帧丢弃。" );
			}

			//追加本次音频输入帧到音频输入帧链表。
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt != NULL )
			{
				memcpy( MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt, ( ( WAVEHDR * )MediaProcThreadPt->m_AudioInput.m_Msg.lParam )->lpData, MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) );
				ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, &MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt, NULL );
				MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt = NULL;
			}

			waveInAddBuffer( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, ( WAVEHDR * )MediaProcThreadPt->m_AudioInput.m_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频输入缓冲区块添加到音频输入设备上。

			if( MediaProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaProcThreadPt->m_AudioInput.m_NowTimeMsec );
				ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, &MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal, NULL );
				LOGFI( "音频输入线程：读取耗时 %" PRIu64 " 毫秒，音频输入帧链表元素个数：%" PRIuPTR "。", MediaProcThreadPt->m_AudioInput.m_NowTimeMsec - MediaProcThreadPt->m_AudioInput.m_LastTimeMsec, MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal );
				MediaProcThreadPt->m_AudioInput.m_LastTimeMsec = MediaProcThreadPt->m_AudioInput.m_NowTimeMsec;
			}
		}
		else if( MediaProcThreadPt->m_AudioInput.m_Msg.message == WIM_OPEN ) //如果是音频输入设备打开消息。
		{
			SetThreadPriority( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaProcThreadPt->m_AudioInput.m_LastTimeMsec );
				LOGI( "音频输入线程：开始准备音频输入。" );
			}
		}
		else if( MediaProcThreadPt->m_AudioInput.m_Msg.message == WIM_CLOSE ) //如果是音频输入设备关闭消息。
		{
			MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceID = WAVE_MAPPER; //设置音频输入设备的标识符为默认的音频输入设备。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输入设备。" );
			break;
		}
	}

	if( MediaProcThreadPt->m_AudioInput.m_Msg.message == WM_QUIT )
	{
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioOutputThreadRun
 * 功能说明：音频输出线程主函数。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioOutputThreadRun( MediaProcThread * MediaProcThreadPt )
{
	//开始音频输出循环。
	while( GetMessage( &MediaProcThreadPt->m_AudioOutput.m_Msg, NULL, 0, 0 ) != 0 )
	{
		if( MediaProcThreadPt->m_AudioOutput.m_Msg.message == WOM_DONE ) //如果是音频输出缓冲区块写入完毕消息。
		{
			//调用用户定义的写入音频输出帧函数，并解码成PCM原始数据。
			switch( MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder ) //使用什么解码器。
			{
				case 0: //如果使用PCM原始数据。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen = ( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength;
					MediaProcThreadPt->m_UserWriteAudioOutputFrameFuncPt( MediaProcThreadPt, ( int16_t * )( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->lpData, NULL, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen );
					break;
				}
				case 1: //如果使用Speex编解码器。
				{
					//调用用户定义的写入音频输出帧函数。
					MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen = MediaProcThreadPt->m_AudioOutput.m_FrameLen;
					MediaProcThreadPt->m_UserWriteAudioOutputFrameFuncPt( MediaProcThreadPt, NULL, MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen );

					//使用Speex解码器。
					if( SpeexDecoderProc( MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderPt, MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen, ( int16_t * )( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->lpData ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：使用Speex解码器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：使用Speex解码器失败。" );
					}
					break;
				}
				case 2: //如果使用Opus编解码器。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：暂不支持使用Opus解码器。" );
				}
			}
			
			//判断音频输出是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
            if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIsMute != 0 )
            {
				memset( ( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->lpData, 0, ( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength );
            }

			//写入本次音频输出帧到音频输出设备。
			waveOutWrite( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl, ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam, sizeof( WAVEHDR ) );
			
			//调用用户定义的获取PCM格式音频输出帧函数。
			MediaProcThreadPt->m_UserGetPcmAudioOutputFrameFuncPt( MediaProcThreadPt, ( int16_t * )( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->lpData, ( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->dwBufferLength );

			//获取一个音频输出空闲帧。
			ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal, NULL );
			if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal > 0 ) //如果音频输出空闲帧链表中有音频输出空闲帧。
			{
				//从音频输出空闲帧链表中取出第一个音频输出空闲帧。
				{
					ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, NULL, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt, NULL, NULL );
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, NULL );
				}
				if( MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFI( "音频输出线程：从音频输出空闲帧链表中取出第一个音频输出空闲帧，音频输出空闲帧链表元素个数：%" PRIuPTR "。", MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal );
			}
			else //如果音频输出空闲帧链表中没有音频输出空闲帧。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFE( "音频输出线程：音频输出空闲帧链表中没有音频输出空闲帧，本次音频输出帧丢弃。" );
			}

			//追加本次音频输出帧到音频输出帧链表。
			if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt != NULL )
			{
				memcpy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt, ( ( WAVEHDR * )MediaProcThreadPt->m_AudioOutput.m_Msg.lParam )->lpData, MediaProcThreadPt->m_AudioOutput.m_FrameLen * sizeof( int16_t ) );
				ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt, NULL );
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt = NULL;
			}

			if( MediaProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaProcThreadPt->m_AudioOutput.m_NowTimeMsec );
				ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal, NULL );
				LOGFI( "音频输出线程：写入耗时 %" PRIu64 " 毫秒，音频输出帧链表元素个数：%" PRIuPTR "。", MediaProcThreadPt->m_AudioOutput.m_NowTimeMsec - MediaProcThreadPt->m_AudioOutput.m_LastTimeMsec, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal );
				MediaProcThreadPt->m_AudioOutput.m_LastTimeMsec = MediaProcThreadPt->m_AudioOutput.m_NowTimeMsec;
			}
		}
		else if( MediaProcThreadPt->m_AudioOutput.m_Msg.message == WOM_OPEN ) //如果是音频输出设备打开消息。
		{
			SetThreadPriority( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( MediaProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &MediaProcThreadPt->m_AudioOutput.m_LastTimeMsec );
				LOGI( "音频输出线程：开始准备音频输出。" );
			}
		}
		else if( MediaProcThreadPt->m_AudioOutput.m_Msg.message == WOM_CLOSE ) //如果是音频输出设备关闭消息。
		{
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceID = WAVE_MAPPER; //设置音频输出设备的标识符为默认的音频输出设备。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输出设备。" );
			break;
		}
	}

	if( MediaProcThreadPt->m_AudioOutput.m_Msg.message == WM_QUIT )
	{
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VideoInputThread
 * 功能说明：视频输入线程主类。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class VideoInputThread : public ISampleGrabberCB
{
public:
	MediaProcThread * m_MediaProcThreadPt;

	VideoInputThread( MediaProcThread * MediaProcThreadPt )
	{
		m_MediaProcThreadPt = MediaProcThreadPt;
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
		if( m_MediaProcThreadPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec );
			LOGFI( "视频输入线程：读取一个视频输入帧。" );
		}

		if( m_MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec != 0 ) //如果已经设置过上一个视频输入帧的时间。
		{
			if( m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec - m_MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec >= m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameTimeStepMsec )
			{
				m_MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec += m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameTimeStepMsec;
			}
			else
			{
				LOGFI( "视频输入线程：采样频率过快，本次视频输入帧丢弃。" );
			}
		}
		else //如果没有设置过上一个视频输入帧的时间。
		{
			m_MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec = m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec;
		}

		//将视频输入帧复制到视频结果帧，方便处理。
		m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen = m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth * m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight * 4;
		memcpy( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, pBuffer, m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen );

		//裁剪视频输入设备帧。
		if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop != 0 )
		{
			if( LibYUVPictrCrop( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight,
								 m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight,
								 m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								 NULL ) == 0 )
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：裁剪视频输入设备帧成功。" );
				m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：裁剪视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto out;
			}
		}
		
		//垂直翻转镜像视频输入设备帧。
		{
			if( LibYUVPictrMirror( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight,
								   1,
								   m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：垂直翻转镜像视频输入设备帧成功。" );
				m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：垂直翻转镜像视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto out;
			}
		}

		//缩放视频输入设备帧。
		if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsScale != 0 )
		{
			if( LibYUVPictrScale( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth, m_MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight,
								  3,
								  m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth, m_MediaProcThreadPt->m_VideoInput.m_FrameHeight,
								  NULL ) == 0 )
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：缩放视频输入设备帧成功。" );
				m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt; m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt = m_MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt; //交换视频输入结果帧和视频输入临时帧。
			}
			else
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：缩放视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto out;
			}
		}
		
		//预览视频输入设备帧。
		if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputPreviewWndHdl != NULL )
		{
			//水平翻转镜像视频输入设备帧。
			if( LibYUVPictrMirror( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth, m_MediaProcThreadPt->m_VideoInput.m_FrameHeight,
								   0,
								   m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt, m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen, NULL, NULL,
								   NULL ) == 0 )
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：水平翻转镜像视频输入设备帧成功。" );
			}
			else
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：水平翻转镜像视频输入设备帧失败，本次视频输入帧丢弃。" );
				goto out;
			}

			//绘制视频输入帧到视频输入预览窗口。
			if( LibYUVPictrDrawToWnd( m_MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth, m_MediaProcThreadPt->m_VideoInput.m_FrameHeight,
									  0,
									  m_MediaProcThreadPt->m_VideoInput.m_VideoInputPreviewWndHdl,
									  NULL ) == 0 )
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：绘制视频输入帧到视频输入预览窗口成功。" );
			}
			else
			{
				if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：绘制视频输入帧到视频输入预览窗口失败，本次视频输入帧丢弃。" );
				goto out;
			}
		}
		
		//获取一个视频输入空闲帧元素。
		ConstLenLnkLstGetTotal( m_MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal, NULL );
        if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal > 0 ) //如果视频输入空闲帧链表中有视频输入空闲帧元素。
        {
            //从视频输入空闲帧链表中取出第一个视频输入空闲帧元素。
            {
                ConstLenLnkLstGetHead( m_MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, NULL, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt, NULL, NULL );
				ConstLenLnkLstDelHead( m_MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, NULL );
            }
            if( m_MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFI( "视频输入线程：从视频输入空闲帧链表中取出第一个视频输入空闲帧元素，视频输入空闲帧链表元素个数：%" PRIuPTR "。", m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal );
        }
        else //如果视频输入空闲帧链表中没有视频输入空闲帧元素。
        {
			if( m_MediaProcThreadPt->m_IsPrintLog != 0 )  LOGE( "视频输入线程：视频输入空闲帧链表中没有视频输入空闲帧元素，本次视频输入帧丢弃。" );
			goto out;
        }

		//将BGRA格式视频输入帧转为YU12格式视频输入帧。
		if( LibYUVPictrFmtCnvrt( m_MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth, m_MediaProcThreadPt->m_VideoInput.m_FrameHeight,
								 m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth * m_MediaProcThreadPt->m_VideoInput.m_FrameHeight * 3 / 2, NULL, PICTR_FMT_BT601F8_YU12_I420,
								 NULL ) == 0 )
		{
			if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧成功。" );
		}
		else
		{
			if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：将BGRA格式视频输入帧转为YU12格式视频输入帧失败，本次视频输入帧丢弃。" );
			goto out;
		}
		/*FILE * p_File = fopen( "123.yuv", "wb+" );
		fwrite( p_VideoInputFrame.m_YU12VideoInputFramePt, 1, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth * m_MediaProcThreadPt->m_VideoInput.m_FrameHeight * 3 / 2, p_File );
		fclose( p_File );*/
		
        //判断视频输入是否黑屏。在视频输入处理完后再设置黑屏，这样可以保证视频输入处理器的连续性。
        if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputIsBlack != 0 )
        {
            int p_TmpLen = m_MediaProcThreadPt->m_VideoInput.m_FrameWidth * m_MediaProcThreadPt->m_VideoInput.m_FrameHeight;
            memset( m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, 0, p_TmpLen );
            memset( m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt + p_TmpLen, 128, p_TmpLen / 2 );
			if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：设置YU12格式视频输入帧为黑屏成功。" );
        }

		//使用编码器。
        switch( m_MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输入线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( OpenH264EncoderProc( m_MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderPt,
										 m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_YU12VideoInputFramePt, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth, m_MediaProcThreadPt->m_VideoInput.m_FrameHeight, m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec,
										 m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFramePt, m_MediaProcThreadPt->m_VideoInput.m_FrameWidth * m_MediaProcThreadPt->m_VideoInput.m_FrameHeight * 3 / 2, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFrameLen,
										 NULL ) == 0 )
                {
                    if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "视频输入线程：使用OpenH264编码器成功。H264格式视频输入帧的数据长度：%" PRIuPTR "，时间戳：%" PRIu64 "，类型：%hhd。", m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFrameLen, m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec, m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt->m_EncoderVideoInputFramePt[4] );
                }
                else
                {
                    if( m_MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输入线程：使用OpenH264编码器失败，本次视频输入帧丢弃。" );
					goto out;
                }
                break;
            }
        }

		//追加本次视频输入帧到视频输入帧链表。
		ConstLenLnkLstPutTail( m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt, NULL );
		m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt = NULL;

		if( m_MediaProcThreadPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &m_MediaProcThreadPt->m_VideoInput.m_NowTimeMsec );
			LOGFI( "视频输入线程：本次视频输入帧处理完毕，耗时 %" PRIu64 " 毫秒。", m_MediaProcThreadPt->m_VideoInput.m_NowTimeMsec - m_MediaProcThreadPt->m_VideoInput.m_LastTimeMsec );
		}

		out:
		if( m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt != NULL ) //如果获取的视频输入空闲帧没有追加到视频输入帧链表。
		{
			ConstLenLnkLstPutTail( m_MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, &m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt, NULL );
			m_MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt = NULL;
		}
		return 0;
	}
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VideoOutputThreadRun
 * 功能说明：视频输出线程主函数。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI VideoOutputThreadRun( MediaProcThread * MediaProcThreadPt )
{
	SetThreadPriority( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
	
    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输出线程：开始准备视频输出。" );

    //开始视频输出循环。
    while( true )
    {
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &MediaProcThreadPt->m_VideoOutput.m_LastTimeMsec );

		//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
		switch( MediaProcThreadPt->m_VideoOutput.m_UseWhatDecoder ) //使用什么解码器。
		{
			case 0: //如果使用YU12原始数据。
			{
				//调用用户定义的写入视频输出帧函数。
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth = 0;
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight = 0;
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen = MediaProcThreadPt->m_VideoOutput.m_FrameWidth * MediaProcThreadPt->m_VideoOutput.m_FrameHeight * 3 / 2;
				MediaProcThreadPt->m_UserWriteVideoOutputFrameFuncPt( MediaProcThreadPt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight, NULL, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen );
				
				if( ( MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth > 0 ) && ( MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight > 0 ) ) //如果本次写入了视频输出帧。
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：使用YU12原始数据成功。YU12格式帧宽度：%" PRId32 "，YU12格式帧高度：%" PRId32 "。", MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight );
                }
                else //如果本次没写入视频输出帧。
                {
                    goto skip;
                }
				break;
			}
			case 1: //如果使用OpenH264解码器。
			{
				//调用用户定义的写入视频输出帧函数。
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen = MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz;
				MediaProcThreadPt->m_UserWriteVideoOutputFrameFuncPt( MediaProcThreadPt, NULL, NULL, NULL, MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen );

				if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen > 0 ) //如果本次写入了视频输出帧。
				{
					//使用OpenH264解码器。
					if( OpenH264DecoderProc( MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderPt,
											 MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen,
											 MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight,
											 NULL ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：使用OpenH264解码器成功。已解码YU12格式帧宽度：%" PRId32 "，已解码YU12格式帧高度：%" PRId32 "。", MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight );
                        if( ( MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth == 0 ) || ( MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight == 0 ) ) goto skip; //如果未解码出YU12格式帧，就把本次视频输出帧丢弃。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "视频输出线程：使用OpenH264解码器失败，本次视频输出帧丢弃。" );
						goto skip;
					}
				}
				else
				{
					goto skip;
				}
				break;
			}
		}

		//用户定义的获取YU12格式视频输出帧函数。
		MediaProcThreadPt->m_UserGetYU12VideoOutputFrameFuncPt( MediaProcThreadPt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight );
		
        //判断视频输出设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
        if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputIsBlack != 0 )
        {
            int p_TmpLen = MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth * MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight;
            memset( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, 0, p_TmpLen );
            memset( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt + p_TmpLen, 128, p_TmpLen / 2 );
        }

		//缩放视频输出帧。
		if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale != 1.0f )
		{
			if( LibYUVPictrScale( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_BT601F8_YU12_I420, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight,
								  3,
								  MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz, NULL, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth * MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight * MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale,
								  NULL ) != 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输出线程：缩放视频输出帧失败，本次视频输出帧丢弃。" );
				goto skip;
			}
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt; MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt; MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt; //交换视频结果帧和视频临时帧。

			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth *= MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale;
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight *= MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayScale;
		}

		//将本次YU12格式视频输出帧转为BGRA格式视频输出帧。
		if( LibYUVPictrFmtCnvrt( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_BT601F8_YU12_I420, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight,
								 MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt, MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz, NULL, PICTR_FMT_SRGBF8_BGRA8888,
								 NULL ) != 0 )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 )  LOGFE( "视频输出线程：将本次YU12格式视频输出帧转为BGRA格式视频输出帧失败，本次视频输出帧丢弃。" );
			goto skip;
		}
		MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt; MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt; MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt = MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt; //交换视频结果帧和视频临时帧。

        //显示视频输出帧。
		if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayWndHdl != NULL )
		{
			//绘制视频输出帧到视频输出显示窗口。
			if( LibYUVPictrDrawToWnd( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt, PICTR_FMT_SRGBF8_BGRA8888, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth, MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight,
									  0,
									  MediaProcThreadPt->m_VideoOutput.m_VideoOutputDisplayWndHdl,
									  NULL ) != 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "视频输出线程：绘制视频输出帧到视频输出显示窗口失败，本次视频输出帧丢弃。" );
				goto skip;
			}
		}

		if( MediaProcThreadPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &MediaProcThreadPt->m_VideoOutput.m_NowTimeMsec );
			LOGFI( "视频输出线程：本次视频输出帧处理完毕，耗时 %" PRIu64 " 毫秒。", MediaProcThreadPt->m_VideoOutput.m_NowTimeMsec - MediaProcThreadPt->m_VideoOutput.m_LastTimeMsec );
		}

		skip:
        if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "视频输出线程：本线程接收到退出请求，开始准备退出。" );
            goto out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	out:
	if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "视频输出线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaProcThreadRun
 * 功能说明：媒体处理线程主函数。
 * 参数说明：MediaProcThreadPt：[输入]，存放媒体处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI MediaProcThreadRun( MediaProcThread * MediaProcThreadPt )
{
	SetThreadPriority( MediaProcThreadPt->m_MediaProcThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。

	int32_t p_TmpInt321;
	int32_t p_TmpInt322;
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;

	ReInit:
	MediaProcThreadPt->m_RunFlag = RUN_FLAG_INIT; //设置本线程运行标记为刚开始运行正在初始化。

	if( MediaProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

	if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本地代码的指令集名称为%s。退出标记：%d。", ( sizeof( size_t ) == 4 ) ? "x86" : "x64", MediaProcThreadPt->m_ExitFlag );

	if( MediaProcThreadPt->m_ExitFlag != 3 ) //如果需要执行用户定义的初始化函数。
	{
		MediaProcThreadPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
		MediaProcThreadPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。

		//调用用户定义的初始化函数。
		p_TmpInt321 = MediaProcThreadPt->m_UserInitFuncPt( MediaProcThreadPt );
		if( p_TmpInt321 == 0 )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的初始化函数成功。返回值：%d。", p_TmpInt321 );
		}
		else
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：调用用户定义的初始化函数失败。返回值：%d。", p_TmpInt321 );
			goto out;
		}
	}
	else
    {
		MediaProcThreadPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
		MediaProcThreadPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。
    }

	//保存设置到文件。
	if( MediaProcThreadPt->m_IsSaveSettingToFile != 0 )
	{
		FILE * p_SettingFileStreamPt;

		p_SettingFileStreamPt = fopen( MediaProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt, "wb+" );
		if( p_SettingFileStreamPt == NULL )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开设置文件 %s 失败。原因：%s", MediaProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt, FuncGetErrInfo( errno, 'E' ) );
			goto out;
		}
		fprintf( p_SettingFileStreamPt, "m_IsSaveSettingToFile：%" PRId32 "\n", MediaProcThreadPt->m_IsSaveSettingToFile );
		fprintf( p_SettingFileStreamPt, "m_SettingFileFullPathVarStrPt->m_StrPt：%s\n", ( MediaProcThreadPt->m_SettingFileFullPathVarStrPt != NULL ) ? MediaProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt : NULL );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_IsPrintLog：%" PRId32 "\n", MediaProcThreadPt->m_IsPrintLog );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_IsUseAudioInput：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SamplingRate：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SamplingRate );
        fprintf( p_SettingFileStreamPt, "m_AudioInput.m_FrameLen：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_FrameLen );
        fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_UseWhatAec：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_UseWhatAec );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecFilterLen：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecFilterLen );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecIsUseRec：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecIsUseRec );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecEchoMutp：%f\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoMutp );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecEchoCntu：%f\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoCntu );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecEchoSupes：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupes );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecEchoSupesAct：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupesAct );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecIsSaveMemFile：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecIsSaveMemFile );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt：%s\n", MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecmIsUseCNGMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecmEchoMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecmEchoMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecmDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecmDelay );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecEchoMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecEchoMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecDelay );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseExtdFilterMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecIsSaveMemFile：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsSaveMemFile );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt：%s\n", MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWorkMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWorkMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp：%f\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu：%f\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_UseWhatNs：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_UseWhatNs );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocIsUseNs：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseNs );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocNoiseSupes：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocNoiseSupes );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocIsUseDereverb：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseDereverb );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcNsxPolicyMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPolicyMode );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_WebRtcNsPolicyMode：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_WebRtcNsPolicyMode );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_IsUseSpeexPprocOther：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocIsUseVad：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseVad );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocVadProbStart：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbStart );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocVadProbCntu：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbCntu );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocIsUseAgc：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseAgc );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocAgcLevel：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcLevel );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocAgcIncrement：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcIncrement );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocAgcDecrement：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcDecrement );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexPprocAgcMaxGain：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcMaxGain );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_UseWhatEncoder：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexEncoderUseCbrOrVbr：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexEncoderQuality：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexEncoderQuality );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexEncoderComplexity：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexEncoderComplexity );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_SpeexEncoderPlcExpectedLossRate：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_IsSaveAudioToFile：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt：%s\n", MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt：%s\n", MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_AudioInputDeviceID：%u\n", MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceID );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_AudioInputWaveHdrTotal：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal );
		fprintf( p_SettingFileStreamPt, "m_AudioInput.m_AudioInputIsMute：%" PRId32 "\n", MediaProcThreadPt->m_AudioInput.m_AudioInputIsMute );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_IsUseAudioOutput：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_SamplingRate：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_SamplingRate );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_FrameLen：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_FrameLen );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_UseWhatDecoder：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_IsSaveAudioToFile：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_IsSaveAudioToFile );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt：%s\n", MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
		fprintf( p_SettingFileStreamPt, "\n" );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_AudioOutputDeviceID：%u\n", MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceID );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_AudioOutputWaveHdrTotal：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal );
		fprintf( p_SettingFileStreamPt, "m_AudioOutput.m_AudioOutputIsMute：%" PRId32 "\n", MediaProcThreadPt->m_AudioOutput.m_AudioOutputIsMute );
		fclose( p_SettingFileStreamPt );
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：保存设置到文件 %s 成功。", MediaProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt );
	}

	//初始化音频输入。
	if( MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput != 0 )
	{
		//创建并初始化声学回音消除器。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatAec )
		{
			case 0: //如果不使用声学回音消除器。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用声学回音消除器。" );
				break;
			}
			case 1: //如果使用Speex声学回音消除器。
			{
				if( ( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) && ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate == MediaProcThreadPt->m_AudioInput.m_SamplingRate ) && ( MediaProcThreadPt->m_AudioOutput.m_FrameLen == MediaProcThreadPt->m_AudioInput.m_FrameLen ) ) //如果要使用音频输出，且音频输出的采样频率和帧的数据长度与音频输入一致。
                {
					if( MediaProcThreadPt->m_AudioInput.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
					{
						if( SpeexAecInitByMemFile( &MediaProcThreadPt->m_AudioInput.m_SpeexAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecFilterLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecIsUseRec, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoMutp, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoCntu, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupes, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupesAct, MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器成功。", MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器失败。原因：%s", MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt == NULL )
					{
						if( SpeexAecInit( &MediaProcThreadPt->m_AudioInput.m_SpeexAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecFilterLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecIsUseRec, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoMutp, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoCntu, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupes, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupesAct ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex声学回音消除器成功。" );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex声学回音消除器失败。" );
							goto out;
						}
					}
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：不使用音频输出、或音频输出的采样频率或帧的数据长度与音频输入不一致，不能使用声学回音消除器。" );
                }
				break;
			}
			case 2: //如果使用WebRtc定点版声学回音消除器。
			{
				if( ( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) && ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate == MediaProcThreadPt->m_AudioInput.m_SamplingRate ) && ( MediaProcThreadPt->m_AudioOutput.m_FrameLen == MediaProcThreadPt->m_AudioInput.m_FrameLen ) ) //如果要使用音频输出，且音频输出的采样频率和帧的数据长度与音频输入一致。
                {
					if( WebRtcAecmInit( &MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcAecmIsUseCNGMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecmEchoMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecmDelay ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc定点版声学回音消除器失败。" );
						goto out;
					}
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：不使用音频输出、或音频输出的采样频率或帧的数据长度与音频输入不一致，不能使用声学回音消除器。" );
                }
				break;
			}
			case 3: //如果使用WebRtc浮点版声学回音消除器。
			{
				if( ( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) && ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate == MediaProcThreadPt->m_AudioInput.m_SamplingRate ) && ( MediaProcThreadPt->m_AudioOutput.m_FrameLen == MediaProcThreadPt->m_AudioInput.m_FrameLen ) ) //如果要使用音频输出，且音频输出的采样频率和帧的数据长度与音频输入一致。
                {
					if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
					{
						if( WebRtcAecInitByMemFile( &MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcAecEchoMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecDelay, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay, MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器成功。", MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器失败。原因：%s", MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt == NULL )
					{
						if( WebRtcAecInit( &MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcAecEchoMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecDelay, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器成功。" );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc浮点版声学回音消除器失败。" );
							goto out;
						}
					}
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：不使用音频输出、或音频输出的采样频率或帧的数据长度与音频输入不一致，不能使用声学回音消除器。" );
                }
				break;
			}
			case 4: //如果使用SpeexWebRtc三重声学回音消除器。
			{
				if( ( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) && ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate == MediaProcThreadPt->m_AudioInput.m_SamplingRate ) && ( MediaProcThreadPt->m_AudioOutput.m_FrameLen == MediaProcThreadPt->m_AudioInput.m_FrameLen ) ) //如果要使用音频输出，且音频输出的采样频率和帧的数据长度与音频输入一致。
                {
					if( SpeexWebRtcAecInit( &MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWorkMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecFilterLen, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecIsUseRec, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoMutp, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoCntu, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupes, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSpeexAecEchoSupesAct, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmEchoMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecmDelay, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecEchoMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecDelay, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecIsUseSameRoomAec, MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecSameRoomEchoMinDelay ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化SpeexWebRtc三重声学回音消除器失败。" );
						goto out;
					}
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：不使用音频输出、或音频输出的采样频率或帧的数据长度与音频输入不一致，不能使用声学回音消除器。" );
                }
				break;
			}
		}

		//创建并初始化噪音抑制器对象。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatNs )
		{
			case 0: //如果不使用噪音抑制器。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用噪音抑制器。" );
				break;
			}
			case 1: //如果使用Speex预处理器的噪音抑制。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：稍后在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" );
				break;
			}
			case 2: //如果使用WebRtc定点版噪音抑制器。
			{
				if( WebRtcNsxInit( &MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPolicyMode ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc定点版噪音抑制器失败。" );
					goto out;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版噪音抑制器。
			{
				if( WebRtcNsInit( &MediaProcThreadPt->m_AudioInput.m_WebRtcNsPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcNsPolicyMode ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化WebRtc浮点版噪音抑制器失败。" );
					goto out;
				}
				break;
			}
			case 4: //如果使用RNNoise噪音抑制器。
			{
				if( RNNoiseInit( &MediaProcThreadPt->m_AudioInput.m_RNNoisePt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化RNNoise噪音抑制器成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化RNNoise噪音抑制器失败。" );
					goto out;
				}
				break;
			}
		}

		//创建并初始化Speex预处理器。
		if( ( MediaProcThreadPt->m_AudioInput.m_UseWhatNs == 1 ) || ( MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther != 0 ) )
		{
			if( MediaProcThreadPt->m_AudioInput.m_UseWhatNs != 1 )
			{
				MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseNs = 0;
				MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseDereverb = 0;
			}
			if( MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther == 0 )
			{
				MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseVad = 0;
				MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseAgc = 0;
			}
			if( SpeexPprocInit( &MediaProcThreadPt->m_AudioInput.m_SpeexPprocPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseNs, MediaProcThreadPt->m_AudioInput.m_SpeexPprocNoiseSupes, MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseDereverb, MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseVad, MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbStart, MediaProcThreadPt->m_AudioInput.m_SpeexPprocVadProbCntu, MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseAgc, MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcLevel, MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcIncrement, MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcDecrement, MediaProcThreadPt->m_AudioInput.m_SpeexPprocAgcMaxGain ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex预处理器成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex预处理器失败。" );
				goto out;
			}
		}

		//创建并初始化编码器。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
				break;
			}
			case 1: //如果使用Speex编码器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_FrameLen != MediaProcThreadPt->m_AudioInput.m_SamplingRate / 1000 * 20 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：帧的数据长度不为20毫秒不能使用Speex编码器。" );
					goto out;
				}
				if( SpeexEncoderInit( &MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_SpeexEncoderUseCbrOrVbr, MediaProcThreadPt->m_AudioInput.m_SpeexEncoderQuality, MediaProcThreadPt->m_AudioInput.m_SpeexEncoderComplexity, MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPlcExpectedLossRate ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex编码器成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex编码器失败。" );
					goto out;
				}
				break;
			}
			case 2: //如果使用Opus编码器。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus编码器。" );
				goto out;
			}
		}

		//创建并初始化音频输入Wave文件写入器、音频结果Wave文件写入器。
		if( MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile != 0 )
		{
			if( WaveFileWriterInit( &MediaProcThreadPt->m_AudioInput.m_AudioInputWaveFileWriterPt, MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt, 1, MediaProcThreadPt->m_AudioInput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器成功。", MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器失败。", MediaProcThreadPt->m_AudioInput.m_AudioInputFileFullPathVarStrPt->m_StrPt );
				goto out;
			}
			if( WaveFileWriterInit( &MediaProcThreadPt->m_AudioInput.m_AudioResultWaveFileWriterPt, MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt, 1, MediaProcThreadPt->m_AudioInput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器成功。", MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器失败。", MediaProcThreadPt->m_AudioInput.m_AudioResultFileFullPathVarStrPt->m_StrPt );
				goto out;
			}
		}
		
		//创建并初始化音频输入帧链表、音频输入空闲帧链表。
		{
			MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal = 6; //音频输入帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输入帧，从而导致不同步。音频输入空闲帧链表最多存储总数与音频输入帧链表一致。
			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal, MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输入帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			//创建并初始化音频输入空闲帧，并追加到音频输入空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioInput.m_AudioInputFrameElmTotal; p_TmpInt322++ )
			{
				MediaProcThreadPt->m_PcmAudioInputFramePt = ( int16_t * )malloc( MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( int16_t ) );
				if( MediaProcThreadPt->m_PcmAudioInputFramePt == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化音频输入空闲帧失败。" );
					goto out; //本函数返回失败。
				}
				if( ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, &MediaProcThreadPt->m_PcmAudioInputFramePt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioInputFramePt );
					MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加音频输入空闲帧到音频输入空闲帧链表失败。" );
					goto out; //本函数返回失败。
				}
				MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
			}
		}

		//初始化音频输入线程的临时变量。
		{
			MediaProcThreadPt->m_AudioInput.m_Msg = { 0 }; //初始化消息。
			MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt = NULL; //初始化音频输入帧的内存指针。
			MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal = 0; //初始化音频输入帧链表的元数总数。
			MediaProcThreadPt->m_AudioInput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaProcThreadPt->m_AudioInput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化音频输入线程的临时变量成功。" );
		}

		//创建并启动音频输入线程。必须在初始化音频输入设备前初始化音频输入线程，因为音频输入设备会使用音频输入线程ID。
		{
			MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioInputThreadRun, MediaProcThreadPt, 0, &MediaProcThreadPt->m_AudioInput.m_AudioInputThreadId );
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl != NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动音频输入线程成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动音频输入线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
				goto out; //本函数返回失败。
			}
		}

		//创建并初始化音频输入缓冲区块数组。
		{
			MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal = 2; //音频输入缓冲区块数组的元素总数固定为2。
			MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal * sizeof( WAVEHDR ) ); //创建并清空音频输入缓冲区块数组的内存块。
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输入缓冲区块数组的内存块失败。" );
				goto out; //本函数返回失败。
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwUser = p_TmpInt322;
				MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwBufferLength = MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( int16_t );
				MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].dwBufferLength );
				if( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输入缓冲区块数组的数据内存块失败。" );
					goto out; //本函数返回失败。
				}
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入缓冲区块数组成功。" );
		}

		//创建并初始化音频输入设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = MediaProcThreadPt->m_AudioInput.m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = MediaProcThreadPt->m_AudioInput.m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt321 = waveInOpen( &MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceID, &WaveFormatEx, ( DWORD_PTR )MediaProcThreadPt->m_AudioInput.m_AudioInputThreadId, ( DWORD_PTR )MediaProcThreadPt, CALLBACK_THREAD );
			if( p_TmpInt321 != MMSYSERR_NOERROR )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开音频输入设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				if( MediaProcThreadPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：打开音频输入设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				goto out;
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				p_TmpInt321 = waveInPrepareHeader( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, &MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：为音频输入设备准备音频输入缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto out;
				}
				p_TmpInt321 = waveInAddBuffer( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, &MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将已准备的音频输入缓冲区块添加到音频输入设备上失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto out;
				}
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输入设备成功。音频输入缓冲区大小：%d * %d。", MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[0].dwBufferLength, MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal );
		}
	} //初始化音频输入完毕。

	//初始化音频输出。
	if( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 )
	{
		//创建并初始化解码器。
		switch( MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
				break;
			}
			case 1: //如果使用Speex解码器。
			{
				if( MediaProcThreadPt->m_AudioOutput.m_FrameLen != MediaProcThreadPt->m_AudioOutput.m_SamplingRate / 1000 * 20 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：帧的数据长度不为20毫秒不能使用Speex解码器。" );
					goto out;
				}
				if( SpeexDecoderInit( &MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderPt, MediaProcThreadPt->m_AudioOutput.m_SamplingRate, MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderIsUsePerceptualEnhancement ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化Speex解码器成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化Speex解码器失败。" );
					goto out;
				}
				break;
			}
			case 2: //如果使用Opus解码器。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus解码器。" );
				goto out;
			}
		}

		//创建并初始化音频输出Wave文件写入器。
		if( MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile != 0 )
		{
			if( WaveFileWriterInit( &MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt, 1, MediaProcThreadPt->m_AudioOutput.m_SamplingRate, 16 ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器成功。", MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器失败。", MediaProcThreadPt->m_AudioOutput.m_AudioOutputFileFullPathVarStrPt->m_StrPt );
				goto out;
			}
		}
		
		//创建并初始化音频输出帧链表、音频输出空闲帧链表。
		{
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal = 6; //音频输出帧链表最多只存储几帧，避免因为音频设备有变化导致卡顿并积累大量音频输出帧，从而导致不同步。音频输出空闲帧链表最多存储总数与音频输出帧链表一致。
			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, sizeof( int16_t * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal, MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出空闲帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化音频输出空闲帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			//创建并初始化音频输出空闲帧，并追加到音频输出空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameElmTotal; p_TmpInt322++ )
			{
				MediaProcThreadPt->m_PcmAudioOutputFramePt = ( int16_t * )malloc( MediaProcThreadPt->m_AudioOutput.m_FrameLen * sizeof( int16_t ) );
				if( MediaProcThreadPt->m_PcmAudioOutputFramePt == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化音频输出空闲帧失败。" );
					goto out; //本函数返回失败。
				}
				if( ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, &MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioOutputFramePt );
					MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加音频输入空闲帧到音频输出空闲帧链表失败。" );
					goto out; //本函数返回失败。
				}
				MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
			}
		}
		
		//初始化音频输出线程的临时变量。
		{
			MediaProcThreadPt->m_AudioOutput.m_Msg = { 0 }; //初始化消息。
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt = NULL; //初始化音频输出帧的内存指针。
			if( MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder != 0 )
			{
				MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_AudioOutput.m_FrameLen * sizeof( uint8_t ) ); //初始化已编码格式音频输出帧的内存指针。
				if( MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建已编码格式音频输出帧的内存块失败。" );
					goto out;
				}
				MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = MediaProcThreadPt->m_AudioOutput.m_FrameLen * sizeof( uint8_t ); //初始化已编码格式音频输出帧的内存大小，单位字节。
			}
			else
			{
				MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt = NULL; //初始化已编码格式音频输出帧的内存指针。
				MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = 0; //初始化已编码格式音频输出帧的内存大小，单位字节。
			}
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen = 0; //初始化音频输出帧的数据长度，单位字节。
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal = 0; //初始化音频输出帧链表的元数总数。
			MediaProcThreadPt->m_AudioOutput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaProcThreadPt->m_AudioOutput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化音频输出线程的临时变量成功。" );
		}

		//创建并启动音频输出线程。必须在初始化音频输出设备前初始化音频输出线程，因为音频输出设备会使用音频输出线程ID。
		MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioOutputThreadRun, MediaProcThreadPt, 0, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadId );
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl != NULL )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动音频输出线程成功。" );
		}
		else
		{
			if( MediaProcThreadPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动音频输出线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto out; //本函数返回失败。
		}

		//创建并初始化音频输出缓冲区块数组。
		{
			if( MediaProcThreadPt->m_AudioOutput.m_FrameLen / ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate / 1000 ) == 10 ) //如果帧的数据长度为10毫秒。
			{
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 4;
			}
			else if( MediaProcThreadPt->m_AudioOutput.m_FrameLen / ( MediaProcThreadPt->m_AudioOutput.m_SamplingRate / 1000 ) == 20 ) //如果帧的数据长度为20毫秒。
			{
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 3;
			}
			else //如果帧的数据长度为大于等于30毫秒。
			{
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 2;
			}
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt = ( WAVEHDR * )calloc( 1, MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal * sizeof( WAVEHDR ) );
			if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输出缓冲区块数组的内存块失败。" );
				goto out; //本函数返回失败。
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].dwBufferLength = MediaProcThreadPt->m_AudioOutput.m_FrameLen * sizeof( int16_t );
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData = ( LPSTR )calloc( 1, MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].dwBufferLength );
				if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并清空音频输出缓冲区块数组的数据内存块失败。" );
					goto out; //本函数返回失败。
				}
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出缓冲区块成功。" );
		}

		//创建并初始化音频输出设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = MediaProcThreadPt->m_AudioOutput.m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = MediaProcThreadPt->m_AudioOutput.m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt321 = waveOutOpen( &MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl, MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceID, &WaveFormatEx, ( DWORD_PTR )MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadId, ( DWORD_PTR )MediaProcThreadPt, CALLBACK_THREAD );
			if( p_TmpInt321 != MMSYSERR_NOERROR )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：打开音频输出设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				if( MediaProcThreadPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：打开音频输出设备失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
				goto out;
			}
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				p_TmpInt321 = waveOutPrepareHeader( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322] ) );
				if( p_TmpInt321 != MMSYSERR_NOERROR )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：为音频输出设备准备音频输出缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt321, 'M' ) );
					goto out;
				}
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化音频输出设备成功。音频输出缓冲区大小：%d * %d。", MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[0].dwBufferLength, MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal );
		}
	} //初始化音频输出完毕。

	//自适应设置回音的延迟。
	if( MediaProcThreadPt->m_AudioInput.m_UseWhatAec != 0 ) //如果要使用音频输入的声学回音消除，就自适应计算声学回音的延迟，并设置到声学回音消除器。
	{
		int32_t p_Delay = ( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal + MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal ) * ( MediaProcThreadPt->m_AudioInput.m_FrameLen / ( MediaProcThreadPt->m_AudioInput.m_SamplingRate / 1000 ) );
		if( ( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmGetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			p_TmpInt321 = p_Delay;
			WebRtcAecmSetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt, p_TmpInt321 );
			WebRtcAecmGetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt, &p_TmpInt321 );
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
		}
		if( ( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt != NULL ) && ( WebRtcAecGetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
        {
            if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt321 = p_Delay;
                WebRtcAecSetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, p_TmpInt321 );
                WebRtcAecGetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 );
            }
            else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                WebRtcAecSetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, 20 );
                WebRtcAecGetDelay( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, &p_TmpInt321 );
            }
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
		if( ( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
        {
			p_TmpInt321 = p_Delay;
            SpeexWebRtcAecSetWebRtcAecmDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, p_TmpInt321 );
            SpeexWebRtcAecGetWebRtcAecmDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
		if( ( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 ) == 0 ) && ( p_TmpInt321 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
        {
            if( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt321 = p_Delay;
                SpeexWebRtcAecSetWebRtcAecDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, p_TmpInt321 );
                SpeexWebRtcAecGetWebRtcAecDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            }
            else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                SpeexWebRtcAecSetWebRtcAecDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, 20 );
                SpeexWebRtcAecGetWebRtcAecDelay( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, &p_TmpInt321 );
            }
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt321 );
        }
	} //自适应设置回音的延迟完毕。

	//初始化视频输入。
	if( MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput != 0 )
	{
		//创建并初始化视频输入线程。必须在初始化视频输入设备前初始化视频输入线程，因为视频输入设备会使用视频输入线程的内存指针。
		{
			MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt = new VideoInputThread( MediaProcThreadPt );
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt != NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入线程成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入线程失败。" );
				goto out;
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

			IBaseFilter * p_VideoInputDeviceFilterPt = NULL; //存放视频输入设备过滤器的内存指针。
			IPin * p_SelPinPt = NULL;
			AM_MEDIA_TYPE * p_SelAmMediaTypePt = NULL;
			int32_t p_AvgTimePerFrame = 1000.0 / ( MediaProcThreadPt->m_VideoInput.m_MaxSamplingRate / 10.0 / 1000.0 );
			double p_FrameWidthToHeightRatio = ( double )MediaProcThreadPt->m_VideoInput.m_FrameWidth / ( double )MediaProcThreadPt->m_VideoInput.m_FrameHeight; //存放指定帧的宽高比。
            double p_VideoInputDeviceFrameWidthToHeightRatio = 0; //存放本次视频输入设备帧的宽高比。
            int32_t p_VideoInputDeviceFrameCropWidth = 0; //存放本次视频输入设备帧的裁剪宽度。
            int32_t p_VideoInputDeviceFrameCropHeight = 0; //存放本次视频输入设备帧的裁剪高度。

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
			if( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, ( void ** )&MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器失败。" );
				goto outInitVideoInputDevice;
			}
			
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->QueryInterface( IID_IMediaEvent, ( LPVOID * )&MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器媒体事件器失败。" );
				goto outInitVideoInputDevice;
			}

			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->QueryInterface( IID_IMediaControl, ( LPVOID * )&MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入过滤器图管理器媒体控制器失败。" );
				goto outInitVideoInputDevice;
			}

			//创建系统设备枚举器。
			if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, ( void ** )&p_CreateDevEnumPt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建系统设备枚举器失败。" );
				goto outInitVideoInputDevice;
			}

			//查找并创建指定的视频输入设备过滤器。
			if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
			{
				for( p_TmpInt322 = 0; p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK; p_TmpInt322++ )
				{
					if( ( p_TmpInt322 == MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceID ) || ( ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceID == WAVE_MAPPER ) && ( p_TmpInt322 == 0 ) ) )
					{
						if( p_MonikerPt->BindToObject( NULL, NULL, IID_IBaseFilter, ( void * * )&p_VideoInputDeviceFilterPt ) == S_OK )
						{
							p_TmpSz1 = p_MonikerPt->Release();
							p_MonikerPt = NULL;
							break;
						}
						else
						{
							p_TmpSz1 = p_MonikerPt->Release();
							p_MonikerPt = NULL;
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：根据Moniker创建视频输入设备过滤器失败。" );
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
			if( p_VideoInputDeviceFilterPt == NULL ) //如果创建视频输入设备失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" );
				if( MediaProcThreadPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：创建视频输入设备过滤器失败。原因：可能没有视频输入设备。" );
				goto outInitVideoInputDevice;
			}
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_VideoInputDeviceFilterPt, L"Video Capture" ) != S_OK ) //如果添加视频输入设备过滤器到视频输入过滤器图失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加视频输入设备过滤器到视频输入过滤器图失败。" );
				goto outInitVideoInputDevice;
			}

			//选择视频输入设备过滤器上合适的引脚和媒体类型。
			if( p_VideoInputDeviceFilterPt->EnumPins( &p_EnumPinsPt ) == S_OK ) //创建引脚枚举器。
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

									if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备支持的媒体格式：subtype：%s  AvgTimePerFrame：%lld  %.1f  Width：%ld  Height：%ld。",
																					  ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
																					  VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ) / 10.0 / 1000.0 ),
																					  VIDEOINFOHEADER_Width( p_AmMediaTypePt ), VIDEOINFOHEADER_Height( p_AmMediaTypePt ) );
									
									if( ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) || ( p_AmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ) //如果媒体格式为MJPEG或YUY2才进行选择。
									{
										//设置本次视频输入设备帧的宽高比、裁剪宽度、裁剪高度。
										p_VideoInputDeviceFrameWidthToHeightRatio = ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt );
										if( p_VideoInputDeviceFrameWidthToHeightRatio >= p_FrameWidthToHeightRatio )
										{
											p_VideoInputDeviceFrameCropWidth = ( int )( ( double )VIDEOINFOHEADER_Height( p_AmMediaTypePt ) * p_FrameWidthToHeightRatio );
											p_VideoInputDeviceFrameCropWidth -= p_VideoInputDeviceFrameCropWidth % 2;
											p_VideoInputDeviceFrameCropHeight = VIDEOINFOHEADER_Height( p_AmMediaTypePt );
										}
										else
										{
											p_VideoInputDeviceFrameCropWidth = VIDEOINFOHEADER_Width( p_AmMediaTypePt );
											p_VideoInputDeviceFrameCropHeight = ( int )( ( double )VIDEOINFOHEADER_Width( p_AmMediaTypePt ) / p_FrameWidthToHeightRatio );
											p_VideoInputDeviceFrameCropHeight -= p_VideoInputDeviceFrameCropHeight % 2;
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
										else if( ( ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth < MediaProcThreadPt->m_VideoInput.m_FrameWidth ) || ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight < MediaProcThreadPt->m_VideoInput.m_FrameHeight ) ) && ( ( p_VideoInputDeviceFrameCropWidth > MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) || ( p_VideoInputDeviceFrameCropHeight > MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) ) )
										{
											p_TmpInt321 = 1;
										}
										else if( ( ( p_VideoInputDeviceFrameCropWidth >= MediaProcThreadPt->m_VideoInput.m_FrameWidth ) || ( p_VideoInputDeviceFrameCropHeight >= MediaProcThreadPt->m_VideoInput.m_FrameHeight ) ) || ( ( p_VideoInputDeviceFrameCropWidth == MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) && ( p_VideoInputDeviceFrameCropHeight == MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) ) )
										{
											if( ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) > p_AvgTimePerFrame ) && ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, < , p_SelAmMediaTypePt ) ) )
											{
												p_TmpInt321 = 1;
											}
											else if( ( VIDEOINFOHEADER_AvgTimePerFrame( p_AmMediaTypePt ) <= p_AvgTimePerFrame ) || ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
											{
												if( ( ( p_VideoInputDeviceFrameCropWidth < MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) || ( p_VideoInputDeviceFrameCropHeight < MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) ) )
												{
													p_TmpInt321 = 1;
												}
												else if( ( ( p_VideoInputDeviceFrameCropWidth == MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) && ( p_VideoInputDeviceFrameCropHeight == MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) ) && ( VIDEOINFOHEADER_AvgTimePerFrameCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
												{
													p_TmpInt321 = 1;
												}
											}
										}
									}
									else
									{
										if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：本次视频输入设备支持的媒体格式不是为MJPEG或YUY2，不能被选择。" );
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

										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceMaxSamplingRate = 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth = VIDEOINFOHEADER_Width( p_SelAmMediaTypePt );
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight = VIDEOINFOHEADER_Height( p_SelAmMediaTypePt );

										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth = p_VideoInputDeviceFrameCropWidth;
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight = p_VideoInputDeviceFrameCropHeight;
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
									else if( ( ( VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt ) < MediaProcThreadPt->m_VideoInput.m_FrameWidth ) || ( VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt ) < MediaProcThreadPt->m_VideoInput.m_FrameHeight ) ) && ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, > , p_SelAmMediaTypePt ) ) )
									{
										p_TmpInt321 = 1;
									}
									else if( ( ( VIDEOINFOHEADER2_Width( p_AmMediaTypePt ) >= MediaProcThreadPt->m_VideoInput.m_FrameWidth ) && ( VIDEOINFOHEADER2_Height( p_AmMediaTypePt ) >= MediaProcThreadPt->m_VideoInput.m_FrameHeight ) ) || ( VIDEOINFOHEADER2_WidthHeightCom( p_AmMediaTypePt, == , p_SelAmMediaTypePt ) ) )
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
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceMaxSamplingRate = 1000.0 / ( VIDEOINFOHEADER2_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 );
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth = VIDEOINFOHEADER2_Width( p_SelAmMediaTypePt );
										MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight = VIDEOINFOHEADER2_Height( p_SelAmMediaTypePt );
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
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" );
				if( MediaProcThreadPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, "媒体处理线程：选择视频输入设备过滤器上合适的引脚和媒体类型失败。" );
				goto outInitVideoInputDevice;
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备选择的媒体格式：subtype：%s  AvgTimePerFrame：%lld  %.1f  Width：%ld  Height：%ld。",
															  ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG ) ? "MJPEG" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 ) ? "YUY2" : ( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_RGB24 ) ? "RGB24" : "unkown",
															  VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ), 1000.0 / ( VIDEOINFOHEADER_AvgTimePerFrame( p_SelAmMediaTypePt ) / 10.0 / 1000.0 ),
															  VIDEOINFOHEADER_Width( p_SelAmMediaTypePt ), VIDEOINFOHEADER_Height( p_SelAmMediaTypePt ) );
			
            //判断视频输入设备帧是否裁剪。
            if( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth > MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) //如果视频输入设备帧的宽度比裁剪宽度大，就表示需要裁剪宽度。
            {
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop = 1; //设置视频输入设备帧要裁剪。

                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX = ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth - MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth ) / 2; //设置视频输入设备帧裁剪区域左上角的横坐标，使裁剪区域居中。
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
            }
            else if( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight > MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) //如果视频输入设备帧的高度比裁剪高度大，就表示需要裁剪高度。
            {
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop = 1; //设置视频输入设备帧要裁剪。

                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY = ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight - MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight ) / 2; //设置视频输入设备帧裁剪区域左上角的纵坐标，使裁剪区域居中。
            }
            else //如果视频输入设备帧的宽度和高度与裁剪宽度和高度一致，就表示不需要裁剪。
            {
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop = 0; //设置视频输入设备帧不裁剪。

                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX = 0; //设置视频输入设备帧裁剪区域左上角的横坐标。
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY = 0; //设置视频输入设备帧裁剪区域左上角的纵坐标。
            }
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备帧是否裁剪：%" PRIi32 "  左上角的横坐标：%" PRIi32 "  纵坐标：%" PRIi32 "  裁剪区域的宽度：%" PRIi32 "  高度：%" PRIi32 "。", MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop, MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX, MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY, MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth, MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight );

            //判断视频输入设备帧是否缩放。
            if( ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth != MediaProcThreadPt->m_VideoInput.m_FrameWidth ) || ( MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight != MediaProcThreadPt->m_VideoInput.m_FrameHeight ) )
            {
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsScale = 1; //设置视频输入设备帧要缩放。
            }
            else
            {
                MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsScale = 0; //设置视频输入设备帧不缩放。
            }
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：视频输入设备帧是否缩放：%" PRIi32 "。", MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsScale );

			//创建视频输入设备过滤器上选择的引脚和媒体类型对应的解码过滤器，并连接引脚。
			if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_MJPG )
			{
				if( CoCreateInstance( CLSID_MjpegDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建MJPG解码过滤器失败。" );
					goto outInitVideoInputDevice;
				}
			}
			else if( p_SelAmMediaTypePt->subtype == MEDIASUBTYPE_YUY2 )
			{
				if( CoCreateInstance( CLSID_AVIDec, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void ** )&p_DecFilterPt ) != S_OK )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建AVI解码过滤器失败。" );
					goto outInitVideoInputDevice;
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
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_DecFilterPt, L"Decompressor" ) != S_OK ) //如果添加解码过滤器到视频输入过滤器图失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加解码过滤器到视频输入过滤器图失败。" );
				goto outInitVideoInputDevice;
			}
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->ConnectDirect( p_SelPinPt, p_DecFilterInputPinPt, p_SelAmMediaTypePt ) != S_OK ) //如果连接引脚失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。" );
				if( MediaProcThreadPt->m_IsShowToast != 0 ) FuncToastFmt( NULL, 3000, NULL, "媒体处理线程：连接视频输入设备过滤器与解码过滤器的引脚失败。原因：可能是视频输入设备无法正常工作。" );
				goto outInitVideoInputDevice;
			}

			//创建采样抓取过滤器，并连接引脚。
			if( CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, ( void * * )&p_SampleGrabberFilterPt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建采样抓取过滤器失败。" );
				goto outInitVideoInputDevice;
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
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->AddFilter( p_SampleGrabberFilterPt, L"Sample Grabber" ) != S_OK ) //如果采样抓取过滤器到视频输入过滤器图失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：添加采样抓取过滤器到视频输入过滤器图失败。" );
				goto outInitVideoInputDevice;
			}
			p_TmpAmMediaType.majortype = MEDIATYPE_Video;
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB555; //PICTR_FMT_SRGBF8_BGR555。
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB565; //PICTR_FMT_SRGBF8_BGR565。
			//p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB24; //PICTR_FMT_SRGBF8_BGR888。
			p_TmpAmMediaType.subtype = MEDIASUBTYPE_RGB32; //PICTR_FMT_SRGBF8_BGRA8888。
			if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->ConnectDirect( p_DecFilterOutputPinPt, p_SampleGrabberFilterInputPinPt, &p_TmpAmMediaType ) != S_OK ) //如果连接引脚失败。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：连接解码过滤器与采样抓取过滤器的引脚失败。" );
				goto outInitVideoInputDevice;
			}

			//设置采样抓取过滤器的回调函数。
			if( p_SampleGrabberFilterPt->QueryInterface( IID_ISampleGrabber, ( void * * )&p_SampleGrabberInterfacePt ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：获取采样抓取过滤器的回调接口失败。" );
				goto outInitVideoInputDevice;
			}
			if( p_SampleGrabberInterfacePt->SetCallback( MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt, 1 ) != S_OK )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：设置采样抓取过滤器的回调函数失败。" );
				goto outInitVideoInputDevice;
			}

			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入设备成功。" );

			p_Result = 0; //设置本函数执行成功。

			outInitVideoInputDevice:
			if( p_VideoInputDeviceFilterPt != NULL )
			{
				p_TmpSz1 = p_VideoInputDeviceFilterPt->Release();
				p_VideoInputDeviceFilterPt = NULL;
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
				goto out;
			}
		} //创建并初始化视频输入设备完毕。
		
		//初始化视频输入线程的临时变量。
		{
			MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec = 0; //初始化上一个视频输入帧的时间。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameTimeStepMsec = 1000 / MediaProcThreadPt->m_VideoInput.m_MaxSamplingRate; //初始化视频输入帧的时间步进。
			if( MediaProcThreadPt->m_VideoInput.m_FrameWidth * MediaProcThreadPt->m_VideoInput.m_FrameHeight >= MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth * MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight ) //如果视频输入帧指定的尺寸大于等于视频输入设备帧的尺寸。
			{
				MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz = MediaProcThreadPt->m_VideoInput.m_FrameWidth * MediaProcThreadPt->m_VideoInput.m_FrameHeight * 4; //初始化视频输入结果帧的内存大小。
			}
			else //如果视频输入帧指定的尺寸小于视频输入设备帧的尺寸。
			{
				MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz = MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth * MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight * 4; //初始化视频输入结果帧的内存大小。
			}
			MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz ); //初始化视频输入结果帧的内存指针。
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频结果帧的内存块失败。" );
				goto out;
			}
			MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz ); //初始化视频输入临时帧的内存指针。
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频临时帧的内存块失败。" );
				goto out;
			}
			MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen = 0; //初始化视频输入结果帧的数据长度。
			MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt = NULL; //初始化视频输入交换帧的内存指针。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt = NULL; //初始化视频输入帧元素的内存指针。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal = 0; //初始化视频输入帧链表的元数总数。
			MediaProcThreadPt->m_VideoInput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaProcThreadPt->m_VideoInput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化视频输入线程的临时变量成功。" );
		}

		//初始化编码器对象。
        switch( MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( OpenH264EncoderInit( &MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderPt, MediaProcThreadPt->m_VideoInput.m_FrameWidth, MediaProcThreadPt->m_VideoInput.m_FrameHeight, MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderVideoType, MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderEncodedBitrate, MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderBitrateControlMode, MediaProcThreadPt->m_VideoInput.m_MaxSamplingRate, MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderIDRFrameIntvl, MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderComplexity, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：创建并初始化OpenH264编码器类对象成功。" );
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化OpenH264编码器类对象失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
                    goto out;
                }
                break;
            }
        }

		//创建并初始化视频输入帧链表、视频输入空闲帧链表。
		{
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal = 2; //视频输入帧链表最多只存储2帧，避免因为视频设备有变化导致卡顿并积累大量视频输入帧，从而导致不及时。视频输入空闲帧链表最多存储总数与视频输入帧链表一致。
			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, sizeof( MediaProcThread::VideoInput::VideoInputFrameElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			if( ConstLenLnkLstInit( &MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, sizeof( MediaProcThread::VideoInput::VideoInputFrameElm * ), LNKLST_BUF_AUTO_ADJ_METH_NONE, 0, MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal, MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并初始化视频输入空闲帧链表成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化视频输入空闲帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				goto out; //本函数返回失败。
			}

			//创建并初始化视频输入空闲帧，并追加到视频输入空闲帧链表。
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmTotal; p_TmpInt322++ )
			{
				MediaProcThreadPt->m_VideoInputFramePt = ( MediaProcThread::VideoInput::VideoInputFrameElm * )malloc( sizeof( MediaProcThread::VideoInput::VideoInputFrameElm ) );
				if( MediaProcThreadPt->m_VideoInputFramePt == NULL )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
					goto out; //本函数返回失败。
				}
				MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_VideoInput.m_FrameWidth * MediaProcThreadPt->m_VideoInput.m_FrameHeight * 3 / 2 );
				if( MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt == NULL )
				{
					free( MediaProcThreadPt->m_VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt = NULL;
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
					goto out; //本函数返回失败。
				}
				if( MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder != 0 )
				{
					MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_VideoInput.m_FrameWidth * MediaProcThreadPt->m_VideoInput.m_FrameHeight * 3 / 2 );
					if( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt == NULL )
					{
						free( MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
						MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
						free( MediaProcThreadPt->m_VideoInputFramePt );
						MediaProcThreadPt->m_VideoInputFramePt = NULL;
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：创建并初始化视频输入空闲帧失败。" );
						goto out; //本函数返回失败。
					}
					MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen = 0;
				}
				else
				{
					MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen = 0;
				}
				if( ConstLenLnkLstPutTail( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, &MediaProcThreadPt->m_VideoInputFramePt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
				{
					free( MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaProcThreadPt->m_VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt = NULL;
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：追加视频输入空闲帧到视频输入空闲帧链表失败。" );
					goto out; //本函数返回失败。
				}
			}
		}
	} //初始化视频输入完毕。

    //初始化视频输出。
    if( MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput != 0 )
    {
        //初始化解码器对象。
        switch( MediaProcThreadPt->m_VideoOutput.m_UseWhatDecoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：使用YU12原始数据。" );
                break;
            }
            case 1: //如果要使用OpenH264解码器。
            {
                if( OpenH264DecoderInit( &MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderPt, MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderDecodeThreadNum, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：创建并初始化OpenH264解码器类对象成功。" );
                }
                else
                {
                    if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建并初始化OpenH264解码器类对象失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
                    goto out;
                }
                break;
            }
        }

		//初始化视频输出线程的临时变量。
		{
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出结果帧的内存指针。
			if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输出结果帧的内存块失败。" );
				goto out;
			}
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出临时帧的内存指针。
			if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建视频输出临时帧的内存块失败。" );
				goto out;
			}
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt = NULL; //初始化视频输出交换帧的内存指针。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz = 960 * 1280 * 4 * 3; //初始化视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen = 0; //初始化视频输出结果帧的数据长度。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth = 0; //初始化视频输出帧的宽度。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight = 0; //初始化视频输出帧的高度。
			MediaProcThreadPt->m_VideoOutput.m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
			MediaProcThreadPt->m_VideoOutput.m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：初始化视频输出线程的临时变量成功。" );
		}

		//创建并启动音频输出线程。
		MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadExitFlag = 0; //设置视频输出线程退出标记为0表示保持运行。
		MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )VideoOutputThreadRun, MediaProcThreadPt, 0, &MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadId );
		if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl != NULL )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：创建并启动视频输出线程成功。" );
		}
		else
		{
			if( MediaProcThreadPt->m_IsPrintLog != NULL ) LOGFE( "媒体处理线程：创建并启动视频输出线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto out; //本函数返回失败。
		}
    } //初始化视频输出完毕。
	
	//初始化媒体处理线程的临时变量。
	{
		MediaProcThreadPt->m_PcmAudioInputFramePt = NULL; //初始化PCM格式音频输入帧的内存指针。
		MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL; //初始化PCM格式音频输出帧的内存指针。
		if( MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput != 0 ) //初始化PCM格式音频结果帧的内存指针。
		{
			MediaProcThreadPt->m_PcmAudioResultFramePt = ( int16_t * )malloc( sizeof( int16_t ) * MediaProcThreadPt->m_AudioInput.m_FrameLen );
			if( MediaProcThreadPt->m_PcmAudioResultFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建PCM格式音频结果帧的内存块失败。" );
				goto out;
			}
		}
		else
		{
			MediaProcThreadPt->m_PcmAudioResultFramePt = NULL;
		}
		if( MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput != 0 ) //初始化PCM格式音频临时帧的内存指针。
		{
			MediaProcThreadPt->m_PcmAudioTmpFramePt = ( int16_t * )malloc( sizeof( int16_t ) * MediaProcThreadPt->m_AudioInput.m_FrameLen );
			if( MediaProcThreadPt->m_PcmAudioTmpFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建PCM格式音频临时帧的内存块失败。" );
				goto out;
			}
		}
		else
		{
			MediaProcThreadPt->m_PcmAudioTmpFramePt = NULL;
		}
		MediaProcThreadPt->m_PcmAudioSwapFramePt = NULL; //初始化PCM格式音频交换帧的内存指针。
		MediaProcThreadPt->m_VoiceActSts = 1; //初始化语音活动状态，预设为1是为了让在没有使用语音活动检测的情况下永远都有语音活动。
		if( ( MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput != 0 ) && ( MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder != 0 ) ) //初始化已编码格式音频输入帧的内存指针和内存大小。
		{
			MediaProcThreadPt->m_EncoderAudioInputFramePt = ( uint8_t * )malloc( MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( uint8_t ) );
			if( MediaProcThreadPt->m_EncoderAudioInputFramePt == NULL )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：创建已编码格式音频输入帧的内存块失败。" );
				goto out;
			}
			MediaProcThreadPt->m_EncoderAudioInputFrameSz = MediaProcThreadPt->m_AudioInput.m_FrameLen * sizeof( uint8_t );
		}
		else
		{
			MediaProcThreadPt->m_EncoderAudioInputFramePt = NULL;
			MediaProcThreadPt->m_EncoderAudioInputFrameSz = 0;
		}
		MediaProcThreadPt->m_EncoderAudioInputFrameLen = 0; //初始化已编码格式音频输入帧的数据长度，单位字节。
		MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans = 1; //初始化已编码格式音频输入帧是否需要传输预设为1，为了让在不使用非连续传输的情况下永远都是需要传输。
		MediaProcThreadPt->m_VideoInputFramePt = NULL; //初始化视频输入帧的内存指针。
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：初始化媒体处理线程的临时变量成功。" );
	} //初始化媒体处理线程的临时变量完毕。

	if( MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput != 0 ) //如果要使用音频输入。
	{
		waveInStart( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl ); //让音频输入设备开始录音。
	}
	if( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) //如果要使用音频输出。
	{
		for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ ) //让音频输出设备开始播放。
		{
			waveOutWrite( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322] ) );
		}
	}
	if( MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput != 0 ) //如果要使用视频输入。
	{
		MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Run(); //让视频输入过滤器图管理器开始运行。
	}
	
	if( MediaProcThreadPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec );
		LOGFI( "媒体处理线程：媒体处理线程初始化完毕，耗时：%" PRIu64 " 毫秒，正式开始处理帧。", p_NowMsec - p_LastMsec );
	}

	MediaProcThreadPt->m_ExitCode = -2; //初始化已经成功了，再将本线程退出代码预设为处理失败，如果处理失败，这个退出代码就不用再设置了，如果处理成功，再设置为成功的退出代码。
	MediaProcThreadPt->m_RunFlag = RUN_FLAG_PROC; //设置本线程运行标记为初始化完毕正在循环处理帧。

	//开始音视频输入输出帧处理循环。
	while( true )
	{
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec );

		//调用用户定义的处理函数。
		p_TmpInt321 = MediaProcThreadPt->m_UserProcessFuncPt( MediaProcThreadPt );
		if( p_TmpInt321 == 0 )
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的处理函数成功。返回值：%d", p_TmpInt321 );
		}
		else
		{
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：调用用户定义的处理函数失败。返回值：%d", p_TmpInt321 );
			goto out;
		}
		
        if( MediaProcThreadPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：调用用户定义的处理函数完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }

		//取出音频输入帧和音频输出帧。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt != NULL ) ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, &p_TmpSz1, NULL ); //获取音频输入帧链表的元素个数。
		else p_TmpSz1 = 0;
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt != NULL ) ConstLenLnkLstGetTotal( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, &p_TmpSz2, NULL ); //获取音频输出帧链表的元素个数。
		else p_TmpSz2 = 0;
		if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) ) //如果音频输入帧链表和音频输出帧链表中都有帧了，才开始取出。
		{
			//从音频输入帧链表中取出第一个音频输入帧。
			{
				ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioInputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL );
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%" PRIi32 "。", p_TmpSz1 );

			//从音频输出帧链表中取出第一个音频输出帧。
			{
				ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioOutputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL );
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%" PRIi32 "。", p_TmpSz2 );

			//将音频输入帧复制到音频结果帧，方便处理。
			memcpy( MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioInputFramePt, sizeof( int16_t ) * MediaProcThreadPt->m_AudioInput.m_FrameLen );
		}
		else if( ( p_TmpSz1 > 0 ) && ( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt == NULL ) ) //如果音频输入帧链表有帧了，且不使用音频输出帧链表，就开始取出。
		{
			//从音频输入帧链表中取出第一个音频输入帧。
			{
				ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL, MediaProcThreadPt->m_PcmAudioInputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL );
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输入帧链表中取出第一个音频输入帧，音频输入帧链表元素个数：%" PRIi32 "。", p_TmpSz1 );

			//将音频输入帧复制到音频结果帧，方便处理。
			memcpy( MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioInputFramePt, sizeof( int16_t ) * MediaProcThreadPt->m_AudioInput.m_FrameLen );
		}
		else if( ( p_TmpSz2 > 0 ) && ( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt == NULL ) ) //如果音频输出帧链表有帧了，且不使用音频输入帧链表，就开始取出。
		{
			//从音频输出帧链表中取出第一个音频输出帧。
			{
				ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioOutputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL );
			}
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从音频输出帧链表中取出第一个音频输出帧，音频输出帧链表元素个数：%" PRIi32 "。", p_TmpSz2 );
		}
		
        //处理音频输入帧。
		if( MediaProcThreadPt->m_PcmAudioInputFramePt != NULL )
		{
			//使用声学回音消除器。
			switch( MediaProcThreadPt->m_AudioInput.m_UseWhatAec )
			{
				case 0: //如果不使用声学回音消除器。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用声学回音消除器。" );
					break;
				}
				case 1: //如果使用Speex声学回音消除器。
				{
					if( ( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt != NULL ) && ( SpeexAecProc( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex声学回音消除器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex声学回音消除器失败。" );
					}
					break;
				}
				case 2: //如果使用WebRtc定点版声学回音消除器。
				{
					if( ( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmProc( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc定点版声学回音消除器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc定点版声学回音消除器失败。" );
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版声学回音消除器。
				{
					if( ( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt != NULL ) && ( WebRtcAecProc( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc浮点版声学回音消除器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc浮点版声学回音消除器失败。" );
					}
					break;
				}
				case 4: //如果使用SpeexWebRtc三重声学回音消除器。
				{
					if( ( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecProc( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 ) )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器失败。" );
					}
					break;
				}
			}

			//使用噪音抑制器。
			switch( MediaProcThreadPt->m_AudioInput.m_UseWhatNs )
			{
				case 0: //如果不使用噪音抑制器。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：不使用噪音抑制器。" );
					break;
				}
				case 1: //如果使用Speex预处理器的噪音抑制。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：稍后在使用Speex预处理器时一起使用噪音抑制。" );
					break;
				}
				case 2: //如果使用WebRtc定点版噪音抑制器。
				{
					if( WebRtcNsxProc( MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc定点版噪音抑制器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc定点版噪音抑制器失败。" );
					}
					break;
				}
				case 3: //如果使用WebRtc浮点版噪音抑制器。
				{
					if( WebRtcNsProc( MediaProcThreadPt->m_AudioInput.m_WebRtcNsPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用WebRtc浮点版噪音抑制器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用WebRtc浮点版噪音抑制器失败。" );
					}
					break;
				}
				case 4: //如果使用RNNoise噪音抑制器。
				{
					if( RNNoiseProc( MediaProcThreadPt->m_AudioInput.m_RNNoisePt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用RNNoise噪音抑制器成功。" );
						MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用RNNoise噪音抑制器失败。" );
					}
					break;
				}
			}

			//使用Speex预处理器。
			if( ( MediaProcThreadPt->m_AudioInput.m_UseWhatNs == 1 ) || ( MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther != 0 ) )
			{
				if( SpeexPprocProc( MediaProcThreadPt->m_AudioInput.m_SpeexPprocPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_PcmAudioTmpFramePt, &MediaProcThreadPt->m_VoiceActSts ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex预处理器成功。语音活动状态：%d", MediaProcThreadPt->m_VoiceActSts );
					MediaProcThreadPt->m_PcmAudioSwapFramePt = MediaProcThreadPt->m_PcmAudioResultFramePt; MediaProcThreadPt->m_PcmAudioResultFramePt = MediaProcThreadPt->m_PcmAudioTmpFramePt; MediaProcThreadPt->m_PcmAudioTmpFramePt = MediaProcThreadPt->m_PcmAudioSwapFramePt; //交换音频结果帧和音频临时帧。
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex预处理器失败。" );
				}
			}

			//判断音频输入是否静音。在音频输入处理完后再设置静音，这样可以保证音频输入处理器的连续性。
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputIsMute != 0 )
			{
				memset( MediaProcThreadPt->m_PcmAudioResultFramePt, 0, sizeof( int16_t ) * MediaProcThreadPt->m_AudioInput.m_FrameLen );
				if( ( MediaProcThreadPt->m_AudioInput.m_IsUseSpeexPprocOther != 0 ) && ( MediaProcThreadPt->m_AudioInput.m_SpeexPprocIsUseVad != 0 ) ) //如果Speex预处理器要使用其他功能，且要使用语音活动检测。
				{
					MediaProcThreadPt->m_VoiceActSts = 0;
				}
			}

			//使用编码器。
			switch( MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder )
			{
				case 0: //如果使用PCM原始数据。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用PCM原始数据。" );
					break;
				}
				case 1: //如果使用Speex编码器。
				{
					if( SpeexEncoderProc( MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_EncoderAudioInputFramePt, MediaProcThreadPt->m_EncoderAudioInputFrameSz, &MediaProcThreadPt->m_EncoderAudioInputFrameLen, &MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用Speex编码器成功。Speex格式输入帧的数据长度：%" PRIuPTR "，Speex格式输入帧是否需要传输：%" PRIi32, MediaProcThreadPt->m_EncoderAudioInputFrameLen, MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用Speex编码器失败。" );
					}
					break;
				}
				case 2: //如果使用Opus编码器。
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：暂不支持使用Opus编码器。" );
					goto out;
				}
			}

			//使用音频输入Wave文件写入器写入输入帧数据、音频结果Wave文件写入器写入结果帧数据。
			if( MediaProcThreadPt->m_AudioInput.m_IsSaveAudioToFile != 0 )
			{
				if( WaveFileWriterWriteData( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveFileWriterPt, ( char * )MediaProcThreadPt->m_PcmAudioInputFramePt, MediaProcThreadPt->m_AudioInput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频输入Wave文件写入器写入输入帧数据失败。" );
				}
				if( WaveFileWriterWriteData( MediaProcThreadPt->m_AudioInput.m_AudioResultWaveFileWriterPt, ( char * )MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_AudioInput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频结果Wave文件写入器写入结果帧数据失败。" );
				}
			}
		}
		
		if( MediaProcThreadPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：处理音频输入帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }
		
        //处理音频输出帧。
		if( MediaProcThreadPt->m_PcmAudioOutputFramePt != NULL )
		{
			//使用音频输出Wave文件写入器写入输出帧数据。
			if( MediaProcThreadPt->m_AudioOutput.m_IsSaveAudioToFile != 0 )
			{
				if( WaveFileWriterWriteData( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt, ( char * )MediaProcThreadPt->m_PcmAudioOutputFramePt, MediaProcThreadPt->m_AudioOutput.m_FrameLen * 2 ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：使用音频输出Wave文件写入器写入输出帧数据失败。" );
				}
			}
		}
		
		if( MediaProcThreadPt->m_IsPrintLog != 0 )
        {
			FuncGetTimeAsMsec( &p_NowMsec );
            LOGFI( "媒体处理线程：处理音频输出帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            p_LastMsec = p_NowMsec;
        }
		
        //处理视频输入帧。
		p_TmpSz1 = 0;
		ConstLenLnkLstGetTotal( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, &p_TmpSz1, NULL );
        if( ( p_TmpSz1 > 0 ) && //如果视频输入帧链表中有帧了。
            ( ( MediaProcThreadPt->m_PcmAudioInputFramePt != NULL ) || ( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt == NULL ) ) ) //且已经处理了音频输入帧或不使用音频输入帧链表。
        {
            //从视频输入帧链表中取出第一个视频输入帧。
            {
				ConstLenLnkLstGetHead( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_VideoInputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, NULL );
            }
            if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：从视频输入帧链表中取出第一个视频输入帧，视频输入帧链表元素个数：%" PRIuPTR "。", p_TmpSz1 );
        }

        if( MediaProcThreadPt->m_IsPrintLog != 0 )
        {
            FuncGetTimeAsMsec( &p_NowMsec );
			LOGFI( "媒体处理线程：处理视频输入帧完毕，耗时 %" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
            FuncGetTimeAsMsec( &p_LastMsec );
        }

		//调用用户定义的读取音视频输入帧函数。
        if( ( MediaProcThreadPt->m_PcmAudioInputFramePt != NULL ) || ( MediaProcThreadPt->m_VideoInputFramePt != NULL ) ) //如果取出了音频输入帧或视频输入帧。
        {
			if( MediaProcThreadPt->m_VideoInputFramePt != NULL ) //如果取出了视频输入帧。
                 MediaProcThreadPt->m_UserReadAudioVideoInputFrameFuncPt( MediaProcThreadPt, MediaProcThreadPt->m_PcmAudioInputFramePt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_VoiceActSts, MediaProcThreadPt->m_EncoderAudioInputFramePt, MediaProcThreadPt->m_EncoderAudioInputFrameLen, MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans, MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt, MediaProcThreadPt->m_VideoInput.m_FrameWidth, MediaProcThreadPt->m_VideoInput.m_FrameHeight, MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt, MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFrameLen );
            else
                MediaProcThreadPt->m_UserReadAudioVideoInputFrameFuncPt( MediaProcThreadPt, MediaProcThreadPt->m_PcmAudioInputFramePt, MediaProcThreadPt->m_PcmAudioResultFramePt, MediaProcThreadPt->m_VoiceActSts, MediaProcThreadPt->m_EncoderAudioInputFramePt, MediaProcThreadPt->m_EncoderAudioInputFrameLen, MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans, NULL, NULL, NULL, NULL, NULL );
		}
		
		if( MediaProcThreadPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &p_NowMsec );
			LOGFI( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时：%" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
			p_LastMsec = p_NowMsec;
		}
		
		if( MediaProcThreadPt->m_PcmAudioInputFramePt != NULL ) //如果取出了PCM格式音频输入帧，就追加到音频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, &MediaProcThreadPt->m_PcmAudioInputFramePt, NULL );
			MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
		}
		if( MediaProcThreadPt->m_PcmAudioOutputFramePt != NULL ) //如果取出了PCM格式音频输出帧，就追加到音频输出空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			ConstLenLnkLstPutTail( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, &MediaProcThreadPt->m_PcmAudioOutputFramePt, NULL );
			MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
		}
		if( MediaProcThreadPt->m_VideoInputFramePt != NULL ) //如果取出了视频输入帧，就追加到视频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
		{
			ConstLenLnkLstPutTail( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, &MediaProcThreadPt->m_VideoInputFramePt, NULL );
			MediaProcThreadPt->m_VideoInputFramePt = NULL;
		}

		//如果音频输入线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl != NULL )
		{
			GetExitCodeThread( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl, ( LPDWORD )&p_TmpInt321 );
			if( p_TmpInt321 != STILL_ACTIVE )
			{
				LOGFI( "媒体处理线程：音频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaProcThreadPt->m_ExitFlag = 3;
			}
		}

		//如果音频输出线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl != NULL )
		{
			GetExitCodeThread( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl, ( LPDWORD )&p_TmpInt321 );
			if( p_TmpInt321 != STILL_ACTIVE )
			{
				LOGFI( "媒体处理线程：音频输出线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaProcThreadPt->m_ExitFlag = 3;
			}
		}
		
		//如果视频输入线程因为设备关闭而退出了，就重启媒体处理线程。
		if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt != NULL )
		{
			long p_EventCode;
			LONG_PTR p_EventParam1, p_EventParam2;
			MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
			if( p_EventCode == EC_DEVICE_LOST )
			{
				LOGFI( "媒体处理线程：视频输入线程因为设备关闭而退出了，准备重启媒体处理线程。" );
				MediaProcThreadPt->m_ExitFlag = 3;
			}
		}

		if( MediaProcThreadPt->m_ExitFlag != 0 ) //如果本线程退出标记为请求退出。
		{
			MediaProcThreadPt->m_ExitCode = 0; //处理已经成功了，再将本线程退出代码设置为正常退出。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：接收到退出请求，开始准备退出。退出标记：%d。", MediaProcThreadPt->m_ExitFlag );
			goto out;
		}

		Sleep( 1 ); //暂停一下，避免CPU使用率过高。
	} //音视频输入输出帧处理循环完毕。

	out:
	MediaProcThreadPt->m_RunFlag = RUN_FLAG_DESTROY; //设置本线程运行标记为跳出循环处理帧正在销毁。
	if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程开始退出。" );
	
	//请求音视频输入输出线程退出。必须在销毁音视频输入输出前退出，因为音视频输入输出线程会使用音视频输入输出相关变量。
	if( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl != NULL ) while( PostThreadMessage( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadId, WM_QUIT, NULL, NULL ) == 0 ) Sleep( 0 ); //循环发送请求音频输入线程退出，直到线程消息发送成功，因为线程刚启动时还来不及创建消息队列从而导致发送失败。不要使用退出标记，因为音频输入设备可能会打开失败，导致音频输入线程不会进入消息循环。
	if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl != NULL ) while( PostThreadMessage( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadId, WM_QUIT, NULL, NULL ) == 0 ) Sleep( 0 ); //循环发送请求音频输出线程退出，直到线程消息发送成功，因为线程刚启动时还来不及创建消息队列从而导致发送失败。不要使用退出标记，因为音频输出设备可能会打开失败，导致音频输出线程不会进入消息循环。
	if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl != NULL ) MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadExitFlag = 1; //请求视频输出线程退出。

	//销毁音频输入。
    {
		//销毁音频输入线程。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl != NULL )
		{
			WaitForSingleObject( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl, INFINITE ); //等待音频输入线程退出。
			CloseHandle( MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl );
			MediaProcThreadPt->m_AudioInput.m_AudioInputThreadHdl = NULL;
			MediaProcThreadPt->m_AudioInput.m_AudioInputThreadId = NULL;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入线程成功。" );
		}
		
		//销毁音频输入线程线程的临时变量。
		{
			MediaProcThreadPt->m_AudioInput.m_Msg = { 0 }; //销毁消息。
			MediaProcThreadPt->m_AudioInput.m_AudioInputFramePt = NULL; //销毁音频输入帧的内存指针。
			MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstElmTotal = 0; //销毁音频输入帧链表的元数总数。
			MediaProcThreadPt->m_AudioInput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaProcThreadPt->m_AudioInput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁音频输入线程线程的临时变量成功。" );
		}

		//销毁音频输入帧链表。
		{
			if( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioInputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioInputFramePt );
					MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_AudioInput.m_AudioInputFrameLnkLstPt = NULL;
			}

			if( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioInputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioInputFramePt );
					MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入空闲帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入空闲帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_AudioInput.m_AudioInputIdleFrameLnkLstPt = NULL;
			}
		}

		//销毁音频输入设备。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				waveInUnprepareHeader( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl, &MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
			}
			waveInReset( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl );
			waveInClose( MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl );
			MediaProcThreadPt->m_AudioInput.m_AudioInputDeviceHdl = NULL;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入设备成功。" );
		}
		
		//销毁音频输入缓冲区块数组。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal; p_TmpInt322++ )
			{
				if( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData != NULL )
				{
					free( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData );
					MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt[p_TmpInt322].lpData = NULL;
				}
			}
			free( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt );
			MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrPt = NULL;
			MediaProcThreadPt->m_AudioInput.m_AudioInputWaveHdrTotal = 0;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入缓冲区块数组成功。" );
		}

		//销毁音频输入Wave文件写入器、音频结果Wave文件写入器。
		if( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( MediaProcThreadPt->m_AudioInput.m_AudioInputWaveFileWriterPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输入Wave文件写入器成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输入Wave文件写入器失败。" );
			}
			MediaProcThreadPt->m_AudioInput.m_AudioInputWaveFileWriterPt = NULL;
		}
		if( MediaProcThreadPt->m_AudioInput.m_AudioResultWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( MediaProcThreadPt->m_AudioInput.m_AudioResultWaveFileWriterPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频结果Wave文件写入器成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频结果Wave文件写入器失败。" );
			}
			MediaProcThreadPt->m_AudioInput.m_AudioResultWaveFileWriterPt = NULL;
		}
		
		//销毁编码器。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatEncoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				break;
			}
			case 1: //如果使用Speex编码器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPt != NULL )
				{
					if( SpeexEncoderDestroy( MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex编码器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex编码器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_SpeexEncoderPt = NULL;
				}
				break;
			}
			case 2: //如果使用Opus编码器。
			{
				break;
			}
		}
		
		//销毁Speex预处理器。
		if( MediaProcThreadPt->m_AudioInput.m_SpeexPprocPt != NULL )
		{
			if( SpeexPprocDestroy( MediaProcThreadPt->m_AudioInput.m_SpeexPprocPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex预处理器成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex预处理器失败。" );
			}
			MediaProcThreadPt->m_AudioInput.m_SpeexPprocPt = NULL;
		}
		
		//销毁噪音抑制器。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatNs )
		{
			case 0: //如果不使用噪音抑制器。
			{
				break;
			}
			case 1: //如果使用Speex预处理器的噪音抑制。
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：之前在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" );
				break;
			}
			case 2: //如果使用WebRtc定点版噪音抑制器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPt != NULL )
				{
					if( WebRtcNsxDestroy( MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc定点版噪音抑制器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc定点版噪音抑制器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_WebRtcNsxPt = NULL;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版噪音抑制器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_WebRtcNsPt != NULL )
				{
					if( WebRtcNsDestroy( MediaProcThreadPt->m_AudioInput.m_WebRtcNsPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc浮点版噪音抑制器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc浮点版噪音抑制器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_WebRtcNsPt = NULL;
				}
				break;
			}
			case 4: //如果使用RNNoise噪音抑制器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_RNNoisePt != NULL )
				{
					if( RNNoiseDestroy( MediaProcThreadPt->m_AudioInput.m_RNNoisePt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁RNNoise噪音抑制器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁RNNoise噪音抑制器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_RNNoisePt = NULL;
				}
				break;
			}
		}
		
		//销毁声学回音消除器。
		switch( MediaProcThreadPt->m_AudioInput.m_UseWhatAec )
		{
			case 0: //如果不使用声学回音消除器。
			{
				break;
			}
			case 1: //如果使用Speex声学回音消除器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt != NULL )
				{
					if( MediaProcThreadPt->m_AudioInput.m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
					{
						if( SpeexAecSaveMemFile( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecFilterLen, MediaProcThreadPt->m_AudioInput.m_SpeexAecIsUseRec, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoMutp, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoCntu, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupes, MediaProcThreadPt->m_AudioInput.m_SpeexAecEchoSupesAct, MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 成功。", MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", MediaProcThreadPt->m_AudioInput.m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( SpeexAecDestroy( MediaProcThreadPt->m_AudioInput.m_SpeexAecPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex声学回音消除器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_SpeexAecPt = NULL;
				}
				break;
			}
			case 2: //如果使用WebRtc定点版声学回音消除器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt != NULL )
				{
					if( WebRtcAecmDestroy( MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc定点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc定点版声学回音消除器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_WebRtcAecmPt = NULL;
				}
				break;
			}
			case 3: //如果使用WebRtc浮点版声学回音消除器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt != NULL )
				{
					if( MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
					{
						if( WebRtcAecSaveMemFile( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt, MediaProcThreadPt->m_AudioInput.m_SamplingRate, MediaProcThreadPt->m_AudioInput.m_FrameLen, MediaProcThreadPt->m_AudioInput.m_WebRtcAecEchoMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecDelay, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseDelayAgnosticMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseExtdFilterMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, MediaProcThreadPt->m_AudioInput.m_WebRtcAecIsUseAdaptAdjDelay, MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 成功。", MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
						}
						else
						{
							if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", MediaProcThreadPt->m_AudioInput.m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						}
					}
					if( WebRtcAecDestroy( MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁WebRtc浮点版声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁WebRtc浮点版声学回音消除器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_WebRtcAecPt = NULL;
				}
				break;
			}
			case 4: //如果使用SpeexWebRtc三重声学回音消除器。
			{
				if( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt != NULL )
				{
					if( SpeexWebRtcAecDestroy( MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器失败。" );
					}
					MediaProcThreadPt->m_AudioInput.m_SpeexWebRtcAecPt = NULL;
				}
				break;
			}
		}
	} //销毁音频输入完毕。

	//销毁音频输出。
	{
		//销毁音频输出线程。
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl != NULL )
		{
			WaitForSingleObject( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl, INFINITE ); //等待音频输出线程退出。
			CloseHandle( MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl );
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadHdl = NULL;
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputThreadId = NULL;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出线程成功。" );
		}
		
		//销毁音频输出线程线程的临时变量。
		{
			MediaProcThreadPt->m_AudioOutput.m_Msg = { 0 }; //销毁消息。
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFramePt = NULL; //销毁音频输出帧的内存指针。
			if( MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt != NULL ) //销毁已编码格式音频输出帧的内存指针。
			{
				free( MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt );
				MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFramePt = NULL;
			}
			MediaProcThreadPt->m_AudioOutput.m_EncoderAudioOutputFrameSz = 0; //销毁已编码格式音频输出帧的内存大小，单位字节。
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLen = 0; //销毁音频输出帧的数据长度，单位字节。
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstElmTotal = 0; //销毁音频输出帧链表的元数总数。
			MediaProcThreadPt->m_AudioOutput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaProcThreadPt->m_AudioOutput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁音频输出线程线程的临时变量成功。" );
		}

		//销毁音频输出帧链表、音频输出空闲帧链表。
		{
			if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioOutputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioOutputFramePt );
					MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputFrameLnkLstPt = NULL;
			}

			if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, NULL, &MediaProcThreadPt->m_PcmAudioOutputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_PcmAudioOutputFramePt );
					MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出空闲帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出空闲帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_AudioOutput.m_AudioOutputIdleFrameLnkLstPt = NULL;
			}
		}

		//销毁音频输出设备。
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				waveOutUnprepareHeader( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl, &MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322], sizeof( WAVEHDR ) );
			}
			waveOutReset( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl );
			waveOutClose( MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl );
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputDeviceHdl = NULL;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出设备成功。" );
		}

		//销毁音频输出缓冲区块数组。
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt != NULL )
		{
			for( p_TmpInt322 = 0; p_TmpInt322 < MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal; p_TmpInt322++ )
			{
				if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData != NULL )
				{
					free( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData );
					MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt[p_TmpInt322].lpData = NULL;
				}
			}
			free( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt );
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrPt = NULL;
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveHdrTotal = 0;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出缓冲区块数组成功。" );
		}

		//销毁音频输出Wave文件写入器。
		if( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt ) == 0 )
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁音频输出Wave文件写入器成功。" );
			}
			else
			{
				if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁音频输出Wave文件写入器失败。" );
			}
			MediaProcThreadPt->m_AudioOutput.m_AudioOutputWaveFileWriterPt = NULL;
		}

		//销毁解码器。
		switch( MediaProcThreadPt->m_AudioOutput.m_UseWhatDecoder )
		{
			case 0: //如果使用PCM原始数据。
			{
				break;
			}
			case 1: //如果使用Speex解码器。
			{
				if( MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderPt != NULL )
				{
					if( SpeexDecoderDestroy( MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderPt ) == 0 )
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁Speex解码器成功。" );
					}
					else
					{
						if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁Speex解码器失败。" );
					}
					MediaProcThreadPt->m_AudioOutput.m_SpeexDecoderPt = NULL;
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
		if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt != NULL )
		{
			MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Stop(); //让视频输入过滤器图管理器停止运行。
			p_TmpSz1 = MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt->Release();
			MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaControlPt = NULL;
		}
		if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt != NULL )
		{
			p_TmpSz1 = MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt->Release();
			MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerMediaEventPt = NULL;
		}
		if( MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt != NULL )
		{
			p_TmpSz1 = MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt->Release();
			MediaProcThreadPt->m_VideoInput.m_FilterGraphManagerPt = NULL;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceMaxSamplingRate = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameWidth = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameHeight = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsCrop = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropX = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropY = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropWidth = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameCropHeight = 0;
			MediaProcThreadPt->m_VideoInput.m_VideoInputDeviceFrameIsScale = 0;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入设备成功。" );
		}

		//销毁视频输入线程。
		if( MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt != NULL )
		{
			delete MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt;
			MediaProcThreadPt->m_VideoInput.m_VideoInputThreadPt = NULL;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入线程成功。" );
		}
		
		//销毁视频输入线程的临时变量。
		{
			MediaProcThreadPt->m_VideoInput.m_LastVideoInputFrameTimeMsec = 0; //销毁上一个视频输入帧的时间。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameTimeStepMsec = 0; //销毁视频输入帧的时间步进。
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt != NULL ) //销毁视频结果帧的内存指针。
			{
				free( MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt );
				MediaProcThreadPt->m_VideoInput.m_VideoInputResultFramePt = NULL;
			}
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt != NULL ) //销毁视频临时帧的内存指针。
			{
				free( MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt );
				MediaProcThreadPt->m_VideoInput.m_VideoInputTmpFramePt = NULL;
			}
			MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameSz = 0; //销毁视频输入结果帧的内存大小。
			MediaProcThreadPt->m_VideoInput.m_VideoInputResultFrameLen = 0; //销毁视频输入结果帧的数据长度。
			MediaProcThreadPt->m_VideoInput.m_VideoInputSwapFramePt = NULL; //销毁视频交换帧的内存指针。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameElmPt = NULL; //销毁视频输入帧元素的内存指针。
			MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstElmTotal = 0; //销毁视频输入帧链表的元数总数。
			MediaProcThreadPt->m_VideoInput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaProcThreadPt->m_VideoInput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁视频输入线程的临时变量成功。" );
		}

		//销毁视频输入帧链表、视频输入空闲帧链表。
		{
			if( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, NULL, &MediaProcThreadPt->m_VideoInputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaProcThreadPt->m_VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁视频输入帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_VideoInput.m_VideoInputFrameLnkLstPt = NULL;
			}

			if( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt != NULL )
			{
				while( ConstLenLnkLstGetHead( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, NULL, &MediaProcThreadPt->m_VideoInputFramePt, NULL, NULL ) == 0 )
				{
					free( MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt->m_YU12VideoInputFramePt = NULL;
					if( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt != NULL )
					{
						free( MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt );
						MediaProcThreadPt->m_VideoInputFramePt->m_EncoderVideoInputFramePt = NULL;
					}
					free( MediaProcThreadPt->m_VideoInputFramePt );
					MediaProcThreadPt->m_VideoInputFramePt = NULL;
					ConstLenLnkLstDelHead( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, NULL );
				}
				if( ConstLenLnkLstDestroy( MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输入空闲帧链表成功。" );
				}
				else
				{
					if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁视频输入空闲帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
				MediaProcThreadPt->m_VideoInput.m_VideoInputIdleFrameLnkLstPt = NULL;
			}
		}

        //销毁编码器。
        switch( MediaProcThreadPt->m_VideoInput.m_UseWhatEncoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                break;
            }
            case 1: //如果要使用OpenH264编码器。
            {
                if( MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderPt != NULL )
                {
                    if( OpenH264EncoderDestroy( MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderPt, NULL ) == 0 )
                    {
                        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁OpenH264编码器成功。" );
                    }
                    else
                    {
                        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "媒体处理线程：销毁OpenH264编码器失败。" );
                    }
                    MediaProcThreadPt->m_VideoInput.m_OpenH264EncoderPt = NULL;
                }
                break;
            }
        }
	} //销毁视频输入完毕。

	//销毁视频输出。
    {
		//销毁视频输出线程。
		if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl != NULL )
		{
			WaitForSingleObject( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl, INFINITE ); //等待视频输出线程退出。
			CloseHandle( MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl );
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadHdl = NULL;
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadId = NULL;
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputThreadExitFlag = 0;
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁视频输出线程成功。" );
		}
		
		//销毁视频输出线程的临时变量。
		{
			if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt != NULL ) //销毁视频输出结果帧的内存指针。
			{
				free( MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt );
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFramePt = NULL;
			}
			if( MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt != NULL ) //销毁视频输出临时帧的内存指针。
			{
				free( MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt );
				MediaProcThreadPt->m_VideoOutput.m_VideoOutputTmpFramePt = NULL;
			}
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputSwapFramePt = NULL; //销毁视频输出交换帧的内存指针。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameSz = 0; //销毁视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputResultFrameLen = 0; //销毁视频输出结果帧的数据长度。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameWidth = 0; //销毁视频输出帧的宽度。
			MediaProcThreadPt->m_VideoOutput.m_VideoOutputFrameHeight = 0; //销毁视频输出帧的高度。
			MediaProcThreadPt->m_VideoOutput.m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
			MediaProcThreadPt->m_VideoOutput.m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
			if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁视频输出线程的临时变量成功。" );
		}

        //销毁解码器类对象。
        switch( MediaProcThreadPt->m_VideoOutput.m_UseWhatDecoder )
        {
            case 0: //如果要使用YU12原始数据。
            {
                break;
            }
            case 1: //如果要使用OpenH264解码器。
            {
                if( MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderPt != NULL )
                {
                    if( OpenH264DecoderDestroy( MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderPt, NULL ) == 0 )
                    {
                        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGI( "媒体处理线程：销毁OpenH264解码器成功。" );
                    }
                    else
                    {
                        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGE( "媒体处理线程：销毁OpenH264解码器失败。" );
                    }
                    MediaProcThreadPt->m_VideoOutput.m_OpenH264DecoderPt = NULL;
                }
                break;
            }
        }
    } //销毁视频输出完毕。

	//销毁媒体处理线程的临时变量。
	{
		if( MediaProcThreadPt->m_PcmAudioInputFramePt != NULL ) //销毁PCM格式音频输入帧的内存指针。
		{
			free( MediaProcThreadPt->m_PcmAudioInputFramePt );
			MediaProcThreadPt->m_PcmAudioInputFramePt = NULL;
		}
		if( MediaProcThreadPt->m_PcmAudioOutputFramePt != NULL ) //销毁PCM格式音频输出帧的内存指针。
		{
			free( MediaProcThreadPt->m_PcmAudioOutputFramePt );
			MediaProcThreadPt->m_PcmAudioOutputFramePt = NULL;
		}
		if( MediaProcThreadPt->m_PcmAudioResultFramePt != NULL ) //销毁PCM格式结果帧的内存指针。
		{
			free( MediaProcThreadPt->m_PcmAudioResultFramePt );
			MediaProcThreadPt->m_PcmAudioResultFramePt = NULL;
		}
		if( MediaProcThreadPt->m_PcmAudioTmpFramePt != NULL ) //销毁PCM格式临时帧的内存指针。
		{
			free( MediaProcThreadPt->m_PcmAudioTmpFramePt );
			MediaProcThreadPt->m_PcmAudioTmpFramePt = NULL;
		}
		MediaProcThreadPt->m_PcmAudioSwapFramePt = NULL; //销毁PCM格式音频交换帧的内存指针。
		MediaProcThreadPt->m_VoiceActSts = 0; //销毁语音活动状态。
		if( MediaProcThreadPt->m_EncoderAudioInputFramePt != NULL ) //销毁已编码格式音频输入帧的内存指针。
		{
			free( MediaProcThreadPt->m_EncoderAudioInputFramePt );
			MediaProcThreadPt->m_EncoderAudioInputFramePt = NULL;
		}
		MediaProcThreadPt->m_EncoderAudioInputFrameSz = 0; //销毁已编码格式音频输入帧的内存大小。
		MediaProcThreadPt->m_EncoderAudioInputFrameLen = 0; //销毁已编码格式音频输入帧的数据长度。
		MediaProcThreadPt->m_EncoderAudioInputFrameIsNeedTrans = 0; //销毁已编码格式音频输入帧是否需要传输。
		MediaProcThreadPt->m_VideoInputFramePt = NULL; //销毁视频输入帧的内存指针。
		if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：销毁媒体处理线程的临时变量成功。" );
	} //销毁媒体处理线程的临时变量完毕。

    if( MediaProcThreadPt->m_ExitFlag != 3 ) //如果需要调用用户定义的销毁函数。
    {
        MediaProcThreadPt->m_UserDestroyFuncPt( MediaProcThreadPt ); //调用用户定义的销毁函数。
        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：调用用户定义的销毁函数成功。" );
    }
	
    MediaProcThreadPt->m_RunFlag = RUN_FLAG_END; //设置本线程运行标记为销毁完毕。

	if( ( MediaProcThreadPt->m_ExitFlag == 0 ) || ( MediaProcThreadPt->m_ExitFlag == 1 ) ) //如果用户需要直接退出。
    {
        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程已退出。" );
    }
    else //如果用户需用重新初始化。
    {
        if( MediaProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "媒体处理线程：本线程重新初始化。" );
		goto ReInit;
    }
	
	//销毁媒体处理线程的句柄。
	if( MediaProcThreadPt->m_MediaProcThreadHdl != NULL )
	{
		CloseHandle( MediaProcThreadPt->m_MediaProcThreadHdl );
		MediaProcThreadPt->m_MediaProcThreadHdl = NULL;
	}

	return 0;
}
