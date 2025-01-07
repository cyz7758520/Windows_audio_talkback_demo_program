#include "MediaPocsThrd.h"

extern "C" int MediaPocsThrdThrdMsgPocs( MsgQueue * MsgQueuePt, MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt );
void MediaPocsThrdAdoVdoInptOtptFrmPocs( MediaPocsThrd * MediaPocsThrdPt );
DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdInit
 * 功能说明：媒体处理线程初始化。
 * 参数说明：MediaPocsThrdPtPt：[输出]，存放媒体处理线程的指针的指针，不能为NULL。
             LicnCodePt：[输入]，存放授权码的指针，可以为NULL。
             UserDataPt：[输入]，存放用户数据的指针，可以为NULL。
             UserInitFuncPt：[输入]，存放用户定义的初始化函数的指针，不能为NULL。
             UserDstoyFuncPt：[输入]，存放用户定义的销毁函数的指针，不能为NULL。
             UserPocsFuncPt：[输入]，存放用户定义的处理函数的指针，不能为NULL。
             UserMsgFuncPt：[输入]，存放用户定义的消息函数的指针，不能为NULL。
             UserDvcChgFuncPt：[输入]，存放用户定义的设备改变函数的指针，不能为NULL。
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

int MediaPocsThrdInit( MediaPocsThrd * * MediaPocsThrdPtPt, const void * LicnCodePt, void * UserDataPt,
					   MediaPocsThrd::MediaPocsThrdUserInitFuncPt UserInitFuncPt, MediaPocsThrd::MediaPocsThrdUserDstoyFuncPt UserDstoyFuncPt,
					   MediaPocsThrd::MediaPocsThrdUserPocsFuncPt UserPocsFuncPt, MediaPocsThrd::MediaPocsThrdUserMsgFuncPt UserMsgFuncPt, MediaPocsThrd::MediaPocsThrdUserDvcChgFuncPt UserDvcChgFuncPt,
					   MediaPocsThrd::MediaPocsThrdUserReadAdoVdoInptFrmFuncPt UserReadAdoVdoInptFrmFuncPt,
					   MediaPocsThrd::MediaPocsThrdUserWriteAdoOtptFrmFuncPt UserWriteAdoOtptFrmFuncPt, MediaPocsThrd::MediaPocsThrdUserGetAdoOtptFrmFuncPt UserGetAdoOtptFrmFuncPt,
					   MediaPocsThrd::MediaPocsThrdUserWriteVdoOtptFrmFuncPt UserWriteVdoOtptFrmFuncPt, MediaPocsThrd::MediaPocsThrdUserGetVdoOtptFrmFuncPt UserGetVdoOtptFrmFuncPt,
					   Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd * p_MediaPocsThrdPt = NULL; //存放媒体处理线程的指针。
	
	//判断各个变量是否正确。
	if( MediaPocsThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针的指针不正确。" ), , );
		goto Out;
	}
	if( UserInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的初始化函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的销毁函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserPocsFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的处理函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserMsgFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的消息函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserDvcChgFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的设备改变函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserReadAdoVdoInptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的读取音视频输入帧函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserWriteAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入音频输出帧函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserGetAdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取音频输出帧函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserWriteVdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的写入视频输出帧函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserGetVdoOtptFrmFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的获取视频输出帧函数的指针不正确。" ), , );
		goto Out;
	}
	
	//创建并清空媒体处理线程内存块。
	p_MediaPocsThrdPt = ( MediaPocsThrd * )calloc( 1, sizeof( MediaPocsThrd ) );
	if( p_MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空媒体处理线程内存块失败。" ), , );
		goto Out;
	}
	
	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。放在这里是防止下面的代码初始化失败导致跳过设置最小计时器分辨率，从而在调用销毁函数时多取消了一次最小计时器分辨率。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置最小计时器分辨率失败。" ), , );
		goto Out;
	}
	
	p_MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 1; //设置上一次调用了用户定义的销毁函数。

	if( MsgQueueInit( &p_MediaPocsThrdPt->m_ThrdMsgQueuePt, NULL, p_MediaPocsThrdPt, ( MsgQueue::MsgQueueUserMsgPocsFuncPt )MediaPocsThrdThrdMsgPocs, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化线程消息队列失败。原因：" ) );
		goto Out;
	}

	//初始化授权码。
	if( LicnCodePt != NULL )
	{
		uint64_t p_LicnCodeLenByt = 8 + *( uint64_t * )LicnCodePt * ( 256 + 8 + 256 ) * 4;
		p_MediaPocsThrdPt->m_LicnCodePt = ( const void * )malloc( p_LicnCodeLenByt );
		memcpy( ( void * )p_MediaPocsThrdPt->m_LicnCodePt, LicnCodePt, p_LicnCodeLenByt );
	}

	//初始化音频输入。
	p_MediaPocsThrdPt->m_AdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输入的媒体处理线程的指针。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入原始Wave文件完整路径动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入结果Wave文件完整路径动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输入设备名称动态字符串失败。" ), , );
		goto Out;
	}
	MediaPocsThrdSetAdoInpt( p_MediaPocsThrdPt, 0, 8000, 20, NULL );

	//初始化音频输出。
	p_MediaPocsThrdPt->m_AdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置音频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.Init( sizeof( AdoOtpt::Strm ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输出流容器失败。原因：" ) );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输出原始Wave文件完整路径动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音频输出设备名称动态字符串失败。" ), , );
		goto Out;
	}
	MediaPocsThrdSetAdoOtpt( p_MediaPocsThrdPt, 0, 8000, 20, NULL );

	//初始化视频输入。
	p_MediaPocsThrdPt->m_VdoInpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输入的媒体处理线程的指针。
	if( VstrInit( &p_MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化视频输入设备名称动态字符串失败。" ), , );
		goto Out;
	}
	MediaPocsThrdSetVdoInpt( p_MediaPocsThrdPt, 0, 15, 480, 640, 0, 0, 0, NULL, NULL );
	MediaPocsThrdVdoInptSetUseDvc( p_MediaPocsThrdPt, 0, 0, NULL );

	//初始化视频输出。
	p_MediaPocsThrdPt->m_VdoOtpt.m_MediaPocsThrdPt = p_MediaPocsThrdPt; //设置视频输出的媒体处理线程的指针。
	if( p_MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.Init( sizeof( VdoOtpt::Strm * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化视频输出流容器失败。原因：" ) );
		goto Out;
	}
	
	//初始化音视频输入输出Avi文件完整路径动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化音视频输入输出Avi文件完整路径动态字符串失败。" ), , );
		goto Out;
	}

	//初始化错误信息动态字符串。
	if( VstrInit( &p_MediaPocsThrdPt->m_ErrInfoVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化错误信息动态字符串失败。" ), , );
		goto Out;
	}

	p_MediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。

	p_MediaPocsThrdPt->m_UserInitFuncPt = UserInitFuncPt; //设置用户定义的初始化函数的指针。
	p_MediaPocsThrdPt->m_UserDstoyFuncPt = UserDstoyFuncPt; //设置用户定义的销毁函数的指针。

	p_MediaPocsThrdPt->m_UserPocsFuncPt = UserPocsFuncPt; //设置用户定义的处理函数的指针。
	p_MediaPocsThrdPt->m_UserMsgFuncPt = UserMsgFuncPt; //设置用户定义的消息函数的指针。
	p_MediaPocsThrdPt->m_UserDvcChgFuncPt = UserDvcChgFuncPt; //设置用户定义的设备改变函数的指针。

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
 * 功能说明：媒体处理线程销毁。
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
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 1, NULL ); //请求媒体处理线程退出，并阻塞等待。
	
	//销毁错误信息动态字符串。
	if( MediaPocsThrdPt->m_ErrInfoVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_ErrInfoVstrPt );
		MediaPocsThrdPt->m_ErrInfoVstrPt = NULL;
	}
	
	//销毁音视频输入输出Avi文件完整路径动态字符串。
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
	
	//销毁视频输出流容器。
	if( MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.m_Pt != NULL )
	{
		MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.Dstoy( NULL );
	}
	
	//销毁视频输入设备名称动态字符串。
	if( MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt );
		MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt = NULL;
	}
	
	//销毁音频输出设备名称动态字符串。
	if( MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt );
		MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt = NULL;
	}
	
	//销毁音频输出原始Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt );
		MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt = NULL;
	}
	
	//销毁音频输出流容器。
	if( MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.m_Pt != NULL )
	{
		MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.Dstoy( NULL );
	}
	
	//销毁音频输入设备名称动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt = NULL;
	}
	
	//销毁音频输入结果Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt = NULL;
	}
	
	//销毁音频输入原始Wave文件完整路径动态字符串。
	if( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt != NULL )
	{
		VstrDstoy( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt );
		MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt = NULL;
	}

	//销毁授权码。
	if( MediaPocsThrdPt->m_LicnCodePt != NULL )
	{
		free( ( void * )MediaPocsThrdPt->m_LicnCodePt );
		MediaPocsThrdPt->m_LicnCodePt = NULL;
	}

	//销毁线程消息容器。
	if( MediaPocsThrdPt->m_ThrdMsgQueuePt != NULL )
	{
		MsgQueueDstoy( MediaPocsThrdPt->m_ThrdMsgQueuePt, NULL );
	}

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	//销毁媒体处理线程内存块。
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
 * 功能说明：获取音频输入设备名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AdoInptDvcNameVstrArrPtPtPt：[输出]，存放音频输入设备名称动态字符串的指针数组的指针，不能为NULL。
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
	UINT p_AdoInptDvcTotal = 0; //存放音频输入设备总数。
	IMMDeviceEnumerator * p_AdoInptDvcEnumPt = NULL; //存放音频输入设备枚举器的指针。
	IMMDeviceCollection * p_AdoInptDvcClctPt = NULL; //存放音频输入设备收集器的指针。
	IMMDevice * p_AdoInptDvcPt = NULL; //存放音频输入设备指针。
	IPropertyStore * p_PrptStorePt = NULL; //存放属性存储器的指针。
	PROPVARIANT p_FriendlyNamePrptVariant = { 0 }; //存放友好名称字符串属性。
	HRESULT p_HRslt;
	
	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。
	PropVariantInit( &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。
	
	//判断各个变量是否正确。
	if( AdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备名称动态字符串数组的指针的指针不正确。" ), , );
		goto Out;
	}
	if( AdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备总数的指针不正确。" ), , );
		goto Out;
	}
	
	p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&p_AdoInptDvcEnumPt ); //初始化音频输入设备枚举器。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输入设备枚举器失败。原因：" ) );
		goto Out;
	}
	
	p_HRslt = p_AdoInptDvcEnumPt->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &p_AdoInptDvcClctPt ); //获取音频输入设备收集器。只收集激活音频输入设备。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输入设备收集器失败。原因：" ) );
		goto Out;
	}

	p_AdoInptDvcClctPt->GetCount( &p_AdoInptDvcTotal ); //获取音频输入设备总数。
	if( p_AdoInptDvcTotal > 0 )
	{
		p_AdoInptDvcTotal++; //递增一个默认音频输入设备。

		//创建并清空音频输入设备名称动态字符串数组内存块。
		p_AdoInptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoInptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoInptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输入设备名称动态字符串的指针数组内存块失败。" ), , );
			goto Out;
		}
		
		//获取默认音频输入设备名称。
		{
			p_HRslt = p_AdoInptDvcEnumPt->GetDefaultAudioEndpoint( eCapture, eConsole, &p_AdoInptDvcPt ); //初始化默认音频输入设备。
			if( p_HRslt == S_OK ) //如果初始化默认音频输入设备成功。
			{
				p_HRslt = p_AdoInptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。
				
				p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。
				
				if( VstrInit( &p_AdoInptDvcNameArrPtPt[ 0 ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
				{
					VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称动态字符串失败。" ), , );
					goto Out;
				}
				VstrIns( p_AdoInptDvcNameArrPtPt[ 0 ], 0, Cu8vstr( "默认：" ) );
				
				p_PrptStorePt->Release(); //销毁属性存储器。

				p_AdoInptDvcPt->Release(); //销毁音频输入设备。
			}
			else //如果初始化默认音频输入设备失败。
			{
				GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化默认音频输入设备失败。原因：" ) );
				goto Out;
			}
		}

		//获取全部音频输入设备名称。
		for( UINT p_DvcNum = 1; p_DvcNum < p_AdoInptDvcTotal; p_DvcNum++ )
		{
			p_HRslt = p_AdoInptDvcClctPt->Item( p_DvcNum - 1, &p_AdoInptDvcPt ); //初始化音频输入设备。

			p_HRslt = p_AdoInptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。

			p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。

			if( VstrInit( &p_AdoInptDvcNameArrPtPt[ p_DvcNum ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输入设备名称动态字符串失败。" ), , );
				goto Out;
			}

			p_PrptStorePt->Release(); //销毁属性存储器。

			p_AdoInptDvcPt->Release(); //销毁音频输入设备。
		}
	}

	p_AdoInptDvcClctPt->Release(); //销毁音频输入设备收集器。

	p_AdoInptDvcEnumPt->Release(); //销毁音频输入设备枚举器。

	*AdoInptDvcNameVstrArrPtPtPt = p_AdoInptDvcNameArrPtPt; //设置音频输入设备名称动态字符串数组的指针。
	*AdoInptDvcTotalPt = p_AdoInptDvcTotal; //设置音频输入设备总数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameArrPtPt, p_AdoInptDvcTotal, NULL );
	}
	PropVariantClear( &p_FriendlyNamePrptVariant ); //销毁友好名称字符串属性。
	CoUninitialize(); //销毁COM库。
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoOtptDvcName
 * 功能说明：获取音频输出设备名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：AdoOtptDvcNameVstrArrPtPtPt：[输出]，存放音频输出设备名称动态字符串的指针数组的指针，不能为NULL。
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
	UINT p_AdoOtptDvcTotal = 0; //存放音频输出设备总数。
	IMMDeviceEnumerator * p_AdoOtptDvcEnumPt = NULL; //存放音频输出设备枚举器的指针。
	IMMDeviceCollection * p_AdoOtptDvcClctPt = NULL; //存放音频输出设备收集器的指针。
	IMMDevice * p_AdoOtptDvcPt = NULL; //存放音频输出设备指针。
	IPropertyStore * p_PrptStorePt = NULL; //存放属性存储器的指针。
	PROPVARIANT p_FriendlyNamePrptVariant = { 0 }; //存放友好名称字符串属性。
	HRESULT p_HRslt;
	
	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。
	PropVariantInit( &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。

	//判断各个变量是否正确。
	if( AdoOtptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备名称动态字符串数组的指针的指针不正确。" ), , );
		goto Out;
	}
	if( AdoOtptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备总数的指针不正确。" ), , );
		goto Out;
	}
	
	p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&p_AdoOtptDvcEnumPt ); //初始化音频输出设备枚举器。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化音频输出设备枚举器失败。原因：" ) );
		goto Out;
	}

	p_HRslt = p_AdoOtptDvcEnumPt->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, &p_AdoOtptDvcClctPt ); //获取音频输出设备收集器。只收集激活音频输出设备。
	if( p_HRslt != S_OK )
	{
		GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输出设备收集器失败。原因：" ) );
		goto Out;
	}
	
	p_AdoOtptDvcClctPt->GetCount( &p_AdoOtptDvcTotal ); //获取音频输出设备总数。
	if( p_AdoOtptDvcTotal > 0 )
	{
		p_AdoOtptDvcTotal++; //递增一个默认音频输出设备。

		//创建并清空音频输出设备名称动态字符串数组内存块。
		p_AdoOtptDvcNameArrPtPt = ( Vstr * * )calloc( p_AdoOtptDvcTotal, sizeof( Vstr * ) );
		if( p_AdoOtptDvcNameArrPtPt == NULL )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空音频输出设备名称动态字符串的指针数组内存块失败。" ), , );
			goto Out;
		}
		
		//获取默认音频输出设备名称。
		{
			p_HRslt = p_AdoOtptDvcEnumPt->GetDefaultAudioEndpoint( eRender, eConsole, &p_AdoOtptDvcPt ); //初始化默认音频输出设备。
			if( p_HRslt == S_OK ) //如果初始化默认音频输出设备成功。
			{
				p_HRslt = p_AdoOtptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。
				
				p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。
				
				if( VstrInit( &p_AdoOtptDvcNameArrPtPt[ 0 ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
				{
					VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输出设备名称动态字符串失败。" ), , );
					goto Out;
				}
				VstrIns( p_AdoOtptDvcNameArrPtPt[ 0 ], 0, Cu8vstr( "默认：" ) );
				
				p_PrptStorePt->Release(); //销毁属性存储器。

				p_AdoOtptDvcPt->Release(); //销毁音频输出设备。
			}
			else //如果初始化默认音频输出设备失败。
			{
				GetWinSysErrInfo( p_HRslt, ErrInfoVstrPt ), VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化默认音频输出设备失败。原因：" ) );
				goto Out;
			}
		}

		//获取全部音频输出设备名称。
		for( UINT p_DvcNum = 1; p_DvcNum < p_AdoOtptDvcTotal; p_DvcNum++ )
		{
			p_HRslt = p_AdoOtptDvcClctPt->Item( p_DvcNum - 1, &p_AdoOtptDvcPt ); //初始化音频输出设备。

			p_HRslt = p_AdoOtptDvcPt->OpenPropertyStore( STGM_READ, &p_PrptStorePt ); //初始化属性存储器。

			p_HRslt = p_PrptStorePt->GetValue( PKEY_Device_FriendlyName, &p_FriendlyNamePrptVariant ); //初始化友好名称字符串属性。

			if( VstrInit( &p_AdoOtptDvcNameArrPtPt[ p_DvcNum ], Utf16, , Cu16vstr( p_FriendlyNamePrptVariant.pwszVal ) ) != 0 )
			{
				VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建音频输出设备名称动态字符串失败。" ), , );
				goto Out;
			}

			p_PrptStorePt->Release(); //销毁属性存储器。

			p_AdoOtptDvcPt->Release(); //销毁音频输出设备。
		}
	}

	p_AdoOtptDvcClctPt->Release(); //销毁音频输出设备收集器。

	p_AdoOtptDvcEnumPt->Release(); //销毁音频输出设备枚举器。

	*AdoOtptDvcNameVstrArrPtPtPt = p_AdoOtptDvcNameArrPtPt; //设置音频输出设备名称动态字符串数组的指针。
	*AdoOtptDvcTotalPt = p_AdoOtptDvcTotal; //设置音频输出设备总数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameArrPtPt, p_AdoOtptDvcTotal, NULL );
	}
	PropVariantClear( &p_FriendlyNamePrptVariant ); //销毁友好名称字符串属性。
	CoUninitialize(); //销毁COM库。
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetVdoInptDvcName
 * 功能说明：获取视频输入设备名称。调用本函数成功后，必须手动调用MediaPocsThrdDstoyDvcName函数销毁。
 * 参数说明：VdoInptDvcNameVstrArrPtPtPt：[输出]，存放视频输入设备名称动态字符串的指针数组的指针，不能为NULL。
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
	UINT p_VdoInptDvcTotal = 0; //存放视频输入设备总数。
	ICreateDevEnum * p_CreateDevEnumPt = NULL;
	IEnumMoniker * p_EnumMonikerPt = NULL;
	IMoniker * p_MonikerPt = NULL;
	IPropertyBag * p_PropertyBagPt = NULL;
	ULONG p_Ulong;
	
	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	//判断各个变量是否正确。
	if( VdoInptDvcNameVstrArrPtPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备名称动态字符串数组的指针的指针不正确。" ), , );
		goto Out;
	}
	if( VdoInptDvcTotalPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备总数的指针不正确。" ), , );
		goto Out;
	}

	//创建系统设备枚举器。
	if( CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, ( void * * )&p_CreateDevEnumPt ) != S_OK )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建系统设备枚举器失败。" ), , );
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
						VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串的指针数组内存块失败。" ), , );
						goto Out;
					}
					p_VdoInptDvcNameArrPtPt = p_TmpVstrPtPt;

					p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcTotal - 1 ] = NULL;
					if( VstrInit( &p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcTotal - 1 ], Utf16, , Cu16vstr( varName.bstrVal ) ) != 0 )
					{
						VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串失败。" ), , );
						goto Out;
					}
					VariantClear( &varName );

					if( SUCCEEDED( p_PropertyBagPt->Read( L"DevicePath", &varName, 0 ) ) )
					{
						if( VstrCat( p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcTotal - 1 ], Cu8vstr( " (" ) ) != 0 )
						{
							VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串失败。" ), , );
							goto Out;
						}
						if( VstrCat( p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcTotal - 1 ], Cu16vstr( varName.bstrVal ) ) != 0 )
						{
							VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串失败。" ), , );
							goto Out;
						}
						if( VstrCat( p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcTotal - 1 ], Cu8vstr( ")" ) ) != 0 )
						{
							VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建视频输入设备名称动态字符串失败。" ), , );
							goto Out;
						}
						VariantClear( &varName );
					}
				}
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
	*VdoInptDvcTotalPt = p_VdoInptDvcTotal; //设置视频输入设备总数。

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
             DvcTotal：[输出]，存放设备总数。
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
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设备名称动态字符串数组的指针不正确。" ), , );
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

	free( DvcNameArrPtPt ); //销毁设备名称动态字符串的指针数组内存块。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoInptDvcId
 * 功能说明：获取音频输入设备标识符。
 * 参数说明：AdoInptDvcNameVstrPt：[输入]，存放音频输入设备名称动态字符串的指针，不能为NULL。
             AdoInptDvcIdPt：[输出]，存放音频输入设备标识符的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoInptDvcId( Vstr * AdoInptDvcNameVstrPt, UINT * AdoInptDvcIdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_AdoInptDvcNameArrPtPt = NULL; //存放音频输入设备名称动态字符串的指针数组的指针。
	UINT p_AdoInptDvcTotal = 0; //存放音频输入设备总数。
	UINT p_AdoInptDvcId; //存放音频输入设备的标识符，取值范围为从1到音频输入设备的总数，为0表示使用默认设备。
	int p_CmpRslt;
	
	//判断各个变量是否正确。
	if( AdoInptDvcNameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备名称动态字符串的指针不正确。" ), , );
		goto Out;
	}
	if( AdoInptDvcIdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输入设备标识符的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdGetAdoInptDvcName( &p_AdoInptDvcNameArrPtPt, &p_AdoInptDvcTotal, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输入设备名称失败。原因：" ) );
		goto Out;
	}
	if( p_AdoInptDvcTotal == 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "系统没有音频输入设备。" ), , );
		goto Out;
	}
	for( p_AdoInptDvcId = 0; p_AdoInptDvcId < p_AdoInptDvcTotal; p_AdoInptDvcId++ )
	{
		if( VstrCmp( AdoInptDvcNameVstrPt, , , p_AdoInptDvcNameArrPtPt[ p_AdoInptDvcId ], , &p_CmpRslt ), p_CmpRslt == 0 ) goto OutFindAdoInptDvc; //如果已找到指定的音频输入设备的标识符。
	}
	VstrCpy( ErrInfoVstrPt, Cu8vstr( "未找到指定的音频输入设备。" ), , );
	goto Out;
	OutFindAdoInptDvc:;
	*AdoInptDvcIdPt = p_AdoInptDvcId; //设置音频输入设备标识符。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameArrPtPt, p_AdoInptDvcTotal, NULL );
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetAdoOtptDvcId
 * 功能说明：获取音频输出设备标识符。
 * 参数说明：AdoOtptDvcNameVstrPt：[输入]，存放音频输出设备名称动态字符串的指针，不能为NULL。
             AdoOtptDvcIdPt：[输出]，存放音频输出设备标识符的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetAdoOtptDvcId( Vstr * AdoOtptDvcNameVstrPt, UINT * AdoOtptDvcIdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_AdoOtptDvcNameArrPtPt = NULL; //存放音频输出设备名称动态字符串的指针数组的指针。
	UINT p_AdoOtptDvcTotal = 0; //存放音频输出设备总数。
	UINT p_AdoOtptDvcId; //存放音频输出设备的标识符，取值范围为从1到音频输出设备的总数，为0表示使用默认设备。
	int p_CmpRslt;
	
	//判断各个变量是否正确。
	if( AdoOtptDvcNameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备名称动态字符串的指针不正确。" ), , );
		goto Out;
	}
	if( AdoOtptDvcIdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "音频输出设备标识符的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameArrPtPt, &p_AdoOtptDvcTotal, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取音频输出设备名称失败。原因：" ) );
		goto Out;
	}
	if( p_AdoOtptDvcTotal == 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "系统没有音频输出设备" ), , );
		goto Out;
	}
	for( p_AdoOtptDvcId = 0; p_AdoOtptDvcId < p_AdoOtptDvcTotal; p_AdoOtptDvcId++ )
	{
		if( VstrCmp( AdoOtptDvcNameVstrPt, , , p_AdoOtptDvcNameArrPtPt[ p_AdoOtptDvcId ], , &p_CmpRslt ), p_CmpRslt == 0 ) goto OutFindAdoOtptDvc; //如果已找到指定的音频输出设备的标识符。
	}
	VstrCpy( ErrInfoVstrPt, Cu8vstr( "未找到指定的音频输出设备。" ), , );
	goto Out;
	OutFindAdoOtptDvc:;
	*AdoOtptDvcIdPt = p_AdoOtptDvcId; //设置音频输出设备标识符。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameArrPtPt, p_AdoOtptDvcTotal, NULL );
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdGetVdoInptDvcId
 * 功能说明：获取视频输入设备标识符。
 * 参数说明：VdoInptDvcNameVstrPt：[输入]，存放视频输入设备名称动态字符串的指针，不能为NULL。
             VdoInptDvcIdPt：[输出]，存放视频输入设备标识符的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdGetVdoInptDvcId( Vstr * VdoInptDvcNameVstrPt, UINT * VdoInptDvcIdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * * p_VdoInptDvcNameArrPtPt = NULL; //存放视频输入设备名称动态字符串的指针数组的指针。
	UINT p_VdoInptDvcTotal = 0; //存放视频输入设备总数。
	UINT p_VdoInptDvcId; //存放视频输入设备的标识符，取值范围为从1到视频输入设备的总数，为0表示使用默认设备。
	int p_CmpRslt;
	
	//判断各个变量是否正确。
	if( VdoInptDvcNameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备名称动态字符串的指针不正确。" ), , );
		goto Out;
	}
	if( VdoInptDvcIdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "视频输入设备标识符的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameArrPtPt, &p_VdoInptDvcTotal, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "获取视频输入设备名称失败。原因：" ) );
		goto Out;
	}
	if( p_VdoInptDvcTotal == 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "系统没有视频输入设备。" ), , );
		goto Out;
	}
	for( p_VdoInptDvcId = 0; p_VdoInptDvcId < p_VdoInptDvcTotal; p_VdoInptDvcId++ )
	{
		if( VstrCmp( VdoInptDvcNameVstrPt, , , p_VdoInptDvcNameArrPtPt[ p_VdoInptDvcId ], , &p_CmpRslt ), p_CmpRslt == 0 ) goto OutFindVdoInptDvc; //如果已找到指定的视频输入设备的标识符。
	}
	VstrCpy( ErrInfoVstrPt, Cu8vstr( "未找到指定的视频输入设备。" ), , );
	goto Out;
	OutFindVdoInptDvc:;
	*VdoInptDvcIdPt = p_VdoInptDvcId; //设置视频输入设备标识符。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameArrPtPt, p_VdoInptDvcTotal, NULL );
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSetAdoInpt
 * 功能说明：媒体处理线程的设置音频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
             FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoInpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetAdoInpt p_ThrdMsgSetAdoInpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ), , );
		goto Out;
	}
	if( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgSetAdoInpt.m_SmplRate = SmplRate;
	p_ThrdMsgSetAdoInpt.m_FrmLenMsec = FrmLenMsec;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetAdoInpt, &p_ThrdMsgSetAdoInpt, sizeof( p_ThrdMsgSetAdoInpt ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsUseSystemAecNsAgc：[输入]，存放是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器，为0表示不使用，为非0表示要使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsUseSystemAecNsAgc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseSystemAecNsAgc, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetIsUseSystemAecNsAgc p_ThrdMsgAdoInptSetIsUseSystemAecNsAgc;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgAdoInptSetIsUseSystemAecNsAgc.m_IsUseSystemAecNsAgc = IsUseSystemAecNsAgc;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetIsUseSystemAecNsAgc, &p_ThrdMsgAdoInptSetIsUseSystemAecNsAgc, sizeof( p_ThrdMsgAdoInptSetIsUseSystemAecNsAgc ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseNoAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseNoAec p_ThrdMsgAdoInptSetUseNoAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseNoAec, NULL, 0, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             FilterLenMsec：[输入]，存放滤波器的长度，单位为毫秒。
             IsUseRec：[输入]，存放是否使用残余回音消除，为非0表示要使用，为0表示不使用。
             EchoMutp：[输入]，存放在残余回音消除时，残余回音倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
             EchoCntu：[输入]，存放在残余回音消除时，残余回音持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
             EchoSupes：[输入]，存放在残余回音消除时，残余回音最大衰减分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             EchoSupesAct：[输入]，存放在残余回音消除时，有近端语音活动时残余回音最大衰减分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t FilterLenMsec, int32_t IsUseRec, float EchoMutp, float EchoCntu, int32_t EchoSupes, int32_t EchoSupesAct, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexAec p_ThrdMsgAdoInptSetUseSpeexAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseSpeexAec.m_FilterLenMsec = FilterLenMsec;
	p_ThrdMsgAdoInptSetUseSpeexAec.m_IsUseRec = IsUseRec;
	p_ThrdMsgAdoInptSetUseSpeexAec.m_EchoMutp = EchoMutp;
	p_ThrdMsgAdoInptSetUseSpeexAec.m_EchoCntu = EchoCntu;
	p_ThrdMsgAdoInptSetUseSpeexAec.m_EchoSupes = EchoSupes;
	p_ThrdMsgAdoInptSetUseSpeexAec.m_EchoSupesAct = EchoSupesAct;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexAec, &p_ThrdMsgAdoInptSetUseSpeexAec, sizeof( p_ThrdMsgAdoInptSetUseSpeexAec ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcAecm
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc定点版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsUseCNGMode：[输入]，存放是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
             EchoMode：[输入]，存放消除模式，消除模式越高消除越强，取值区间为[0,4]。
             Delay：[输入]，存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcAecm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseCNGMode, int32_t EchoMode, int32_t Delay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAecm p_ThrdMsgAdoInptSetUseWebRtcAecm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseWebRtcAecm.m_IsUseCNGMode = IsUseCNGMode;
	p_ThrdMsgAdoInptSetUseWebRtcAecm.m_EchoMode = EchoMode;
	p_ThrdMsgAdoInptSetUseWebRtcAecm.m_Delay = Delay;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAecm, &p_ThrdMsgAdoInptSetUseWebRtcAecm, sizeof( p_ThrdMsgAdoInptSetUseWebRtcAecm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             EchoMode：[输入]，存放消除模式，消除模式越高消除越强，取值区间为[0,2]。
             Delay：[输入]，存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             IsUseDelayAgstcMode：[输入]，存放是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             IsUseExtdFilterMode：[输入]，存放是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             IsUseRefinedFilterAdaptAecMode：[输入]，存放是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             IsUseAdaptAdjDelay：[输入]，存放是否使用自适应调节回音延迟，为非0表示要使用，为0表示不使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t EchoMode, int32_t Delay, int32_t IsUseDelayAgstcMode, int32_t IsUseExtdFilterMode, int32_t IsUseRefinedFilterAdaptAecMode, int32_t IsUseAdaptAdjDelay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec p_ThrdMsgAdoInptSetUseWebRtcAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_EchoMode = EchoMode;
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_Delay = Delay;
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_IsUseDelayAgstcMode = IsUseDelayAgstcMode;
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_IsUseExtdFilterMode = IsUseExtdFilterMode;
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_IsUseRefinedFilterAdaptAecMode = IsUseRefinedFilterAdaptAecMode;
	p_ThrdMsgAdoInptSetUseWebRtcAec.m_IsUseAdaptAdjDelay = IsUseAdaptAdjDelay;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAec, &p_ThrdMsgAdoInptSetUseWebRtcAec, sizeof( p_ThrdMsgAdoInptSetUseWebRtcAec ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAdoInptSetUseWebRtcAec3
 * 功能说明：媒体处理线程的音频输入设置要使用WebRtc第三版声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             Delay：[输入]，存放回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcAec3( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t Delay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec3 p_ThrdMsgAdoInptSetUseWebRtcAec3;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgAdoInptSetUseWebRtcAec3.m_Delay = Delay;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAec3, &p_ThrdMsgAdoInptSetUseWebRtcAec3, sizeof( p_ThrdMsgAdoInptSetUseWebRtcAec3 ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAdoInptSetUseSpeexWebRtcAec
 * 功能说明：媒体处理线程的音频输入设置要使用SpeexWebRtc三重声学回音消除器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             WorkMode：[输入]，存放工作模式，为1表示Speex声学回音消除器+WebRtc定点版声学回音消除器，为2表示WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器，为3表示Speex声学回音消除器+WebRtc定点版声学回音消除器+WebRtc浮点版声学回音消除器。
             SpeexAecFilterLenMsec：[输入]，存放Speex声学回音消除器滤波器的长度，单位为毫秒。
             SpeexAecIsUseRec：[输入]，存放Speex声学回音消除器是否使用残余回音消除，为非0表示要使用，为0表示不使用。
             SpeexAecEchoMutp：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音倍数，倍数越大消除越强，取值区间为[0.0,100.0]。
             SpeexAecEchoCntu：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音持续系数，系数越大消除越强，取值区间为[0.0,0.9]。
             SpeexAecEchoSupes：[输入]，存放Speex声学回音消除器在残余回音消除时，残余回音最大衰减分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             SpeexAecEchoSupesAct：[输入]，存放Speex声学回音消除器在残余回音消除时，有近端语音活动时残余回音最大衰减分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             WebRtcAecmIsUseCNGMode：[输入]，存放WebRtc定点版声学回音消除器是否使用舒适噪音生成模式，为非0表示要使用，为0表示不使用。
             WebRtcAecmEchoMode：[输入]，存放WebRtc定点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,4]。
             WebRtcAecmDelay：[输入]，存放WebRtc定点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecEchoMode：[输入]，存放WebRtc浮点版声学回音消除器的消除模式，消除模式越高消除越强，取值区间为[0,2]。
             WebRtcAecDelay：[输入]，存放WebRtc浮点版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             WebRtcAecIsUseDelayAgstcMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用回音延迟不可知模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseExtdFilterMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用扩展滤波器模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseRefinedFilterAdaptAecMode：[输入]，存放WebRtc浮点版声学回音消除器是否使用精制滤波器自适应Aec模式，为非0表示要使用，为0表示不使用。
             WebRtcAecIsUseAdaptAdjDelay：[输入]，存放WebRtc浮点版声学回音消除器是否使用自适应调节回音延迟，为非0表示要使用，为0表示不使用。
             WebRtcAec3Delay：[输入]，存放WebRtc第三版声学回音消除器的回音延迟，单位为毫秒，取值区间为[-2147483648,2147483647]，为0表示自适应设置。
             IsUseSameRoomAec：[输入]，存放是否使用同一房间声学回音消除，为非0表示要使用，为0表示不使用。
             SameRoomEchoMinDelay：[输入]，存放同一房间回音最小延迟，单位为毫秒，取值区间为[1,2147483647]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexWebRtcAec( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t WorkMode, int32_t SpeexAecFilterLenMsec, int32_t SpeexAecIsUseRec, float SpeexAecEchoMutp, float SpeexAecEchoCntu, int32_t SpeexAecEchoSupes, int32_t SpeexAecEchoSupesAct, int32_t WebRtcAecmIsUseCNGMode, int32_t WebRtcAecmEchoMode, int32_t WebRtcAecmDelay, int32_t WebRtcAecEchoMode, int32_t WebRtcAecDelay, int32_t WebRtcAecIsUseDelayAgstcMode, int32_t WebRtcAecIsUseExtdFilterMode, int32_t WebRtcAecIsUseRefinedFilterAdaptAecMode, int32_t WebRtcAecIsUseAdaptAdjDelay, int32_t WebRtcAec3Delay, int32_t IsUseSameRoomAec, int32_t SameRoomEchoMinDelay, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexWebRtcAec p_ThrdMsgAdoInptSetUseSpeexWebRtcAec;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WorkMode = WorkMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecFilterLenMsec = SpeexAecFilterLenMsec;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecIsUseRec = SpeexAecIsUseRec;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoMutp = SpeexAecEchoMutp;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoCntu = SpeexAecEchoCntu;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoSupes = SpeexAecEchoSupes;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SpeexAecEchoSupesAct = SpeexAecEchoSupesAct;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode = WebRtcAecmIsUseCNGMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmEchoMode = WebRtcAecmEchoMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecmDelay = WebRtcAecmDelay;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecEchoMode = WebRtcAecEchoMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecDelay = WebRtcAecDelay;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode = WebRtcAecIsUseDelayAgstcMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode = WebRtcAecIsUseExtdFilterMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = WebRtcAecIsUseRefinedFilterAdaptAecMode;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay = WebRtcAecIsUseAdaptAdjDelay;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_WebRtcAec3Delay = WebRtcAec3Delay;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_IsUseSameRoomAec = IsUseSameRoomAec;
	p_ThrdMsgAdoInptSetUseSpeexWebRtcAec.m_SameRoomEchoMinDelay = SameRoomEchoMinDelay;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexWebRtcAec, &p_ThrdMsgAdoInptSetUseSpeexWebRtcAec, sizeof( p_ThrdMsgAdoInptSetUseSpeexWebRtcAec ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseNoNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseNoNs p_ThrdMsgAdoInptSetUseNoNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseNoNs, &p_ThrdMsgAdoInptSetUseNoNs, sizeof( p_ThrdMsgAdoInptSetUseNoNs ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsUseNs：[输入]，存放是否使用噪音抑制，为非0表示要使用，为0表示不使用。
             NoiseSupes：[输入]，存放在噪音抑制时，噪音最大衰减分贝值，分贝值越小衰减越大，取值区间为[-2147483648,0]。
             IsUseDereverb：[输入]，存放是否使用混响音消除，为非0表示要使用，为0表示不使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexPrpocsNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseNs, int32_t NoiseSupes, int32_t IsUseDereverb, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexPrpocsNs p_ThrdMsgAdoInptSetUseSpeexPrpocsNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseSpeexPrpocsNs.m_IsUseNs = IsUseNs;
	p_ThrdMsgAdoInptSetUseSpeexPrpocsNs.m_NoiseSupes = NoiseSupes;
	p_ThrdMsgAdoInptSetUseSpeexPrpocsNs.m_IsUseDereverb = IsUseDereverb;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexPrpocsNs, &p_ThrdMsgAdoInptSetUseSpeexPrpocsNs, sizeof( p_ThrdMsgAdoInptSetUseSpeexPrpocsNs ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             PolicyMode：[输入]，存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcNsx( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNsx p_ThrdMsgAdoInptSetUseWebRtcNsx;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgAdoInptSetUseWebRtcNsx.m_PolicyMode = PolicyMode;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcNsx, &p_ThrdMsgAdoInptSetUseWebRtcNsx, sizeof( p_ThrdMsgAdoInptSetUseWebRtcNsx ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             PolicyMode：[输入]，存放策略模式，策略模式越高抑制越强，取值区间为[0,3]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseWebRtcNs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t PolicyMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNs p_ThrdMsgAdoInptSetUseWebRtcNs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetUseWebRtcNs.m_PolicyMode = PolicyMode;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcNs, &p_ThrdMsgAdoInptSetUseWebRtcNs, sizeof( p_ThrdMsgAdoInptSetUseWebRtcNs ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseRNNoise( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseRNNoise p_ThrdMsgAdoInptSetUseRNNoise;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseRNNoise, &p_ThrdMsgAdoInptSetUseRNNoise, sizeof( p_ThrdMsgAdoInptSetUseRNNoise ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置是否使用Speex预处理器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsUseSpeexPrpocs：[输入]，存放是否使用Speex预处理器，为非0表示要使用，为0表示不使用。
             IsUseVad：[输入]，存放是否使用语音活动检测，为非0表示要使用，为0表示不使用。
             VadProbStart：[输入]，存放在语音活动检测时，从无语音活动到有语音活动判断百分比概率，概率越大越难判断为有语音活，取值区间为[0,100]。
             VadProbCntu：[输入]，存放在语音活动检测时，从有语音活动到无语音活动判断百分比概率，概率越大越容易判断为无语音活动，取值区间为[0,100]。
             IsUseAgc：[输入]，存放是否使用自动增益控制，为非0表示要使用，为0表示不使用。
             AgcLevel：[输入]，存放在自动增益控制时，增益目标等级，目标等级越大增益越大，取值区间为[1,2147483647]。
             AgcIncrement：[输入]，存放在自动增益控制时，每秒最大增益分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
             AgcDecrement：[输入]，存放在自动增益控制时，每秒最大减益分贝值，分贝值越小减益越大，取值区间为[-2147483648,0]。
             AgcMaxGain：[输入]，存放在自动增益控制时，最大增益分贝值，分贝值越大增益越大，取值区间为[0,2147483647]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsUseSpeexPrpocs( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseSpeexPrpocs, int32_t IsUseVad, int32_t VadProbStart, int32_t VadProbCntu, int32_t IsUseAgc, int32_t AgcLevel, int32_t AgcIncrement, int32_t AgcDecrement, int32_t AgcMaxGain, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetIsUseSpeexPrpocs p_ThrdMsgAdoInptSetIsUseSpeexPrpocs;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseSpeexPrpocs = IsUseSpeexPrpocs;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseVad = IsUseVad;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_VadProbStart = VadProbStart;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_VadProbCntu = VadProbCntu;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_IsUseAgc = IsUseAgc;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_AgcLevel = AgcLevel;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_AgcIncrement = AgcIncrement;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_AgcDecrement = AgcDecrement;
	p_ThrdMsgAdoInptSetIsUseSpeexPrpocs.m_AgcMaxGain = AgcMaxGain;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetIsUseSpeexPrpocs, &p_ThrdMsgAdoInptSetIsUseSpeexPrpocs, sizeof( p_ThrdMsgAdoInptSetIsUseSpeexPrpocs ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置要使用Pcm原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUsePcm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUsePcm p_ThrdMsgAdoInptSetUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUsePcm, &p_ThrdMsgAdoInptSetUsePcm, sizeof( p_ThrdMsgAdoInptSetUsePcm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置要使用Speex编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             UseCbrOrVbr：[输入]，存放使用固定比特率还是动态比特率进行编码，为0表示要使用固定比特率，为非0表示要使用动态比特率。
             Qualt：[输入]，存放编码质量等级，质量等级越高音质越好、压缩率越低，取值区间为[0,10]。
             Cmplxt：[输入]，存放编码复杂度，复杂度越高压缩率不变、CPU使用率越高、音质越好，取值区间为[0,10]。
             PlcExptLossRate：[输入]，存放在数据包丢失隐藏时，数据包预计丢失概率，预计丢失概率越高抗网络抖动越强、压缩率越低，取值区间为[0,100]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseSpeexEncd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t UseCbrOrVbr, int32_t Qualt, int32_t Cmplxt, int32_t PlcExptLossRate, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexEncd p_ThrdMsgAdoInptSetUseSpeexEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgAdoInptSetUseSpeexEncd.m_UseCbrOrVbr = UseCbrOrVbr;
	p_ThrdMsgAdoInptSetUseSpeexEncd.m_Qualt = Qualt;
	p_ThrdMsgAdoInptSetUseSpeexEncd.m_Cmplxt = Cmplxt;
	p_ThrdMsgAdoInptSetUseSpeexEncd.m_PlcExptLossRate = PlcExptLossRate;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexEncd, &p_ThrdMsgAdoInptSetUseSpeexEncd, sizeof( p_ThrdMsgAdoInptSetUseSpeexEncd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置要使用Opus编码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseOpusEncd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseOpusEncd p_ThrdMsgAdoInptSetUseOpusEncd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseOpusEncd, &p_ThrdMsgAdoInptSetUseOpusEncd, sizeof( p_ThrdMsgAdoInptSetUseOpusEncd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置是否绘制音频波形到窗口。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsDraw：[输入]，存放是否绘制音频波形到窗口，为非0表示要使用，为0表示不使用。
             RsltWndHdl：[输入]，存放结果波形窗口的句柄。
             SrcWndHdl：[输入]，存放原始波形窗口的句柄。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsDraw, HWND SrcWndHdl, HWND RsltWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( IsDraw != 0 ) && ( SrcWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始波形窗口的句柄不正确。" ), , );
		goto Out;
	}
	if( ( IsDraw != 0 ) && ( RsltWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "结果波形窗口的句柄不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd.m_IsDraw = IsDraw;
	if( IsDraw != 0 )
	{
		p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd.m_SrcWndHdl = SrcWndHdl;
		p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd.m_RsltWndHdl = RsltWndHdl;
	}
	else
	{
		p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd.m_SrcWndHdl = NULL;
		p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd.m_RsltWndHdl = NULL;
	}
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetIsDrawAdoWavfmToWnd, &p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd, sizeof( p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile
 * 功能说明：媒体处理线程的音频输入设置设置是否保存音频到Wave文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsSave：[输入]，存放是否保存，为非0表示要保存，为0表示不保存。
             SrcFullPathVstrPt：[输入]，存放原始完整路径动态字符串的指针。
             RsltFullPathVstrPt：[输入]，存放结果完整路径动态字符串的指针。
             WrBufSzByt：[输入]，存放写入缓冲区大小，单位为字节。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, const Vstr * RsltFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetIsSaveAdoToWaveFile p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile;
	p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt = NULL;
	p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( IsSave != 0 ) && ( SrcFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始完整路径动态字符串不正确。" ), , );
		goto Out;
	}
	if( ( IsSave != 0 ) && ( RsltFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "结果完整路径动态字符串不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_IsSave = IsSave;
	if( IsSave != 0 )
	{
		if( VstrInit( &p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt, Utf8, , SrcFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置原始Wave文件完整路径动态字符串失败。" ), , );
			goto Out;
		}
		if( VstrInit( &p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltFullPathVstrPt, Utf8, , RsltFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置结果Wave文件完整路径动态字符串失败。" ), , );
			goto Out;
		}
	}
	p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_WrBufSzByt = WrBufSzByt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetIsSaveAdoToWaveFile, &p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile, sizeof( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt != NULL ) VstrDstoy( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt );
		if( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltFullPathVstrPt != NULL ) VstrDstoy( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFile.m_RsltFullPathVstrPt );
	}

	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoInptSetUseDvc
 * 功能说明：媒体处理线程的音频输入设置设置使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             NameVstrPt：[输入]，存放名称动态字符串的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetUseDvc p_ThrdMsgAdoInptSetUseDvc;
	p_ThrdMsgAdoInptSetUseDvc.m_NameVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( NameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "名称动态字符串的指针不正确。" ), , );
		goto Out;
	}

	if( VstrInit( &p_ThrdMsgAdoInptSetUseDvc.m_NameVstrPt, Utf8, , NameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置名称动态字符串失败。" ), , );
		goto Out;
	}
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetUseDvc, &p_ThrdMsgAdoInptSetUseDvc, sizeof( p_ThrdMsgAdoInptSetUseDvc ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输入设置设置是否静音。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoInptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoInptSetIsMute p_ThrdMsgAdoInptSetIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoInptSetIsMute.m_IsMute = IsMute;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoInptSetIsMute, &p_ThrdMsgAdoInptSetIsMute, sizeof( p_ThrdMsgAdoInptSetIsMute ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             SmplRate：[输入]，存放采样频率，单位为赫兹，取值只能为8000、16000、32000、48000。
             FrmLenMsec：[输入]，存放帧的长度，单位为毫秒，取值只能为10、20、30。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetAdoOtpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t SmplRate, size_t FrmLenMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetAdoOtpt p_ThrdMsgSetAdoOtpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( SmplRate != 8000 ) && ( SmplRate != 16000 ) && ( SmplRate != 32000 ) && ( SmplRate != 48000 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "采样频率不正确。" ), , );
		goto Out;
	}
	if( ( FrmLenMsec <= 0 ) || ( FrmLenMsec % 10 != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧的长度不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetAdoOtpt.m_SmplRate = SmplRate;
	p_ThrdMsgSetAdoOtpt.m_FrmLenMsec = FrmLenMsec;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetAdoOtpt, &p_ThrdMsgSetAdoOtpt, sizeof( p_ThrdMsgSetAdoOtpt ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptAddStrm p_ThrdMsgAdoOtptAddStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptAddStrm.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptAddStrm, &p_ThrdMsgAdoOtptAddStrm, sizeof( p_ThrdMsgAdoOtptAddStrm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             AddFirstOrLast：[输入]，存放添加到消息队列第一个或最后一个，为0表示第一个，为1表示最后一个。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int AddFirstOrLast, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptDelStrm p_ThrdMsgAdoOtptDelStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptDelStrm.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, AddFirstOrLast, MediaPocsThrd::ThrdMsgTypAdoOtptDelStrm, &p_ThrdMsgAdoOtptDelStrm, sizeof( p_ThrdMsgAdoOtptDelStrm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音频输出设置流要使用Pcm原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUsePcm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetStrmUsePcm p_ThrdMsgAdoOtptSetStrmUsePcm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetStrmUsePcm.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUsePcm, &p_ThrdMsgAdoOtptSetStrmUsePcm, sizeof( p_ThrdMsgAdoOtptSetStrmUsePcm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             IsUsePrcplEnhsmt：[输入]，存放是否使用知觉增强，为非0表示要使用，为0表示不使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUseSpeexDecd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t StrmIdx, int32_t IsUsePrcplEnhsmt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseSpeexDecd p_ThrdMsgAdoOtptSetStrmUseSpeexDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetStrmUseSpeexDecd.m_StrmIdx = StrmIdx;
	p_ThrdMsgAdoOtptSetStrmUseSpeexDecd.m_IsUsePrcplEnhsmt = IsUsePrcplEnhsmt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUseSpeexDecd, &p_ThrdMsgAdoOtptSetStrmUseSpeexDecd, sizeof( p_ThrdMsgAdoOtptSetStrmUseSpeexDecd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmUseOpusDecd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseOpusDecd p_ThrdMsgAdoOtptSetStrmUseOpusDecd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetStrmUseOpusDecd.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUseOpusDecd, &p_ThrdMsgAdoOtptSetStrmUseOpusDecd, sizeof( p_ThrdMsgAdoOtptSetStrmUseOpusDecd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             IsUse：[输入]，存放是否使用流，为0表示不使用，为非0表示要使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t StrmIdx, int32_t IsUse, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetStrmIsUse p_ThrdMsgAdoOtptSetStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetStrmIsUse.m_StrmIdx = StrmIdx;
	p_ThrdMsgAdoOtptSetStrmIsUse.m_IsUse = IsUse;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmIsUse, &p_ThrdMsgAdoOtptSetStrmIsUse, sizeof( p_ThrdMsgAdoOtptSetStrmIsUse ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsDraw：[输入]，存放是否绘制，为非0表示要绘制，为0表示不绘制。
             SrcWndHdl：[输入]，存放原始波形窗口的句柄。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsDrawAdoWavfmToWnd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsDraw, HWND SrcWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( IsDraw != 0 ) && ( SrcWndHdl == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始波形窗口的句柄不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd.m_IsDraw = IsDraw;
	p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd.m_SrcWndHdl = SrcWndHdl;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetIsDrawAdoWavfmToWnd, &p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd, sizeof( p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsSave：[输入]，存放是否保存，为非0表示要使用，为0表示不使用。
             SrcFullPathVstrPt：[输入]，存放原始Wave文件完整路径动态字符串的指针。
             WrBufSzByt：[输入]，存放Wave文件写入缓冲区大小，单位为字节。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsSaveAdoToWaveFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsSave, const Vstr * SrcFullPathVstrPt, size_t WrBufSzByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetIsSaveAdoToWaveFile p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile;
	p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( IsSave != 0 ) && ( SrcFullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "原始Wave文件完整路径动态字符串不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_IsSave = IsSave;
	if( IsSave != 0 )
	{
		if( VstrInit( &p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt, Utf8, , SrcFullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置原始Wave文件完整路径动态字符串失败。" ), , );
			goto Out;
		}
	}
	p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_WrBufSzByt = WrBufSzByt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetIsSaveAdoToWaveFile, &p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile, sizeof( p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt != NULL ) VstrDstoy( p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFile.m_SrcFullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoOtptSetUseDvc
 * 功能说明：媒体处理线程的音频输出设置设置使用的设备。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             NameVstrPt：[输入]，存放名称动态字符串的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetUseDvc p_ThrdMsgAdoOtptSetUseDvc;
	p_ThrdMsgAdoOtptSetUseDvc.m_NameVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( NameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "名称动态字符串的指针不正确。" ), , );
		goto Out;
	}

	if( VstrInit( &p_ThrdMsgAdoOtptSetUseDvc.m_NameVstrPt, Utf8, , NameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置名称动态字符串失败。" ), , );
		goto Out;
	}
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetUseDvc, &p_ThrdMsgAdoOtptSetUseDvc, sizeof( p_ThrdMsgAdoOtptSetUseDvc ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsMute：[输入]，存放是否静音，为0表示有声音，为非0表示静音。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdAdoOtptSetIsMute( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsMute, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAdoOtptSetIsMute p_ThrdMsgAdoOtptSetIsMute;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAdoOtptSetIsMute.m_IsMute = IsMute;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypAdoOtptSetIsMute, &p_ThrdMsgAdoOtptSetIsMute, sizeof( p_ThrdMsgAdoOtptSetIsMute ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程设置视频输入。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             MaxSmplRate：[输入]，存放最大采样频率，取值范围为[1,60]，实际帧率以视频输入设备支持的为准。
             FrmWidth：[输入]，存放帧的宽度，单位为像素，只能为偶数。
             FrmHeight：[输入]，存放帧的高度，单位为像素，只能为偶数。
             SrcMaxSmplRate：[输入]，存放原始的最大采样频率，取值范围为[1,60]，为0表示自动选择。
             SrcFrmWidth：[输入]，存放原始帧的宽度，单位为像素，只能为偶数，为0表示自动选择。
             SrcFrmHeight：[输入]，存放原始帧的高度，单位为像素，只能为偶数，为0表示自动选择。
             PrvwWndHdl：[输入]，存放预览窗口的句柄，可以为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetVdoInpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t MaxSmplRate, int32_t FrmWidth, int32_t FrmHeight, int32_t SrcMaxSmplRate, int32_t SrcFrmWidth, int32_t SrcFrmHeight, HWND PrvwWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoInpt p_ThrdMsgSetVdoInpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( MaxSmplRate < 1 ) || ( MaxSmplRate > 60 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "最大采样频率不正确。" ), , );
		goto Out;
	}
	if( ( FrmWidth <= 0 ) || ( ( FrmWidth & 1 ) != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧宽度不正确。" ), , );
		goto Out;
	}
	if( ( FrmHeight <= 0 ) || ( ( FrmHeight & 1 ) != 0 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "帧高度不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoInpt.m_MaxSmplRate = MaxSmplRate;
	p_ThrdMsgSetVdoInpt.m_FrmWidth = FrmWidth;
	p_ThrdMsgSetVdoInpt.m_FrmHeight = FrmHeight;
	p_ThrdMsgSetVdoInpt.m_SrcMaxSmplRate = SrcMaxSmplRate;
	p_ThrdMsgSetVdoInpt.m_SrcFrmWidth = SrcFrmWidth;
	p_ThrdMsgSetVdoInpt.m_SrcFrmHeight = SrcFrmHeight;
	p_ThrdMsgSetVdoInpt.m_PrvwWndHdl = PrvwWndHdl;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetVdoInpt, &p_ThrdMsgSetVdoInpt, sizeof( p_ThrdMsgSetVdoInpt ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseYu12( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgVdoInptSetUseYu12 p_ThrdMsgVdoInptSetUseYu12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoInptSetUseYu12, &p_ThrdMsgVdoInptSetUseYu12, sizeof( p_ThrdMsgVdoInptSetUseYu12 ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             VdoType：[输入]，存放视频类型，为0表示实时摄像头视频，为1表示实时屏幕内容视频，为2表示非实时摄像头视频，为3表示非实时屏幕内容视频，为4表示其他视频。
             EncdBitrate：[输入]，存放编码后比特率，单位为bps。
             BitrateCtrlMode：[输入]，存放比特率控制模式，为0表示质量优先模式，为1表示比特率优先模式，为2表示缓冲区优先模式，为3表示时间戳优先模式。
             IDRFrmIntvl：[输入]，存放IDR帧间隔帧数，单位为个帧，为0表示仅第一帧为IDR帧，为大于0表示每隔这么帧就至少有一个IDR帧。
             Cmplxt：[输入]，存放复杂度，复杂度越高压缩率不变、CPU使用率越高、画质越好，取值区间为[0,2]。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseOpenH264Encd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t VdoType, int32_t EncdBitrate, int32_t BitrateCtrlMode, int32_t IDRFrmIntvl, int32_t Cmplxt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgVdoInptSetUseOpenH264Encd p_ThrdMsgVdoInptSetUseOpenH264Encd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgVdoInptSetUseOpenH264Encd.m_VdoType = VdoType;
	p_ThrdMsgVdoInptSetUseOpenH264Encd.m_EncdBitrate = EncdBitrate;
	p_ThrdMsgVdoInptSetUseOpenH264Encd.m_BitrateCtrlMode = BitrateCtrlMode;
	p_ThrdMsgVdoInptSetUseOpenH264Encd.m_IDRFrmIntvl = IDRFrmIntvl;
	p_ThrdMsgVdoInptSetUseOpenH264Encd.m_Cmplxt = Cmplxt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoInptSetUseOpenH264Encd, &p_ThrdMsgVdoInptSetUseOpenH264Encd, sizeof( p_ThrdMsgVdoInptSetUseOpenH264Encd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             NameVstrPt：[输入]，存放名称动态字符串的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetUseDvc( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, Vstr * NameVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgVdoInptSetUseDvc p_ThrdMsgVdoInptSetUseDvc;
	p_ThrdMsgVdoInptSetUseDvc.m_NameVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( NameVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "名称动态字符串的指针不正确。" ), , );
		goto Out;
	}

	if( VstrInit( &p_ThrdMsgVdoInptSetUseDvc.m_NameVstrPt, Utf8, , NameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置名称动态字符串失败。" ), , );
		goto Out;
	}
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoInptSetUseDvc, &p_ThrdMsgVdoInptSetUseDvc, sizeof( p_ThrdMsgVdoInptSetUseDvc ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsBlack：[输入]，存放是否黑屏，为0表示有图像，为非0表示黑屏。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoInptSetIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoInptIsBlack p_ThrdMsgSetVdoInptIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoInptIsBlack.m_IsBlack = IsBlack;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoInptSetIsBlack, &p_ThrdMsgSetVdoInptIsBlack, sizeof( p_ThrdMsgSetVdoInptIsBlack ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptAddStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgAddVdoOtptStrm p_ThrdMsgAddVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgAddVdoOtptStrm.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptAddStrm, &p_ThrdMsgAddVdoOtptStrm, sizeof( p_ThrdMsgAddVdoOtptStrm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             AddFirstOrLast：[输入]，存放添加到消息队列第一个或最后一个，为0表示第一个，为1表示最后一个。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptDelStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int AddFirstOrLast, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgDelVdoOtptStrm p_ThrdMsgDelVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgDelVdoOtptStrm.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, AddFirstOrLast, MediaPocsThrd::ThrdMsgTypVdoOtptDelStrm, &p_ThrdMsgDelVdoOtptStrm, sizeof( p_ThrdMsgDelVdoOtptStrm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             DspyWndHdl：[输入]，存放显示窗口的句柄，可以为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrm( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, HWND DspyWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoOtptStrm p_ThrdMsgSetVdoOtptStrm;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoOtptStrm.m_StrmIdx = StrmIdx;
	p_ThrdMsgSetVdoOtptStrm.m_DspyWndHdl = DspyWndHdl;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptSetStrm, &p_ThrdMsgSetVdoOtptStrm, sizeof( p_ThrdMsgSetVdoOtptStrm ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmUseYu12( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseYu12 p_ThrdMsgSetVdoOtptStrmUseYu12;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoOtptStrmUseYu12.m_StrmIdx = StrmIdx;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmUseYu12, &p_ThrdMsgSetVdoOtptStrmUseYu12, sizeof( p_ThrdMsgSetVdoOtptStrmUseYu12 ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             DecdThrdNum：[输入]，存放解码子线程数量，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程数量。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmUseOpenH264Decd( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, int32_t DecdThrdNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseOpenH264Decd p_ThrdMsgSetVdoOtptStrmUseOpenH264Decd;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoOtptStrmUseOpenH264Decd.m_StrmIdx = StrmIdx;
	p_ThrdMsgSetVdoOtptStrmUseOpenH264Decd.m_DecdThrdNum = DecdThrdNum;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmUseOpenH264Decd, &p_ThrdMsgSetVdoOtptStrmUseOpenH264Decd, sizeof( p_ThrdMsgSetVdoOtptStrmUseOpenH264Decd ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             IsBlack：[输入]，存放是否黑屏，为0表示有图像，为非0表示黑屏。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmIsBlack( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, int32_t IsBlack, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsBlack p_ThrdMsgSetVdoOtptStrmIsBlack;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoOtptStrmIsBlack.m_StrmIdx = StrmIdx;
	p_ThrdMsgSetVdoOtptStrmIsBlack.m_IsBlack = IsBlack;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmIsBlack, &p_ThrdMsgSetVdoOtptStrmIsBlack, sizeof( p_ThrdMsgSetVdoOtptStrmIsBlack ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             StrmIdx：[输入]，存放流索引。
             IsUse：[输入]，存放是否使用，为0表示不使用，为非0表示要使用。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdVdoOtptSetStrmIsUse( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, uint32_t StrmIdx, int32_t IsUse, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsUse p_ThrdMsgSetVdoOtptStrmIsUse;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetVdoOtptStrmIsUse.m_StrmIdx = StrmIdx;
	p_ThrdMsgSetVdoOtptStrmIsUse.m_IsUse = IsUse;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmIsUse, &p_ThrdMsgSetVdoOtptStrmIsUse, sizeof( p_ThrdMsgSetVdoOtptStrmIsUse ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
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

int MediaPocsThrdSetIsUseAdoVdoInptOtpt( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUseAdoInpt, int32_t IsUseAdoOtpt, int32_t IsUseVdoInpt, int32_t IsUseVdoOtpt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetIsUseAdoVdoInptOtpt p_ThrdMsgSetIsUseAdoVdoInptOtpt;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoInpt = IsUseAdoInpt;
	p_ThrdMsgSetIsUseAdoVdoInptOtpt.m_IsUseAdoOtpt = IsUseAdoOtpt;
	p_ThrdMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoInpt = IsUseVdoInpt;
	p_ThrdMsgSetIsUseAdoVdoInptOtpt.m_IsUseVdoOtpt = IsUseVdoOtpt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetIsUseAdoVdoInptOtpt, &p_ThrdMsgSetIsUseAdoVdoInptOtpt, sizeof( p_ThrdMsgSetIsUseAdoVdoInptOtpt ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             IsPrintLog：[输入]，存放是否打印Log日志，为非0表示要打印，为0表示不打印。
             IsShowToast：[输入]，存放是否显示Toast，为非0表示要显示，为0表示不显示。
             ShowToastWndHdl：[输入]，存放显示Toast窗口的句柄，为NULL表示桌面窗口。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSetIsUsePrvntSysSleep( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetIsUsePrvntSysSleep p_ThrdMsgSetIsUsePrvntSysSleep;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgSetIsUsePrvntSysSleep.m_IsUsePrvntSysSleep = IsUsePrvntSysSleep;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetIsUsePrvntSysSleep, &p_ThrdMsgSetIsUsePrvntSysSleep, sizeof( p_ThrdMsgSetIsUsePrvntSysSleep ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             FullPathVstrPt：[输入]，存放完整路径动态字符串的指针。
             WrBufSzByt：[输入]，存放写入缓冲区大小，单位为字节。
             MaxStrmNum：[输入]，存放最大流数量，取值区间为[1,100]。
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

int MediaPocsThrdSetIsSaveAdoVdoInptOtptToAviFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, const Vstr * FullPathVstrPt, size_t WrBufSzByt, int32_t MaxStrmNum, int32_t IsSaveAdoInpt, int32_t IsSaveAdoOtpt, int32_t IsSaveVdoInpt, int32_t IsSaveVdoOtpt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) ) && ( FullPathVstrPt == NULL ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "完整路径动态字符串不正确。" ), , );
		goto Out;
	}
	
	if( ( IsSaveAdoInpt != 0 ) || ( IsSaveAdoOtpt != 0 ) || ( IsSaveVdoInpt != 0 ) || ( IsSaveVdoOtpt != 0 ) )
	{
		if( VstrInit( &p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt, Utf8, , FullPathVstrPt ) != 0 )
		{
			VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置完整路径动态字符串失败。" ), , );
			goto Out;
		}
	}
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_WrBufSzByt = WrBufSzByt;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_MaxStrmNum = MaxStrmNum;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveAdoInpt = IsSaveAdoInpt;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveAdoOtpt = IsSaveAdoOtpt;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveVdoInpt = IsSaveVdoInpt;
	p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_IsSaveVdoOtpt = IsSaveVdoOtpt;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSetIsSaveAdoVdoInptOtptToAviFile, &p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile, sizeof( p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt != NULL ) VstrDstoy( p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile.m_FullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdSaveStsToTxtFile
 * 功能说明：媒体处理线程的保存状态到Txt文件。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             FullPathVstrPt：[输入]，存放完整路径动态字符串的指针，不能为NULL。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSaveStsToTxtFile( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, const Vstr * FullPathVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgSaveStsToTxtFile p_ThrdMsgSaveStsToTxtFile;
	p_ThrdMsgSaveStsToTxtFile.m_FullPathVstrPt = NULL;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( FullPathVstrPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "完整路径动态字符串的指针不正确。" ), , );
		goto Out;
	}
	
	if( VstrInit( &p_ThrdMsgSaveStsToTxtFile.m_FullPathVstrPt, Utf8, , FullPathVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置完整路径动态字符串失败。" ), , );
		goto Out;
	}
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypSaveStsToTxtFile, &p_ThrdMsgSaveStsToTxtFile, sizeof( p_ThrdMsgSaveStsToTxtFile ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_ThrdMsgSaveStsToTxtFile.m_FullPathVstrPt != NULL ) VstrDstoy( p_ThrdMsgSaveStsToTxtFile.m_FullPathVstrPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdPrvntSysSleepInitOrDstoy
 * 功能说明：媒体处理线程的阻止系统睡眠初始化或销毁。
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
* 功能说明：媒体处理线程启动。
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
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程已经启动。" ), , );
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
* 功能说明：媒体处理线程请求退出。
* 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
			IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
			ExitFlag：[输入]，存放退出标记，为0表示保持运行，为1表示请求退出，为2表示请求重启，为3表示请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
            ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
* 返回说明：0：成功。
			非0：失败。
* 线程安全：是 或 否
* 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdRqirExit( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, int ExitFlag, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	MediaPocsThrd::ThrdMsgRqirExit p_ThrdMsgRqirExit;

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	if( ( ExitFlag < 0 ) || ( ExitFlag > 3 ) )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "退出标记不正确。" ), , );
		goto Out;
	}
	
	p_ThrdMsgRqirExit.m_ExitFlag = ExitFlag;
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypRqirExit, &p_ThrdMsgRqirExit, sizeof( p_ThrdMsgRqirExit ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 函数名称：MediaPocsThrdSendUserMsg
 * 功能说明：发送用户消息到媒体处理线程。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsBlockWait：[输入]，存放是否阻塞等待，为0表示不阻塞，为非0表示要阻塞。
             MsgTyp：[输入]，存放消息类型。
             MsgParmPt：[输入]，存放消息参数的指针。如果没有消息参数，则本参数为NULL。
             MsgParmLenByt：[输入]，存放消息参数的长度，单位为字节。如果没有消息参数，则本参数为0。
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdSendUserMsg( MediaPocsThrd * MediaPocsThrdPt, int IsBlockWait, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( MediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, MediaPocsThrd::ThrdMsgTypUserMsgMinVal + MsgTyp, MsgParmPt, MsgParmLenByt, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
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
 * 功能说明：媒体处理线程的音视频输入输出Avi文件写入器初始化。
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
			if( AviFileWriterInit( MediaPocsThrdPt->m_LicnCodePt, &MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_MaxStrmNum, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件 %vs Avi文件写入器成功。" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件 %vs Avi文件写入器失败。原因：%vs" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			uint64_t p_AdoVdoStartTimeStamp = FuncGetTickAsMsec();
			AviFileWriterSetStartTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, p_AdoVdoStartTimeStamp, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件时间线起始时间戳为 %uz64d 。" ), p_AdoVdoStartTimeStamp );
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx = UINT32_MAX;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt = new std::map< uint32_t, uint32_t >;
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt == NULL )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音视频输入输出Avi文件视频输出已编码格式原始流索引映射失败。原因：内存不足。" ) );
				goto Out;
			}
		}

		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt != 0 ) //如果要保存音频输入。
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
		}

		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt != 0 ) //如果要保存音频输出。
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
 * 功能说明：媒体处理线程的音视频输入输出Avi文件写入器销毁。
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
 * 功能说明：媒体处理线程的临时变量初始化。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdTmpVarInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	MediaPocsThrdPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程临时变量。
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
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入Pcm格式结果帧内存块失败。" ) );
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
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入Pcm格式临时帧内存块失败。" ) );
				goto Out;
			}
		}
		MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt = NULL;
		MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 1; //设置音频输入Pcm格式结果帧语音活动状态为1，为了让在不使用语音活动检测情况下永远都是有语音活动。
		if( MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd != 0 ) //如果音频输入不使用Pcm原始数据。
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
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输入已编码格式结果帧内存块失败。" ) );
					goto Out;
				}
			}
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmLenByt = 0;
			MediaPocsThrdPt->m_Thrd.m_AdoInptEncdRsltFrmIsNeedTrans = 1; //设置音频输入已编码格式结果帧是否需要传输为1，为了让在不使用非连续传输情况下永远都是需要传输。
		}
		else //如果音频输入要使用Pcm原始数据。
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
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化媒体处理线程临时变量成功。" ) );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdTmpVarDstoy
 * 功能说明：媒体处理线程的临时变量销毁。
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
		MediaPocsThrdPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程临时变量。
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
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁媒体处理线程临时变量成功。" ) );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoVdoInptOtptInit
 * 功能说明：媒体处理线程的音视频输入输出初始化。
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
 * 功能说明：媒体处理线程的音视频输入输出销毁。
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
 * 函数名称：MediaPocsThrdThrdMsgPocs
 * 功能说明：媒体处理线程的线程消息处理。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int MediaPocsThrdThrdMsgPocs( MsgQueue * MsgQueuePt, MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为1表示线程消息容器为空，为-1表示失败。
	int32_t p_TmpInt32;

	//因为IGlobalInterfaceTable::GetInterfaceFromGlobal会向创建此Com对象单元套间线程发送消息，所以要求创建此Com对象单元套间线程有消息处理，若没有，则IGlobalInterfaceTable::GetInterfaceFromGlobal会一直阻塞。
	/*{
		MSG msg;
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}*/

	switch( MsgTyp )
	{
		case MediaPocsThrd::ThrdMsgTypSetAdoInpt:
		{
			MediaPocsThrd::ThrdMsgSetAdoInpt * p_ThrdMsgSetAdoInptPt = ( MediaPocsThrd::ThrdMsgSetAdoInpt * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptDstoy( &MediaPocsThrdPt->m_AdoInpt );
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_SmplRate = p_ThrdMsgSetAdoInptPt->m_SmplRate;
			MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec = p_ThrdMsgSetAdoInptPt->m_FrmLenMsec;
			MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoInpt.m_SmplRate / 1000;
			MediaPocsThrdPt->m_AdoInpt.m_FrmLenData = MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit * 1;
			MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoInpt.m_FrmLenData * sizeof( int16_t );

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
				{
					if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
				}
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
				if( AdoInptInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
				if( MediaPocsThrdTmpVarInit( MediaPocsThrdPt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetIsUseSystemAecNsAgc:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetIsUseSystemAecNsAgc * p_ThrdMsgAdoInptSetIsUseSystemAecNsAgcPt = ( MediaPocsThrd::ThrdMsgAdoInptSetIsUseSystemAecNsAgc * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_IsUseSystemAecNsAgc = p_ThrdMsgAdoInptSetIsUseSystemAecNsAgcPt->m_IsUseSystemAecNsAgc;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
				{
					if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
				}
				if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseNoAec:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseNoAec * p_ThrdMsgAdoInptSetUseNoAecPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseNoAec * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 0;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexAec:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexAec * p_ThrdMsgAdoInptSetUseSpeexAecPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexAec * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 1;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_FilterLenMsec = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_FilterLenMsec;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsUseRec = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_IsUseRec;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoMutp = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_EchoMutp;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoCntu = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_EchoCntu;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupes = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_EchoSupes;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupesAct = p_ThrdMsgAdoInptSetUseSpeexAecPt->m_EchoSupesAct;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAecm:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAecm * p_ThrdMsgAdoInptSetUseWebRtcAecmPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAecm * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 2;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode = p_ThrdMsgAdoInptSetUseWebRtcAecmPt->m_IsUseCNGMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_EchoMode = p_ThrdMsgAdoInptSetUseWebRtcAecmPt->m_EchoMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Delay = p_ThrdMsgAdoInptSetUseWebRtcAecmPt->m_Delay;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAec:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec * p_ThrdMsgAdoInptSetUseWebRtcAecPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 3;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_EchoMode = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_EchoMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Delay = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_Delay;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_IsUseDelayAgstcMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_IsUseExtdFilterMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_IsUseRefinedFilterAdaptAecMode;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay = p_ThrdMsgAdoInptSetUseWebRtcAecPt->m_IsUseAdaptAdjDelay;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcAec3:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec3 * p_ThrdMsgAdoInptSetUseWebRtcAec3Pt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcAec3 * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 4;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Delay = p_ThrdMsgAdoInptSetUseWebRtcAec3Pt->m_Delay;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexWebRtcAec:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexWebRtcAec * p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexWebRtcAec * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptAecDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec = 5;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WorkMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecFilterLenMsec;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecIsUseRec;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoMutp;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoCntu;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoSupes;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SpeexAecEchoSupesAct;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmIsUseCNGMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmEchoMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecmDelay;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecEchoMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecDelay;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseDelayAgstcMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseExtdFilterMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAecIsUseAdaptAdjDelay;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAec3Delay = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_WebRtcAec3Delay;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_IsUseSameRoomAec;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay = p_ThrdMsgAdoInptSetUseSpeexWebRtcAecPt->m_SameRoomEchoMinDelay;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptAecInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseNoNs:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseNoNs * p_ThrdMsgAdoInptSetUseNoNsPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseNoNs * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 0;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexPrpocsNs:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexPrpocsNs * p_ThrdMsgAdoInptSetUseSpeexPrpocsNsPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexPrpocsNs * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 1;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs = p_ThrdMsgAdoInptSetUseSpeexPrpocsNsPt->m_IsUseNs;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes = p_ThrdMsgAdoInptSetUseSpeexPrpocsNsPt->m_NoiseSupes;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb = p_ThrdMsgAdoInptSetUseSpeexPrpocsNsPt->m_IsUseDereverb;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcNsx:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNsx * p_ThrdMsgAdoInptSetUseWebRtcNsxPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNsx * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 2;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsx.m_PolicyMode = p_ThrdMsgAdoInptSetUseWebRtcNsxPt->m_PolicyMode;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseWebRtcNs:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNs * p_ThrdMsgAdoInptSetUseWebRtcNsPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseWebRtcNs * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 3;
			MediaPocsThrdPt->m_AdoInpt.m_WebRtcNs.m_PolicyMode = p_ThrdMsgAdoInptSetUseWebRtcNsPt->m_PolicyMode;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseRNNoise:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseRNNoise * p_ThrdMsgAdoInptSetUseRNNoisePt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseRNNoise * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptNsDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );
			}

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs = 4;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptNsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetIsUseSpeexPrpocs:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetIsUseSpeexPrpocs * p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt = ( MediaPocsThrd::ThrdMsgAdoInptSetIsUseSpeexPrpocs * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptSpeexPrpocsDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseSpeexPrpocs;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseVad = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseVad;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbStart = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_VadProbStart;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_VadProbCntu;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_IsUseAgc;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcLevel = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcLevel;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcIncrement;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcDecrement;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain = p_ThrdMsgAdoInptSetIsUseSpeexPrpocsPt->m_AgcMaxGain;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( AdoInptSpeexPrpocsInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmVoiceActSts = 1;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUsePcm:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUsePcm * p_ThrdMsgAdoInptSetUsePcmPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUsePcm * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 0;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseSpeexEncd:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexEncd * p_ThrdMsgAdoInptSetUseSpeexEncdPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseSpeexEncd * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 1;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr = p_ThrdMsgAdoInptSetUseSpeexEncdPt->m_UseCbrOrVbr;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Qualt = p_ThrdMsgAdoInptSetUseSpeexEncdPt->m_Qualt;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Cmplxt = p_ThrdMsgAdoInptSetUseSpeexEncdPt->m_Cmplxt;
			MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate = p_ThrdMsgAdoInptSetUseSpeexEncdPt->m_PlcExptLossRate;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseOpusEncd:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseOpusEncd * p_ThrdMsgAdoInptSetUseOpusEncdPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseOpusEncd * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptEncdDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd = 2;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptEncdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			MediaPocsThrdTmpVarInit( MediaPocsThrdPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetIsSaveAdoToWaveFile:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetIsSaveAdoToWaveFile * p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt = ( MediaPocsThrd::ThrdMsgAdoInptSetIsSaveAdoToWaveFile * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_IsSave = p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_IsSave;
			if( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt != NULL )
			{
				VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt, p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt, , );
				VstrDstoy( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt );
			}
			if( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltFullPathVstrPt != NULL )
			{
				VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt, p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltFullPathVstrPt, , );
				VstrDstoy( p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_RsltFullPathVstrPt );
			}
			MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_WrBufSzByt = p_ThrdMsgAdoInptSetIsSaveAdoToWaveFilePt->m_WrBufSzByt;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetIsDrawAdoWavfmToWnd:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd * p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWndPt = ( MediaPocsThrd::ThrdMsgAdoInptSetIsDrawAdoWavfmToWnd * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptWavfmDstoy( &MediaPocsThrdPt->m_AdoInpt );

			MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDraw = p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_IsDraw;
			MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWndHdl = p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_SrcWndHdl;
			MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWndHdl = p_ThrdMsgAdoInptSetIsDrawAdoWavfmToWndPt->m_RsltWndHdl;

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) if( AdoInptWavfmInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetUseDvc:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetUseDvc * p_ThrdMsgAdoInptSetUseDvcPt = ( MediaPocsThrd::ThrdMsgAdoInptSetUseDvc * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			}

			VstrCpy( MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt, p_ThrdMsgAdoInptSetUseDvcPt->m_NameVstrPt, , );
			VstrDstoy( p_ThrdMsgAdoInptSetUseDvcPt->m_NameVstrPt );

			if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
			{
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
				{
					if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
				}
				if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptSetIsMute:
		{
			MediaPocsThrd::ThrdMsgAdoInptSetIsMute * p_ThrdMsgAdoInptSetIsMutePt = ( MediaPocsThrd::ThrdMsgAdoInptSetIsMute * )MsgParmPt;

			MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsMute = p_ThrdMsgAdoInptSetIsMutePt->m_IsMute;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSetAdoOtpt:
		{
			MediaPocsThrd::ThrdMsgSetAdoOtpt * p_ThrdMsgSetAdoOtptPt = ( MediaPocsThrd::ThrdMsgSetAdoOtpt * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
			{
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoOtptDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			}

			MediaPocsThrdPt->m_AdoOtpt.m_SmplRate = p_ThrdMsgSetAdoOtptPt->m_SmplRate;
			MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec = p_ThrdMsgSetAdoOtptPt->m_FrmLenMsec;
			MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec * MediaPocsThrdPt->m_AdoOtpt.m_SmplRate / 1000;
			MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit * 1;
			MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt = MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData * sizeof( int16_t );

			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
			{
				if( AdoOtptInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
				{
					AdoInptSetIsCanUseAec( &MediaPocsThrdPt->m_AdoInpt );
					if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
				}
				else
				{
					MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
					MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
				}
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptAddStrm:
		{
			MediaPocsThrd::ThrdMsgAdoOtptAddStrm * p_ThrdMsgAdoOtptAddStrmPt = ( MediaPocsThrd::ThrdMsgAdoOtptAddStrm * )MsgParmPt;

			AdoOtptAddStrm( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptAddStrmPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptDelStrm:
		{
			MediaPocsThrd::ThrdMsgAdoOtptDelStrm * p_ThrdMsgAdoOtptDelStrmPt = ( MediaPocsThrd::ThrdMsgAdoOtptDelStrm * )MsgParmPt;

			AdoOtptDelStrm( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptDelStrmPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUsePcm:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetStrmUsePcm * p_ThrdMsgAdoOtptSetStrmUsePcmPt = ( MediaPocsThrd::ThrdMsgAdoOtptSetStrmUsePcm * )MsgParmPt;

			AdoOtptSetStrmUsePcm( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptSetStrmUsePcmPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUseSpeexDecd:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseSpeexDecd * p_ThrdMsgAdoOtptSetStrmUseSpeexDecdPt = ( MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseSpeexDecd * )MsgParmPt;

			AdoOtptSetStrmUseSpeexDecd( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptSetStrmUseSpeexDecdPt->m_StrmIdx, p_ThrdMsgAdoOtptSetStrmUseSpeexDecdPt->m_IsUsePrcplEnhsmt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmUseOpusDecd:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseOpusDecd * p_ThrdMsgAdoOtptSetStrmUseOpusDecdPt = ( MediaPocsThrd::ThrdMsgAdoOtptSetStrmUseOpusDecd * )MsgParmPt;

			AdoOtptSetStrmUseOpusDecd( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptSetStrmUseOpusDecdPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetStrmIsUse:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetStrmIsUse * p_ThrdMsgAdoOtptSetStrmIsUsePt = ( MediaPocsThrd::ThrdMsgAdoOtptSetStrmIsUse * )MsgParmPt;

			AdoOtptSetStrmIsUse( &MediaPocsThrdPt->m_AdoOtpt, p_ThrdMsgAdoOtptSetStrmIsUsePt->m_StrmIdx, p_ThrdMsgAdoOtptSetStrmIsUsePt->m_IsUse );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetIsSaveAdoToWaveFile:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetIsSaveAdoToWaveFile * p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt = ( MediaPocsThrd::ThrdMsgAdoOtptSetIsSaveAdoToWaveFile * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptWaveFileWriterDstoy( &MediaPocsThrdPt->m_AdoOtpt );

			MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_IsSave = p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_IsSave;
			if( p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt != NULL )
			{
				VstrCpy( MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt, p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt, , );
				VstrDstoy( p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_SrcFullPathVstrPt );
			}
			MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_WrBufSzByt = p_ThrdMsgAdoOtptSetIsSaveAdoToWaveFilePt->m_WrBufSzByt;

			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptWaveFileWriterInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetIsDrawAdoWavfmToWnd:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd * p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWndPt = ( MediaPocsThrd::ThrdMsgAdoOtptSetIsDrawAdoWavfmToWnd * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) AdoOtptWavfmDstoy( &MediaPocsThrdPt->m_AdoOtpt );

			MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDraw = p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWndPt->m_IsDraw;
			MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWndHdl = p_ThrdMsgAdoOtptSetIsDrawAdoWavfmToWndPt->m_SrcWndHdl;

			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) if( AdoOtptWavfmInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetUseDvc:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetUseDvc * p_ThrdMsgAdoOtptSetUseDvcPt = ( MediaPocsThrd::ThrdMsgAdoOtptSetUseDvc * )MsgParmPt;
			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
			{
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 ) AdoInptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoInpt );
				AdoOtptDvcAndThrdDstoy( &MediaPocsThrdPt->m_AdoOtpt );
			}
			
			VstrCpy( MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt, p_ThrdMsgAdoOtptSetUseDvcPt->m_NameVstrPt, , );
			VstrDstoy( p_ThrdMsgAdoOtptSetUseDvcPt->m_NameVstrPt );

			if( MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 )
			{
				if( AdoOtptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoOtpt ) != 0 ) goto Out;
				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输出流时间戳要重置。
				if( MediaPocsThrdPt->m_AdoInpt.m_IsInit != 0 )
				{
					if( AdoInptDvcAndThrdInit( &MediaPocsThrdPt->m_AdoInpt ) != 0 ) goto Out;
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 1; //设置音视频输入输出Avi文件音频输入流时间戳要重置。
				}
				else
				{
					MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
					MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
				}
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptSetIsMute:
		{
			MediaPocsThrd::ThrdMsgAdoOtptSetIsMute * p_ThrdMsgAdoOtptSetIsMutePt = ( MediaPocsThrd::ThrdMsgAdoOtptSetIsMute * )MsgParmPt;

			MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_IsMute = p_ThrdMsgAdoOtptSetIsMutePt->m_IsMute;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSetVdoInpt:
		{
			MediaPocsThrd::ThrdMsgSetVdoInpt * p_ThrdMsgSetVdoInptPt = ( MediaPocsThrd::ThrdMsgSetVdoInpt * )MsgParmPt;
			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

			MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate = p_ThrdMsgSetVdoInptPt->m_MaxSmplRate;
			MediaPocsThrdPt->m_VdoInpt.m_FrmWidth = p_ThrdMsgSetVdoInptPt->m_FrmWidth;
			MediaPocsThrdPt->m_VdoInpt.m_FrmHeight = p_ThrdMsgSetVdoInptPt->m_FrmHeight;
			MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt = p_ThrdMsgSetVdoInptPt->m_FrmWidth * p_ThrdMsgSetVdoInptPt->m_FrmHeight * 3 / 2;
			MediaPocsThrdPt->m_VdoInpt.m_SrcMaxSmplRate = p_ThrdMsgSetVdoInptPt->m_SrcMaxSmplRate;
			MediaPocsThrdPt->m_VdoInpt.m_SrcFrmWidth = p_ThrdMsgSetVdoInptPt->m_SrcFrmWidth;
			MediaPocsThrdPt->m_VdoInpt.m_SrcFrmHeight = p_ThrdMsgSetVdoInptPt->m_SrcFrmHeight;
			MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl = p_ThrdMsgSetVdoInptPt->m_PrvwWndHdl;

			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptSetUseYu12:
		{
			MediaPocsThrd::ThrdMsgVdoInptSetUseYu12 * p_ThrdMsgVdoInptSetUseYu12Pt = ( MediaPocsThrd::ThrdMsgVdoInptSetUseYu12 * )MsgParmPt;
			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

			MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 0;

			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptSetUseOpenH264Encd:
		{
			MediaPocsThrd::ThrdMsgVdoInptSetUseOpenH264Encd * p_ThrdMsgVdoInptSetUseOpenH264EncdPt = ( MediaPocsThrd::ThrdMsgVdoInptSetUseOpenH264Encd * )MsgParmPt;
			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

			MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd = 1;
			MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_VdoType = p_ThrdMsgVdoInptSetUseOpenH264EncdPt->m_VdoType;
			MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_EncdBitrate = p_ThrdMsgVdoInptSetUseOpenH264EncdPt->m_EncdBitrate;
			MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode = p_ThrdMsgVdoInptSetUseOpenH264EncdPt->m_BitrateCtrlMode;
			MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl = p_ThrdMsgVdoInptSetUseOpenH264EncdPt->m_IDRFrmIntvl;
			MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_Cmplxt = p_ThrdMsgVdoInptSetUseOpenH264EncdPt->m_Cmplxt;

			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptSetUseDvc:
		{
			MediaPocsThrd::ThrdMsgVdoInptSetUseDvc * p_ThrdMsgVdoInptSetUseDvcPt = ( MediaPocsThrd::ThrdMsgVdoInptSetUseDvc * )MsgParmPt;
			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) VdoInptDstoy( &MediaPocsThrdPt->m_VdoInpt );

			VstrCpy( MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt, p_ThrdMsgVdoInptSetUseDvcPt->m_NameVstrPt, , );
			VstrDstoy( p_ThrdMsgVdoInptSetUseDvcPt->m_NameVstrPt );

			if( MediaPocsThrdPt->m_VdoInpt.m_IsInit != 0 ) if( VdoInptInit( &MediaPocsThrdPt->m_VdoInpt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptSetIsBlack:
		{
			MediaPocsThrd::ThrdMsgSetVdoInptIsBlack * p_ThrdMsgSetVdoInptIsBlackPt = ( MediaPocsThrd::ThrdMsgSetVdoInptIsBlack * )MsgParmPt;

			MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_IsBlack = p_ThrdMsgSetVdoInptIsBlackPt->m_IsBlack;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptAddStrm:
		{
			MediaPocsThrd::ThrdMsgAddVdoOtptStrm * p_ThrdMsgAddVdoOtptStrmPt = ( MediaPocsThrd::ThrdMsgAddVdoOtptStrm * )MsgParmPt;

			VdoOtptAddStrm( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgAddVdoOtptStrmPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptDelStrm:
		{
			MediaPocsThrd::ThrdMsgDelVdoOtptStrm * p_ThrdMsgDelVdoOtptStrmPt = ( MediaPocsThrd::ThrdMsgDelVdoOtptStrm * )MsgParmPt;

			VdoOtptDelStrm( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgDelVdoOtptStrmPt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptSetStrm:
		{
			MediaPocsThrd::ThrdMsgSetVdoOtptStrm * p_ThrdMsgSetVdoOtptStrmPt = ( MediaPocsThrd::ThrdMsgSetVdoOtptStrm * )MsgParmPt;

			VdoOtptSetStrm( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgSetVdoOtptStrmPt->m_StrmIdx, p_ThrdMsgSetVdoOtptStrmPt->m_DspyWndHdl );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmUseYu12:
		{
			MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseYu12 * p_ThrdMsgSetVdoOtptStrmUseYu12Pt = ( MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseYu12 * )MsgParmPt;

			VdoOtptSetStrmUseYu12( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgSetVdoOtptStrmUseYu12Pt->m_StrmIdx );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmUseOpenH264Decd:
		{
			MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseOpenH264Decd * p_ThrdMsgSetVdoOtptStrmUseOpenH264DecdPt = ( MediaPocsThrd::ThrdMsgSetVdoOtptStrmUseOpenH264Decd * )MsgParmPt;

			VdoOtptSetStrmUseOpenH264Decd( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgSetVdoOtptStrmUseOpenH264DecdPt->m_StrmIdx, p_ThrdMsgSetVdoOtptStrmUseOpenH264DecdPt->m_DecdThrdNum );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmIsBlack:
		{
			MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsBlack * p_ThrdMsgSetVdoOtptStrmIsBlackPt = ( MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsBlack * )MsgParmPt;

			VdoOtptSetStrmIsBlack( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgSetVdoOtptStrmIsBlackPt->m_StrmIdx, p_ThrdMsgSetVdoOtptStrmIsBlackPt->m_IsBlack );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoOtptSetStrmIsUse:
		{
			MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsUse * p_ThrdMsgSetVdoOtptStrmIsUsePt = ( MediaPocsThrd::ThrdMsgSetVdoOtptStrmIsUse * )MsgParmPt;

			VdoOtptSetStrmIsUse( &MediaPocsThrdPt->m_VdoOtpt, p_ThrdMsgSetVdoOtptStrmIsUsePt->m_StrmIdx, p_ThrdMsgSetVdoOtptStrmIsUsePt->m_IsUse );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSetIsUseAdoVdoInptOtpt:
		{
			MediaPocsThrd::ThrdMsgSetIsUseAdoVdoInptOtpt * p_ThrdMsgSetIsUseAdoVdoInptOtptPt = ( MediaPocsThrd::ThrdMsgSetIsUseAdoVdoInptOtpt * )MsgParmPt;

			if( p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt >= 0 ) MediaPocsThrdPt->m_AdoInpt.m_IsUse = p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoInpt;
			if( p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt >= 0 ) MediaPocsThrdPt->m_AdoOtpt.m_IsUse = p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseAdoOtpt;
			if( p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt >= 0 ) MediaPocsThrdPt->m_VdoInpt.m_IsUse = p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoInpt;
			if( p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt >= 0 ) MediaPocsThrdPt->m_VdoOtpt.m_IsUse = p_ThrdMsgSetIsUseAdoVdoInptOtptPt->m_IsUseVdoOtpt;

			MediaPocsThrd::ThrdMsgAdoVdoInptOtptInit p_ThrdMsgSetIsUseAdoVdoInptOtpt;
			if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptInit, &p_ThrdMsgSetIsUseAdoVdoInptOtpt, sizeof( p_ThrdMsgSetIsUseAdoVdoInptOtpt ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSetIsUsePrvntSysSleep:
		{
			MediaPocsThrd::ThrdMsgSetIsUsePrvntSysSleep * p_ThrdMsgSetIsUsePrvntSysSleepPt = ( MediaPocsThrd::ThrdMsgSetIsUsePrvntSysSleep * )MsgParmPt;

			MediaPocsThrdPt->m_IsUsePrvntSysSleep = p_ThrdMsgSetIsUsePrvntSysSleepPt->m_IsUsePrvntSysSleep;
			MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, MediaPocsThrdPt->m_IsUsePrvntSysSleep );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSetIsSaveAdoVdoInptOtptToAviFile:
		{
			MediaPocsThrd::ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile * p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt = ( MediaPocsThrd::ThrdMsgSetIsSaveAdoVdoInptOtptToAviFile * )MsgParmPt;
			int p_CmpRslt;
			if( ( VstrCmp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, , , p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt, , &p_CmpRslt ), p_CmpRslt != 0 ) || //当完整路径字符串或写入缓冲区的大小或最大流数量有修改时，才销毁。
				( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt != p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_WrBufSzByt ) )
				MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy( MediaPocsThrdPt );

			if( p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt != NULL )
			{
				VstrCpy( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt, p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt, , );
				VstrDstoy( p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_FullPathVstrPt );
			}
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_WrBufSzByt;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_MaxStrmNum = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_MaxStrmNum;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveAdoInpt;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveAdoOtpt;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveVdoInpt;
			MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt = p_ThrdMsgSetIsSaveAdoVdoInptOtptToAviFilePt->m_IsSaveVdoOtpt;

			if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) if( MediaPocsThrdAdoVdoInptOtptAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto Out;
			break;
		}
		case MediaPocsThrd::ThrdMsgTypSaveStsToTxtFile:
		{
			MediaPocsThrd::ThrdMsgSaveStsToTxtFile * p_ThrdMsgSaveStsToTxtFilePt = ( MediaPocsThrd::ThrdMsgSaveStsToTxtFile * )MsgParmPt;
			File * p_StsFilePt;

			if( FileInitByPath( &p_StsFilePt, p_ThrdMsgSaveStsToTxtFilePt->m_FullPathVstrPt, NoRd_Wr, Create_AlExist_Clr, 8192, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化状态文件 %vs 失败。原因：%vs" ), p_ThrdMsgSaveStsToTxtFilePt->m_FullPathVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt );
				VstrDstoy( p_ThrdMsgSaveStsToTxtFilePt->m_FullPathVstrPt );
				goto Out;
			}

			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_IsPrintLog：%z32d\n" ), MediaPocsThrdPt->m_IsPrintLog );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_IsShowToast：%z32d\n" ), MediaPocsThrdPt->m_IsShowToast );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_ShowToastWndHdl：%#P\n" ), MediaPocsThrdPt->m_ShowToastWndHdl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_IsUsePrvntSysSleep：%z32d\n" ), MediaPocsThrdPt->m_IsUsePrvntSysSleep );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_FullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_FullPathVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_WrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WrBufSzByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_MaxStrmNum：%uzd\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_MaxStrmNum );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt：%z32d\n" ), MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoOtpt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );

			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsUse );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_IsInit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SmplRate );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenMsec：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenMsec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenUnit：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenData：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenData );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatAec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_FilterLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_FilterLenMsec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_IsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_IsUseRec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoMutp );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoCntu );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupes );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexAec.m_EchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexAec.m_EchoSupesAct );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_IsUseCNGMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_EchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_EchoMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAecm.m_Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Delay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_EchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_EchoMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Delay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseExtdFilterMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseAdaptAdjDelay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcAec3.m_Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Delay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WorkMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecIsUseRec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoMutp );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu：%f\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoCntu );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupes );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmEchoMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecmDelay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecEchoMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecDelay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAec3Delay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAec3Delay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_IsUseSameRoomAec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_SameRoomEchoMinDelay );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatNs );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseNs );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_NoiseSupes );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocsNs.m_IsUseDereverb );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNsx.m_PolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNsx.m_PolicyMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WebRtcNs.m_PolicyMode：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WebRtcNs.m_PolicyMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseSpeexPrpocs );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseVad：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseVad );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_VadProbStart：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbStart );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_VadProbCntu );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_IsUseAgc );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcLevel：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcLevel );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcIncrement );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcDecrement );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexPrpocs.m_AgcMaxGain );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_UseWhatEncd );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_UseCbrOrVbr );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_Qualt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Qualt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_Cmplxt：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_Cmplxt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_SpeexEncd.m_PlcExptLossRate );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_IsDraw：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDraw );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_SrcWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWndHdl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Wavfm.m_RsltWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWndHdl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFileWriter.m_IsSave：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_IsSave );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcFullPathVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltFullPathVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_WaveFileWriter.m_WrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_WrBufSzByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Dvc.m_NameVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoInpt.m_Dvc.m_IsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_IsMute );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );

			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsUse );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_IsInit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_SmplRate：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_SmplRate );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenMsec：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenUnit：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenData：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenData );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_StrmCntnr.m_Pt：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.m_Pt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			{
				AdoOtpt::Strm * p_AdoOtptStrmPt; //存放音频输出流的指针。
				for( size_t p_StrmNum = 0; MediaPocsThrdPt->m_AdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_AdoOtptStrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
				{
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_AdoOtptStrmPt：%#P\n" ), p_AdoOtptStrmPt );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_AdoOtptStrmPt->m_Idx：%z32d\n" ), p_AdoOtptStrmPt->m_Idx );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_AdoOtptStrmPt->m_IsUse：%z32d\n" ), p_AdoOtptStrmPt->m_IsUse );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_AdoOtptStrmPt->m_UseWhatDecd：%z32d\n" ), p_AdoOtptStrmPt->m_UseWhatDecd );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_AdoOtptStrmPt->m_SpeexDecd.m_IsUsePrcplEnhsmt：%z32d\n" ), p_AdoOtptStrmPt->m_SpeexDecd.m_IsUsePrcplEnhsmt );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
				}
				FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_StrmUseTotal：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_StrmUseTotal );
				FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			}
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Wavfm.m_IsDraw：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDraw );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Wavfm.m_SrcWndHdl：%#P\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWndHdl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFileWriter.m_IsSave：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_IsSave );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcFullPathVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_WaveFileWriter.m_WrBufSzByt：%uzd\n" ), MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_WrBufSzByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_NameVstrPt：%vs\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_BufSzUnit：%uz32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_BufSzUnit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_AdoOtpt.m_Dvc.m_IsMute：%z32d\n" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_IsMute );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );

			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsUse );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_IsInit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_MaxSmplRate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_MaxSmplRate );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmWidth：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmWidth );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_FrmHeight：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_FrmHeight );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Yu12FrmLenByt：%uzd\n" ), MediaPocsThrdPt->m_VdoInpt.m_Yu12FrmLenByt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_UseWhatEncd：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_UseWhatEncd );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_VdoType：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_VdoType );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_EncdBitrate：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_EncdBitrate );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_BitrateCtrlMode );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_IDRFrmIntvl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_OpenH264Encd.m_Cmplxt：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_OpenH264Encd.m_Cmplxt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_NameVstrPt：%vs\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_PrvwWndHdl：%#P\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoInpt.m_Dvc.m_IsBlack：%z32d\n" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_IsBlack );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );

			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsUse：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsUse );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_IsInit：%z32d\n" ), MediaPocsThrdPt->m_VdoOtpt.m_IsInit );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "m_VdoOtpt.m_StrmCntnr.m_Pt：%#P\n" ), MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.m_Pt );
			FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
			{
				VdoOtpt::Strm * p_VdoOtptStrmPt; //存放视频输出流的指针。
				for( size_t p_StrmNum = 0; MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, &p_VdoOtptStrmPt, NULL, 0, 0, NULL ) == 0; p_StrmNum++ )
				{
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt：%#P\n" ), p_VdoOtptStrmPt );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_Idx：%z32d\n" ), p_VdoOtptStrmPt->m_Idx );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_IsUse：%z32d\n" ), p_VdoOtptStrmPt->m_IsUse );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_UseWhatDecd：%z32d\n" ), p_VdoOtptStrmPt->m_UseWhatDecd );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_OpenH264Decd.m_DecdThrdNum：%z32d\n" ), p_VdoOtptStrmPt->m_OpenH264Decd.m_DecdThrdNum );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_Dvc.m_DspyWndHdl：%#P\n" ), p_VdoOtptStrmPt->m_Dvc.m_DspyWndHdl );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "p_VdoOtptStrmPt->m_Dvc.m_IsBlack：%z32d\n" ), p_VdoOtptStrmPt->m_Dvc.m_IsBlack );
					FileFmtWrite( p_StsFilePt, 0, NULL, Cu8vstr( "\n" ) );
				}
			}

			FileDstoy( p_StsFilePt, 1, NULL );
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：保存状态到Txt文件 %vs 成功。" ), p_ThrdMsgSaveStsToTxtFilePt->m_FullPathVstrPt );
			VstrDstoy( p_ThrdMsgSaveStsToTxtFilePt->m_FullPathVstrPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypRqirExit:
		{
			MediaPocsThrd::ThrdMsgRqirExit * p_ThrdMsgRqirExitPt = ( MediaPocsThrd::ThrdMsgRqirExit * )MsgParmPt;

			switch( p_ThrdMsgRqirExitPt->m_ExitFlag )
			{
				case 1: //请求退出。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：接收退出请求：退出。" ) );
						
					//执行顺序：媒体销毁，用户销毁。
					if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
					{
						MediaPocsThrd::ThrdMsgAdoVdoInptOtptDstoy p_ThrdMsgAdoVdoInptOtptDstoy;
						if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptDstoy, &p_ThrdMsgAdoVdoInptOtptDstoy, sizeof( p_ThrdMsgAdoVdoInptOtptDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto Out;
						}
						MediaPocsThrd::ThrdMsgUserDstoy p_ThrdMsgUserDstoy;
						if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypUserDstoy, &p_ThrdMsgUserDstoy, sizeof( p_ThrdMsgUserDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto Out;
						}
					}
					else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
					{
						
					}
					break;
				}
				case 2: //请求重启。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：接收退出请求：重启。" ) );

					//执行顺序：媒体销毁，用户销毁，用户初始化，媒体初始化。
					if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数。
					{
						MediaPocsThrd::ThrdMsgAdoVdoInptOtptDstoy p_ThrdMsgAdoVdoInptOtptDstoy;
						if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptDstoy, &p_ThrdMsgAdoVdoInptOtptDstoy, sizeof( p_ThrdMsgAdoVdoInptOtptDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto Out;
						}
						MediaPocsThrd::ThrdMsgUserDstoy p_ThrdMsgUserDstoy;
						if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypUserDstoy, &p_ThrdMsgUserDstoy, sizeof( p_ThrdMsgUserDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto Out;
						}
					}
					else //如果上一次调用了用户定义的销毁函数，就不再进行媒体销毁，用户销毁。
					{
						
					}
					MediaPocsThrd::ThrdMsgUserInit p_ThrdMsgUserInit;
					if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypUserInit, &p_ThrdMsgUserInit, sizeof( p_ThrdMsgUserInit ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto Out;
					}
					MediaPocsThrd::ThrdMsgAdoVdoInptOtptInit p_ThrdMsgAdoVdoInptOtptInit;
					if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptInit, &p_ThrdMsgAdoVdoInptOtptInit, sizeof( p_ThrdMsgAdoVdoInptOtptInit ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto Out;
					}
					break;
				}
				case 3: //请求重启但不执行用户定义的UserInit初始化函数和UserDstoy销毁函数。
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：接收退出请求：重启但不调用用户定义的UserInit初始化函数和UserDstoy销毁函数。" ) );

					//执行顺序：媒体销毁，媒体初始化。
					MediaPocsThrd::ThrdMsgAdoVdoInptOtptDstoy p_ThrdMsgAdoVdoInptOtptDstoy;
					if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptDstoy, &p_ThrdMsgAdoVdoInptOtptDstoy, sizeof( p_ThrdMsgAdoVdoInptOtptDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto Out;
					}
					MediaPocsThrd::ThrdMsgAdoVdoInptOtptInit p_ThrdMsgAdoVdoInptOtptInit;
					if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptInit, &p_ThrdMsgAdoVdoInptOtptInit, sizeof( p_ThrdMsgAdoVdoInptOtptInit ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto Out;
					}
					break;
				}
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypUserInit:
		{
			MediaPocsThrd::ThrdMsgUserInit * p_ThrdMsgUserInitPt = ( MediaPocsThrd::ThrdMsgUserInit * )MsgParmPt;

			MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 0; //设置上一次调用了用户定义的初始化函数。
			MediaPocsThrdPt->m_UserInitFuncPt( MediaPocsThrdPt ); //调用用户定义的初始化函数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的初始化函数成功。" ) );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypUserDstoy:
		{
			MediaPocsThrd::ThrdMsgUserDstoy * p_ThrdMsgUserDstoyPt = ( MediaPocsThrd::ThrdMsgUserDstoy * )MsgParmPt;

			MediaPocsThrdPt->m_LastCallUserInitOrDstoy = 1; //设置上一次调用了用户定义的销毁函数。
			MediaPocsThrdPt->m_UserDstoyFuncPt( MediaPocsThrdPt ); //调用用户定义的销毁函数。
			if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的销毁函数成功。" ) );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptInit:
		{
			if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数，就初始化音视频输入输出、音视频输入输出Avi文件写入器。
			{
				if( MediaPocsThrdAdoVdoInptOtptInit( MediaPocsThrdPt ) != 0 ) goto Out;
				if( MediaPocsThrdAdoVdoInptOtptAviFileWriterInit( MediaPocsThrdPt ) != 0 ) goto Out;
			}
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptDstoy:
		{
			MediaPocsThrdAdoVdoInptOtptAviFileWriterDstoy( MediaPocsThrdPt );
			MediaPocsThrdAdoVdoInptOtptDstoy( MediaPocsThrdPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptDvcChg:
		{
			MediaPocsThrdPt->m_UserDvcChgFuncPt( MediaPocsThrdPt, MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt, NULL, NULL );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoInptDvcClos:
		{
			Vstr * * p_AdoInptDvcNameArrPtPt = NULL;
			UINT p_AdoInptDvcTotal = 0;

			MediaPocsThrdGetAdoInptDvcName( &p_AdoInptDvcNameArrPtPt, &p_AdoInptDvcTotal, MediaPocsThrdPt->m_ErrInfoVstrPt );
			if( p_AdoInptDvcTotal > 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入设备[%vs]已经关闭，切换到音频输入设备[%vs]。" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt, p_AdoInptDvcNameArrPtPt[ 0 ] );
				MediaPocsThrdAdoInptSetUseDvc( MediaPocsThrdPt, 0, p_AdoInptDvcNameArrPtPt[ 0 ], NULL );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入设备[%vs]已经关闭，且无音频输入设备，请求退出媒体处理线程。" ), MediaPocsThrdPt->m_AdoInpt.m_Dvc.m_NameVstrPt );
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 0, 1, NULL );
			}
			MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameArrPtPt, p_AdoInptDvcTotal, NULL );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptDvcChg:
		{
			MediaPocsThrdPt->m_UserDvcChgFuncPt( MediaPocsThrdPt, NULL, MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt, NULL );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypAdoOtptDvcClos:
		{
			Vstr * * p_AdoOtptDvcNameArrPtPt = NULL;
			UINT p_AdoOtptDvcTotal = 0;

			MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameArrPtPt, &p_AdoOtptDvcTotal, MediaPocsThrdPt->m_ErrInfoVstrPt );
			if( p_AdoOtptDvcTotal > 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出设备[%vs]已经关闭，切换到音频输出设备[%vs]。" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt, p_AdoOtptDvcNameArrPtPt[ 0 ] );
				MediaPocsThrdAdoOtptSetUseDvc( MediaPocsThrdPt, 0, p_AdoOtptDvcNameArrPtPt[ 0 ], NULL );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出设备[%vs]已经关闭，且无音频输出设备，请求退出媒体处理线程。" ), MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt );
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 0, 1, NULL );
			}
			MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameArrPtPt, p_AdoOtptDvcTotal, NULL );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptDvcChg:
		{
			MediaPocsThrdPt->m_UserDvcChgFuncPt( MediaPocsThrdPt, NULL, NULL, MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt );
			break;
		}
		case MediaPocsThrd::ThrdMsgTypVdoInptDvcClos:
		{
			Vstr * * p_VdoInptDvcNameArrPtPt = NULL;
			UINT p_VdoInptDvcTotal = 0;

			MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameArrPtPt, &p_VdoInptDvcTotal, MediaPocsThrdPt->m_ErrInfoVstrPt );
			if( p_VdoInptDvcTotal > 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输入设备[%vs]已经关闭，切换到视频输入设备[%vs]。" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt, p_VdoInptDvcNameArrPtPt[ 0 ] );
				MediaPocsThrdVdoInptSetUseDvc( MediaPocsThrdPt, 0, p_VdoInptDvcNameArrPtPt[ 0 ], NULL );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输入设备[%vs]已经关闭，且无视频输入设备，请求退出媒体处理线程。" ), MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_NameVstrPt );
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 0, 1, NULL );
			}
			MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameArrPtPt, p_VdoInptDvcTotal, NULL );
			break;
		}
		default: //用户消息。
		{
			MediaPocsThrd::ThrdMsgUserMsg * p_ThrdMsgUserMsgParmPt = ( MediaPocsThrd::ThrdMsgUserMsg * )MsgParmPt;

			p_TmpInt32 = MediaPocsThrdPt->m_UserMsgFuncPt( MediaPocsThrdPt, MsgTyp - MediaPocsThrd::ThrdMsgTypUserMsgMinVal, MsgParmPt, MsgParmLenByt );
			if( p_TmpInt32 == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的消息函数成功。返回值：%d。" ), p_TmpInt32 );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：调用用户定义的消息函数失败。返回值：%d。" ), p_TmpInt32 );
				goto Out;
			}
			break;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt < 0 ) //如果本函数执行失败。
	{
		if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy == 0 ) //如果上一次调用了用户定义的初始化函数，就执行销毁。
		{
			//执行顺序：媒体销毁，用户销毁。
			MediaPocsThrd::ThrdMsgAdoVdoInptOtptDstoy p_ThrdMsgAdoVdoInptOtptDstoy;
			MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptDstoy, &p_ThrdMsgAdoVdoInptOtptDstoy, sizeof( p_ThrdMsgAdoVdoInptOtptDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			MediaPocsThrd::ThrdMsgUserDstoy p_ThrdMsgUserDstoy;
			MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 1, 0, MediaPocsThrd::ThrdMsgTypUserDstoy, &p_ThrdMsgUserDstoy, sizeof( p_ThrdMsgUserDstoy ), MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdAdoVdoInptOtptFrmPocs
 * 功能说明：媒体处理线程的音视频输入输出帧处理。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：无。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void MediaPocsThrdAdoVdoInptOtptFrmPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	size_t p_TmpSz1;
	size_t p_TmpSz2;
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec();

	//调用用户定义的处理函数。
	{
		MediaPocsThrdPt->m_UserPocsFuncPt( MediaPocsThrdPt );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			p_NowTickMsec = FuncGetTickAsMsec();
			//LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的处理函数成功。耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
			p_LastTickMsec = p_NowTickMsec;
		}
	}

	//取出音频输入Pcm格式原始帧和音频输出Pcm格式原始帧。
	if( MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.m_Pt != NULL ) MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取音频输入Pcm格式原始帧容器元素总数。
	else p_TmpSz1 = 0;
	if( MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.m_Pt != NULL ) MediaPocsThrdPt->m_AdoOtpt.m_PcmSrcFrmCntnr.GetTotal( &p_TmpSz2, 1, NULL ); //获取音频输出Pcm格式原始帧容器元素总数。
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
				case 4: //如果要使用WebRtc第三版声学回音消除器。
				{
					if( ( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt != NULL ) && ( WebRtcAec3Pocs( MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ) == 0 ) )
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用WebRtc第三版声学回音消除器成功。" ) );
						SWAPPT( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmTmpFrmPt ); //交换音频输入Pcm格式结果帧和音频输入Pcm格式临时帧。
					}
					else
					{
						if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用WebRtc第三版声学回音消除器失败。" ) );
					}
					break;
				}
				case 5: //如果要使用SpeexWebRtc三重声学回音消除器。
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
			case 0: //如果要使用Pcm原始数据。
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用Pcm原始数据。" ) );
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
					
		//使用波形器。
		if( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_IsDraw != 0 )
		{
			if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_SrcWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始波形器绘制音频输入原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltPt, MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoInpt.m_Wavfm.m_RsltWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果波形器绘制音频输入结果波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		//使用Wave文件写入器。
		if( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_IsSave != 0 )
		{
			if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_SrcWriterPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入Pcm格式原始帧成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入原始Wave文件写入器写入音频输入Pcm格式原始帧失败。" ) );
			}
			if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoInpt.m_WaveFileWriter.m_RsltWriterPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoInptPcmRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入Pcm格式结果帧成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输入结果Wave文件写入器写入音频输入Pcm格式结果帧失败。" ) );
			}
		}

		//使用音视频输入输出Avi文件。
		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoInpt != 0 )
		{
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset != 0 ) //如果音视频输入输出Avi文件音频输入流时间戳要重置。
			{
				p_NowTickMsec = FuncGetTickAsMsec();

				AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, p_NowTickMsec, NULL );
				AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmRsltStrmIdx, p_NowTickMsec, NULL );
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件音频输入Pcm格式原始结果流当前时间戳为 %uz64d 。" ), p_NowTickMsec );

				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptStrmTimeStampIsReset = 0; //设置音视频输入输出Avi文件音频输入流时间戳不重置。
			}

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
			//{ uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoInptPcmSrcStrmIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件音频输入帧时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp ); }
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			p_NowTickMsec = FuncGetTickAsMsec();
			LOGFI( Cu8vstr( "媒体处理线程：音频输入帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
			p_LastTickMsec = p_NowTickMsec;
		}
	} //处理音频输入帧结束。

	//处理音频输出帧开始。
	if( MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt != NULL )
	{
		//使用波形器。
		if( MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_IsDraw != 0 )
		{
			if( AdoWavfmDrawToWnd( MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcPt, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, MediaPocsThrdPt->m_AdoOtpt.m_Wavfm.m_SrcWndHdl, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始波形器绘制音频输出原始波形到窗口失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		//使用Wave文件写入器。
		if( MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_IsSave != 0 )
		{
			if( WaveFileWriterWrite( MediaPocsThrdPt->m_AdoOtpt.m_WaveFileWriter.m_SrcPt, ( char * )MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入音频输出Pcm格式原始帧成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音频输出原始Wave文件写入器写入音频输出Pcm格式原始帧失败。" ) );
			}
		}

		//使用音视频输入输出Avi文件。
		if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveAdoOtpt != 0 )
		{
			if( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset != 0 ) //如果音视频输入输出Avi文件音频输出流时间戳要重置。
			{
				p_NowTickMsec = FuncGetTickAsMsec();

				AviFileWriterAdoStrmSetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, p_NowTickMsec, NULL );
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：设置音视频输入输出Avi文件音频输出Pcm格式原始流当前时间戳为 %uz64d 。" ), p_NowTickMsec );

				MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptStrmTimeStampIsReset = 0; //设置音视频输入输出Avi文件音频输出流时间戳不重置。
			}

			if( AviFileWriterAdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, MediaPocsThrdPt->m_Thrd.m_AdoOtptPcmSrcFrmPt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输出Pcm格式原始帧成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入音频输出Pcm格式原始帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			//{ uint64_t p_Tick, p_CurTimeStamp; p_Tick = FuncGetTickAsMsec(); AviFileWriterAdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_AdoOtptPcmSrcStrmIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件音频输出帧时间戳：%uz64d  %uz64d  %z64d" ), p_Tick, p_CurTimeStamp, p_Tick - p_CurTimeStamp ); }
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			p_NowTickMsec = FuncGetTickAsMsec();
			LOGFI( Cu8vstr( "媒体处理线程：音频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
			p_LastTickMsec = p_NowTickMsec;
		}
	} //处理音频输出帧结束。

	//处理视频输入帧开始。
	if( MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.m_Pt != NULL ) MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输入帧容器元素总数。
	else p_TmpSz1 = 0;
	if( ( p_TmpSz1 > 0 ) && //如果视频输入帧容器中有帧了。
		( ( MediaPocsThrdPt->m_Thrd.m_AdoInptPcmSrcFrmPt != NULL ) || ( MediaPocsThrdPt->m_AdoInpt.m_PcmSrcFrmCntnr.m_Pt == NULL ) ) ) //且已经处理了音频输入帧或不使用音频输入Pcm格式原始帧容器。
	{
		MediaPocsThrdPt->m_VdoInpt.m_FrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt, NULL, 1, 1, NULL ); //从视频输入帧容器中取出并删除第一个帧。
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输入帧容器中取出并删除第一个帧，视频输入帧容器元素总数：%uzd。" ), p_TmpSz1 );
					
		//使用音视频输入输出Avi文件。
		if( ( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_IsSaveVdoInpt != 0 ) && ( MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt != 0 ) )
		{
			if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_TimeStampMsec, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_EncdRsltFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入视频输入已编码格式结果帧成功。" ) );
			}
			else
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：使用音视频输入输出Avi文件写入器写入视频输入已编码格式结果帧失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			//{ uint64_t p_CurTimeStamp; AviFileWriterVdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoInptEncdRsltStrmIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件视频输入帧时间戳：%uz64d  %uz64d  %z64d" ), MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_TimeStampMsec, p_CurTimeStamp, MediaPocsThrdPt->m_Thrd.m_VdoInptFrmPt->m_TimeStampMsec - p_CurTimeStamp ); }
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			p_NowTickMsec = FuncGetTickAsMsec();
			LOGFI( Cu8vstr( "媒体处理线程：视频输入帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
			p_LastTickMsec = p_NowTickMsec;
		}
	} //处理视频输入帧结束。
				
	//处理视频输出帧开始。
	if( MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.m_Pt != NULL ) MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.GetTotal( &p_TmpSz1, 1, NULL ); //获取视频输出帧容器元素总数。
	else p_TmpSz1 = 0;
	if( p_TmpSz1 > 0 ) //如果视频输出帧容器中有帧了。
	{
		MediaPocsThrdPt->m_VdoOtpt.m_FrmCntnr.GetHead( &MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt, NULL, 1, 1, NULL ); //从视频输出帧容器中取出并删除第一个帧。
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：从视频输出帧容器中取出并删除第一个帧，视频输出帧容器元素总数：%uzd。" ), p_TmpSz1 );

		//使用音视频输入输出Avi文件。
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
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引%uz32d：音视频输入输出Avi文件添加视频输出已编码格式原始流成功。索引：%uz32d。" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, p_VdoOtptStrmAviFileIdx );
					MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_VdoOtptEncdSrcStrmIdxMapPt->insert( std::map< uint32_t, uint32_t >::value_type( MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, p_VdoOtptStrmAviFileIdx ) );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引%uz32d：音视频输入输出Avi文件添加视频输出已编码格式原始流失败。原因：%vs" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
			}

			if( p_VdoOtptStrmAviFileIdx != UINT32_MAX )
			{
				if( AviFileWriterVdoStrmWrite( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, p_VdoOtptStrmAviFileIdx, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_TimeStampMsec, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_EncdSrcFrmPt, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_EncdSrcFrmLenByt, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引%uz32d：使用音视频输入输出Avi文件写入器写入视频输出已编码格式原始帧成功。" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx );
				}
				else
				{
					if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引%uz32d：使用音视频输入输出Avi文件写入器写入视频输出已编码格式原始帧失败。原因：%vs" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_StrmIdx, MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
				//{ uint64_t p_CurTimeStamp; AviFileWriterVdoStrmGetCurTimeStamp( MediaPocsThrdPt->m_AdoVdoInptOtptAviFile.m_WriterPt, p_VdoOtptStrmAviFileIdx, &p_CurTimeStamp, NULL ); LOGFE( Cu8vstr( "音视频输入输出Avi文件视频输出帧时间戳：%uz64d  %uz64d  %z64d" ), MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_TimeStampMsec, p_CurTimeStamp, MediaPocsThrdPt->m_Thrd.m_VdoOtptFrmPt->m_TimeStampMsec - p_CurTimeStamp ); }
			}
		}

		if( MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			p_NowTickMsec = FuncGetTickAsMsec();
			LOGFI( Cu8vstr( "媒体处理线程：视频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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
			LOGFI( Cu8vstr( "媒体处理线程：调用用户定义的读取音视频输入帧函数完毕，耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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

	//如果视频输入设备已经关闭，就切换到默认视频输入设备，并重启媒体处理线程。
	if( MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_FilterGraphMediaEventPt != NULL )
	{
		long p_EventCode;
		LONG_PTR p_EventParam1, p_EventParam2;
		MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_FilterGraphMediaEventPt->GetEvent( &p_EventCode, &p_EventParam1, &p_EventParam2, 0 );
		if( p_EventCode == EC_DEVICE_LOST )
		{
			MediaPocsThrd::ThrdMsgVdoInptDvcClos p_ThrdMsgVdoInptDvcClos;
			if( MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypVdoInptDvcClos, &p_ThrdMsgVdoInptDvcClos, sizeof( p_ThrdMsgVdoInptDvcClos ), MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：发送视频输入设备关闭线程消息失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本次媒体处理失败。
	{

	}
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：MediaPocsThrdRun
 * 功能说明：媒体处理线程的主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
             非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI MediaPocsThrdRun( MediaPocsThrd * MediaPocsThrdPt )
{
	uint64_t p_LastTickMsec = 0;

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。
	MediaPocsThrdPt->m_ThrdMsgQueuePt->m_MsgPocsThrdInfoPt = MediaPocsThrdPt->m_Thrd.m_ThrdInfoPt; //设置线程消息队列的消息处理线程信息。

	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：本地代码指令集名称：%s。" ), ( sizeof( size_t ) == 4 ) ? "x86" : "x64" );

	MediaPocsThrdPt->m_RunFlag = MediaPocsThrd::RunFlagRun; //设置本线程运行标记为正在运行。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：开始准备音视频输入输出帧处理。" ) );

	MediaPocsThrd::ThrdMsgUserInit p_ThrdMsgUserInit;
	MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 1, MediaPocsThrd::ThrdMsgTypUserInit, &p_ThrdMsgUserInit, sizeof( p_ThrdMsgUserInit ), MediaPocsThrdPt->m_ErrInfoVstrPt );
	MediaPocsThrd::ThrdMsgAdoVdoInptOtptInit p_ThrdMsgAdoVdoInptOtptInit;
	MsgQueueSendMsg( MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 1, MediaPocsThrd::ThrdMsgTypAdoVdoInptOtptInit, &p_ThrdMsgAdoVdoInptOtptInit, sizeof( p_ThrdMsgAdoVdoInptOtptInit ), MediaPocsThrdPt->m_ErrInfoVstrPt );

	//媒体处理循环开始。
	while( true )
	{
		while( MsgQueueMsgPocsThrdMsgPocs( MediaPocsThrdPt->m_ThrdMsgQueuePt, NULL ) == 0 ); //进行消息处理，直到线程消息队列为空。
		if( MediaPocsThrdPt->m_LastCallUserInitOrDstoy != 0 ) break; //如果线程消息队列为空，且上一次调用了用户定义的销毁函数，就表示本线程需要退出。

		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec();

		MediaPocsThrdAdoVdoInptOtptFrmPocs( MediaPocsThrdPt ); //音视频输入输出帧处理。

		//if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音视频输入输出帧处理全部完毕，耗时 %uz64d 毫秒。" ), FuncGetTickAsMsec() - p_LastTickMsec );

		FuncSleep( 1 ); //暂停一下，避免CPU使用率过高。
	} //媒体处理循环结束。
	
	MediaPocsThrdTmpVarDstoy( MediaPocsThrdPt );
	
	MediaPocsThrdPrvntSysSleepInitOrDstoy( MediaPocsThrdPt, 0 ); //销毁阻止系统睡眠。

	MediaPocsThrdPt->m_RunFlag = MediaPocsThrd::RunFlagExit; //设置本线程运行标记为已经退出。
	if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：本线程已退出。" ) );

	CoUninitialize(); //销毁COM库。

	return 0;
}

//回调MediaPocsThrdCls类的用户定义的初始化函数。
extern "C" void __cdecl MediaPocsThrdClsUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserInit();
}

//回调MediaPocsThrdCls类的用户定义的销毁函数。
extern "C" void __cdecl MediaPocsThrdClsUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserDstoy();
}

//回调MediaPocsThrdCls类的用户定义的处理函数。
extern "C" void __cdecl MediaPocsThrdClsUserPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserPocs();
}

//回调MediaPocsThrdCls类的用户定义的消息函数。
extern "C" int __cdecl MediaPocsThrdClsUserMsg( MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt )
{
	return ( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserMsg( MsgTyp, MsgParmPt, MsgParmLenByt );
}

//回调MediaPocsThrdCls类的用户定义的设备改变函数。
extern "C" void __cdecl MediaPocsThrdClsUserDvcChg( MediaPocsThrd * MediaPocsThrdPt, Vstr * AdoInptDvcNameVstrPt, Vstr * AdoOtptDvcNameVstrPt, Vstr * VdoInptDvcNameVstrPt )
{
	return ( ( MediaPocsThrdCls * )MediaPocsThrdPt->m_UserDataPt )->UserDvcChg( AdoInptDvcNameVstrPt, AdoOtptDvcNameVstrPt, VdoInptDvcNameVstrPt );
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

int MediaPocsThrdCls::Init( const void * LicnCodePt, Vstr * ErrInfoVstrPt )
{
	return MediaPocsThrdInit( &m_MediaPocsThrdPt, LicnCodePt, this,
							  MediaPocsThrdClsUserInit, MediaPocsThrdClsUserDstoy,
							  MediaPocsThrdClsUserPocs, MediaPocsThrdClsUserMsg, MediaPocsThrdClsUserDvcChg,
							  MediaPocsThrdClsUserReadAdoVdoInptFrm,
							  MediaPocsThrdClsUserWriteAdoOtptFrm, MediaPocsThrdClsUserGetAdoOtptFrm,
							  MediaPocsThrdClsUserWriteVdoOtptFrm, MediaPocsThrdClsUserGetVdoOtptFrm,
							  ErrInfoVstrPt );
}
