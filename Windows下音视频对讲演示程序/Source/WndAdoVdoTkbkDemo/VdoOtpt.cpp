#include "VdoOtpt.h"

typedef struct VdoOtptThrdParm
{
	VdoOtpt * VdoOtptPt;
	VdoOtptStrm * VdoOtptStrmPt;
}VdoOtptThrdParm;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmDecdInit
 * 功能说明：初始化视频输出的解码器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptVdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	switch( VdoOtptStrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：初始化YU12原始数据成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( OpenH264DecdInit( &VdoOtptStrmPt->m_OpenH264DecdPt, VdoOtptStrmPt->m_OpenH264DecdDecdThrdNum, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：初始化OpenH264解码器成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			}
			else
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：初始化OpenH264解码器失败。原因：%vs" ), VdoOtptStrmPt->m_VdoOtptStrmIdx, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmDecdDstoy
 * 功能说明：销毁视频输出的解码器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptVdoOtptStrmDecdDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	switch( VdoOtptStrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：销毁YU12原始数据成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( VdoOtptStrmPt->m_OpenH264DecdPt != NULL )
			{
				if( OpenH264DecdDstoy( VdoOtptStrmPt->m_OpenH264DecdPt, NULL ) == 0 )
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：销毁OpenH264解码器成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
				}
				else
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：销毁OpenH264解码器失败。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
				}
				VdoOtptStrmPt->m_OpenH264DecdPt = NULL;
			}
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmThrdInit
 * 功能说明：初始化视频输出的视频输出线程。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptVdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//初始化视频输出线程的临时变量。
	{
		VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar = 1; //设置已初始化视频输出线程的临时变量。
		VdoOtptStrmPt->m_VdoOtptRsltFrmPt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出结果帧的指针。
		if( VdoOtptStrmPt->m_VdoOtptRsltFrmPt == NULL )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：创建视频输出结果帧的内存块失败。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			goto Out;
		}
		VdoOtptStrmPt->m_VdoOtptTmpFrmPt = ( uint8_t * )malloc( 960 * 1280 * 4 * 3 ); //初始化视频输出临时帧的指针。
		if( VdoOtptStrmPt->m_VdoOtptTmpFrmPt == NULL )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：创建视频输出临时帧的内存块失败。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			goto Out;
		}
		VdoOtptStrmPt->m_VdoOtptSwapFrmPt = NULL; //初始化视频输出交换帧的指针。
		VdoOtptStrmPt->m_VdoOtptRsltFrmSz = 960 * 1280 * 4 * 3; //初始化视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
		VdoOtptStrmPt->m_VdoOtptRsltFrmLen = 0; //初始化视频输出结果帧的长度。
		VdoOtptStrmPt->m_VdoOtptFrmWidth = 0; //初始化视频输出帧的宽度。
		VdoOtptStrmPt->m_VdoOtptFrmHeight = 0; //初始化视频输出帧的高度。
		VdoOtptStrmPt->m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
		VdoOtptStrmPt->m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：初始化视频输出线程的临时变量成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}

	//创建并启动音频输出线程。
	{
		VdoOtptStrmPt->m_VdoOtptThrdExitFlag = 0; //设置视频输出线程退出标记为0表示保持运行。
		VdoOtptThrdParm * p_VdoOtptThrdParmPt = ( VdoOtptThrdParm * )malloc( sizeof( VdoOtptThrdParm ) );
		p_VdoOtptThrdParmPt->VdoOtptPt = VdoOtptPt;
		p_VdoOtptThrdParmPt->VdoOtptStrmPt = VdoOtptStrmPt;
		if( ThrdInit( &VdoOtptStrmPt->m_VdoOtptThrdInfoPt, ( LPTHREAD_START_ROUTINE )VdoOtptThrdRun, p_VdoOtptThrdParmPt, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：创建并启动视频输出线程成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：创建并启动视频输出线程失败。原因：%vs" ), VdoOtptStrmPt->m_VdoOtptStrmIdx, GetWinLastErrInfo( , VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmThrdDstoy
 * 功能说明：销毁视频输出的视频输出线程。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptVdoOtptStrmThrdDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	//销毁视频输出线程。
	if( VdoOtptStrmPt->m_VdoOtptThrdInfoPt != NULL )
	{
		VdoOtptStrmPt->m_VdoOtptThrdExitFlag = 1; //请求视频输出线程退出。
		ThrdWaitDstoy( VdoOtptStrmPt->m_VdoOtptThrdInfoPt, NULL ); //等待音频输出线程退出并销毁。
		VdoOtptStrmPt->m_VdoOtptThrdInfoPt = NULL;
		VdoOtptStrmPt->m_VdoOtptThrdExitFlag = 0;
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：销毁视频输出线程成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}

	//销毁视频输出线程的临时变量。
	if( VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar != 0 )
	{
		VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar = 0; //设置未初始化视频输出线程的临时变量。
		if( VdoOtptStrmPt->m_VdoOtptRsltFrmPt != NULL ) //销毁视频输出结果帧的指针。
		{
			free( VdoOtptStrmPt->m_VdoOtptRsltFrmPt );
			VdoOtptStrmPt->m_VdoOtptRsltFrmPt = NULL;
		}
		if( VdoOtptStrmPt->m_VdoOtptTmpFrmPt != NULL ) //销毁视频输出临时帧的指针。
		{
			free( VdoOtptStrmPt->m_VdoOtptTmpFrmPt );
			VdoOtptStrmPt->m_VdoOtptTmpFrmPt = NULL;
		}
		VdoOtptStrmPt->m_VdoOtptSwapFrmPt = NULL; //销毁视频输出交换帧的指针。
		VdoOtptStrmPt->m_VdoOtptRsltFrmSz = 0; //销毁视频输出结果帧的内存大小。按照最高分辨率、BGRA格式、3倍缩放来计算。
		VdoOtptStrmPt->m_VdoOtptRsltFrmLen = 0; //销毁视频输出结果帧的长度。
		VdoOtptStrmPt->m_VdoOtptFrmWidth = 0; //销毁视频输出帧的宽度。
		VdoOtptStrmPt->m_VdoOtptFrmHeight = 0; //销毁视频输出帧的高度。
		VdoOtptStrmPt->m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
		VdoOtptStrmPt->m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %z32d：销毁视频输出线程的临时变量成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}

	InvalidateRect( VdoOtptStrmPt->m_VdoOtptDspyWndHdl, NULL, TRUE ); //重绘视频输出显示窗口。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmInit
 * 功能说明：初始化视频输出的视频输出流。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptVdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	
	if( VdoOtptVdoOtptStrmDecdInit( VdoOtptPt, VdoOtptStrmPt ) != 0 ) goto Out;
	if( VdoOtptVdoOtptStrmThrdInit( VdoOtptPt, VdoOtptStrmPt ) != 0 ) goto Out;

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmDstoy
 * 功能说明：销毁视频输出的视频输出流。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptVdoOtptStrmDstoy( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	VdoOtptVdoOtptStrmThrdDstoy( VdoOtptPt, VdoOtptStrmPt );
	VdoOtptVdoOtptStrmDecdDstoy( VdoOtptPt, VdoOtptStrmPt );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptThrdRun
 * 功能说明：视频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI VdoOtptThrdRun( VdoOtptThrdParm * VdoOtptThrdParmPt )
{
	VdoOtpt * p_VdoOtptPt = VdoOtptThrdParmPt->VdoOtptPt;
	VdoOtptStrm * p_VdoOtptStrmPt = VdoOtptThrdParmPt->VdoOtptStrmPt;
	free( VdoOtptThrdParmPt );

    if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输出线程：开始准备视频输出。" ) );

    //开始视频输出循环。
    while( true )
    {
		if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_VdoOtptStrmPt->m_LastTimeMsec );

		//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
		switch( p_VdoOtptStrmPt->m_UseWhatDecd ) //使用什么解码器。
		{
			case 0: //如果使用YU12原始数据。
			{
				//调用用户定义的写入视频输出帧函数。
				p_VdoOtptStrmPt->m_VdoOtptFrmWidth = 0;
				p_VdoOtptStrmPt->m_VdoOtptFrmHeight = 0;
				p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen = p_VdoOtptStrmPt->m_VdoOtptRsltFrmSz;
				p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, &p_VdoOtptStrmPt->m_VdoOtptFrmWidth, &p_VdoOtptStrmPt->m_VdoOtptFrmHeight, NULL, &p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen );
				
				if( ( p_VdoOtptStrmPt->m_VdoOtptFrmWidth > 0 ) && ( p_VdoOtptStrmPt->m_VdoOtptFrmHeight > 0 ) ) //如果本次写入了视频输出帧。
                {
                    if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：使用YU12原始数据成功。YU12格式帧宽度：%z32d，YU12格式帧高度：%z32d。" ), p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight );
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
				p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen = p_VdoOtptStrmPt->m_VdoOtptRsltFrmSz;
				p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx, NULL, NULL, NULL, p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt, &p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen );

				if( p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen > 0 ) //如果本次写入了视频输出帧。
				{
					//使用OpenH264解码器。
					if( OpenH264DecdPocs( p_VdoOtptStrmPt->m_OpenH264DecdPt,
										  p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt, p_VdoOtptStrmPt->m_VdoOtptRsltFrmLen,
										  p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, p_VdoOtptStrmPt->m_VdoOtptRsltFrmSz, &p_VdoOtptStrmPt->m_VdoOtptFrmWidth, &p_VdoOtptStrmPt->m_VdoOtptFrmHeight,
										  NULL ) == 0 )
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：使用OpenH264解码器成功。已解码YU12格式帧宽度：%z32d，已解码YU12格式帧高度：%z32d。" ), p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight );
                        if( ( p_VdoOtptStrmPt->m_VdoOtptFrmWidth == 0 ) || ( p_VdoOtptStrmPt->m_VdoOtptFrmHeight == 0 ) ) goto Skip; //如果未解码出YU12格式帧，就把本次视频输出帧丢弃。
					}
					else
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：使用OpenH264解码器失败，本次视频输出帧丢弃。" ) );
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
		p_VdoOtptPt->m_MediaPocsThrdPt->m_UserGetYU12VdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight );
		
        //判断视频输出设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
        if( p_VdoOtptStrmPt->m_VdoOtptIsBlack != 0 )
        {
            int p_TmpLen = p_VdoOtptStrmPt->m_VdoOtptFrmWidth * p_VdoOtptStrmPt->m_VdoOtptFrmHeight;
            memset( p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, 0, p_TmpLen );
            memset( p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt + p_TmpLen, 128, p_TmpLen / 2 );
        }

		//缩放视频输出帧。
		if( p_VdoOtptStrmPt->m_VdoOtptDspyScale != 1.0f )
		{
			if( LibYUVPictrScale( p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, PICTR_FMT_BT601F8_YU12_I420, p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight,
								  3,
								  p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt, p_VdoOtptStrmPt->m_VdoOtptRsltFrmSz, NULL, p_VdoOtptStrmPt->m_VdoOtptFrmWidth * p_VdoOtptStrmPt->m_VdoOtptDspyScale, p_VdoOtptStrmPt->m_VdoOtptFrmHeight * p_VdoOtptStrmPt->m_VdoOtptDspyScale,
								  NULL ) != 0 )
			{
				if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输出线程：缩放视频输出帧失败，本次视频输出帧丢弃。" ) );
				goto Skip;
			}
			p_VdoOtptStrmPt->m_VdoOtptSwapFrmPt = p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt; p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt = p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt; p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt = p_VdoOtptStrmPt->m_VdoOtptSwapFrmPt; //交换视频结果帧和视频临时帧。

			p_VdoOtptStrmPt->m_VdoOtptFrmWidth *= p_VdoOtptStrmPt->m_VdoOtptDspyScale;
			p_VdoOtptStrmPt->m_VdoOtptFrmHeight *= p_VdoOtptStrmPt->m_VdoOtptDspyScale;
		}

		//将本次YU12格式视频输出帧转为BGRA格式视频输出帧。
		if( LibYUVPictrFmtCnvrt( p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, PICTR_FMT_BT601F8_YU12_I420, p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight,
								 p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt, p_VdoOtptStrmPt->m_VdoOtptRsltFrmSz, NULL, PICTR_FMT_SRGBF8_BGRA8888,
								 NULL ) != 0 )
		{
			if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：将本次YU12格式视频输出帧转为BGRA格式视频输出帧失败，本次视频输出帧丢弃。" ) );
			goto Skip;
		}
		p_VdoOtptStrmPt->m_VdoOtptSwapFrmPt = p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt; p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt = p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt; p_VdoOtptStrmPt->m_VdoOtptTmpFrmPt = p_VdoOtptStrmPt->m_VdoOtptSwapFrmPt; //交换视频结果帧和视频临时帧。

        //显示视频输出帧。
		if( p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl != NULL )
		{
			//绘制视频输出帧到视频输出显示窗口。
			if( LibYUVPictrDrawToWnd( p_VdoOtptStrmPt->m_VdoOtptRsltFrmPt, PICTR_FMT_SRGBF8_BGRA8888, p_VdoOtptStrmPt->m_VdoOtptFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmHeight,
									  0,
									  p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl,
									  NULL ) != 0 )
			{
				if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "视频输出线程：绘制视频输出帧到视频输出显示窗口失败，本次视频输出帧丢弃。" ) );
				goto Skip;
			}
		}

		if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			FuncGetTimeAsMsec( &p_VdoOtptStrmPt->m_NowTimeMsec );
			LOGFI( Cu8vstr( "视频输出线程：本次视频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_VdoOtptStrmPt->m_NowTimeMsec - p_VdoOtptStrmPt->m_LastTimeMsec );
		}

		Skip:
        if( p_VdoOtptStrmPt->m_VdoOtptThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "视频输出线程：本线程接收到退出请求，开始准备退出。" ) );
            goto Out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	Out:
	if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：本线程已退出。" ) );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptAddVdoOtptStrm
 * 功能说明：添加视频输出的视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptAddVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引是否已经存在。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			return;
		}
	}

	if( VdoOtptPt->m_VdoOtptStrmLnkLst.PutTail( NULL, 0, NULL ) != 0 )
	{
		return;
	}
	VdoOtptPt->m_VdoOtptStrmLnkLst.GetTail( NULL, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL );
	p_VdoOtptStrmPt->m_VdoOtptStrmIdx = VdoOtptStrmIdx;
	p_VdoOtptStrmPt->m_UseWhatDecd = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptDelVdoOtptStrm
 * 功能说明：删除视频输出的视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptDelVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );
			VdoOtptPt->m_VdoOtptStrmLnkLst.DelByIdx( p_VdoOtptStrmLnkLstIdx, 0, NULL );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtpt
 * 功能说明：设置视频输出的视频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             IsUseVdoOtpt：[输入]，存放是否使用视频输出，为非0表示要使用，为0表示不使用。
			 FrmWidth：[输入]，存放帧的宽度，单位为像素。
			 FrmHeight：[输入]，存放帧的高度，单位为像素。
			 VdoOtptDspyWndHdl：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 VdoOtptDspyScale：[输入]，存放视频输出缩放的倍数。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrm( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, HWND VdoOtptDspyWndHdl, float VdoOtptDspyScale )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );

			p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl = VdoOtptDspyWndHdl;
			p_VdoOtptStrmPt->m_VdoOtptDspyScale = VdoOtptDspyScale;

			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtptStrmUseYU12
 * 功能说明：设置视频输出的视频输出流使用YU12原始数据。
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

