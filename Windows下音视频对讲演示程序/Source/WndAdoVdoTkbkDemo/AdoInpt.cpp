#include "AdoInpt.h"

DWORD WINAPI AdoInptThrdRun( AdoInpt * AdoInptPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptAecInit
 * 功能说明：初始化音频输入的声学回音消除器。
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

int AdoInptAecInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatAec )
	{
		case 0: //如果不使用声学回音消除器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：不使用声学回音消除器。" ) );
			break;
		}
		case 1: //如果要使用Speex声学回音消除器。
		{
			if( AdoInptPt->m_SpeexAec.m_IsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
			{
				if( SpeexAecInitByMemFile( &AdoInptPt->m_SpeexAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexAec.m_FilterLenMsec, AdoInptPt->m_SpeexAec.m_IsUseRec, AdoInptPt->m_SpeexAec.m_EchoMutp, AdoInptPt->m_SpeexAec.m_EchoCntu, AdoInptPt->m_SpeexAec.m_EchoSupes, AdoInptPt->m_SpeexAec.m_EchoSupesAct, AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：根据Speex声学回音消除器内存块文件 %vs 来初始化Speex声学回音消除器成功。" ), AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：根据Speex声学回音消除器内存块文件 %vs 来初始化Speex声学回音消除器失败。原因：%vs" ), AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
			}
			if( AdoInptPt->m_SpeexAec.m_Pt == NULL )
			{
				if( SpeexAecInit( &AdoInptPt->m_SpeexAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexAec.m_FilterLenMsec, AdoInptPt->m_SpeexAec.m_IsUseRec, AdoInptPt->m_SpeexAec.m_EchoMutp, AdoInptPt->m_SpeexAec.m_EchoCntu, AdoInptPt->m_SpeexAec.m_EchoSupes, AdoInptPt->m_SpeexAec.m_EchoSupesAct, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Speex声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Speex声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
			break;
		}
		case 2: //如果要使用WebRtc定点版声学回音消除器。
		{
			if( WebRtcAecmInit( &AdoInptPt->m_WebRtcAecm.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAecm.m_IsUseCNGMode, AdoInptPt->m_WebRtcAecm.m_EchoMode, AdoInptPt->m_WebRtcAecm.m_Delay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版声学回音消除器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAec.m_IsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
			{
				if( WebRtcAecInitByMemFile( &AdoInptPt->m_WebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAec.m_EchoMode, AdoInptPt->m_WebRtcAec.m_Delay, AdoInptPt->m_WebRtcAec.m_IsUseDelayAgstcMode, AdoInptPt->m_WebRtcAec.m_IsUseExtdFilterMode, AdoInptPt->m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAec.m_IsUseAdaptAdjDelay, AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来初始化WebRtc浮点版声学回音消除器成功。" ), AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
			}
			if( AdoInptPt->m_WebRtcAec.m_Pt == NULL )
			{
				if( WebRtcAecInit( &AdoInptPt->m_WebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAec.m_EchoMode, AdoInptPt->m_WebRtcAec.m_Delay, AdoInptPt->m_WebRtcAec.m_IsUseDelayAgstcMode, AdoInptPt->m_WebRtcAec.m_IsUseExtdFilterMode, AdoInptPt->m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAec.m_IsUseAdaptAdjDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
			break;
		}
		case 4: //如果要使用SpeexWebRtc三重声学回音消除器。
		{
			if( SpeexWebRtcAecInit( &AdoInptPt->m_SpeexWebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexWebRtcAec.m_WorkMode, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecIsUseRec, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoMutp, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoCntu, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoSupes, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmEchoMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmDelay, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecEchoMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecDelay, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_SpeexWebRtcAec.m_IsUseSameRoomAec, AdoInptPt->m_SpeexWebRtcAec.m_SameRoomEchoMinDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化SpeexWebRtc三重声学回音消除器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化SpeexWebRtc三重声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoInptAecDstoy
 * 功能说明：销毁音频输入的声学回音消除器。
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

void AdoInptAecDstoy( AdoInpt * AdoInptPt )
{
	switch( AdoInptPt->m_UseWhatAec )
	{
		case 0: //如果不使用声学回音消除器。
		{
			break;
		}
		case 1: //如果要使用Speex声学回音消除器。
		{
			if( AdoInptPt->m_SpeexAec.m_Pt != NULL )
			{
				if( AdoInptPt->m_SpeexAec.m_IsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
				{
					if( SpeexAecSaveMemFile( AdoInptPt->m_SpeexAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexAec.m_FilterLenMsec, AdoInptPt->m_SpeexAec.m_IsUseRec, AdoInptPt->m_SpeexAec.m_EchoMutp, AdoInptPt->m_SpeexAec.m_EchoCntu, AdoInptPt->m_SpeexAec.m_EchoSupes, AdoInptPt->m_SpeexAec.m_EchoSupesAct, AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：将Speex声学回音消除器内存块保存到指定的文件 %vs 成功。" ), AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：将Speex声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), AdoInptPt->m_SpeexAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
				if( SpeexAecDstoy( AdoInptPt->m_SpeexAec.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Speex声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁Speex声学回音消除器失败。" ) );
				}
				AdoInptPt->m_SpeexAec.m_Pt = NULL;
			}
			break;
		}
		case 2: //如果要使用WebRtc定点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAecm.m_Pt != NULL )
			{
				if( WebRtcAecmDstoy( AdoInptPt->m_WebRtcAecm.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAecm.m_Pt = NULL;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAec.m_Pt != NULL )
			{
				if( AdoInptPt->m_WebRtcAec.m_IsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
				{
					if( WebRtcAecSaveMemFile( AdoInptPt->m_WebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAec.m_EchoMode, AdoInptPt->m_WebRtcAec.m_Delay, AdoInptPt->m_WebRtcAec.m_IsUseDelayAgstcMode, AdoInptPt->m_WebRtcAec.m_IsUseExtdFilterMode, AdoInptPt->m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAec.m_IsUseAdaptAdjDelay, AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 成功。" ), AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), AdoInptPt->m_WebRtcAec.m_MemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
				if( WebRtcAecDstoy( AdoInptPt->m_WebRtcAec.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAec.m_Pt = NULL;
			}
			break;
		}
		case 4: //如果要使用SpeexWebRtc三重声学回音消除器。
		{
			if( AdoInptPt->m_SpeexWebRtcAec.m_Pt != NULL )
			{
				if( SpeexWebRtcAecDstoy( AdoInptPt->m_SpeexWebRtcAec.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁SpeexWebRtc三重声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁SpeexWebRtc三重声学回音消除器失败。" ) );
				}
				AdoInptPt->m_SpeexWebRtcAec.m_Pt = NULL;
			}
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptSetIsCanUseAec
 * 功能说明：设置音频输入是否可以使用声学回音消除器。
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

void AdoInptSetIsCanUseAec( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_UseWhatAec != 0 ) //如果要使用声学回音消除器。
	{
		if( AdoInptPt->m_IsUse == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为不使用音频输入，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsUse == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为不使用音频输出，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate != AdoInptPt->m_SmplRate )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为音频输出的采样频率与音频输入不一致，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenMsec != AdoInptPt->m_FrmLenMsec ) || ( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit != AdoInptPt->m_FrmLenUnit ) )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为音频输出帧的长度与音频输入不一致，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为要使用声学回音消除器，所以可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 1; //设置是否可以使用声学回音消除器为可以。
		}
	}
	else //如果不使用声学回音消除器。
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：因为不使用声学回音消除器，所以不可以使用声学回音消除器。" ) );
		AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptNsInit
 * 功能说明：初始化音频输入的噪音抑制器。
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

int AdoInptNsInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatNs )
	{
		case 0: //如果不使用噪音抑制器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：不使用噪音抑制器。" ) );
			break;
		}
		case 1: //如果要使用Speex预处理器的噪音抑制。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" ) );
			break;
		}
		case 2: //如果要使用WebRtc定点版噪音抑制器。
		{
			if( WebRtcNsxInit( &AdoInptPt->m_WebRtcNsx.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcNsx.m_PolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版噪音抑制器。
		{
			if( WebRtcNsInit( &AdoInptPt->m_WebRtcNs.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcNs.m_PolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 4: //如果要使用RNNoise噪音抑制器。
		{
			if( RNNoiseInit( &AdoInptPt->m_RNNoise.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化RNNoise噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化RNNoise噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoInptNsDstoy
 * 功能说明：销毁音频输入的噪音抑制器。
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

void AdoInptNsDstoy( AdoInpt * AdoInptPt )
{
	switch( AdoInptPt->m_UseWhatNs )
	{
		case 0: //如果不使用噪音抑制器。
		{
			break;
		}
		case 1: //如果要使用Speex预处理器的噪音抑制。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" ) );
			break;
		}
		case 2: //如果要使用WebRtc定点版噪音抑制器。
		{
			if( AdoInptPt->m_WebRtcNsx.m_Pt != NULL )
			{
				if( WebRtcNsxDstoy( AdoInptPt->m_WebRtcNsx.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版噪音抑制器失败。" ) );
				}
				AdoInptPt->m_WebRtcNsx.m_Pt = NULL;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版噪音抑制器。
		{
			if( AdoInptPt->m_WebRtcNs.m_Pt != NULL )
			{
				if( WebRtcNsDstoy( AdoInptPt->m_WebRtcNs.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版噪音抑制器失败。" ) );
				}
				AdoInptPt->m_WebRtcNs.m_Pt = NULL;
			}
			break;
		}
		case 4: //如果要使用RNNoise噪音抑制器。
		{
			if( AdoInptPt->m_RNNoise.m_Pt != NULL )
			{
				if( RNNoiseDstoy( AdoInptPt->m_RNNoise.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁RNNoise噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁RNNoise噪音抑制器失败。" ) );
				}
				AdoInptPt->m_RNNoise.m_Pt = NULL;
			}
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptSpeexPrpocsInit
 * 功能说明：初始化音频输入的Speex预处理器。
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

int AdoInptSpeexPrpocsInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( AdoInptPt->m_UseWhatNs == 1 ) || ( AdoInptPt->m_SpeexPrpocs.m_IsUseSpeexPrpocs != 0 ) )
	{
		if( SpeexPrpocsInit( &AdoInptPt->m_SpeexPrpocs.m_Pt,
							 AdoInptPt->m_SmplRate,
							 AdoInptPt->m_FrmLenUnit,
							 ( AdoInptPt->m_UseWhatNs == 1 ) ? AdoInptPt->m_SpeexPrpocsNs.m_IsUseNs : 0,
							 AdoInptPt->m_SpeexPrpocsNs.m_NoiseSupes,
							 ( AdoInptPt->m_UseWhatNs == 1 ) ? AdoInptPt->m_SpeexPrpocsNs.m_IsUseDereverb : 0,
							 ( AdoInptPt->m_SpeexPrpocs.m_IsUseSpeexPrpocs != 0 ) ? AdoInptPt->m_SpeexPrpocs.m_IsUseVad : 0,
							 AdoInptPt->m_SpeexPrpocs.m_VadProbStart,
							 AdoInptPt->m_SpeexPrpocs.m_VadProbCntu,
							 ( AdoInptPt->m_SpeexPrpocs.m_IsUseSpeexPrpocs != 0 ) ? AdoInptPt->m_SpeexPrpocs.m_IsUseAgc : 0,
							 AdoInptPt->m_SpeexPrpocs.m_AgcLevel,
							 AdoInptPt->m_SpeexPrpocs.m_AgcIncrement,
							 AdoInptPt->m_SpeexPrpocs.m_AgcDecrement,
							 AdoInptPt->m_SpeexPrpocs.m_AgcMaxGain,
							 AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Speex预处理器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Speex预处理器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoInptSpeexPrpocsDstoy
 * 功能说明：销毁音频输入的Speex预处理器。
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

void AdoInptSpeexPrpocsDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_SpeexPrpocs.m_Pt != NULL )
	{
		if( SpeexPrpocsDstoy( AdoInptPt->m_SpeexPrpocs.m_Pt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Speex预处理器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁Speex预处理器失败。" ) );
		}
		AdoInptPt->m_SpeexPrpocs.m_Pt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptEncdInit
 * 功能说明：初始化音频输入的编码器。
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

int AdoInptEncdInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用PCM原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化PCM原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用Speex编码器。
		{
			if( AdoInptPt->m_FrmLenMsec != 20 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：帧的长度不为20毫秒不能使用Speex编码器。" ) );
				goto Out;
			}
			if( SpeexEncdInit( &AdoInptPt->m_SpeexEncd.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_SpeexEncd.m_UseCbrOrVbr, AdoInptPt->m_SpeexEncd.m_Qualt, AdoInptPt->m_SpeexEncd.m_Cmplxt, AdoInptPt->m_SpeexEncd.m_PlcExptLossRate ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Speex编码器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Speex编码器失败。" ) );
				goto Out;
			}
			break;
		}
		case 2: //如果要使用Opus编码器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：暂不支持使用Opus编码器。" ) );
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
 * 函数名称：AdoInptEncdDstoy
 * 功能说明：销毁音频输入的编码器。
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

void AdoInptEncdDstoy( AdoInpt * AdoInptPt )
{
	switch( AdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果要使用PCM原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁PCM原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用Speex编码器。
		{
			if( AdoInptPt->m_SpeexEncd.m_Pt != NULL )
			{
				if( SpeexEncdDstoy( AdoInptPt->m_SpeexEncd.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Speex编码器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁Speex编码器失败。" ) );
				}
				AdoInptPt->m_SpeexEncd.m_Pt = NULL;
			}
			break;
		}
		case 2: //如果要使用Opus编码器。
		{
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptWavfmInit
 * 功能说明：初始化音频输入的波形器。
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

int AdoInptWavfmInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_Wavfm.m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoWavfmInit( &AdoInptPt->m_Wavfm.m_SrcWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化原始波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化原始波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		if( AdoWavfmInit( &AdoInptPt->m_Wavfm.m_RsltWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化结果波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化结果波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoInptWavfmDstoy
 * 功能说明：销毁音频输入的波形器。
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

void AdoInptWavfmDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_Wavfm.m_SrcWavfmPt != NULL )
	{
		if( AdoWavfmDstoy( AdoInptPt->m_Wavfm.m_SrcWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁原始波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁原始波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_Wavfm.m_SrcWavfmPt = NULL;
	}
	if( AdoInptPt->m_Wavfm.m_RsltWavfmPt != NULL )
	{
		if( AdoWavfmDstoy( AdoInptPt->m_Wavfm.m_RsltWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁结果波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁结果波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_Wavfm.m_RsltWavfmPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptWaveFileWriterInit
 * 功能说明：初始化音频输入的Wave文件写入器。
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

int AdoInptWaveFileWriterInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_WaveFile.m_IsSaveAdoToWaveFile != 0 )
	{
		if( WaveFileWriterInit( &AdoInptPt->m_WaveFile.m_SrcWaveFileWriterPt, AdoInptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt, AdoInptPt->m_WaveFile.m_WaveFileWrBufSzByt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化原始Wave文件 %vs 的Wave文件写入器成功。" ), AdoInptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化原始Wave文件 %vs 的Wave文件写入器失败。" ), AdoInptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt );
			goto Out;
		}
		if( WaveFileWriterInit( &AdoInptPt->m_WaveFile.m_RsltWaveFileWriterPt, AdoInptPt->m_WaveFile.m_RsltWaveFileFullPathVstrPt, AdoInptPt->m_WaveFile.m_WaveFileWrBufSzByt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化结果Wave文件 %vs 的Wave文件写入器成功。" ), AdoInptPt->m_WaveFile.m_RsltWaveFileFullPathVstrPt );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化结果Wave文件 %vs 的Wave文件写入器失败。" ), AdoInptPt->m_WaveFile.m_RsltWaveFileFullPathVstrPt );
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
 * 函数名称：AdoInptWaveFileWriterDstoy
 * 功能说明：销毁音频输入的Wave文件写入器。
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

void AdoInptWaveFileWriterDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_WaveFile.m_SrcWaveFileWriterPt != NULL )
	{
		if( WaveFileWriterDstoy( AdoInptPt->m_WaveFile.m_SrcWaveFileWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁原始Wave文件写入器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁原始Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_WaveFile.m_SrcWaveFileWriterPt = NULL;
	}
	if( AdoInptPt->m_WaveFile.m_RsltWaveFileWriterPt != NULL )
	{
		if( WaveFileWriterDstoy( AdoInptPt->m_WaveFile.m_RsltWaveFileWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁结果Wave文件写入器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁结果Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_WaveFile.m_RsltWaveFileWriterPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptDvcAndThrdInit
 * 功能说明：初始化音频输入的设备和线程。
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

int AdoInptDvcAndThrdInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//初始化设备。
	{
		HRESULT p_HRslt;

		if( ( AdoInptPt->m_IsUseSystemAecNsAgc != 0 ) && ( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) ) //如果要使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器，且已初始化音频输出。如果没有音频输出，则系统自带的声学回音消除器、噪音抑制器和自动增益控制器不会输出音频数据。
		{
			//初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象。
			{
				p_HRslt = CoCreateInstance( CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, ( void * * )&AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}

				IPropertyStore * p_PrptStorPt;
				PROPVARIANT p_ProVar = { 0 };
				p_HRslt = AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt->QueryInterface( IID_IPropertyStore, ( void * * )&p_PrptStorPt );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的属性接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}
				p_ProVar.vt = VT_I4;
				p_ProVar.intVal = ( ( AdoInptPt->m_Dvc.m_ID - 1 ) & 0xFFFF ) | ( ( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ID - 1 ) & 0xFFFF ) << 16 );
				p_PrptStorPt->SetValue( MFPKEY_WMAAECMA_DEVICE_INDEXES, p_ProVar ); //设置音频输入输出设备的索引。
				p_PrptStorPt->Release();

				DMO_MEDIA_TYPE p_MediaTyp;
				p_MediaTyp.majortype = MEDIATYPE_Audio;
				p_MediaTyp.subtype = MEDIASUBTYPE_PCM;
				p_MediaTyp.lSampleSize = 0;
				p_MediaTyp.bFixedSizeSamples = TRUE;
				p_MediaTyp.bTemporalCompression = FALSE;
				p_MediaTyp.formattype = FORMAT_WaveFormatEx;

				p_HRslt = MoInitMediaType( &p_MediaTyp, sizeof( WAVEFORMATEX ) );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化媒体类型失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}

				WAVEFORMATEX * p_WaveFmtExPt = ( WAVEFORMATEX * )p_MediaTyp.pbFormat;
				p_WaveFmtExPt->wFormatTag = WAVE_FORMAT_PCM;
				p_WaveFmtExPt->nChannels = 1;
				p_WaveFmtExPt->nSamplesPerSec = ( AdoInptPt->m_SmplRate <= 22050 ) ? AdoInptPt->m_SmplRate : 22050;
				p_WaveFmtExPt->nAvgBytesPerSec = p_WaveFmtExPt->nSamplesPerSec * 2;
				p_WaveFmtExPt->wBitsPerSample = 16;
				p_WaveFmtExPt->nBlockAlign = p_WaveFmtExPt->wBitsPerSample / 8;
				p_WaveFmtExPt->cbSize = 0;
				AdoInptPt->m_Dvc.m_WaveFmtExPt = ( WAVEFORMATEX * )CoTaskMemAlloc( sizeof( WAVEFORMATEX ) );
				if( AdoInptPt->m_Dvc.m_WaveFmtExPt == NULL )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：创建格式的内存块失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}
				memcpy( AdoInptPt->m_Dvc.m_WaveFmtExPt, p_WaveFmtExPt, sizeof( WAVEFORMATEX ) );
				AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit = AdoInptPt->m_FrmLenMsec * AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec / 1000; //设置Pcm格式缓冲区帧的长度。
				AdoInptPt->m_Dvc.m_PcmBufFrmLenByt = AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit * sizeof( int16_t ); //设置Pcm格式缓冲区帧的长度。

				p_HRslt = AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt->SetOutputType( 0, &p_MediaTyp, 0 );
				if( p_HRslt == S_FALSE )
				{
					MoFreeMediaType( &p_MediaTyp );
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：设置媒体类型失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}

				MoFreeMediaType( &p_MediaTyp );

				p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoInptPt->m_Dvc.m_GlblIntfcTablePt ); //初始化设备全局接口表。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备的全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}

				p_HRslt = AdoInptPt->m_Dvc.m_GlblIntfcTablePt->RegisterInterfaceInGlobal( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt, IID_IMediaObject, &AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie ); //注册系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象到全局接口表。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：注册系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}
			}

			//初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象。
			{
				AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt = ( DMO_OUTPUT_DATA_BUFFER * )calloc( 1, sizeof( DMO_OUTPUT_DATA_BUFFER ) );
				if( AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt == NULL )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象失败。原因：内存不足。" ) );
					goto Out;
				}
				class IMediaBufferImpl final : public IMediaBuffer
				{
				public:
					IMediaBufferImpl()
					{
						m_BufLenByt = 0;
						m_RefCount = 0;
						AddRef();
					}

					~IMediaBufferImpl()
					{

					}

					HRESULT STDMETHODCALLTYPE SetLength( DWORD cbLength )
					{
						int p_Rslt = S_FALSE; //存放本函数的执行结果，为0表示成功，为非0表示失败。

						if( cbLength > sizeof( m_BufArr ) )
						{
							goto Out;
						}

						m_BufLenByt = cbLength;

						p_Rslt = S_OK; //设置本函数执行成功。

						Out:
						//if( p_Rslt != 0 ) //如果本函数执行失败。
						{

						}
						return p_Rslt;
					}

					HRESULT STDMETHODCALLTYPE GetMaxLength( DWORD * pcbMaxLength )
					{
						int p_Rslt = S_FALSE; //存放本函数的执行结果，为0表示成功，为非0表示失败。

						if( pcbMaxLength == NULL )
						{
							goto Out;
						}

						*pcbMaxLength = sizeof( m_BufArr );

						p_Rslt = S_OK; //设置本函数执行成功。

						Out:
						//if( p_Rslt != 0 ) //如果本函数执行失败。
						{

						}
						return p_Rslt;
					}

					HRESULT STDMETHODCALLTYPE GetBufferAndLength( _Outptr_opt_result_bytebuffer_( *pcbLength ) BYTE * * ppBuffer, DWORD * pcbLength )
					{
						int p_Rslt = S_FALSE; //存放本函数的执行结果，为0表示成功，为非0表示失败。

						if( ppBuffer != NULL ) *ppBuffer = m_BufArr;
						if( pcbLength != NULL ) *pcbLength = m_BufLenByt;

						p_Rslt = S_OK; //设置本函数执行成功。

						Out:
						//if( p_Rslt != 0 ) //如果本函数执行失败。
						{

						}
						return p_Rslt;
					}

					STDMETHOD_( ULONG, AddRef() )
					{
						return InterlockedIncrement( &m_RefCount );
					}

					STDMETHOD( QueryInterface( REFIID riid, void * * ppv ) )
					{
						if( !ppv )
						{
							return E_POINTER;
						}
						else if( riid != IID_IMediaBuffer && riid != IID_IUnknown )
						{
							return E_NOINTERFACE;
						}

						*ppv = static_cast< IMediaBuffer * >( this );
						AddRef();
						return S_OK;
					}

					STDMETHOD_( ULONG, Release() )
					{
						LONG p_RefCount = InterlockedDecrement( &m_RefCount );
						if( p_RefCount == 0 )
						{
							delete this;
						}

						return p_RefCount;
					}

					BYTE m_BufArr[ 4096 ];
					DWORD m_BufLenByt;
					ULONG m_RefCount;
				};
				AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer = new IMediaBufferImpl();
				if( AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer == NULL )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象失败。原因：内存不足。" ) );
					goto Out;
				}
			}
		}
		else //如果不使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
		{
			p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&AdoInptPt->m_Dvc.m_EnumPt ); //初始化设备枚举器。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备枚举器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			if( AdoInptPt->m_Dvc.m_ID == 0 ) //如果要使用默认的设备。
			{
				p_HRslt = AdoInptPt->m_Dvc.m_EnumPt->GetDefaultAudioEndpoint( eCapture, eConsole, &AdoInptPt->m_Dvc.m_Pt ); //初始化设备。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}
			}
			else //如果要使用指定的设备。
			{
				p_HRslt = AdoInptPt->m_Dvc.m_EnumPt->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &AdoInptPt->m_Dvc.m_ClctPt ); //获取设备收集器。只收集激活的设备。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取设备收集器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}

				p_HRslt = AdoInptPt->m_Dvc.m_ClctPt->Item( AdoInptPt->m_Dvc.m_ID - 1, &AdoInptPt->m_Dvc.m_Pt ); //初始化设备。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：音频输入：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto Out;
				}
			}

			p_HRslt = AdoInptPt->m_Dvc.m_Pt->Activate( IID_IAudioClient, CLSCTX_INPROC_SERVER, NULL, ( void * * )&AdoInptPt->m_Dvc.m_ClntPt ); //激活设备的客户端接口。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：激活设备的客户端接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->GetMixFormat( &AdoInptPt->m_Dvc.m_WaveFmtExPt ); //获取设备的格式。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取设备的格式失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			//修改格式为16位整型，不能修改采样频率和声道数。
			if( AdoInptPt->m_Dvc.m_WaveFmtExPt->wFormatTag != WAVE_FORMAT_EXTENSIBLE )
			{
				AdoInptPt->m_Dvc.m_WaveFmtExPt->wFormatTag = WAVE_FORMAT_PCM;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample = 16;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign = AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels * AdoInptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample / 8;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->nAvgBytesPerSec = AdoInptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign * AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
			}
			else
			{
				( ( WAVEFORMATEXTENSIBLE * )AdoInptPt->m_Dvc.m_WaveFmtExPt )->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
				( ( WAVEFORMATEXTENSIBLE * )AdoInptPt->m_Dvc.m_WaveFmtExPt )->Samples.wValidBitsPerSample = 16;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample = 16;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign = AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels * AdoInptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample / 8;
				AdoInptPt->m_Dvc.m_WaveFmtExPt->nAvgBytesPerSec = AdoInptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign * AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
			}

			p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, 1000 * 1000 * 10, 0, AdoInptPt->m_Dvc.m_WaveFmtExPt, NULL ); //初始化设备客户端。这里缓冲区大小要比帧的长度大一点，就算1000 * 1000 * 10 * 100纳秒=1000毫秒，否则缓冲区很快就会被填满导致丢数据。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备的客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->GetBufferSize( &AdoInptPt->m_Dvc.m_BufSzUnit ); //获取设备的缓冲区大小。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取设备的缓冲区大小失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->GetService( IID_IAudioCaptureClient, ( void * * )&AdoInptPt->m_Dvc.m_CptrClntPt ); //获取设备的捕获客户端。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取设备的捕获客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
			
			p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoInptPt->m_Dvc.m_GlblIntfcTablePt ); //初始化设备全局接口表。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备的全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoInptPt->m_Dvc.m_GlblIntfcTablePt->RegisterInterfaceInGlobal( AdoInptPt->m_Dvc.m_CptrClntPt, IID_IAudioCaptureClient, &AdoInptPt->m_Dvc.m_CptrClntCookie ); //注册设备的捕获客户端到全局接口表。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：注册设备的捕获客户端到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit = AdoInptPt->m_FrmLenMsec * AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec / 1000; //设置Pcm格式缓冲区帧的长度。
		AdoInptPt->m_Dvc.m_PcmBufFrmLenByt = AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit * sizeof( int16_t ); //设置Pcm格式缓冲区帧的长度。
		if( AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.Init( AdoInptPt->m_Dvc.m_PcmBufFrmLenByt, BufAutoAdjMethFreeNumber, AdoInptPt->m_Dvc.m_PcmBufFrmLenByt, AdoInptPt->m_Dvc.m_PcmBufFrmLenByt * 50, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式缓冲区帧的缓冲区队列失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}

		AdoInptPt->m_Dvc.m_PcmBufFrmPt = ( int16_t * )malloc( AdoInptPt->m_Dvc.m_PcmBufFrmLenByt ); //创建Pcm格式缓冲区帧的内存块。
		if( AdoInptPt->m_Dvc.m_PcmBufFrmPt == NULL )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：音频输入：创建Pcm格式缓冲区帧的内存块失败。原因：内存不足。" ) );
			goto Out;
		}

		if( ( AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec != AdoInptPt->m_SmplRate ) && ( SpeexResamplerInit( &AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt, AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec, AdoInptPt->m_SmplRate, 3, NULL ) != 0 ) )
		{
			LOGE( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式原始帧Speex重采样器失败。" ) );
			goto Out;
		}

		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
		{
			if( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt != NULL ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化系统自带的声学回音消除器、噪音抑制器和自动增益控制器成功。" ) );
			else LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化设备成功。采样频率：%z32d, 声道数：%z16d，缓冲区大小：%uz32d。" ), AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec, AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels, AdoInptPt->m_Dvc.m_BufSzUnit );
		}
	}

	//初始化Pcm格式原始帧容器。
	if( AdoInptPt->m_PcmSrcFrmCntnr.Init( sizeof( int16_t * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式原始帧容器成功。" ) );
	}
	else
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式原始帧容器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化Pcm格式空闲帧容器。
	if( AdoInptPt->m_PcmIdleFrmCntnr.Init( sizeof( int16_t * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式空闲帧容器成功。" ) );
	}
	else
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式空闲帧容器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化线程的临时变量。
	{
		AdoInptPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程的临时变量。
		AdoInptPt->m_Thrd.m_PcmSrcFrmPt = NULL; //初始化Pcm格式原始帧的指针。
		AdoInptPt->m_Thrd.m_ElmTotal = 0; //初始化元素总数。
		AdoInptPt->m_Thrd.m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		AdoInptPt->m_Thrd.m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：初始化线程的临时变量成功。" ) );
	}

	//初始化线程。
	{
		AdoInptPt->m_Thrd.m_IsStartAdoOtptThrd = 0; //设置未开始音频输出线程。
		AdoInptPt->m_Thrd.m_ExitFlag = 0; //设置退出标记为0表示保持运行。
		if( ThrdInit( &AdoInptPt->m_Thrd.m_ThrdInfoPt, ( LPTHREAD_START_ROUTINE )AdoInptThrdRun, AdoInptPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化线程成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化线程失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoInptDvcAndThrdDstoy
 * 功能说明：销毁音频输入的设备和线程。
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

void AdoInptDvcAndThrdDstoy( AdoInpt * AdoInptPt )
{
	//销毁线程。
	if( AdoInptPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		AdoInptPt->m_Thrd.m_ExitFlag = 1; //请求线程退出。
		ThrdWaitDstoy( AdoInptPt->m_Thrd.m_ThrdInfoPt, NULL ); //等待线程退出并销毁。
		AdoInptPt->m_Thrd.m_ThrdInfoPt = NULL;
		AdoInptPt->m_Thrd.m_ExitFlag = 0;
		AdoInptPt->m_Thrd.m_IsStartAdoOtptThrd = 0;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁线程成功。" ) );
	}

	//销毁线程的临时变量。
	if( AdoInptPt->m_Thrd.m_IsInitThrdTmpVar != 0 )
	{
		AdoInptPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程的临时变量。
		AdoInptPt->m_Thrd.m_PcmSrcFrmPt = NULL; //销毁Pcm格式原始帧的指针。
		AdoInptPt->m_Thrd.m_ElmTotal = 0; //销毁元素总数。
		AdoInptPt->m_Thrd.m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		AdoInptPt->m_Thrd.m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁线程的临时变量成功。" ) );
	}

	//销毁Pcm格式空闲帧容器。
	if( AdoInptPt->m_PcmIdleFrmCntnr.m_CQueuePt != NULL )
	{
		while( AdoInptPt->m_PcmIdleFrmCntnr.GetHead( &AdoInptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			free( AdoInptPt->m_Thrd.m_PcmSrcFrmPt );
			AdoInptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
		}
		if( AdoInptPt->m_PcmIdleFrmCntnr.Dstoy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Pcm格式空闲帧容器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁Pcm格式空闲帧容器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	//销毁Pcm格式原始帧容器。
	if( AdoInptPt->m_PcmSrcFrmCntnr.m_CQueuePt != NULL )
	{
		while( AdoInptPt->m_PcmSrcFrmCntnr.GetHead( &AdoInptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			free( AdoInptPt->m_Thrd.m_PcmSrcFrmPt );
			AdoInptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
		}
		if( AdoInptPt->m_PcmSrcFrmCntnr.Dstoy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Pcm格式原始帧容器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁Pcm格式原始帧容器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁设备。
	{
		if( AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt != NULL ) //销毁Pcm格式原始帧Speex重采样器。
		{
			SpeexResamplerDstoy( AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt );
			AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt = NULL;
		}
		if( AdoInptPt->m_Dvc.m_PcmBufFrmPt != NULL ) //销毁Pcm格式缓冲区帧。
		{
			free( AdoInptPt->m_Dvc.m_PcmBufFrmPt );
			AdoInptPt->m_Dvc.m_PcmBufFrmPt = NULL;
		}
		if( AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.m_BufQueuePt != NULL ) //销毁Pcm格式缓冲区帧的缓冲区队列。
		{
			AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.Dstoy( NULL );
		}
		AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit = 0; //销毁Pcm格式缓冲区帧的长度。
		AdoInptPt->m_Dvc.m_PcmBufFrmLenByt = 0; //销毁Pcm格式缓冲区帧的长度。
		if( AdoInptPt->m_Dvc.m_CptrClntCookie != 0 ) //销毁设备捕获客户端的Cookie。
		{
			AdoInptPt->m_Dvc.m_GlblIntfcTablePt->RevokeInterfaceFromGlobal( AdoInptPt->m_Dvc.m_CptrClntCookie );
			AdoInptPt->m_Dvc.m_CptrClntCookie = 0;
		}
		if( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie != 0 ) //销毁系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的Cookie。
		{
			AdoInptPt->m_Dvc.m_GlblIntfcTablePt->RevokeInterfaceFromGlobal( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie );
			AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie = 0;
		}
		if( AdoInptPt->m_Dvc.m_GlblIntfcTablePt != NULL ) //销毁设备的全局接口表。
		{
			AdoInptPt->m_Dvc.m_GlblIntfcTablePt->Release();
			AdoInptPt->m_Dvc.m_GlblIntfcTablePt = NULL;
		}
		if( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt != NULL )
		{
			if( AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt != NULL ) //销毁系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象。
			{
				if( AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer != NULL )
				{
					delete AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer;
					AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer = NULL;
				}
				free( AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt );
				AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt != NULL ) //销毁系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象。
			{
				AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt->FreeStreamingResources();
				AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt->Release();
				AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjPt = NULL;
			}
			AdoInptPt->m_Dvc.m_IsClos = 0;
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁系统自带的声学回音消除器、噪音抑制器和自动增益控制器成功。" ) );
		}
		if( AdoInptPt->m_Dvc.m_Pt != NULL )
		{
			if( AdoInptPt->m_Dvc.m_CptrClntPt != NULL ) //销毁设备的捕获客户端。
			{
				AdoInptPt->m_Dvc.m_CptrClntPt->Release();
				AdoInptPt->m_Dvc.m_CptrClntPt = NULL;
			}
			AdoInptPt->m_Dvc.m_BufSzUnit = 0; //销毁设备的缓冲区大小。
			if( AdoInptPt->m_Dvc.m_WaveFmtExPt != NULL ) //销毁设备的格式。
			{
				CoTaskMemFree( AdoInptPt->m_Dvc.m_WaveFmtExPt );
				AdoInptPt->m_Dvc.m_WaveFmtExPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_ClntPt != NULL ) //销毁设备的客户端。
			{
				AdoInptPt->m_Dvc.m_ClntPt->Stop();
				AdoInptPt->m_Dvc.m_ClntPt->Release();
				AdoInptPt->m_Dvc.m_ClntPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_Pt != NULL ) //销毁设备。
			{
				AdoInptPt->m_Dvc.m_Pt->Release();
				AdoInptPt->m_Dvc.m_Pt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_ClctPt != NULL ) //销毁设备收集器。
			{
				AdoInptPt->m_Dvc.m_ClctPt->Release();
				AdoInptPt->m_Dvc.m_ClctPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_EnumPt != NULL ) //销毁设备枚举器。
			{
				AdoInptPt->m_Dvc.m_EnumPt->Release();
				AdoInptPt->m_Dvc.m_EnumPt = NULL;
			}
			AdoInptPt->m_Dvc.m_IsClos = 0;
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁设备成功。" ) );
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptInit
 * 功能说明：初始化音频输入。
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

int AdoInptInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录初始化开始的嘀嗒钟。
	
	if( AdoInptAecInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptNsInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptSpeexPrpocsInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptEncdInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptWavfmInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptWaveFileWriterInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptDvcAndThrdInit( AdoInptPt ) != 0 ) goto Out;

	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		AdoInptDstoy( AdoInptPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptDstoy
 * 功能说明：销毁音频输入。
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

void AdoInptDstoy( AdoInpt * AdoInptPt )
{
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录销毁开始的嘀嗒钟。

	AdoInptDvcAndThrdDstoy( AdoInptPt );
	AdoInptWaveFileWriterDstoy( AdoInptPt );
	AdoInptWavfmDstoy( AdoInptPt );
	AdoInptEncdDstoy( AdoInptPt );
	AdoInptSpeexPrpocsDstoy( AdoInptPt );
	AdoInptNsDstoy( AdoInptPt );
	AdoInptAecDstoy( AdoInptPt );
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录销毁结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}
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

DWORD WINAPI AdoInptThrdRun( AdoInpt * AdoInptPt )
{
	IMediaObject * p_SystemAecNsAgcMediaObjPt = NULL; //存放系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的指针。
	IAudioCaptureClient * p_DvcCptrClntPt = NULL; //存放设备捕获客户端的指针。
	int16_t * p_PcmBufFrmPt; //存放Pcm格式缓冲区帧的指针。
	UINT32 p_DvcBufLenUnit; //存放设备缓冲区的长度，单位为采样单元。
	UINT32 p_DvcBufLenByt; //存放设备缓冲区的长度，单位为字节。
	DWORD p_DvcBufFlag;
	size_t p_TmpSz;
	HRESULT p_HRslt;

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	/*FILE * AdoInptFile1Pt = NULL;
	FILE * AdoInptFile2Pt = NULL;
	FILE * AdoInptFile3Pt = NULL;
	char path[ 1024 ];
	sprintf( path, "F:\\%lld-AdoInptAndOtpt-Inpt1.pcm", time( NULL ) );
	AdoInptFile1Pt = fopen( path, "wb+" );
	sprintf( path, "F:\\%lld-AdoInptAndOtpt-Inpt2.pcm", time( NULL ) );
	AdoInptFile2Pt = fopen( path, "wb+" );
	sprintf( path, "F:\\%lld-AdoInptAndOtpt-Inpt3.pcm", time( NULL ) );
	AdoInptFile3Pt = fopen( path, "wb+" );*/

	//获取设备捕获客户端。
	{
		IGlobalInterfaceTable * p_DvcGlblIntfcTablePt;
		CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&p_DvcGlblIntfcTablePt );
		if( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie != 0 )
		{
			p_DvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie, IID_IMediaObject, ( void * * )&p_SystemAecNsAgcMediaObjPt );
		}
		else
		{
			p_DvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoInptPt->m_Dvc.m_CptrClntCookie, IID_IAudioCaptureClient, ( void * * )&p_DvcCptrClntPt );
		}
		p_DvcGlblIntfcTablePt->Release();
	}
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：开始准备音频输入。" ) );

	//自适应设置声学回音的延迟。
	if( AdoInptPt->m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器，就自适应计算声学回音的延迟，并设置到声学回音消除器。
	{
		int32_t p_AdoInptDelay; //存放音频输入延迟。
		int32_t p_AdoOtptDelay; //存放音频输出延迟。
		int32_t p_Delay; //存放声学回音的延迟，单位为毫秒。
		int32_t p_TmpInt32;

		p_AdoInptDelay = /*AdoInptPt->m_Dvc.m_BufSzUnit * 1000 / AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec*/10; //音频输入的延迟固定为10毫秒。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：音频输入延迟 %z32d 毫秒。" ), p_AdoInptDelay );
		p_AdoOtptDelay = AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_BufSzUnit * 1000 / AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：音频输出延迟 %z32d 毫秒。" ), p_AdoOtptDelay );
		p_Delay = p_AdoInptDelay + p_AdoOtptDelay;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：声学回音延迟 %z32d 毫秒。" ), p_Delay );

		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt != NULL ) && ( WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			WebRtcAecmSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
			WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt != NULL ) && ( WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_IsUseDelayAgstcMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
			{
				WebRtcAecSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, p_Delay );
				WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, &p_TmpInt32 );
			}
			else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
			{
				WebRtcAecSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, 20 );
				WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, &p_TmpInt32 );
			}
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
		{
			SpeexWebRtcAecSetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
			SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
			{
				SpeexWebRtcAecSetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, p_Delay );
				SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 );
			}
			else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
			{
				SpeexWebRtcAecSetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, 20 );
				SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 );
			}
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
	} //自适应设置声学回音的延迟完毕。
	{
		if( p_SystemAecNsAgcMediaObjPt != NULL ) p_SystemAecNsAgcMediaObjPt->AllocateStreamingResources(); //让音频输入设备开始录音。
		else AdoInptPt->m_Dvc.m_ClntPt->Start(); //让音频输入设备开始录音。
		AdoInptPt->m_Thrd.m_IsStartAdoOtptThrd = 1; //设置已开始音频输出线程。在开始音频输出线程前设置，这样可以保证不会误判断。
		if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) //如果已初始化音频输出。
		{
			AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_ClntPt->Start(); //让音频输出设备开始播放。
			AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Thrd.m_ThrdIsStart = 1; //设置音频输出线程已开始。
		}
	}

	//音频输入循环开始。
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) AdoInptPt->m_Thrd.m_LastTickMsec = FuncGetTickAsMsec();
	while( 1 )
	{
		{
			if( p_SystemAecNsAgcMediaObjPt != NULL ) //如果要使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			{
				p_HRslt = p_SystemAecNsAgcMediaObjPt->ProcessOutput( DMO_PROCESS_OUTPUT_DISCARD_WHEN_NO_BUFFER, 1, AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt, &p_DvcBufFlag ); //从系统自带的声学回音消除器、噪音抑制器和自动增益控制器获取Pcm格式缓冲区帧。
				if( p_HRslt == S_OK ) //如果获取成功。
				{
					AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer->GetBufferAndLength( ( BYTE * * )&p_PcmBufFrmPt, ( DWORD * )&p_DvcBufLenByt );
					if( p_DvcBufLenByt > 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：PcmBufFrmPt：%P，p_DvcBufLenByt：%uz32d，DvcBufFlag：%uz32d。" ), p_PcmBufFrmPt, p_DvcBufLenByt, p_DvcBufFlag );
						//fwrite( p_PcmBufFrmPt, p_DvcBufLenByt, 1, AdoInptFile1Pt );
						AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.PutTail( p_PcmBufFrmPt, p_DvcBufLenByt, 0, NULL ); //将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。
						AdoInptPt->m_Dvc.m_SystemAecNsAgcOtptDataBufPt->pBuffer->SetLength( 0 ); //清空系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区。
					}

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.GetLen( &p_TmpSz, 0, NULL ); //获取Pcm格式缓冲区帧缓冲区队列的长度。
					if( p_TmpSz < AdoInptPt->m_Dvc.m_PcmBufFrmLenByt ) goto OutPocs; //如果Pcm格式缓冲区帧缓冲区队列的长度不够一个Pcm格式缓冲区帧的长度，就跳出处理。
				}
				else //如果获取失败。
				{
					if( p_HRslt != S_FALSE ) //如果是其他错误。
					{
						if( AdoInptPt->m_Dvc.m_IsClos == 0 )
						{
							AdoInptPt->m_Dvc.m_IsClos = 1; //设置设备已经关闭。
							if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到设备关闭消息。" ) );
						}
					}
					goto OutPocs;
				}
			}
			else //如果不使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			{
				//获取设备的缓冲区，并将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。
				p_HRslt = p_DvcCptrClntPt->GetBuffer( ( BYTE ** )&p_PcmBufFrmPt, &p_DvcBufLenUnit, &p_DvcBufFlag, NULL, NULL ); //获取设备的Pcm格式缓冲区。
				if( p_HRslt == S_OK ) //如果获取设备的Pcm格式缓冲区成功。
				{
					p_DvcBufLenByt = p_DvcBufLenUnit * sizeof( int16_t );
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：PcmBufFrmPt：%P，DvcBufLenUnit：%uz32d，p_DvcBufLenByt：%uz32d，DvcBufFlag：%uz32d。" ), p_PcmBufFrmPt, p_DvcBufLenUnit, p_DvcBufLenByt, p_DvcBufFlag );

					if( p_DvcBufFlag & AUDCLNT_BUFFERFLAGS_SILENT ) //如果本次Pcm格式缓冲区帧为静音。
					{
						//if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：AUDCLNT_BUFFERFLAGS_SILENT。" ) );
						memset( p_PcmBufFrmPt, 0, p_DvcBufLenByt );
					}

					if( AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels == 2 ) //如果Pcm格式缓冲区帧为双声道。
					{
						for( UINT i = 0; i < p_DvcBufLenByt; i += sizeof( int16_t ) ) //双声道合并为单声道。
							p_PcmBufFrmPt[ i / sizeof( int16_t ) ] = ( p_PcmBufFrmPt[ i ] + p_PcmBufFrmPt[ i + 1 ] ) / 2;
					}
					//fwrite( p_PcmBufFrmPt, p_DvcBufLenByt, 1, AdoInptFile1Pt );

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.PutTail( p_PcmBufFrmPt, p_DvcBufLenByt, 0, NULL ); //将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。

					p_DvcCptrClntPt->ReleaseBuffer( p_DvcBufLenUnit ); //释放设备的Pcm格式缓冲区。

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.GetLen( &p_TmpSz, 0, NULL ); //获取Pcm格式缓冲区帧缓冲区队列的长度。
					if( p_TmpSz < AdoInptPt->m_Dvc.m_PcmBufFrmLenByt ) goto OutPocs; //如果Pcm格式缓冲区帧缓冲区队列的长度不够一个Pcm格式缓冲区帧的长度，就跳出处理。
				}
				else //如果获取设备的Pcm格式缓冲区失败。
				{
					if( p_HRslt == AUDCLNT_E_DEVICE_INVALIDATED ) //如果设备已经关闭。
					{
						if( AdoInptPt->m_Dvc.m_IsClos == 0 )
						{
							AdoInptPt->m_Dvc.m_IsClos = 1; //设置设备已经关闭。
							if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到设备关闭消息。" ) );
						}
					}
					else if( p_HRslt == AUDCLNT_S_BUFFER_EMPTY ) //如果设备的Pcm格式缓冲区当前为空，表示还需要等待。
					{
						//if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_S_BUFFER_EMPTY。" ) );
						Sleep( 1 ); //暂停一下，避免CPU使用率过高。
					}
					else if( p_HRslt == AUDCLNT_E_BUFFER_ERROR )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_E_BUFFER_ERROR。" ) );
					}
					else if( p_HRslt == AUDCLNT_E_OUT_OF_ORDER )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_E_OUT_OF_ORDER。" ) );
					}
					else if( p_HRslt == AUDCLNT_E_BUFFER_OPERATION_PENDING )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_E_BUFFER_OPERATION_PENDING。" ) );
					}
					else if( p_HRslt == AUDCLNT_E_SERVICE_NOT_RUNNING )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_E_SERVICE_NOT_RUNNING。" ) );
					}
					else if( p_HRslt == E_POINTER )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：E_POINTER。" ) );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：p_HRslt:%ld。" ), p_HRslt );
					}
					goto OutPocs;
				}
			}

			//获取一个Pcm格式空闲帧。
			AdoInptPt->m_PcmIdleFrmCntnr.GetTotal( &AdoInptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取Pcm格式空闲帧容器的元素总数。
			if( AdoInptPt->m_Thrd.m_ElmTotal > 0 ) //如果Pcm格式空闲帧容器中有帧。
			{
				AdoInptPt->m_PcmIdleFrmCntnr.GetHead( &AdoInptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 1, NULL ); //从Pcm格式空闲帧容器中取出并删除第一个帧。
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：从Pcm格式空闲帧容器中取出并删除第一个帧，Pcm格式空闲帧容器元素总数：%uzd。" ), AdoInptPt->m_Thrd.m_ElmTotal );
			}
			else //如果Pcm格式空闲帧容器中没有帧。
			{
				AdoInptPt->m_PcmSrcFrmCntnr.GetTotal( &AdoInptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取Pcm格式原始帧容器的元素总数。
				if( AdoInptPt->m_Thrd.m_ElmTotal <= 50 )
				{
					AdoInptPt->m_Thrd.m_PcmSrcFrmPt = ( int16_t * )calloc( AdoInptPt->m_FrmLenByt, 1 ); //创建一个Pcm格式空闲帧。
					if( AdoInptPt->m_Thrd.m_PcmSrcFrmPt != NULL )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：Pcm格式空闲帧容器中没有帧，创建一个Pcm格式空闲帧成功。" ) );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：Pcm格式空闲帧容器中没有帧，创建一个Pcm格式空闲帧失败。原因：内存不足。" ) );
						Sleep( 1 ); //暂停一下，避免CPU使用率过高。
						goto OutPocs;
					}
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：Pcm格式原始帧容器中帧总数为%uzd已经超过上限50，不再创建Pcm格式空闲帧。" ), AdoInptPt->m_Thrd.m_ElmTotal );
					Sleep( 1 ); //暂停一下，避免CPU使用率过高。
					goto OutPocs;
				}
			}

			//将Pcm格式缓冲区帧转换为Pcm格式原始帧。
			AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.GetHead( AdoInptPt->m_Dvc.m_PcmBufFrmPt, AdoInptPt->m_Dvc.m_PcmBufFrmLenByt, NULL, 1, 0, NULL );
			//fwrite( AdoInptPt->m_Dvc.m_PcmBufFrmPt, AdoInptPt->m_Dvc.m_PcmBufFrmLenByt, 1, AdoInptFile2Pt );
			if( AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt != NULL )
			{
				SpeexResamplerPocs( AdoInptPt->m_Dvc.m_PcmSrcFrmSpeexResamplerPt, AdoInptPt->m_Dvc.m_PcmBufFrmPt, AdoInptPt->m_Dvc.m_PcmBufFrmLenUnit, AdoInptPt->m_Thrd.m_PcmSrcFrmPt, AdoInptPt->m_FrmLenUnit, NULL );
				//fwrite( AdoInptPt->m_Thrd.m_PcmSrcFrmPt, AdoInptPt->m_FrmLenByt, 1, AdoInptFile3Pt );
			}
			else
			{
				memcpy( AdoInptPt->m_Thrd.m_PcmSrcFrmPt, AdoInptPt->m_Dvc.m_PcmBufFrmPt, AdoInptPt->m_FrmLenByt );
				//fwrite( AdoInptPt->m_Thrd.m_PcmSrcFrmPt, AdoInptPt->m_FrmLenByt, 1, AdoInptFile3Pt );
			}

			//追加本次Pcm格式原始帧到Pcm格式原始帧容器。注意：从取出到追加过程中不能跳出，否则会内存泄露。
			{
				AdoInptPt->m_PcmSrcFrmCntnr.PutTail( &AdoInptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, NULL );
				AdoInptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
			}

			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				AdoInptPt->m_Thrd.m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "音频输入线程：本次帧处理完毕，耗时 %uz64d 毫秒。" ), AdoInptPt->m_Thrd.m_NowTickMsec - AdoInptPt->m_Thrd.m_LastTickMsec );
				AdoInptPt->m_Thrd.m_LastTickMsec = AdoInptPt->m_Thrd.m_NowTickMsec;
			}
		}
		OutPocs:;

		if( AdoInptPt->m_Thrd.m_ExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到退出请求，开始准备退出。" ) );
            break;
        }
	} //音频输入循环结束。

	if( p_SystemAecNsAgcMediaObjPt != NULL ) p_SystemAecNsAgcMediaObjPt->Release();
	CoUninitialize(); //销毁COM库。
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：本线程已退出。" ) );
	return 0;
}
