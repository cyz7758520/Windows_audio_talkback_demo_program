#include "MediaPocsThrd.h"

DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdInit
 * 功能说明：初始化媒体处理线程。
 * 参数说明：MediaPocsThrdPtPt：[输出]，存放媒体处理线程指针的指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的指针，不能为NULL。
			 UserPocsFuncPt：[输入]，存放用户定义的处理函数的指针，不能为NULL。
			 UserDstoyFuncPt：[输入]，存放用户定义的销毁函数的指针，不能为NULL。
			 UserMsgFuncPt：[输入]，存放用户定义的消息函数的指针，不能为NULL。
			 UserReadAdoVdoInptFrmFuncPt：[输入]，存放用户定义的读取音视频输入帧函数的指针，不能为NULL。
			 UserWriteAdoOtptFrmFuncPt：[输入]，存放用户定义的写入音频输出帧函数的指针，不能为NULL。
			 UserGetAdoOtptFrmFuncPt：[输入]，存放用户定义的获取音频输出帧函数的指针，不能为NULL。
			 UserWriteVdoOtptFrmFuncPt：[输入]，存放用户定义的写入视频输出帧函数，不能为NULL。
			 UserGetVdoOtptFrmFuncPt：[输入]，存放用户定义的获取视频输出帧函数，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
					   MediaPocsThrd::USER_INIT_FUNC_PT UserInitFuncPt, MediaPocsThrd::USER_POCS_FUNC_PT UserPocsFuncPt, MediaPocsThrd::USER_DSTOY_FUNC_PT UserDstoyFuncPt, MediaPocsThrd::USER_MSG_FUNC_PT UserMsgFuncPt,
					   MediaPocsThrd::USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   MediaPocsThrd::USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, MediaPocsThrd::USER_GET_ADO_OTPT_FRM_FUNC_PT UserGetAdoOtptFrmFuncPt,
					   MediaPocsThrd::USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, MediaPocsThrd::USER_GET_VDO_OTPT_FRM_FUNC_PT UserGetVdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd * p_MediaPocsThrdPt = NULL; //存放媒体处理线程的指针。
	
	//判断各个变量是否正确。
	if( MediaPocsThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程指针的指针不正确。" ) );
		goto Out;
	}
	if( UserInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的初始化函数的指针不正确。" ) );
		goto Out;
	}
	if( UserPocsFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的处理函数的指针不正确。" ) );
		goto Out;
	}
	if( UserDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的销毁函数的指针不正确。" ) );
		goto Out;
	}
	if( UserMsgFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的消息函数的指针不正确。" ) );
		goto Out;
	}
	if( UserReadAdoVdoInptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的读取音视频输入帧函数的指针不正确。" ) );
		goto Out;
	}
	if( UserWriteAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入音频输出帧函数的指针不正确。" ) );
		goto Out;
	}
	if( UserGetAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取音频输出帧函数的指针不正确。" ) );
		goto Out;
	}
	if( UserWriteVdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入视频输出帧函数的指针不正确。" ) );
		goto Out;
	}
	if( UserGetVdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取视频输出帧函数的指针不正确。" ) );
		goto Out;
	}
	
	//创建并清空媒体处理线程的内存块。
	p_MediaPocsThrdPt = ( MediaPocsThrd * )calloc( 1, sizeof( MediaPocsThrd ) );
	if( p_MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空媒体处理线程的内存块失败。" ) );
		goto Out;
	}
	
	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。放在这里是防止下面的代码初始化失败导致跳过设置最小计时器分辨率，从而在调用销毁函数时多取消了一次最小计时器分辨率。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置最小计时器分辨率失败。" ) );
		goto Out;
	}
	
	p_MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 1; //设置上一次调用了用户定义的销毁函数。
	p_MediaPocsThrdPt->m_ReadyExitCnt = 1; //设置准备退出计数为1，当第一次处理调用用户定义的初始化函数消息时会递减。

	if( p_MediaPocsThrdPt->m_MediaMsgCntnr.Init( 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化媒体消息容器失败。原因：" ) );
		goto Out;
	}

	//初始化音频输入。
	p_MediaPocsThrdPt->m_AdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输入的媒体处理线程的指针。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入Speex声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入的原始Wave文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入的结果Wave文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	MediaPocsThrdSetAdoInpt( p_MediaPocsThrdPt, 8000, 20, NULL );

	//初始化音频输出。
	p_MediaPocsThrdPt->m_AdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.Init( sizeof( AdoOtpt::Strm ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输出的流容器失败。原因：" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输出的原始Wave文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	MediaPocsThrdSetAdoOtpt( p_MediaPocsThrdPt, 8000, 20, NULL );

	//初始化视频输入。
	p_MediaPocsThrdPt->m_VdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输入的媒体处理线程的指针。
	MediaPocsThrdSetVdoInpt( p_MediaPocsThrdPt, 15, 480, 640, NULL, NULL );
	MediaPocsThrdVdoInptSetUseDvc( p_MediaPocsThrdPt, 0, NULL );

	//初始化视频输出。
	p_MediaPocsThrdPt->m_VdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.Init( sizeof( VdoOtpt::Strm ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化视频输出的流容器失败。原因：" ) );
		goto Out;
	}
	
	//初始化音视频输入输出Avi文件的完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音视频输入输出Avi文件的完整路径动态字符串失败。" ) );
		goto Out;
	}

	//初始化错误信息动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_ErrInfoVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化错误信息动态字符串失败。" ) );
		goto Out;
	}

	p_MediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_MediaPocsThrdPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的指针。
	p_MediaPocsThrdPt->m_UserPocsFuncPt = UserPocsFuncPt; //设置用户定义的处理函数的指针。
	p_MediaPocsThrdPt->m_UserDstoyFuncPt = UserDstoyFuncPt; //设置用户定义的销毁函数的指针。
	p_MediaPocsThrdPt->m_UserMsgFuncPt = UserMsgFuncPt; //设置用户定义的消息函数的指针。
	p_MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt = UserReadAdoVdoInptFrmFuncPt; //设置用户定义的读取音视频输入帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt = UserWriteAdoOtptFrmFuncPt; //设置用户定义的写入音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetAdoOtptFrmFuncPt = UserGetAdoOtptFrmFuncPt; //设置用户定义的获取音频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt = UserWriteVdoOtptFrmFuncPt; //设置用户定义的写入视频输出帧函数的指针。
	p_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt = UserGetVdoOtptFrmFuncPt; //设置用户定义的获取视频输出帧函数的指针。

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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 1, NULL ); //请求媒体处理线程退出，并阻塞等待。
	
	//销毁错误信息动态字符串。
	if( MediaPocsThrdPt->m_ErrInfoVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_ErrInfoVstrPt );
		MediaPocsThrdPt->m_ErrInfoVstrPt = NULL;
	}
	
	//销毁音视频输入输出Avi文件的完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt );
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt = NULL;
	}

	//销毁媒体处理线程信息。
	if( MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		ThrdWaitDstoy( MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt, NULL );
		MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt = NULL;
	}

	//销毁视频输出的流容器。
	if( MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.m_CQueuePt != NULL )
	{
		MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.Dstoy( NULL );
	}

	//销毁音频输出的原始Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt = NULL;
	}
	
	//销毁音频输出的流容器。
	if( MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.m_CQueuePt != NULL )
	{
		MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.Dstoy( NULL );
	}

	//销毁音频输入的结果Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt = NULL;
	}
	
	//销毁音频输入的原始Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt = NULL;
	}

	//销毁音频输入WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt = NULL;
	}

	//销毁音频输入Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt = NULL;
	}
	
	//销毁媒体消息容器。
	if( MediaPocsThrdPt->m_MediaMsgCntnr.m_VQueuePt != NULL )
	{
		MediaPocsThrdPt->m_MediaMsgCntnr.Dstoy( NULL );
	}

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	//销毁媒体处理线程的内存块。
	free( MediaPocsThrdPt );

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
 * 参数说明：AdoInptDvcNameVstrArrPtPtPt：[输出]，存放音频输入设备名称动态字符串数组指针的指针，不能为NULL。
			 AdoInptDvcTotalPt：[输出]，存放音频输入设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoInptDvcName( Vstr * * * AdoInptDvcNameVstrArrPtPtPt, UINT * AdoInptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_AdoInptDvcNameArrPtPt = NULL; //存放音频输入设备名称动态字符串数组的指针。
	UINT p_AdoInptDvcTotal = 0; //存放音频输入设备的总数。
	IMMDeviceEnumerator * p_AdoInptDvcEnumPt = NULL; //存放音频输入设备枚举器的指针。
	IMMDeviceCollection * p_AdoInptDvcClctPt = NULL; //存放音频输入设备收集器的指针。
	IMMDevice * p_AdoInptDvcPt = NULL; //存放音频输入设备的指针。
	IPropertyStore * p_PrptStorePt = NULL; //存放属性存储器的指针。
	PROPVARIANT p_FriendlyNamePrptVariant = { 0 }; //存放友好名称字符串的属性。
	HRESULT p_HRslt;
	
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。
	PropVariantInit( &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。
	
	//判断各个变量是否正确。
	if( AdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out;
	}
	if( AdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备总数的指针不正确。" ) );
		goto Out;
	}
	
	p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&p_AdoInptDvcEnumPt ); //初始化音频输入设备枚举器。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输入设备枚举器失败。原因：" ) );
		goto Out;
	}
	
	p_HRslt = p_AdoInptDvcEnumPt->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &p_AdoInptDvcClctPt ); //获取音频输入设备收集器。只收集激活的音频输入设备。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输入设备收集器失败。原因：" ) );
		goto Out;
	}

	p_AdoInptDvcClctPt->GetCount( &p_AdoInptDvcTotal ); //获取音频输入设备的总数。
	if( p_AdoInptDvcTotal > 0 )
	{
		p_AdoInptDvcTotal++; //递增一个默认音频输入设备。

		//创建并清空音频输入设备名称动态字符串数组的内存块。
		p_AdoInptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoInptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoInptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输入设备名称动态字符串数组的内存块失败。" ) );
			goto Out;
		}
		
		//获取默认音频输入设备的名称。
		{
			p_HRslt = p_AdoInptDvcEnumPt->GetDefaultAudioEndpoint( eCapture, eConsole, &p_AdoInptDvcPt ); //初始化默认的音频输入设备。
			if( p_HRslt == S_OK ) //如果初始化默认的音频输入设备成功。
			{
				p_HRslt = p_AdoInptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。
				
				p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。
				
				if( VstrInit( &p_AdoInptDvcNameArrPtPt[ 0 ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
				{
					VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称的动态字符串失败。" ) );
					goto Out;
				}
				VstrIns( p_AdoInptDvcNameArrPtPt[ 0 ], 0, Cu8vstr( "默认：" ) );
				
				p_PrptStorePt->Release(); //销毁属性存储器。

				p_AdoInptDvcPt->Release(); //销毁音频输入设备。
			}
			else //如果初始化默认的音频输入设备失败。
			{
				GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化默认的音频输入设备失败。原因：" ) );
				goto Out;
			}
		}

		//获取全部音频输入设备的名称。
		for( UINT p_DvcNum = 1; p_DvcNum < p_AdoInptDvcTotal; p_DvcNum++ )
		{
			p_HRslt = p_AdoInptDvcClctPt->Item( p_DvcNum - 1, &p_AdoInptDvcPt ); //初始化音频输入设备。

			p_HRslt = p_AdoInptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。

			p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。

			if( VstrInit( &p_AdoInptDvcNameArrPtPt[ p_DvcNum ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称的动态字符串失败。" ) );
				goto Out;
			}

			p_PrptStorePt->Release(); //销毁属性存储器。

			p_AdoInptDvcPt->Release(); //销毁音频输入设备。
		}
	}

	p_AdoInptDvcClctPt->Release(); //销毁音频输入设备收集器。

	p_AdoInptDvcEnumPt->Release(); //销毁音频输入设备枚举器。

	*AdoInptDvcNameVstrArrPtPtPt = p_AdoInptDvcNameArrPtPt; //设置音频输入设备名称动态字符串数组的指针。
	*AdoInptDvcTotalPt = p_AdoInptDvcTotal; //设置音频输入设备的总数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameArrPtPt, p_AdoInptDvcTotal, NULL );
	}
	PropVariantClear( &p_FriendlyNamePrptVariant ); //销毁友好名称字符串的属性。
	CoUninitialize(); //销毁COM库。
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoOtptDvcName
 * 功能说明：获取音频输出设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AdoOtptDvcNameVstrArrPtPtPt：[输出]，存放音频输出设备名称动态字符串数组指针的指针，不能为NULL。
			 AdoOtptDvcTotalPt：[输出]，存放音频输出设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoOtptDvcName( Vstr * * * AdoOtptDvcNameVstrArrPtPtPt, UINT * AdoOtptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_AdoOtptDvcNameArrPtPt = NULL; //存放音频输出设备名称动态字符串数组的指针。
	UINT p_AdoOtptDvcTotal = 0; //存放音频输出设备的总数。
	IMMDeviceEnumerator * p_AdoOtptDvcEnumPt = NULL; //存放音频输出设备枚举器的指针。
	IMMDeviceCollection * p_AdoOtptDvcClctPt = NULL; //存放音频输出设备收集器的指针。
	IMMDevice * p_AdoOtptDvcPt = NULL; //存放音频输出设备的指针。
	IPropertyStore * p_PrptStorePt = NULL; //存放属性存储器的指针。
	PROPVARIANT p_FriendlyNamePrptVariant = { 0 }; //存放友好名称字符串的属性。
	HRESULT p_HRslt;
	
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。
	PropVariantInit( &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。

	//判断各个变量是否正确。
	if( AdoOtptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out;
	}
	if( AdoOtptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备总数的指针不正确。" ) );
		goto Out;
	}
	
	p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&p_AdoOtptDvcEnumPt ); //初始化音频输出设备枚举器。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输出设备枚举器失败。原因：" ) );
		goto Out;
	}

	p_HRslt = p_AdoOtptDvcEnumPt->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, &p_AdoOtptDvcClctPt ); //获取音频输出设备收集器。只收集激活的音频输出设备。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输出设备收集器失败。原因：" ) );
		goto Out;
	}
	
	p_AdoOtptDvcClctPt->GetCount( &p_AdoOtptDvcTotal ); //获取音频输出设备的总数。
	if( p_AdoOtptDvcTotal > 0 )
	{
		p_AdoOtptDvcTotal++; //递增一个默认音频输出设备。

		//创建并清空音频输出设备名称动态字符串数组的内存块。
		p_AdoOtptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoOtptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoOtptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输出设备名称动态字符串数组的内存块失败。" ) );
			goto Out;
		}
		
		//获取默认音频输出设备的名称。
		{
			p_HRslt = p_AdoOtptDvcEnumPt->GetDefaultAudioEndpoint( eRender, eConsole, &p_AdoOtptDvcPt ); //初始化默认的音频输出设备。
			if( p_HRslt == S_OK ) //如果初始化默认的音频输出设备成功。
			{
				p_HRslt = p_AdoOtptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。
				
				p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。
				
				if( VstrInit( &p_AdoOtptDvcNameArrPtPt[ 0 ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
				{
					VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输出设备名称的动态字符串失败。" ) );
					goto Out;
				}
				VstrIns( p_AdoOtptDvcNameArrPtPt[ 0 ], 0, Cu8vstr( "默认：" ) );
				
				p_PrptStorePt->Release(); //销毁属性存储器。

				p_AdoOtptDvcPt->Release(); //销毁音频输出设备。
			}
			else //如果初始化默认的音频输出设备失败。
			{
				GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化默认的音频输出设备失败。原因：" ) );
				goto Out;
			}
		}

		//获取全部音频输出设备的名称。
		for( UINT p_DvcNum = 1; p_DvcNum < p_AdoOtptDvcTotal; p_DvcNum++ )
		{
			p_HRslt = p_AdoOtptDvcClctPt->Item( p_DvcNum - 1, &p_AdoOtptDvcPt ); //初始化音频输出设备。

			p_HRslt = p_AdoOtptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。

			p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串的属性。

			if( VstrInit( &p_AdoOtptDvcNameArrPtPt[ p_DvcNum ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输出设备名称的动态字符串失败。" ) );
				goto Out;
			}

			p_PrptStorePt->Release(); //销毁属性存储器。

			p_AdoOtptDvcPt->Release(); //销毁音频输出设备。
		}
	}

	p_AdoOtptDvcClctPt->Release(); //销毁音频输出设备收集器。

	p_AdoOtptDvcEnumPt->Release(); //销毁音频输出设备枚举器。

	*AdoOtptDvcNameVstrArrPtPtPt = p_AdoOtptDvcNameArrPtPt; //设置音频输出设备名称动态字符串数组的指针。
	*AdoOtptDvcTotalPt = p_AdoOtptDvcTotal; //设置音频输出设备的总数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameArrPtPt, p_AdoOtptDvcTotal, NULL );
	}
	PropVariantClear( &p_FriendlyNamePrptVariant ); //销毁友好名称字符串的属性。
	CoUninitialize(); //销毁COM库。
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetVdoInptDvcName
 * 功能说明：获取视频输入设备的名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：VdoInptDvcNameVstrArrPtPtPt：[输出]，存放视频输入设备名称动态字符串数组指针的指针，不能为NULL。
			 VdoInptDvcTotalPt：[输出]，存放视频输入设备总数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetVdoInptDvcName( Vstr * * * VdoInptDvcNameVstrArrPtPtPt, UINT * VdoInptDvcTotalPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_VdoInptDvcNameArrPtPt = NULL; //存放视频输入设备名称动态字符串数组的指针。
	Vstr * * p_TmpVstrPtPt = NULL;
	UINT p_VdoInptDvcTotal = 0; //存放视频输入设备的总数。
	ICreateDevEnum * p_CreateDevEnumPt = NULL;
	IEnumMoniker * p_EnumMonikerPt = NULL;
	IMoniker * p_MonikerPt = NULL;
	IPropertyBag * p_PropertyBagPt = NULL;
	ULONG p_Ulong;
	
	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	//判断各个变量是否正确。
	if( VdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备名称动态字符串数组指针的指针不正确。" ) );
		goto Out;
	}
	if( VdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备总数的指针不正确。" ) );
		goto Out;
	}

	//创建系统设备枚举器。
	if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, ( void * * )&p_CreateDevEnumPt ) != S_OK )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建系统设备枚举器失败。" ) );
		goto Out;
	}
	
	if( p_CreateDevEnumPt->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &p_EnumMonikerPt, 0 ) == S_OK )
	{
		while( p_EnumMonikerPt->Next( 1, &p_MonikerPt, NULL ) == S_OK )
		{
			if( SUCCEEDED( p_MonikerPt->BindToStorage( NULL, NULL, IID_IPropertyBag, ( void * * )&p_PropertyBagPt ) ) )
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
						goto Out;
					}
					p_VdoInptDvcNameArrPtPt = p_TmpVstrPtPt;
					p_VdoInptDvcNameArrPtPt[p_VdoInptDvcTotal - 1] = NULL;
					if( VstrInit( &p_VdoInptDvcNameArrPtPt[p_VdoInptDvcTotal - 1], Utf16, , Cu16vstr( varName.bstrVal ) ) != 0 )
					{
						VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称的动态字符串失败。" ) );
						goto Out;
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
	CoUninitialize(); //销毁COM库。
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	UINT p_TmpUint;

	//判断各个变量是否正确。
	if( DvcNameArrPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设备名称动态字符串数组的指针不正确。" ) );
		goto Out;
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

typedef enum
{
	MediaMsgTypSetAdoInpt,
	MediaMsgTypAdoInptSetIsUseSystemAecNsAgc,
	MediaMsgTypAdoInptSetUseNoAec,
	MediaMsgTypAdoInptSetUseSpeexAec,
	MediaMsgTypAdoInptSetUseWebRtcAecm,
	MediaMsgTypAdoInptSetUseWebRtcAec,
	MediaMsgTypAdoInptSetUseSpeexWebRtcAec,
	MediaMsgTypAdoInptSetUseNoNs,
	MediaMsgTypAdoInptSetUseSpeexPrpocsNs,
	MediaMsgTypAdoInptSetUseWebRtcNsx,
	MediaMsgTypAdoInptSetUseWebRtcNs,
	MediaMsgTypAdoInptSetUseRNNoise,
	MediaMsgTypAdoInptSetIsUseSpeexPrpocs,
	MediaMsgTypAdoInptSetUsePcm,
	MediaMsgTypAdoInptSetUseSpeexEncd,
	MediaMsgTypAdoInptSetUseOpusEncd,
	MediaMsgTypAdoInptSetIsSaveAdoToWaveFile,
	MediaMsgTypAdoInptSetIsDrawAdoWavfmToWnd,
	MediaMsgTypAdoInptSetUseDvc,
	MediaMsgTypAdoInptSetIsMute,

	MediaMsgTypSetAdoOtpt,
	MediaMsgTypAdoOtptAddStrm,
	MediaMsgTypAdoOtptDelStrm,
	MediaMsgTypAdoOtptSetStrmUsePcm,
	MediaMsgTypAdoOtptSetStrmUseSpeexDecd,
	MediaMsgTypAdoOtptSetStrmUseOpusDecd,
	MediaMsgTypAdoOtptSetStrmIsUse,
	MediaMsgTypAdoOtptSetIsSaveAdoToWaveFile,
	MediaMsgTypAdoOtptSetIsDrawAdoWavfmToWnd,
	MediaMsgTypAdoOtptSetUseDvc,
	MediaMsgTypAdoOtptSetIsMute,

	MediaMsgTypSetVdoInpt,
	MediaMsgTypVdoInptSetUseYu12,
	MediaMsgTypVdoInptSetUseOpenH264Encd,
	MediaMsgTypVdoInptSetUseDvc,
	MediaMsgTypVdoInptSetIsBlack,

	MediaMsgTypVdoOtptAddStrm,
	MediaMsgTypVdoOtptDelStrm,
	MediaMsgTypVdoOtptSetStrm,
	MediaMsgTypVdoOtptSetStrmUseYu12,
	MediaMsgTypVdoOtptSetStrmUseOpenH264Decd,
	MediaMsgTypVdoOtptSetStrmIsBlack,
	MediaMsgTypVdoOtptSetStrmIsUse,

	MediaMsgTypSetIsUseAdoVdoInptOtpt,

	MediaMsgTypSetIsUsePrvntSysSleep,
	MediaMsgTypSetIsSaveAdoVdoInptOtptToAviFile,
	MediaMsgTypSaveStngToFile,

	MediaMsgTypRqirExit,

	MediaMsgTypUserInit,
	MediaMsgTypUserDstoy,
	MediaMsgTypUserMsg,

	MediaMsgTypAdoVdoInptOtptInit,
	MediaMsgTypAdoVdoInptOtptDstoy,
} MediaMsgTyp;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_SmplRate;
	size_t m_FrmLenMsec;
} MediaMsgSetAdoInpt;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUseSystemAecNsAgc;
} MediaMsgAdoInptSetIsUseSystemAecNsAgc;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoInptSetUseNoAec;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_FilterLenMsec;
	int32_t m_IsUseRec;
	float m_EchoMutp;
	float m_EchoCntu;
	int32_t m_EchoSupes;
	int32_t m_EchoSupesAct;
	int32_t m_IsSaveMemFile;
	Vstr * m_MemFileFullPathVstrPt;
} MediaMsgAdoInptSetUseSpeexAec;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUseCNGMode;
	int32_t m_EchoMode;
	int32_t m_Delay;
} MediaMsgAdoInptSetUseWebRtcAecm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_EchoMode;
	int32_t m_Delay;
	int32_t m_IsUseDelayAgstcMode;
	int32_t m_IsUseExtdFilterMode;
	int32_t m_IsUseRefinedFilterAdaptAecMode;
	int32_t m_IsUseAdaptAdjDelay;
	int32_t m_IsSaveMemFile;
	Vstr * m_MemFileFullPathVstrPt;
} MediaMsgAdoInptSetUseWebRtcAec;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_WorkMode;
	int32_t m_SpeexAecFilterLenMsec;
	int32_t m_SpeexAecIsUseRec;
	float m_SpeexAecEchoMutp;
	float m_SpeexAecEchoCntu;
	int32_t m_SpeexAecEchoSupes;
	int32_t m_SpeexAecEchoSupesAct;
	int32_t m_WebRtcAecmIsUseCNGMode;
	int32_t m_WebRtcAecmEchoMode;
	int32_t m_WebRtcAecmDelay;
	int32_t m_WebRtcAecEchoMode;
	int32_t m_WebRtcAecDelay;
	int32_t m_WebRtcAecIsUseDelayAgstcMode;
	int32_t m_WebRtcAecIsUseExtdFilterMode;
	int32_t m_WebRtcAecIsUseRefinedFilterAdaptAecMode;
	int32_t m_WebRtcAecIsUseAdaptAdjDelay;
	int32_t m_IsUseSameRoomAec;
	int32_t m_SameRoomEchoMinDelay;
} MediaMsgAdoInptSetUseSpeexWebRtcAec;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoInptSetUseNoNs;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUseNs;
	int32_t m_NoiseSupes;
	int32_t m_IsUseDereverb;
} MediaMsgAdoInptSetUseSpeexPrpocsNs;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_PolicyMode;
} MediaMsgAdoInptSetUseWebRtcNsx;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_PolicyMode;
} MediaMsgAdoInptSetUseWebRtcNs;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoInptSetUseRNNoise;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUseSpeexPrpocs;
	int32_t m_IsUseVad;
	int32_t m_VadProbStart;
	int32_t m_VadProbCntu;
	int32_t m_IsUseAgc;
	int32_t m_AgcLevel;
	int32_t m_AgcIncrement;
	int32_t m_AgcDecrement;
	int32_t m_AgcMaxGain;
} MediaMsgAdoInptSetIsUseSpeexPrpocs;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoInptSetUsePcm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_UseCbrOrVbr;
	int32_t m_Qualt;
	int32_t m_Cmplxt;
	int32_t m_PlcExptLossRate;
} MediaMsgAdoInptSetUseSpeexEncd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoInptSetUseOpusEncd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsSaveAdoToWaveFile;
	Vstr * m_SrcWaveFileFullPathVstrPt;
	Vstr * m_RsltWaveFileFullPathVstrPt;
	size_t m_WaveFileWrBufSzByt;
} MediaMsgAdoInptSetIsSaveAdoToWaveFile;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsDrawAdoWavfmToWnd;
	HWND m_SrcWavfmWndHdl;
	HWND m_RsltWavfmWndHdl;
} MediaMsgAdoInptSetIsDrawAdoWavfmToWnd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	UINT m_ID;
} MediaMsgAdoInptSetUseDvc;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsMute;
} MediaMsgAdoInptSetIsMute;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_SmplRate;
	size_t m_FrmLenMsec;
} MediaMsgSetAdoOtpt;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgAdoOtptAddStrm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgAdoOtptDelStrm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgAdoOtptSetStrmUsePcm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	int32_t m_IsUsePrcplEnhsmt;
} MediaMsgAdoOtptSetStrmUseSpeexDecd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgAdoOtptSetStrmUseOpusDecd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	int32_t m_IsUse;
} MediaMsgAdoOtptSetStrmIsUse;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsSaveAdoToWaveFile;
	Vstr * m_SrcWaveFileFullPathVstrPt;
	size_t m_WaveFileWrBufSzByt;
} MediaMsgAdoOtptSetIsSaveAdoToWaveFile;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsDrawAdoWavfmToWnd;
	HWND m_SrcWavfmWndHdl;
} MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	UINT m_ID;
} MediaMsgAdoOtptSetUseDvc;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsMute;
} MediaMsgAdoOtptSetIsMute;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_MaxSmplRate;
	int32_t m_FrmWidth;
	int32_t m_FrmHeight;
	HWND m_PrvwWndHdl;
} MediaMsgSetVdoInpt;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgSetVdoInptUseYu12;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_VdoType;
	int32_t m_EncdBitrate;
	int32_t m_BitrateCtrlMode;
	int32_t m_IDRFrmIntvl;
	int32_t m_Cmplxt;
} MediaMsgSetVdoInptUseOpenH264Encd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	UINT m_ID;
} MediaMsgSetVdoInptUseDvc;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsBlack;
} MediaMsgSetVdoInptIsBlack;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgAddVdoOtptStrm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgDelVdoOtptStrm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	HWND m_DspyWndHdl;
} MediaMsgSetVdoOtptStrm;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
} MediaMsgSetVdoOtptStrmUseYu12;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	int32_t m_DecdThrdNum;
} MediaMsgSetVdoOtptStrmUseOpenH264Decd;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	int32_t m_IsBlack;
} MediaMsgSetVdoOtptStrmIsBlack;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_StrmIdx;
	int32_t m_IsUse;
} MediaMsgSetVdoOtptStrmIsUse;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUsePrvntSysSleep;
} MediaMsgSetIsUsePrvntSysSleep;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	Vstr * m_FullPathVstrPt;
	size_t m_WrBufSzByt;
	int32_t m_IsSaveAdoInpt;
	int32_t m_IsSaveAdoOtpt;
	int32_t m_IsSaveVdoInpt;
	int32_t m_IsSaveVdoOtpt;
} MediaMsgSetIsSaveAdoVdoInptOtptToAviFile;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	Vstr * m_FullPathVstrPt;
} MediaMsgSaveStngToFile;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_IsUseAdoInpt;
	int32_t m_IsUseAdoOtpt;
	int32_t m_IsUseVdoInpt;
	int32_t m_IsUseVdoOtpt;
} MediaMsgSetIsUseAdoVdoInptOtpt;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	int32_t m_ExitFlag;
	int32_t m_IsBlockWait;
} MediaMsgRqirExit;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgUserInit;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgUserDstoy;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	void * m_MsgArgPt;
} MediaMsgUserMsg;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoVdoInptOtptInit;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
} MediaMsgAdoVdoInptOtptDstoy;
typedef struct
{
	MediaMsgTyp m_MediaMsgTyp;
	uint8_t m_MsgArgArr[ sizeof( MediaMsgAdoInptSetUseSpeexWebRtcAec ) ];
} MediaMsg;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInpt
 * 功能说明：媒体处理线程的设置音频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInpt p_MediaMsgSetAdoInpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ) );
		goto Out;
	}
	if( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInpt.m_MediaMsgTyp = MediaMsgTypSetAdoInpt;
	p_MediaMsgSetAdoInpt.m_SmplRate = SmplRate;
	p_MediaMsgSetAdoInpt.m_FrmLenMsec = FrmLenMsec;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetAdoInpt, sizeof( p_MediaMsgSetAdoInpt ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc
 * 功能说明：媒体处理线程的音频输入设置是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseSystemAecNsAgc：[输入]，存放是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseSystemAecNsAgc, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetIsUseSystemAecNsAgc p_MediaMsgAdoInptSetIsUseSystemAecNsAgc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetIsUseSystemAecNsAgc.m_MediaMsgTyp = MediaMsgTypAdoInptSetIsUseSystemAecNsAgc;
	p_MediaMsgAdoInptSetIsUseSystemAecNsAgc.m_IsUseSystemAecNsAgc = IsUseSystemAecNsAgc;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetIsUseSystemAecNsAgc, sizeof( p_MediaMsgAdoInptSetIsUseSystemAecNsAgc ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseNoAec
 * 功能说明：媒体处理线程的音频输入设置不使用声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseNoAec( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseNoAec p_MediaMsgAdoInptSetUseNoAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetUseNoAec.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseNoAec;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseNoAec, sizeof( p_MediaMsgAdoInptSetUseNoAec ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseSpeexAec
 * 功能说明：媒体处理线程的音频输入设置要使用Speex声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FilterLenMsec：[输入]，存放滤波器的长度，单位为毫秒。
			 IsUseRec：[输入]，存放是否使用残余回音消除，为非0表示要使用，为0表示不使用。
			 EchoMutp：[输入]，存放在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
			 EchoCntu：[输入]，存放在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
			 EchoSupes：[输入]，存放在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 EchoSupesAct：[输入]，存放在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 IsSaveMemFile：[输入]，存放是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathVstrPt：[输入]，存放内存块文件完整路径动态字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseSpeexAec p_MediaMsgAdoInptSetUseSpeexAec;
	p_MediaMsgAdoInptSetUseSpeexAec.m_MemFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseSpeexAec.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseSpeexAec;
	p_MediaMsgAdoInptSetUseSpeexAec.m_FilterLenMsec = FilterLenMsec;
	p_MediaMsgAdoInptSetUseSpeexAec.m_IsUseRec = IsUseRec;
    p_MediaMsgAdoInptSetUseSpeexAec.m_EchoMutp = EchoMutp;
    p_MediaMsgAdoInptSetUseSpeexAec.m_EchoCntu = EchoCntu;
    p_MediaMsgAdoInptSetUseSpeexAec.m_EchoSupes = EchoSupes;
    p_MediaMsgAdoInptSetUseSpeexAec.m_EchoSupesAct = EchoSupesAct;
	p_MediaMsgAdoInptSetUseSpeexAec.m_IsSaveMemFile = IsSaveMemFile;
	p_MediaMsgAdoInptSetUseSpeexAec.m_MemFileFullPathVstrPt = NULL;
	if( IsSaveMemFile != 0 )
	{
		if( VstrInit( &p_MediaMsgAdoInptSetUseSpeexAec.m_MemFileFullPathVstrPt, Utf8, , MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseSpeexAec, sizeof( p_MediaMsgAdoInptSetUseSpeexAec ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgAdoInptSetUseSpeexAec.m_MemFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgAdoInptSetUseSpeexAec.m_MemFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcAecm
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseWebRtcAecm p_MediaMsgAdoInptSetUseWebRtcAecm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseWebRtcAecm.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseWebRtcAecm;
	p_MediaMsgAdoInptSetUseWebRtcAecm.m_IsUseCNGMode = IsUseCNGMode;
	p_MediaMsgAdoInptSetUseWebRtcAecm.m_EchoMode = EchoMode;
	p_MediaMsgAdoInptSetUseWebRtcAecm.m_Delay = Delay;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseWebRtcAecm, sizeof( p_MediaMsgAdoInptSetUseWebRtcAecm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcAec
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc浮点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 EchoMode：[输入]，存放消除模式，消除模式越高消除越强，取值区间为[0,2]。
			 Delay：[输入]，存放回音的延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 IsUseDelayAgstcMode：[输入]，存放是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
			 IsUseExtdFilterMode：[输入]，存放是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
			 IsUseRefinedFilterAdaptAecMode：[输入]，存放是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
			 IsUseAdaptAdjDelay：[输入]，存放是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
			 IsSaveMemFile：[输入]，存放是否保存内存块到文件，为非0表示要保存，为0表示不保存。
			 MemFileFullPathVstrPt：[输入]，存放内存块文件完整路径动态字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, int32_t IsSaveMemFile, const Vstr * MemFileFullPathStrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseWebRtcAec p_MediaMsgAdoInptSetUseWebRtcAec;
	p_MediaMsgAdoInptSetUseWebRtcAec.m_MemFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseWebRtcAec.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseWebRtcAec;
	p_MediaMsgAdoInptSetUseWebRtcAec.m_EchoMode = EchoMode;
    p_MediaMsgAdoInptSetUseWebRtcAec.m_Delay = Delay;
    p_MediaMsgAdoInptSetUseWebRtcAec.m_IsUseDelayAgstcMode = IsUseDelayAgstcMode;
    p_MediaMsgAdoInptSetUseWebRtcAec.m_IsUseExtdFilterMode = IsUseExtdFilterMode;
    p_MediaMsgAdoInptSetUseWebRtcAec.m_IsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    p_MediaMsgAdoInptSetUseWebRtcAec.m_IsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	p_MediaMsgAdoInptSetUseWebRtcAec.m_IsSaveMemFile = IsSaveMemFile;
	p_MediaMsgAdoInptSetUseWebRtcAec.m_MemFileFullPathVstrPt = NULL;
	if( IsSaveMemFile != 0 )
	{
		if( VstrInit( &p_MediaMsgAdoInptSetUseWebRtcAec.m_MemFileFullPathVstrPt, Utf8, , MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseWebRtcAec, sizeof( p_MediaMsgAdoInptSetUseWebRtcAec ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgAdoInptSetUseWebRtcAec.m_MemFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgAdoInptSetUseWebRtcAec.m_MemFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseSpeexWebRtcAec
 * 功能说明：媒体处理线程的音频输入设置要使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 WorkMode：[输入]，存放工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
			 SpeexAecFilterLenMsec：[输入]，存放Speex声学回音消除器滤波器的长度，单位为毫秒。
			 SpeexAecIsUseRec：[输入]，存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
			 SpeexAecEchoMutp：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
			 SpeexAecEchoCntu：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
			 SpeexAecEchoSupes：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 SpeexAecEchoSupesAct：[输入]，存放Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 WebRtcAecmIsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 WebRtcAecmEchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 WebRtcAecmDelay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 WebRtcAecEchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
			 WebRtcAecDelay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 WebRtcAecIsUseDelayAgstcMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
			 WebRtcAecIsUseExtdFilterMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
			 WebRtcAecIsUseRefinedFilterAdaptAecMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
			 WebRtcAecIsUseAdaptAdjDelay：[输入]，存放WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
			 IsUseSameRoomAec：[输入]，存放是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
			 SameRoomEchoMinDelay：[输入]，存放同一房间回音最小延迟，单位为毫秒，取值区间为[1,2147483647]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseSpeexWebRtcAec p_MediaMsgAdoInptSetUseSpeexWebRtcAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseSpeexWebRtcAec;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WorkMode = WorkMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecFilterLenMsec = SpeexAecFilterLenMsec;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecIsUseRec = SpeexAecIsUseRec;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoMutp = SpeexAecEchoMutp;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoCntu = SpeexAecEchoCntu;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoSupes = SpeexAecEchoSupes;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmEchoMode = WebRtcAecmEchoMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmDelay = WebRtcAecmDelay;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecEchoMode = WebRtcAecEchoMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecDelay = WebRtcAecDelay;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode = WebRtcAecIsUseDelayAgstcMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_IsUseSameRoomAec = IsUseSameRoomAec;
	p_MediaMsgAdoInptSetUseSpeexWebRtcAec.m_SameRoomEchoMinDelay = SameRoomEchoMinDelay;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseSpeexWebRtcAec, sizeof( p_MediaMsgAdoInptSetUseSpeexWebRtcAec ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseNoNs
 * 功能说明：媒体处理线程的音频输入设置不使用噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseNoNs( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseNoNs p_MediaMsgAdoInptSetUseNoNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetUseNoNs.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseNoNs;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseNoNs, sizeof( p_MediaMsgAdoInptSetUseNoNs ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseSpeexPrpocsNs
 * 功能说明：媒体处理线程的音频输入设置要使用Speex预处理器的噪音抑制。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseNs：[输入]，存放是否使用噪音抑制，为非0表示要使用，为0表示不使用。
			 NoiseSupes：[输入]，存放在噪音抑制时，噪音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
			 IsUseDereverb：[输入]，存放是否使用混响音消除，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseSpeexPrpocsNs p_MediaMsgAdoInptSetUseSpeexPrpocsNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseSpeexPrpocsNs.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseSpeexPrpocsNs;
	p_MediaMsgAdoInptSetUseSpeexPrpocsNs.m_IsUseNs = IsUseNs;
	p_MediaMsgAdoInptSetUseSpeexPrpocsNs.m_NoiseSupes = NoiseSupes;
    p_MediaMsgAdoInptSetUseSpeexPrpocsNs.m_IsUseDereverb = IsUseDereverb;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseSpeexPrpocsNs, sizeof( p_MediaMsgAdoInptSetUseSpeexPrpocsNs ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcNsx
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc定点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseWebRtcNsx p_MediaMsgAdoInptSetUseWebRtcNsx;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetUseWebRtcNsx.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseWebRtcNsx;
	p_MediaMsgAdoInptSetUseWebRtcNsx.m_PolicyMode = PolicyMode;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseWebRtcNsx, sizeof( p_MediaMsgAdoInptSetUseWebRtcNsx ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcNs
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc浮点版噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             PolicyMode：[输入]，存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseWebRtcNs p_MediaMsgAdoInptSetUseWebRtcNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseWebRtcNs.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseWebRtcNs;
	p_MediaMsgAdoInptSetUseWebRtcNs.m_PolicyMode = PolicyMode;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseWebRtcNs, sizeof( p_MediaMsgAdoInptSetUseWebRtcNs ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseRNNoise
 * 功能说明：媒体处理线程的音频输入设置要使用RNNoise噪音抑制器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseRNNoise p_MediaMsgAdoInptSetUseRNNoise;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseRNNoise.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseRNNoise;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseRNNoise, sizeof( p_MediaMsgAdoInptSetUseRNNoise ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetIsUseSpeexPrpocs
 * 功能说明：媒体处理线程的音频输入设置是否使用Speex预处理器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseSpeexPrpocs：[输入]，存放是否使用Speex预处理器，为非0表示要使用，为0表示不使用。
             IsUseVad：[输入]，存放是否使用语音活动检测，为非0表示要使用，为0表示不使用。
			 VadProbStart：[输入]，存放在语音活动检测时，从无语音活动到有语音活动的判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
			 VadProbCntu：[输入]，存放在语音活动检测时，从有语音活动到无语音活动的判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
			 IsUseAgc：[输入]，存放是否使用自动增益控制，为非0表示要使用，为0表示不使用。
			 AgcLevel：[输入]，存放在自动增益控制时，增益的目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
			 AgcIncrement：[输入]，存放在自动增益控制时，每秒最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
			 AgcDecrement：[输入]，存放在自动增益控制时，每秒最大减益的分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
			 AgcMaxGain：[输入]，存放在自动增益控制时，最大增益的分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetIsUseSpeexPrpocs p_MediaMsgAdoInptSetIsUseSpeexPrpocs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_MediaMsgTyp = MediaMsgTypAdoInptSetIsUseSpeexPrpocs;
	p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseSpeexPrpocs = IsUseSpeexPrpocs;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseVad = IsUseVad;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_VadProbStart = VadProbStart;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_VadProbCntu = VadProbCntu;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseAgc = IsUseAgc;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_AgcLevel = AgcLevel;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_AgcIncrement = AgcIncrement;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_AgcDecrement = AgcDecrement;
    p_MediaMsgAdoInptSetIsUseSpeexPrpocs.m_AgcMaxGain = AgcMaxGain;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetIsUseSpeexPrpocs, sizeof( p_MediaMsgAdoInptSetIsUseSpeexPrpocs ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUsePcm
 * 功能说明：媒体处理线程的音频输入设置要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUsePcm( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUsePcm p_MediaMsgAdoInptSetUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUsePcm.m_MediaMsgTyp = MediaMsgTypAdoInptSetUsePcm;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUsePcm, sizeof( p_MediaMsgAdoInptSetUsePcm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseSpeexEncd
 * 功能说明：媒体处理线程的音频输入设置要使用Speex编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             UseCbrOrVbr：[输入]，存放使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             Qualt：[输入]，存放编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             Cmplxt：[输入]，存放编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             PlcExptLossRate：[输入]，存放在数据包丢失隐藏时，数据包的预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseSpeexEncd p_MediaMsgAdoInptSetUseSpeexEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetUseSpeexEncd.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseSpeexEncd;
	p_MediaMsgAdoInptSetUseSpeexEncd.m_UseCbrOrVbr = UseCbrOrVbr;
    p_MediaMsgAdoInptSetUseSpeexEncd.m_Qualt = Qualt;
    p_MediaMsgAdoInptSetUseSpeexEncd.m_Cmplxt = Cmplxt;
    p_MediaMsgAdoInptSetUseSpeexEncd.m_PlcExptLossRate = PlcExptLossRate;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseSpeexEncd, sizeof( p_MediaMsgAdoInptSetUseSpeexEncd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseOpusEncd
 * 功能说明：媒体处理线程的音频输入设置要使用Opus编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseOpusEncd p_MediaMsgAdoInptSetUseOpusEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetUseOpusEncd.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseOpusEncd;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseOpusEncd, sizeof( p_MediaMsgAdoInptSetUseOpusEncd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd
 * 功能说明：媒体处理线程的音频输入设置是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToWaveFile：[输入]，存放是否保存音频到Wave文件，非0表示要使用，0表示不使用。
			 RsltWavfmWndHdl：[输入]，存放结果波形窗口的句柄。
			 SrcWavfmWndHdl：[输入]，存放原始波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND SrcWavfmWndHdl, HWND RsltWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetIsDrawAdoWavfmToWnd p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( SrcWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始波形窗口的句柄不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( RsltWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "结果波形窗口的句柄不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_MediaMsgTyp = MediaMsgTypAdoInptSetIsDrawAdoWavfmToWnd;
	p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	if( IsDrawAdoWavfmToWnd != 0 )
	{
		p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_SrcWavfmWndHdl = SrcWavfmWndHdl;
		p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_RsltWavfmWndHdl = RsltWavfmWndHdl;
	}
	else
	{
		p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_SrcWavfmWndHdl = NULL;
		p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd.m_RsltWavfmWndHdl = NULL;
	}
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd, sizeof( p_MediaMsgAdoInptSetIsDrawAdoWavfmToWnd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDrawAdoWavfmToWnd = 0;
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile
 * 功能说明：媒体处理线程的音频输入设置是否保存音频到Wave文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToWaveFile：[输入]，存放是否保存音频到Wave文件，非0表示要使用，0表示不使用。
			 SrcWaveFileFullPathVstrPt：[输入]，存放原始Wave文件完整路径动态字符串的指针。
			 RsltWaveFileFullPathVstrPt：[输入]，存放结果Wave文件完整路径动态字符串的指针。
			 WaveFileWrBufSzByt：[输入]，存放Wave文件写入缓冲区的大小，单位为字节。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToWaveFile, const Vstr * SrcWaveFileFullPathVstrPt, const Vstr * RsltWaveFileFullPathVstrPt, size_t WaveFileWrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetIsSaveAdoToWaveFile p_MediaMsgAdoInptSetIsSaveAdoToWaveFile;
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt = NULL;
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltWaveFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToWaveFile != 0 ) && ( SrcWaveFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始Wave文件的完整路径动态字符串不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToWaveFile != 0 ) && ( RsltWaveFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "结果Wave文件的完整路径动态字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_MediaMsgTyp = MediaMsgTypAdoInptSetIsSaveAdoToWaveFile;
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_IsSaveAdoToWaveFile = IsSaveAdoToWaveFile;
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt = NULL;
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltWaveFileFullPathVstrPt = NULL;
	if( IsSaveAdoToWaveFile != 0 )
	{
		if( VstrInit( &p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt, Utf8, , SrcWaveFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置原始Wave文件完整路径动态字符串失败。" ) );
			goto Out;
		}
		if( VstrInit( &p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltWaveFileFullPathVstrPt, Utf8, , RsltWaveFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置结果Wave文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_WaveFileWrBufSzByt = WaveFileWrBufSzByt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetIsSaveAdoToWaveFile, sizeof( p_MediaMsgAdoInptSetIsSaveAdoToWaveFile ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt );
		if( p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltWaveFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltWaveFileFullPathVstrPt );
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseDvc
 * 功能说明：媒体处理线程的音频输入设置使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ID：[输入]，存放标识符，取值范围为从1到音频输入设备的总数，为0表示使用默认设备。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT ID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetUseDvc p_MediaMsgAdoInptSetUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgAdoInptSetUseDvc.m_MediaMsgTyp = MediaMsgTypAdoInptSetUseDvc;
	p_MediaMsgAdoInptSetUseDvc.m_ID = ID;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetUseDvc, sizeof( p_MediaMsgAdoInptSetUseDvc ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetIsMute
 * 功能说明：媒体处理线程的音频输入设置是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoInptSetIsMute p_MediaMsgAdoInptSetIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoInptSetIsMute.m_MediaMsgTyp = MediaMsgTypAdoInptSetIsMute;
	p_MediaMsgAdoInptSetIsMute.m_IsMute = IsMute;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoInptSetIsMute, sizeof( p_MediaMsgAdoInptSetIsMute ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtpt
 * 功能说明：媒体处理线程的设置音频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtpt p_MediaMsgSetAdoOtpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ) );
		goto Out;
	}
	if( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtpt.m_MediaMsgTyp = MediaMsgTypSetAdoOtpt;
	p_MediaMsgSetAdoOtpt.m_SmplRate = SmplRate;
	p_MediaMsgSetAdoOtpt.m_FrmLenMsec = FrmLenMsec;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetAdoOtpt, sizeof( p_MediaMsgSetAdoOtpt ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptAddStrm
 * 功能说明：媒体处理线程的音频输出添加流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptAddStrm p_MediaMsgAdoOtptAddStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptAddStrm.m_MediaMsgTyp = MediaMsgTypAdoOtptAddStrm;
	p_MediaMsgAdoOtptAddStrm.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptAddStrm, sizeof( p_MediaMsgAdoOtptAddStrm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptDelStrm
 * 功能说明：媒体处理线程的音频输出删除流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptDelStrm p_MediaMsgAdoOtptDelStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptDelStrm.m_MediaMsgTyp = MediaMsgTypAdoOtptDelStrm;
	p_MediaMsgAdoOtptDelStrm.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptDelStrm, sizeof( p_MediaMsgAdoOtptDelStrm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetStrmUsePcm
 * 功能说明：媒体处理线程的音频输出设置流要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetStrmUsePcm p_MediaMsgAdoOtptSetStrmUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetStrmUsePcm.m_MediaMsgTyp = MediaMsgTypAdoOtptSetStrmUsePcm;
	p_MediaMsgAdoOtptSetStrmUsePcm.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetStrmUsePcm, sizeof( p_MediaMsgAdoOtptSetStrmUsePcm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetStrmUseSpeexDecd
 * 功能说明：媒体处理线程的音频输出设置流要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
             IsUsePrcplEnhsmt：[输入]，存放是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetStrmUseSpeexDecd p_MediaMsgAdoOtptSetStrmUseSpeexDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetStrmUseSpeexDecd.m_MediaMsgTyp = MediaMsgTypAdoOtptSetStrmUseSpeexDecd;
	p_MediaMsgAdoOtptSetStrmUseSpeexDecd.m_StrmIdx = StrmIdx;
	p_MediaMsgAdoOtptSetStrmUseSpeexDecd.m_IsUsePrcplEnhsmt = IsUsePrcplEnhsmt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetStrmUseSpeexDecd, sizeof( p_MediaMsgAdoOtptSetStrmUseSpeexDecd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetStrmUseOpusDecd
 * 功能说明：媒体处理线程的音频输出设置流要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetStrmUseOpusDecd p_MediaMsgAdoOtptSetStrmUseOpusDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetStrmUseOpusDecd.m_MediaMsgTyp = MediaMsgTypAdoOtptSetStrmUseOpusDecd;
	p_MediaMsgAdoOtptSetStrmUseOpusDecd.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetStrmUseOpusDecd, sizeof( p_MediaMsgAdoOtptSetStrmUseOpusDecd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetStrmIsUse
 * 功能说明：媒体处理线程的音频输出设置流是否要使用。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 IsUse：[输入]，存放是否使用流，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int32_t StrmIdx, int32_t IsUse, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetStrmIsUse p_MediaMsgAdoOtptSetStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetStrmIsUse.m_MediaMsgTyp = MediaMsgTypAdoOtptSetStrmIsUse;
	p_MediaMsgAdoOtptSetStrmIsUse.m_StrmIdx = StrmIdx;
	p_MediaMsgAdoOtptSetStrmIsUse.m_IsUse = IsUse;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetStrmIsUse, sizeof( p_MediaMsgAdoOtptSetStrmIsUse ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd
 * 功能说明：媒体处理线程的音频输出设置是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsDrawAdoWavfmToWnd：[输入]，存放是否绘制音频波形到窗口，非0表示要绘制，0表示不绘制。
			 SrcWavfmWndHdl：[输入]，存放原始波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND SrcWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( SrcWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始波形窗口的句柄不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd.m_MediaMsgTyp = MediaMsgTypAdoOtptSetIsDrawAdoWavfmToWnd;
	p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd.m_SrcWavfmWndHdl = SrcWavfmWndHdl;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd, sizeof( p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile
 * 功能说明：媒体处理线程的音频输出设置是否保存音频到Wave文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToWaveFile：[输入]，存放是否保存音频到Wave文件，非0表示要使用，0表示不使用。
			 SrcWaveFileFullPathVstrPt：[输入]，存放原始Wave文件完整路径动态字符串的指针。
			 WaveFileWrBufSzByt：[输入]，存放Wave文件写入缓冲区的大小，单位为字节。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToWaveFile, const Vstr * SrcWaveFileFullPathVstrPt, size_t WaveFileWrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetIsSaveAdoToWaveFile p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile;
	p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToWaveFile != 0 ) && ( SrcWaveFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始Wave文件的完整路径动态字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_MediaMsgTyp = MediaMsgTypAdoOtptSetIsSaveAdoToWaveFile;
	p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_IsSaveAdoToWaveFile = IsSaveAdoToWaveFile;
	p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt = NULL;
	if( IsSaveAdoToWaveFile != 0 )
	{
		if( VstrInit( &p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt, Utf8, , SrcWaveFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置原始Wave文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_WaveFileWrBufSzByt = WaveFileWrBufSzByt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile, sizeof( p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcWaveFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetUseDvc
 * 功能说明：媒体处理线程的音频输出设置使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ID：[输入]，存放标识符，取值范围为从1到音频输出设备的总数，为0表示使用默认设备。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT ID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetUseDvc p_MediaMsgAdoOtptSetUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetUseDvc.m_MediaMsgTyp = MediaMsgTypAdoOtptSetUseDvc;
	p_MediaMsgAdoOtptSetUseDvc.m_ID = ID;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetUseDvc, sizeof( p_MediaMsgAdoOtptSetUseDvc ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetIsMute
 * 功能说明：媒体处理线程的音频输出设置是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAdoOtptSetIsMute p_MediaMsgAdoOtptSetIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAdoOtptSetIsMute.m_MediaMsgTyp = MediaMsgTypAdoOtptSetIsMute;
	p_MediaMsgAdoOtptSetIsMute.m_IsMute = IsMute;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAdoOtptSetIsMute, sizeof( p_MediaMsgAdoOtptSetIsMute ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetVdoInpt
 * 功能说明：媒体处理线程的设置视频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 MaxSmplRate：[输入]，存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
			 FrmWidth：[输入]，存放帧的宽度，单位为像素。
			 FrmHeight：[输入]，存放帧的高度，单位为像素。
			 PrvwWndHdl：[输入]，存放预览窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND PrvwWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInpt p_MediaMsgSetVdoInpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( MaxSmplRate < 1 ) || ( MaxSmplRate > 60 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "最大采样频率不正确。" ) );
		goto Out;
	}
	if( ( FrmWidth <= 0 ) || ( ( FrmWidth & 1 ) != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的宽度不正确。" ) );
		goto Out;
	}
	if( ( FrmHeight <= 0 ) || ( ( FrmHeight & 1 ) != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的高度不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInpt.m_MediaMsgTyp = MediaMsgTypSetVdoInpt;
	p_MediaMsgSetVdoInpt.m_MaxSmplRate = MaxSmplRate;
    p_MediaMsgSetVdoInpt.m_FrmWidth = FrmWidth;
	p_MediaMsgSetVdoInpt.m_FrmHeight = FrmHeight;
	p_MediaMsgSetVdoInpt.m_PrvwWndHdl = PrvwWndHdl;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoInpt, sizeof( p_MediaMsgSetVdoInpt ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoInptSetUseYu12
 * 功能说明：媒体处理线程的视频输入设置要使用Yu12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseYu12( MediaPocsThrd * MediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseYu12 p_MediaMsgSetVdoInptUseYu12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseYu12.m_MediaMsgTyp = MediaMsgTypVdoInptSetUseYu12;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoInptUseYu12, sizeof( p_MediaMsgSetVdoInptUseYu12 ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoInptSetUseOpenH264Encd
 * 功能说明：媒体处理线程的视频输入设置要使用OpenH264编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoType：[输入]，存放视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
             EncdBitrate：[输入]，存放编码后比特率，单位为bps。
             BitrateCtrlMode：[输入]，存放比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
             IDRFrmIntvl：[输入]，存放IDR帧间隔帧数，单位为个，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
             Cmplxt：[输入]，存放复杂度，复杂度越高压缩率不变、CPU使用率越高、画质越好，取值区间为[0,2]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseOpenH264Encd p_MediaMsgSetVdoInptUseOpenH264Encd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_MediaMsgTyp = MediaMsgTypVdoInptSetUseOpenH264Encd;
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_VdoType = VdoType;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_EncdBitrate = EncdBitrate;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_BitrateCtrlMode = BitrateCtrlMode;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_IDRFrmIntvl = IDRFrmIntvl;
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_Cmplxt = Cmplxt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoInptUseOpenH264Encd, sizeof( p_MediaMsgSetVdoInptUseOpenH264Encd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoInptSetUseDvc
 * 功能说明：媒体处理线程的视频输入设置使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 ID：[输入]，存放标识符，取值范围为从0到视频输入设备的总数减一。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, UINT ID, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseDvc p_MediaMsgSetVdoInptUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseDvc.m_MediaMsgTyp = MediaMsgTypVdoInptSetUseDvc;
	p_MediaMsgSetVdoInptUseDvc.m_ID = ID;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoInptUseDvc, sizeof( p_MediaMsgSetVdoInptUseDvc ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoInptSetIsBlack
 * 功能说明：媒体处理线程的视频输入设置是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输入]，存放是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptIsBlack p_MediaMsgSetVdoInptIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptIsBlack.m_MediaMsgTyp = MediaMsgTypVdoInptSetIsBlack;
	p_MediaMsgSetVdoInptIsBlack.m_IsBlack = IsBlack;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoInptIsBlack, sizeof( p_MediaMsgSetVdoInptIsBlack ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptAddStrm
 * 功能说明：媒体处理线程的视频输出添加流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAddVdoOtptStrm p_MediaMsgAddVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAddVdoOtptStrm.m_MediaMsgTyp = MediaMsgTypVdoOtptAddStrm;
	p_MediaMsgAddVdoOtptStrm.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgAddVdoOtptStrm, sizeof( p_MediaMsgAddVdoOtptStrm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptDelStrm
 * 功能说明：媒体处理线程的视频输出删除流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgDelVdoOtptStrm p_MediaMsgDelVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgDelVdoOtptStrm.m_MediaMsgTyp = MediaMsgTypVdoOtptDelStrm;
	p_MediaMsgDelVdoOtptStrm.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgDelVdoOtptStrm, sizeof( p_MediaMsgDelVdoOtptStrm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptSetStrm
 * 功能说明：媒体处理线程的视频输出设置流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 DspyWndHdl：[输入]，存放显示窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, HWND DspyWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrm p_MediaMsgSetVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrm.m_MediaMsgTyp = MediaMsgTypVdoOtptSetStrm;
	p_MediaMsgSetVdoOtptStrm.m_StrmIdx = StrmIdx;
	p_MediaMsgSetVdoOtptStrm.m_DspyWndHdl = DspyWndHdl;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoOtptStrm, sizeof( p_MediaMsgSetVdoOtptStrm ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptSetStrmUseYu12
 * 功能说明：媒体处理线程的视频输出设置流要使用Yu12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmUseYu12( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmUseYu12 p_MediaMsgSetVdoOtptStrmUseYu12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmUseYu12.m_MediaMsgTyp = MediaMsgTypVdoOtptSetStrmUseYu12;
	p_MediaMsgSetVdoOtptStrmUseYu12.m_StrmIdx = StrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoOtptStrmUseYu12, sizeof( p_MediaMsgSetVdoOtptStrmUseYu12 ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd
 * 功能说明：媒体处理线程的视频输出设置流要使用OpenH264解码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
             DecdThrdNum：[输入]，存放解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmUseOpenH264Decd p_MediaMsgSetVdoOtptStrmUseOpenH264Decd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_MediaMsgTyp = MediaMsgTypVdoOtptSetStrmUseOpenH264Decd;
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_StrmIdx = StrmIdx;
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_DecdThrdNum = DecdThrdNum;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoOtptStrmUseOpenH264Decd, sizeof( p_MediaMsgSetVdoOtptStrmUseOpenH264Decd ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptSetStrmIsBlack
 * 功能说明：媒体处理线程的视频输出设置流是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 IsBlack：[输入]，存放是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmIsBlack p_MediaMsgSetVdoOtptStrmIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmIsBlack.m_MediaMsgTyp = MediaMsgTypVdoOtptSetStrmIsBlack;
	p_MediaMsgSetVdoOtptStrmIsBlack.m_StrmIdx = StrmIdx;
	p_MediaMsgSetVdoOtptStrmIsBlack.m_IsBlack = IsBlack;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoOtptStrmIsBlack, sizeof( p_MediaMsgSetVdoOtptStrmIsBlack ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdVdoOtptSetStrmIsUse
 * 功能说明：媒体处理线程的视频输出设置流是否使用。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流的索引。
			 IsUse：[输入]，存放是否使用，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, uint32_t StrmIdx, int32_t IsUse, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmIsUse p_MediaMsgSetVdoOtptStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmIsUse.m_MediaMsgTyp = MediaMsgTypVdoOtptSetStrmIsUse;
	p_MediaMsgSetVdoOtptStrmIsUse.m_StrmIdx = StrmIdx;
	p_MediaMsgSetVdoOtptStrmIsUse.m_IsUse = IsUse;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetVdoOtptStrmIsUse, sizeof( p_MediaMsgSetVdoOtptStrmIsUse ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUseAdoVdoInptOtpt
 * 功能说明：媒体处理线程的设置音视频输入输出是否使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsUseAdoInpt：[输入]，存放音频输入是否使用，为0表示不使用，为非0表示要使用。
			 IsUseAdoOtpt：[输入]，存放音频输出是否使用，为0表示不使用，为非0表示要使用。
			 IsUseVdoInpt：[输入]，存放视频输入是否使用，为0表示不使用，为非0表示要使用。
			 IsUseVdoOtpt：[输入]，存放视频输出是否使用，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUseAdoVdoInptOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetIsUseAdoVdoInptOtpt p_MediaMsgSetIsUseAdoVdoInptOtpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_MediaMsgTyp = MediaMsgTypSetIsUseAdoVdoInptOtpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoInpt = IsUseAdoInpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoOtpt = IsUseAdoOtpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoInpt = IsUseVdoInpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoOtpt = IsUseVdoOtpt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetIsUseAdoVdoInptOtpt, sizeof( p_MediaMsgSetIsUseAdoVdoInptOtpt ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsPrintLogShowToast
 * 功能说明：媒体处理线程的设置是否打印Log日志、显示Toast。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
			 ShowToastWndHdl：[输入]，存放显示Toast窗口的句柄，为NULL表示桌面窗口。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsPrintLogShowToast( MediaPocsThrd * MediaPocsThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	MediaPocsThrdPt->m_IsPrintLog = IsPrintLog;
	MediaPocsThrdPt->m_IsShowToast = IsShowToast;
	MediaPocsThrdPt->m_ShowToastWndHdl = ShowToastWndHdl;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsUsePrvntSysSleep
 * 功能说明：媒体处理线程的设置是否使用阻止系统睡眠。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
			 IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
			 ShowToastWndHdl：[输入]，存放显示Toast窗口的句柄，为NULL表示桌面窗口。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUsePrvntSysSleep( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetIsUsePrvntSysSleep p_MediaMsgSetIsUsePrvntSysSleep;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetIsUsePrvntSysSleep.m_MediaMsgTyp = MediaMsgTypSetIsUsePrvntSysSleep;
	p_MediaMsgSetIsUsePrvntSysSleep.m_IsUsePrvntSysSleep = IsUsePrvntSysSleep;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetIsUsePrvntSysSleep, sizeof( p_MediaMsgSetIsUsePrvntSysSleep ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile
 * 功能说明：媒体处理线程的设置是否保存音视频输入输出到Avi文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FileFullPathVstrPt：[输入]，存放完整路径动态字符串的指针。
			 WrBufSzByt：[输入]，存放写入缓冲区的大小，单位为字节。
			 IsSaveAdoInpt：[输入]，存放是否保存音频输入，为非0表示要保存，为0表示不保存。
			 IsSaveAdoOtpt：[输入]，存放是否保存音频输出，为非0表示要保存，为0表示不保存。
			 IsSaveVdoInpt：[输入]，存放是否保存视频输入，为非0表示要保存，为0表示不保存。
			 IsSaveVdoOtpt：[输入]，存放是否保存视频输出，为非0表示要保存，为0表示不保存。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * FileFullPathVstrPt, size_t WrBufSzByt, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetIsSaveAdoVdoInptOtptToAviFile p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) ) && ( FileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "完整路径动态字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_MediaMsgTyp = MediaMsgTypSetIsSaveAdoVdoInptOtptToAviFile;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt = NULL;
	if( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) )
	{
		if( VstrInit( &p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt, Utf8, , FileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_WrBufSzByt = WrBufSzByt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveAdoInpt = IsSaveAdoInpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveAdoOtpt = IsSaveAdoOtpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveVdoInpt = IsSaveVdoInpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveVdoOtpt = IsSaveVdoOtpt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile, sizeof( p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSaveStngToFile
 * 功能说明：媒体处理线程的保存设置到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FullPathVstrPt：[输入]，存放完整路径动态字符串的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSaveStngToFile p_MediaMsgSaveStngToFile;
	p_MediaMsgSaveStngToFile.m_FullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( FullPathVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "完整路径动态字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSaveStngToFile.m_MediaMsgTyp = MediaMsgTypSaveStngToFile;
	p_MediaMsgSaveStngToFile.m_FullPathVstrPt = NULL;
	if( VstrInit( &p_MediaMsgSaveStngToFile.m_FullPathVstrPt, Utf8, , FullPathVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgSaveStngToFile, sizeof( p_MediaMsgSaveStngToFile ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSaveStngToFile.m_FullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSaveStngToFile.m_FullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdPrvntSysSleepInitOrDstoy
 * 功能说明：初始化或销毁媒体处理线程的阻止系统睡眠。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUsePrvntSysSleep：[输入]，存放是否使用阻止系统睡眠，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrd * MediaPocsThrdPt, int32_t IsInitPrvntSysSleep )
{
	if( IsInitPrvntSysSleep != 0 ) //如果要初始化阻止系统睡眠。
	{
		SetThreadExecutionState( ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_AWAYMODE_REQUIRED );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化阻止系统睡眠成功。" ) );
	}
	else //如果要销毁阻止系统睡眠。
	{
		SetThreadExecutionState( ES_CONTINUOUS );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁阻止系统睡眠成功。" ) );
	}
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程已经启动。" ) );
		goto Out;
	}

	if( ThrdInit( &MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt, ( LPTHREAD_START_ROUTINE )MediaPocsThrdRun, MediaPocsThrdPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
	{
		if( MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：初始化媒体处理线程失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化媒体处理线程成功。" ) );

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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgRqirExit p_MediaMsgRqirExit;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "退出标记不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgRqirExit.m_MediaMsgTyp = MediaMsgTypRqirExit;
	p_MediaMsgRqirExit.m_ExitFlag = ExitFlag;
	p_MediaMsgRqirExit.m_IsBlockWait = IsBlockWait;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgRqirExit, sizeof( p_MediaMsgRqirExit ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}
	MediaPocsThrdPt->m_ReadyExitCnt++; //设置准备退出计数递增。

	if( IsBlockWait != 0 ) //如果需要阻塞等待。
	{
		if( ExitFlag == 1 ) //如果是请求退出。
		{
			ThrdWaitGetExitCode( MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt, NULL, NULL ); //等待媒体处理线程退出。
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
 * 函数名称：MediaPocsThrdSendUserMsg
 * 功能说明：发送用户消息到媒体处理线程。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 MsgArgPt：[输入]，存放消息参数的指针。如果没有消息参数，则本参数为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSendUserMsg( MediaPocsThrd * MediaPocsThrdPt, void * MsgArgPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgUserMsg p_MediaMsgUserMsg;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgUserMsg.m_MediaMsgTyp = MediaMsgTypUserMsg;
	p_MediaMsgUserMsg.m_MsgArgPt = MsgArgPt;
	if( MediaPocsThrdPt->m_MediaMsgCntnr.PutTail( &p_MediaMsgUserMsg, sizeof( p_MediaMsgUserMsg ), NULL, 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoVdoInptOtptAviFileWriterInit
 * 功能说明：初始化媒体处理线程的音视频输入输出Avi文件写入器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoVdoInptOtptAviFileWriterInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt != 0 ) || ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt != 0 ) || ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt != 0 ) || ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt != 0 ) )
	{
		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt == NULL )
		{
			if( AviFileWriterInit( &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt, 5, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件 %vs 的Avi文件写入器成功。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件 %vs 的Avi文件写入器失败。原因：%vs" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			uint64_t p_AdoVdoStartTimeStamp = FuncGetTickAsMsec();
			AviFileWriterSetStartTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, p_AdoVdoStartTimeStamp, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件时间线的起始时间戳为 %uz64d 。" ), p_AdoVdoStartTimeStamp );
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt = new std::map< uint32_t, uint32_t >;
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件视频输出已编码格式原始流的索引映射失败。原因：内存不足。" ) );
				goto Out;
			}
		}

		if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset != 0 ) ) //如果要保存音频输入，且音频输入流时间戳要重置。
		{
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx == UINT32_MAX )
			{
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 1, &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输入Pcm格式原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输入Pcm格式原始流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 1, &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输入Pcm格式结果流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输入Pcm格式结果流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}

			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec, NULL );
			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件音频输入Pcm格式原始结果流的当前时间戳为 %uz64d 。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec );

			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 0; //设置音频输入流时间戳不重置。
		}

		if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset != 0 ) ) //如果要保存音频输出，且音频输出流时间戳要重置。
		{
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx == UINT32_MAX )
			{
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, 1, &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输出Pcm格式原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加音频输出Pcm格式原始流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}

			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件音频输出Pcm格式原始流的当前时间戳为 %uz64d 。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec );

			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 0; //设置音频输出流时间戳不重置。
		}

		if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt != 0 ) && ( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) && ( MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd != 0 ) ) //如果要保存视频输入，且已初始化视频输入，且视频输入不使用Yu12原始数据。
		{
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx == UINT32_MAX )
			{
				if( AviFileWriterAddVdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, '462H', 50, &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //最大采样频率应该尽量被1000整除。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加视频输入已编码格式结果流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加视频输入已编码格式结果流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
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
 * 函数名称：MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy
 * 功能说明：销毁媒体处理线程的音视频输入输出Avi文件写入器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt != NULL )
	{
		if( AviFileWriterDstoy( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音视频输入输出Avi文件写入器成功。" ) );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音视频输入输出Avi文件写入器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt = NULL;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx = UINT32_MAX;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx = UINT32_MAX;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 0;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx = UINT32_MAX;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 0;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = 0;
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx = UINT32_MAX;
		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt != NULL )
		{
			delete MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdTmpVarInit
 * 功能说明：初始化媒体处理线程的临时变量。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdTmpVarInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	MediaPocsThrdPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程的临时变量。
	if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
	{
		MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt = NULL;
		if( ( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt == NULL )
		{
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
			if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入Pcm格式结果帧的内存块失败。" ) );
				goto Out;
			}
		}
		if( ( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt == NULL )
		{
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
			if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入Pcm格式临时帧的内存块失败。" ) );
				goto Out;
			}
		}
		MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt = NULL;
		MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 1; //设置音频输入Pcm格式结果帧的语音活动状态为1，为了让在不使用语音活动检测的情况下永远都是有语音活动。
		if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd != 0 ) //如果音频输入不使用PCM原始数据。
		{
			if( ( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
			{
				free( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt );
				MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt = NULL;
			}
			if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt == NULL )
			{
				MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
				if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入已编码格式结果帧的内存块失败。" ) );
					goto Out;
				}
			}
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt = 0;
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans = 1; //设置音频输入已编码格式结果帧是否需要传输为1，为了让在不使用非连续传输的情况下永远都是需要传输。
		}
		else //如果音频输入要使用PCM原始数据。
		{
			if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt != NULL )
			{
				free( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt );
				MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt = NULL;
			}
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt = 0;
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans = 0;
		}
	}
	else
	{
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt = NULL;
		}
		MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 0;
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt = NULL;
		}
		MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt = 0;
		MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans = 0;
	}
	MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt = NULL;
	MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt = NULL;
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化媒体处理线程的临时变量成功。" ) );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdTmpVarDstoy
 * 功能说明：销毁媒体处理线程的临时变量。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdTmpVarDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	if( MediaPocsThrdPt->m_Thrd.m_IsInitThrdTmpVar != 0 )
	{
		MediaPocsThrdPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程的临时变量。
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt = NULL;
		}
		MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 0;
		if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt );
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt = NULL;
		}
		MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt = 0;
		MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans = 0;
		MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt = NULL;
		MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt = NULL;
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁媒体处理线程的临时变量成功。" ) );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoVdoInptOtptInit
 * 功能说明：初始化音视频输入输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoVdoInptOtptInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( MediaPocsThrdPt->m_AdoOtpt.m_IsUse != 0 ) //如果要使用音频输出。在初始化音频输入前初始化音频输出，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
	{
		if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit == 0 ) //如果未初始化音频输出。
		{
			if( AdoOtptInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_AdoOtpt.m_IsInit = 1; //设置已初始化音频输出。
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音视频输入输出Avi文件音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
			if( MediaPocsThrdPt->m_AdoInpt.m_IsUse == 0 ) //如果不使用音频输入。
			{
				MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
				MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
			} //如果要使用音频输入，就不设置已初始化音频输出，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
		}
		else //如果已初始化音频输出。
		{
			if( MediaPocsThrdPt->m_AdoInpt.m_IsUse != 0 ) //如果要使用音频输入。
			{
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInit == 0 ) //如果未初始化音频输入。
				{
					AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt ); //销毁并初始化音频输出设备和线程，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
					if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音视频输入输出Avi文件音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
				} //如果音频输入已初始化，表示音频输入输出都已初始化，无需再销毁并初始化。
			}
		}
	}
	else //如果不使用音频输出。
	{
		if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) //如果已初始化音频输出。
		{
			AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			MediaPocsThrdPt->m_AdoOtpt.m_IsInit = 0; //设置未初始化音频输出。
		}
	}

	if( MediaPocsThrdPt->m_AdoInpt.m_IsUse != 0 ) //如果要使用音频输入。
	{
		if( MediaPocsThrdPt->m_AdoInpt.m_IsInit == 0 ) //如果未初始化音频输入。
		{
			AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			if( AdoInptInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out; //在音频输出初始化后再初始化音频输入，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
			MediaPocsThrdPt->m_AdoInpt.m_IsInit = 1; //设置已初始化音频输入。
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音视频输入输出Avi文件音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
		}
		else //如果已初始化音频输入。
		{
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) //如果已初始化音频输出。
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart == 0 ) //如果音频输出线程未开始。
				{
					if( MediaPocsThrdPt->m_AdoInpt.m_Thrd.m_IsStartAdoOtptThrd != 0 ) //如果音频输入线程已开始音频输出线程。
					{
						AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt ); //销毁并初始化音频输入设备和线程，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
						AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音视频输入输出Avi文件音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
					} //如果音频输入线程未开始音频输出线程，就不用管，等一会音频输入线程就会开始音频输出线程。
				} //如果音频输出线程已开始，表示音频输入输出都已初始化，无需再销毁并初始化。
			}
			else //如果不使用音频输出。
			{
				if( ( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_SystemAecNsAgcMediaObjPt != NULL ) ) //如果要使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
				{
					AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt ); //销毁并初始化音频输入设备和线程，因为系统自带的声学回音消除器、噪音抑制器和自动增益控制器没有音频输出无法工作。
					AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
					if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音视频输入输出Avi文件音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
				}
				else
				{
					AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
				}
			}
		}
	}
	else //如果不使用音频输入。
	{
		if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) //如果已初始化音频输入。
		{
			AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
			MediaPocsThrdPt->m_AdoInpt.m_IsInit = 0; //设置未初始化音频输入。
			AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
		}
	}

	if( MediaPocsThrdPt->m_VdoInpt.m_IsUse != 0 ) //如果要使用视频输入。
	{
		if( MediaPocsThrdPt->m_VdoInpt.m_IsInit == 0 ) //如果未初始化视频输入。
		{
			if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_VdoInpt.m_IsInit = 1; //设置已初始化视频输入。
		}
	}
	else //如果不使用视频输入。
	{
		if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) //如果已初始化视频输入。
		{
			VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );
			MediaPocsThrdPt->m_VdoInpt.m_IsInit = 0; //设置未初始化视频输入。
		}
	}

	if( MediaPocsThrdPt->m_VdoOtpt.m_IsUse != 0 ) //如果要使用视频输出。
	{
		if( MediaPocsThrdPt->m_VdoOtpt.m_IsInit == 0 ) //如果未初始化视频输出。
		{
			if( VdoOtptInit( &MediaPocsThrdPt->m_VdoOtpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_VdoOtpt.m_IsInit = 1; //设置已初始化视频输出。
		}
	}
	else //如果不使用视频输出。
	{
		if( MediaPocsThrdPt->m_VdoOtpt.m_IsInit != 0 ) //如果已初始化视频输出。
		{
			VdoOtptDstoy( &MediaPocsThrdPt->m_VdoOtpt );
			MediaPocsThrdPt->m_VdoOtpt.m_IsInit = 0; //设置未初始化视频输出。
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
 * 函数名称：MediaPocsThrdAdoVdoInptOtptDstoy
 * 功能说明：销毁音视频输入输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdAdoVdoInptOtptDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) //如果未初始化音频输入。
	{
		AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
		MediaPocsThrdPt->m_AdoInpt.m_IsInit = 0; //设置未初始化音频输入。
		MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
	}

	if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) //如果已初始化音频输出。
	{
		AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
		MediaPocsThrdPt->m_AdoOtpt.m_IsInit = 0; //设置未初始化音频输出。
	}

	if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) //如果已初始化视频输入。
	{
		VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );
		MediaPocsThrdPt->m_VdoInpt.m_IsInit = 0; //设置未初始化视频输入。
	}

	if( MediaPocsThrdPt->m_VdoOtpt.m_IsInit != 0 ) //如果已初始化视频输出。
	{
		VdoOtptDstoy( &MediaPocsThrdPt->m_VdoOtpt );
		MediaPocsThrdPt->m_VdoOtpt.m_IsInit = 0; //设置未初始化视频输出。
	}
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
	int32_t p_TmpInt32;
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本地代码的指令集名称：%s。" ), ( sizeof( size_t ) == 4 ) ? "x86" : "x64" );

	MediaPocsThrdPt->m_RunFlag = MediaPocsThrd::RunFlagRun; //设置本线程运行标记为正在运行。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：开始准备音视频输入输出帧处理。" ) );

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启，相当于调用用户定义的初始化函数和音视频输入输出初始化函数。

	//媒体处理循环开始。
	while( true )
	{
		//因为IGlobalInterfaceTable::GetInterfaceFromGlobal会向创建此Com对象的单元套间线程发送消息，所以要求创建此Com对象的单元套间线程有消息处理，若没有，则IGlobalInterfaceTable::GetInterfaceFromGlobal会一直阻塞。
		/*{
			MSG msg;
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0 )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}*/

		int p_MediaPocsRslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
		size_t p_MediaMsgCntnrElmTotal; //存放媒体消息容器的元素总数。
		MediaMsg p_MediaMsg;

		{
			MediaPocsThrdPt->m_MediaMsgCntnr.GetTotal( &p_MediaMsgCntnrElmTotal, 1, NULL ); //从媒体消息容器中取出并删除第一个媒体消息。
			if( p_MediaMsgCntnrElmTotal > 0 ) //如果有媒体消息需要处理。
			{
				MediaPocsThrdPt->m_MediaMsgCntnr.GetHead( &p_MediaMsg, sizeof( p_MediaMsg ), NULL, NULL, 1, 1, NULL );
				switch( p_MediaMsg.m_MediaMsgTyp )
				{
					case MediaMsgTypSetAdoInpt:
					{
						MediaMsgSetAdoInpt * p_MediaMsgSetAdoInptPt = ( MediaMsgSetAdoInpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_SmplRate = p_MediaMsgSetAdoInptPt->m_SmplRate;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec = p_MediaMsgSetAdoInptPt->m_FrmLenMsec;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoInpt.m_SmplRate / 1000;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenData = MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit * 1;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoInpt.m_FrmLenData * sizeof( int16_t );

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
							if( AdoInptInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdTmpVarInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetIsUseSystemAecNsAgc:
					{
						MediaMsgAdoInptSetIsUseSystemAecNsAgc * p_MediaMsgAdoInptSetIsUseSystemAecNsAgcPt = ( MediaMsgAdoInptSetIsUseSystemAecNsAgc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_IsUseSystemAecNsAgc = p_MediaMsgAdoInptSetIsUseSystemAecNsAgcPt->m_IsUseSystemAecNsAgc;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseNoAec:
					{
						MediaMsgAdoInptSetUseNoAec * p_MediaMsgAdoInptSetUseNoAecPt = ( MediaMsgAdoInptSetUseNoAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseSpeexAec:
					{
						MediaMsgAdoInptSetUseSpeexAec * p_MediaMsgAdoInptSetUseSpeexAecPt = ( MediaMsgAdoInptSetUseSpeexAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_FilterLenMsec = p_MediaMsgAdoInptSetUseSpeexAecPt->m_FilterLenMsec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsUseRec = p_MediaMsgAdoInptSetUseSpeexAecPt->m_IsUseRec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoMutp = p_MediaMsgAdoInptSetUseSpeexAecPt->m_EchoMutp;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoCntu = p_MediaMsgAdoInptSetUseSpeexAecPt->m_EchoCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupes = p_MediaMsgAdoInptSetUseSpeexAecPt->m_EchoSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupesAct = p_MediaMsgAdoInptSetUseSpeexAecPt->m_EchoSupesAct;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsSaveMemFile = p_MediaMsgAdoInptSetUseSpeexAecPt->m_IsSaveMemFile;
						if( p_MediaMsgAdoInptSetUseSpeexAecPt->m_MemFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt, p_MediaMsgAdoInptSetUseSpeexAecPt->m_MemFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgAdoInptSetUseSpeexAecPt->m_MemFileFullPathVstrPt );
						}

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseWebRtcAecm:
					{
						MediaMsgAdoInptSetUseWebRtcAecm * p_MediaMsgAdoInptSetUseWebRtcAecmPt = ( MediaMsgAdoInptSetUseWebRtcAecm * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 2;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode = p_MediaMsgAdoInptSetUseWebRtcAecmPt->m_IsUseCNGMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_EchoMode = p_MediaMsgAdoInptSetUseWebRtcAecmPt->m_EchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Delay = p_MediaMsgAdoInptSetUseWebRtcAecmPt->m_Delay;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseWebRtcAec:
					{
						MediaMsgAdoInptSetUseWebRtcAec * p_MediaMsgAdoInptSetUseWebRtcAecPt = ( MediaMsgAdoInptSetUseWebRtcAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 3;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_EchoMode = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_EchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Delay = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_Delay;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_IsUseDelayAgstcMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_IsUseExtdFilterMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_IsUseRefinedFilterAdaptAecMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_IsUseAdaptAdjDelay;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsSaveMemFile = p_MediaMsgAdoInptSetUseWebRtcAecPt->m_IsSaveMemFile;
						if( p_MediaMsgAdoInptSetUseWebRtcAecPt->m_MemFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt, p_MediaMsgAdoInptSetUseWebRtcAecPt->m_MemFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgAdoInptSetUseWebRtcAecPt->m_MemFileFullPathVstrPt );
						}

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseSpeexWebRtcAec:
					{
						MediaMsgAdoInptSetUseSpeexWebRtcAec * p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt = ( MediaMsgAdoInptSetUseSpeexWebRtcAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 4;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WorkMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecFilterLenMsec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecIsUseRec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoMutp;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoSupesAct;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmIsUseCNGMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmEchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecEchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseDelayAgstcMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseExtdFilterMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseAdaptAdjDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_IsUseSameRoomAec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay = p_MediaMsgAdoInptSetUseSpeexWebRtcAecPt->m_SameRoomEchoMinDelay;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseNoNs:
					{
						MediaMsgAdoInptSetUseNoNs * p_MediaMsgAdoInptSetUseNoNsPt = ( MediaMsgAdoInptSetUseNoNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseSpeexPrpocsNs:
					{
						MediaMsgAdoInptSetUseSpeexPrpocsNs * p_MediaMsgAdoInptSetUseSpeexPrpocsNsPt = ( MediaMsgAdoInptSetUseSpeexPrpocsNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs = p_MediaMsgAdoInptSetUseSpeexPrpocsNsPt->m_IsUseNs;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes = p_MediaMsgAdoInptSetUseSpeexPrpocsNsPt->m_NoiseSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb = p_MediaMsgAdoInptSetUseSpeexPrpocsNsPt->m_IsUseDereverb;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseWebRtcNsx:
					{
						MediaMsgAdoInptSetUseWebRtcNsx * p_MediaMsgAdoInptSetUseWebRtcNsxPt = ( MediaMsgAdoInptSetUseWebRtcNsx * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 2;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsx.m_PolicyMode = p_MediaMsgAdoInptSetUseWebRtcNsxPt->m_PolicyMode;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseWebRtcNs:
					{
						MediaMsgAdoInptSetUseWebRtcNs * p_MediaMsgAdoInptSetUseWebRtcNsPt = ( MediaMsgAdoInptSetUseWebRtcNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 3;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNs.m_PolicyMode = p_MediaMsgAdoInptSetUseWebRtcNsPt->m_PolicyMode;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUseRNNoise:
					{
						MediaMsgAdoInptSetUseRNNoise * p_MediaMsgAdoInptSetUseRNNoisePt = ( MediaMsgAdoInptSetUseRNNoise * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 4;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetIsUseSpeexPrpocs:
					{
						MediaMsgAdoInptSetIsUseSpeexPrpocs * p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt = ( MediaMsgAdoInptSetIsUseSpeexPrpocs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseSpeexPrpocs;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseVad = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseVad;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbStart = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_VadProbStart;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_VadProbCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseAgc;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcLevel = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcLevel;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcIncrement;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcDecrement;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain = p_MediaMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcMaxGain;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 1;
						}
						break;
					}
					case MediaMsgTypAdoInptSetUsePcm:
					{
						MediaMsgAdoInptSetUsePcm * p_MediaMsgAdoInptSetUsePcmPt = ( MediaMsgAdoInptSetUsePcm * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MediaMsgTypAdoInptSetUseSpeexEncd:
					{
						MediaMsgAdoInptSetUseSpeexEncd * p_MediaMsgAdoInptSetUseSpeexEncdPt = ( MediaMsgAdoInptSetUseSpeexEncd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr = p_MediaMsgAdoInptSetUseSpeexEncdPt->m_UseCbrOrVbr;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Qualt = p_MediaMsgAdoInptSetUseSpeexEncdPt->m_Qualt;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Cmplxt = p_MediaMsgAdoInptSetUseSpeexEncdPt->m_Cmplxt;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate = p_MediaMsgAdoInptSetUseSpeexEncdPt->m_PlcExptLossRate;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MediaMsgTypAdoInptSetUseOpusEncd:
					{
						MediaMsgAdoInptSetUseOpusEncd * p_MediaMsgAdoInptSetUseOpusEncdPt = ( MediaMsgAdoInptSetUseOpusEncd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 2;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MediaMsgTypAdoInptSetIsSaveAdoToWaveFile:
					{
						MediaMsgAdoInptSetIsSaveAdoToWaveFile * p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt = ( MediaMsgAdoInptSetIsSaveAdoToWaveFile * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_IsSaveAdoToWaveFile = p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_IsSaveAdoToWaveFile;
						if( p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt, p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt );
						}
						if( p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltWaveFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt, p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltWaveFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltWaveFileFullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_WaveFileWrBufSzByt = p_MediaMsgAdoInptSetIsSaveAdoToWaveFilePt->m_WaveFileWrBufSzByt;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypAdoInptSetIsDrawAdoWavfmToWnd:
					{
						MediaMsgAdoInptSetIsDrawAdoWavfmToWnd * p_MediaMsgAdoInptSetIsDrawAdoWavfmToWndPt = ( MediaMsgAdoInptSetIsDrawAdoWavfmToWnd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptWavfmDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDrawAdoWavfmToWnd = p_MediaMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_IsDrawAdoWavfmToWnd;
						MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWavfmWndHdl = p_MediaMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_SrcWavfmWndHdl;
						MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWavfmWndHdl = p_MediaMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_RsltWavfmWndHdl;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptWavfmInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypAdoInptSetUseDvc:
					{
						MediaMsgAdoInptSetUseDvc * p_MediaMsgAdoInptSetUseDvcPt = ( MediaMsgAdoInptSetUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_ID = p_MediaMsgAdoInptSetUseDvcPt->m_ID;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
						{
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoInptSetIsMute:
					{
						MediaMsgAdoInptSetIsMute * p_MediaMsgAdoInptSetIsMutePt = ( MediaMsgAdoInptSetIsMute * )&p_MediaMsg;

						MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsMute = p_MediaMsgAdoInptSetIsMutePt->m_IsMute;
						break;
					}
					case MediaMsgTypSetAdoOtpt:
					{
						MediaMsgSetAdoOtpt * p_MediaMsgSetAdoOtptPt = ( MediaMsgSetAdoOtpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
						{
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoOtpt.m_SmplRate = p_MediaMsgSetAdoOtptPt->m_SmplRate;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec = p_MediaMsgSetAdoOtptPt->m_FrmLenMsec;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit * 1;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData * sizeof( int16_t );

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
						{
							if( AdoOtptInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
							{
								AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
								if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							}
							else
							{
								MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
								MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
							}
						}
						break;
					}
					case MediaMsgTypAdoOtptAddStrm:
					{
						MediaMsgAdoOtptAddStrm * p_MediaMsgAdoOtptAddStrmPt = ( MediaMsgAdoOtptAddStrm * )&p_MediaMsg;

						AdoOtptAddStrm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptAddStrmPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypAdoOtptDelStrm:
					{
						MediaMsgAdoOtptDelStrm * p_MediaMsgAdoOtptDelStrmPt = ( MediaMsgAdoOtptDelStrm * )&p_MediaMsg;

						AdoOtptDelStrm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptDelStrmPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypAdoOtptSetStrmUsePcm:
					{
						MediaMsgAdoOtptSetStrmUsePcm * p_MediaMsgAdoOtptSetStrmUsePcmPt = ( MediaMsgAdoOtptSetStrmUsePcm * )&p_MediaMsg;

						AdoOtptSetStrmUsePcm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptSetStrmUsePcmPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypAdoOtptSetStrmUseSpeexDecd:
					{
						MediaMsgAdoOtptSetStrmUseSpeexDecd * p_MediaMsgAdoOtptSetStrmUseSpeexDecdPt = ( MediaMsgAdoOtptSetStrmUseSpeexDecd * )&p_MediaMsg;

						AdoOtptSetStrmUseSpeexDecd( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptSetStrmUseSpeexDecdPt->m_StrmIdx, p_MediaMsgAdoOtptSetStrmUseSpeexDecdPt->m_IsUsePrcplEnhsmt );
						break;
					}
					case MediaMsgTypAdoOtptSetStrmUseOpusDecd:
					{
						MediaMsgAdoOtptSetStrmUseOpusDecd * p_MediaMsgAdoOtptSetStrmUseOpusDecdPt = ( MediaMsgAdoOtptSetStrmUseOpusDecd * )&p_MediaMsg;

						AdoOtptSetStrmUseOpusDecd( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptSetStrmUseOpusDecdPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypAdoOtptSetStrmIsUse:
					{
						MediaMsgAdoOtptSetStrmIsUse * p_MediaMsgAdoOtptSetStrmIsUsePt = ( MediaMsgAdoOtptSetStrmIsUse * )&p_MediaMsg;

						AdoOtptSetStrmIsUse( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAdoOtptSetStrmIsUsePt->m_StrmIdx, p_MediaMsgAdoOtptSetStrmIsUsePt->m_IsUse );
						break;
					}
					case MediaMsgTypAdoOtptSetIsSaveAdoToWaveFile:
					{
						MediaMsgAdoOtptSetIsSaveAdoToWaveFile * p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt = ( MediaMsgAdoOtptSetIsSaveAdoToWaveFile * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoOtpt );

						MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_IsSaveAdoToWaveFile = p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_IsSaveAdoToWaveFile;
						if( p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt, p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcWaveFileFullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_WaveFileWrBufSzByt = p_MediaMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_WaveFileWrBufSzByt;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypAdoOtptSetIsDrawAdoWavfmToWnd:
					{
						MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd * p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWndPt = ( MediaMsgAdoOtptSetIsDrawAdoWavfmToWnd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptWavfmDstoy( &MediaPocsThrdPt->m_AdoOtpt );

						MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDrawAdoWavfmToWnd = p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWndPt->m_IsDrawAdoWavfmToWnd;
						MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWavfmWndHdl = p_MediaMsgAdoOtptSetIsDrawAdoWavfmToWndPt->m_SrcWavfmWndHdl;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptWavfmInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypAdoOtptSetUseDvc:
					{
						MediaMsgAdoOtptSetUseDvc * p_MediaMsgAdoOtptSetUseDvcPt = ( MediaMsgAdoOtptSetUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
						{
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ID = p_MediaMsgAdoOtptSetUseDvcPt->m_ID;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
						{
							if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
							{
								if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							}
							else
							{
								MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
								MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
							}
						}
						break;
					}
					case MediaMsgTypAdoOtptSetIsMute:
					{
						MediaMsgAdoOtptSetIsMute * p_MediaMsgAdoOtptSetIsMutePt = ( MediaMsgAdoOtptSetIsMute * )&p_MediaMsg;

						MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_IsMute = p_MediaMsgAdoOtptSetIsMutePt->m_IsMute;
						break;
					}
					case MediaMsgTypSetVdoInpt:
					{
						MediaMsgSetVdoInpt * p_MediaMsgSetVdoInptPt = ( MediaMsgSetVdoInpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate = p_MediaMsgSetVdoInptPt->m_MaxSmplRate;
						MediaPocsThrdPt->m_VdoInpt.m_FrmWidth = p_MediaMsgSetVdoInptPt->m_FrmWidth;
						MediaPocsThrdPt->m_VdoInpt.m_FrmHeight = p_MediaMsgSetVdoInptPt->m_FrmHeight;
						MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt = p_MediaMsgSetVdoInptPt->m_FrmWidth * p_MediaMsgSetVdoInptPt->m_FrmHeight * 3 / 2;
						MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl = p_MediaMsgSetVdoInptPt->m_PrvwWndHdl;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypVdoInptSetUseYu12:
					{
						MediaMsgSetVdoInptUseYu12 * p_MediaMsgSetVdoInptUseYu12Pt = ( MediaMsgSetVdoInptUseYu12 * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 0;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypVdoInptSetUseOpenH264Encd:
					{
						MediaMsgSetVdoInptUseOpenH264Encd * p_MediaMsgSetVdoInptUseOpenH264EncdPt = ( MediaMsgSetVdoInptUseOpenH264Encd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 1;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_VdoType = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_VdoType;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_EncdBitrate = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_EncdBitrate;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_BitrateCtrlMode;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_IDRFrmIntvl;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_Cmplxt = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_Cmplxt;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypVdoInptSetUseDvc:
					{
						MediaMsgSetVdoInptUseDvc * p_MediaMsgSetVdoInptUseDvcPt = ( MediaMsgSetVdoInptUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_ID = p_MediaMsgSetVdoInptUseDvcPt->m_ID;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypVdoInptSetIsBlack:
					{
						MediaMsgSetVdoInptIsBlack * p_MediaMsgSetVdoInptIsBlackPt = ( MediaMsgSetVdoInptIsBlack * )&p_MediaMsg;

						MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_IsBlack = p_MediaMsgSetVdoInptIsBlackPt->m_IsBlack;
						break;
					}
					case MediaMsgTypVdoOtptAddStrm:
					{
						MediaMsgAddVdoOtptStrm * p_MediaMsgAddVdoOtptStrmPt = ( MediaMsgAddVdoOtptStrm * )&p_MediaMsg;

						VdoOtptAddStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgAddVdoOtptStrmPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypVdoOtptDelStrm:
					{
						MediaMsgDelVdoOtptStrm * p_MediaMsgDelVdoOtptStrmPt = ( MediaMsgDelVdoOtptStrm * )&p_MediaMsg;

						VdoOtptDelStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgDelVdoOtptStrmPt->m_StrmIdx );
						break;
					}
					case MediaMsgTypVdoOtptSetStrm:
					{
						MediaMsgSetVdoOtptStrm * p_MediaMsgSetVdoOtptStrmPt = ( MediaMsgSetVdoOtptStrm * )&p_MediaMsg;

						VdoOtptSetStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmPt->m_StrmIdx, p_MediaMsgSetVdoOtptStrmPt->m_DspyWndHdl );
						break;
					}
					case MediaMsgTypVdoOtptSetStrmUseYu12:
					{
						MediaMsgSetVdoOtptStrmUseYu12 * p_MediaMsgSetVdoOtptStrmUseYu12Pt = ( MediaMsgSetVdoOtptStrmUseYu12 * )&p_MediaMsg;

						VdoOtptSetStrmUseYu12( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmUseYu12Pt->m_StrmIdx );
						break;
					}
					case MediaMsgTypVdoOtptSetStrmUseOpenH264Decd:
					{
						MediaMsgSetVdoOtptStrmUseOpenH264Decd * p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt = ( MediaMsgSetVdoOtptStrmUseOpenH264Decd * )&p_MediaMsg;

						VdoOtptSetStrmUseOpenH264Decd( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt->m_StrmIdx, p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt->m_DecdThrdNum );
						break;
					}
					case MediaMsgTypVdoOtptSetStrmIsBlack:
					{
						MediaMsgSetVdoOtptStrmIsBlack * p_MediaMsgSetVdoOtptStrmIsBlackPt = ( MediaMsgSetVdoOtptStrmIsBlack * )&p_MediaMsg;

						VdoOtptSetStrmIsBlack( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmIsBlackPt->m_StrmIdx, p_MediaMsgSetVdoOtptStrmIsBlackPt->m_IsBlack );
						break;
					}
					case MediaMsgTypVdoOtptSetStrmIsUse:
					{
						MediaMsgSetVdoOtptStrmIsUse * p_MediaMsgSetVdoOtptStrmIsUsePt = ( MediaMsgSetVdoOtptStrmIsUse * )&p_MediaMsg;

						VdoOtptSetStrmIsUse( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmIsUsePt->m_StrmIdx, p_MediaMsgSetVdoOtptStrmIsUsePt->m_IsUse );
						break;
					}
					case MediaMsgTypSetIsUseAdoVdoInptOtpt:
					{
						MediaMsgSetIsUseAdoVdoInptOtpt * p_MediaMsgSetIsUseAdoVdoInptOtptPt = ( MediaMsgSetIsUseAdoVdoInptOtpt * )&p_MediaMsg;

						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt >= 0 ) MediaPocsThrdPt->m_AdoInpt.m_IsUse = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt >= 0 ) MediaPocsThrdPt->m_AdoOtpt.m_IsUse = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt >= 0 ) MediaPocsThrdPt->m_VdoInpt.m_IsUse = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt >= 0 ) MediaPocsThrdPt->m_VdoOtpt.m_IsUse = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt;

						MediaMsgAdoVdoInptOtptInit p_MediaMsgSetIsUseAdoVdoInptOtpt;
						p_MediaMsgSetIsUseAdoVdoInptOtpt.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptInit;
						if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgSetIsUseAdoVdoInptOtpt, sizeof( p_MediaMsgSetIsUseAdoVdoInptOtpt ), NULL, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypSetIsUsePrvntSysSleep:
					{
						MediaMsgSetIsUsePrvntSysSleep * p_MediaMsgSetIsUsePrvntSysSleepPt = ( MediaMsgSetIsUsePrvntSysSleep * )&p_MediaMsg;
						MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, p_MediaMsgSetIsUsePrvntSysSleepPt->m_IsUsePrvntSysSleep );
						break;
					}
					case MediaMsgTypSetIsSaveAdoVdoInptOtptToAviFile:
					{
						MediaMsgSetIsSaveAdoVdoInptOtptToAviFile * p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt = ( MediaMsgSetIsSaveAdoVdoInptOtptToAviFile * )&p_MediaMsg;
						//MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy( MediaPocsThrdPt ); //这里不用销毁。

						if( p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt );
							VstrDstoy( p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt = p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_WrBufSzByt;
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveAdoInpt;
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveAdoOtpt;
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveVdoInpt;
						MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveVdoOtpt;

						if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) if( MediaPocsThrdAdoVdoInptOtptAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MediaMsgTypSaveStngToFile:
					{
						MediaMsgSaveStngToFile * p_MediaMsgSaveStngToFilePt = ( MediaMsgSaveStngToFile * )&p_MediaMsg;
						File * p_StngFilePt;

						if( FileInitByPath( &p_StngFilePt, p_MediaMsgSaveStngToFilePt->m_FullPathVstrPt, NoRd_Wr, Create_AlExist_Clr, 4096, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化设置文件 %vs 失败。原因：%vs" ), p_MediaMsgSaveStngToFilePt->m_FullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							VstrDstoy( p_MediaMsgSaveStngToFilePt->m_FullPathVstrPt );
							goto OutMediaPocs;
						}

						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsPrintLog：%z32d\n" ), MediaPocsThrdPt->m_IsPrintLog );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsShowToast：%z32d\n" ), MediaPocsThrdPt->m_IsShowToast );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_ShowToastWndHdl：%#P\n" ), MediaPocsThrdPt->m_ShowToastWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsUsePrvntSysSleep：%z32d\n" ), MediaPocsThrdPt->m_IsUsePrvntSysSleep );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_FullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_WrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsUse );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsInit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenMsec：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenUnit：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenData：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenData );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_FilterLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_FilterLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_IsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsUseRec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoMutp );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoCntu );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupes );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupesAct );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_IsSaveMemFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsSaveMemFile );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_MemFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_EchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_EchoMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Delay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_EchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_EchoMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Delay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsSaveMemFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsSaveMemFile );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_MemFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNsx.m_PolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsx.m_PolicyMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNs.m_PolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNs.m_PolicyMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseVad：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseVad );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_VadProbStart：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbStart );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcLevel：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcLevel );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_Qualt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Qualt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_Cmplxt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Cmplxt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDrawAdoWavfmToWnd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_SrcWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_RsltWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFile.m_IsSaveAdoToWaveFile：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_IsSaveAdoToWaveFile );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFile.m_WaveFileWrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_WaveFileWrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Dvc.m_ID：%ud\n" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_ID );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Dvc.m_IsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsMute );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsUse );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsInit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_SmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenMsec：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenUnit：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenData：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_StrmCntnr.m_CQueuePt：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.m_CQueuePt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						{
							AdoOtpt::Strm * p_AdoOtptStrmPt; //存放音频输出流的指针。
							for( size_t p_StrmNum = 0; MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_AdoOtptStrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
							{
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Idx：%z32d\n" ), p_AdoOtptStrmPt->m_Idx );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsUse：%z32d\n" ), p_AdoOtptStrmPt->m_IsUse );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_UseWhatDecd：%z32d\n" ), p_AdoOtptStrmPt->m_UseWhatDecd );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SpeexDecd.m_IsUsePrcplEnhsmt：%z32d\n" ), p_AdoOtptStrmPt->m_SpeexDecd.m_IsUsePrcplEnhsmt );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
							}
							FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_StrmUseTotal：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_StrmUseTotal );
							FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						}
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Wavfm.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDrawAdoWavfmToWnd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Wavfm.m_SrcWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFile.m_IsSaveAdoToWaveFile：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_IsSaveAdoToWaveFile );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFile.m_WaveFileWrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_WaveFileWrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_ID：%ud\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ID );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_BufSzUnit：%uz32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_BufSzUnit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_IsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_IsMute );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsUse );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsInit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_MaxSmplRate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmWidth：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmWidth );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmHeight：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmHeight );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Yu12FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_VdoType：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_VdoType );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_EncdBitrate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_EncdBitrate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_Cmplxt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_Cmplxt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_ID：%ud\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_ID );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_PrvwWndHdl：%#P\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_IsBlack：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_IsBlack );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsUse );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsInit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_CQueuePt：%#P\n" ), MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.m_CQueuePt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						{
							VdoOtpt::Strm * p_VdoOtptStrmPt; //存放视频输出流的指针。
							for( size_t p_StrmNum = 0; MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_VdoOtptStrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
							{
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_Idx：%z32d\n" ), p_VdoOtptStrmPt->m_Idx );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_IsUse：%z32d\n" ), p_VdoOtptStrmPt->m_IsUse );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_UseWhatDecd：%z32d\n" ), p_VdoOtptStrmPt->m_UseWhatDecd );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_OpenH264Decd.m_DecdThrdNum：%z32d\n" ), p_VdoOtptStrmPt->m_OpenH264Decd.m_DecdThrdNum );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_Dvc.m_DspyWndHdl：%#P\n" ), p_VdoOtptStrmPt->m_Dvc.m_DspyWndHdl );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_Dvc.m_IsBlack：%z32d\n" ), p_VdoOtptStrmPt->m_Dvc.m_IsBlack );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
							}
						}

						FileDstoy( p_StngFilePt, 1, NULL );
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：保存设置到文件 %vs 成功。" ), p_MediaMsgSaveStngToFilePt->m_FullPathVstrPt );
						VstrDstoy( p_MediaMsgSaveStngToFilePt->m_FullPathVstrPt );
						break;
					}
					case MediaMsgTypRqirExit:
					{
						MediaMsgRqirExit * p_MediaMsgRqirExitPt = ( MediaMsgRqirExit * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：接收到退出请求，开始准备退出。" ) );

						switch( p_MediaMsgRqirExitPt->m_ExitFlag )
						{
							case 1: //为请求退出。
							{
								//执行顺序：媒体销毁，用户销毁并退出。
								MediaPocsThrdPt->m_MediaMsgCntnr.Locked( NULL );
								if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
								{
									MediaMsgUserDstoy p_MediaMsgUserDstoy;
									p_MediaMsgUserDstoy.m_MediaMsgTyp = MediaMsgTypUserDstoy;
									if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto OutMediaPocs;
									}
									MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
									p_MediaMsgAdoVdoInptOtptDstoy.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptDstoy;
									if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto OutMediaPocs;
									}
								}
								else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
								{
									MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								}
								MediaPocsThrdPt->m_MediaMsgCntnr.Unlock( NULL );
								break;
							}
							case 2: //请求重启。
							{
								//执行顺序：媒体销毁，用户销毁，用户初始化，媒体初始化。
								MediaPocsThrdPt->m_MediaMsgCntnr.Locked( NULL );
								MediaMsgAdoVdoInptOtptInit p_MediaMsgAdoVdoInptOtptInit;
								p_MediaMsgAdoVdoInptOtptInit.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptInit;
								if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptInit, sizeof( p_MediaMsgAdoVdoInptOtptInit ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto OutMediaPocs;
								}
								MediaMsgUserInit p_MediaMsgUserInit;
								p_MediaMsgUserInit.m_MediaMsgTyp = MediaMsgTypUserInit;
								if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgUserInit, sizeof( p_MediaMsgUserInit ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto OutMediaPocs;
								}
								if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
								{
									MediaMsgUserDstoy p_MediaMsgUserDstoy;
									p_MediaMsgUserDstoy.m_MediaMsgTyp = MediaMsgTypUserDstoy;
									if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto OutMediaPocs;
									}
									MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
									p_MediaMsgAdoVdoInptOtptDstoy.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptDstoy;
									if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto OutMediaPocs;
									}
									MediaPocsThrdPt->m_MediaMsgCntnr.Locked( NULL );
								}
								else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
								{
									MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								}
								MediaPocsThrdPt->m_MediaMsgCntnr.Unlock( NULL );
								break;
							}
							case 3: //请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
							{
								//执行顺序：媒体销毁，媒体初始化。
								MediaPocsThrdPt->m_MediaMsgCntnr.Locked( NULL );
								MediaMsgAdoVdoInptOtptInit p_MediaMsgAdoVdoInptOtptInit;
								p_MediaMsgAdoVdoInptOtptInit.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptInit;
								if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptInit, sizeof( p_MediaMsgAdoVdoInptOtptInit ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto OutMediaPocs;
								}
								MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
								p_MediaMsgAdoVdoInptOtptDstoy.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptDstoy;
								if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto OutMediaPocs;
								}
								MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								MediaPocsThrdPt->m_MediaMsgCntnr.Unlock( NULL );
								break;
							}
						}
						break;
					}
					case MediaMsgTypUserInit:
					{
						MediaMsgUserInit * p_MediaMsgUserInitPt = ( MediaMsgUserInit * )&p_MediaMsg;

						MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeNormal; //清空退出码。
						MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 0; //设置上一次调用了用户定义的初始化函数。
						MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
						p_TmpInt32 = MediaPocsThrdPt->m_UserInitFuncPt( MediaPocsThrdPt ); //调用用户定义的初始化函数。
						if( p_TmpInt32 == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数成功。返回值：%d。" ), p_TmpInt32 );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数失败。返回值：%d。" ), p_TmpInt32 );
							goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypUserDstoy:
					{
						MediaMsgUserDstoy * p_MediaMsgUserDstoyPt = ( MediaMsgUserDstoy * )&p_MediaMsg;

						MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 1;
						MediaPocsThrdPt->m_UserDstoyFuncPt( MediaPocsThrdPt ); //调用用户定义的销毁函数。
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的销毁函数成功。" ) );
						break;
					}
					case MediaMsgTypUserMsg:
					{
						MediaMsgUserMsg * p_MediaMsgUserMsgPt = ( MediaMsgUserMsg * )&p_MediaMsg;

						p_TmpInt32 = MediaPocsThrdPt->m_UserMsgFuncPt( MediaPocsThrdPt, p_MediaMsgUserMsgPt->m_MsgArgPt );
						if( p_TmpInt32 == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的消息函数成功。返回值：%d。" ), p_TmpInt32 );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的消息函数失败。返回值：%d。" ), p_TmpInt32 );
							goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoVdoInptOtptInit:
					{
						if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数，就初始化音视频输入输出、音视频输入输出Avi文件写入器。
						{
							if( MediaPocsThrdAdoVdoInptOtptInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdAdoVdoInptOtptAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MediaMsgTypAdoVdoInptOtptDstoy:
					{
						MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy( MediaPocsThrdPt );
						MediaPocsThrdAdoVdoInptOtptDstoy( MediaPocsThrdPt );
						break;
					}
				}
			}
			else //如果没有媒体消息需要处理。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec();

				//调用用户定义的处理函数。
				{
					p_TmpInt32 = MediaPocsThrdPt->m_UserPocsFuncPt( MediaPocsThrdPt );
					if( p_TmpInt32 == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数成功。返回值：%d" ), p_TmpInt32 );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的处理函数失败。返回值：%d" ), p_TmpInt32 );
						goto OutMediaPocs;
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				}

				//取出音频输入Pcm格式原始帧和音频输出Pcm格式原始帧。
				if( MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.m_CQueuePt != NULL ) MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取音频输入Pcm格式原始帧容器的元素总数。
				else p_TmpSz1 = 0;
				if( MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.m_CQueuePt != NULL ) MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.GetTotal( &p_TmpSz2, 1, NULL ); //获取音频输出Pcm格式原始帧容器的元素总数。
				else p_TmpSz2 = 0;
				if( MediaPocsThrdPt->m_AdoInpt.m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器。
				{
					if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) ) //如果音频输入Pcm格式原始帧容器和音频输出Pcm格式原始帧容器中都有帧了，就开始取出。
					{
						MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, NULL, 1, 1, NULL ); //从音频输入Pcm格式原始帧容器中取出并删除第一个帧。
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输入Pcm格式原始帧容器中取出并删除第一个帧，音频输入Pcm格式原始帧容器元素总数：%z32d。" ), p_TmpSz1 );

						//从音频输出Pcm格式原始帧容器中取出并删除第一个帧。
						{
							MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, NULL, 1, 1, NULL );
						}
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输出Pcm格式原始帧容器中取出并删除第一个帧，音频输出Pcm格式原始帧容器元素总数：%z32d。" ), p_TmpSz2 );

						//将音频输入Pcm格式原始帧复制到音频输入Pcm格式结果帧，方便处理。
						memcpy( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
					}
				}
				else //如果不可以使用声学回音消除器。
				{
					if( p_TmpSz1 > 0 ) //如果音频输入Pcm格式原始帧容器有帧了，就开始取出。
					{
						MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, NULL, 1, 1, NULL ); //从音频输入Pcm格式原始帧容器中取出并删除第一个帧。
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输入Pcm格式原始帧容器中取出并删除第一个帧，音频输入Pcm格式原始帧容器元素总数：%z32d。" ), p_TmpSz1 );

						//将音频输入Pcm格式原始帧复制到音频输入Pcm格式结果帧，方便处理。
						memcpy( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
					}

					if( p_TmpSz2 > 0 ) //如果音频输出Pcm格式原始帧容器有帧了，就开始取出。
					{
						MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, NULL, 1, 1, NULL ); //从音频输出Pcm格式原始帧容器中取出并删除第一个帧。
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从音频输出Pcm格式原始帧容器中取出并删除第一个帧，音频输出Pcm格式原始帧容器元素总数：%z32d。" ), p_TmpSz2 );
					}
				}

				//处理音频输入帧开始。
				if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL )
				{
					//使用声学回音消除器。
					if( MediaPocsThrdPt->m_AdoInpt.m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器。
					{
						switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec )
						{
							case 0: //如果不使用声学回音消除器。
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用声学回音消除器。" ) );
								break;
							}
							case 1: //如果要使用Speex声学回音消除器。
							{
								if( SpeexAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器失败。" ) );
								}
								break;
							}
							case 2: //如果要使用WebRtc定点版声学回音消除器。
							{
								if( WebRtcAecmPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器失败。" ) );
								}
								break;
							}
							case 3: //如果要使用WebRtc浮点版声学回音消除器。
							{
								if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt != NULL ) && ( WebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 ) )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器失败。" ) );
								}
								break;
							}
							case 4: //如果要使用SpeexWebRtc三重声学回音消除器。
							{
								if( SpeexWebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器失败。" ) );
								}
								break;
							}
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
						case 1: //如果要使用Speex预处理器的噪音抑制。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：在使用Speex预处理器时一起使用噪音抑制。" ) );
							break;
						}
						case 2: //如果要使用WebRtc定点版噪音抑制器。
						{
							if( WebRtcNsxPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsx.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 3: //如果要使用WebRtc浮点版噪音抑制器。
						{
							if( WebRtcNsPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNs.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 4: //如果要使用RNNoise噪音抑制器。
						{
							if( RNNoisePocs( MediaPocsThrdPt->m_AdoInpt.m_RNNoise.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用RNNoise噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用RNNoise噪音抑制器失败。" ) );
							}
							break;
						}
					}

					//使用Speex预处理器。
					if( ( MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs == 1 ) || ( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs != 0 ) )
					{
						if( SpeexPrpocsPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt, &MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex预处理器成功。语音活动状态：%d" ), MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts );
							SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex预处理器失败。" ) );
						}
					}

					//判断音频输入是否静音。在音频输入处理完后再设置静音，这样可以保证音频输入处理器的连续性。
					if( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsMute != 0 )
					{
						memset( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, 0, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
						if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs != 0 ) && ( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseVad != 0 ) ) //如果要使用Speex预处理器，且要使用语音活动检测。
						{
							MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 0;
						}
					}

					//使用编码器。
					switch( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd )
					{
						case 0: //如果要使用PCM原始数据。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用PCM原始数据。" ) );
							break;
						}
						case 1: //如果要使用Speex编码器。
						{
							if( SpeexEncdPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, &MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt, &MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex编码器成功。音频输入已编码格式结果帧的长度：%uzd，是否需要传输：%z32d" ), MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex编码器失败。" ) );
							}
							break;
						}
						case 2: //如果要使用Opus编码器。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：暂不支持使用Opus编码器。" ) );
							break;
						}
					}
					
					//使用音频输入原始波形器绘制音频输入原始波形到窗口、音频输入结果波形器绘制音频输入结果波形到窗口。
					if( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWavfmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWavfmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					//使用音频输入原始Wave文件写入器写入音频输入Pcm格式原始帧、音频输入结果Wave文件写入器写入音频输入Pcm格式结果帧。
					if( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_IsSaveAdoToWaveFile != 0 )
					{
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_SrcWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入Pcm格式原始帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入Pcm格式原始帧失败。" ) );
						}
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_WaveFile.m_RsltWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入Pcm格式结果帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入Pcm格式结果帧失败。" ) );
						}
					}

					//使用音视频输入输出Avi文件写入器写入音频输入Pcm格式原始帧、音频输入Pcm格式结果帧。
					if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt != 0 )
					{
						//uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件音频输入帧时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp );
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输入Pcm格式原始帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输入Pcm格式原始帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输入Pcm格式结果帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输入Pcm格式结果帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：本次音频输入帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				} //处理音频输入帧结束。

				//处理音频输出帧开始。
				if( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt != NULL )
				{
					//使用音频输出原始波形器绘制音频输出原始波形到窗口。
					if( MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWavfmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					//使用音频输出原始Wave文件写入器写入音频输出Pcm格式原始帧。
					if( MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_IsSaveAdoToWaveFile != 0 )
					{
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoOtpt.m_WaveFile.m_SrcWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入音频输出Pcm格式原始帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入音频输出Pcm格式原始帧失败。" ) );
						}
					}

					//使用音视频输入输出Avi文件写入器写入音频输出Pcm格式原始帧。
					if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt != 0 )
					{
						//uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件音频输出帧时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp );
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输出Pcm格式原始帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输出Pcm格式原始帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：本次音频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				} //处理音频输出帧结束。

				//处理视频输入帧开始。
				if( MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.m_CQueuePt != NULL ) MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输入帧容器的元素总数。
				else p_TmpSz1 = 0;
				if( ( p_TmpSz1 > 0 ) && //如果视频输入帧容器中有帧了。
					( ( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL ) || ( MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.m_CQueuePt == NULL ) ) ) //且已经处理了音频输入帧或不使用音频输入Pcm格式原始帧容器。
				{
					MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt, NULL, 1, 1, NULL ); //从视频输入帧容器中取出并删除第一个帧。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输入帧容器中取出并删除第一个帧，视频输入帧容器元素总数：%uzd。" ), p_TmpSz1 );
					
					//使用音视频输入输出Avi文件写入器写入视频输入已编码格式结果帧。
					if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt != 0 ) && ( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt != 0 ) )
					{
						//LOGFE( Cu8vstr( "音视频输入输出Avi文件视频输入帧时间戳：%uz64d" ), MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_TimeStampMsec );
						if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_TimeStampMsec, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入视频输入已编码格式结果帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入视频输入已编码格式结果帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：本次视频输入帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				} //处理视频输入帧结束。
				
				//处理视频输出帧开始。
				if( MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.m_CQueuePt != NULL ) MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输出帧容器的元素总数。
				else p_TmpSz1 = 0;
				if( p_TmpSz1 > 0 ) //如果视频输出帧容器中有帧了。
				{
					MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt, NULL, 1, 1, NULL ); //从视频输出帧容器中取出并删除第一个帧。
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输出帧容器中取出并删除第一个帧，视频输出帧容器元素总数：%uzd。" ), p_TmpSz1 );

					//使用音视频输入输出Avi文件写入器写入视频输出已编码格式原始帧。
					if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt != 0 ) && ( MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_EncdSrcFrmLenByt != 0 ) )
					{
						std::map< uint32_t, uint32_t >::iterator p_VdoOtptStrmAviFileIdxMapItrtr;
						uint32_t p_VdoOtptStrmAviFileIdx = UINT32_MAX;

						p_VdoOtptStrmAviFileIdxMapItrtr = MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt->find( MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx );
						if( p_VdoOtptStrmAviFileIdxMapItrtr != MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt->end() )
						{
							p_VdoOtptStrmAviFileIdx = p_VdoOtptStrmAviFileIdxMapItrtr->second;
						}
						else
						{
							if( AviFileWriterAddVdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, '462H', 50, &p_VdoOtptStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //最大采样频率应该尽量被1000整除。
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：音视频输入输出Avi文件添加视频输出已编码格式原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, p_VdoOtptStrmAviFileIdx );
								MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt->insert( std::map< uint32_t, uint32_t >::value_type( MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, p_VdoOtptStrmAviFileIdx ) );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：音视频输入输出Avi文件添加视频输出已编码格式原始流失败。原因：%vs" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}

						if( p_VdoOtptStrmAviFileIdx != UINT32_MAX )
						{
							//LOGFE( Cu8vstr( "音视频输入输出Avi文件视频输出帧时间戳：%uz64d" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_TimeStampMsec );
							if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, p_VdoOtptStrmAviFileIdx, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_TimeStampMsec, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_EncdSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_EncdSrcFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：使用音视频输入输出Avi文件写入器写入视频输出已编码格式原始帧成功。" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：使用音视频输入输出Avi文件写入器写入视频输出已编码格式原始帧失败。原因：%vs" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：本次视频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				} //处理视频输出帧结束。

				//调用用户定义的读取音视频输入帧函数。
				{
					if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL ) //如果有音频输入帧。
					{
						if( MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt != NULL ) //如果有音频输入已编码格式结果帧。
						{
							if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt != NULL ) //如果有视频输入帧。
							{
								if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt != NULL ) //如果有视频输入已编码格式结果帧。
								{
									MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt );
								}
								else //如果没有视频输入已编码格式结果帧。
								{
									MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																					NULL, 0 );
								}
							}
							else //如果没有视频输入帧。
							{
								MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																				MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																				MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt, MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans,
																				NULL, 0, 0, 0,
																				NULL, 0, 0, 0,
																				NULL, 0 );
							}
						}
						else //如果没有音频输入已编码格式结果帧。
						{
							if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt != NULL ) //如果有视频输入帧。
							{
								if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt != NULL ) //如果有视频输入已编码格式结果帧。
								{
									MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																					NULL, 0, 0,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt );
								}
								else //如果没有视频输入已编码格式结果帧。
								{
									MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																					MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																					NULL, 0, 0,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																					NULL, 0 );
								}
							}
							else //如果没有视频输入帧。
							{
								MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																				MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts,
																				NULL, 0, 0,
																				NULL, 0, 0, 0,
																				NULL, 0, 0, 0,
																				NULL, 0 );
							}
						}
					}
					else //如果没有音频输入帧。
					{
						if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt != NULL ) //如果有视频输入帧。
						{
							if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt != NULL ) //如果有视频输入已编码格式结果帧。
							{
								MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																				NULL, NULL, 0, 0,
																				NULL, 0, 0,
																				MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																				MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																				MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt );
							}
							else //如果没有视频输入已编码格式结果帧。
							{
								MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																				NULL, NULL, 0, 0,
																				NULL, 0, 0,
																				MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_BgraSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmLenByt,
																				MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_Yu12RsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleWidth, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_BgraSrcFrmScaleHeight, MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt,
																				NULL, 0 );
							}
						}
						else //如果没有视频输入帧。
						{
							goto OutUserReadAdoVdoInptFrmFunc;
						}
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时：%uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				}
				OutUserReadAdoVdoInptFrmFunc:;

				if( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL ) //如果取出了音频输入Pcm格式原始帧，就追加到音频输入Pcm格式空闲帧容器。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoInpt.m_PcmIdleFrmCntnr.PutTail( &MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, NULL, 1, NULL );
					MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt != NULL ) //如果取出了音频输出Pcm格式原始帧，就追加到音频输出Pcm格式空闲帧容器。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_PcmIdleFrmCntnr.PutTail( &MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, NULL, 1, NULL );
					MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt != NULL ) //如果取出了视频输入帧，就追加到视频输入空闲帧容器。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_VdoInpt.m_IdleFrmCntnr.PutTail( &MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt, NULL, 1, NULL );
					MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt != NULL ) //如果取出了视频输出帧，就追加到视频输出空闲帧容器。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_VdoOtpt.m_IdleFrmCntnr.PutTail( &MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt, NULL, 1, NULL );
					MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt = NULL;
				}

				//如果音频输入设备已经关闭，就切换到默认的音频输入设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsClos != 0 )
				{
					MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_ID = 0; //设置音频输入设备的标识符为默认的音频输入设备。
					LOGFI( Cu8vstr( "媒体处理线程：音频输入设备已经关闭，切换到默认的音频输入设备，并重启媒体处理线程。" ) );
					MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL );
				}

				//如果音频输出设备已经关闭，就切换到默认的音频输出设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_IsClos != 0 )
				{
					MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ID = 0; //设置音频输出设备的标识符为默认的音频输出设备。
					LOGFI( Cu8vstr( "媒体处理线程：音频输出设备已经关闭，切换到默认的音频输出设备，并重启媒体处理线程。" ) );
					MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL );
				}

				//如果视频输入设备已经关闭，就切换到默认的视频输入设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_FilterGraphMediaEventPt != NULL )
				{
					long p_EventCode;
					LONG_PTR p_EventParam1, p_EventParam2;
					MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_FilterGraphMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
					if( p_EventCode == EC_DEVICE_LOST )
					{
						MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_ID = 0; //设置视频输入设备的标识符为默认的视频输入设备。
						LOGFI( Cu8vstr( "媒体处理线程：视频输入设备已经关闭，切换到默认的视频输入设备，并重启媒体处理线程。" ) );
						MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL );
					}
				}

				Sleep( 1 ); //暂停一下，避免CPU使用率过高。
			} //音视频输入输出帧处理结束。

			p_MediaPocsRslt = 0; //设置本媒体消息处理成功。
		}
		OutMediaPocs:;

		if( p_MediaPocsRslt != 0 ) //如果本次媒体处理失败。
		{
			if( p_MediaMsgCntnrElmTotal > 0 ) //如果是媒体消息处理失败。
			{
				if( p_MediaMsg.m_MediaMsgTyp == MediaMsgTypUserInit )
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeUserInit; //设置退出码为调用用户定义的初始化函数失败。
				else if( p_MediaMsg.m_MediaMsgTyp == MediaMsgTypAdoVdoInptOtptInit )
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeAdoVdoInptOtptInit; //设置退出码为音视频输入输出初始化失败。
				else
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeMediaMsgPocs; //设置退出码为媒体消息处理失败。
			}
			else MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs; //设置退出码为音视频输入输出处理失败。
			if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数，就执行销毁。
			{
				//执行顺序：媒体销毁，用户销毁并退出。
				MediaPocsThrdPt->m_ReadyExitCnt++;
				MediaMsgUserDstoy p_MediaMsgUserDstoy;
				p_MediaMsgUserDstoy.m_MediaMsgTyp = MediaMsgTypUserDstoy;
				if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto OutMediaPocs;
				}
				MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
				p_MediaMsgAdoVdoInptOtptDstoy.m_MediaMsgTyp = MediaMsgTypAdoVdoInptOtptDstoy;
				if( MediaPocsThrdPt->m_MediaMsgCntnr.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), NULL, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "放入媒体消息到媒体消息容器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto OutMediaPocs;
				}
			}
		}

		MediaPocsThrdPt->m_MediaMsgCntnr.GetTotal( &p_MediaMsgCntnrElmTotal, 1, NULL );
		if( ( p_MediaMsgCntnrElmTotal == 0 ) && ( MediaPocsThrdPt->m_ReadyExitCnt > 0 ) ) break; //如果媒体消息处理完毕，且媒体处理线程准备退出。
	} //媒体处理循环结束。
	
	MediaPocsThrdTmpVarDstoy( MediaPocsThrdPt );
	
	MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, 0 ); //销毁阻止系统睡眠。

	MediaPocsThrdPt->m_RunFlag = MediaPocsThrd::RunFlagExit; //设置本线程运行标记为已经退出。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：本线程已退出。" ) );

	CoUninitialize(); //销毁COM库。

	return 0;
}

//回调MediaPocsThrdCls类的用户定义的初始化函数。
extern "C" int __cdecl MediaPocsThrdClsUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	return ( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserInit();
}

//回调MediaPocsThrdCls类的用户定义的处理函数。
extern "C" int __cdecl MediaPocsThrdClsUserPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	return ( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserPocs();
}

//回调MediaPocsThrdCls类的用户定义的销毁函数。
extern "C" void __cdecl MediaPocsThrdClsUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserDstoy();
}

//回调MediaPocsThrdCls类的用户定义的消息函数。
extern "C" int __cdecl MediaPocsThrdClsUserMsg( MediaPocsThrd * MediaPocsThrdPt, void * MsgArgPt )
{
	return ( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserMsg( MsgArgPt );
}

//回调MediaPocsThrdCls类的用户定义的读取音视频输入帧函数。
extern "C" void MediaPocsThrdClsUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt,
													   int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
													   uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
													   uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
													   uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
													   uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserReadAdoVdoInptFrm( AdoInptPcmSrcFrmPt, AdoInptPcmRsltFrmPt, AdoInptPcmFrmLenUnit, AdoInptPcmRsltFrmVoiceActSts,
																					AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt, AdoInptEncdRsltFrmIsNeedTrans,
																					VdoInptBgraSrcFrmPt, VdoInptBgraSrcFrmWidth, VdoInptBgraSrcFrmHeight, VdoInptBgraSrcFrmLenByt,
																					VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth, VdoInptYu12RsltFrmHeight, VdoInptYu12RsltFrmLenByt,
																					VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的写入音频输出帧函数。
extern "C" void MediaPocsThrdClsUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
													 int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
													 uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserWriteAdoOtptFrm( AdoOtptStrmIdx, AdoOtptPcmSrcFrmPt, AdoOtptPcmFrmLenUnit, AdoOtptEncdSrcFrmPt, AdoOtptEncdSrcFrmSzByt, AdoOtptEncdSrcFrmLenBytPt );
}

//回调MediaPocsThrdCls类的用户定义的获取音频输出帧函数。
extern "C" void MediaPocsThrdClsUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
												   int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
												   uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserGetAdoOtptFrm( AdoOtptStrmIdx,
																				AdoOtptPcmSrcFrmPt, AdoOtptPcmFrmLenUnit,
																				AdoOtptEncdSrcFrmPt, AdoOtptEncdSrcFrmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的写入视频输出帧函数。
extern "C" void MediaPocsThrdClsUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
													 uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
													 uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserWriteVdoOtptFrm( VdoOtptStrmIdx,
																				  VdoOtptYu12SrcFrmPt, VdoOtptYu12SrcFrmWidthPt, VdoOtptYu12SrcFrmHeightPt,
																				  VdoOtptEncdSrcFrmPt, VdoOtptEncdSrcFrmSzByt, VdoOtptEncdSrcFrmLenBytPt );
}

//回调MediaPocsThrdCls类的用户定义的获取视频输出帧函数。
extern "C" void MediaPocsThrdClsUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
												   uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
												   uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserGetVdoOtptFrm( VdoOtptStrmIdx,
																				VdoOtptYu12SrcFrmPt, VdoOtptYu12SrcFrmWidth, VdoOtptYu12SrcFrmHeight,
																				VdoOtptEncdSrcFrmPt, VdoOtptEncdSrcFrmLenByt );
}

int MediaPocsThrdCls::Init( VstrCls * ErrInfoVstrPt )
{
	return MediaPocsThrdInit( &m_MediaPocsThrdPt, this,
							  MediaPocsThrdClsUserInit, MediaPocsThrdClsUserPocs, MediaPocsThrdClsUserDstoy, MediaPocsThrdClsUserMsg,
							  MediaPocsThrdClsUserReadAdoVdoInptFrm,
							  MediaPocsThrdClsUserWriteAdoOtptFrm, MediaPocsThrdClsUserGetAdoOtptFrm,
							  MediaPocsThrdClsUserWriteVdoOtptFrm, MediaPocsThrdClsUserGetVdoOtptFrm,
							  ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL );
}
