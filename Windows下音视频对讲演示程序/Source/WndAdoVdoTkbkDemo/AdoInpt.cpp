#include "AdoInpt.h"
#include "MediaPocsThrd.h"

DWORD WINAPI AdoInptThrdRun( AdoInpt * AdoInptPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptAecInit
 * 功能说明：初始化音频输入的声学回音消除器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
			if( SpeexAecInit( AdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &AdoInptPt->m_SpeexAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexAec.m_FilterLenMsec, AdoInptPt->m_SpeexAec.m_IsUseRec, AdoInptPt->m_SpeexAec.m_EchoMutp, AdoInptPt->m_SpeexAec.m_EchoCntu, AdoInptPt->m_SpeexAec.m_EchoSupes, AdoInptPt->m_SpeexAec.m_EchoSupesAct, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Speex声学回音消除器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化Speex声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 2: //如果要使用WebRtc定点版声学回音消除器。
		{
			#if IsIcludWebRtc
			if( WebRtcAecmInit( &AdoInptPt->m_WebRtcAecm.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAecm.m_IsUseCNGMode, AdoInptPt->m_WebRtcAecm.m_EchoMode, AdoInptPt->m_WebRtcAecm.m_Delay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版声学回音消除器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含WebRtc。" ), , );
			#endif
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版声学回音消除器。
		{
			#if IsIcludWebRtc
			if( WebRtcAecInit( AdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &AdoInptPt->m_WebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAec.m_EchoMode, AdoInptPt->m_WebRtcAec.m_Delay, AdoInptPt->m_WebRtcAec.m_IsUseDelayAgstcMode, AdoInptPt->m_WebRtcAec.m_IsUseExtdFilterMode, AdoInptPt->m_WebRtcAec.m_IsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAec.m_IsUseAdaptAdjDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版声学回音消除器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含WebRtc。" ), , );
			#endif
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 4: //如果要使用WebRtc第三版声学回音消除器。
		{
			#if IsIcludWebRtc3
			if( WebRtcAec3Init( AdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &AdoInptPt->m_WebRtcAec3.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcAec3.m_Delay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc第三版声学回音消除器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含WebRtc3。" ), , );
			#endif
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc第三版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 5: //如果要使用SpeexWebRtc三重声学回音消除器。
		{
			#if IsIcludSpeexWebRtcAec
			if( SpeexWebRtcAecInit( AdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &AdoInptPt->m_SpeexWebRtcAec.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_SpeexWebRtcAec.m_WorkMode, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecFilterLenMsec, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecIsUseRec, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoMutp, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoCntu, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoSupes, AdoInptPt->m_SpeexWebRtcAec.m_SpeexAecEchoSupesAct, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmIsUseCNGMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmEchoMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecmDelay, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecEchoMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecDelay, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseExtdFilterMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_SpeexWebRtcAec.m_WebRtcAec3Delay, AdoInptPt->m_SpeexWebRtcAec.m_IsUseSameRoomAec, AdoInptPt->m_SpeexWebRtcAec.m_SameRoomEchoMinDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化SpeexWebRtc三重声学回音消除器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含SpeexWebRtcAec。" ), , );
			#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
				#if IsIcludWebRtc
				if( WebRtcAecmDstoy( AdoInptPt->m_WebRtcAecm.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版声学回音消除器成功。" ) );
				}
				else
				#endif
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
				#if IsIcludWebRtc
				if( WebRtcAecDstoy( AdoInptPt->m_WebRtcAec.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版声学回音消除器成功。" ) );
				}
				else
				#endif
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAec.m_Pt = NULL;
			}
			break;
		}
		case 4: //如果要使用WebRtc第三版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAec3.m_Pt != NULL )
			{
				#if IsIcludWebRtc3
				if( WebRtcAec3Dstoy( AdoInptPt->m_WebRtcAec3.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc第三版声学回音消除器成功。" ) );
				}
				else
				#endif
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc第三版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAec3.m_Pt = NULL;
			}
			break;
		}
		case 5: //如果要使用SpeexWebRtc三重声学回音消除器。
		{
			if( AdoInptPt->m_SpeexWebRtcAec.m_Pt != NULL )
			{
				#if IsIcludSpeexWebRtcAec
				if( SpeexWebRtcAecDstoy( AdoInptPt->m_SpeexWebRtcAec.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁SpeexWebRtc三重声学回音消除器成功。" ) );
				}
				else
				#else
				VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含SpeexWebRtcAec。" ), , );
				#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
			#if IsIcludWebRtc
			if( WebRtcNsxInit( &AdoInptPt->m_WebRtcNsx.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcNsx.m_PolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版噪音抑制器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含WebRtc。" ), , );
			#endif
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc定点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果要使用WebRtc浮点版噪音抑制器。
		{
			#if IsIcludWebRtc
			if( WebRtcNsInit( &AdoInptPt->m_WebRtcNs.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_WebRtcNs.m_PolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版噪音抑制器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含WebRtc。" ), , );
			#endif
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化WebRtc浮点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 4: //如果要使用RNNoise噪音抑制器。
		{
			#if IsIcludRNNoise
			if( RNNoiseInit( AdoInptPt->m_MediaPocsThrdPt->m_LicnCodePt, &AdoInptPt->m_RNNoise.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLenUnit, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化RNNoise噪音抑制器成功。" ) );
			}
			else
			#else
			VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含RNNoise。" ), , );
			#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
				#if IsIcludWebRtc
				if( WebRtcNsxDstoy( AdoInptPt->m_WebRtcNsx.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc定点版噪音抑制器成功。" ) );
				}
				else
				#endif
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
				#if IsIcludWebRtc
				if( WebRtcNsDstoy( AdoInptPt->m_WebRtcNs.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁WebRtc浮点版噪音抑制器成功。" ) );
				}
				else
				#endif
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
				#if IsIcludRNNoise
				if( RNNoiseDstoy( AdoInptPt->m_RNNoise.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁RNNoise噪音抑制器成功。" ) );
				}
				else
				#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
		case 0: //如果要使用Pcm原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用Speex编码器。
		{
			if( AdoInptPt->m_FrmLenMsec != 20 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：帧的长度不为20毫秒不能使用Speex编码器。" ) );
				goto Out;
			}
			#if IsIcludSpeex
			if( SpeexEncdInit( &AdoInptPt->m_SpeexEncd.m_Pt, AdoInptPt->m_SmplRate, AdoInptPt->m_SpeexEncd.m_UseCbrOrVbr, AdoInptPt->m_SpeexEncd.m_Qualt, AdoInptPt->m_SpeexEncd.m_Cmplxt, AdoInptPt->m_SpeexEncd.m_PlcExptLossRate ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化Speex编码器成功。" ) );
			}
			else
			#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
		case 0: //如果要使用Pcm原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Pcm原始数据成功。" ) );
			break;
		}
		case 1: //如果要使用Speex编码器。
		{
			if( AdoInptPt->m_SpeexEncd.m_Pt != NULL )
			{
				#if IsIcludSpeex
				if( SpeexEncdDstoy( AdoInptPt->m_SpeexEncd.m_Pt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁Speex编码器成功。" ) );
				}
				else
				#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AdoInptWavfmInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_Wavfm.m_IsDraw != 0 )
	{
		#if IsIcludAdoWavfm
		if( AdoWavfmInit( &AdoInptPt->m_Wavfm.m_SrcPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化原始波形器成功。" ) );
		}
		else
		#else
		VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含AdoWavfm。" ), , );
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化原始波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		#if IsIcludAdoWavfm
		if( AdoWavfmInit( &AdoInptPt->m_Wavfm.m_RsltPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化结果波形器成功。" ) );
		}
		else
		#else
		VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含AdoWavfm。" ), , );
		#endif
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoInptWavfmDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_Wavfm.m_SrcPt != NULL )
	{
		#if IsIcludAdoWavfm
		if( AdoWavfmDstoy( AdoInptPt->m_Wavfm.m_SrcPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁原始波形器成功。" ) );
		}
		else
		#else
		VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含AdoWavfm。" ), , );
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁原始波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_Wavfm.m_SrcPt = NULL;
	}
	if( AdoInptPt->m_Wavfm.m_RsltPt != NULL )
	{
		#if IsIcludAdoWavfm
		if( AdoWavfmDstoy( AdoInptPt->m_Wavfm.m_RsltPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁结果波形器成功。" ) );
		}
		else
		#else
		VstrCpy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含AdoWavfm。" ), , );
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁结果波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_Wavfm.m_RsltPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptWaveFileWriterInit
 * 功能说明：初始化音频输入的Wave文件写入器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AdoInptWaveFileWriterInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_WaveFileWriter.m_IsSave != 0 )
	{
		#if IsIcludMediaFile
		if( WaveFileWriterInit( &AdoInptPt->m_WaveFileWriter.m_SrcWriterPt, AdoInptPt->m_WaveFileWriter.m_SrcFullPathVstrPt, AdoInptPt->m_WaveFileWriter.m_WrBufSzByt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化原始Wave文件 %vs 写入器成功。" ), AdoInptPt->m_WaveFileWriter.m_SrcFullPathVstrPt );
		}
		else
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化原始Wave文件 %vs 写入器失败。" ), AdoInptPt->m_WaveFileWriter.m_SrcFullPathVstrPt );
			goto Out;
		}
		#if IsIcludMediaFile
		if( WaveFileWriterInit( &AdoInptPt->m_WaveFileWriter.m_RsltWriterPt, AdoInptPt->m_WaveFileWriter.m_RsltFullPathVstrPt, AdoInptPt->m_WaveFileWriter.m_WrBufSzByt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化结果Wave文件 %vs 写入器成功。" ), AdoInptPt->m_WaveFileWriter.m_RsltFullPathVstrPt );
		}
		else
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化结果Wave文件 %vs 写入器失败。" ), AdoInptPt->m_WaveFileWriter.m_RsltFullPathVstrPt );
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
 * 函数名称：AdoInptWaveFileDstoy
 * 功能说明：销毁音频输入的Wave文件写入器。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoInptWaveFileWriterDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_WaveFileWriter.m_SrcWriterPt != NULL )
	{
		#if IsIcludMediaFile
		if( WaveFileWriterDstoy( AdoInptPt->m_WaveFileWriter.m_SrcWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁原始Wave文件写入器成功。" ) );
		}
		else
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁原始Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_WaveFileWriter.m_SrcWriterPt = NULL;
	}
	if( AdoInptPt->m_WaveFileWriter.m_RsltWriterPt != NULL )
	{
		#if IsIcludMediaFile
		if( WaveFileWriterDstoy( AdoInptPt->m_WaveFileWriter.m_RsltWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁结果Wave文件写入器成功。" ) );
		}
		else
		#endif
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：销毁结果Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_WaveFileWriter.m_RsltWriterPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptDvcAndThrdInit
 * 功能说明：初始化音频输入的设备和线程。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AdoInptDvcAndThrdInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	HRESULT p_HRslt;
	int p_CmpRslt;
	UINT p_AdoInptDvcId; //存放音频输入设备的标识符，取值范围为从1到音频输入设备的总数，为0表示使用默认设备。

	//初始化设备。
	{
		//初始化设备枚举器。要在初始化默认设备改变消息客户端和初始化音频输入设备前执行。
		{
			p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&AdoInptPt->m_Dvc.m_EnumPt );
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备枚举器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		//初始化默认设备改变消息客户端。
		if( VstrCmp( AdoInptPt->m_Dvc.m_NameVstrPt, 0, Cu8vstr( "默认：" )->m_LenChr, Cu8vstr( "默认：" ), 0, &p_CmpRslt ), p_CmpRslt == 0 ) //如果当前使用默认的音频输入设备。
		{
			class MyMMNotificationClient : public IMMNotificationClient
			{
			public:
				ULONG m_RefCnt;
				AdoInpt * m_AdoInptPt;

				MyMMNotificationClient( AdoInpt * AdoInptPt )
				{
					m_AdoInptPt = AdoInptPt;
					m_RefCnt = 0;
					AddRef();
				}

				~MyMMNotificationClient()
				{
					
				}
				
				HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR * ppvObject )
				{
					if( riid == __uuidof( IUnknown ) || riid == __uuidof( IMMNotificationClient ) )
					{
						*ppvObject = ( IMMNotificationClient * )this;
						AddRef();
						return S_OK;
					}
					else
					{
						*ppvObject = NULL;
						return E_NOINTERFACE;
					}
				}

				ULONG STDMETHODCALLTYPE AddRef()
				{
					return InterlockedIncrement( &m_RefCnt );
				}

				ULONG STDMETHODCALLTYPE Release()
				{
					ULONG p_RefCnt = InterlockedDecrement( &m_RefCnt );
					if( p_RefCnt == 0 )
					{
						delete this;
					}
					return p_RefCnt;
				}

				HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId ) //默认的音频输入输出设备改变回调。
				{
					if( ( flow == eCapture ) && ( role == eConsole ) ) //如果默认音频输入设备改变，且当前使用默认的音频输入设备。
					{
						if( m_AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：接收到默认音频输入设备改变消息，且当前使用默认的音频输入设备，准备切换到新的设备，发送音频输入设备关闭线程消息。" ) );
						MediaPocsThrd::ThrdMsgAdoInptDvcClos p_ThrdMsgAdoInptDvcClos;
						if( MsgQueueSendMsg( m_AdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypAdoInptDvcClos, &p_ThrdMsgAdoInptDvcClos, sizeof( p_ThrdMsgAdoInptDvcClos ), m_AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( m_AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：发送音频输入设备关闭线程消息失败。原因：%vs" ), m_AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
					}
					return S_OK;
				}

				HRESULT STDMETHODCALLTYPE OnDeviceAdded( LPCWSTR pwstrDeviceId ) //音频输入输出设备增加回调。
				{
					return S_OK;
				}

				HRESULT STDMETHODCALLTYPE OnDeviceRemoved( LPCWSTR pwstrDeviceId ) //音频输入输出设备删除回调。
				{
					return S_OK;
				}

				HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( LPCWSTR pwstrDeviceId, DWORD dwNewState ) //音频输入输出设备状态改变回调。
				{
					return S_OK;
				}

				HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( LPCWSTR pwstrDeviceId, const PROPERTYKEY key ) //音频输入输出设备属性改变回调。
				{
					return S_OK;
				}
			};
			AdoInptPt->m_Dvc.m_NotificationClientPt = new MyMMNotificationClient( AdoInptPt );
			AdoInptPt->m_Dvc.m_EnumPt->RegisterEndpointNotificationCallback( AdoInptPt->m_Dvc.m_NotificationClientPt );
		}

		//设置音频输入设备标识符。
		if( MediaPocsThrdGetAdoInptDvcId( AdoInptPt->m_Dvc.m_NameVstrPt, &p_AdoInptDvcId, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：获取音频输入设备名称[%vs]的标识符失败，发送音频输入设备关闭线程消息。原因：%vs" ), AdoInptPt->m_Dvc.m_NameVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{
				MediaPocsThrd::ThrdMsgAdoInptDvcClos p_ThrdMsgAdoInptDvcClos;
				if( MsgQueueSendMsg( AdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypAdoInptDvcClos, &p_ThrdMsgAdoInptDvcClos, sizeof( p_ThrdMsgAdoInptDvcClos ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：发送音频输入设备关闭线程消息失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
			p_Rslt = 0; //这里返回成功是为了防止媒体处理线程报错退出。
			goto Out;
		}

		//初始化设备。
		if( p_AdoInptDvcId == 0 ) //如果要使用默认的设备。
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

			p_HRslt = AdoInptPt->m_Dvc.m_ClctPt->Item( p_AdoInptDvcId - 1, &AdoInptPt->m_Dvc.m_Pt ); //初始化设备。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：音频输入：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		if( AdoInptPt->m_IsUseSystemAecNsAgc != 0 ) //如果要使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
		{
			//初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			{
				int p_OneInitSystemAecNsAgcRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。
				LPWSTR p_AdoOtptDvcIdStrPt = NULL;

				p_HRslt = AdoInptPt->m_Dvc.m_Pt->Activate( IID_IAudioClient2, CLSCTX_INPROC_SERVER, NULL, ( void * * )&AdoInptPt->m_Dvc.m_ClntPt ); //激活设备的客户端接口。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：激活设备的客户端接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
				}

				//设置音频类别。
				{
					AudioClientProperties p_ClntPrpt = {};
					p_ClntPrpt.cbSize = sizeof( AudioClientProperties );
					//p_ClntPrpt.eCategory = AudioCategory_FarFieldSpeech; //这个类别会增大回音。
					p_ClntPrpt.eCategory = AudioCategory_Communications;
					p_HRslt = ( ( IAudioClient2 * )AdoInptPt->m_Dvc.m_ClntPt )->SetClientProperties( &p_ClntPrpt );
					if( p_HRslt != S_OK )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：设置音频类别失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutOneInitSystemAecNsAgc;
					}
				}

				p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->GetMixFormat( &AdoInptPt->m_Dvc.m_WaveFmtExPt ); //获取设备的格式。
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：获取设备的格式失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
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
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化设备的客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
				}

				p_HRslt = AdoInptPt->m_Dvc.m_ClntPt->GetService( IID_IAcousticEchoCancellationControl, ( void * * )&AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：获取声学回音消除控制器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
				}

				p_HRslt = AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_Pt->GetId( &p_AdoOtptDvcIdStrPt );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：获取音频输出设备的Id字符串失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
				}

				p_HRslt = AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt->SetEchoCancellationRenderEndpoint( p_AdoOtptDvcIdStrPt );
				if( p_HRslt != S_OK )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：设置声学回音消除参考的音频输出设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
					goto OutOneInitSystemAecNsAgc;
				}

				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器成功。" ) );
				
				p_OneInitSystemAecNsAgcRslt = 0; //设置本处理段执行成功。

				OutOneInitSystemAecNsAgc:
				if( p_OneInitSystemAecNsAgcRslt != 0 ) //如果本处理段执行失败。
				{
					if( p_AdoOtptDvcIdStrPt != NULL )
					{
						CoTaskMemFree( p_AdoOtptDvcIdStrPt );
						p_AdoOtptDvcIdStrPt = NULL;
					}
					if( AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt != NULL )
					{
						AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt->Release();
						AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt = NULL;
					}
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
				}
			}

			//初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			if( AdoInptPt->m_Dvc.m_ClntPt == NULL ) //如果初始化第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。
			{
				int p_TwoInitSystemAecNsAgcRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。

				if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsInit == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：没有初始化音频输出，会导致第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器不会输出音频数据。" ) );
					goto OutTwoInitSystemAecNsAgc;
				}

				if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_WaveFmtExPt->nChannels >= 8 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：音频输出大于等于8声道，会导致第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器无法正常工作。" ) );
					goto OutTwoInitSystemAecNsAgc;
				}

				//初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象。
				{
					UINT p_AdoOtptDvcId; //存放音频输出设备的标识符，取值范围为从1到音频输出设备的总数，为0表示使用默认设备。

					//设置音频输出设备标识符。
					if( MediaPocsThrdGetAdoOtptDvcId( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_NameVstrPt, &p_AdoOtptDvcId, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：获取音频输出设备名称[%vs]的标识符失败。原因：%vs" ), AdoInptPt->m_Dvc.m_NameVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto OutTwoInitSystemAecNsAgc;
					}

					p_HRslt = CoCreateInstance( CLSID_CWMAudioAEC, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, ( void * * )&AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt );
					if( p_HRslt != S_OK )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}

					IPropertyStore * p_PrptStorPt;
					PROPVARIANT p_ProVar = { 0 };
					p_HRslt = AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->QueryInterface( IID_IPropertyStore, ( void * * )&p_PrptStorPt );
					if( p_HRslt != S_OK )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：获取第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的属性接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}
					p_ProVar.vt = VT_I4;
					p_ProVar.intVal = ( ( p_AdoInptDvcId - 1 ) & 0xFFFF ) | ( ( ( p_AdoOtptDvcId - 1 ) & 0xFFFF ) << 16 );
					p_PrptStorPt->SetValue( MFPKEY_WMAAECMA_DEVICE_INDEXES, p_ProVar ); //设置音频输入输出设备的索引。
					p_ProVar.vt = VT_BOOL;
					p_ProVar.boolVal = VARIANT_FALSE;
					p_PrptStorPt->SetValue( MFPKEY_WMAAECMA_MIC_GAIN_BOUNDER, p_ProVar ); //关闭修改麦克风增益边界功能。因为打开时系统自带的声学回音消除器会自动调节麦克风的系统音量，不太友好。
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
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化媒体类型失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
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
						MoFreeMediaType( &p_MediaTyp );
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：创建格式的内存块失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}
					memcpy( AdoInptPt->m_Dvc.m_WaveFmtExPt, p_WaveFmtExPt, sizeof( WAVEFORMATEX ) );

					p_HRslt = AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->SetOutputType( 0, &p_MediaTyp, 0 );
					if( p_HRslt == S_FALSE )
					{
						MoFreeMediaType( &p_MediaTyp );
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：设置媒体类型失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}

					MoFreeMediaType( &p_MediaTyp );

					p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoInptPt->m_Dvc.m_GlblIntfcTablePt ); //初始化设备全局接口表。
					if( p_HRslt != S_OK )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化设备的全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}

					p_HRslt = AdoInptPt->m_Dvc.m_GlblIntfcTablePt->RegisterInterfaceInGlobal( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt, IID_IMediaObject, &AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie ); //注册系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象到全局接口表。
					if( p_HRslt != S_OK )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：注册第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
						goto OutTwoInitSystemAecNsAgc;
					}
				}

				//初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象。
				{
					AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt = ( DMO_OUTPUT_DATA_BUFFER * )calloc( 1, sizeof( DMO_OUTPUT_DATA_BUFFER ) );
					if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt == NULL )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化输出数据缓冲区对象失败。原因：内存不足。" ) );
						goto OutTwoInitSystemAecNsAgc;
					}
					class MyMediaBuffer final : public IMediaBuffer
					{
					public:
						BYTE m_BufArr[ 4096 ];
						DWORD m_BufLenByt;
						ULONG m_RefCnt;

						MyMediaBuffer()
						{
							m_BufLenByt = 0;
							m_RefCnt = 0;
							AddRef();
						}

						~MyMediaBuffer()
						{

						}
					
						HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR * ppvObject )
						{
							if( !ppvObject )
							{
								return E_POINTER;
							}
							else if( riid != IID_IMediaBuffer && riid != IID_IUnknown )
							{
								return E_NOINTERFACE;
							}

							*ppvObject = static_cast< IMediaBuffer * >( this );
							AddRef();
							return S_OK;
						}
					
						ULONG STDMETHODCALLTYPE AddRef()
						{
							return InterlockedIncrement( &m_RefCnt );
						}

						ULONG STDMETHODCALLTYPE Release()
						{
							LONG p_RefCnt = InterlockedDecrement( &m_RefCnt );
							if( p_RefCnt == 0 )
							{
								delete this;
							}
							return p_RefCnt;
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
					};
					AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer = new MyMediaBuffer();
					if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer == NULL )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器失败。原因：初始化输出数据缓冲区对象失败。原因：内存不足。" ) );
						goto OutTwoInitSystemAecNsAgc;
					}
				}
				
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器成功。" ) );

				p_TwoInitSystemAecNsAgcRslt = 0; //设置本处理段执行成功。

				OutTwoInitSystemAecNsAgc:
				if( p_TwoInitSystemAecNsAgcRslt != 0 ) //如果本处理段执行失败。
				{
					if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt != NULL )
					{
						if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer != NULL )
						{
							AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer->Release();
							AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer = NULL;
						}
						free( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt );
						AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt = NULL;
					}
					if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt != NULL )
					{
						AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->FreeStreamingResources();
						AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->Release();
						AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt = NULL;
					}
				}
			}
		}

		if( ( AdoInptPt->m_Dvc.m_ClntPt == NULL ) && ( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt == NULL ) )
		{
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
		}

		if( AdoInptPt->m_Dvc.m_ClntPt != NULL )
		{
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
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：音频输入：初始化Pcm格式原始帧Speex重采样器失败。" ) );
			goto Out;
		}

		//发送音频输入设备改变线程消息。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：发送音频输入设备改变线程消息。" ) );
			MediaPocsThrd::ThrdMsgAdoInptDvcChg p_ThrdMsgAdoInptDvcChg;
			if( MsgQueueSendMsg( AdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypAdoInptDvcChg, &p_ThrdMsgAdoInptDvcChg, sizeof( p_ThrdMsgAdoInptDvcChg ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输入：发送音频输入设备改变线程消息失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
		}

		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：初始化设备成功。采样频率：%z32d, 声道数：%z16d，缓冲区大小：%uz32d。" ), AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec, AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels, AdoInptPt->m_Dvc.m_BufSzUnit );
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
	if( AdoInptPt->m_PcmIdleFrmCntnr.m_Pt != NULL )
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
	if( AdoInptPt->m_PcmSrcFrmCntnr.m_Pt != NULL )
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
		if( AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.m_Pt != NULL ) //销毁Pcm格式缓冲区帧的缓冲区队列。
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
		if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt != NULL )
		{
			if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt != NULL ) //销毁第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区对象。
			{
				if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer != NULL )
				{
					AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer->Release();
					AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer = NULL;
				}
				free( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt );
				AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt != NULL ) //销毁第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象。
			{
				AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->FreeStreamingResources();
				AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt->Release();
				AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcMediaObjPt = NULL;
			}
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入：销毁第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器成功。" ) );
		}
		if( AdoInptPt->m_Dvc.m_Pt != NULL )
		{
			if( AdoInptPt->m_Dvc.m_CptrClntPt != NULL ) //销毁设备的捕获客户端。
			{
				AdoInptPt->m_Dvc.m_CptrClntPt->Release();
				AdoInptPt->m_Dvc.m_CptrClntPt = NULL;
			}
			if( AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt != NULL ) //销毁第一种系统自带的声学回音消除器、噪音抑制器和自动增益控制器的声学回音消除控制器。
			{
				AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt->Release();
				AdoInptPt->m_Dvc.m_OneSystemAecNsAgcAecCtrlPt = NULL;
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
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输入：销毁设备成功。" ) );
		}
		if( AdoInptPt->m_Dvc.m_NotificationClientPt != NULL ) //销毁默认设备改变消息客户端。
		{
			AdoInptPt->m_Dvc.m_EnumPt->UnregisterEndpointNotificationCallback( AdoInptPt->m_Dvc.m_NotificationClientPt );
			AdoInptPt->m_Dvc.m_NotificationClientPt->Release();
			AdoInptPt->m_Dvc.m_NotificationClientPt = NULL;
		}
		if( AdoInptPt->m_Dvc.m_EnumPt != NULL ) //销毁设备枚举器。
		{
			AdoInptPt->m_Dvc.m_EnumPt->Release();
			AdoInptPt->m_Dvc.m_EnumPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptInit
 * 功能说明：初始化音频输入。
 * 参数说明：参数1名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             参数2名称：[输入|输出|输入&输出]，存放此参数的使用说明。
             ……
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
             ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
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
	IMediaObject * p_TwoSystemAecNsAgcMediaObjPt = NULL; //存放第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器对象的指针。
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
			p_DvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoInptPt->m_Dvc.m_SystemAecNsAgcMediaObjCookie, IID_IMediaObject, ( void * * )&p_TwoSystemAecNsAgcMediaObjPt );
		}
		else
		{
			p_DvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoInptPt->m_Dvc.m_CptrClntCookie, IID_IAudioCaptureClient, ( void * * )&p_DvcCptrClntPt );
		}
		p_DvcGlblIntfcTablePt->Release();
	}
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：开始准备音频输入。" ) );

	if( AdoInptPt->m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器，就自适应计算回音延迟，并设置到声学回音消除器。放在音频输入线程中计算，可以减少媒体处理线程的初始化时间。
	{
		int32_t p_AdoInptDelay; //存放音频输入延迟。
		int32_t p_AdoOtptDelay; //存放音频输出延迟。
		int32_t p_Delay; //存放回音延迟，单位为毫秒。
		int32_t p_TmpInt32;

		p_AdoInptDelay = /*AdoInptPt->m_Dvc.m_BufSzUnit * 1000 / AdoInptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec*/10; //音频输入延迟固定为10毫秒。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：音频输入延迟 %z32d 毫秒。" ), p_AdoInptDelay );
		p_AdoOtptDelay = AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_BufSzUnit * 1000 / AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：音频输出延迟 %z32d 毫秒。" ), p_AdoOtptDelay );
		p_Delay = p_AdoInptDelay + p_AdoOtptDelay;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：回音延迟 %z32d 毫秒。" ), p_Delay );

		#if IsIcludWebRtc
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt != NULL ) && ( WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音延迟。
		{
			WebRtcAecmSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际回音延迟的二分之一，这样效果最好。
			WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecm.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt != NULL ) && ( WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音延迟。
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
		#endif
		#if IsIcludWebRtc3
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt != NULL ) && ( WebRtcAec3GetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc第三版声学回音消除器，且需要自适应设置回音延迟。
		{
			WebRtcAec3SetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt, p_Delay );
			WebRtcAec3GetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAec3.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置WebRtc第三版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		#endif
		#if IsIcludSpeexWebRtcAec
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音延迟。
		{
			SpeexWebRtcAecSetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, p_Delay / 2 ); //设置WebRtc定点版声学回音消除器的回音延迟为实际回音延迟的二分之一，这样效果最好。
			SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音延迟。
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
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt != NULL ) && ( SpeexWebRtcAecGetWebRtcAec3Delay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc第三版声学回音消除器需要自适应设置回音延迟。
		{
			SpeexWebRtcAecSetWebRtcAec3Delay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, p_Delay );
			SpeexWebRtcAecGetWebRtcAec3Delay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAec.m_Pt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc第三版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		#endif
	} //自适应设置回音延迟完毕。
	{
		if( p_TwoSystemAecNsAgcMediaObjPt != NULL ) p_TwoSystemAecNsAgcMediaObjPt->AllocateStreamingResources(); //让音频输入设备开始录音。
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
			if( p_TwoSystemAecNsAgcMediaObjPt != NULL ) //如果要使用第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			{
				p_HRslt = p_TwoSystemAecNsAgcMediaObjPt->ProcessOutput( DMO_PROCESS_OUTPUT_DISCARD_WHEN_NO_BUFFER, 1, AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt, &p_DvcBufFlag ); //从第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器获取Pcm格式缓冲区帧。
				if( p_HRslt == S_OK ) //如果获取成功。
				{
					AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer->GetBufferAndLength( ( BYTE * * )&p_PcmBufFrmPt, ( DWORD * )&p_DvcBufLenByt );
					if( p_DvcBufLenByt > 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：PcmBufFrmPt：%P，p_DvcBufLenByt：%uz32d，DvcBufFlag：%uz32d。" ), p_PcmBufFrmPt, p_DvcBufLenByt, p_DvcBufFlag );
						//fwrite( p_PcmBufFrmPt, p_DvcBufLenByt, 1, AdoInptFile1Pt );
						AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.PutTail( p_PcmBufFrmPt, p_DvcBufLenByt, 0, NULL ); //将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。
						AdoInptPt->m_Dvc.m_TwoSystemAecNsAgcOtptDataBufPt->pBuffer->SetLength( 0 ); //清空第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器输出数据缓冲区。
					}

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.GetLen( &p_TmpSz, 0, NULL ); //获取Pcm格式缓冲区帧缓冲区队列的长度。
					if( p_TmpSz < AdoInptPt->m_Dvc.m_PcmBufFrmLenByt ) goto OutPocs; //如果Pcm格式缓冲区帧缓冲区队列的长度不够一个Pcm格式缓冲区帧的长度，就跳出处理。
				}
				else //如果获取失败。
				{
					if( p_HRslt != S_FALSE ) //如果是其他错误。
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：接收设备关闭消息，或第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器不支持该设备，发送音频输入设备关闭线程消息。" ) );
						{
							MediaPocsThrd::ThrdMsgAdoInptDvcClos p_ThrdMsgAdoInptDvcClos;
							if( MsgQueueSendMsg( AdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypAdoInptDvcClos, &p_ThrdMsgAdoInptDvcClos, sizeof( p_ThrdMsgAdoInptDvcClos ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：发送音频输入设备关闭线程消息失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						break; //这里要退出线程，防止多次发送线程消息。
					}
					goto OutPocs;
				}
			}
			else //如果不使用第二种系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
			{
				//获取设备的缓冲区，并将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。
				p_HRslt = p_DvcCptrClntPt->GetBuffer( ( BYTE * * )&p_PcmBufFrmPt, &p_DvcBufLenUnit, &p_DvcBufFlag, NULL, NULL ); //获取设备的Pcm格式缓冲区。
				if( p_HRslt == S_OK ) //如果获取设备的Pcm格式缓冲区成功。
				{
					p_DvcBufLenByt = p_DvcBufLenUnit * sizeof( int16_t );
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：PcmBufFrmPt：%P，DvcBufLenUnit：%uz32d，p_DvcBufLenByt：%uz32d，DvcBufFlag：%uz32d。" ), p_PcmBufFrmPt, p_DvcBufLenUnit, p_DvcBufLenByt, p_DvcBufFlag );

					if( p_DvcBufFlag & AUDCLNT_BUFFERFLAGS_SILENT ) //如果本次Pcm格式缓冲区帧为静音。
					{
						//if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：AUDCLNT_BUFFERFLAGS_SILENT。" ) );
						memset( p_PcmBufFrmPt, 0, p_DvcBufLenByt );
					}

					if( AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels > 1 ) //如果Pcm格式缓冲区帧为多声道。
					{
						int32_t p_PcmMix;
						for( int i = 0; i < p_DvcBufLenUnit; i++ ) //多声道合并为单声道。
						{
							p_PcmMix = 0;
							for( int j = AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels - 1; j >= 0; j-- )
							{
								p_PcmMix = p_PcmBufFrmPt[ i * AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels + j ] + p_PcmMix - ( ( p_PcmBufFrmPt[ i * AdoInptPt->m_Dvc.m_WaveFmtExPt->nChannels + j ] * p_PcmMix ) >> 0x10 );
							}
							p_PcmBufFrmPt[ i ] = p_PcmMix;
						}
					}
					//fwrite( p_PcmBufFrmPt, p_DvcBufLenByt, 1, AdoInptFile1Pt );

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.PutTail( p_PcmBufFrmPt, p_DvcBufLenByt, 0, NULL ); //将Pcm格式缓冲区帧放入Pcm格式缓冲区帧的缓冲区队列。

					p_DvcCptrClntPt->ReleaseBuffer( p_DvcBufLenUnit ); //释放设备的Pcm格式缓冲区。

					AdoInptPt->m_Dvc.m_PcmBufFrmBufQueue.GetLen( &p_TmpSz, 0, NULL ); //获取Pcm格式缓冲区帧的缓冲区队列的长度。
					if( p_TmpSz < AdoInptPt->m_Dvc.m_PcmBufFrmLenByt ) goto OutPocs; //如果Pcm格式缓冲区帧的缓冲区队列的长度不够一个Pcm格式缓冲区帧的长度，就跳出处理。
				}
				else //如果获取设备的Pcm格式缓冲区失败。
				{
					if( p_HRslt == AUDCLNT_E_DEVICE_INVALIDATED ) //如果设备已经关闭。
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：接收设备关闭消息，发送音频输入设备关闭线程消息。" ) );
						{
							MediaPocsThrd::ThrdMsgAdoInptDvcClos p_ThrdMsgAdoInptDvcClos;
							if( MsgQueueSendMsg( AdoInptPt->m_MediaPocsThrdPt->m_ThrdMsgQueuePt, 0, 0, MediaPocsThrd::ThrdMsgTypAdoInptDvcClos, &p_ThrdMsgAdoInptDvcClos, sizeof( p_ThrdMsgAdoInptDvcClos ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：发送音频输入设备关闭线程消息失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							}
						}
						break; //这里要退出线程，防止多次发送线程消息。
					}
					else if( p_HRslt == AUDCLNT_S_BUFFER_EMPTY ) //如果设备的Pcm格式缓冲区当前为空，表示还需要等待。
					{
						//if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输入线程：AUDCLNT_S_BUFFER_EMPTY。" ) );
						FuncSleep( 1 ); //暂停一下，避免CPU使用率过高。
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
						FuncSleep( 1 ); //暂停一下，避免CPU使用率过高。
						goto OutPocs;
					}
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：Pcm格式原始帧容器中帧总数为%uzd已经超过上限50，不再创建Pcm格式空闲帧。" ), AdoInptPt->m_Thrd.m_ElmTotal );
					FuncSleep( 1 ); //暂停一下，避免CPU使用率过高。
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
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：接收退出请求。" ) );
			break;
		}
	} //音频输入循环结束。

	if( p_TwoSystemAecNsAgcMediaObjPt != NULL ) p_TwoSystemAecNsAgcMediaObjPt->Release();
	if( p_DvcCptrClntPt != NULL ) p_DvcCptrClntPt->Release();

	/*if( AdoInptFile1Pt != NULL ) fclose( AdoInptFile1Pt );
	if( AdoInptFile2Pt != NULL ) fclose( AdoInptFile2Pt );
	if( AdoInptFile3Pt != NULL ) fclose( AdoInptFile3Pt );*/

	CoUninitialize(); //销毁COM库。
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：本线程已退出。" ) );
	return 0;
}