void VdoOtptSetVdoOtptStrmUseYU12( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );

			p_VdoOtptStrmPt->m_UseWhatDecd = 0;

			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtptStrmUseOpenH264Decd
 * 功能说明：设置视频输出的视频输出流使用OpenH264解码器。
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

void VdoOtptSetVdoOtptStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t DecdThrdNum )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );

			p_VdoOtptStrmPt->m_UseWhatDecd = 1;
			p_VdoOtptStrmPt->m_OpenH264DecdDecdThrdNum = DecdThrdNum;

			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtptStrmIsBlack
 * 功能说明：设置视频输出的视频输出流是否黑屏。
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

void VdoOtptSetVdoOtptStrmIsBlack( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t IsBlack )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			p_VdoOtptStrmPt->m_VdoOtptIsBlack = IsBlack;
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtptStrmIsUse
 * 功能说明：设置视频输出的视频输出流是否使用。
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

void VdoOtptSetVdoOtptStrmIsUse( VdoOtpt * VdoOtptPt, int32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			if( IsUseVdoOtptStrm != 0 ) //如果要使用视频输出流。
			{
				if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm == 0 ) //如果当前不使用视频输出流。
				{
					if( VdoOtptPt->m_IsInitVdoOtpt != 0 ) //如果已初始化视频输出。
					{
						if( VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt ) == 0 ) //如果初始化视频输出流成功。
						{
							p_VdoOtptStrmPt->m_IsUseVdoOtptStrm = 1;
						}
					}
					else //如果未初始化视频输出。
					{
						p_VdoOtptStrmPt->m_IsUseVdoOtptStrm = 1;
					}
				}
			}
			else //如果不使用视频输出流。
			{
				if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) //如果当前要使用视频输出流。
				{
					if( VdoOtptPt->m_IsInitVdoOtpt != 0 ) //如果已初始化视频输出。
					{
						VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );
						p_VdoOtptStrmPt->m_IsUseVdoOtptStrm = 0;
					}
					else //如果未初始化视频输出。
					{
						p_VdoOtptStrmPt->m_IsUseVdoOtptStrm = 0;
					}
				}
			}
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptInit
 * 功能说明：初始化视频输出。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptInit( VdoOtpt * VdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。
	
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void ** )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm )
		{
			if( VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt ) != 0 ) goto Out;
		}
	}

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec ); //记录初始化结束的时间。
		LOGFI( Cu8vstr( "媒体处理线程：初始化视频输出耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		VdoOtptDstoy( VdoOtptPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptDstoy
 * 功能说明：销毁视频输出。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptDstoy( VdoOtpt * VdoOtptPt )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void ** )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm )
		{
			VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );
		}
	}
	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输出成功。" ) );
}
