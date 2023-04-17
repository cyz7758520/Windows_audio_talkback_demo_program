#include "VdoOtpt.h"

typedef struct //视频输出线程的参数。
{
	VdoOtpt * m_VdoOtptPt;
	VdoOtpt::Strm * m_StrmPt;
} VdoOtptThrdParm;

DWORD WINAPI VdoOtptThrdRun( VdoOtptThrdParm * VdoOtptThrdParmPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptStrmDecdInit
 * 功能说明：初始化视频输出流的解码器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( StrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化YU12原始数据成功。" ), StrmPt->m_Idx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( OpenH264DecdInit( &StrmPt->m_OpenH264Decd.m_Pt, StrmPt->m_OpenH264Decd.m_DecdThrdNum, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化OpenH264解码器成功。" ), StrmPt->m_Idx );
			}
			else
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化OpenH264解码器失败。原因：%vs" ), StrmPt->m_Idx, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：VdoOtptStrmDecdDstoy
 * 功能说明：销毁视频输出流的解码器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptStrmDecdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	switch( StrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁YU12原始数据成功。" ), StrmPt->m_Idx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( StrmPt->m_OpenH264Decd.m_Pt != NULL )
			{
				if( OpenH264DecdDstoy( StrmPt->m_OpenH264Decd.m_Pt, NULL ) == 0 )
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁OpenH264解码器成功。" ), StrmPt->m_Idx );
				}
				else
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁OpenH264解码器失败。" ), StrmPt->m_Idx );
				}
				StrmPt->m_OpenH264Decd.m_Pt = NULL;
			}
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptStrmThrdInit
 * 功能说明：初始化视频输出流的线程。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
	//初始化线程的临时变量。
	{
		StrmPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程的临时变量。
		StrmPt->m_Thrd.m_BgraRlstFrmPt = ( uint8_t * )malloc( VdoOtptPt->m_FrmMaxWidth * VdoOtptPt->m_FrmMaxHeight * 4 * 3 ); //初始化BGRA8888格式结果帧的指针。按照最高分辨率、BGRA格式来计算。
		if( StrmPt->m_Thrd.m_BgraRlstFrmPt == NULL )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：创建BGRA8888格式结果帧的内存块失败。" ), StrmPt->m_Idx );
			goto Out;
		}
		StrmPt->m_Thrd.m_FrmPt = NULL; //初始化帧的指针。
		StrmPt->m_Thrd.m_LnkLstElmTotal = 0; //初始化链表的元素总数。
		StrmPt->m_Thrd.m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		StrmPt->m_Thrd.m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化线程的临时变量成功。" ), StrmPt->m_Idx );
	}

	//初始化线程。
	{
		StrmPt->m_Thrd.m_ExitFlag = 0; //设置退出标记为0表示保持运行。
		VdoOtptThrdParm * p_VdoOtptThrdParmPt = ( VdoOtptThrdParm * )malloc( sizeof( VdoOtptThrdParm ) );
		p_VdoOtptThrdParmPt->m_VdoOtptPt = VdoOtptPt;
		p_VdoOtptThrdParmPt->m_StrmPt = StrmPt;
		if( ThrdInit( &StrmPt->m_Thrd.m_ThrdInfoPt, ( LPTHREAD_START_ROUTINE )VdoOtptThrdRun, p_VdoOtptThrdParmPt, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化线程成功。" ), StrmPt->m_Idx );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化线程线程失败。原因：%vs" ), StrmPt->m_Idx, GetWinLastErrInfo( , VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
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
 * 函数名称：VdoOtptStrmThrdDstoy
 * 功能说明：销毁视频输出流的线程。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptStrmThrdDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	//销毁线程。
	if( StrmPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		StrmPt->m_Thrd.m_ExitFlag = 1; //请求线程退出。
		ThrdWaitDstoy( StrmPt->m_Thrd.m_ThrdInfoPt, NULL ); //等待线程退出并销毁。
		StrmPt->m_Thrd.m_ThrdInfoPt = NULL;
		StrmPt->m_Thrd.m_ExitFlag = 0;
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁线程成功。" ), StrmPt->m_Idx );
	}

	//销毁线程的临时变量。
	if( StrmPt->m_Thrd.m_IsInitThrdTmpVar != 0 )
	{
		StrmPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程的临时变量。
		if( StrmPt->m_Thrd.m_BgraRlstFrmPt != NULL ) //销毁BGRA8888格式结果帧的指针。
		{
			free( StrmPt->m_Thrd.m_BgraRlstFrmPt );
			StrmPt->m_Thrd.m_BgraRlstFrmPt = NULL;
		}
		StrmPt->m_Thrd.m_FrmPt = NULL; //销毁帧的指针。
		StrmPt->m_Thrd.m_LnkLstElmTotal = 0; //销毁链表的元素总数。
		StrmPt->m_Thrd.m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		StrmPt->m_Thrd.m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁线程的临时变量成功。" ), StrmPt->m_Idx );
	}
	
	InvalidateRect( StrmPt->m_Dvc.m_DspyWndHdl, NULL, TRUE ); //重绘显示窗口。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptStrmInit
 * 功能说明：初始化视频输出的流。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
	if( VdoOtptStrmDecdInit( VdoOtptPt, StrmPt ) != 0 ) goto Out;
	if( VdoOtptStrmThrdInit( VdoOtptPt, StrmPt ) != 0 ) goto Out;

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptStrmDstoy
 * 功能说明：销毁视频输出的流。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptStrmDstoy( VdoOtpt * VdoOtptPt, VdoOtpt::Strm * StrmPt )
{
	VdoOtptStrmThrdDstoy( VdoOtptPt, StrmPt );
	VdoOtptStrmDecdDstoy( VdoOtptPt, StrmPt );
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
	VdoOtpt * p_VdoOtptPt = VdoOtptThrdParmPt->m_VdoOtptPt;
	VdoOtpt::Strm * p_StrmPt = VdoOtptThrdParmPt->m_StrmPt;
	free( VdoOtptThrdParmPt );

    if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：开始准备视频输出。" ), p_StrmPt->m_Idx );

    //视频输出循环开始。
    while( true )
    {
		{
			if( p_StrmPt->m_Thrd.m_FrmPt == NULL ) //如果没获取一个空闲帧。
			{
				//获取一个空闲帧。
				p_VdoOtptPt->m_IdleFrmLnkLst.GetTotal( &p_StrmPt->m_Thrd.m_LnkLstElmTotal, 1, NULL ); //获取空闲帧链表的元素总数。
				if( p_StrmPt->m_Thrd.m_LnkLstElmTotal > 0 ) //如果空闲帧链表中有帧。
				{
					//从空闲帧链表中取出第一个帧。
					{
						p_VdoOtptPt->m_IdleFrmLnkLst.Locked( NULL ); //空闲帧链表的互斥锁加锁。
						p_VdoOtptPt->m_IdleFrmLnkLst.GetHead( NULL, &p_StrmPt->m_Thrd.m_FrmPt, NULL, 0, NULL );
						p_VdoOtptPt->m_IdleFrmLnkLst.DelHead( 0, NULL );
						p_VdoOtptPt->m_IdleFrmLnkLst.Unlock( NULL ); //空闲帧链表的互斥锁解锁。
					}
					if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：从空闲帧链表中取出第一个帧，空闲帧链表元素总数：%uzd。" ), p_StrmPt->m_Idx, p_StrmPt->m_Thrd.m_LnkLstElmTotal );
				}
				else //如果空闲帧链表中没有帧。
				{
					p_VdoOtptPt->m_FrmLnkLst.GetTotal( &p_StrmPt->m_Thrd.m_LnkLstElmTotal, 1, NULL ); //获取帧链表的元素总数。
					if( p_StrmPt->m_Thrd.m_LnkLstElmTotal <= 20 )
					{
						//创建一个空闲帧。
						{
							int p_CreateVdoOtptIdleFrmRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。

							p_StrmPt->m_Thrd.m_FrmPt = ( VdoOtpt::Frm * )calloc( 1, sizeof( VdoOtpt::Frm ) );
							if( p_StrmPt->m_Thrd.m_FrmPt == NULL )
							{
								goto OutCreateVdoOtptIdleFrm;
							}
							p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt = ( uint8_t * )malloc( p_VdoOtptPt->m_FrmMaxWidth * p_VdoOtptPt->m_FrmMaxHeight * 3 / 2 );
							if( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt == NULL )
							{
								goto OutCreateVdoOtptIdleFrm;
							}
							if( p_StrmPt->m_UseWhatDecd != 0 )
							{
								p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt = ( uint8_t * )malloc( p_VdoOtptPt->m_FrmMaxWidth * p_VdoOtptPt->m_FrmMaxHeight * 3 / 2 );
								if( p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt == NULL )
								{
									goto OutCreateVdoOtptIdleFrm;
								}
							}
							
							p_CreateVdoOtptIdleFrmRslt = 0; //设置本处理段执行成功。

							OutCreateVdoOtptIdleFrm:
							if( p_CreateVdoOtptIdleFrmRslt != 0 ) //如果本处理段执行失败。
							{
								if( p_StrmPt->m_Thrd.m_FrmPt != NULL )
								{
									if( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt != NULL ) free( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt );
									if( p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt != NULL ) free( p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt );
									free( p_StrmPt->m_Thrd.m_FrmPt );
									p_StrmPt->m_Thrd.m_FrmPt = NULL;
								}
							}
						}

						if( p_StrmPt->m_Thrd.m_FrmPt != NULL )
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：空闲帧链表中没有帧，创建一个空闲帧成功。" ), p_StrmPt->m_Idx );
						}
						else
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：空闲帧链表中没有帧，创建一个空闲帧失败。原因：内存不足。" ), p_StrmPt->m_Idx );
							goto OutPocs;
						}
					}
					else
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：帧链表中帧总数为%uzd已经超过上限20，不再创建空闲帧。" ), p_StrmPt->m_Idx, p_StrmPt->m_Thrd.m_LnkLstElmTotal );
						goto OutPocs;
					}
				}
			}

			p_StrmPt->m_Thrd.m_LastTickMsec = FuncGetTickAsMsec();

			//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
			switch( p_StrmPt->m_UseWhatDecd ) //使用什么解码器。
			{
				case 0: //如果要使用YU12原始数据。
				{
					//调用用户定义的写入视频输出帧函数。
					p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth = 0;
					p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight = 0;
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																				 p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, &p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, &p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
																				 NULL, 0, NULL );

					if( ( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth > 0 ) && ( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight > 0 ) ) //如果本次写入了帧。
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用YU12原始数据成功。YU12格式原始帧宽度：%z32d，高度：%z32d。" ), p_StrmPt->m_Idx, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight );
					}
					else //如果本次没写入帧。
					{
						goto OutPocs;
					}

					//用户定义的获取视频输出帧函数。
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																			   p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
																			   NULL, 0 );
					break;
				}
				case 1: //如果要使用OpenH264解码器。
				{
					//调用用户定义的写入视频输出帧函数。
					p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmLenByt = 0;
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																				 NULL, NULL, NULL,
																				 p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt, p_VdoOtptPt->m_FrmMaxWidth * p_VdoOtptPt->m_FrmMaxHeight * 3 / 2, &p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmLenByt );

					if( p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmLenByt > 0 ) //如果本次写入了帧。
					{
						//使用OpenH264解码器。
						if( OpenH264DecdPocs( p_StrmPt->m_OpenH264Decd.m_Pt,
											  p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt, p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmLenByt,
											  p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, p_VdoOtptPt->m_FrmMaxWidth * p_VdoOtptPt->m_FrmMaxHeight * 3 / 2, &p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, &p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
											  NULL ) == 0 )
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用OpenH264解码器成功。YU12格式原始帧宽度：%z32d，高度：%z32d。" ), p_StrmPt->m_Idx, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight );
							if( ( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth == 0 ) || ( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight == 0 ) ) goto OutPocs; //如果未解码出YU12格式帧，就把本次帧丢弃。
						}
						else
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用OpenH264解码器失败，本次帧丢弃。" ), p_StrmPt->m_Idx );
							goto OutPocs;
						}
					}
					else //如果本次没写入帧。
					{
						goto OutPocs;
					}

					//用户定义的获取视频输出帧函数。
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																			   p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
																			   p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmPt, p_StrmPt->m_Thrd.m_FrmPt->m_EncdSrcFrmLenByt );
					break;
				}
			}

			//判断设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
			if( p_StrmPt->m_Dvc.m_IsBlack != 0 )
			{
				size_t p_TmpLenByt = p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth * p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight;
				memset( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, 0, p_TmpLenByt );
				memset( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt + p_TmpLenByt, 128, p_TmpLenByt / 2 );
			}

			//将本次YU12格式帧转为BGRA格式帧。
			if( LibYUVPictrFmtCnvrt( p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmPt, PICTR_FMT_BT601F8_YU12_I420, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
									 p_StrmPt->m_Thrd.m_BgraRlstFrmPt, p_VdoOtptPt->m_FrmMaxWidth * p_VdoOtptPt->m_FrmMaxHeight * 4, NULL, PICTR_FMT_SRGBF8_BGRA8888,
									 NULL ) != 0 )
			{
				if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：将本次YU12格式原始帧转为BGRA格式结果帧失败，本次帧丢弃。" ), p_StrmPt->m_Idx );
				goto OutPocs;
			}

			//显示帧。
			if( p_StrmPt->m_Dvc.m_DspyWndHdl != NULL )
			{
				//绘制帧到显示窗口。
				if( LibYUVPictrDrawToWnd( p_StrmPt->m_Thrd.m_BgraRlstFrmPt, PICTR_FMT_SRGBF8_BGRA8888, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmWidth, p_StrmPt->m_Thrd.m_FrmPt->m_YU12SrcFrmHeight,
										  0,
										  p_StrmPt->m_Dvc.m_DspyWndHdl,
										  NULL ) != 0 )
				{
					if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：绘制BGRA格式结果帧到显示窗口失败，本次帧丢弃。" ), p_StrmPt->m_Idx );
					goto OutPocs;
				}
			}

			p_StrmPt->m_Thrd.m_FrmPt->m_StrmIdx = p_StrmPt->m_Idx; //设置流索引。
			p_StrmPt->m_Thrd.m_FrmPt->m_TimeStampMsec = p_StrmPt->m_Thrd.m_LastTickMsec; //设置时间戳。

			//追加本次帧到帧链表。
			{
				p_VdoOtptPt->m_FrmLnkLst.PutTail( &p_StrmPt->m_Thrd.m_FrmPt, 1, NULL );
				p_StrmPt->m_Thrd.m_FrmPt = NULL;
			}

			if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				p_StrmPt->m_Thrd.m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本次帧处理完毕，耗时 %uz64d 毫秒。" ), p_StrmPt->m_Idx, p_StrmPt->m_Thrd.m_NowTickMsec - p_StrmPt->m_Thrd.m_LastTickMsec );
			}
		}
		OutPocs:;

        if( p_StrmPt->m_Thrd.m_ExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本线程接收到退出请求，开始准备退出。" ), p_StrmPt->m_Idx );
            goto Out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	Out:
	if( p_StrmPt->m_Thrd.m_FrmPt != NULL ) //如果获取的空闲帧没有追加到帧链表。
	{
		p_VdoOtptPt->m_IdleFrmLnkLst.PutTail( &p_StrmPt->m_Thrd.m_FrmPt, 1, NULL );
		p_StrmPt->m_Thrd.m_FrmPt = NULL;
	}
	if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本线程已退出。" ), p_StrmPt->m_Idx );
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptAddStrm
 * 功能说明：添加视频输出的流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptAddStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			return;
		}
	}

	//添加到流链表。
	if( VdoOtptPt->m_StrmLnkLst.PutTail( NULL, 0, NULL ) != 0 )
	{
		return;
	}
	VdoOtptPt->m_StrmLnkLst.GetTail( NULL, NULL, ( void * * )&p_StrmPt, 0, NULL );
	p_StrmPt->m_Idx = StrmIdx;
	p_StrmPt->m_UseWhatDecd = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptDelStrm
 * 功能说明：删除视频输出的流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptDelStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );
			VdoOtptPt->m_StrmLnkLst.DelByIdx( p_StrmLnkLstIdx, 0, NULL );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtpt
 * 功能说明：设置视频输出的流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 DspyWndHdl：[输入]，存放显示窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetStrm( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, HWND DspyWndHdl )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );

			p_StrmPt->m_Dvc.m_DspyWndHdl = DspyWndHdl;

			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmInit( VdoOtptPt, p_StrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetStrmUseYU12
 * 功能说明：设置视频输出的流使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetStrmUseYU12( VdoOtpt * VdoOtptPt, uint32_t StrmIdx )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );

			p_StrmPt->m_UseWhatDecd = 0;

			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmInit( VdoOtptPt, p_StrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetStrmUseOpenH264Decd
 * 功能说明：设置视频输出的流使用OpenH264解码器。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 DecdThrdNum：[输入]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t DecdThrdNum )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );

			p_StrmPt->m_UseWhatDecd = 1;
			p_StrmPt->m_OpenH264Decd.m_DecdThrdNum = DecdThrdNum;

			if( ( VdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) VdoOtptStrmInit( VdoOtptPt, p_StrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetStrmIsBlack
 * 功能说明：设置视频输出的流是否黑屏。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 IsBlack：[输入]，存放是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetStrmIsBlack( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t IsBlack )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx )
		{
			p_StrmPt->m_Dvc.m_IsBlack = IsBlack;
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetStrmIsUse
 * 功能说明：设置视频输出的流是否使用。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
             IsUseStrm：[输入]，存放是否使用流，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetStrmIsUse( VdoOtpt * VdoOtptPt, uint32_t StrmIdx, int32_t IsUseStrm )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	//查找流索引。
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果索引找到了。
		{
			if( IsUseStrm != 0 ) //如果要使用流。
			{
				if( p_StrmPt->m_IsUse == 0 ) //如果当前不使用流。
				{
					if( VdoOtptPt->m_IsInit != 0 ) //如果已初始化视频输出。
					{
						if( VdoOtptStrmInit( VdoOtptPt, p_StrmPt ) == 0 ) //如果初始化流成功。
						{
							p_StrmPt->m_IsUse = 1;
						}
					}
					else //如果未初始化视频输出。
					{
						p_StrmPt->m_IsUse = 1;
					}
				}
			}
			else //如果不使用流。
			{
				if( p_StrmPt->m_IsUse != 0 ) //如果当前要使用流。
				{
					if( VdoOtptPt->m_IsInit != 0 ) //如果已初始化视频输出。
					{
						VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );
						p_StrmPt->m_IsUse = 0;
					}
					else //如果未初始化视频输出。
					{
						p_StrmPt->m_IsUse = 0;
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptInit( VdoOtpt * VdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录初始化开始的嘀嗒钟。
	
	//初始化帧链表。
	if( VdoOtptPt->m_FrmLnkLst.Init( sizeof( VdoOtpt::Frm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出：初始化帧链表成功。" ) );
	}
	else
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出：初始化帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化空闲帧链表。
	if( VdoOtptPt->m_IdleFrmLnkLst.Init( sizeof( VdoOtpt::Frm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出：初始化空闲帧链表成功。" ) );
	}
	else
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出：初始化空闲帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void ** )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_IsUse )
		{
			if( VdoOtptStrmInit( VdoOtptPt, p_StrmPt ) != 0 ) goto Out;
		}
	}

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：视频输出：初始化耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptDstoy( VdoOtpt * VdoOtptPt )
{
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。
	VdoOtpt::Frm * p_FrmPt; //存放帧的指针。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录销毁开始的嘀嗒钟。
	
	for( size_t p_StrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_StrmLnkLst.GetNextByIdx( p_StrmLnkLstIdx, &p_StrmLnkLstIdx, NULL, ( void * * )&p_StrmPt, 0, NULL ) == 0; )
	{
		if( p_StrmPt->m_IsUse )
		{
			VdoOtptStrmDstoy( VdoOtptPt, p_StrmPt );
		}
	}

	//销毁空闲帧链表。
	if( VdoOtptPt->m_IdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoOtptPt->m_IdleFrmLnkLst.GetHead( NULL, &p_FrmPt, NULL, 0, NULL ) == 0 )
		{
			if( p_FrmPt->m_YU12SrcFrmPt )
			{
				free( p_FrmPt->m_YU12SrcFrmPt );
				p_FrmPt->m_YU12SrcFrmPt = NULL;
			}
			if( p_FrmPt->m_EncdSrcFrmPt != NULL )
			{
				free( p_FrmPt->m_EncdSrcFrmPt );
				p_FrmPt->m_EncdSrcFrmPt = NULL;
			}
			free( p_FrmPt );
			p_FrmPt = NULL;
			VdoOtptPt->m_IdleFrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoOtptPt->m_IdleFrmLnkLst.Dstoy( VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出：销毁空闲帧链表成功。" ) );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出：销毁空闲帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁帧链表。
	if( VdoOtptPt->m_FrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoOtptPt->m_FrmLnkLst.GetHead( NULL, &p_FrmPt, NULL, 0, NULL ) == 0 )
		{
			if( p_FrmPt->m_YU12SrcFrmPt )
			{
				free( p_FrmPt->m_YU12SrcFrmPt );
				p_FrmPt->m_YU12SrcFrmPt = NULL;
			}
			if( p_FrmPt->m_EncdSrcFrmPt != NULL )
			{
				free( p_FrmPt->m_EncdSrcFrmPt );
				p_FrmPt->m_EncdSrcFrmPt = NULL;
			}
			free( p_FrmPt );
			p_FrmPt = NULL;
			VdoOtptPt->m_FrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoOtptPt->m_FrmLnkLst.Dstoy( VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出：销毁帧链表成功。" ) );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出：销毁帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录销毁结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：视频输出：销毁耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}
}
