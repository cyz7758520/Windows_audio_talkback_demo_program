#include "MediaPocsThrd.h"

void MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUsePrvntSysSleep );
DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdInit
 * 功能说明：初始化媒体处理线程。
 * 参数说明：MediaPocsThrdPtPt：[输出]，存放媒体处理线程指针的指针，不能为NULL。
			 UserDataPt：[输入]，存放用户数据的指针，可以为NULL。
			 UserInitFuncPt：[输入]，存放用户定义的初始化函数的指针，不能为NULL。
			 UserPocsessFuncPt：[输入]，存放用户定义的处理函数的指针，不能为NULL。
			 UserDstoyFuncPt：[输入]，存放用户定义的销毁函数的指针，不能为NULL。
			 UserMsgFuncPt：[输入]，存放用户定义的消息函数的指针，不能为NULL。
			 UserReadAdoVdoInptFrmFuncPt：[输入]，存放用户定义的读取音视频输入帧函数的指针，不能为NULL。
			 UserWriteAdoOtptFrmFuncPt：[输入]，存放用户定义的写入音频输出帧函数的指针，不能为NULL。
			 UserGetAdoOtptFrmFuncPt：[输入]，存放用户定义的获取音频输出帧函数的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, void * UserDataPt,
					   USER_INIT_FUNC_PT UserInitFuncPt, USER_POCS_FUNC_PT UserPocsFuncPt, USER_DSTOY_FUNC_PT UserDstoyFuncPt, USER_MSG_FUNC_PT UserMsgFuncPt,
					   USER_READ_ADO_VDO_INPT_FRM_FUNC_PT UserReadAdoVdoInptFrmFuncPt,
					   USER_WRITE_ADO_OTPT_FRM_FUNC_PT UserWriteAdoOtptFrmFuncPt, USER_GET_ADO_OTPT_FRM_FUNC_PT UserGetAdoOtptFrmFuncPt,
					   USER_WRITE_VDO_OTPT_FRM_FUNC_PT UserWriteVdoOtptFrmFuncPt, USER_GET_VDO_OTPT_FRM_FUNC_PT UserGetYU12VdoOtptFrmFuncPt,
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
	if( UserGetYU12VdoOtptFrmFuncPt == NULL )
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

	if( p_MediaPocsThrdPt->m_MediaMsgLnkLst.Init( BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化媒体消息链表失败。原因：" ) );
		goto Out;
	}

	//初始化音频输入。
	p_MediaPocsThrdPt->m_AdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输入的媒体处理线程的指针。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化Speex声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入原始文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入结果文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	MediaPocsThrdSetAdoInpt( p_MediaPocsThrdPt, 8000, 20, NULL );

	//初始化音频输出。
	p_MediaPocsThrdPt->m_AdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptStrmLnkLst.Init( sizeof( AdoOtptStrm ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输出流链表失败。原因：" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输出原始文件完整路径动态字符串失败。" ) );
		goto Out;
	}
	MediaPocsThrdSetAdoOtpt( p_MediaPocsThrdPt, 8000, 20, NULL );

	//初始化视频输入。
	p_MediaPocsThrdPt->m_VdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输入的媒体处理线程的指针。
	MediaPocsThrdSetVdoInpt( p_MediaPocsThrdPt, 15, 480, 640, NULL, NULL );
	MediaPocsThrdSetVdoInptUseDvc( p_MediaPocsThrdPt, 0, NULL );

	//初始化视频输出。
	p_MediaPocsThrdPt->m_VdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptStrmLnkLst.Init( sizeof( VdoOtptStrm ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化视频输出流链表失败。原因：" ) );
		goto Out;
	}
	
	//初始化音视频文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音视频文件完整路径动态字符串失败。" ) );
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
	p_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt = UserGetYU12VdoOtptFrmFuncPt; //设置用户定义的获取视频输出帧函数的指针。

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
	
	//销毁音视频文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt = NULL;
	}

	//销毁媒体处理线程信息。
	if( MediaPocsThrdPt->m_MediaPocsThrdInfoPt != NULL )
	{
		ThrdWaitDstoy( MediaPocsThrdPt->m_MediaPocsThrdInfoPt, NULL );
		MediaPocsThrdPt->m_MediaPocsThrdInfoPt = NULL;
	}

	//销毁视频输出流链表。
	if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptStrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptStrmLnkLst.Dstoy( NULL );
	}

	//销毁音频输出原始文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt = NULL;
	}
	
	//销毁音频输出流链表。
	if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptStrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptStrmLnkLst.Dstoy( NULL );
	}

	//销毁音频输入结果文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt = NULL;
	}
	
	//销毁音频输入原始文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt = NULL;
	}

	//销毁WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt = NULL;
	}

	//销毁Speex声学回音消除器的内存块文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt = NULL;
	}
	
	//销毁媒体消息链表。
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.m_VarLenLnkLstPt != NULL )
	{
		MediaPocsThrdPt->m_MediaMsgLnkLst.Dstoy( NULL );
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
	SetAdoInpt,
	SetAdoInptUseNoAec,
	SetAdoInptUseSpeexAec,
	SetAdoInptUseWebRtcAecm,
	SetAdoInptUseWebRtcAec,
	SetAdoInptUseSpeexWebRtcAec,
	SetAdoInptUseNoNs,
	SetAdoInptUseSpeexPrpocsNs,
	SetAdoInptUseWebRtcNsx,
	SetAdoInptUseWebRtcNs,
	SetAdoInptUseRNNoise,
	SetAdoInptIsUseSpeexPrpocsOther,
	SetAdoInptUsePcm,
	SetAdoInptUseSpeexEncd,
	SetAdoInptUseOpusEncd,
	SetAdoInptIsSaveAdoToFile,
	SetAdoInptIsDrawAdoWavfmToWnd,
	SetAdoInptUseDvc,
	SetAdoInptIsMute,

	SetAdoOtpt,
	AddAdoOtptStrm,
	DelAdoOtptStrm,
	SetAdoOtptStrmUsePcm,
	SetAdoOtptStrmUseSpeexDecd,
	SetAdoOtptStrmUseOpusDecd,
	SetAdoOtptStrmIsUse,
	SetAdoOtptIsSaveAdoToFile,
	SetAdoOtptIsDrawAdoWavfmToWnd,
	SetAdoOtptUseDvc,
	SetAdoOtptIsMute,

	SetVdoInpt,
	SetVdoInptUseYU12,
	SetVdoInptUseOpenH264Encd,
	SetVdoInptUseDvc,
	SetVdoInptIsBlack,

	AddVdoOtptStrm,
	DelVdoOtptStrm,
	SetVdoOtptStrm,
	SetVdoOtptStrmUseYU12,
	SetVdoOtptStrmUseOpenH264Decd,
	SetVdoOtptStrmIsBlack,
	SetVdoOtptStrmIsUse,

	SetIsUseAdoVdoInptOtpt,
	
	SetIsUsePrvntSysSleep,
	SetIsSaveAdoVdoInptOtptToFile,
	SaveStngToFile,

	RqirExit,

	UserInit,
	UserDstoy,
	UserMsg,

	AdoVdoInptOtptInit,
	AdoVdoInptOtptDstoy,
} MsgTyp;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_SmplRate;
	int32_t m_FrmLenMsec;
} MediaMsgSetAdoInpt;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetAdoInptUseNoAec;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_FilterLen;
	int32_t m_IsUseRec;
	float m_EchoMutp;
	float m_EchoCntu;
	int32_t m_EchoSupes;
	int32_t m_EchoSupesAct;
	int32_t m_IsSaveMemFile;
	Vstr * m_MemFileFullPathVstrPt;
}MediaMsgSetAdoInptUseSpeexAec;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsUseCNGMode;
	int32_t m_EchoMode;
	int32_t m_Delay;
} MediaMsgSetAdoInptUseWebRtcAecm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_EchoMode;
	int32_t m_Delay;
	int32_t m_IsUseDelayAgstcMode;
	int32_t m_IsUseExtdFilterMode;
	int32_t m_IsUseRefinedFilterAdaptAecMode;
	int32_t m_IsUseAdaptAdjDelay;
	int32_t m_IsSaveMemFile;
	Vstr * m_MemFileFullPathVstrPt;
} MediaMsgSetAdoInptUseWebRtcAec;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_WorkMode;
	int32_t m_SpeexAecFilterLen;
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
} MediaMsgSetAdoInptUseSpeexWebRtcAec;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetAdoInptUseNoNs;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsUseNs;
	int32_t m_NoiseSupes;
	int32_t m_IsUseDereverb;
} MediaMsgSetAdoInptUseSpeexPrpocsNs;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_PolicyMode;
} MediaMsgSetAdoInptUseWebRtcNsx;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_PolicyMode;
} MediaMsgSetAdoInptUseWebRtcNs;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetAdoInptUseRNNoise;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsUseOther;
	int32_t m_IsUseVad;
	int32_t m_VadProbStart;
	int32_t m_VadProbCntu;
	int32_t m_IsUseAgc;
	int32_t m_AgcLevel;
	int32_t m_AgcIncrement;
	int32_t m_AgcDecrement;
	int32_t m_AgcMaxGain;
} MediaMsgSetAdoInptIsUseSpeexPrpocsOther;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetAdoInptUsePcm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_UseCbrOrVbr;
	int32_t m_Qualt;
	int32_t m_Cmplxt;
	int32_t m_PlcExptLossRate;
} MediaMsgSetAdoInptUseSpeexEncd;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetAdoInptUseOpusEncd;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsSaveAdoToFile;
	Vstr * m_AdoInptSrcFileFullPathVstrPt;
	Vstr * m_AdoInptRsltFileFullPathVstrPt;
	size_t m_AdoInptFileWrBufSzByt;
} MediaMsgSetAdoInptIsSaveAdoToFile;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsDrawAdoWavfmToWnd;
	HWND m_AdoInptSrcWavfmWndHdl;
	HWND m_AdoInptRsltWavfmWndHdl;
} MediaMsgSetAdoInptIsDrawAdoWavfmToWnd;
typedef struct
{
	MsgTyp m_MsgTyp;
	UINT m_AdoInptDvcID;
} MediaMsgSetAdoInptUseDvc;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsMute;
} MediaMsgSetAdoInptIsMute;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_SmplRate;
	int32_t m_FrmLenMsec;
} MediaMsgSetAdoOtpt;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
} MediaMsgAddAdoOtptStrm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
} MediaMsgDelAdoOtptStrm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
} MediaMsgSetAdoOtptStrmUsePcm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
	int32_t m_IsUsePrcplEnhsmt;
} MediaMsgSetAdoOtptStrmUseSpeexDecd;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
} MediaMsgSetAdoOtptStrmUseOpusDecd;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_AdoOtptStrmIdx;
	int32_t m_IsUseAdoOtptStrm;
} MediaMsgSetAdoOtptStrmIsUse;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsSaveAdoToFile;
	Vstr * m_AdoOtptSrcFileFullPathVstrPt;
	size_t m_AdoOtptFileWrBufSzByt;
} MediaMsgSetAdoOtptIsSaveAdoToFile;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsDrawAdoWavfmToWnd;
	HWND m_AdoOtptSrcWavfmWndHdl;
} MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd;
typedef struct
{
	MsgTyp m_MsgTyp;
	UINT m_AdoOtptDvcID;
} MediaMsgSetAdoOtptUseDvc;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsMute;
} MediaMsgSetAdoOtptIsMute;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_MaxSmplRate;
	int32_t m_FrmWidth;
	int32_t m_FrmHeight;
	HWND m_VdoInptPrvwWndHdl;
} MediaMsgSetVdoInpt;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgSetVdoInptUseYU12;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoType;
	int32_t m_EncdBitrate;
	int32_t m_BitrateCtrlMode;
	int32_t m_IDRFrmIntvl;
	int32_t m_Cmplxt;
} MediaMsgSetVdoInptUseOpenH264Encd;
typedef struct
{
	MsgTyp m_MsgTyp;
	UINT m_VdoInptDvcID;
} MediaMsgSetVdoInptUseDvc;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsBlack;
} MediaMsgSetVdoInptIsBlack;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
} MediaMsgAddVdoOtptStrm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
} MediaMsgDelVdoOtptStrm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
	HWND m_VdoOtptDspyWndHdl;
} MediaMsgSetVdoOtptStrm;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
} MediaMsgSetVdoOtptStrmUseYU12;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
	int32_t m_DecdThrdNum;
} MediaMsgSetVdoOtptStrmUseOpenH264Decd;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
	int32_t m_IsBlack;
} MediaMsgSetVdoOtptStrmIsBlack;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_VdoOtptStrmIdx;
	int32_t m_IsUseVdoOtptStrm;
} MediaMsgSetVdoOtptStrmIsUse;
typedef struct
{
	MsgTyp m_MsgTyp;
	Vstr * m_AdoVdoInptOtptFileFullPathVstrPt;
	size_t m_AdoVdoInptOtptFileWrBufSzByt;
	int32_t m_IsSaveAdoInpt;
	int32_t m_IsSaveAdoOtpt;
	int32_t m_IsSaveVdoInpt;
	int32_t m_IsSaveVdoOtpt;
} MediaMsgSetIsSaveAdoVdoInptOtptToFile;
typedef struct
{
	MsgTyp m_MsgTyp;
	Vstr * m_StngFileFullPathVstrPt;
} MediaMsgSaveStngToFile;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_IsUseAdoInpt;
	int32_t m_IsUseAdoOtpt;
	int32_t m_IsUseVdoInpt;
	int32_t m_IsUseVdoOtpt;
} MediaMsgSetIsUseAdoVdoInptOtpt;
typedef struct
{
	MsgTyp m_MsgTyp;
	int32_t m_ExitFlag;
	int32_t m_IsBlockWait;
} MediaMsgRqirExit;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgUserInit;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgUserDstoy;
typedef struct
{
	MsgTyp m_MsgTyp;
	void * m_MsgArgPt;
} MediaMsgUserMsg;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgAdoVdoInptOtptInit;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgAdoVdoInptOtptDstoy;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgAdoVdoAviFileWriterInit;
typedef struct
{
	MsgTyp m_MsgTyp;
} MediaMsgAdoVdoAviFileWriterDstoy;
typedef struct
{
	MsgTyp m_MsgTyp;
	uint8_t m_MsgArgArr[ sizeof( MediaMsgSetAdoInptUseSpeexWebRtcAec ) ];
} MediaMsg;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInpt
 * 功能说明：设置音频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt )
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

	p_MediaMsgSetAdoInpt.m_MsgTyp = MsgTyp::SetAdoInpt;
	p_MediaMsgSetAdoInpt.m_SmplRate = SmplRate;
	p_MediaMsgSetAdoInpt.m_FrmLenMsec = FrmLenMsec;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInpt, sizeof( p_MediaMsgSetAdoInpt ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseNoAec p_MediaMsgSetAdoInptUseNoAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInptUseNoAec.m_MsgTyp = MsgTyp::SetAdoInptUseNoAec;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseNoAec, sizeof( p_MediaMsgSetAdoInptUseNoAec ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoInptUseSpeexAec
 * 功能说明：设置音频输入要使用Speex声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 FilterLen：[输入]，存放Speex声学回音消除器滤波器的长度，单位为毫秒。
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseSpeexAec p_MediaMsgSetAdoInptUseSpeexAec;
	p_MediaMsgSetAdoInptUseSpeexAec.m_MemFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "Speex声学回音消除器的内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseSpeexAec.m_MsgTyp = MsgTyp::SetAdoInptUseSpeexAec;
	p_MediaMsgSetAdoInptUseSpeexAec.m_FilterLen = FilterLen;
	p_MediaMsgSetAdoInptUseSpeexAec.m_IsUseRec = IsUseRec;
    p_MediaMsgSetAdoInptUseSpeexAec.m_EchoMutp = EchoMutp;
    p_MediaMsgSetAdoInptUseSpeexAec.m_EchoCntu = EchoCntu;
    p_MediaMsgSetAdoInptUseSpeexAec.m_EchoSupes = EchoSupes;
    p_MediaMsgSetAdoInptUseSpeexAec.m_EchoSupesAct = EchoSupesAct;
	p_MediaMsgSetAdoInptUseSpeexAec.m_IsSaveMemFile = IsSaveMemFile;
	p_MediaMsgSetAdoInptUseSpeexAec.m_MemFileFullPathVstrPt = NULL;
	if( IsSaveMemFile != 0 )
	{
		if( VstrInit( &p_MediaMsgSetAdoInptUseSpeexAec.m_MemFileFullPathVstrPt, Utf8, , MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置Speex声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseSpeexAec, sizeof( p_MediaMsgSetAdoInptUseSpeexAec ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetAdoInptUseSpeexAec.m_MemFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetAdoInptUseSpeexAec.m_MemFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseWebRtcAecm
 * 功能说明：设置音频输入要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
			 EchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
			 Delay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseWebRtcAecm p_MediaMsgSetAdoInptUseWebRtcAecm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseWebRtcAecm.m_MsgTyp = MsgTyp::SetAdoInptUseWebRtcAecm;
	p_MediaMsgSetAdoInptUseWebRtcAecm.m_IsUseCNGMode = IsUseCNGMode;
	p_MediaMsgSetAdoInptUseWebRtcAecm.m_EchoMode = EchoMode;
	p_MediaMsgSetAdoInptUseWebRtcAecm.m_Delay = Delay;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseWebRtcAecm, sizeof( p_MediaMsgSetAdoInptUseWebRtcAecm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoInptUseWebRtcAec
 * 功能说明：设置音频输入要使用WebRtc浮点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             EchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             Delay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseWebRtcAec p_MediaMsgSetAdoInptUseWebRtcAec;
	p_MediaMsgSetAdoInptUseWebRtcAec.m_MemFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveMemFile != 0 ) && ( MemFileFullPathStrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseWebRtcAec.m_MsgTyp = MsgTyp::SetAdoInptUseWebRtcAec;
	p_MediaMsgSetAdoInptUseWebRtcAec.m_EchoMode = EchoMode;
    p_MediaMsgSetAdoInptUseWebRtcAec.m_Delay = Delay;
    p_MediaMsgSetAdoInptUseWebRtcAec.m_IsUseDelayAgstcMode = IsUseDelayAgstcMode;
    p_MediaMsgSetAdoInptUseWebRtcAec.m_IsUseExtdFilterMode = IsUseExtdFilterMode;
    p_MediaMsgSetAdoInptUseWebRtcAec.m_IsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
    p_MediaMsgSetAdoInptUseWebRtcAec.m_IsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	p_MediaMsgSetAdoInptUseWebRtcAec.m_IsSaveMemFile = IsSaveMemFile;
	p_MediaMsgSetAdoInptUseWebRtcAec.m_MemFileFullPathVstrPt = NULL;
	if( IsSaveMemFile != 0 )
	{
		if( VstrInit( &p_MediaMsgSetAdoInptUseWebRtcAec.m_MemFileFullPathVstrPt, Utf8, , MemFileFullPathStrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置WebRtc浮点版声学回音消除器的内存块文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseWebRtcAec, sizeof( p_MediaMsgSetAdoInptUseWebRtcAec ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetAdoInptUseWebRtcAec.m_MemFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetAdoInptUseWebRtcAec.m_MemFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptUseSpeexWebRtcAec
 * 功能说明：设置音频输入要使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             WorkMode：[输入]，存放SpeexWebRtc三重声学回音消除器的工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
             SpeexAecFilterLen：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器滤波器的长度，单位为毫秒。
             SpeexAecEchoMutp：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
             SpeexAecEchoCntu：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音的持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
             SpeexAecEchoSupes：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             SpeexAecEchoSupesAct：[输入]，存放SpeexWebRtc三重声学回音消除器的Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减的分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             WebRtcAecmIsUseCNGMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
             WebRtcAecmEchoMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
             WebRtcAecmDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecEchoMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             WebRtcAecDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecIsUseDelayAgstcMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseExtdFilterMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseRefinedFilterAdaptAecMode：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseAdaptAdjDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器是否使用自适应调节回音的延迟，为非0表示要使用，为0表示不使用。
			 IsUseSameRoomAec：[输入]，存放SpeexWebRtc三重声学回音消除器是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
			 SameRoomEchoMinDelay：[输入]，存放SpeexWebRtc三重声学回音消除器的同一房间回音最小延迟，单位为毫秒，取值区间为[1,2147483647]。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int32_t WorkMode, int32_t SpeexAecFilterLen, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseSpeexWebRtcAec p_MediaMsgSetAdoInptUseSpeexWebRtcAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_MsgTyp = MsgTyp::SetAdoInptUseSpeexWebRtcAec;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WorkMode = WorkMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecFilterLen = SpeexAecFilterLen;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecIsUseRec = SpeexAecIsUseRec;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecEchoMutp = SpeexAecEchoMutp;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecEchoCntu = SpeexAecEchoCntu;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecEchoSupes = SpeexAecEchoSupes;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecmEchoMode = WebRtcAecmEchoMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecmDelay = WebRtcAecmDelay;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecEchoMode = WebRtcAecEchoMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecDelay = WebRtcAecDelay;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode = WebRtcAecIsUseDelayAgstcMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_IsUseSameRoomAec = IsUseSameRoomAec;
	p_MediaMsgSetAdoInptUseSpeexWebRtcAec.m_SameRoomEchoMinDelay = SameRoomEchoMinDelay;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseSpeexWebRtcAec, sizeof( p_MediaMsgSetAdoInptUseSpeexWebRtcAec ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseNoNs p_MediaMsgSetAdoInptUseNoNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInptUseNoNs.m_MsgTyp = MsgTyp::SetAdoInptUseNoNs;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseNoNs, sizeof( p_MediaMsgSetAdoInptUseNoNs ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseSpeexPrpocsNs p_MediaMsgSetAdoInptUseSpeexPrpocsNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseSpeexPrpocsNs.m_MsgTyp = MsgTyp::SetAdoInptUseSpeexPrpocsNs;
	p_MediaMsgSetAdoInptUseSpeexPrpocsNs.m_IsUseNs = IsUseNs;
	p_MediaMsgSetAdoInptUseSpeexPrpocsNs.m_NoiseSupes = NoiseSupes;
    p_MediaMsgSetAdoInptUseSpeexPrpocsNs.m_IsUseDereverb = IsUseDereverb;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseSpeexPrpocsNs, sizeof( p_MediaMsgSetAdoInptUseSpeexPrpocsNs ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseWebRtcNsx p_MediaMsgSetAdoInptUseWebRtcNsx;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInptUseWebRtcNsx.m_MsgTyp = MsgTyp::SetAdoInptUseWebRtcNsx;
	p_MediaMsgSetAdoInptUseWebRtcNsx.m_PolicyMode = PolicyMode;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseWebRtcNsx, sizeof( p_MediaMsgSetAdoInptUseWebRtcNsx ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseWebRtcNs p_MediaMsgSetAdoInptUseWebRtcNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseWebRtcNs.m_MsgTyp = MsgTyp::SetAdoInptUseWebRtcNs;
	p_MediaMsgSetAdoInptUseWebRtcNs.m_PolicyMode = PolicyMode;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseWebRtcNs, sizeof( p_MediaMsgSetAdoInptUseWebRtcNs ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseRNNoise p_MediaMsgSetAdoInptUseRNNoise;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseRNNoise.m_MsgTyp = MsgTyp::SetAdoInptUseRNNoise;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseRNNoise, sizeof( p_MediaMsgSetAdoInptUseRNNoise ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptIsUseSpeexPrpocsOther p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_MsgTyp = MsgTyp::SetAdoInptIsUseSpeexPrpocsOther;
	p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_IsUseOther = IsUseOther;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_IsUseVad = IsUseVad;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_VadProbStart = VadProbStart;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_VadProbCntu = VadProbCntu;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_IsUseAgc = IsUseAgc;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_AgcLevel = AgcLevel;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_AgcIncrement = AgcIncrement;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_AgcDecrement = AgcDecrement;
    p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther.m_AgcMaxGain = AgcMaxGain;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther, sizeof( p_MediaMsgSetAdoInptIsUseSpeexPrpocsOther ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUsePcm p_MediaMsgSetAdoInptUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUsePcm.m_MsgTyp = MsgTyp::SetAdoInptUsePcm;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUsePcm, sizeof( p_MediaMsgSetAdoInptUsePcm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseSpeexEncd p_MediaMsgSetAdoInptUseSpeexEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInptUseSpeexEncd.m_MsgTyp = MsgTyp::SetAdoInptUseSpeexEncd;
	p_MediaMsgSetAdoInptUseSpeexEncd.m_UseCbrOrVbr = UseCbrOrVbr;
    p_MediaMsgSetAdoInptUseSpeexEncd.m_Qualt = Qualt;
    p_MediaMsgSetAdoInptUseSpeexEncd.m_Cmplxt = Cmplxt;
    p_MediaMsgSetAdoInptUseSpeexEncd.m_PlcExptLossRate = PlcExptLossRate;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseSpeexEncd, sizeof( p_MediaMsgSetAdoInptUseSpeexEncd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseOpusEncd p_MediaMsgSetAdoInptUseOpusEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptUseOpusEncd.m_MsgTyp = MsgTyp::SetAdoInptUseOpusEncd;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseOpusEncd, sizeof( p_MediaMsgSetAdoInptUseOpusEncd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoInptIsSaveAdoToFile
 * 功能说明：设置音频输入是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoInptSrcFileFullPathVstrPt：[输入]，存放音频输入原始文件完整路径动态字符串的指针。
			 AdoInptRsltFileFullPathVstrPt：[输入]，存放音频输入结果文件完整路径动态字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoInptSrcFileFullPathVstrPt, const Vstr * AdoInptRsltFileFullPathVstrPt, size_t AdoInptFileWrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptIsSaveAdoToFile p_MediaMsgSetAdoInptIsSaveAdoToFile;
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptSrcFileFullPathVstrPt = NULL;
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptRsltFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoInptSrcFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入原始文件的完整路径字符串不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoInptRsltFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入结果文件的完整路径字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_MsgTyp = MsgTyp::SetAdoInptIsSaveAdoToFile;
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_IsSaveAdoToFile = IsSaveAdoToFile;
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptSrcFileFullPathVstrPt = NULL;
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptRsltFileFullPathVstrPt = NULL;
	if( IsSaveAdoToFile != 0 )
	{
		if( VstrInit( &p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptSrcFileFullPathVstrPt, Utf8, , AdoInptSrcFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频输入原始文件完整路径动态字符串失败。" ) );
			goto Out;
		}
		if( VstrInit( &p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptRsltFileFullPathVstrPt, Utf8, , AdoInptRsltFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频输入结果文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptFileWrBufSzByt = AdoInptFileWrBufSzByt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptIsSaveAdoToFile, sizeof( p_MediaMsgSetAdoInptIsSaveAdoToFile ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptSrcFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptSrcFileFullPathVstrPt );
		if( p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptRsltFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetAdoInptIsSaveAdoToFile.m_AdoInptRsltFileFullPathVstrPt );
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd
 * 功能说明：设置音频输入是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoInptSrcWavfmWndHdl：[输入]，存放音频输入原始波形窗口的句柄。
			 AdoInptRsltWavfmWndHdl：[输入]，存放音频输入结果波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoInptSrcWavfmWndHdl, HWND AdoInptRsltWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptIsDrawAdoWavfmToWnd p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoInptSrcWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入原始波形窗口的句柄不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoInptRsltWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入结果波形窗口的句柄不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_MsgTyp = MsgTyp::SetAdoInptIsDrawAdoWavfmToWnd;
	p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	if( IsDrawAdoWavfmToWnd != 0 )
	{
		p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_AdoInptSrcWavfmWndHdl = AdoInptSrcWavfmWndHdl;
		p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_AdoInptRsltWavfmWndHdl = AdoInptRsltWavfmWndHdl;
	}
	else
	{
		p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_AdoInptSrcWavfmWndHdl = NULL;
		p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd.m_AdoInptRsltWavfmWndHdl = NULL;
	}
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd, sizeof( p_MediaMsgSetAdoInptIsDrawAdoWavfmToWnd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptUseDvc p_MediaMsgSetAdoInptUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	p_MediaMsgSetAdoInptUseDvc.m_MsgTyp = MsgTyp::SetAdoInptUseDvc;
	p_MediaMsgSetAdoInptUseDvc.m_AdoInptDvcID = AdoInptDvcID;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptUseDvc, sizeof( p_MediaMsgSetAdoInptUseDvc ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoInptIsMute p_MediaMsgSetAdoInptIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoInptIsMute.m_MsgTyp = MsgTyp::SetAdoInptIsMute;
	p_MediaMsgSetAdoInptIsMute.m_IsMute = IsMute;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoInptIsMute, sizeof( p_MediaMsgSetAdoInptIsMute ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 功能说明：设置是否使用音频输出。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseAdoOtpt：[输入]，存放是否使用音频输出，为非0表示要使用，为0表示不使用。
			 SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
			 FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int32_t SmplRate, int32_t FrmLenMsec, Vstr * ErrInfoVstrPt )
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
	
	p_MediaMsgSetAdoOtpt.m_MsgTyp = MsgTyp::SetAdoOtpt;
	p_MediaMsgSetAdoOtpt.m_SmplRate = SmplRate;
	p_MediaMsgSetAdoOtpt.m_FrmLenMsec = FrmLenMsec;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtpt, sizeof( p_MediaMsgSetAdoOtpt ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAddAdoOtptStrm
 * 功能说明：添加音频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             AdoOtptStrmIdx：[输入]，存放音频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAddAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAddAdoOtptStrm p_MediaMsgAddAdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAddAdoOtptStrm.m_MsgTyp = MsgTyp::AddAdoOtptStrm;
	p_MediaMsgAddAdoOtptStrm.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgAddAdoOtptStrm, sizeof( p_MediaMsgAddAdoOtptStrm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdDelAdoOtptStrm
 * 功能说明：删除音频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             AdoOtptStrmIdx：[输入]，存放音频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDelAdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgDelAdoOtptStrm p_MediaMsgDelAdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgDelAdoOtptStrm.m_MsgTyp = MsgTyp::DelAdoOtptStrm;
	p_MediaMsgDelAdoOtptStrm.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgDelAdoOtptStrm, sizeof( p_MediaMsgDelAdoOtptStrm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoOtptStrmUsePcm
 * 功能说明：设置音频输出流要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptStrmUsePcm p_MediaMsgSetAdoOtptStrmUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptStrmUsePcm.m_MsgTyp = MsgTyp::SetAdoOtptStrmUsePcm;
	p_MediaMsgSetAdoOtptStrmUsePcm.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptStrmUsePcm, sizeof( p_MediaMsgSetAdoOtptStrmUsePcm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoOtptStrmUseSpeexDecd
 * 功能说明：设置音频输出流要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             IsUsePrcplEnhsmt：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptStrmUseSpeexDecd p_MediaMsgSetAdoOtptStrmUseSpeexDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptStrmUseSpeexDecd.m_MsgTyp = MsgTyp::SetAdoOtptStrmUseSpeexDecd;
	p_MediaMsgSetAdoOtptStrmUseSpeexDecd.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	p_MediaMsgSetAdoOtptStrmUseSpeexDecd.m_IsUsePrcplEnhsmt = IsUsePrcplEnhsmt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptStrmUseSpeexDecd, sizeof( p_MediaMsgSetAdoOtptStrmUseSpeexDecd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoOtptStrmUseOpusDecd
 * 功能说明：设置音频输出流要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptStrmUseOpusDecd p_MediaMsgSetAdoOtptStrmUseOpusDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptStrmUseOpusDecd.m_MsgTyp = MsgTyp::SetAdoOtptStrmUseOpusDecd;
	p_MediaMsgSetAdoOtptStrmUseOpusDecd.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptStrmUseOpusDecd, sizeof( p_MediaMsgSetAdoOtptStrmUseOpusDecd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoOtptStrmIsUse
 * 功能说明：设置音频输出流是否要使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptStrmIsUse p_MediaMsgSetAdoOtptStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptStrmIsUse.m_MsgTyp = MsgTyp::SetAdoOtptStrmIsUse;
	p_MediaMsgSetAdoOtptStrmIsUse.m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	p_MediaMsgSetAdoOtptStrmIsUse.m_IsUseAdoOtptStrm = IsUseAdoOtptStrm;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptStrmIsUse, sizeof( p_MediaMsgSetAdoOtptStrmIsUse ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetAdoOtptIsSaveAdoToFile
 * 功能说明：设置音频输出是否保存音频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 IsSaveAdoToFile：[输入]，存放是否保存音频到文件，非0表示要使用，0表示不使用。
			 AdoOtptSrcFileFullPathVstrPt：[输入]，存放音频输出原始文件完整路径字符串的指针。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptIsSaveAdoToFile( MediaPocsThrd * MediaPocsThrdPt, int32_t IsSaveAdoToFile, const Vstr * AdoOtptSrcFileFullPathVstrPt, size_t AdoOtptFileWrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptIsSaveAdoToFile p_MediaMsgSetAdoOtptIsSaveAdoToFile;
	p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptSrcFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsSaveAdoToFile != 0 ) && ( AdoOtptSrcFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出原始文件的完整路径字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_MsgTyp = MsgTyp::SetAdoOtptIsSaveAdoToFile;
	p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_IsSaveAdoToFile = IsSaveAdoToFile;
	p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptSrcFileFullPathVstrPt = NULL;
	if( IsSaveAdoToFile != 0 )
	{
		if( VstrInit( &p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptSrcFileFullPathVstrPt, Utf8, , AdoOtptSrcFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音频输出原始文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptFileWrBufSzByt = AdoOtptFileWrBufSzByt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptIsSaveAdoToFile, sizeof( p_MediaMsgSetAdoOtptIsSaveAdoToFile ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptSrcFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetAdoOtptIsSaveAdoToFile.m_AdoOtptSrcFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd
 * 功能说明：设置音频输出是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsDrawAdoWavfmToWnd：[输出]，存放是否绘制音频波形到窗口，非0表示要绘制，0表示不绘制。
			 AdoOtptSrcWavfmWndHdl：[输出]，存放音频输出原始波形窗口的句柄。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtptIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int32_t IsDrawAdoWavfmToWnd, HWND AdoOtptSrcWavfmWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( IsDrawAdoWavfmToWnd != 0 ) && ( AdoOtptSrcWavfmWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出原始波形窗口的句柄不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd.m_MsgTyp = MsgTyp::SetAdoOtptIsDrawAdoWavfmToWnd;
	p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd.m_IsDrawAdoWavfmToWnd = IsDrawAdoWavfmToWnd;
	p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd.m_AdoOtptSrcWavfmWndHdl = AdoOtptSrcWavfmWndHdl;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd, sizeof( p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptUseDvc p_MediaMsgSetAdoOtptUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptUseDvc.m_MsgTyp = MsgTyp::SetAdoOtptUseDvc;
	p_MediaMsgSetAdoOtptUseDvc.m_AdoOtptDvcID = AdoOtptDvcID;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptUseDvc, sizeof( p_MediaMsgSetAdoOtptUseDvc ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetAdoOtptIsMute p_MediaMsgSetAdoOtptIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetAdoOtptIsMute.m_MsgTyp = MsgTyp::SetAdoOtptIsMute;
	p_MediaMsgSetAdoOtptIsMute.m_IsMute = IsMute;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetAdoOtptIsMute, sizeof( p_MediaMsgSetAdoOtptIsMute ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 功能说明：设置视频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
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

int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, HWND VdoInptPrvwWnd, Vstr * ErrInfoVstrPt )
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
	
	p_MediaMsgSetVdoInpt.m_MsgTyp = MsgTyp::SetVdoInpt;
	p_MediaMsgSetVdoInpt.m_MaxSmplRate = MaxSmplRate;
    p_MediaMsgSetVdoInpt.m_FrmWidth = FrmWidth;
	p_MediaMsgSetVdoInpt.m_FrmHeight = FrmHeight;
	p_MediaMsgSetVdoInpt.m_VdoInptPrvwWndHdl = VdoInptPrvwWnd;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoInpt, sizeof( p_MediaMsgSetVdoInpt ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseYU12 p_MediaMsgSetVdoInptUseYU12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseYU12.m_MsgTyp = MsgTyp::SetVdoInptUseYU12;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoInptUseYU12, sizeof( p_MediaMsgSetVdoInptUseYU12 ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseOpenH264Encd p_MediaMsgSetVdoInptUseOpenH264Encd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_MsgTyp = MsgTyp::SetVdoInptUseOpenH264Encd;
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_VdoType = VdoType;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_EncdBitrate = EncdBitrate;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_BitrateCtrlMode = BitrateCtrlMode;
    p_MediaMsgSetVdoInptUseOpenH264Encd.m_IDRFrmIntvl = IDRFrmIntvl;
	p_MediaMsgSetVdoInptUseOpenH264Encd.m_Cmplxt = Cmplxt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoInptUseOpenH264Encd, sizeof( p_MediaMsgSetVdoInptUseOpenH264Encd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptUseDvc p_MediaMsgSetVdoInptUseDvc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptUseDvc.m_MsgTyp = MsgTyp::SetVdoInptUseDvc;
	p_MediaMsgSetVdoInptUseDvc.m_VdoInptDvcID = VdoInptDvcID;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoInptUseDvc, sizeof( p_MediaMsgSetVdoInptUseDvc ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoInptIsBlack p_MediaMsgSetVdoInptIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoInptIsBlack.m_MsgTyp = MsgTyp::SetVdoInptIsBlack;
	p_MediaMsgSetVdoInptIsBlack.m_IsBlack = IsBlack;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoInptIsBlack, sizeof( p_MediaMsgSetVdoInptIsBlack ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAddVdoOtptStrm
 * 功能说明：添加视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAddVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgAddVdoOtptStrm p_MediaMsgAddVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgAddVdoOtptStrm.m_MsgTyp = MsgTyp::AddVdoOtptStrm;
	p_MediaMsgAddVdoOtptStrm.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgAddVdoOtptStrm, sizeof( p_MediaMsgAddVdoOtptStrm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdDelVdoOtptStrm
 * 功能说明：删除视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdDelVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgDelVdoOtptStrm p_MediaMsgDelVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgDelVdoOtptStrm.m_MsgTyp = MsgTyp::DelVdoOtptStrm;
	p_MediaMsgDelVdoOtptStrm.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgDelVdoOtptStrm, sizeof( p_MediaMsgDelVdoOtptStrm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrm
 * 功能说明：设置视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVdoOtpt：[输入]，存放是否使用视频输出，为非0表示要使用，为0表示不使用。
			 FrmWidth：[输入]，存放帧的宽度，单位为像素。
			 FrmHeight：[输入]，存放帧的高度，单位为像素。
			 VdoOtptDspyWndHdl：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptStrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, HWND VdoOtptDspyWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrm p_MediaMsgSetVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrm.m_MsgTyp = MsgTyp::SetVdoOtptStrm;
	p_MediaMsgSetVdoOtptStrm.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	p_MediaMsgSetVdoOtptStrm.m_VdoOtptDspyWndHdl = VdoOtptDspyWndHdl;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoOtptStrm, sizeof( p_MediaMsgSetVdoOtptStrm ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrmUseYU12
 * 功能说明：设置视频输出流要使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptStrmUseYU12( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmUseYU12 p_MediaMsgSetVdoOtptStrmUseYU12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmUseYU12.m_MsgTyp = MsgTyp::SetVdoOtptStrmUseYU12;
	p_MediaMsgSetVdoOtptStrmUseYU12.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoOtptStrmUseYU12, sizeof( p_MediaMsgSetVdoOtptStrmUseYU12 ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrmUseOpenH264Decd
 * 功能说明：设置视频输出流要使用OpenH264解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             DecdThrdNum：[输出]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmUseOpenH264Decd p_MediaMsgSetVdoOtptStrmUseOpenH264Decd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_MsgTyp = MsgTyp::SetVdoOtptStrmUseOpenH264Decd;
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	p_MediaMsgSetVdoOtptStrmUseOpenH264Decd.m_DecdThrdNum = DecdThrdNum;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoOtptStrmUseOpenH264Decd, sizeof( p_MediaMsgSetVdoOtptStrmUseOpenH264Decd ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrmIsBlack
 * 功能说明：设置视频输出流是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmIsBlack p_MediaMsgSetVdoOtptStrmIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmIsBlack.m_MsgTyp = MsgTyp::SetVdoOtptStrmIsBlack;
	p_MediaMsgSetVdoOtptStrmIsBlack.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	p_MediaMsgSetVdoOtptStrmIsBlack.m_IsBlack = IsBlack;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoOtptStrmIsBlack, sizeof( p_MediaMsgSetVdoOtptStrmIsBlack ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrmIsUse
 * 功能说明：设置视频输出流是否使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoOtptStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetVdoOtptStrmIsUse p_MediaMsgSetVdoOtptStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetVdoOtptStrmIsUse.m_MsgTyp = MsgTyp::SetVdoOtptStrmIsUse;
	p_MediaMsgSetVdoOtptStrmIsUse.m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	p_MediaMsgSetVdoOtptStrmIsUse.m_IsUseVdoOtptStrm = IsUseVdoOtptStrm;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetVdoOtptStrmIsUse, sizeof( p_MediaMsgSetVdoOtptStrmIsUse ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSetVdoOtptStrmIsUse
 * 功能说明：设置视频输出流是否使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 IsBlack：[输出]，存放视频输出设备帧是否黑屏，为0表示有图像，为非0表示黑屏。
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
	
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_MsgTyp = MsgTyp::SetIsUseAdoVdoInptOtpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoInpt = IsUseAdoInpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoOtpt = IsUseAdoOtpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoInpt = IsUseVdoInpt;
	p_MediaMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoOtpt = IsUseVdoOtpt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetIsUseAdoVdoInptOtpt, sizeof( p_MediaMsgSetIsUseAdoVdoInptOtpt ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 功能说明：设置是否打印Log日志、显示Toast。
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
 * 功能说明：设置是否使用阻止系统睡眠。
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

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}

	MediaPocsThrdPt->m_IsUsePrvntSysSleep = IsUsePrvntSysSleep;

	if( MediaPocsThrdPt->m_RunFlag == MediaPocsThrd::RunFlagRun ) //如果本线程为正在运行，就立即修改唤醒锁。
    {
		MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, IsUsePrvntSysSleep );
    }

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetIsSaveAdoVdoInptOtptToFile
 * 功能说明：设置是否保存音视频到文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			 AdoVdoInptOtptFileFullPathVstrPt：[输入]，存放音视频输入输出Avi文件完整路径字符串的指针。
			 AdoVdoInptOtptFileWrBufSzByt：[输入]，存放音视频输入输出Avi文件写入缓冲区的大小，单位为字节。
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

int MediaPocsThrdSetIsSaveAdoVdoInptOtptToFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * AdoVdoInptOtptFileFullPathVstrPt, size_t AdoVdoInptOtptFileWrBufSzByt, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSetIsSaveAdoVdoInptOtptToFile p_MediaMsgSetIsSaveAdoVdoInptOtptToFile;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( ( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) ) && ( AdoVdoInptOtptFileFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音视频输入输出Avi文件的完整路径字符串不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_MsgTyp = MsgTyp::SetIsSaveAdoVdoInptOtptToFile;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileFullPathVstrPt = NULL;
	if( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) )
	{
		if( VstrInit( &p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileFullPathVstrPt, Utf8, , AdoVdoInptOtptFileFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置音视频输入输出Avi文件完整路径动态字符串失败。" ) );
			goto Out;
		}
	}
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileWrBufSzByt = AdoVdoInptOtptFileWrBufSzByt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_IsSaveAdoInpt = IsSaveAdoInpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_IsSaveAdoOtpt = IsSaveAdoOtpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_IsSaveVdoInpt = IsSaveVdoInpt;
	p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_IsSaveVdoOtpt = IsSaveVdoOtpt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSetIsSaveAdoVdoInptOtptToFile, sizeof( p_MediaMsgSetIsSaveAdoVdoInptOtptToFile ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSetIsSaveAdoVdoInptOtptToFile.m_AdoVdoInptOtptFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSaveStngToFile
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

int MediaPocsThrdSaveStngToFile( MediaPocsThrd * MediaPocsThrdPt, const Vstr * StngFileFullPathVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaMsgSaveStngToFile p_MediaMsgSaveStngToFile;
	p_MediaMsgSaveStngToFile.m_StngFileFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ) );
		goto Out;
	}
	if( StngFileFullPathVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置文件完整路径字符串的指针不正确。" ) );
		goto Out;
	}
	
	p_MediaMsgSaveStngToFile.m_MsgTyp = MsgTyp::SaveStngToFile;
	p_MediaMsgSaveStngToFile.m_StngFileFullPathVstrPt = NULL;
	if( VstrInit( &p_MediaMsgSaveStngToFile.m_StngFileFullPathVstrPt, Utf8, , StngFileFullPathVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置设置文件完整路径字符串失败。" ) );
		goto Out;
	}
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgSaveStngToFile, sizeof( p_MediaMsgSaveStngToFile ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_MediaMsgSaveStngToFile.m_StngFileFullPathVstrPt != NULL ) VstrDstoy( p_MediaMsgSaveStngToFile.m_StngFileFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdPrvntSysSleepInitOrDstoy
 * 功能说明：初始化或销毁阻止系统睡眠。
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
	if( MediaPocsThrdPt->m_MediaPocsThrdInfoPt != NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程已经启动。" ) );
		goto Out;
	}

	if( ThrdInit( &MediaPocsThrdPt->m_MediaPocsThrdInfoPt, ( LPTHREAD_START_ROUTINE )MediaPocsThrdRun, MediaPocsThrdPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
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
	
	p_MediaMsgRqirExit.m_MsgTyp = MsgTyp::RqirExit;
	p_MediaMsgRqirExit.m_ExitFlag = ExitFlag;
	p_MediaMsgRqirExit.m_IsBlockWait = IsBlockWait;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgRqirExit, sizeof( p_MediaMsgRqirExit ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
		goto Out;
	}
	MediaPocsThrdPt->m_ReadyExitCnt++; //设置准备退出计数递增。

	if( IsBlockWait != 0 ) //如果需要阻塞等待。
	{
		if( ExitFlag == 1 ) //如果是请求退出。
		{
			ThrdWaitGetExitCode( MediaPocsThrdPt->m_MediaPocsThrdInfoPt, NULL, NULL ); //等待媒体处理线程退出。
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
 * 功能说明：发送用户消息。
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
	
	p_MediaMsgUserMsg.m_MsgTyp = MsgTyp::UserMsg;
	p_MediaMsgUserMsg.m_MsgArgPt = MsgArgPt;
	if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutTail( &p_MediaMsgUserMsg, sizeof( p_MediaMsgUserMsg ), 1, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAviFileWriterInit
 * 功能说明：初始化媒体处理线程的Avi文件写入器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAviFileWriterInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( MediaPocsThrdPt->m_IsSaveAdoInpt != 0 ) || ( MediaPocsThrdPt->m_IsSaveAdoOtpt != 0 ) || ( MediaPocsThrdPt->m_IsSaveVdoInpt != 0 ) || ( MediaPocsThrdPt->m_IsSaveVdoOtpt != 0 ) )
	{
		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt == NULL )
		{
			if( AviFileWriterInit( &MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt, MediaPocsThrdPt->m_AdoVdoInptOtptFileWrBufSzByt, 5, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音视频输入输出文件 %vs 的Avi文件写入器成功。" ), MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音视频输入输出文件 %vs 的Avi文件写入器失败。原因：%vs" ), MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			uint64_t p_AdoVdoStartTimeStamp = FuncGetTickAsMsec();
			AviFileWriterSetStartTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, p_AdoVdoStartTimeStamp, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频的起始时间戳为 %uz64d 。" ), p_AdoVdoStartTimeStamp );
			MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx = UINT32_MAX;
			MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx = UINT32_MAX;
			MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx = UINT32_MAX;
			MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx = UINT32_MAX;
			MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt = new std::map< uint32_t, uint32_t >;
			if( MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化已编码格式视频输出原始流的Avi文件索引映射失败。原因：内存不足。" ) );
				goto Out;
			}
		}

		if( ( MediaPocsThrdPt->m_IsSaveAdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoInptStrmTimeStampIsReset != 0 ) ) //如果要保存音频输入，且音频输入流时间戳要重置。
		{
			if( MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx == UINT32_MAX )
			{
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 1, &MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输入原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输入原始流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoInpt.m_SmplRate, 1, &MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输入结果流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输入结果流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}

			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx, MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec, NULL );
			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx, MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置PCM格式音频输入原始结果流的当前时间戳为 %uz64d 。" ), MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec );

			MediaPocsThrdPt->m_AdoInptStrmTimeStampIsReset = 0; //设置音频输入流时间戳不重置。
		}

		if( ( MediaPocsThrdPt->m_IsSaveAdoOtpt != 0 ) && ( MediaPocsThrdPt->m_AdoOtptStrmTimeStampIsReset != 0 ) ) //如果要保存音频输出，且音频输出流时间戳要重置。
		{
			if( MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx == UINT32_MAX )
			{
				if( AviFileWriterAddAdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, WAVE_FORMAT_PCM, MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, 1, &MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输出原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加PCM格式音频输出原始流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}

			AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx, MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置PCM格式音频输出原始流的当前时间戳为 %uz64d 。" ), MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec );

			MediaPocsThrdPt->m_AdoOtptStrmTimeStampIsReset = 0; //设置音频输出流时间戳不重置。
		}

		if( ( MediaPocsThrdPt->m_IsSaveVdoInpt != 0 ) && ( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) ) //如果要保存视频输入，且已初始化视频输入。
		{
			if( MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx == UINT32_MAX )
			{
				if( AviFileWriterAddVdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, '462H', 50, &MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //最大采样频率应该尽量被1000整除。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加已编码格式视频输入结果流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音视频输入输出Avi文件添加已编码格式视频输入结果流失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：MediaPocsThrdAviFileWriterDstoy
 * 功能说明：销毁媒体处理线程的Avi文件写入器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdAviFileWriterDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt != NULL )
	{
		if( AviFileWriterDstoy( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音视频输入输出Avi文件写入器成功。" ) );
		}
		else
		{
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音视频输入输出Avi文件写入器失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt = NULL;
		MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx = UINT32_MAX;
		MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx = UINT32_MAX;
		MediaPocsThrdPt->m_AdoInptStrmTimeStampIsReset = 0;
		MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx = UINT32_MAX;
		MediaPocsThrdPt->m_AdoOtptStrmTimeStampIsReset = 0;
		MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec = 0;
		MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx = UINT32_MAX;
		if( MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt != NULL )
		{
			delete MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt;
			MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt = NULL;
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

	if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
	{
		MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt = NULL;
		if( ( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
		{
			free( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt );
			MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt == NULL )
		{
			MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
			if( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建PCM格式音频输入结果帧的内存块失败。" ) );
				goto Out;
			}
		}
		if( ( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
		{
			free( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt );
			MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt == NULL )
		{
			MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt = ( int16_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
			if( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建PCM格式音频临时帧的内存块失败。" ) );
				goto Out;
			}
		}
		MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt = NULL;
		MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts = 1; //设置PCM格式音频输入原始帧的语音活动状态为1，为了让在不使用语音活动检测的情况下永远都是有语音活动。
		if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd != 0 ) //如果音频输入不使用PCM原始数据。
		{
			if( ( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt != NULL ) && ( _msize( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt ) < MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) )
			{
				free( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt );
				MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt = NULL;
			}
			if( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt == NULL )
			{
				MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt = ( uint8_t * )malloc( MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
				if( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt == NULL )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建已编码格式音频结果帧的内存块失败。" ) );
					goto Out;
				}
			}
			MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt = 0;
			MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans = 1; //设置已编码格式音频结果帧是否需要传输为1，为了让在不使用非连续传输的情况下永远都是需要传输。
		}
		else //如果音频输入要使用PCM原始数据。
		{
			if( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt != NULL )
			{
				free( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt );
				MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt = NULL;
			}
			MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt = 0;
			MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans = 0;
		}
	}
	else
	{
		if( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt );
			MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt );
			MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt );
			MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt = NULL;
		}
		if( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt );
			MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt = NULL;
		}
		MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts = 0;
		if( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt != NULL )
		{
			free( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt );
			MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt = NULL;
		}
		MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt = 0;
		MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans = 0;
	}
	MediaPocsThrdPt->m_VdoInptFrmElmPt = NULL;
	MediaPocsThrdPt->m_VdoOtptFrmElmPt = NULL;
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
	if( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL )
	{
		free( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt );
		MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt = NULL;
	}
	if( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt != NULL )
	{
		free( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt );
		MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt = NULL;
	}
	if( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt != NULL )
	{
		free( MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt );
		MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt = NULL;
	}
	if( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt != NULL )
	{
		free( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt );
		MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt = NULL;
	}
	MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts = 0;
	if( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt != NULL )
	{
		free( MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt );
		MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt = NULL;
	}
	MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt = 0;
	MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans = 0;
	MediaPocsThrdPt->m_VdoInptFrmElmPt = NULL;
	MediaPocsThrdPt->m_VdoOtptFrmElmPt = NULL;
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁媒体处理线程的临时变量成功。" ) );
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

	if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) //如果要使用音频输出。在初始化音频输入前初始化音频输出，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
	{
		if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt == 0 ) //如果未初始化音频输出。
		{
			if( AdoOtptInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_AdoOtptStrmTimeStampIsReset = 1; //设置音频输出流时间戳要重置。
			MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
			if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt == 0 ) //如果不使用音频输入。
			{
				MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt = 1; //设置已初始化音频输出。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcClntPt->Start(); //让音频输出设备开始播放。
				MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdIsStart = 1; //设置音频输出线程已开始。
			} //如果要使用音频输入，就不设置已初始化音频输出，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
		}
		else //如果已初始化音频输出。
		{
			if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) //如果要使用音频输入。
			{
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt == 0 ) //如果未初始化音频输入。
				{
					AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt ); //销毁并初始化音频输出设备和线程，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
					if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoOtptStrmTimeStampIsReset = 1; //设置音频输出流时间戳要重置。
					MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
				} //如果音频输入已初始化，表示音频输入输出都已初始化，无需再销毁并初始化。
			}
		}
	}
	else //如果不使用音频输出。
	{
		if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) //如果已初始化音频输出。
		{
			AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt = 0; //设置未初始化音频输出。
		}
	}

	if( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) //如果要使用音频输入。
	{
		if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt == 0 ) //如果未初始化音频输入。
		{
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) //如果要使用音频输出。
            {
                MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt = 1; //设置音频输出已初始化。在这里设置是因为音频输出实际已初始化，音频输出在等待音频输入线程让音频输出设备开始播放和开始音频输出线程。
            }
			AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			if( AdoInptInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out; //在音频输出初始化后再初始化音频输入，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
			MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt = 1; //设置已初始化音频输入。
			MediaPocsThrdPt->m_AdoInptStrmTimeStampIsReset = 1; //设置音频输出流时间戳要重置。
			MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
		}
		else //如果已初始化音频输入。
		{
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) //如果要使用音频输出。
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdIsStart == 0 ) //如果音频输出线程未开始。
				{
					AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt ); //销毁并初始化音频输入设备和线程，因为要音频输入线程让音频输出设备开始播放和开始音频输出线程。
					MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt = 1; //设置音频输出已初始化。在这里设置是因为音频输出实际已初始化，音频输出在等待音频输入线程让音频输出设备开始播放和开始音频输出线程。
					AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
					if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoInptStrmTimeStampIsReset = 1; //设置音频输出流时间戳要重置。
					MediaPocsThrdPt->p_AdoInptOtptStrmCurTimeStampMsec = FuncGetTickAsMsec(); //设置音频输入输出流的当前时间戳。在这里设置是为了防止音视频输入输出初始化时堆积大量帧，导致音视频输入输出Avi文件不同步。
				} //如果音频输出线程已开始，表示音频输入输出都已初始化，无需再销毁并初始化。
			}
			else //如果不使用音频输出。
			{
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
		}
	}
	else //如果不使用音频输入。
	{
		if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) //如果已初始化音频输入。
		{
			AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
			MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt = 0; //设置未初始化音频输入。
			AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
		}
	}

	if( MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt != 0 ) //如果要使用视频输入。
	{
		if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt == 0 ) //如果未初始化视频输入。
		{
			if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt = 1; //设置已初始化视频输入。
		}
	}
	else //如果不使用视频输入。
	{
		if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) //如果已初始化视频输入。
		{
			VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );
			MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt = 0; //设置未初始化视频输入。
		}
	}

	if( MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt != 0 ) //如果要使用视频输出。
	{
		if( MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt == 0 ) //如果未初始化视频输出。
		{
			if( VdoOtptInit( &MediaPocsThrdPt->m_VdoOtpt ) != 0 ) goto Out;
			MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt = 1; //设置已初始化视频输出。
		}
	}
	else //如果不使用视频输出。
	{
		if( MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt != 0 ) //如果已初始化视频输出。
		{
			VdoOtptDstoy( &MediaPocsThrdPt->m_VdoOtpt );
			MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt = 0; //设置未初始化视频输出。
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
	if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) //如果未初始化音频输入。
	{
		AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
		MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt = 0; //设置未初始化音频输入。
		MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
	}

	if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) //如果已初始化音频输出。
	{
		AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
		MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt = 0; //设置未初始化音频输出。
	}

	if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) //如果已初始化视频输入。
	{
		VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );
		MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt = 0; //设置未初始化视频输入。
	}

	if( MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt != 0 ) //如果已初始化视频输出。
	{
		VdoOtptDstoy( &MediaPocsThrdPt->m_VdoOtpt );
		MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt = 0; //设置未初始化视频输出。
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
	int32_t p_TmpInt321;
	int32_t p_TmpInt322;
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;

	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本地代码的指令集名称为%s。" ), ( sizeof( size_t ) == 4 ) ? "x86" : "x64" );

	MediaPocsThrdPt->m_RunFlag = MediaPocsThrd::RunFlagRun; //设置本线程运行标记为正在运行。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：开始准备音视频输入输出帧处理。" ) );

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启，相当于调用用户定义的初始化函数和音视频输入输出初始化函数。

	//媒体处理循环开始。
	while( true )
	{
		int p_MediaPocsRslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
		size_t p_MediaMsgLnkLstElmTotal; //存放媒体消息链表的元素总数。
		MediaMsg p_MediaMsg;

		{
			MediaPocsThrdPt->m_MediaMsgLnkLst.GetTotal( &p_MediaMsgLnkLstElmTotal, 1, NULL );
			if( p_MediaMsgLnkLstElmTotal > 0 ) //如果有媒体消息需要处理。
			{
				{
					MediaPocsThrdPt->m_MediaMsgLnkLst.Locked( NULL );
					MediaPocsThrdPt->m_MediaMsgLnkLst.GetHead( NULL, &p_MediaMsg, sizeof( p_MediaMsg ), NULL, NULL, 0, NULL );
					MediaPocsThrdPt->m_MediaMsgLnkLst.DelHead( 0, NULL );
					MediaPocsThrdPt->m_MediaMsgLnkLst.Unlock( NULL );
				}
				switch( p_MediaMsg.m_MsgTyp )
				{
					case MsgTyp::SetAdoInpt:
					{
						MediaMsgSetAdoInpt * p_MediaMsgSetAdoInptPt = ( MediaMsgSetAdoInpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_SmplRate = p_MediaMsgSetAdoInptPt->m_SmplRate;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec = p_MediaMsgSetAdoInptPt->m_FrmLenMsec;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoInpt.m_SmplRate / 1000;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenData = MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit * 1;
						MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoInpt.m_FrmLenData * sizeof( int16_t );

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
							if( AdoInptInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdTmpVarInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptUseNoAec:
					{
						MediaMsgSetAdoInptUseNoAec * p_MediaMsgSetAdoInptUseNoAecPt = ( MediaMsgSetAdoInptUseNoAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MsgTyp::SetAdoInptUseSpeexAec:
					{
						MediaMsgSetAdoInptUseSpeexAec * p_MediaMsgSetAdoInptUseSpeexAecPt = ( MediaMsgSetAdoInptUseSpeexAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecFilterLen = p_MediaMsgSetAdoInptUseSpeexAecPt->m_FilterLen;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsUseRec = p_MediaMsgSetAdoInptUseSpeexAecPt->m_IsUseRec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoMutp = p_MediaMsgSetAdoInptUseSpeexAecPt->m_EchoMutp;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoCntu = p_MediaMsgSetAdoInptUseSpeexAecPt->m_EchoCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupes = p_MediaMsgSetAdoInptUseSpeexAecPt->m_EchoSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecEchoSupesAct = p_MediaMsgSetAdoInptUseSpeexAecPt->m_EchoSupesAct;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexAecIsSaveMemFile = p_MediaMsgSetAdoInptUseSpeexAecPt->m_IsSaveMemFile;
						if( p_MediaMsgSetAdoInptUseSpeexAecPt->m_MemFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecMemFileFullPathVstrPt, p_MediaMsgSetAdoInptUseSpeexAecPt->m_MemFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetAdoInptUseSpeexAecPt->m_MemFileFullPathVstrPt );
						}

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MsgTyp::SetAdoInptUseWebRtcAecm:
					{
						MediaMsgSetAdoInptUseWebRtcAecm * p_MediaMsgSetAdoInptUseWebRtcAecmPt = ( MediaMsgSetAdoInptUseWebRtcAecm * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 2;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmIsUseCNGMode = p_MediaMsgSetAdoInptUseWebRtcAecmPt->m_IsUseCNGMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmEchoMode = p_MediaMsgSetAdoInptUseWebRtcAecmPt->m_EchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmDelay = p_MediaMsgSetAdoInptUseWebRtcAecmPt->m_Delay;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MsgTyp::SetAdoInptUseWebRtcAec:
					{
						MediaMsgSetAdoInptUseWebRtcAec * p_MediaMsgSetAdoInptUseWebRtcAecPt = ( MediaMsgSetAdoInptUseWebRtcAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 3;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecEchoMode = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_EchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecDelay = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_Delay;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_IsUseDelayAgstcMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseExtdFilterMode = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_IsUseExtdFilterMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_IsUseRefinedFilterAdaptAecMode;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseAdaptAdjDelay = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_IsUseAdaptAdjDelay;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsSaveMemFile = p_MediaMsgSetAdoInptUseWebRtcAecPt->m_IsSaveMemFile;
						if( p_MediaMsgSetAdoInptUseWebRtcAecPt->m_MemFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecMemFileFullPathVstrPt, p_MediaMsgSetAdoInptUseWebRtcAecPt->m_MemFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetAdoInptUseWebRtcAecPt->m_MemFileFullPathVstrPt );
						}

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MsgTyp::SetAdoInptUseSpeexWebRtcAec:
					{
						MediaMsgSetAdoInptUseSpeexWebRtcAec * p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt = ( MediaMsgSetAdoInptUseSpeexWebRtcAec * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 4;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWorkMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WorkMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecFilterLen = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecFilterLen;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecIsUseRec = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecIsUseRec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoMutp = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecEchoMutp;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoCntu = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecEchoCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupes = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecEchoSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSpeexAecEchoSupesAct = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SpeexAecEchoSupesAct;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecmIsUseCNGMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmEchoMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecmEchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecmDelay = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecmDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecEchoMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecEchoMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecDelay = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecIsUseDelayAgstcMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecIsUseExtdFilterMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_WebRtcAecIsUseAdaptAdjDelay;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecIsUseSameRoomAec = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_IsUseSameRoomAec;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecSameRoomEchoMinDelay = p_MediaMsgSetAdoInptUseSpeexWebRtcAecPt->m_SameRoomEchoMinDelay;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
						}
						break;
					}
					case MsgTyp::SetAdoInptUseNoNs:
					{
						MediaMsgSetAdoInptUseNoNs * p_MediaMsgSetAdoInptUseNoNsPt = ( MediaMsgSetAdoInptUseNoNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptUseSpeexPrpocsNs:
					{
						MediaMsgSetAdoInptUseSpeexPrpocsNs * p_MediaMsgSetAdoInptUseSpeexPrpocsNsPt = ( MediaMsgSetAdoInptUseSpeexPrpocsNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseNs = p_MediaMsgSetAdoInptUseSpeexPrpocsNsPt->m_IsUseNs;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNoiseSupes = p_MediaMsgSetAdoInptUseSpeexPrpocsNsPt->m_NoiseSupes;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseDereverb = p_MediaMsgSetAdoInptUseSpeexPrpocsNsPt->m_IsUseDereverb;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptUseWebRtcNsx:
					{
						MediaMsgSetAdoInptUseWebRtcNsx * p_MediaMsgSetAdoInptUseWebRtcNsxPt = ( MediaMsgSetAdoInptUseWebRtcNsx * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 2;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPolicyMode = p_MediaMsgSetAdoInptUseWebRtcNsxPt->m_PolicyMode;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptUseWebRtcNs:
					{
						MediaMsgSetAdoInptUseWebRtcNs * p_MediaMsgSetAdoInptUseWebRtcNsPt = ( MediaMsgSetAdoInptUseWebRtcNs * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 3;
						MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPolicyMode = p_MediaMsgSetAdoInptUseWebRtcNsPt->m_PolicyMode;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptUseRNNoise:
					{
						MediaMsgSetAdoInptUseRNNoise * p_MediaMsgSetAdoInptUseRNNoisePt = ( MediaMsgSetAdoInptUseRNNoise * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 4;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptIsUseSpeexPrpocsOther:
					{
						MediaMsgSetAdoInptIsUseSpeexPrpocsOther * p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt = ( MediaMsgSetAdoInptIsUseSpeexPrpocsOther * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_IsUseOther;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_IsUseVad;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbStart = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_VadProbStart;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsVadProbCntu = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_VadProbCntu;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseAgc = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_IsUseAgc;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcLevel = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_AgcLevel;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcIncrement = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_AgcIncrement;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcDecrement = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_AgcDecrement;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsAgcMaxGain = p_MediaMsgSetAdoInptIsUseSpeexPrpocsOtherPt->m_AgcMaxGain;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts = 1;
						}
						break;
					}
					case MsgTyp::SetAdoInptUsePcm:
					{
						MediaMsgSetAdoInptUsePcm * p_MediaMsgSetAdoInptUsePcmPt = ( MediaMsgSetAdoInptUsePcm * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 0;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MsgTyp::SetAdoInptUseSpeexEncd:
					{
						MediaMsgSetAdoInptUseSpeexEncd * p_MediaMsgSetAdoInptUseSpeexEncdPt = ( MediaMsgSetAdoInptUseSpeexEncd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 1;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdUseCbrOrVbr = p_MediaMsgSetAdoInptUseSpeexEncdPt->m_UseCbrOrVbr;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdQualt = p_MediaMsgSetAdoInptUseSpeexEncdPt->m_Qualt;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdCmplxt = p_MediaMsgSetAdoInptUseSpeexEncdPt->m_Cmplxt;
						MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPlcExptLossRate = p_MediaMsgSetAdoInptUseSpeexEncdPt->m_PlcExptLossRate;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MsgTyp::SetAdoInptUseOpusEncd:
					{
						MediaMsgSetAdoInptUseOpusEncd * p_MediaMsgSetAdoInptUseOpusEncdPt = ( MediaMsgSetAdoInptUseOpusEncd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 2;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
						break;
					}
					case MsgTyp::SetAdoInptIsSaveAdoToFile:
					{
						MediaMsgSetAdoInptIsSaveAdoToFile * p_MediaMsgSetAdoInptIsSaveAdoToFilePt = ( MediaMsgSetAdoInptIsSaveAdoToFile * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile = p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_IsSaveAdoToFile;
						if( p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptSrcFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt, p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptSrcFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptSrcFileFullPathVstrPt );
						}
						if( p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptRsltFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt, p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptRsltFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptRsltFileFullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileWrBufSzByt = p_MediaMsgSetAdoInptIsSaveAdoToFilePt->m_AdoInptFileWrBufSzByt;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) if( AdoInptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetAdoInptIsDrawAdoWavfmToWnd:
					{
						MediaMsgSetAdoInptIsDrawAdoWavfmToWnd * p_MediaMsgSetAdoInptIsDrawAdoWavfmToWndPt = ( MediaMsgSetAdoInptIsDrawAdoWavfmToWnd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptWavfmDstoy( &MediaPocsThrdPt->m_AdoInpt );

						MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd = p_MediaMsgSetAdoInptIsDrawAdoWavfmToWndPt->m_IsDrawAdoWavfmToWnd;
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcWavfmWndHdl = p_MediaMsgSetAdoInptIsDrawAdoWavfmToWndPt->m_AdoInptSrcWavfmWndHdl;
						MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltWavfmWndHdl = p_MediaMsgSetAdoInptIsDrawAdoWavfmToWndPt->m_AdoInptRsltWavfmWndHdl;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) if( AdoInptWavfmInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetAdoInptUseDvc:
					{
						MediaMsgSetAdoInptUseDvc * p_MediaMsgSetAdoInptUseDvcPt = ( MediaMsgSetAdoInptUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID = p_MediaMsgSetAdoInptUseDvcPt->m_AdoInptDvcID;

						if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
						{
							if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetAdoInptIsMute:
					{
						MediaMsgSetAdoInptIsMute * p_MediaMsgSetAdoInptIsMutePt = ( MediaMsgSetAdoInptIsMute * )&p_MediaMsg;

						MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute = p_MediaMsgSetAdoInptIsMutePt->m_IsMute;
						break;
					}
					case MsgTyp::SetAdoOtpt:
					{
						MediaMsgSetAdoOtpt * p_MediaMsgSetAdoOtptPt = ( MediaMsgSetAdoOtpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 )
						{
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoOtpt.m_SmplRate = p_MediaMsgSetAdoOtptPt->m_SmplRate;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec = p_MediaMsgSetAdoOtptPt->m_FrmLenMsec;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit * 1;
						MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData * sizeof( int16_t );

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 )
						{
							if( AdoOtptInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
							{
								AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
								if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							}
							else
							{
								MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcClntPt->Start(); //让音频输出设备开始播放。
								MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdIsStart = 1; //设置音频输出线程已开始。
							}
						}
						break;
					}
					case MsgTyp::AddAdoOtptStrm:
					{
						MediaMsgAddAdoOtptStrm * p_MediaMsgAddAdoOtptStrmPt = ( MediaMsgAddAdoOtptStrm * )&p_MediaMsg;

						AdoOtptAddAdoOtptStrm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgAddAdoOtptStrmPt->m_AdoOtptStrmIdx );
						break;
					}
					case MsgTyp::DelAdoOtptStrm:
					{
						MediaMsgDelAdoOtptStrm * p_MediaMsgDelAdoOtptStrmPt = ( MediaMsgDelAdoOtptStrm * )&p_MediaMsg;

						AdoOtptDelAdoOtptStrm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgDelAdoOtptStrmPt->m_AdoOtptStrmIdx );
						break;
					}
					case MsgTyp::SetAdoOtptStrmUsePcm:
					{
						MediaMsgSetAdoOtptStrmUsePcm * p_MediaMsgSetAdoOtptStrmUsePcmPt = ( MediaMsgSetAdoOtptStrmUsePcm * )&p_MediaMsg;

						AdoOtptSetAdoOtptStrmUsePcm( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgSetAdoOtptStrmUsePcmPt->m_AdoOtptStrmIdx );
						break;
					}
					case MsgTyp::SetAdoOtptStrmUseSpeexDecd:
					{
						MediaMsgSetAdoOtptStrmUseSpeexDecd * p_MediaMsgSetAdoOtptStrmUseSpeexDecdPt = ( MediaMsgSetAdoOtptStrmUseSpeexDecd * )&p_MediaMsg;

						AdoOtptSetAdoOtptStrmUseSpeexDecd( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgSetAdoOtptStrmUseSpeexDecdPt->m_AdoOtptStrmIdx, p_MediaMsgSetAdoOtptStrmUseSpeexDecdPt->m_IsUsePrcplEnhsmt );
						break;
					}
					case MsgTyp::SetAdoOtptStrmUseOpusDecd:
					{
						MediaMsgSetAdoOtptStrmUseOpusDecd * p_MediaMsgSetAdoOtptStrmUseOpusDecdPt = ( MediaMsgSetAdoOtptStrmUseOpusDecd * )&p_MediaMsg;

						AdoOtptSetAdoOtptStrmUseOpusDecd( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgSetAdoOtptStrmUseOpusDecdPt->m_AdoOtptStrmIdx );
						break;
					}
					case MsgTyp::SetAdoOtptStrmIsUse:
					{
						MediaMsgSetAdoOtptStrmIsUse * p_MediaMsgSetAdoOtptStrmIsUsePt = ( MediaMsgSetAdoOtptStrmIsUse * )&p_MediaMsg;

						AdoOtptSetAdoOtptStrmIsUse( &MediaPocsThrdPt->m_AdoOtpt, p_MediaMsgSetAdoOtptStrmIsUsePt->m_AdoOtptStrmIdx, p_MediaMsgSetAdoOtptStrmIsUsePt->m_IsUseAdoOtptStrm );
						break;
					}
					case MsgTyp::SetAdoOtptIsSaveAdoToFile:
					{
						MediaMsgSetAdoOtptIsSaveAdoToFile * p_MediaMsgSetAdoOtptIsSaveAdoToFilePt = ( MediaMsgSetAdoOtptIsSaveAdoToFile * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) AdoOtptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoOtpt );

						MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile = p_MediaMsgSetAdoOtptIsSaveAdoToFilePt->m_IsSaveAdoToFile;
						if( p_MediaMsgSetAdoOtptIsSaveAdoToFilePt->m_AdoOtptSrcFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt, p_MediaMsgSetAdoOtptIsSaveAdoToFilePt->m_AdoOtptSrcFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetAdoOtptIsSaveAdoToFilePt->m_AdoOtptSrcFileFullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileWrBufSzByt = p_MediaMsgSetAdoOtptIsSaveAdoToFilePt->m_AdoOtptFileWrBufSzByt;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) if( AdoOtptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetAdoOtptIsDrawAdoWavfmToWnd:
					{
						MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd * p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWndPt = ( MediaMsgSetAdoOtptIsDrawAdoWavfmToWnd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) AdoOtptWavfmDstoy( &MediaPocsThrdPt->m_AdoOtpt );

						MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd = p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWndPt->m_IsDrawAdoWavfmToWnd;
						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcWavfmWndHdl = p_MediaMsgSetAdoOtptIsDrawAdoWavfmToWndPt->m_AdoOtptSrcWavfmWndHdl;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) if( AdoOtptWavfmInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetAdoOtptUseDvc:
					{
						MediaMsgSetAdoOtptUseDvc * p_MediaMsgSetAdoOtptUseDvcPt = ( MediaMsgSetAdoOtptUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 )
						{
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
							AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
						}

						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID = p_MediaMsgSetAdoOtptUseDvcPt->m_AdoOtptDvcID;

						if( MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 )
						{
							if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt != 0 )
							{
								if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto OutMediaPocs;
							}
							else
							{
								MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcClntPt->Start(); //让音频输出设备开始播放。
								MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdIsStart = 1; //设置音频输出线程已开始。
							}
						}
						break;
					}
					case MsgTyp::SetAdoOtptIsMute:
					{
						MediaMsgSetAdoOtptIsMute * p_MediaMsgSetAdoOtptIsMutePt = ( MediaMsgSetAdoOtptIsMute * )&p_MediaMsg;

						MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIsMute = p_MediaMsgSetAdoOtptIsMutePt->m_IsMute;
						break;
					}
					case MsgTyp::SetVdoInpt:
					{
						MediaMsgSetVdoInpt * p_MediaMsgSetVdoInptPt = ( MediaMsgSetVdoInpt * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate = p_MediaMsgSetVdoInptPt->m_MaxSmplRate;
						MediaPocsThrdPt->m_VdoInpt.m_FrmWidth = p_MediaMsgSetVdoInptPt->m_FrmWidth;
						MediaPocsThrdPt->m_VdoInpt.m_FrmHeight = p_MediaMsgSetVdoInptPt->m_FrmHeight;
						MediaPocsThrdPt->m_VdoInpt.m_YU12FrmLenByt = p_MediaMsgSetVdoInptPt->m_FrmWidth * p_MediaMsgSetVdoInptPt->m_FrmHeight * 3 / 2;
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptPrvwWndHdl = p_MediaMsgSetVdoInptPt->m_VdoInptPrvwWndHdl;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetVdoInptUseYU12:
					{
						MediaMsgSetVdoInptUseYU12 * p_MediaMsgSetVdoInptUseYU12Pt = ( MediaMsgSetVdoInptUseYU12 * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 0;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetVdoInptUseOpenH264Encd:
					{
						MediaMsgSetVdoInptUseOpenH264Encd * p_MediaMsgSetVdoInptUseOpenH264EncdPt = ( MediaMsgSetVdoInptUseOpenH264Encd * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 1;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdVdoType = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_VdoType;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdEncdBitrate = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_EncdBitrate;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdBitrateCtrlMode = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_BitrateCtrlMode;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdIDRFrmIntvl = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_IDRFrmIntvl;
						MediaPocsThrdPt->m_VdoInpt.m_OpenH264EncdCmplxt = p_MediaMsgSetVdoInptUseOpenH264EncdPt->m_Cmplxt;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetVdoInptUseDvc:
					{
						MediaMsgSetVdoInptUseDvc * p_MediaMsgSetVdoInptUseDvcPt = ( MediaMsgSetVdoInptUseDvc * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID = p_MediaMsgSetVdoInptUseDvcPt->m_VdoInptDvcID;

						if( MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SetVdoInptIsBlack:
					{
						MediaMsgSetVdoInptIsBlack * p_MediaMsgSetVdoInptIsBlackPt = ( MediaMsgSetVdoInptIsBlack * )&p_MediaMsg;

						MediaPocsThrdPt->m_VdoInpt.m_VdoInptIsBlack = p_MediaMsgSetVdoInptIsBlackPt->m_IsBlack;
						break;
					}
					case MsgTyp::AddVdoOtptStrm:
					{
						MediaMsgAddVdoOtptStrm * p_MediaMsgAddVdoOtptStrmPt = ( MediaMsgAddVdoOtptStrm * )&p_MediaMsg;

						VdoOtptAddVdoOtptStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgAddVdoOtptStrmPt->m_VdoOtptStrmIdx );
						break;
					}
					case MsgTyp::DelVdoOtptStrm:
					{
						MediaMsgDelVdoOtptStrm * p_MediaMsgDelVdoOtptStrmPt = ( MediaMsgDelVdoOtptStrm * )&p_MediaMsg;

						VdoOtptDelVdoOtptStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgDelVdoOtptStrmPt->m_VdoOtptStrmIdx );
						break;
					}
					case MsgTyp::SetVdoOtptStrm:
					{
						MediaMsgSetVdoOtptStrm * p_MediaMsgSetVdoOtptStrmPt = ( MediaMsgSetVdoOtptStrm * )&p_MediaMsg;

						VdoOtptSetVdoOtptStrm( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmPt->m_VdoOtptStrmIdx, p_MediaMsgSetVdoOtptStrmPt->m_VdoOtptDspyWndHdl );
						break;
					}
					case MsgTyp::SetVdoOtptStrmUseYU12:
					{
						MediaMsgSetVdoOtptStrmUseYU12 * p_MediaMsgSetVdoOtptStrmUseYU12Pt = ( MediaMsgSetVdoOtptStrmUseYU12 * )&p_MediaMsg;

						VdoOtptSetVdoOtptStrmUseYU12( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmUseYU12Pt->m_VdoOtptStrmIdx );
						break;
					}
					case MsgTyp::SetVdoOtptStrmUseOpenH264Decd:
					{
						MediaMsgSetVdoOtptStrmUseOpenH264Decd * p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt = ( MediaMsgSetVdoOtptStrmUseOpenH264Decd * )&p_MediaMsg;

						VdoOtptSetVdoOtptStrmUseOpenH264Decd( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt->m_VdoOtptStrmIdx, p_MediaMsgSetVdoOtptStrmUseOpenH264DecdPt->m_DecdThrdNum );
						break;
					}
					case MsgTyp::SetVdoOtptStrmIsBlack:
					{
						MediaMsgSetVdoOtptStrmIsBlack * p_MediaMsgSetVdoOtptStrmIsBlackPt = ( MediaMsgSetVdoOtptStrmIsBlack * )&p_MediaMsg;

						VdoOtptSetVdoOtptStrmIsBlack( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmIsBlackPt->m_VdoOtptStrmIdx, p_MediaMsgSetVdoOtptStrmIsBlackPt->m_IsBlack );
						break;
					}
					case MsgTyp::SetVdoOtptStrmIsUse:
					{
						MediaMsgSetVdoOtptStrmIsUse * p_MediaMsgSetVdoOtptStrmIsUsePt = ( MediaMsgSetVdoOtptStrmIsUse * )&p_MediaMsg;

						VdoOtptSetVdoOtptStrmIsUse( &MediaPocsThrdPt->m_VdoOtpt, p_MediaMsgSetVdoOtptStrmIsUsePt->m_VdoOtptStrmIdx, p_MediaMsgSetVdoOtptStrmIsUsePt->m_IsUseVdoOtptStrm );
						break;
					}
					case MsgTyp::SetIsUseAdoVdoInptOtpt:
					{
						MediaMsgSetIsUseAdoVdoInptOtpt * p_MediaMsgSetIsUseAdoVdoInptOtptPt = ( MediaMsgSetIsUseAdoVdoInptOtpt * )&p_MediaMsg;

						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt >= 0 ) MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt >= 0 ) MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt >= 0 ) MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt;
						if( p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt >= 0 ) MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = p_MediaMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt;

						MediaMsgAdoVdoInptOtptInit p_MediaMsgSetIsUseAdoVdoInptOtpt;
						p_MediaMsgSetIsUseAdoVdoInptOtpt.m_MsgTyp = MsgTyp::AdoVdoInptOtptInit;
						if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgSetIsUseAdoVdoInptOtpt, sizeof( p_MediaMsgSetIsUseAdoVdoInptOtpt ), 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
							goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::SetIsSaveAdoVdoInptOtptToFile:
					{
						MediaMsgSetIsSaveAdoVdoInptOtptToFile * p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt = ( MediaMsgSetIsSaveAdoVdoInptOtptToFile * )&p_MediaMsg;
						MediaPocsThrdAviFileWriterDstoy( MediaPocsThrdPt );

						if( p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_AdoVdoInptOtptFileFullPathVstrPt != NULL )
						{
							VstrCpy( MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt, p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_AdoVdoInptOtptFileFullPathVstrPt );
							VstrDstoy( p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_AdoVdoInptOtptFileFullPathVstrPt );
						}
						MediaPocsThrdPt->m_AdoVdoInptOtptFileWrBufSzByt = p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_AdoVdoInptOtptFileWrBufSzByt;
						MediaPocsThrdPt->m_IsSaveAdoInpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_IsSaveAdoInpt;
						MediaPocsThrdPt->m_IsSaveAdoOtpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_IsSaveAdoOtpt;
						MediaPocsThrdPt->m_IsSaveVdoInpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_IsSaveVdoInpt;
						MediaPocsThrdPt->m_IsSaveVdoOtpt = p_MediaMsgSetIsSaveAdoVdoInptOtptToFilePt->m_IsSaveVdoOtpt;

						if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) if( MediaPocsThrdAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						break;
					}
					case MsgTyp::SaveStngToFile:
					{
						MediaMsgSaveStngToFile * p_MediaMsgSaveStngToFilePt = ( MediaMsgSaveStngToFile * )&p_MediaMsg;
						File * p_StngFilePt;

						if( FileInitByPath( &p_StngFilePt, p_MediaMsgSaveStngToFilePt->m_StngFileFullPathVstrPt, NoRd_Wr, Create_AlExist_Clr, 4096, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化设置文件 %vs 失败。原因：%vs" ), p_MediaMsgSaveStngToFilePt->m_StngFileFullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
							VstrDstoy( p_MediaMsgSaveStngToFilePt->m_StngFileFullPathVstrPt );
							goto OutMediaPocs;
						}

						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsPrintLog：%z32d\n" ), MediaPocsThrdPt->m_IsPrintLog );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsShowToast：%z32d\n" ), MediaPocsThrdPt->m_IsShowToast );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_ShowToastWndHdl：%zd\n" ), MediaPocsThrdPt->m_ShowToastWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsUsePrvntSysSleep：%zd\n" ), MediaPocsThrdPt->m_IsUsePrvntSysSleep );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInptRsltFileFullPathVstrPt：%uzd\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptFileWrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsSaveAdoInpt：%z64d\n" ), MediaPocsThrdPt->m_IsSaveAdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsSaveAdoOtpt：%z64d\n" ), MediaPocsThrdPt->m_IsSaveAdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsSaveVdoInpt：%z64d\n" ), MediaPocsThrdPt->m_IsSaveVdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_IsSaveVdoOtpt：%z64d\n" ), MediaPocsThrdPt->m_IsSaveVdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsUseAdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsInitAdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsInitAdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenUnit：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenByt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
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
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptFileWrBufSzByt：%zd\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptFileWrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptSrcWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptRsltWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptDvcID：%ud\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_AdoInptIsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsUseAdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsInitAdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_SmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenUnit：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenByt：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						{
							AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
							for( size_t p_AdoOtptStrmInfoIdx = SIZE_MAX; MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmInfoIdx, &p_AdoOtptStrmInfoIdx, NULL, ( void ** )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
							{
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptStrmIdx：%z32d\n" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_UseWhatDecd：%z32d\n" ), p_AdoOtptStrmPt->m_UseWhatDecd );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SpeexDecdIsUsePrcplEnhsmt：%z32d\n" ), p_AdoOtptStrmPt->m_SpeexDecdIsUsePrcplEnhsmt );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
							}
						}
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsSaveAdoToFile：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcFileFullPathVstrPt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptFileWrBufSzByt：%zd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptFileWrBufSzByt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsDrawAdoWavfmToWnd：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptSrcWavfmWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcWavfmWndHdl );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptDvcID：%ud\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_AdoOtptIsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptIsMute );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );

						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsUseVdoInpt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsInitVdoInpt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsInitVdoInpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_MaxSmplRate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmWidth：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmWidth );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmHeight：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmHeight );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmLenByt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_YU12FrmLenByt );
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
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsInitVdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt );
						FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
						{
							VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
							for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void ** )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
							{
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptStrmIdx：%z32d\n" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_UseWhatDecd：%z32d\n" ), p_VdoOtptStrmPt->m_UseWhatDecd );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_OpenH264DecdDecdThrdNum：%z32d\n" ), p_VdoOtptStrmPt->m_OpenH264DecdDecdThrdNum );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptDspyWndHdl：%#P\n" ), p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_VdoOtptIsBlack：%z32d\n" ), p_VdoOtptStrmPt->m_VdoOtptIsBlack );
								FileFmtWrite( p_StngFilePt, 0, NULL, Cu8vstr( "\n" ) );
							}
						}

						FileDstoy( p_StngFilePt, 1, NULL );
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：保存设置到文件 %vs 成功。" ), p_MediaMsgSaveStngToFilePt->m_StngFileFullPathVstrPt );
						VstrDstoy( p_MediaMsgSaveStngToFilePt->m_StngFileFullPathVstrPt );
						break;
					}
					case MsgTyp::RqirExit:
					{
						MediaMsgRqirExit * p_MediaMsgRqirExitPt = ( MediaMsgRqirExit * )&p_MediaMsg;
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：接收到退出请求，开始准备退出。" ) );

						switch( p_MediaMsgRqirExitPt->m_ExitFlag )
						{
							case 1: //为请求退出。
							{
								//执行顺序：媒体销毁，用户销毁并退出。
								MediaPocsThrdPt->m_MediaMsgLnkLst.Locked( NULL );
								if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
								{
									MediaMsgUserDstoy p_MediaMsgUserDstoy;
									p_MediaMsgUserDstoy.m_MsgTyp = MsgTyp::UserDstoy;
									if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
										goto OutMediaPocs;
									}
									MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
									p_MediaMsgAdoVdoInptOtptDstoy.m_MsgTyp = MsgTyp::AdoVdoInptOtptDstoy;
									if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
										goto OutMediaPocs;
									}
								}
								else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
								{
									MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								}
								MediaPocsThrdPt->m_MediaMsgLnkLst.Unlock( NULL );
								break;
							}
							case 2: //请求重启。
							{
								//执行顺序：媒体销毁，用户销毁，用户初始化，媒体初始化。
								MediaPocsThrdPt->m_MediaMsgLnkLst.Locked( NULL );
								MediaMsgAdoVdoInptOtptInit p_MediaMsgAdoVdoInptOtptInit;
								p_MediaMsgAdoVdoInptOtptInit.m_MsgTyp = MsgTyp::AdoVdoInptOtptInit;
								if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptInit, sizeof( p_MediaMsgAdoVdoInptOtptInit ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
									goto OutMediaPocs;
								}
								MediaMsgUserInit p_MediaMsgUserInit;
								p_MediaMsgUserInit.m_MsgTyp = MsgTyp::UserInit;
								if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgUserInit, sizeof( p_MediaMsgUserInit ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
									goto OutMediaPocs;
								}
								if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
								{
									MediaMsgUserDstoy p_MediaMsgUserDstoy;
									p_MediaMsgUserDstoy.m_MsgTyp = MsgTyp::UserDstoy;
									if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
										goto OutMediaPocs;
									}
									MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
									p_MediaMsgAdoVdoInptOtptDstoy.m_MsgTyp = MsgTyp::AdoVdoInptOtptDstoy;
									if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
									{
										VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
										goto OutMediaPocs;
									}
									MediaPocsThrdPt->m_MediaMsgLnkLst.Locked( NULL );
								}
								else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
								{
									MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								}
								MediaPocsThrdPt->m_MediaMsgLnkLst.Unlock( NULL );
								break;
							}
							case 3: //请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
							{
								//执行顺序：媒体销毁，媒体初始化。
								MediaPocsThrdPt->m_MediaMsgLnkLst.Locked( NULL );
								MediaMsgAdoVdoInptOtptInit p_MediaMsgAdoVdoInptOtptInit;
								p_MediaMsgAdoVdoInptOtptInit.m_MsgTyp = MsgTyp::AdoVdoInptOtptInit;
								if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptInit, sizeof( p_MediaMsgAdoVdoInptOtptInit ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
									goto OutMediaPocs;
								}
								MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
								p_MediaMsgAdoVdoInptOtptDstoy.m_MsgTyp = MsgTyp::AdoVdoInptOtptDstoy;
								if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
									goto OutMediaPocs;
								}
								MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
								MediaPocsThrdPt->m_MediaMsgLnkLst.Unlock( NULL );
								break;
							}
						}
						break;
					}
					case MsgTyp::UserInit:
					{
						MediaMsgUserInit * p_MediaMsgUserInitPt = ( MediaMsgUserInit * )&p_MediaMsg;

						MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeNormal; //清空退出码。
						MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 0; //设置上一次调用了用户定义的初始化函数。
						MediaPocsThrdPt->m_ReadyExitCnt--; //设置准备退出计数递减。因为在请求退出时递增了。
						p_TmpInt321 = MediaPocsThrdPt->m_UserInitFuncPt( MediaPocsThrdPt ); //调用用户定义的初始化函数。
						if( p_TmpInt321 == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数成功。返回值：%d。" ), p_TmpInt321 );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数失败。返回值：%d。" ), p_TmpInt321 );
							goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::UserDstoy:
					{
						MediaMsgUserDstoy * p_MediaMsgUserDstoyPt = ( MediaMsgUserDstoy * )&p_MediaMsg;

						MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 1;
						MediaPocsThrdPt->m_UserDstoyFuncPt( MediaPocsThrdPt ); //调用用户定义的销毁函数。
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的销毁函数成功。" ) );
						break;
					}
					case MsgTyp::UserMsg:
					{
						MediaMsgUserMsg * p_MediaMsgUserMsgPt = ( MediaMsgUserMsg * )&p_MediaMsg;

						p_TmpInt321 = MediaPocsThrdPt->m_UserMsgFuncPt( MediaPocsThrdPt, p_MediaMsgUserMsgPt->m_MsgArgPt );
						if( p_TmpInt321 == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的消息函数成功。返回值：%d。" ), p_TmpInt321 );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的消息函数失败。返回值：%d。" ), p_TmpInt321 );
							goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::AdoVdoInptOtptInit:
					{
						if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数，就初始化音视频输入输出、音视频输入输出Avi文件写入器。
						{
							if( MediaPocsThrdAdoVdoInptOtptInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
							if( MediaPocsThrdAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto OutMediaPocs;
						}
						break;
					}
					case MsgTyp::AdoVdoInptOtptDstoy:
					{
						MediaPocsThrdAviFileWriterDstoy( MediaPocsThrdPt );
						MediaPocsThrdAdoVdoInptOtptDstoy( MediaPocsThrdPt );
						break;
					}
				}
			}
			else //如果没有媒体消息需要处理，就音视频输入输出帧处理。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec();

				//调用用户定义的处理函数。
				{
					p_TmpInt321 = MediaPocsThrdPt->m_UserPocsFuncPt( MediaPocsThrdPt );
					if( p_TmpInt321 == 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数成功。返回值：%d" ), p_TmpInt321 );
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的处理函数失败。返回值：%d" ), p_TmpInt321 );
						goto OutMediaPocs;
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				}

				//取出PCM格式音频输入原始帧和PCM格式音频输出原始帧。
				if( MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取PCM格式音频输入原始帧链表的元素总数。
				else p_TmpSz1 = 0;
				if( MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.GetTotal( &p_TmpSz2, 1, NULL ); //获取PCM格式音频输出原始帧链表的元素总数。
				else p_TmpSz2 = 0;
				if( MediaPocsThrdPt->m_AdoInpt.m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器。
				{
					if( ( p_TmpSz1 > 0 ) && ( p_TmpSz2 > 0 ) ) //如果PCM格式音频输入原始帧链表和PCM格式音频输出原始帧链表中都有帧了，就开始取出。
					{
						//从PCM格式音频输入原始帧链表中取出第一个帧。
						{
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.Locked( NULL ); //PCM格式音频输入原始帧链表的互斥锁加锁。
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, NULL, 0, NULL );
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.DelHead( 0, NULL );
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.Unlock( NULL ); //PCM格式音频输入原始帧链表的互斥锁解锁。
						}
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从PCM格式音频输入原始帧链表中取出第一个帧，PCM格式音频输入原始帧链表元素总数：%z32d。" ), p_TmpSz1 );

						//从PCM格式音频输出原始帧链表中取出第一个帧。
						{
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.Locked( NULL ); //PCM格式音频输出原始帧链表的互斥锁加锁。
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, NULL, 0, NULL );
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.DelHead( 0, NULL );
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.Unlock( NULL ); //PCM格式音频输出原始帧链表的互斥锁解锁。
						}
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从PCM格式音频输出原始帧链表中取出第一个帧，PCM格式音频输出原始帧链表元素总数：%z32d。" ), p_TmpSz2 );

						//将PCM格式音频输入原始帧复制到PCM格式音频输入结果帧，方便处理。
						memcpy( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
					}
				}
				else //如果不可以使用声学回音消除器。
				{
					if( p_TmpSz1 > 0 ) //如果PCM格式音频输入原始帧链表有帧了，就开始取出。
					{
						//从PCM格式音频输入原始帧链表中取出第一个帧。
						{
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.Locked( NULL ); //PCM格式音频输入原始帧链表的互斥锁加锁。
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, NULL, 0, NULL );
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.DelHead( 0, NULL );
							MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.Unlock( NULL ); //PCM格式音频输入原始帧链表的互斥锁解锁。
						}
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从PCM格式音频输入原始帧链表中取出第一个帧，PCM格式音频输入原始帧链表元素总数：%z32d。" ), p_TmpSz1 );

						//将PCM格式音频输入原始帧复制到PCM格式音频输入结果帧，方便处理。
						memcpy( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
					}

					if( p_TmpSz2 > 0 ) //如果PCM格式音频输出原始帧链表有帧了，就开始取出。
					{
						//从PCM格式音频输出原始帧链表中取出第一个帧。
						{
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.Locked( NULL ); //PCM格式音频输出原始帧链表的互斥锁加锁。
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, NULL, 0, NULL );
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.DelHead( 0, NULL );
							MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptSrcFrmLnkLst.Unlock( NULL ); //PCM格式音频输出原始帧链表的互斥锁解锁。
						}
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从PCM格式音频输出原始帧链表中取出第一个帧，PCM格式音频输出原始帧链表元素总数：%z32d。" ), p_TmpSz2 );
					}
				}

				//处理音频输入帧开始。
				if( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL )
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
							case 1: //如果使用Speex声学回音消除器。
							{
								if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt != NULL ) && ( SpeexAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexAecPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 ) )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex声学回音消除器失败。" ) );
								}
								break;
							}
							case 2: //如果使用WebRtc定点版声学回音消除器。
							{
								if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 ) )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版声学回音消除器失败。" ) );
								}
								break;
							}
							case 3: //如果使用WebRtc浮点版声学回音消除器。
							{
								if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt != NULL ) && ( WebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 ) )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
								}
								else
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版声学回音消除器失败。" ) );
								}
								break;
							}
							case 4: //如果使用SpeexWebRtc三重声学回音消除器。
							{
								if( ( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 ) )
								{
									if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用SpeexWebRtc三重声学回音消除器成功。" ) );
									SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
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
						case 1: //如果使用Speex预处理器的噪音抑制。
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：在使用Speex预处理器时一起使用噪音抑制。" ) );
							break;
						}
						case 2: //如果使用WebRtc定点版噪音抑制器。
						{
							if( WebRtcNsxPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsxPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc定点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 3: //如果使用WebRtc浮点版噪音抑制器。
						{
							if( WebRtcNsPocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc浮点版噪音抑制器失败。" ) );
							}
							break;
						}
						case 4: //如果使用RNNoise噪音抑制器。
						{
							if( RNNoisePocs( MediaPocsThrdPt->m_AdoInpt.m_RNNoisePt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用RNNoise噪音抑制器成功。" ) );
								SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
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
						if( SpeexPrpocsPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt, &MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex预处理器成功。语音活动状态：%d" ), MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts );
							SWAPPT( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_PcmAdoInptTmpFrmPt ); //交换PCM格式音频输入结果帧和PCM格式音频输入临时帧。
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用Speex预处理器失败。" ) );
						}
					}

					//判断音频输入是否静音。在音频输入处理完后再设置静音，这样可以保证音频输入处理器的连续性。
					if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptIsMute != 0 )
					{
						memset( MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, 0, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
						if( ( MediaPocsThrdPt->m_AdoInpt.m_IsUseSpeexPrpocsOther != 0 ) && ( MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsIsUseVad != 0 ) ) //如果Speex预处理器要使用其他功能，且要使用语音活动检测。
						{
							MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts = 0;
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
							if( SpeexEncdPocs( MediaPocsThrdPt->m_AdoInpt.m_SpeexEncdPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, &MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt, &MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Speex编码器成功。Speex格式音频输入结果帧的长度：%uzd，是否需要传输：%z32d" ), MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans );
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
							break;
						}
					}
					
					//使用音频输入原始波形器绘制音频输入原始波形到窗口、音频输入结果波形器绘制音频输入结果波形到窗口。
					if( MediaPocsThrdPt->m_AdoInpt.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcWavfmPt, MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltWavfmPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					//使用音频输入原始Wave文件写入器写入音频输入原始帧、音频输入结果Wave文件写入器写入音频输入结果帧。
					if( MediaPocsThrdPt->m_AdoInpt.m_IsSaveAdoToFile != 0 )
					{
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_AdoInptSrcWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入原始帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入原始帧失败。" ) );
						}
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_AdoInptRsltWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入结果帧成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入结果帧失败。" ) );
						}
					}

					//使用音视频输入输出Avi文件写入器写入音频输入流到文件。
					if( ( MediaPocsThrdPt->m_IsSaveAdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt != NULL ) )
					{
						//uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音频输入时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp );
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoInptSrcStrmAviFileIdx, MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输入原始流到文件成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输入流到文件失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoInptRsltStrmAviFileIdx, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输入结果流到文件成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输入结果流到文件失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
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
				if( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt != NULL )
				{
					//使用音频输出原始波形器绘制音频输出原始波形到窗口。
					if( MediaPocsThrdPt->m_AdoOtpt.m_IsDrawAdoWavfmToWnd != 0 )
					{
						if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcWavfmPt, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcWavfmWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}

					//使用音频输出原始Wave文件写入器写入输出帧数据。
					if( MediaPocsThrdPt->m_AdoOtpt.m_IsSaveAdoToFile != 0 )
					{
						if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptSrcWaveFileWriterPt, ( char * )MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入输出帧数据成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入输出帧数据失败。" ) );
						}
					}

					//使用音视频输入输出Avi文件写入器写入音频输出流到文件。
					if( ( MediaPocsThrdPt->m_IsSaveAdoOtpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt != NULL ) )
					{
						//uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音频输出时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp );
						if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_PcmAdoOtptSrcStrmAviFileIdx, MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输出原始流到文件成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入PCM格式音频输出原始流到文件失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
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
				if( MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输入帧链表的元素总数。
				else p_TmpSz1 = 0;
				if( ( p_TmpSz1 > 0 ) && //如果视频输入帧链表中有帧了。
					( ( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL ) || ( MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptSrcFrmLnkLst.m_ConstLenLnkLstPt == NULL ) ) ) //且已经处理了音频输入帧或不使用PCM格式音频输入原始帧链表。
				{
					//从视频输入帧链表中取出第一个视频输入帧。
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Locked( NULL ); //视频输入帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VdoInptFrmElmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptFrmLnkLst.Unlock( NULL ); //视频输入帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输入帧链表中取出第一个视频输入帧，视频输入帧链表元素总数：%uzd。" ), p_TmpSz1 );
					
					//使用音视频输入输出Avi文件写入器写入视频输入流到文件。
					if( ( MediaPocsThrdPt->m_IsSaveVdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt != NULL ) && ( MediaPocsThrdPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmLenByt != SIZE_MAX ) )
					{
						//LOGFE( Cu8vstr( "视频输入时间戳：%uz64d" ), MediaPocsThrdPt->m_VdoInptFrmElmPt->m_TimeStampMsec );
						if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, MediaPocsThrdPt->m_EncdVdoInptRsltStrmAviFileIdx, MediaPocsThrdPt->m_VdoInptFrmElmPt->m_TimeStampMsec, MediaPocsThrdPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt, MediaPocsThrdPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入已编码格式视频输入结果流到文件成功。" ) );
						}
						else
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入已编码格式视频输入结果流到文件失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
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
				if( MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.m_ConstLenLnkLstPt != NULL ) MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输出帧链表的元素总数。
				else p_TmpSz1 = 0;
				if( p_TmpSz1 > 0 ) //如果视频输出帧链表中有帧了。
				{
					//从视频输出帧链表中取出第一个视频输出帧。
					{
						MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.Locked( NULL ); //视频输出帧链表的互斥锁加锁。
						MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.GetHead( NULL, &MediaPocsThrdPt->m_VdoOtptFrmElmPt, NULL, 0, NULL );
						MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.DelHead( 0, NULL );
						MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptFrmLnkLst.Unlock( NULL ); //视频输出帧链表的互斥锁解锁。
					}
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输出帧链表中取出第一个视频输出帧，视频输出帧链表元素总数：%uzd。" ), p_TmpSz1 );

					//使用音视频输入输出Avi文件写入器写入视频输出流到文件。
					if( ( MediaPocsThrdPt->m_IsSaveVdoOtpt != 0 ) && ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt != NULL ) )
					{
						std::map< uint32_t, uint32_t >::iterator p_VdoOtptStrmAviFileIdxMapItrtr;
						uint32_t p_VdoOtptStrmAviFileIdx = UINT32_MAX;

						p_VdoOtptStrmAviFileIdxMapItrtr = MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt->find( MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx );
						if( p_VdoOtptStrmAviFileIdxMapItrtr != MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt->end() )
						{
							p_VdoOtptStrmAviFileIdx = p_VdoOtptStrmAviFileIdxMapItrtr->second;
						}
						else
						{
							if( AviFileWriterAddVdoStrm( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, '462H', 50, &p_VdoOtptStrmAviFileIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //最大采样频率应该尽量被1000整除。
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：音视频输入输出Avi文件添加已编码格式视频输出原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmAviFileIdx );
								MediaPocsThrdPt->m_EncdVdoOtptSrcStrmAviFileIdxMapPt->insert( std::map< uint32_t, uint32_t >::value_type( MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmAviFileIdx ) );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：音视频输入输出Avi文件添加已编码格式视频输出原始流失败。原因：%vs" ), MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}

						if( ( p_VdoOtptStrmAviFileIdx != UINT32_MAX ) && ( MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt != NULL ) )
						{
							//LOGFE( Cu8vstr( "视频输出时间戳：%uz64d" ), MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_TimeStampMsec );
							if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFileWriterPt, p_VdoOtptStrmAviFileIdx, MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_TimeStampMsec, MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt, MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：使用音视频输入输出Avi文件写入器写入已编码格式视频输出原始流到文件成功。" ), MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx );
							}
							else
							{
								if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：使用音视频输入输出Avi文件写入器写入已编码格式视频输出原始流到文件失败。原因：%vs" ), MediaPocsThrdPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
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
				if( ( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL ) || ( MediaPocsThrdPt->m_VdoInptFrmElmPt != NULL ) ) //如果取出了音频输入帧或视频输入帧。
				{
					if( MediaPocsThrdPt->m_VdoInptFrmElmPt != NULL ) //如果取出了视频输入帧。
					{
						MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																		MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts,
																		MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans,
																		MediaPocsThrdPt->m_VdoInptFrmElmPt->m_BgraVdoInptSrcFrmPt, MediaPocsThrdPt->m_VdoInpt.m_BgraVdoInptSrcFrmWidth, MediaPocsThrdPt->m_VdoInpt.m_BgraVdoInptSrcFrmHeight, MediaPocsThrdPt->m_VdoInpt.m_BgraVdoInptSrcFrmLenByt,
																		MediaPocsThrdPt->m_VdoInptFrmElmPt->m_YU12VdoInptRsltFrmPt, MediaPocsThrdPt->m_VdoInpt.m_FrmWidth, MediaPocsThrdPt->m_VdoInpt.m_FrmHeight, MediaPocsThrdPt->m_VdoInpt.m_YU12FrmLenByt,
																		MediaPocsThrdPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmPt, MediaPocsThrdPt->m_VdoInptFrmElmPt->m_EncdVdoInptRsltFrmLenByt );
					}
					else //如果没有取出视频输入帧。
					{
						MediaPocsThrdPt->m_UserReadAdoVdoInptFrmFuncPt( MediaPocsThrdPt,
																		MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, MediaPocsThrdPt->m_PcmAdoInptRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_PcmAdoInptRsltFrmVoiceActSts,
																		MediaPocsThrdPt->m_EncdAdoInptRsltFrmPt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmLenByt, MediaPocsThrdPt->m_EncdAdoInptRsltFrmIsNeedTrans,
																		NULL, 0, 0, 0,
																		NULL, 0, 0, 0,
																		NULL, 0 );
					}

					if( MediaPocsThrdPt->m_IsPrintLog != 0 )
					{
						p_NowTickMsec = FuncGetTickAsMsec();
						LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时：%uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
						p_LastTickMsec = p_NowTickMsec;
					}
				}

				if( MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt != NULL ) //如果取出了PCM格式音频输入原始帧，就追加到PCM格式音频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoInpt.m_PcmAdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt, 1, NULL );
					MediaPocsThrdPt->m_PcmAdoInptSrcFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt != NULL ) //如果取出了PCM格式音频输出原始帧，就追加到PCM格式音频输出空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_AdoOtpt.m_PcmAdoOtptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt, 1, NULL );
					MediaPocsThrdPt->m_PcmAdoOtptSrcFrmPt = NULL;
				}
				if( MediaPocsThrdPt->m_VdoInptFrmElmPt != NULL ) //如果取出了视频输入帧，就追加到视频输入空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_VdoInpt.m_VdoInptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_VdoInptFrmElmPt, 1, NULL );
					MediaPocsThrdPt->m_VdoInptFrmElmPt = NULL;
				}
				if( MediaPocsThrdPt->m_VdoOtptFrmElmPt != NULL ) //如果取出了视频输出帧，就追加到视频输出空闲帧链表。注意：从取出到追加过程中不能跳出，否则会内存泄露。
				{
					MediaPocsThrdPt->m_VdoOtpt.m_VdoOtptIdleFrmLnkLst.PutTail( &MediaPocsThrdPt->m_VdoOtptFrmElmPt, 1, NULL );
					MediaPocsThrdPt->m_VdoOtptFrmElmPt = NULL;
				}

				//如果音频输入设备已经关闭，就切换到默认的音频输入设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcIsClos != 0 )
				{
					MediaPocsThrdPt->m_AdoInpt.m_AdoInptDvcID = 0; //设置音频输入设备的标识符为默认的音频输入设备。
					LOGFI( Cu8vstr( "媒体处理线程：音频输入设备已经关闭，切换到默认的音频输入设备，并重启媒体处理线程。" ) );
					MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL );
				}

				//如果音频输出设备已经关闭，就切换到默认的音频输出设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcIsClos != 0 )
				{
					MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcID = 0; //设置音频输出设备的标识符为默认的音频输出设备。
					LOGFI( Cu8vstr( "媒体处理线程：音频输出设备已经关闭，切换到默认的音频输出设备，并重启媒体处理线程。" ) );
					MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL );
				}

				//如果视频输入设备已经关闭，就切换到默认的视频输入设备，并重启媒体处理线程。
				if( MediaPocsThrdPt->m_VdoInpt.m_FilterGraphMediaEventPt != NULL )
				{
					long p_EventCode;
					LONG_PTR p_EventParam1, p_EventParam2;
					MediaPocsThrdPt->m_VdoInpt.m_FilterGraphMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
					if( p_EventCode == EC_DEVICE_LOST )
					{
						MediaPocsThrdPt->m_VdoInpt.m_VdoInptDvcID = 0; //设置视频输入设备的标识符为默认的视频输入设备。
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
			if( p_MediaMsgLnkLstElmTotal > 0 ) //如果是媒体消息处理失败。
			{
				if( p_MediaMsg.m_MsgTyp == MsgTyp::UserInit )
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeUserInit; //设置退出码为调用用户定义的初始化函数失败。
				else if( p_MediaMsg.m_MsgTyp == MsgTyp::AdoVdoInptOtptInit )
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeAdoVdoInptOtptInit; //设置退出码为音视频输入输出初始化失败。
				else
					MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeMediaMsgPocs; //设置退出码为媒体消息处理失败。
			}
			else MediaPocsThrdPt->m_ExitCode = MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs; //设置退出码为音视频输入输出处理失败。
			//执行顺序：媒体销毁，用户销毁并退出。
			MediaPocsThrdPt->m_ReadyExitCnt++;
			MediaMsgUserDstoy p_MediaMsgUserDstoy;
			p_MediaMsgUserDstoy.m_MsgTyp = MsgTyp::UserDstoy;
			if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgUserDstoy, sizeof( p_MediaMsgUserDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
				goto OutMediaPocs;
			}
			MediaMsgAdoVdoInptOtptDstoy p_MediaMsgAdoVdoInptOtptDstoy;
			p_MediaMsgAdoVdoInptOtptDstoy.m_MsgTyp = MsgTyp::AdoVdoInptOtptDstoy;
			if( MediaPocsThrdPt->m_MediaMsgLnkLst.PutHead( &p_MediaMsgAdoVdoInptOtptDstoy, sizeof( p_MediaMsgAdoVdoInptOtptDstoy ), 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "放入媒体消息到媒体消息链表失败。原因：" ) );
				goto OutMediaPocs;
			}
		}

		MediaPocsThrdPt->m_MediaMsgLnkLst.GetTotal( &p_MediaMsgLnkLstElmTotal, 1, NULL );
		if( ( p_MediaMsgLnkLstElmTotal == 0 ) && ( MediaPocsThrdPt->m_ReadyExitCnt > 0 ) ) break; //如果媒体消息处理完毕，且媒体处理线程准备退出。
	} //媒体处理循环结束。
	
	MediaPocsThrdTmpVarDstoy( MediaPocsThrdPt );
	
	MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, 0 );

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
													   int16_t * PcmAdoInptSrcFrmPt, int16_t * PcmAdoInptRsltFrmPt, size_t PcmAdoInptFrmLenUnit, int32_t PcmAdoInptRsltFrmVoiceActSts,
													   uint8_t * EncdAdoInptRsltFrmPt, size_t EncdAdoInptRsltFrmLenByt, int32_t EncdAdoInptRsltFrmIsNeedTrans,
													   uint8_t * BgraVdoInptSrcFrmPt, int32_t BgraVdoInptSrcFrmWidth, int32_t BgraVdoInptSrcFrmHeight, size_t BgraVdoInptSrcFrmLenByt,
													   uint8_t * YU12VdoInptRsltFrmPt, int32_t YU12VdoInptRsltFrmWidth, int32_t YU12VdoInptRsltFrmHeight, size_t YU12VdoInptRsltFrmLenByt,
													   uint8_t * EncdVdoInptRsltFrmPt, size_t EncdVdoInptRsltFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserReadAdoVdoInptFrm( PcmAdoInptSrcFrmPt, PcmAdoInptRsltFrmPt, PcmAdoInptFrmLenUnit, PcmAdoInptRsltFrmVoiceActSts,
																					EncdAdoInptRsltFrmPt, EncdAdoInptRsltFrmLenByt, EncdAdoInptRsltFrmIsNeedTrans,
																					BgraVdoInptSrcFrmPt, BgraVdoInptSrcFrmWidth, BgraVdoInptSrcFrmHeight, BgraVdoInptSrcFrmLenByt,
																					YU12VdoInptRsltFrmPt, YU12VdoInptRsltFrmWidth, YU12VdoInptRsltFrmHeight, YU12VdoInptRsltFrmLenByt,
																					EncdVdoInptRsltFrmPt, EncdVdoInptRsltFrmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的写入音频输出帧函数。
extern "C" void MediaPocsThrdClsUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
													 int16_t * PcmAdoOtptSrcFrmPt, size_t PcmAdoOtptFrmLenUnit,
													 uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmSzByt, size_t * EncdAdoOtptSrcFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserWriteAdoOtptFrm( AdoOtptStrmIdx, PcmAdoOtptSrcFrmPt, PcmAdoOtptFrmLenUnit, EncdAdoOtptSrcFrmPt, EncdAdoOtptSrcFrmSzByt, EncdAdoOtptSrcFrmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的获取音频输出帧函数。
extern "C" void MediaPocsThrdClsUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
												   int16_t * PcmAdoOtptFrmPt, size_t PcmAdoOtptFrmLenUnit,
												   uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmLenByt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserGetAdoOtptFrm( AdoOtptStrmIdx,
																				PcmAdoOtptFrmPt, PcmAdoOtptFrmLenUnit,
																				EncdAdoOtptSrcFrmPt, EncdAdoOtptSrcFrmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的写入视频输出帧函数。
extern "C" void MediaPocsThrdClsUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx, uint8_t * YU12VdoOtptFrmPt, int32_t * YU12VdoOtptFrmWidthPt, int32_t * YU12VdoOtptFrmHeightPt, uint8_t * EncdVdoOtptFrmPt, size_t EncdVdoOtptFrmSzByt, size_t * EncdVdoOtptFrmLenBytPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserWriteVdoOtptFrm( VdoOtptStrmIdx, YU12VdoOtptFrmPt, YU12VdoOtptFrmWidthPt, YU12VdoOtptFrmHeightPt, EncdVdoOtptFrmPt, EncdVdoOtptFrmSzByt, EncdVdoOtptFrmLenBytPt );
}

//回调MediaPocsThrdCls类的用户定义的获取视频输出帧函数。
extern "C" void MediaPocsThrdClsUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
												   uint8_t * YU12VdoOtptFrmPt, int32_t YU12VdoOtptFrmWidth, int32_t YU12VdoOtptFrmHeight,
												   uint8_t * EncdVdoOtptFrmPt, size_t EncdVdoOtptFrmLenBytPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserGetVdoOtptFrm( VdoOtptStrmIdx,
																				YU12VdoOtptFrmPt, YU12VdoOtptFrmWidth, YU12VdoOtptFrmHeight,
																				EncdVdoOtptFrmPt, EncdVdoOtptFrmLenBytPt );
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
