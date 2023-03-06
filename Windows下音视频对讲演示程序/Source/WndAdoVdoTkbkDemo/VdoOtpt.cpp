#include "VdoOtpt.h"

#define FrmMaxWidth  960
#define FrmMaxHeight 1280

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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptVdoOtptStrmDecdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( VdoOtptStrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用YU12原始数据。
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化YU12原始数据成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( OpenH264DecdInit( &VdoOtptStrmPt->m_OpenH264DecdPt, VdoOtptStrmPt->m_OpenH264DecdDecdThrdNum, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化OpenH264解码器成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			}
			else
			{
				if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化OpenH264解码器失败。原因：%vs" ), VdoOtptStrmPt->m_VdoOtptStrmIdx, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁YU12原始数据成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			break;
		}
		case 1: //如果要使用OpenH264解码器。
		{
			if( VdoOtptStrmPt->m_OpenH264DecdPt != NULL )
			{
				if( OpenH264DecdDstoy( VdoOtptStrmPt->m_OpenH264DecdPt, NULL ) == 0 )
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁OpenH264解码器成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
				}
				else
				{
					if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁OpenH264解码器失败。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int VdoOtptVdoOtptStrmThrdInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
	//初始化视频输出线程的临时变量。
	{
		VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar = 1; //设置已初始化视频输出线程的临时变量。
		VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt = ( uint8_t * )malloc( FrmMaxWidth * FrmMaxHeight * 4 * 3 ); //初始化BGRA8888格式视频输出结果帧的指针。按照最高分辨率、BGRA格式来计算。
		if( VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt == NULL )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：创建BGRA8888格式视频输出结果帧的内存块失败。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
			goto Out;
		}
		VdoOtptStrmPt->m_VdoOtptFrmElmPt = NULL; //初始化视频输出帧元素的指针。
		VdoOtptStrmPt->m_FrmLnkLstElmTotal = 0; //初始化帧链表的元素总数。
		VdoOtptStrmPt->m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		VdoOtptStrmPt->m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：初始化视频输出线程的临时变量成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}

	//创建并启动音频输出线程。
	{
		VdoOtptStrmPt->m_VdoOtptThrdExitFlag = 0; //设置视频输出线程退出标记为0表示保持运行。
		VdoOtptThrdParm * p_VdoOtptThrdParmPt = ( VdoOtptThrdParm * )malloc( sizeof( VdoOtptThrdParm ) );
		p_VdoOtptThrdParmPt->VdoOtptPt = VdoOtptPt;
		p_VdoOtptThrdParmPt->VdoOtptStrmPt = VdoOtptStrmPt;
		if( ThrdInit( &VdoOtptStrmPt->m_VdoOtptThrdInfoPt, ( LPTHREAD_START_ROUTINE )VdoOtptThrdRun, p_VdoOtptThrdParmPt, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：创建并启动视频输出线程成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：创建并启动视频输出线程失败。原因：%vs" ), VdoOtptStrmPt->m_VdoOtptStrmIdx, GetWinLastErrInfo( , VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁视频输出线程成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}

	//销毁视频输出线程的临时变量。
	if( VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar != 0 )
	{
		VdoOtptStrmPt->m_IsInitVdoOtptThrdTmpVar = 0; //设置未初始化视频输出线程的临时变量。
		if( VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt != NULL ) //销毁BGRA8888格式视频输出结果帧的指针。
		{
			free( VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt );
			VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt = NULL;
		}
		VdoOtptStrmPt->m_VdoOtptFrmElmPt = NULL; //销毁视频输出帧元素的指针。
		VdoOtptStrmPt->m_FrmLnkLstElmTotal = 0; //销毁帧链表的元素总数。
		VdoOtptStrmPt->m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		VdoOtptStrmPt->m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：视频输出流索引 %uz32d：销毁视频输出线程的临时变量成功。" ), VdoOtptStrmPt->m_VdoOtptStrmIdx );
	}
	
	InvalidateRect( VdoOtptStrmPt->m_VdoOtptDspyWndHdl, NULL, TRUE ); //重绘视频输出显示窗口。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptVdoOtptStrmInit
 * 功能说明：初始化视频输出的视频输出流。
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

int VdoOtptVdoOtptStrmInit( VdoOtpt * VdoOtptPt, VdoOtptStrm * VdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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

    if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：开始准备视频输出。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );

    //开始视频输出循环。
    while( true )
    {
		if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt == NULL ) //如果没获取一个视频输出空闲帧。
		{
			//获取一个视频输出空闲帧。
			p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.GetTotal( &p_VdoOtptStrmPt->m_FrmLnkLstElmTotal, 1, NULL ); //获取视频输出空闲帧链表的元素总数。
			if( p_VdoOtptStrmPt->m_FrmLnkLstElmTotal > 0 ) //如果视频输出空闲帧链表中有帧。
			{
				//从视频输出空闲帧链表中取出第一个帧。
				{
					p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.Locked( NULL ); //视频输出空闲帧链表的互斥锁加锁。
					p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.GetHead( NULL, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt, NULL, 0, NULL );
					p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
					p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.Unlock( NULL ); //视频输出空闲帧链表的互斥锁解锁。
				}
				if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )  LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：从视频输出空闲帧链表中取出第一个帧，视频输出空闲帧链表元素总数：%uzd。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_FrmLnkLstElmTotal );
			}
			else //如果视频输出空闲帧链表中没有帧。
			{
				p_VdoOtptPt->m_VdoOtptFrmLnkLst.GetTotal( &p_VdoOtptStrmPt->m_FrmLnkLstElmTotal, 1, NULL ); //获取视频输出帧链表的元素总数。
				if( p_VdoOtptStrmPt->m_FrmLnkLstElmTotal <= 1 ) //视频输出帧链表最多只存储2帧，避免因为视频输出帧太多导致卡顿。
				{
					//创建一个视频输出空闲帧。
					{
						int p_CreateVdoOtptIdleFrmRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。

						p_VdoOtptStrmPt->m_VdoOtptFrmElmPt = ( VdoOtptFrmElm * )calloc( 1, sizeof( VdoOtptFrmElm ) );
						if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt == NULL )
						{
							goto OutCreateVdoOtptIdleFrm;
						}
						p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt = ( uint8_t * )malloc( FrmMaxWidth * FrmMaxHeight * 3 / 2 );
						if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt == NULL )
						{
							goto OutCreateVdoOtptIdleFrm;
						}
						p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt = ( uint8_t * )malloc( FrmMaxWidth * FrmMaxHeight * 3 / 2 );
						if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt == NULL )
						{
							goto OutCreateVdoOtptIdleFrm;
						}

						p_CreateVdoOtptIdleFrmRslt = 0; //设置本处理段执行成功。

						OutCreateVdoOtptIdleFrm:
						if( p_CreateVdoOtptIdleFrmRslt != 0 ) //如果本处理段执行失败。
						{
							if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt != NULL )
							{
								if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt != NULL ) free( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt );
								if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt != NULL ) free( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt );
								free( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt );
								p_VdoOtptStrmPt->m_VdoOtptFrmElmPt = NULL;
							}
						}
					}

					if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt != NULL )
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：视频输出空闲帧链表中没有帧，创建一个视频输出空闲帧成功。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
					}
					else
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：视频输出空闲帧链表中没有帧，创建一个视频输出空闲帧失败。原因：内存不足。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
					}
				}
				else
				{
					if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：视频输出帧链表中帧数量为%uzd已经超过上限1，不再创建视频输出空闲帧。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_FrmLnkLstElmTotal );
				}
			}
		}

		if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt != NULL ) //如果获取了一个视频输出空闲帧。
		{
			p_VdoOtptStrmPt->m_LastTickMsec = FuncGetTickAsMsec();

			//调用用户定义的写入视频输出帧函数，并解码成YU12原始数据。
			switch( p_VdoOtptStrmPt->m_UseWhatDecd ) //使用什么解码器。
			{
				case 0: //如果使用YU12原始数据。
				{
					//调用用户定义的写入视频输出帧函数。
					p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth = 0;
					p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight = 0;
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx,
																				 p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
																				 NULL, 0, NULL );

					if( ( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth > 0 ) && ( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight > 0 ) ) //如果本次写入了视频输出帧。
					{
						if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用YU12原始数据成功。YU12格式视频输出原始帧宽度：%z32d，高度：%z32d。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight );
					}
					else //如果本次没写入视频输出帧。
					{
						goto Skip;
					}

					//用户定义的获取视频输出帧函数。
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx,
																			   p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
																			   NULL, 0 );
					break;
				}
				case 1: //如果使用OpenH264解码器。
				{
					//调用用户定义的写入视频输出帧函数。
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserWriteVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx,
																				 NULL, NULL, NULL,
																				 p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt, FrmMaxWidth * FrmMaxHeight * 3 / 2, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmLenByt );

					if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmLenByt > 0 ) //如果本次写入了视频输出帧。
					{
						//使用OpenH264解码器。
						if( OpenH264DecdPocs( p_VdoOtptStrmPt->m_OpenH264DecdPt,
											  p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmLenByt,
											  p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, FrmMaxWidth * FrmMaxHeight * 3 / 2, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
											  NULL ) == 0 )
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用OpenH264解码器成功。YU12格式视频输出原始帧宽度：%z32d，高度：%z32d。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight );
							if( ( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth == 0 ) || ( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight == 0 ) ) goto Skip; //如果未解码出YU12格式帧，就把本次视频输出帧丢弃。
						}
						else
						{
							if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：使用OpenH264解码器失败，本次视频输出帧丢弃。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
							goto Skip;
						}
					}
					else //如果本次没写入视频输出帧。
					{
						goto Skip;
					}

					//用户定义的获取视频输出帧函数。
					p_VdoOtptPt->m_MediaPocsThrdPt->m_UserGetVdoOtptFrmFuncPt( p_VdoOtptPt->m_MediaPocsThrdPt, p_VdoOtptStrmPt->m_VdoOtptStrmIdx,
																			   p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
																			   p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmLenByt );
					break;
				}
			}

			//判断视频输出设备是否黑屏。在视频处理完后再设置黑屏，这样可以保证视频处理器的连续性。
			if( p_VdoOtptStrmPt->m_VdoOtptIsBlack != 0 )
			{
				int p_TmpLen = p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth * p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight;
				memset( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, 0, p_TmpLen );
				memset( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt + p_TmpLen, 128, p_TmpLen / 2 );
			}

			//将本次YU12格式视频输出帧转为BGRA格式视频输出帧。
			if( LibYUVPictrFmtCnvrt( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt, PICTR_FMT_BT601F8_YU12_I420, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
									 p_VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt, FrmMaxWidth * FrmMaxHeight * 4, NULL, PICTR_FMT_SRGBF8_BGRA8888,
									 NULL ) != 0 )
			{
				if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：将本次YU12格式视频输出原始帧转为BGRA格式视频输出结果帧失败，本次视频输出帧丢弃。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
				goto Skip;
			}
			
			//显示视频输出帧。
			if( p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl != NULL )
			{
				//绘制视频输出帧到视频输出显示窗口。
				if( LibYUVPictrDrawToWnd( p_VdoOtptStrmPt->m_BgraVdoOtptRlstFrmPt, PICTR_FMT_SRGBF8_BGRA8888, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmWidth, p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmHeight,
										  0,
										  p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl,
										  NULL ) != 0 )
				{
					if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：绘制BGRA格式视频输出结果帧到视频输出显示窗口失败，本次视频输出帧丢弃。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
					goto Skip;
				}
			}
			
			p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_VdoOtptStrmIdx = p_VdoOtptStrmPt->m_VdoOtptStrmIdx; //设置视频输出流索引。
			p_VdoOtptStrmPt->m_VdoOtptFrmElmPt->m_TimeStampMsec = p_VdoOtptStrmPt->m_LastTickMsec; //设置时间戳。

			//追加本次视频输出帧到视频输出帧链表。
			{
				p_VdoOtptPt->m_VdoOtptFrmLnkLst.PutTail( &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt, 1, NULL );
				p_VdoOtptStrmPt->m_VdoOtptFrmElmPt = NULL;
			}

			if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				p_VdoOtptStrmPt->m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本次视频输出帧处理完毕，耗时 %uz64d 毫秒。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx, p_VdoOtptStrmPt->m_NowTickMsec - p_VdoOtptStrmPt->m_LastTickMsec );
			}
		}

		Skip:
        if( p_VdoOtptStrmPt->m_VdoOtptThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本线程接收到退出请求，开始准备退出。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
            goto Out;
        }

        Sleep( 1 ); //暂停一下，避免CPU使用率过高。
    } //视频输出循环完毕。
	
	Out:
	if( p_VdoOtptStrmPt->m_VdoOtptFrmElmPt != NULL ) //如果获取的视频输出空闲帧没有追加到视频输出帧链表。
	{
		p_VdoOtptPt->m_VdoOtptIdleFrmLnkLst.PutTail( &p_VdoOtptStrmPt->m_VdoOtptFrmElmPt, 1, NULL );
		p_VdoOtptStrmPt->m_VdoOtptFrmElmPt = NULL;
	}
	if( p_VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频输出线程：视频输出流索引 %uz32d：本线程已退出。" ), p_VdoOtptStrmPt->m_VdoOtptStrmIdx );
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

void VdoOtptAddVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx )
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

void VdoOtptDelVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx )
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
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 VdoOtptDspyWndHdl：[输入]，存放视频输出显示窗口的句柄，可以为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrm( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, HWND VdoOtptDspyWndHdl )
{
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	
	//查找视频输出流索引。
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_VdoOtptStrmIdx == VdoOtptStrmIdx )
		{
			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );

			p_VdoOtptStrmPt->m_VdoOtptDspyWndHdl = VdoOtptDspyWndHdl;

			if( ( VdoOtptPt->m_IsInitVdoOtpt != 0 ) && ( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm != 0 ) ) VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt );
			return;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：VdoOtptSetVdoOtptStrmUseYU12
 * 功能说明：设置视频输出的视频输出流使用YU12原始数据。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrmUseYU12( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx )
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
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 DecdThrdNum：[输入]，存放OpenH264解码器的解码线程数，单位为个，为0表示直接在调用线程解码，为1或2或3表示解码子线程的数量。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrmUseOpenH264Decd( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t DecdThrdNum )
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
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
			 IsBlack：[输入]，存放视频输出是否黑屏，为0表示有图像，为非0表示黑屏。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrmIsBlack( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t IsBlack )
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
             VdoOtptStrmIdx：[输入]，存放视频输出流索引。
             IsUseVdoOtptStrm：[输入]，存放是否使用视频输出流，为0表示不使用，为非0表示要使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VdoOtptSetVdoOtptStrmIsUse( VdoOtpt * VdoOtptPt, uint32_t VdoOtptStrmIdx, int32_t IsUseVdoOtptStrm )
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
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录初始化开始的嘀嗒钟。
	
	//初始化视频输出帧链表。
	if( VdoOtptPt->m_VdoOtptFrmLnkLst.Init( sizeof( VdoOtptFrmElm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输出帧链表成功。" ) );
	}
	else
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化视频输出帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化视频输出空闲帧链表。
	if( VdoOtptPt->m_VdoOtptIdleFrmLnkLst.Init( sizeof( VdoOtptFrmElm * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化视频输出空闲帧链表成功。" ) );
	}
	else
	{
		if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化视频输出空闲帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void ** )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm )
		{
			if( VdoOtptVdoOtptStrmInit( VdoOtptPt, p_VdoOtptStrmPt ) != 0 ) goto Out;
		}
	}

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：初始化视频输出耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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
	VdoOtptStrm * p_VdoOtptStrmPt; //存放视频输出流的指针。
	VdoOtptFrmElm * p_VdoOtptFrmElmPt; //存放视频输出帧元素的指针。
	
	for( size_t p_VdoOtptStrmLnkLstIdx = SIZE_MAX; VdoOtptPt->m_VdoOtptStrmLnkLst.GetNextByIdx( p_VdoOtptStrmLnkLstIdx, &p_VdoOtptStrmLnkLstIdx, NULL, ( void ** )&p_VdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_VdoOtptStrmPt->m_IsUseVdoOtptStrm )
		{
			VdoOtptVdoOtptStrmDstoy( VdoOtptPt, p_VdoOtptStrmPt );
		}
	}

	//销毁视频输出空闲帧链表。
	if( VdoOtptPt->m_VdoOtptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoOtptPt->m_VdoOtptIdleFrmLnkLst.GetHead( NULL, &p_VdoOtptFrmElmPt, NULL, 0, NULL ) == 0 )
		{
			if( p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt != NULL )
			{
				free( p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt );
				p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt = NULL;
			}
			if( p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt )
			{
				free( p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt );
				p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt = NULL;
			}
			free( p_VdoOtptFrmElmPt );
			p_VdoOtptFrmElmPt = NULL;
			VdoOtptPt->m_VdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoOtptPt->m_VdoOtptIdleFrmLnkLst.Dstoy( VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输出空闲帧链表成功。" ) );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输出空闲帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁视频输出帧链表。
	if( VdoOtptPt->m_VdoOtptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( VdoOtptPt->m_VdoOtptFrmLnkLst.GetHead( NULL, &p_VdoOtptFrmElmPt, NULL, 0, NULL ) == 0 )
		{
			if( p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt != NULL )
			{
				free( p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt );
				p_VdoOtptFrmElmPt->m_EncdVdoOtptSrcFrmPt = NULL;
			}
			if( p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt )
			{
				free( p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt );
				p_VdoOtptFrmElmPt->m_YU12VdoOtptSrcFrmPt = NULL;
			}
			free( p_VdoOtptFrmElmPt );
			p_VdoOtptFrmElmPt = NULL;
			VdoOtptPt->m_VdoOtptFrmLnkLst.DelHead( 0, NULL );
		}
		if( VdoOtptPt->m_VdoOtptFrmLnkLst.Dstoy( VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁视频输出帧链表成功。" ) );
		}
		else
		{
			if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁视频输出帧链表失败。原因：%vs" ), VdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	if( VdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁视频输出成功。" ) );
}
