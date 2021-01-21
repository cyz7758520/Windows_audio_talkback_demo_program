#include "AudioProcThread.h"
#include "Func.h"
#include "Log.h"
#include <mmeapi.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <mmddk.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

DWORD WINAPI AudioInputThreadRun( AudioProcThread * AudioProcThreadPt );
DWORD WINAPI AudioOutputThreadRun( AudioProcThread * AudioProcThreadPt );
DWORD WINAPI AudioProcThreadRun( AudioProcThread * AudioProcThreadPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadInit
 * 功能说明：创建并初始化音频处理线程。
 * 参数说明：AudioProcThreadPtPt：[输出]，存放用于存放音频处理线程内存指针的内存指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的内存指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的内存指针，不能为NULL。
			 UserProcessFuncPt：[输入]，存放用户定义的处理函数的内存指针，不能为NULL。
			 UserDestroyFuncPt：[输入]，存放用户定义的销毁函数的内存指针，不能为NULL。
			 UserReadInputFrameFuncPt：[输入]，存放用户定义的读取输入帧函数的内存指针，不能为NULL。
			 UserWriteOutputFrameFuncPt：[输入]，存放用户定义的写入输出帧函数的内存指针，不能为NULL。
			 UserGetPcmOutputFrameFuncPt：[输入]，存放用户定义的获取PCM格式输出帧函数的内存指针，不能为NULL。
			 SamplingRate：[输入]，存放采样频率，单位赫兹，取值只能为8000、16000、32000。
			 FrameLen：[输入]，存放帧的数据长度，单位采样数据，取值只能为10ms的倍数。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadInit( AudioProcThread * * AudioProcThreadPtPt, void * UserDataPt, USER_INIT_FUNC_PT UserInitFuncPt, USER_PROCESS_FUNC_PT UserProcessFuncPt, USER_DESTROY_FUNC_PT UserDestroyFuncPt, USER_READ_INPUT_FRAME_FUNC_PT UserReadInputFrameFuncPt, USER_WRITE_OUTPUT_FRAME_FUNC_PT UserWriteOutputFrameFuncPt, USER_GET_PCM_OUTPUT_FRAME_FUNC_PT UserGetPcmOutputFrameFuncPt, int32_t SamplingRate, int32_t FrameLenMsec, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	AudioProcThread * p_AudioProcThreadPt = NULL; //存放音频处理线程的内存指针。

	//判断各个变量是否正确。
	if( AudioProcThreadPtPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用于存放音频处理线程内存指针的内存指针不正确。" );
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
	if( UserReadInputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的读取输入帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserWriteOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的写入输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( UserGetPcmOutputFrameFuncPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "用户定义的获取PCM格式输出帧函数的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( SamplingRate != 8000 ) && ( SamplingRate != 16000 ) && ( SamplingRate != 32000 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "采样频率不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( FrameLenMsec == 0 ) || ( FrameLenMsec % 10 != 0 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "帧的毫秒长度不正确。" );
		goto out; //本函数返回失败。
	}

	//创建并清空音频处理线程的内存块。
	p_AudioProcThreadPt = ( AudioProcThread * )calloc( 1, sizeof( AudioProcThread ) );
	if( p_AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "calloc 创建并清空音频处理线程的内存块失败。" );
		goto out;
	}

	p_AudioProcThreadPt->m_SamplingRate = SamplingRate; //设置采样频率。
	p_AudioProcThreadPt->m_FrameLen = FrameLenMsec * SamplingRate / 1000; //设置帧的数据长度。

	p_AudioProcThreadPt->m_InputDeviceID = WAVE_MAPPER; //设置音频输入设备的标识符。
	p_AudioProcThreadPt->m_OutputDeviceID = WAVE_MAPPER; //设置音频输出设备的标识符。

	if( VarStrInit( &p_AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //初始化错误信息动态字符串。
	{
		VarStrCpy( ErrInfoVarStrPt, "VarStrInit 初始化错误信息动态字符串失败。" );
		goto out;
	}

	p_AudioProcThreadPt->m_UserDataPt = UserDataPt; //设置用户数据的内存指针。
	p_AudioProcThreadPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的内存指针。
	p_AudioProcThreadPt->m_UserProcessFuncPt = UserProcessFuncPt; //设置用户定义的处理函数的内存指针。
	p_AudioProcThreadPt->m_UserDestroyFuncPt = UserDestroyFuncPt; //设置用户定义的销毁函数的内存指针。
	p_AudioProcThreadPt->m_UserReadInputFrameFuncPt = UserReadInputFrameFuncPt; //设置用户定义的读取输入帧函数的内存指针。
	p_AudioProcThreadPt->m_UserWriteOutputFrameFuncPt = UserWriteOutputFrameFuncPt; //设置用户定义的写入输出帧函数的内存指针。
	p_AudioProcThreadPt->m_UserGetPcmOutputFrameFuncPt = UserGetPcmOutputFrameFuncPt; //设置用户定义的获取PCM格式输出帧函数的内存指针。

	*AudioProcThreadPtPt = p_AudioProcThreadPt; //设置音频处理线程的内存指针。

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		AudioProcThreadDestroy( p_AudioProcThreadPt, NULL );
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetSaveSettingToFile
 * 功能说明：设置是否保存设置到文件。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsSaveSettingToFile：[输入]，存放是否保存设置到文件，为非0表示要保存，为0表示不保存。
			 SettingFileFullPathStrPt：[输入]，存放设置文件完整路径字符串的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetSaveSettingToFile( AudioProcThread * AudioProcThreadPt, int IsSaveSettingToFile, const char * SettingFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveSettingToFile != 0 ) && ( SettingFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "设置文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_IsSaveSettingToFile = IsSaveSettingToFile;
	if( IsSaveSettingToFile != 0 )
	{
		if( AudioProcThreadPt->m_SettingFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_SettingFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化设置文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_SettingFileFullPathVarStrPt, SettingFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置设置文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		AudioProcThreadPt->m_IsSaveSettingToFile = 0;
		if( AudioProcThreadPt->m_SettingFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_SettingFileFullPathVarStrPt );
			AudioProcThreadPt->m_SettingFileFullPathVarStrPt = NULL;
		}
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetPrintLog
 * 功能说明：设置是否打印Log日志。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetPrintLog( AudioProcThread * AudioProcThreadPt, int IsPrintLog, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_IsPrintLog = IsPrintLog;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadGetDeviceCaps
 * 功能说明：获取音频输入和输出设备的功能。调用本函数成功后，必须手动调用free(*InputCapsArrPtPt)和free(*OutputCapsArrPtPt)函数销毁。
 * 参数说明：InputCapsArrPtPt：[输出]，存放用于存放音频输入设备功能数组内存指针的内存指针，可以为NULL。
			 InputCapsArrTotal：[输出]，存放用于存放音频输入设备功能数组元素总数的内存指针，可以为NULL。
			 OutputCapsArrPtPt：[输出]，存放用于存放音频输出设备功能数组内存指针的内存指针，可以为NULL。
			 OutputCapsArrTotal：[输出]，存放用于存放音频输出设备功能数组元素总数的内存指针，可以为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadGetDeviceCaps( WAVEINCAPS * * InputCapsArrPtPt, UINT * InputCapsArrTotalPt, WAVEOUTCAPS * * OutputCapsArrPtPt, UINT * OutputCapsArrTotalPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	WAVEINCAPS * p_InputCapsArrPt = NULL; //存放音频输入设备功能数组的内存指针。
	UINT p_InputCapsArrTotal; //存放音频输入设备功能数组的元素总数。
	WAVEOUTCAPS * p_OutputCapsArrPt = NULL; //存放音频输出设备功能的内存指针。
	UINT p_OutputCapsArrTotal; //存放音频输出设备功能数组的元素总数。
	MMRESULT p_MMResult;
	UINT p_TmpUint;

	p_InputCapsArrTotal = waveInGetNumDevs();
	if( InputCapsArrTotalPt != NULL ) *InputCapsArrTotalPt = p_InputCapsArrTotal; //设置音频输入设备功能数组的元素总数。
	if( InputCapsArrPtPt != NULL )
	{
		p_InputCapsArrPt = ( WAVEINCAPS * )malloc( sizeof( WAVEINCAPS ) * p_InputCapsArrTotal );
		if( p_InputCapsArrPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "malloc 创建音频输入设备功能数组的内存块失败。" );
			goto out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_InputCapsArrTotal; p_TmpUint++ )
		{
			p_MMResult = waveInGetDevCaps( p_TmpUint, p_InputCapsArrPt + p_TmpUint, sizeof( WAVEINCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrCpy( ErrInfoVarStrPt, "waveInGetDevCaps 获取音频输入设备功能失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( p_MMResult, 'M' ) );
				goto out; //本函数返回失败。
			}
		}

		*InputCapsArrPtPt = p_InputCapsArrPt; //设置音频输入设备功能数组的内存指针。
	}

	p_OutputCapsArrTotal = waveOutGetNumDevs();
	if( OutputCapsArrTotalPt != NULL ) *OutputCapsArrTotalPt = p_OutputCapsArrTotal; //设置音频输出设备功能数组的元素总数。
	if( OutputCapsArrPtPt != NULL )
	{
		p_OutputCapsArrPt = ( WAVEOUTCAPS * )malloc( sizeof( WAVEOUTCAPS ) * p_OutputCapsArrTotal );
		if( p_OutputCapsArrPt == NULL )
		{
			VarStrCpy( ErrInfoVarStrPt, "malloc 创建音频输出设备功能数组的内存块失败。" );
			goto out; //本函数返回失败。
		}
		for( p_TmpUint = 0; p_TmpUint < p_OutputCapsArrTotal; p_TmpUint++ )
		{
			p_MMResult = waveOutGetDevCaps( p_TmpUint, p_OutputCapsArrPt + p_TmpUint, sizeof( WAVEOUTCAPS ) );
			if( p_MMResult != MMSYSERR_NOERROR )
			{
				VarStrCpy( ErrInfoVarStrPt, "waveOutGetDevCaps 获取音频输出设备功能失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( p_MMResult, 'M' ) );
				goto out; //本函数返回失败。
			}
		}

		*OutputCapsArrPtPt = p_OutputCapsArrPt; //设置音频输出设备功能数组的内存指针。
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		if( p_InputCapsArrPt != NULL )
		{
			free( p_InputCapsArrPt );
			p_InputCapsArrPt = NULL;
		}
		if( p_OutputCapsArrPt != NULL )
		{
			free( p_OutputCapsArrPt );
			p_OutputCapsArrPt = NULL;
		}
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseDevice
 * 功能说明：设置使用的音频输入和输出设备。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 InputDeviceID：[输入]，存放音频输入设备的标识符，从0到音频输入设备的总数减一。
			 OutputDeviceID：[输入]，存放音频输出设备的标识符，从0到音频输出设备的总数减一。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseDevice( AudioProcThread * AudioProcThreadPt, UINT InputDeviceID, UINT OutputDeviceID, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( InputDeviceID != WAVE_MAPPER ) && ( InputDeviceID >= waveInGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入设备的标识符不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( InputDeviceID != WAVE_MAPPER ) && ( OutputDeviceID >= waveOutGetNumDevs() ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出设备的标识符不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_InputDeviceID = InputDeviceID;
	AudioProcThreadPt->m_OutputDeviceID = OutputDeviceID;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetAudioInputDeviceMute
 * 功能说明：设置音频输入设备是否静音。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetAudioInputDeviceMute( AudioProcThread * AudioProcThreadPt, int IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_AudioInputDeviceIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetAudioOutputDeviceMute
 * 功能说明：设置音频输出设备是否静音。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetAudioOutputDeviceMute( AudioProcThread * AudioProcThreadPt, int IsMute, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_AudioOutputDeviceIsMute = IsMute;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseNoAec
 * 功能说明：设置不使用声学回音消除器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseNoAec( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatAec = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseSpeexAec
 * 功能说明：设置使用Speex声学回音消除器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 FilterLen：[输入]，存放Speex声学回音消除器的滤波器数据长度，单位毫秒。
			 IsSaveMemFile：[输入]，存放Speex声学回音消除器是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathStrPt：[输入]，存放Speex声学回音消除器的内存块文件完整路径字符串的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseSpeexAec( AudioProcThread * AudioProcThreadPt, int32_t FilterLen, int32_t IsUseRec, float EchoMultiple, float EchoCont, int32_t EchoSupes, int32_t EchoSupesAct, int IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "Speex声学回音消除器的内存块文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	
	AudioProcThreadPt->m_UseWhatAec = 1;
	AudioProcThreadPt->m_SpeexAecFilterLen = FilterLen;
	AudioProcThreadPt->m_SpeexAecIsUseRec = IsUseRec;
    AudioProcThreadPt->m_SpeexAecEchoMultiple = EchoMultiple;
    AudioProcThreadPt->m_SpeexAecEchoCont = EchoCont;
    AudioProcThreadPt->m_SpeexAecEchoSupes = EchoSupes;
    AudioProcThreadPt->m_SpeexAecEchoSupesAct = EchoSupesAct;
	AudioProcThreadPt->m_SpeexAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置Speex声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		AudioProcThreadPt->m_UseWhatAec = 0;
		if( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt );
			AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt = NULL;
		}
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseWebRtcAecm
 * 功能说明：设置使用WebRtc定点版声学回音消除器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseWebRtcAecm( AudioProcThread * AudioProcThreadPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatAec = 2;
	AudioProcThreadPt->m_WebRtcAecmIsUseCNGMode = IsUseCNGMode;
	AudioProcThreadPt->m_WebRtcAecmEchoMode = EchoMode;
	AudioProcThreadPt->m_WebRtcAecmDelay = Delay;
	
	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseWebRtcAec
 * 功能说明：设置使用WebRtc浮点版声学回音消除器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
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

int AudioProcThreadSetUseWebRtcAec( AudioProcThread * AudioProcThreadPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgnosticMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int IsSaveMemFile, const char * MemFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "WebRtc浮点版声学回音消除器的内存块文件完整路径字符串的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatAec = 3;
	AudioProcThreadPt->m_WebRtcAecEchoMode = EchoMode;
    AudioProcThreadPt->m_WebRtcAecDelay = Delay;
    AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode = IsUseDelayAgnosticMode;
    AudioProcThreadPt->m_WebRtcAecIsUseExtdFilterMode = IsUseExtdFilterMode;
    AudioProcThreadPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    AudioProcThreadPt->m_WebRtcAecIsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	AudioProcThreadPt->m_WebRtcAecIsSaveMemFile = IsSaveMemFile;
	if( IsSaveMemFile != 0 )
	{
		if( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt, MemFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		AudioProcThreadPt->m_UseWhatAec = 0;
		if( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt );
			AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt = NULL;
		}
	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseSpeexWebRtcAec
 * 功能说明：设置使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             WorkMode：[输入]，存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
             SpeexAecFilterLen：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器的滤波器数据长度，单位毫秒。
             SpeexAecEchoMultiple：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
             SpeexAecEchoCont：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
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

int AudioProcThreadSetUseSpeexWebRtcAec( AudioProcThread * AudioProcThreadPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMultiple, float SpeexAecEchoCont, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgnosticMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatAec = 4;
	AudioProcThreadPt->m_SpeexWebRtcAecWorkMode = WorkMode;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecFilterLen = SpeexAecFilterLen;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecIsUseRec = SpeexAecIsUseRec;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoMultiple = SpeexAecEchoMultiple;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoCont = SpeexAecEchoCont;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupes = SpeexAecEchoSupes;
	AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmEchoMode = WebRtcAecmEchoMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmDelay = WebRtcAecmDelay;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecEchoMode = WebRtcAecEchoMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecDelay = WebRtcAecDelay;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode = WebRtcAecIsUseDelayAgnosticMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	AudioProcThreadPt->m_SpeexWebRtcAecIsUseSameRoomAec = IsUseSameRoomAec;
	AudioProcThreadPt->m_SpeexWebRtcAecSameRoomEchoMinDelay = SameRoomEchoMinDelay;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseNoNs
 * 功能说明：设置不使用噪音抑制器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseNoNs( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatNs = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseSpeexPprocNs
 * 功能说明：设置使用Speex预处理器的噪音抑制。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             IsUseNs：[输入]，存放Speex预处理器是否使用噪音抑制，为非0表示要使用，为0表示不使用。
			 NoiseSupes：[输入]，存放Speex预处理器在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 IsUseDereverb：[输入]，存放Speex预处理器是否使用混响音消除，为非0表示要使用，为0表示不使用。
			 IsUseRec：[输入]，存放Speex预处理器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
			 EchoMultiple：[输入]，存放Speex预处理器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
			 EchoCont：[输入]，存放Speex预处理器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
			 EchoSupes：[输入]，存放Speex预处理器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 EchoSupesAct：[输入]，存放Speex预处理器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseSpeexPprocNs( AudioProcThread * AudioProcThreadPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatNs = 1;
	AudioProcThreadPt->m_SpeexPprocIsUseNs = IsUseNs;
	AudioProcThreadPt->m_SpeexPprocNoiseSupes = NoiseSupes;
    AudioProcThreadPt->m_SpeexPprocIsUseDereverb = IsUseDereverb;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseWebRtcNsx
 * 功能说明：设置使用WebRtc定点版噪音抑制器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc定点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseWebRtcNsx( AudioProcThread * AudioProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatNs = 2;
	AudioProcThreadPt->m_WebRtcNsxPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseWebRtcNs
 * 功能说明：设置使用WebRtc浮点版噪音抑制器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             PolicyMode：[输入]，存放WebRtc浮点版噪音抑制器的策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseWebRtcNs( AudioProcThread * AudioProcThreadPt, int32_t PolicyMode, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatNs = 3;
	AudioProcThreadPt->m_WebRtcNsPolicyMode = PolicyMode;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseRNNoise
 * 功能说明：设置使用RNNoise噪音抑制器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseRNNoise( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatNs = 4;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetSpeexPprocOther
 * 功能说明：设置Speex预处理器的其他功能。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             m_IsUseOther：[输入]，存放Speex预处理器是否使用其他功能，为非0表示要使用，为0表示不使用。
             IsUseVad：[输入]，存放Speex预处理器是否使用语音活动检测，为非0表示要使用，为0表示不使用。
             VadProbStart：[输入]，存放Speex预处理器在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
             VadProbCont：[输入]，存放Speex预处理器在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
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

int AudioProcThreadSetSpeexPprocOther( AudioProcThread * AudioProcThreadPt, int32_t IsUseOther, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCont, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_IsUseSpeexPprocOther = IsUseOther;
    AudioProcThreadPt->m_SpeexPprocIsUseVad = IsUseVad;
    AudioProcThreadPt->m_SpeexPprocVadProbStart = VadProbStart;
    AudioProcThreadPt->m_SpeexPprocVadProbCont = VadProbCont;
    AudioProcThreadPt->m_SpeexPprocIsUseAgc = IsUseAgc;
    AudioProcThreadPt->m_SpeexPprocAgcLevel = AgcLevel;
    AudioProcThreadPt->m_SpeexPprocAgcIncrement = AgcIncrement;
    AudioProcThreadPt->m_SpeexPprocAgcDecrement = AgcDecrement;
    AudioProcThreadPt->m_SpeexPprocAgcMaxGain = AgcMaxGain;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUsePcm
 * 功能说明：设置使用PCM原始数据。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUsePcm( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatCodec = 0;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseSpeexCodec
 * 功能说明：设置使用Speex编解码器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
             EncoderUseCbrOrVbr：[输入]，存放Speex编码器使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             EncoderQuality：[输入]，存放Speex编码器的编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             EncoderComplexity：[输入]，存放Speex编码器的编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             EncoderPlcExpectedLossRate：[输入]，存放Speex编码器在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             DecoderIsUsePerceptualEnhancement：[输入]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseSpeexCodec( AudioProcThread * AudioProcThreadPt, int32_t EncoderUseCbrOrVbr, int32_t EncoderQuality, int32_t EncoderComplexity, int32_t EncoderPlcExpectedLossRate, int32_t DecoderIsUsePerceptualEnhancement, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatCodec = 1;
	AudioProcThreadPt->m_SpeexCodecEncoderUseCbrOrVbr = EncoderUseCbrOrVbr;
    AudioProcThreadPt->m_SpeexCodecEncoderQuality = EncoderQuality;
    AudioProcThreadPt->m_SpeexCodecEncoderComplexity = EncoderComplexity;
    AudioProcThreadPt->m_SpeexCodecEncoderPlcExpectedLossRate = EncoderPlcExpectedLossRate;
    AudioProcThreadPt->m_SpeexCodecDecoderIsUsePerceptualEnhancement = DecoderIsUsePerceptualEnhancement;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetUseOpus
 * 功能说明：设置使用Opus编解码器。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetUseOpus( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_UseWhatCodec = 2;

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadSetSaveAudioToFile
 * 功能说明：设置保存音频到文件。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			 IsSaveAudioToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AudioInputFileFullPathStrPt：[输入]，存放音频输入文件完整路径字符串的内存指针。
			 AudioOutputFileFullPathStrPt：[输入]，存放音频输出文件完整路径字符串的内存指针。
			 AudioResultFileFullPathStrPt：[输入]，存放音频结果文件完整路径字符串的内存指针。
			 ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadSetSaveAudioToFile( AudioProcThread * AudioProcThreadPt, int32_t IsSaveAudioToFile, const char * AudioInputFileFullPathStrPt, const char * AudioOutputFileFullPathStrPt, const char * AudioResultFileFullPathStrPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioInputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输入文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioOutputFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频输出文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( IsSaveAudioToFile != 0 ) && ( AudioResultFileFullPathStrPt == NULL ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频结果文件的完整路径字符串不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_IsSaveAudioToFile = IsSaveAudioToFile;
	if( IsSaveAudioToFile != 0 )
	{
		if( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输入文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt, AudioInputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输入文件完整路径动态字符串失败。" );
			goto out;
		}
		if( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频输出文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt, AudioOutputFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频输出文件完整路径动态字符串失败。" );
			goto out;
		}
		if( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt == NULL )
		{
			if( VarStrInit( &AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt ) != 0 )
			{
				VarStrCpy( ErrInfoVarStrPt, "创建并初始化音频结果文件完整路径动态字符串失败。" );
				goto out;
			}
		}
		if( VarStrCpy( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt, AudioResultFileFullPathStrPt ) != 0 )
		{
			VarStrCpy( ErrInfoVarStrPt, "设置音频结果文件完整路径动态字符串失败。" );
			goto out;
		}
	}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		AudioProcThreadPt->m_IsSaveAudioToFile = 0;
		if( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt );
			AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt = NULL;
		}
		if( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt );
			AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt = NULL;
		}
		if( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt != NULL )
		{
			VarStrDestroy( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt );
			AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt = NULL;
		}
	}

	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：AudioProcThreadStart
* 功能说明：启动音频处理线程。
* 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadStart( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( AudioProcThreadPt->m_AudioProcThreadHdl != NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程已经启动。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_AudioProcThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioProcThreadRun, AudioProcThreadPt, 0, NULL ); //创建并启动音频处理线程。
	if( AudioProcThreadPt->m_AudioProcThreadHdl == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "CreateThread 创建并启动音频处理线程失败。原因：" ), VarStrCat( ErrInfoVarStrPt, FuncGetErrInfo( GetLastError(), 'G' ) );
		goto out; //本函数返回失败。
	}
	CloseHandle( AudioProcThreadPt->m_AudioProcThreadHdl ); //关闭音频处理线程的线程句柄，以后就不用再关闭了。

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{

	}
	return p_Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 函数名称：AudioProcThreadRequireExit
* 功能说明：请求音频处理线程退出。
* 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
            ExitFlag：[输入]，存放退出标记的值，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDestroy销毁函数。
			IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示阻塞。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadRequireExit( AudioProcThread * AudioProcThreadPt, int ExitFlag, int IsBlockWait, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VarStrCpy( ErrInfoVarStrPt, "退出标记的值不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadPt->m_ExitFlag = ExitFlag; //设置音频处理线程的退出标记。

	if( IsBlockWait != 0 ) //如果需要阻塞等待。
    {
        if( ExitFlag == 1 ) //如果是请求退出。
		{
			do
			{
				if( AudioProcThreadPt->m_AudioProcThreadHdl == NULL ) //如果音频处理线程的线程句柄为空，表示音频处理线程已经退出。
				{
					break;
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			}while( true );
		}
		else //如果是请求重启。
        {
            //等待重启完毕。
            do
            {
				if( AudioProcThreadPt->m_AudioProcThreadHdl == NULL ) //如果音频处理线程的线程句柄为空，表示音频处理线程已经退出。
				{
					break;
				}
				if( AudioProcThreadPt->m_ExitFlag == 0 ) //如果退出标记为0保持运行，表示重启完毕。
				{
					break;
				}

                Sleep( 1 ); //暂停一下，避免CPU使用率过高。
            }
            while( true );
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
* 函数名称：AudioProcThreadDestroy
* 功能说明：销毁音频处理线程。
* 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
			ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AudioProcThreadDestroy( AudioProcThread * AudioProcThreadPt, VarStr * ErrInfoVarStrPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( AudioProcThreadPt == NULL )
	{
		VarStrCpy( ErrInfoVarStrPt, "音频处理线程的内存指针不正确。" );
		goto out; //本函数返回失败。
	}

	AudioProcThreadRequireExit( AudioProcThreadPt, 1, 1, NULL ); //请求音频处理线程退出，并阻塞等待。

	//销毁设置文件的完整路径动态字符串。
	if( AudioProcThreadPt->m_SettingFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_SettingFileFullPathVarStrPt );
		AudioProcThreadPt->m_SettingFileFullPathVarStrPt = NULL;
	}
	
	//销毁Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt );
		AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt = NULL;
	}
	
	//销毁WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt );
		AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt = NULL;
	}

	//销毁音频输入文件完整路径动态字符串、音频输出文件完整路径动态字符串、音频结果文件完整路径动态字符串。
	if( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt );
		AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt = NULL;
	}
	if( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt );
		AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt = NULL;
	}
	if( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt );
		AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt = NULL;
	}

	//销毁错误信息动态字符串。
	if( AudioProcThreadPt->m_ErrInfoVarStrPt != NULL )
	{
		VarStrDestroy( AudioProcThreadPt->m_ErrInfoVarStrPt );
		AudioProcThreadPt->m_ErrInfoVarStrPt = NULL;
	}

	//销毁音频处理线程的内存块。
	free( AudioProcThreadPt );

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
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioInputThreadRun( AudioProcThread * AudioProcThreadPt )
{
	MSG p_Msg;
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;
	size_t p_InputFrameLnkLstTotal = 0;

	//开始音频输入循环。
	while( GetMessage( &p_Msg, NULL, 0, 0 ) != 0 )
	{
		if( p_Msg.message == WIM_DATA )
		{
			if( ( ( WAVEHDR * )p_Msg.lParam )->dwBytesRecorded != AudioProcThreadPt->m_FrameLen * sizeof( int16_t ) ) //如果该输入帧的数据长度不等于帧的数据长度，就表示该输入帧不完整。
			{
				waveInAddBuffer( AudioProcThreadPt->m_AudioInputDeviceHdl, ( WAVEHDR * )p_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频缓冲区块添加到音频输入设备上。
				continue;
			}

			//追加本次输入帧到输入帧链表。
			MutexLockLock( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, NULL );
			ConstLenLnkLstPutTail( AudioProcThreadPt->m_InputFrameLnkLstPt, ( ( WAVEHDR * )p_Msg.lParam )->lpData, NULL );
			MutexLockUnlock( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, NULL );

			waveInAddBuffer( AudioProcThreadPt->m_AudioInputDeviceHdl, ( WAVEHDR * )p_Msg.lParam, sizeof( WAVEHDR ) ); //将已准备的音频缓冲区块添加到音频输入设备上。

			if( AudioProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &p_NowMsec );
				ConstLenLnkLstGetTotal( AudioProcThreadPt->m_InputFrameLnkLstPt, &p_InputFrameLnkLstTotal, NULL );
				LOGFI( "音频输入线程：读取耗时 %" PRIu64 " 毫秒，输入帧链表元素个数：%" PRIuPTR "。", p_NowMsec - p_LastMsec, p_InputFrameLnkLstTotal );
				p_LastMsec = p_NowMsec;
			}
		}
		else if( p_Msg.message == WIM_OPEN )
		{
			SetThreadPriority( AudioProcThreadPt->m_AudioInputThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec );
		}
		else if( p_Msg.message == WIM_CLOSE )
		{
			AudioProcThreadPt->m_InputDeviceID = WAVE_MAPPER; //设置音频输入设备的标识符为默认的音频输入设备。
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输入设备。" );
			break;
		}
	}

	if( p_Msg.message == WM_QUIT )
	{
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输入线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioOutputThreadRun
 * 功能说明：音频输出线程主函数。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioOutputThreadRun( AudioProcThread * AudioProcThreadPt )
{
	MSG p_Msg;
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;
	size_t p_OutputFrameLnkLstTotal = 0;

	//开始音频输出循环。
	while( GetMessage( &p_Msg, NULL, 0, 0 ) != 0 )
	{
		if( p_Msg.message == WOM_DONE )
		{
			//调用用户定义的写入输出帧函数，并解码成PCM原始数据。
			switch( AudioProcThreadPt->m_UseWhatCodec ) //使用什么编解码器。
			{
				case 0: //如果使用PCM原始数据。
				{
					//调用用户定义的写入输出帧函数。
					AudioProcThreadPt->m_UserWriteOutputFrameFuncPt( AudioProcThreadPt, ( int16_t * )( ( WAVEHDR * )p_Msg.lParam )->lpData, NULL, 0 );
					break;
				}
				case 1: //如果使用Speex编解码器。
				{
					int8_t p_SpeexOutputFramePt[960]; //Speex格式输出帧。
					size_t p_SpeexOutputFrameLen; //Speex格式输出帧的数据长度，单位字节。

					//调用用户定义的写入输出帧函数。
					AudioProcThreadPt->m_UserWriteOutputFrameFuncPt( AudioProcThreadPt, NULL, p_SpeexOutputFramePt, &p_SpeexOutputFrameLen );

					//使用Speex解码器。
					if( SpeexDecoderProc( AudioProcThreadPt->m_SpeexDecoderPt, p_SpeexOutputFramePt, p_SpeexOutputFrameLen, ( int16_t * )( ( WAVEHDR * )p_Msg.lParam )->lpData ) == 0 ) //如果本次Speex格式输出帧接收到了或丢失了。
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：使用Speex解码器成功。" );
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：使用Speex解码器失败。" );
					}
					break;
				}
				case 2: //如果使用Opus编解码器。
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频输出线程：暂不支持使用Opus解码器。" );
				}
			}
			
			//判断音频输出设备是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
            if( AudioProcThreadPt->m_AudioOutputDeviceIsMute != 0 )
            {
				memset( ( ( WAVEHDR * )p_Msg.lParam )->lpData, 0, ( ( WAVEHDR * )p_Msg.lParam )->dwBufferLength );
            }

			//写入本次输出帧。
			waveOutWrite( AudioProcThreadPt->m_AudioOutputDeviceHdl, ( WAVEHDR * )p_Msg.lParam, sizeof( WAVEHDR ) );
			
			//调用用户定义的获取PCM格式输出帧函数。
			AudioProcThreadPt->m_UserGetPcmOutputFrameFuncPt( AudioProcThreadPt, ( int16_t * )( ( WAVEHDR * )p_Msg.lParam )->lpData );

			if( AudioProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &p_NowMsec );
				ConstLenLnkLstGetTotal( AudioProcThreadPt->m_OutputFrameLnkLstPt, &p_OutputFrameLnkLstTotal, NULL );
				LOGFI( "音频输出线程：写入耗时 %" PRIu64 " 毫秒，输出帧链表元素个数：%" PRIuPTR "。", p_NowMsec - p_LastMsec, p_OutputFrameLnkLstTotal );
				p_LastMsec = p_NowMsec;
			}

			//追加本次输出帧到输出帧链表。
			MutexLockLock( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, NULL );
			ConstLenLnkLstPutTail( AudioProcThreadPt->m_OutputFrameLnkLstPt, ( ( WAVEHDR * )p_Msg.lParam )->lpData, NULL );
			MutexLockUnlock( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, NULL );
		}
		else if( p_Msg.message == WOM_OPEN )
		{
			SetThreadPriority( AudioProcThreadPt->m_AudioOutputThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec );
		}
		else if( p_Msg.message == WOM_CLOSE )
		{
			AudioProcThreadPt->m_OutputDeviceID = WAVE_MAPPER; //设置音频输出设备的标识符为默认的音频输出设备。
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到设备关闭消息，开始准备退出，并切换到默认的音频输出设备。" );
			break;
		}
	}

	if( p_Msg.message == WM_QUIT )
	{
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGI( "音频输出线程：本线程接收到退出请求，开始准备退出。" );
	}

	if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输出线程：本线程已退出。" );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AudioProcThreadRun
 * 功能说明：音频处理线程主函数。
 * 参数说明：AudioProcThreadPt：[输入]，存放音频处理线程的内存指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AudioProcThreadRun( AudioProcThread * AudioProcThreadPt )
{
	SetThreadPriority( AudioProcThreadPt->m_AudioProcThreadHdl, THREAD_PRIORITY_ABOVE_NORMAL ); //设置本线程优先级。

	int p_TmpInt;
	int32_t p_TmpInt32;
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;

	int16_t * p_PcmInputFramePt = NULL; //PCM格式输入帧。
	int16_t * p_PcmOutputFramePt = NULL; //PCM格式输出帧。
	int16_t * p_PcmResultFramePt = NULL; //PCM格式结果帧。
	int16_t * p_PcmTmpFramePt = NULL; //PCM格式临时帧。
	int16_t * p_PcmSwapFramePt = NULL; //PCM格式交换帧。
	int32_t p_VoiceActSts = 1; //语音活动状态，为1表示有语音活动，为0表示无语音活动。
	int8_t * p_SpeexInputFramePt = NULL; //Speex格式输入帧。
	size_t p_SpeexInputFrameSz = 0; //Speex格式输入帧的内存长度，单位字节。
    size_t p_SpeexInputFrameLen = 0; //Speex格式输入帧的数据长度，单位字节。
    int32_t p_SpeexInputFrameIsNeedTrans = 1; //Speex格式输入帧是否需要传输，为1表示需要传输，为0表示不需要传输。

	ReInit:
	if( AudioProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

	if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：本地代码的指令集名称为%s。退出标记：%d。", ( sizeof( size_t ) == 4 ) ? "x86" : "x64", AudioProcThreadPt->m_ExitFlag );

	if( AudioProcThreadPt->m_ExitFlag != 3 ) //如果需要执行用户定义的初始化函数。
	{
		AudioProcThreadPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
		AudioProcThreadPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。

		//调用用户定义的初始化函数。
		p_TmpInt = AudioProcThreadPt->m_UserInitFuncPt( AudioProcThreadPt );
		if( p_TmpInt == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：调用用户定义的初始化函数成功。返回值：%d。", p_TmpInt );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：调用用户定义的初始化函数失败。返回值：%d。", p_TmpInt );
			goto out;
		}
	}
	else
    {
		AudioProcThreadPt->m_ExitFlag = 0; //设置本线程退出标记为保持运行。
		AudioProcThreadPt->m_ExitCode = -1; //先将本线程退出代码预设为初始化失败，如果初始化失败，这个退出代码就不用再设置了，如果初始化成功，再设置为成功的退出代码。
    }

	//保存设置到文件。
	if( AudioProcThreadPt->m_IsSaveSettingToFile != 0 )
	{
		FILE * p_SettingFileStreamPt;

		p_SettingFileStreamPt = fopen( AudioProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt, "wb+" );
		if( p_SettingFileStreamPt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：打开设置文件 %s 失败。原因：%s", AudioProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt, FuncGetErrInfo( errno, 'E' ) );
			goto out;
		}

		fprintf( p_SettingFileStreamPt,
				 "m_SamplingRate：%" PRId32 " \
				  \nm_FrameLen：%" PRId32 " \
				  \n \
				  \nm_IsSaveSettingToFile：%d \
				  \nm_SettingFileFullPathVarStrPt->m_StrPt：%s \
				  \n \
				  \nm_IsPrintLog：%d \
				  \n \
				  \nm_UseWhatAec：%d \
				  \n \
				  \nm_SpeexAecFilterLen：%" PRId32 " \
				  \nm_SpeexAecIsUseRec：%" PRId32 " \
				  \nm_SpeexAecEchoMultiple：%f \
				  \nm_SpeexAecEchoCont：%f \
				  \nm_SpeexAecEchoSupes：%" PRId32 " \
				  \nm_SpeexAecEchoSupesAct：%" PRId32 " \
				  \nm_SpeexAecIsSaveMemFile：%d \
				  \nm_SpeexAecMemFileFullPathVarStrPt->m_StrPt：%s \
				  \n \
				  \nm_WebRtcAecmIsUseCNGMode：%" PRId32 " \
				  \nm_WebRtcAecmEchoMode：%" PRId32 " \
				  \nm_WebRtcAecmDelay：%" PRId32 " \
				  \n \
				  \nm_WebRtcAecEchoMode：%" PRId32 " \
				  \nm_WebRtcAecDelay：%" PRId32 " \
				  \nm_WebRtcAecIsUseDelayAgnosticMode：%" PRId32 " \
				  \nm_WebRtcAecIsUseExtdFilterMode：%" PRId32 " \
				  \nm_WebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 " \
				  \nm_WebRtcAecIsUseAdaptAdjDelay：%" PRId32 " \
				  \nm_WebRtcAecIsSaveMemFile：%d \
				  \nm_WebRtcAecMemFileFullPathVarStrPt->m_StrPt：%s \
				  \n \
				  \nm_SpeexWebRtcAecWorkMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecSpeexAecFilterLen：%" PRId32 " \
				  \nm_SpeexWebRtcAecSpeexAecIsUseRec：%" PRId32 " \
				  \nm_SpeexWebRtcAecSpeexAecEchoMultiple：%f \
				  \nm_SpeexWebRtcAecSpeexAecEchoCont：%f \
				  \nm_SpeexWebRtcAecSpeexAecEchoSupes：%" PRId32 " \
				  \nm_SpeexWebRtcAecSpeexAecEchoSupesAct：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecmIsUseCNGMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecmEchoMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecmDelay：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecEchoMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecDelay：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode：%" PRId32 " \
				  \nm_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay：%" PRId32 " \
				  \n \
				  \nm_UseWhatNs：%d \
				  \n \
				  \nm_SpeexPprocIsUseNs：%" PRId32 " \
				  \nm_SpeexPprocNoiseSupes：%" PRId32 " \
				  \nm_SpeexPprocIsUseDereverb：%" PRId32 " \
				  \n \
				  \nm_WebRtcNsxPolicyMode：%" PRId32 " \
				  \n \
				  \nm_WebRtcNsPolicyMode：%" PRId32 " \
				  \n \
				  \nm_IsUseSpeexPprocOther：%d \
				  \nm_SpeexPprocIsUseVad：%" PRId32 " \
				  \nm_SpeexPprocVadProbStart：%" PRId32 " \
				  \nm_SpeexPprocVadProbCont：%" PRId32 " \
				  \nm_SpeexPprocIsUseAgc：%" PRId32 " \
				  \nm_SpeexPprocAgcLevel：%" PRId32 " \
				  \nm_SpeexPprocAgcIncrement：%" PRId32 " \
				  \nm_SpeexPprocAgcDecrement：%" PRId32 " \
				  \nm_SpeexPprocAgcMaxGain：%" PRId32 " \
				  \n \
				  \nm_UseWhatCodec：%d \
				  \n \
				  \nm_SpeexCodecEncoderUseCbrOrVbr：%" PRId32 " \
				  \nm_SpeexCodecEncoderQuality：%" PRId32 " \
				  \nm_SpeexCodecEncoderComplexity：%" PRId32 " \
				  \nm_SpeexCodecEncoderPlcExpectedLossRate：%" PRId32 " \
				  \nm_SpeexCodecDecoderIsUsePerceptualEnhancement：%" PRId32 " \
				  \n \
				  \nm_IsSaveAudioToFile：%d \
				  \nm_AudioInputFileFullPathVarStrPt->m_StrPt：%s \
				  \nm_AudioOutputFileFullPathVarStrPt->m_StrPt：%s \
				  \nm_AudioResultFileFullPathVarStrPt->m_StrPt：%s \
				  \n \
				  \nm_InputDeviceID：%u \
				  \nm_OutputDeviceID：%u",
				  AudioProcThreadPt->m_SamplingRate,				  AudioProcThreadPt->m_FrameLen,				  				  AudioProcThreadPt->m_IsSaveSettingToFile,				  ( AudioProcThreadPt->m_SettingFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt : NULL,				  				  AudioProcThreadPt->m_IsPrintLog,				  				  AudioProcThreadPt->m_UseWhatAec,				  				  AudioProcThreadPt->m_SpeexAecFilterLen,				  AudioProcThreadPt->m_SpeexAecIsUseRec,				  AudioProcThreadPt->m_SpeexAecEchoMultiple,				  AudioProcThreadPt->m_SpeexAecEchoCont,				  AudioProcThreadPt->m_SpeexAecEchoSupes,				  AudioProcThreadPt->m_SpeexAecEchoSupesAct,				  AudioProcThreadPt->m_SpeexAecIsSaveMemFile,				  ( AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt : NULL,				  				  AudioProcThreadPt->m_WebRtcAecmIsUseCNGMode,				  AudioProcThreadPt->m_WebRtcAecmEchoMode,				  AudioProcThreadPt->m_WebRtcAecmDelay,				  				  AudioProcThreadPt->m_WebRtcAecEchoMode,				  AudioProcThreadPt->m_WebRtcAecDelay,				  AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode,				  AudioProcThreadPt->m_WebRtcAecIsUseExtdFilterMode,				  AudioProcThreadPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode,				  AudioProcThreadPt->m_WebRtcAecIsUseAdaptAdjDelay,				  AudioProcThreadPt->m_WebRtcAecIsSaveMemFile,				  ( AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt : NULL,				  				  AudioProcThreadPt->m_SpeexWebRtcAecWorkMode,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecFilterLen,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecIsUseRec,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoMultiple,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoCont,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupes,				  AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupesAct,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmEchoMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmDelay,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecEchoMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecDelay,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode,				  AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay,				  				  AudioProcThreadPt->m_UseWhatNs,				  				  AudioProcThreadPt->m_SpeexPprocIsUseNs,				  AudioProcThreadPt->m_SpeexPprocNoiseSupes,				  AudioProcThreadPt->m_SpeexPprocIsUseDereverb,				  				  AudioProcThreadPt->m_WebRtcNsxPolicyMode,				  				  AudioProcThreadPt->m_WebRtcNsPolicyMode,				  				  AudioProcThreadPt->m_IsUseSpeexPprocOther,				  AudioProcThreadPt->m_SpeexPprocIsUseVad,				  AudioProcThreadPt->m_SpeexPprocVadProbStart,				  AudioProcThreadPt->m_SpeexPprocVadProbCont,				  AudioProcThreadPt->m_SpeexPprocIsUseAgc,				  AudioProcThreadPt->m_SpeexPprocAgcLevel,				  AudioProcThreadPt->m_SpeexPprocAgcIncrement,				  AudioProcThreadPt->m_SpeexPprocAgcDecrement,				  AudioProcThreadPt->m_SpeexPprocAgcMaxGain,				  				  AudioProcThreadPt->m_UseWhatCodec,				  				  AudioProcThreadPt->m_SpeexCodecEncoderUseCbrOrVbr,				  AudioProcThreadPt->m_SpeexCodecEncoderQuality,				  AudioProcThreadPt->m_SpeexCodecEncoderComplexity,				  AudioProcThreadPt->m_SpeexCodecEncoderPlcExpectedLossRate,				  AudioProcThreadPt->m_SpeexCodecDecoderIsUsePerceptualEnhancement,				  				  AudioProcThreadPt->m_IsSaveAudioToFile,				  ( AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt->m_StrPt : NULL,				  ( AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt->m_StrPt : NULL,				  ( AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt != NULL ) ? AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt->m_StrPt : NULL,				  				  AudioProcThreadPt->m_InputDeviceID,				  AudioProcThreadPt->m_OutputDeviceID);
		fclose( p_SettingFileStreamPt );
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：保存设置到文件 %s 成功。", AudioProcThreadPt->m_SettingFileFullPathVarStrPt->m_StrPt );
	}

	//创建PCM格式输入帧、PCM格式输出帧、PCM格式结果帧、PCM格式临时帧、PCM格式交换帧、语音活动状态、Speex格式输入帧、Speex格式输入帧的内存大小、Speex格式输入帧的数据长度、Speex格式输入帧是否需要传输。
	{
		p_PcmInputFramePt = ( int16_t * )malloc( sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
		if( p_PcmInputFramePt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建PCM格式输入帧的内存块失败。" );
			goto out;
		}
		p_PcmOutputFramePt = ( int16_t * )malloc( sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
		if( p_PcmOutputFramePt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建PCM格式输出帧的内存块失败。" );
			goto out;
		}
		p_PcmResultFramePt = ( int16_t * )malloc( sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
		if( p_PcmResultFramePt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建PCM格式结果帧的内存块失败。" );
			goto out;
		}
		p_PcmTmpFramePt = ( int16_t * )malloc( sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
		if( p_PcmTmpFramePt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建PCM格式临时帧的内存块失败。" );
			goto out;
		}
		p_PcmSwapFramePt = NULL;
		p_VoiceActSts = 1; //语音活动状态预设为1，为了让在没有使用语音活动检测的情况下永远都是有语音活动。
		if( AudioProcThreadPt->m_UseWhatCodec == 1 )
		{
			p_SpeexInputFramePt = ( int8_t * )malloc( AudioProcThreadPt->m_FrameLen );
			if( p_SpeexInputFramePt == NULL )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建Speex格式输入帧的内存块失败。" );
				goto out;
			}
			p_SpeexInputFrameSz = AudioProcThreadPt->m_FrameLen;
		}
		else
		{
			p_SpeexInputFramePt = NULL;
			p_SpeexInputFrameSz = 0;
		}
		p_SpeexInputFrameLen = 0;
		p_SpeexInputFrameIsNeedTrans = 1; //Speex格式输入帧是否需要传输预设为1，为了让在没有使用非连续传输的情况下永远都是需要传输。
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建PCM格式输入帧、PCM格式输出帧、PCM格式结果帧、PCM格式临时帧、PCM格式交换帧、语音活动状态、Speex格式输入帧、Speex格式输入帧的内存长度、Speex格式输入帧的数据长度、Speex格式输入帧是否需要传输成功。" );
	}

	//创建并初始化输入帧链表、输入帧链表互斥锁、输出帧链表、输出帧链表互斥锁。
	{
		if( ConstLenLnkLstInit( &AudioProcThreadPt->m_InputFrameLnkLstPt, AudioProcThreadPt->m_FrameLen * 2, LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, 1, 2, 2, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //输入帧链表最多只存储2帧，避免因为音频设备有变化导致积累大量输入帧，从而导致不同步。
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化输入帧链表成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化输入帧链表失败。原因：%s", AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			goto out; //本函数返回失败。
		}
		if( MutexLockInit( &AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化输入帧链表互斥锁成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化输入帧链表互斥锁失败。原因：%s", AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			goto out; //本函数返回失败。
		}
		if( ConstLenLnkLstInit( &AudioProcThreadPt->m_OutputFrameLnkLstPt, AudioProcThreadPt->m_FrameLen * 2, LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, 1, 2, 2, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //输出帧链表最多只存储2帧，避免因为音频设备有变化导致积累大量输出帧，从而导致不同步。
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化输出帧链表成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化输出帧链表失败。原因：%s", AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			goto out; //本函数返回失败。
		}
		if( MutexLockInit( &AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化输出帧链表互斥锁成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化输出帧链表互斥锁失败。原因：%s", AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			goto out; //本函数返回失败。
		}
	}
	
	//创建并初始化声学回音消除器。
	switch( AudioProcThreadPt->m_UseWhatAec )
	{
		case 0: //如果不使用声学回音消除器。
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：不使用声学回音消除器。" );
			break;
		}
		case 1: //如果使用Speex声学回音消除器。
		{
			if( AudioProcThreadPt->m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
			{
				if( SpeexAecInitByMemFile( &AudioProcThreadPt->m_SpeexAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_SpeexAecFilterLen, AudioProcThreadPt->m_SpeexAecIsUseRec, AudioProcThreadPt->m_SpeexAecEchoMultiple, AudioProcThreadPt->m_SpeexAecEchoCont, AudioProcThreadPt->m_SpeexAecEchoSupes, AudioProcThreadPt->m_SpeexAecEchoSupesAct, AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器成功。", AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：根据Speex声学回音消除器内存块文件 %s 来创建并初始化Speex声学回音消除器失败。原因：%s", AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}
			if( AudioProcThreadPt->m_SpeexAecPt == NULL )
			{
				if( SpeexAecInit( &AudioProcThreadPt->m_SpeexAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_SpeexAecFilterLen, AudioProcThreadPt->m_SpeexAecIsUseRec, AudioProcThreadPt->m_SpeexAecEchoMultiple, AudioProcThreadPt->m_SpeexAecEchoCont, AudioProcThreadPt->m_SpeexAecEchoSupes, AudioProcThreadPt->m_SpeexAecEchoSupesAct ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化Speex声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化Speex声学回音消除器失败。" );
					goto out;
				}
			}
			break;
		}
		case 2: //如果使用WebRtc定点版声学回音消除器。
		{
			if( WebRtcAecmInit( &AudioProcThreadPt->m_WebRtcAecmPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcAecmIsUseCNGMode, AudioProcThreadPt->m_WebRtcAecmEchoMode, AudioProcThreadPt->m_WebRtcAecmDelay ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化WebRtc定点版声学回音消除器成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化WebRtc定点版声学回音消除器失败。" );
				goto out;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版声学回音消除器。
		{
			if( AudioProcThreadPt->m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
			{
				if( WebRtcAecInitByMemFile( &AudioProcThreadPt->m_WebRtcAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcAecEchoMode, AudioProcThreadPt->m_WebRtcAecDelay, AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode, AudioProcThreadPt->m_WebRtcAecIsUseExtdFilterMode, AudioProcThreadPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AudioProcThreadPt->m_WebRtcAecIsUseAdaptAdjDelay, AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器成功。", AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %s 来创建并初始化WebRtc浮点版声学回音消除器失败。原因：%s", AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				}
			}
			if( AudioProcThreadPt->m_WebRtcAecPt == NULL )
			{
				if( WebRtcAecInit( &AudioProcThreadPt->m_WebRtcAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcAecEchoMode, AudioProcThreadPt->m_WebRtcAecDelay, AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode, AudioProcThreadPt->m_WebRtcAecIsUseExtdFilterMode, AudioProcThreadPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AudioProcThreadPt->m_WebRtcAecIsUseAdaptAdjDelay ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化WebRtc浮点版声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化WebRtc浮点版声学回音消除器失败。" );
					goto out;
				}
			}
			break;
		}
		case 4: //如果使用SpeexWebRtc三重声学回音消除器。
        {
			if( SpeexWebRtcAecInit( &AudioProcThreadPt->m_SpeexWebRtcAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_SpeexWebRtcAecWorkMode, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecFilterLen, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecIsUseRec, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoMultiple, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoCont, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupes, AudioProcThreadPt->m_SpeexWebRtcAecSpeexAecEchoSupesAct, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmEchoMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecmDelay, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecEchoMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecDelay, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, AudioProcThreadPt->m_SpeexWebRtcAecIsUseSameRoomAec, AudioProcThreadPt->m_SpeexWebRtcAecSameRoomEchoMinDelay ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化SpeexWebRtc三重声学回音消除器成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化SpeexWebRtc三重声学回音消除器失败。" );
				goto out;
			}
            break;
        }
	}
	
    //创建并初始化噪音抑制器对象。
    switch( AudioProcThreadPt->m_UseWhatNs )
    {
        case 0: //如果不使用噪音抑制器。
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：不使用噪音抑制器。" );
            break;
        }
        case 1: //如果使用Speex预处理器的噪音抑制。
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：稍后在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" );
            break;
        }
        case 2: //如果使用WebRtc定点版噪音抑制器。
        {
            if( WebRtcNsxInit( &AudioProcThreadPt->m_WebRtcNsxPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcNsxPolicyMode ) == 0 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化WebRtc定点版噪音抑制器成功。" );
            }
            else
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化WebRtc定点版噪音抑制器失败。" );
                goto out;
            }
            break;
        }
        case 3: //如果使用WebRtc浮点版噪音抑制器。
        {
            if( WebRtcNsInit( &AudioProcThreadPt->m_WebRtcNsPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcNsPolicyMode ) == 0 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化WebRtc浮点版噪音抑制器成功。" );
            }
            else
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化WebRtc浮点版噪音抑制器失败。" );
                goto out;
            }
            break;
        }
        case 4: //如果使用RNNoise噪音抑制器。
        {
            if( RNNoiseInit( &AudioProcThreadPt->m_RNNoisePt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen ) == 0 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化RNNoise噪音抑制器成功。" );
            }
            else
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化RNNoise噪音抑制器失败。" );
                goto out;
            }
            break;
        }
    }
	
    //创建并初始化Speex预处理器。
    if( ( AudioProcThreadPt->m_UseWhatNs == 1 ) || ( AudioProcThreadPt->m_IsUseSpeexPprocOther != 0 ) )
    {
        if( AudioProcThreadPt->m_UseWhatNs != 1 )
        {
            AudioProcThreadPt->m_SpeexPprocIsUseNs = 0;
            AudioProcThreadPt->m_SpeexPprocIsUseDereverb = 0;
        }
        if( AudioProcThreadPt->m_IsUseSpeexPprocOther == 0 )
        {
            AudioProcThreadPt->m_SpeexPprocIsUseVad = 0;
            AudioProcThreadPt->m_SpeexPprocIsUseAgc = 0;
        }
        if( SpeexPprocInit( &AudioProcThreadPt->m_SpeexPprocPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_SpeexPprocIsUseNs, AudioProcThreadPt->m_SpeexPprocNoiseSupes, AudioProcThreadPt->m_SpeexPprocIsUseDereverb, AudioProcThreadPt->m_SpeexPprocIsUseVad, AudioProcThreadPt->m_SpeexPprocVadProbStart, AudioProcThreadPt->m_SpeexPprocVadProbCont, AudioProcThreadPt->m_SpeexPprocIsUseAgc, AudioProcThreadPt->m_SpeexPprocAgcLevel, AudioProcThreadPt->m_SpeexPprocAgcIncrement, AudioProcThreadPt->m_SpeexPprocAgcDecrement, AudioProcThreadPt->m_SpeexPprocAgcMaxGain ) == 0 )
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化Speex预处理器成功。"  );
        }
        else
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化Speex预处理器失败。" );
            goto out;
        }
    }
	
    //创建并初始化编解码器。
    switch( AudioProcThreadPt->m_UseWhatCodec )
    {
        case 0: //如果使用PCM原始数据。
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用PCM原始数据。" );
            break;
        }
        case 1: //如果使用Speex编解码器。
        {
            if( AudioProcThreadPt->m_FrameLen != AudioProcThreadPt->m_SamplingRate / 1000 * 20 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：帧的数据长度不为20毫秒不能使用Speex编解码器。" );
                goto out;
            }
            if( SpeexEncoderInit( &AudioProcThreadPt->m_SpeexEncoderPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_SpeexCodecEncoderUseCbrOrVbr, AudioProcThreadPt->m_SpeexCodecEncoderQuality, AudioProcThreadPt->m_SpeexCodecEncoderComplexity, AudioProcThreadPt->m_SpeexCodecEncoderPlcExpectedLossRate ) == 0 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化Speex编码器成功。" );
            }
            else
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化Speex编码器失败。" );
                goto out;
            }
            if( SpeexDecoderInit( &AudioProcThreadPt->m_SpeexDecoderPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_SpeexCodecDecoderIsUsePerceptualEnhancement ) == 0 )
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化Speex解码器成功。" );
            }
            else
            {
                if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化Speex解码器失败。" );
                goto out;
            }
            break;
        }
        case 2: //如果使用Opus编解码器。
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：暂不支持使用Opus编解码器。" );
            goto out;
        }
    }

	//创建并初始化音频输入Wave文件写入器、音频输出Wave文件写入器、音频结果Wave文件写入器。
	if( AudioProcThreadPt->m_IsSaveAudioToFile != 0 )
	{
		if( WaveFileWriterInit( &AudioProcThreadPt->m_AudioInputWaveFileWriterPt, AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt->m_StrPt, 1, AudioProcThreadPt->m_SamplingRate, 16 ) == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器成功。", AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt->m_StrPt );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化音频输入文件 %s 的Wave文件写入器失败。", AudioProcThreadPt->m_AudioInputFileFullPathVarStrPt->m_StrPt );
			goto out;
		}
		if( WaveFileWriterInit( &AudioProcThreadPt->m_AudioOutputWaveFileWriterPt, AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt->m_StrPt, 1, AudioProcThreadPt->m_SamplingRate, 16 ) == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器成功。", AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt->m_StrPt );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化音频输出文件 %s 的Wave文件写入器失败。", AudioProcThreadPt->m_AudioOutputFileFullPathVarStrPt->m_StrPt );
			goto out;
		}
		if( WaveFileWriterInit( &AudioProcThreadPt->m_AudioResultWaveFileWriterPt, AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt->m_StrPt, 1, AudioProcThreadPt->m_SamplingRate, 16 ) == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器成功。", AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt->m_StrPt );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：创建并初始化音频结果文件 %s 的Wave文件写入器失败。", AudioProcThreadPt->m_AudioResultFileFullPathVarStrPt->m_StrPt );
			goto out;
		}
	}

	//创建并启动音频输入线程、音频输出线程，创建并初始化音频缓冲区块、音频输入设备、音频输出设备。
	{
		//创建并启动音频输入线程、音频输出线程。必须在初始化音频输入设备、音频输出设备前初始化音频输入线程、音频输出线程，因为音频输入设备、音频输出设备会使用音频输入线程ID、音频输出线程ID。
		AudioProcThreadPt->m_AudioInputThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioInputThreadRun, AudioProcThreadPt, 0, &AudioProcThreadPt->m_AudioInputThreadId );
		if( AudioProcThreadPt->m_AudioInputThreadHdl != NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并启动音频输入线程成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != NULL ) LOGFE( "音频处理线程：创建并启动音频输入线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto out; //本函数返回失败。
		}
		AudioProcThreadPt->m_AudioOutputThreadHdl = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )AudioOutputThreadRun, AudioProcThreadPt, 0, &AudioProcThreadPt->m_AudioOutputThreadId );
		if( AudioProcThreadPt->m_AudioOutputThreadHdl != NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并启动音频输出线程成功。" );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != NULL ) LOGFE( "音频处理线程：创建并启动音频输出线程失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
			goto out; //本函数返回失败。
		}

		//创建并初始化音频缓冲区块。
		if( AudioProcThreadPt->m_FrameLen / ( AudioProcThreadPt->m_SamplingRate / 1000 ) == 10 ) //如果帧的数据长度为10毫秒。
		{
			AudioProcThreadPt->m_AudioWaveHdrTotal = 2 + 4;
		}
		else if( AudioProcThreadPt->m_FrameLen / ( AudioProcThreadPt->m_SamplingRate / 1000 ) == 20 ) //如果帧的数据长度为20毫秒。
		{
			AudioProcThreadPt->m_AudioWaveHdrTotal = 2 + 3;
		}
		else //如果帧的数据长度为大于等于30毫秒。
		{
			AudioProcThreadPt->m_AudioWaveHdrTotal = 2 + 2;
		}
		AudioProcThreadPt->m_AudioWaveHdrPt = ( WAVEHDR * )calloc( 1, AudioProcThreadPt->m_AudioWaveHdrTotal * sizeof( WAVEHDR ) ); //创建并清空音频缓冲区块数组的内存块。
		if( AudioProcThreadPt->m_AudioWaveHdrPt == NULL )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：calloc 创建并清空音频缓冲区块数组的内存块失败。" );
			goto out; //本函数返回失败。
		}
		for( int i = 0; i < AudioProcThreadPt->m_AudioWaveHdrTotal; i++ )
		{
			AudioProcThreadPt->m_AudioWaveHdrPt[i].dwBufferLength = AudioProcThreadPt->m_FrameLen * sizeof( int16_t );
			AudioProcThreadPt->m_AudioWaveHdrPt[i].lpData = ( LPSTR )calloc( 1, AudioProcThreadPt->m_AudioWaveHdrPt[i].dwBufferLength );
			if( AudioProcThreadPt->m_AudioWaveHdrPt[i].lpData == NULL )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：calloc 创建并清空音频缓冲区块数组的数据内存块失败。" );
				goto out; //本函数返回失败。
			}
		}
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频缓冲区块成功。" );

		//创建并初始化音频输入设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = AudioProcThreadPt->m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = AudioProcThreadPt->m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt = waveInOpen( &AudioProcThreadPt->m_AudioInputDeviceHdl, AudioProcThreadPt->m_InputDeviceID, &WaveFormatEx, ( DWORD_PTR )AudioProcThreadPt->m_AudioInputThreadId, ( DWORD_PTR )AudioProcThreadPt, CALLBACK_THREAD );
			if( p_TmpInt != MMSYSERR_NOERROR )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：waveInOpen 打开音频输入设备失败。原因：%s", FuncGetErrInfo( p_TmpInt, 'M' ) );
				goto out;
			}
			for( int i = 0; i < 2; i++ )
			{
				p_TmpInt = waveInPrepareHeader( AudioProcThreadPt->m_AudioInputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( AudioProcThreadPt->m_AudioWaveHdrPt[i] ) );
				if( p_TmpInt != MMSYSERR_NOERROR )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：waveInPrepareHeader 为音频输入设备准备音频缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt, 'M' ) );
					goto out;
				}
				p_TmpInt = waveInAddBuffer( AudioProcThreadPt->m_AudioInputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( AudioProcThreadPt->m_AudioWaveHdrPt[i] ) );
				if( p_TmpInt != MMSYSERR_NOERROR )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：waveInAddBuffer 将已准备的音频缓冲区块添加到音频输入设备上失败。原因：%s", FuncGetErrInfo( p_TmpInt, 'M' ) );
					goto out;
				}
			}
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频输入设备成功。音频输入缓冲区大小：%d", 2 * AudioProcThreadPt->m_AudioWaveHdrPt[0].dwBufferLength );
		}

		//创建并初始化音频输出设备。
		{
			WAVEFORMATEX WaveFormatEx;
			WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormatEx.nChannels = 1;
			WaveFormatEx.nSamplesPerSec = AudioProcThreadPt->m_SamplingRate;
			WaveFormatEx.nAvgBytesPerSec = AudioProcThreadPt->m_SamplingRate * 1 * sizeof( int16_t );
			WaveFormatEx.nBlockAlign = 1 * sizeof( int16_t );
			WaveFormatEx.wBitsPerSample = sizeof( int16_t ) * 8;
			WaveFormatEx.cbSize = 0;
			p_TmpInt = waveOutOpen( &AudioProcThreadPt->m_AudioOutputDeviceHdl, AudioProcThreadPt->m_OutputDeviceID, &WaveFormatEx, ( DWORD_PTR )AudioProcThreadPt->m_AudioOutputThreadId, ( DWORD_PTR )AudioProcThreadPt, CALLBACK_THREAD );
			if( p_TmpInt != MMSYSERR_NOERROR )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：waveOutOpen 打开音频输出设备失败。原因：%s", FuncGetErrInfo( p_TmpInt, 'M' ) );
				goto out;
			}
			for( int i = 2; i < AudioProcThreadPt->m_AudioWaveHdrTotal; i++ )
			{
				p_TmpInt = waveOutPrepareHeader( AudioProcThreadPt->m_AudioOutputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( AudioProcThreadPt->m_AudioWaveHdrPt[i] ) );
				if( p_TmpInt != MMSYSERR_NOERROR )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：waveOutPrepareHeader 为音频输出设备准备音频缓冲区块失败。原因：%s", FuncGetErrInfo( p_TmpInt, 'M' ) );
					goto out;
				}
			}
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：创建并初始化音频输出设备成功。音频输出缓冲区大小：%d", ( AudioProcThreadPt->m_AudioWaveHdrTotal - 2 ) * AudioProcThreadPt->m_AudioWaveHdrPt[0].dwBufferLength );
		}

		waveInStart( AudioProcThreadPt->m_AudioInputDeviceHdl ); //让音频输入设备开始录音。
		for( int i = 2; i < AudioProcThreadPt->m_AudioWaveHdrTotal; i++ ) //让音频输出设备开始播放。
		{
			waveOutWrite( AudioProcThreadPt->m_AudioOutputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( AudioProcThreadPt->m_AudioWaveHdrPt[i] ) );
		}

		//自适应设置回音的延迟。
		int32_t p_Delay = AudioProcThreadPt->m_AudioWaveHdrTotal * ( AudioProcThreadPt->m_FrameLen / ( AudioProcThreadPt->m_SamplingRate / 1000 ) );
		if( ( AudioProcThreadPt->m_WebRtcAecmPt != NULL ) && ( WebRtcAecmGetDelay( AudioProcThreadPt->m_WebRtcAecmPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果使用了WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			p_TmpInt32 = p_Delay;
			WebRtcAecmSetDelay( AudioProcThreadPt->m_WebRtcAecmPt, p_TmpInt32 );
			WebRtcAecmGetDelay( AudioProcThreadPt->m_WebRtcAecmPt, &p_TmpInt32 );
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt32 );
		}
		if( ( AudioProcThreadPt->m_WebRtcAecPt != NULL ) && ( WebRtcAecGetDelay( AudioProcThreadPt->m_WebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果使用了WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
        {
            if( AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt32 = p_Delay;
                WebRtcAecSetDelay( AudioProcThreadPt->m_WebRtcAecPt, p_TmpInt32 );
                WebRtcAecGetDelay( AudioProcThreadPt->m_WebRtcAecPt, &p_TmpInt32 );
            }
            else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                WebRtcAecSetDelay( AudioProcThreadPt->m_WebRtcAecPt, 20 );
                WebRtcAecGetDelay( AudioProcThreadPt->m_WebRtcAecPt, &p_TmpInt32 );
            }
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt32 );
        }
		if( ( AudioProcThreadPt->m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果使用了SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
        {
			p_TmpInt32 = p_Delay;
            SpeexWebRtcAecSetWebRtcAecmDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, p_TmpInt32 );
            SpeexWebRtcAecGetWebRtcAecmDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, &p_TmpInt32 );
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt32 );
        }
		if( ( AudioProcThreadPt->m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果使用了SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
        {
            if( AudioProcThreadPt->m_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
            {
				p_TmpInt32 = p_Delay;
                SpeexWebRtcAecSetWebRtcAecDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, p_TmpInt32 );
                SpeexWebRtcAecGetWebRtcAecDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, &p_TmpInt32 );
            }
            else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
            {
                SpeexWebRtcAecSetWebRtcAecDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, 20 );
                SpeexWebRtcAecGetWebRtcAecDelay( AudioProcThreadPt->m_SpeexWebRtcAecPt, &p_TmpInt32 );
            }
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %" PRId32 " 毫秒。", p_TmpInt32 );
        }
	}

	AudioProcThreadPt->m_ExitCode = -2; //初始化已经成功了，再将本线程退出代码预设为处理失败，如果处理失败，这个退出代码就不用再设置了，如果处理成功，再设置为成功的退出代码。

	if( AudioProcThreadPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec );
		LOGFI( "音频处理线程：音频处理线程初始化完毕，耗时：%" PRIu64 " 毫秒，正式开始处理音频。", p_NowMsec - p_LastMsec );
	}

	while( 1 )
	{
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec );

		//调用用户定义的处理函数。
		p_TmpInt = AudioProcThreadPt->m_UserProcessFuncPt( AudioProcThreadPt );
		if( p_TmpInt == 0 )
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：调用用户定义的处理函数成功。返回值：%d", p_TmpInt );
		}
		else
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：调用用户定义的处理函数失败。返回值：%d", p_TmpInt );
			goto out;
		}

		//开始处理输入帧。
		ConstLenLnkLstGetTotal( AudioProcThreadPt->m_InputFrameLnkLstPt, &p_TmpSz1, NULL );
		ConstLenLnkLstGetTotal( AudioProcThreadPt->m_OutputFrameLnkLstPt, &p_TmpSz2, NULL );
		if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) || //如果输入帧链表和输出帧链表中都有帧了，才开始处理。
			( p_TmpSz1 > 15 ) ) //如果输入帧链表里已经累积很多输入帧了，说明输出帧链表里迟迟没有音频输出帧，也开始处理。
		{
			//从输入帧链表中取出第一个输入帧。
			{
				MutexLockLock( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, NULL );
				ConstLenLnkLstGetHead( AudioProcThreadPt->m_InputFrameLnkLstPt, NULL, p_PcmInputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( AudioProcThreadPt->m_InputFrameLnkLstPt, NULL );
				MutexLockUnlock( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, NULL );
			}
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：从输入帧链表中取出第一个输入帧。" );

			//从输出帧链表中取出第一个输出帧。
			if( p_TmpSz2 > 0 ) //如果输出帧链表里有输出帧。
			{
				MutexLockLock( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, NULL );
				ConstLenLnkLstGetHead( AudioProcThreadPt->m_OutputFrameLnkLstPt, NULL, p_PcmOutputFramePt, NULL, NULL );
				ConstLenLnkLstDelHead( AudioProcThreadPt->m_OutputFrameLnkLstPt, NULL );
				MutexLockUnlock( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, NULL );
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：从输出帧链表中取出第一个输出帧。" );
			}
			else //如果输出帧链表里没有输出帧。
			{
				memset( p_PcmOutputFramePt, 0, sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：输出帧链表中没有输出帧，用一个空帧代替。" );
			}

			//将输入帧复制到结果帧，方便处理。
			memcpy( p_PcmResultFramePt, p_PcmInputFramePt, sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );

			//使用声学回音消除器。
            switch( AudioProcThreadPt->m_UseWhatAec )
            {
                case 0: //如果不使用声学回音消除器。
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：不使用声学回音消除器。" );
					break;
				}
                case 1: //如果使用Speex声学回音消除器。
				{
					if( SpeexAecProc( AudioProcThreadPt->m_SpeexAecPt, p_PcmResultFramePt, p_PcmOutputFramePt, p_PcmTmpFramePt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用Speex声学回音消除器成功。" );
						p_PcmSwapFramePt = p_PcmResultFramePt; p_PcmResultFramePt = p_PcmTmpFramePt; p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用Speex声学回音消除器失败。" );
					}
					break;
				}
                case 2: //如果使用WebRtc定点版声学回音消除器。
				{
					if( WebRtcAecmProc( AudioProcThreadPt->m_WebRtcAecmPt, p_PcmResultFramePt, p_PcmOutputFramePt, p_PcmTmpFramePt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用WebRtc定点版声学回音消除器成功。" );
						p_PcmSwapFramePt = p_PcmResultFramePt; p_PcmResultFramePt = p_PcmTmpFramePt; p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用WebRtc定点版声学回音消除器失败。" );
					}
					break;
				}
                case 3: //如果使用WebRtc浮点版声学回音消除器。
				{
					if( WebRtcAecProc( AudioProcThreadPt->m_WebRtcAecPt, p_PcmResultFramePt, p_PcmOutputFramePt, p_PcmTmpFramePt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用WebRtc浮点版声学回音消除器成功。" );
						p_PcmSwapFramePt = p_PcmResultFramePt; p_PcmResultFramePt = p_PcmTmpFramePt; p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用WebRtc浮点版声学回音消除器失败。" );
					}
					break;
				}
                case 4: //如果使用SpeexWebRtc三重浮点版声学回音消除器。
				{
					if( SpeexWebRtcAecProc( AudioProcThreadPt->m_SpeexWebRtcAecPt, p_PcmResultFramePt, p_PcmOutputFramePt, p_PcmTmpFramePt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用SpeexWebRtc三重浮点版声学回音消除器成功。" );
						p_PcmSwapFramePt = p_PcmResultFramePt; p_PcmResultFramePt = p_PcmTmpFramePt; p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用SpeexWebRtc三重浮点版声学回音消除器失败。" );
					}
					break;
				}
            }

            //使用噪音抑制器。
            switch( AudioProcThreadPt->m_UseWhatNs )
            {
                case 0: //如果不使用噪音抑制器。
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：不使用噪音抑制器。" );
                    break;
                }
                case 1: //如果使用Speex预处理器的噪音抑制。
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：稍后在使用Speex预处理器时一起使用噪音抑制。" );
                    break;
                }
                case 2: //如果使用WebRtc定点版噪音抑制器。
                {
                    if( WebRtcNsxProc( AudioProcThreadPt->m_WebRtcNsxPt, p_PcmResultFramePt, p_PcmTmpFramePt ) == 0 )
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用WebRtc定点版噪音抑制器成功。" );
                        p_PcmSwapFramePt = p_PcmResultFramePt;p_PcmResultFramePt = p_PcmTmpFramePt;p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
                    }
                    else
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用WebRtc定点版噪音抑制器失败。" );
                    }
                    break;
                }
                case 3: //如果使用WebRtc浮点版噪音抑制器。
                {
                    if( WebRtcNsProc( AudioProcThreadPt->m_WebRtcNsPt, p_PcmResultFramePt, p_PcmTmpFramePt ) == 0 )
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用WebRtc浮点版噪音抑制器成功。" );
                        p_PcmSwapFramePt = p_PcmResultFramePt;p_PcmResultFramePt = p_PcmTmpFramePt;p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
                    }
                    else
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用WebRtc浮点版噪音抑制器失败。" );
                    }
                    break;
                }
                case 4: //如果使用RNNoise噪音抑制器。
                {
                    if( RNNoiseProc( AudioProcThreadPt->m_RNNoisePt, p_PcmResultFramePt, p_PcmTmpFramePt ) == 0 )
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用RNNoise噪音抑制器成功。" );
                        p_PcmSwapFramePt = p_PcmResultFramePt;p_PcmResultFramePt = p_PcmTmpFramePt;p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
                    }
                    else
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用RNNoise噪音抑制器失败。" );
                    }
                    break;
                }
            }
			
            //使用Speex预处理器。
            if( ( AudioProcThreadPt->m_UseWhatNs == 1 ) || ( AudioProcThreadPt->m_IsUseSpeexPprocOther != 0 ) )
            {
                if( SpeexPprocProc( AudioProcThreadPt->m_SpeexPprocPt, p_PcmResultFramePt, p_PcmTmpFramePt, &p_VoiceActSts ) == 0 )
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用Speex预处理器成功。语音活动状态：%d", p_VoiceActSts );
                    p_PcmSwapFramePt = p_PcmResultFramePt;p_PcmResultFramePt = p_PcmTmpFramePt;p_PcmTmpFramePt = p_PcmSwapFramePt; //交换结果帧和临时帧。
                }
                else
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用Speex预处理器失败。" );
                }
            }
			
            //判断音频输入设备是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
            if( AudioProcThreadPt->m_AudioInputDeviceIsMute != 0 )
            {
				memset( p_PcmResultFramePt, 0, sizeof( int16_t ) * AudioProcThreadPt->m_FrameLen );
                if( ( AudioProcThreadPt->m_IsUseSpeexPprocOther != 0 ) && ( AudioProcThreadPt->m_SpeexPprocIsUseVad != 0 ) ) //如果Speex预处理器使用了其他功能，且使用了语音活动检测。
                {
                    p_VoiceActSts = 0;
                }
            }

            //使用编码器。
            switch( AudioProcThreadPt->m_UseWhatCodec )
            {
                case 0: //如果使用PCM原始数据。
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用PCM原始数据。" );
                    break;
                }
                case 1: //如果使用Speex编码器。
                {
                    if( SpeexEncoderProc( AudioProcThreadPt->m_SpeexEncoderPt, p_PcmResultFramePt, p_SpeexInputFramePt, p_SpeexInputFrameSz, &p_SpeexInputFrameLen, &p_SpeexInputFrameIsNeedTrans ) == 0 )
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用Speex编码器成功。Speex格式输入帧的数据长度：%" PRIuPTR "，Speex格式输入帧是否需要传输：%" PRIi32, p_SpeexInputFrameLen, p_SpeexInputFrameIsNeedTrans );
                    }
                    else
                    {
                        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用Speex编码器失败。" );
                    }
                    break;
                }
                case 2: //如果使用Opus编码器。
                {
                    if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：暂不支持使用Opus编码器。" );
                    goto out;
                }
            }

			//使用音频输入Wave文件写入器写入输入帧数据、音频输出Wave文件写入器写入输出帧数据、音频结果Wave文件写入器写入结果帧数据。
			if( AudioProcThreadPt->m_IsSaveAudioToFile != 0 )
			{
				if( WaveFileWriterWriteData( AudioProcThreadPt->m_AudioInputWaveFileWriterPt, ( char * )p_PcmInputFramePt, AudioProcThreadPt->m_FrameLen * 2 ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用音频输入Wave文件写入器写入输入帧数据成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用音频输入Wave文件写入器写入输入帧数据失败。" );
				}
				if( WaveFileWriterWriteData( AudioProcThreadPt->m_AudioOutputWaveFileWriterPt, ( char * )p_PcmOutputFramePt, AudioProcThreadPt->m_FrameLen * 2 ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用音频输出Wave文件写入器写入输出帧数据成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用音频输出Wave文件写入器写入输出帧数据失败。" );
				}
				if( WaveFileWriterWriteData( AudioProcThreadPt->m_AudioResultWaveFileWriterPt, ( char * )p_PcmResultFramePt, AudioProcThreadPt->m_FrameLen * 2 ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：使用音频结果Wave文件写入器写入结果帧数据成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：使用音频结果Wave文件写入器写入结果帧数据失败。" );
				}
			}

			//调用用户定义的读取输入帧函数。
			p_TmpInt = AudioProcThreadPt->m_UserReadInputFrameFuncPt( AudioProcThreadPt, p_PcmInputFramePt, p_PcmResultFramePt, p_VoiceActSts, p_SpeexInputFramePt, p_SpeexInputFrameLen, p_SpeexInputFrameIsNeedTrans );
			if( p_TmpInt == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：调用用户定义的读取输入帧函数成功。返回值：%d", p_TmpInt );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：调用用户定义的读取输入帧函数失败。返回值：%d", p_TmpInt );
				goto out;
			}

			if( AudioProcThreadPt->m_IsPrintLog != 0 )
			{
				FuncGetTimeAsMsec( &p_NowMsec );
				LOGFI( "音频处理线程：本音频帧处理完毕，耗时：%" PRIu64 " 毫秒。", p_NowMsec - p_LastMsec );
			}
		}

		//如果音频输入线程因为设备关闭而退出了，就重启音频处理线程。
		GetExitCodeThread( AudioProcThreadPt->m_AudioInputThreadHdl, ( LPDWORD )&p_TmpInt32 );
		if( p_TmpInt32 != STILL_ACTIVE )
		{
			LOGFI( "音频处理线程：音频输入线程因为设备关闭而退出了，准备重启音频处理线程。" );

			AudioProcThreadPt->m_ExitFlag = 3;
		}

		//如果音频输出线程因为设备关闭而退出了，就重启音频处理线程。
		GetExitCodeThread( AudioProcThreadPt->m_AudioOutputThreadHdl, ( LPDWORD )&p_TmpInt32 );
		if( p_TmpInt32 != STILL_ACTIVE )
		{
			LOGFI( "音频处理线程：音频输出线程因为设备关闭而退出了，准备重启音频处理线程。" );

			AudioProcThreadPt->m_ExitFlag = 3;
		}

		if( AudioProcThreadPt->m_ExitFlag != 0 ) //如果本线程退出标记为请求退出。
		{
			AudioProcThreadPt->m_ExitCode = 0; //处理已经成功了，再将本线程退出代码设置为正常退出。
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：接收到退出请求，开始准备退出。退出标记：%d。", AudioProcThreadPt->m_ExitFlag );
			goto out;
		}

		Sleep( 1 ); //暂停一下，避免CPU使用率过高。
	}

	out:
	if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：本线程开始退出。" );
	
	//销毁音频输入线程、音频输出线程、音频输入设备、音频输出设备、音频缓冲区块。
	{
		{//必须在销毁音频输入、音频输出前销毁音频输入线程、音频输出线程，因为音频输入线程、音频输出线程会使用音频输入设备、音频输出设备。
			if( AudioProcThreadPt->m_AudioInputThreadHdl != NULL ) PostThreadMessage( AudioProcThreadPt->m_AudioInputThreadId, WM_QUIT, NULL, NULL ); //请求音频输入线程退出。不要使用退出标记，因为音频输入设备可能会打开失败，导致音频输入线程不会进入消息循环。
			if( AudioProcThreadPt->m_AudioOutputThreadHdl != NULL ) PostThreadMessage( AudioProcThreadPt->m_AudioOutputThreadId, WM_QUIT, NULL, NULL ); //请求音频输出线程退出。不要使用退出标记，因为音频输出设备可能会打开失败，导致音频输出线程不会进入消息循环。
			if( AudioProcThreadPt->m_AudioInputThreadHdl != NULL )
			{
				WaitForSingleObject( AudioProcThreadPt->m_AudioInputThreadHdl, INFINITE ); //等待音频输入线程退出。
				CloseHandle( AudioProcThreadPt->m_AudioInputThreadHdl );
				AudioProcThreadPt->m_AudioInputThreadHdl = NULL;
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输入线程成功。" );
			}
			if( AudioProcThreadPt->m_AudioOutputThreadHdl != NULL )
			{
				WaitForSingleObject( AudioProcThreadPt->m_AudioOutputThreadHdl, INFINITE ); //等待音频输出线程退出。
				CloseHandle( AudioProcThreadPt->m_AudioOutputThreadHdl );
				AudioProcThreadPt->m_AudioOutputThreadHdl = NULL;
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输出线程成功。" );
			}
		}
		if( AudioProcThreadPt->m_AudioInputDeviceHdl != NULL )
		{
			for( int i = 0; i < 2; i++ )
			{
				waveInUnprepareHeader( AudioProcThreadPt->m_AudioInputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( WAVEHDR ) );
			}
			waveInReset( AudioProcThreadPt->m_AudioInputDeviceHdl );
			waveInClose( AudioProcThreadPt->m_AudioInputDeviceHdl );
			AudioProcThreadPt->m_AudioInputDeviceHdl = NULL;
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输入设备成功。" );
		}
		if( AudioProcThreadPt->m_AudioOutputDeviceHdl != NULL )
		{
			for( int i = 2; i < AudioProcThreadPt->m_AudioWaveHdrTotal; i++ )
			{
				waveOutUnprepareHeader( AudioProcThreadPt->m_AudioOutputDeviceHdl, &AudioProcThreadPt->m_AudioWaveHdrPt[i], sizeof( WAVEHDR ) );
			}
			waveOutReset( AudioProcThreadPt->m_AudioOutputDeviceHdl );
			waveOutClose( AudioProcThreadPt->m_AudioOutputDeviceHdl );
			AudioProcThreadPt->m_AudioOutputDeviceHdl = NULL;
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输出设备成功。" );
		}
		if( AudioProcThreadPt->m_AudioWaveHdrPt != NULL )
		{
			for( int i = 0; i < AudioProcThreadPt->m_AudioWaveHdrTotal; i++ )
			{
				if( AudioProcThreadPt->m_AudioWaveHdrPt[i].lpData != NULL )
				{
					free( AudioProcThreadPt->m_AudioWaveHdrPt[i].lpData );
					AudioProcThreadPt->m_AudioWaveHdrPt[i].lpData = NULL;
				}
			}
			free( AudioProcThreadPt->m_AudioWaveHdrPt );
			AudioProcThreadPt->m_AudioWaveHdrPt = NULL;
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频缓冲区块成功。" );
		}
	}

	//销毁音频输入Wave文件写入器、音频输出Wave文件写入器、音频结果Wave文件写入器。
	if( AudioProcThreadPt->m_IsSaveAudioToFile != 0 )
	{
		if( AudioProcThreadPt->m_AudioInputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( AudioProcThreadPt->m_AudioInputWaveFileWriterPt ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输入Wave文件写入器成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁音频输入Wave文件写入器失败。" );
			}
			AudioProcThreadPt->m_AudioInputWaveFileWriterPt = NULL;
		}
		if( AudioProcThreadPt->m_AudioOutputWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( AudioProcThreadPt->m_AudioOutputWaveFileWriterPt ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频输出Wave文件写入器成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁音频输出Wave文件写入器失败。" );
			}
			AudioProcThreadPt->m_AudioOutputWaveFileWriterPt = NULL;
		}
		if( AudioProcThreadPt->m_AudioResultWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDestroy( AudioProcThreadPt->m_AudioResultWaveFileWriterPt ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁音频结果Wave文件写入器成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁音频结果Wave文件写入器失败。" );
			}
			AudioProcThreadPt->m_AudioResultWaveFileWriterPt = NULL;
		}
	}
	
    //销毁编码器。
    switch( AudioProcThreadPt->m_UseWhatCodec )
    {
        case 0: //如果使用PCM原始数据。
        {
            break;
        }
        case 1: //如果使用Speex编码器。
        {
			if( AudioProcThreadPt->m_SpeexEncoderPt != NULL )
			{
				if( SpeexEncoderDestroy( AudioProcThreadPt->m_SpeexEncoderPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁Speex编码器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁Speex编码器失败。" );
				}
				AudioProcThreadPt->m_SpeexEncoderPt = NULL;
			}
			if( AudioProcThreadPt->m_SpeexDecoderPt != NULL )
			{
				if( SpeexDecoderDestroy( AudioProcThreadPt->m_SpeexDecoderPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁Speex解码器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁Speex解码器失败。" );
				}
				AudioProcThreadPt->m_SpeexDecoderPt = NULL;
			}
            break;
        }
        case 2: //如果使用Opus编码器。
        {
            break;
        }
    }

    //销毁Speex预处理器。
    if( AudioProcThreadPt->m_SpeexPprocPt != NULL )
    {
        if( SpeexPprocDestroy( AudioProcThreadPt->m_SpeexPprocPt ) == 0 )
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁Speex预处理器成功。" );
        }
        else
        {
            if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁Speex预处理器失败。" );
        }
        AudioProcThreadPt->m_SpeexPprocPt = NULL;
    }

	//销毁噪音抑制器。
	switch( AudioProcThreadPt->m_UseWhatNs )
    {
        case 0: //如果不使用噪音抑制器。
		{
			break;
		}
		case 1: //如果使用Speex预处理器的噪音抑制。
		{
			if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：之前在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" );
			break;
		}
		case 2: //如果使用WebRtc定点版噪音抑制器。
		{
			if( AudioProcThreadPt->m_WebRtcNsxPt != NULL )
			{
				if( WebRtcNsxDestroy( AudioProcThreadPt->m_WebRtcNsxPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁WebRtc定点版噪音抑制器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁WebRtc定点版噪音抑制器失败。" );
				}
				AudioProcThreadPt->m_WebRtcNsxPt = NULL;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版噪音抑制器。
		{
			if( AudioProcThreadPt->m_WebRtcNsPt != NULL )
			{
				if( WebRtcNsDestroy( AudioProcThreadPt->m_WebRtcNsPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁WebRtc浮点版噪音抑制器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁WebRtc浮点版噪音抑制器失败。" );
				}
				AudioProcThreadPt->m_WebRtcNsPt = NULL;
			}
			break;
		}
		case 4: //如果使用RNNoise噪音抑制器。
		{
			if( AudioProcThreadPt->m_RNNoisePt != NULL )
			{
				if( RNNoiseDestroy( AudioProcThreadPt->m_RNNoisePt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁RNNoise噪音抑制器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁RNNoise噪音抑制器失败。" );
				}
				AudioProcThreadPt->m_RNNoisePt = NULL;
			}
			break;
		}
	}

	//销毁声学回音消除器。
	switch( AudioProcThreadPt->m_UseWhatAec )
	{
		case 0: //如果不使用声学回音消除器。
		{
			break;
		}
		case 1: //如果使用Speex声学回音消除器。
		{
			if( AudioProcThreadPt->m_SpeexAecPt != NULL )
			{
				if( AudioProcThreadPt->m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
				{
					if( SpeexAecSaveMemFile( AudioProcThreadPt->m_SpeexAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_SpeexAecFilterLen, AudioProcThreadPt->m_SpeexAecIsUseRec, AudioProcThreadPt->m_SpeexAecEchoMultiple, AudioProcThreadPt->m_SpeexAecEchoCont, AudioProcThreadPt->m_SpeexAecEchoSupes, AudioProcThreadPt->m_SpeexAecEchoSupesAct, AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 成功。", AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt );
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：将Speex声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", AudioProcThreadPt->m_SpeexAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					}
				}
				if( SpeexAecDestroy( AudioProcThreadPt->m_SpeexAecPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁Speex声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁Speex声学回音消除器失败。" );
				}
				AudioProcThreadPt->m_SpeexAecPt = NULL;
			}
			break;
		}
		case 2: //如果使用WebRtc定点版声学回音消除器。
		{
			if( AudioProcThreadPt->m_WebRtcAecmPt != NULL )
			{
				if( WebRtcAecmDestroy( AudioProcThreadPt->m_WebRtcAecmPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁WebRtc定点版声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁WebRtc定点版声学回音消除器失败。" );
				}
				AudioProcThreadPt->m_WebRtcAecmPt = NULL;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版声学回音消除器。
		{
			if( AudioProcThreadPt->m_WebRtcAecPt != NULL )
			{
				if( AudioProcThreadPt->m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
				{
					if( WebRtcAecSaveMemFile( AudioProcThreadPt->m_WebRtcAecPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, AudioProcThreadPt->m_WebRtcAecEchoMode, AudioProcThreadPt->m_WebRtcAecDelay, AudioProcThreadPt->m_WebRtcAecIsUseDelayAgnosticMode, AudioProcThreadPt->m_WebRtcAecIsUseExtdFilterMode, AudioProcThreadPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AudioProcThreadPt->m_WebRtcAecIsUseAdaptAdjDelay, AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 成功。", AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt );
					}
					else
					{
						if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %s 失败。原因：%s", AudioProcThreadPt->m_WebRtcAecMemFileFullPathVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					}
				}
				if( WebRtcAecDestroy( AudioProcThreadPt->m_WebRtcAecPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁WebRtc浮点版声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁WebRtc浮点版声学回音消除器失败。" );
				}
				AudioProcThreadPt->m_WebRtcAecPt = NULL;
			}
			break;
		}
		case 4: //如果使用SpeexWebRtc三重声学回音消除器。
        {
			if( AudioProcThreadPt->m_SpeexWebRtcAecPt != NULL )
			{
				if( SpeexWebRtcAecDestroy( AudioProcThreadPt->m_SpeexWebRtcAecPt ) == 0 )
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁SpeexWebRtc三重声学回音消除器成功。" );
				}
				else
				{
					if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁SpeexWebRtc三重声学回音消除器失败。" );
				}
				AudioProcThreadPt->m_SpeexWebRtcAecPt = NULL;
			}
            break;
        }
	}

	//销毁输入帧链表、输入帧链表互斥锁、输出帧链表、输出帧链表互斥锁。
	{
		if( AudioProcThreadPt->m_InputFrameLnkLstPt != NULL )
		{
			if( ConstLenLnkLstDestroy( AudioProcThreadPt->m_InputFrameLnkLstPt, NULL ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁输入帧链表成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁输入帧链表失败。" );
			}
			AudioProcThreadPt->m_InputFrameLnkLstPt = NULL;
		}
		if( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt != NULL )
		{
			if( MutexLockDestroy( AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt, NULL ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁输入帧链表互斥锁成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁输入帧链表互斥锁失败。" );
			}
			AudioProcThreadPt->m_InputFrameLnkLstMutexLockPt = NULL;
		}
		if( AudioProcThreadPt->m_OutputFrameLnkLstPt != NULL )
		{
			if( ConstLenLnkLstDestroy( AudioProcThreadPt->m_OutputFrameLnkLstPt, NULL ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁输出帧链表成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁输出帧链表失败。" );
			}
			AudioProcThreadPt->m_OutputFrameLnkLstPt = NULL;
		}
		if( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt != NULL )
		{
			if( MutexLockDestroy( AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt, NULL ) == 0 )
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁输出帧链表互斥锁成功。" );
			}
			else
			{
				if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFE( "音频处理线程：销毁输出帧链表互斥锁失败。" );
			}
			AudioProcThreadPt->m_OutputFrameLnkLstMutexLockPt = NULL;
		}
	}

	//销毁PCM格式输入帧、PCM格式输出帧、PCM格式结果帧、PCM格式临时帧、PCM格式交换帧、语音活动状态、Speex格式输入帧、Speex格式输入帧的内存长度、Speex格式输入帧的数据长度、Speex格式输入帧是否需要传输。
	{
		if( p_PcmInputFramePt != NULL ) //销毁PCM格式输入帧的内存块。
		{
			free( p_PcmInputFramePt );
			p_PcmInputFramePt = NULL;
		}
		if( p_PcmOutputFramePt != NULL ) //销毁PCM格式音频输出帧的内存块。
		{
			free( p_PcmOutputFramePt );
			p_PcmOutputFramePt = NULL;
		}
		if( p_PcmResultFramePt != NULL ) //销毁PCM格式结果帧的内存块。
		{
			free( p_PcmResultFramePt );
			p_PcmResultFramePt = NULL;
		}
		if( p_PcmTmpFramePt != NULL ) //销毁PCM格式临时帧的内存块。
		{
			free( p_PcmTmpFramePt );
			p_PcmTmpFramePt = NULL;
		}
		p_PcmSwapFramePt = NULL;
		p_VoiceActSts = 0; //销毁语音活动状态。
		if( AudioProcThreadPt->m_UseWhatCodec == 1 )
		{
			if( p_SpeexInputFramePt != NULL )
			{
				free( p_SpeexInputFramePt );
			}
		}
		p_SpeexInputFramePt = NULL;
		p_SpeexInputFrameSz = 0;
		p_SpeexInputFrameLen = 0;
		p_SpeexInputFrameIsNeedTrans = 0; //销毁Speex格式输入帧是否需要传输。
		if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：销毁PCM格式输入帧、PCM格式输出帧、PCM格式结果帧、PCM格式临时帧、PCM格式交换帧、语音活动状态、Speex格式输入帧、Speex格式输入帧的内存长度、Speex格式输入帧的数据长度、Speex格式输入帧是否需要传输成功。" );
	}

    if( AudioProcThreadPt->m_ExitFlag != 3 ) //如果需要调用用户定义的销毁函数。
    {
        AudioProcThreadPt->m_UserDestroyFuncPt( AudioProcThreadPt ); //调用用户定义的销毁函数。
        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：调用用户定义的销毁函数成功。" );
    }
	
	if( ( AudioProcThreadPt->m_ExitFlag == 0 ) || ( AudioProcThreadPt->m_ExitFlag == 1 ) ) //如果用户需要直接退出。
    {
        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：本线程已退出。" );
    }
    else //如果用户需用重新初始化。
    {
        if( AudioProcThreadPt->m_IsPrintLog != 0 ) LOGFI( "音频处理线程：本线程重新初始化。" );
		goto ReInit;
    }

	AudioProcThreadPt->m_AudioProcThreadHdl = NULL; //设置音频处理线程的线程句柄。

	return 0;
}
