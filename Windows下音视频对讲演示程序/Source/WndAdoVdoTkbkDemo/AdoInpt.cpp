#include "AdoInpt.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptAecInit
 * 功能说明：初始化音频输入的声学回音消除器。
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

int AdoInptAecInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatAec )
	{
		case 0: //如果不使用声学回音消除器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用声学回音消除器。" ) );
			break;
		}
		case 1: //如果使用Speex声学回音消除器。
		{
			if( AdoInptPt->m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
			{
				if( SpeexAecInitByMemFile( &AdoInptPt->m_SpeexAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_SpeexAecFilterLen, AdoInptPt->m_SpeexAecIsUseRec, AdoInptPt->m_SpeexAecEchoMutp, AdoInptPt->m_SpeexAecEchoCntu, AdoInptPt->m_SpeexAecEchoSupes, AdoInptPt->m_SpeexAecEchoSupesAct, AdoInptPt->m_SpeexAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：根据Speex声学回音消除器内存块文件 %vs 来初始化Speex声学回音消除器成功。" ), AdoInptPt->m_SpeexAecMemFileFullPathVstrPt );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据Speex声学回音消除器内存块文件 %vs 来初始化Speex声学回音消除器失败。原因：%vs" ), AdoInptPt->m_SpeexAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
			}
			if( AdoInptPt->m_SpeexAecPt == NULL )
			{
				if( SpeexAecInit( &AdoInptPt->m_SpeexAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_SpeexAecFilterLen, AdoInptPt->m_SpeexAecIsUseRec, AdoInptPt->m_SpeexAecEchoMutp, AdoInptPt->m_SpeexAecEchoCntu, AdoInptPt->m_SpeexAecEchoSupes, AdoInptPt->m_SpeexAecEchoSupesAct, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化Speex声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化Speex声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
			break;
		}
		case 2: //如果使用WebRtc定点版声学回音消除器。
		{
			if( WebRtcAecmInit( &AdoInptPt->m_WebRtcAecmPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcAecmIsUseCNGMode, AdoInptPt->m_WebRtcAecmEchoMode, AdoInptPt->m_WebRtcAecmDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化WebRtc定点版声学回音消除器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化WebRtc定点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
			{
				if( WebRtcAecInitByMemFile( &AdoInptPt->m_WebRtcAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcAecEchoMode, AdoInptPt->m_WebRtcAecDelay, AdoInptPt->m_WebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_WebRtcAecIsUseExtdFilterMode, AdoInptPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来初始化WebRtc浮点版声学回音消除器成功。" ), AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：根据WebRtc浮点版声学回音消除器内存块文件 %vs 来初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}
			}
			if( AdoInptPt->m_WebRtcAecPt == NULL )
			{
				if( WebRtcAecInit( &AdoInptPt->m_WebRtcAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcAecEchoMode, AdoInptPt->m_WebRtcAecDelay, AdoInptPt->m_WebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_WebRtcAecIsUseExtdFilterMode, AdoInptPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化WebRtc浮点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化WebRtc浮点版声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
			break;
		}
		case 4: //如果使用SpeexWebRtc三重声学回音消除器。
		{
			if( SpeexWebRtcAecInit( &AdoInptPt->m_SpeexWebRtcAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_SpeexWebRtcAecWorkMode, AdoInptPt->m_SpeexWebRtcAecSpeexAecFilterLen, AdoInptPt->m_SpeexWebRtcAecSpeexAecIsUseRec, AdoInptPt->m_SpeexWebRtcAecSpeexAecEchoMutp, AdoInptPt->m_SpeexWebRtcAecSpeexAecEchoCntu, AdoInptPt->m_SpeexWebRtcAecSpeexAecEchoSupes, AdoInptPt->m_SpeexWebRtcAecSpeexAecEchoSupesAct, AdoInptPt->m_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecmEchoMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecmDelay, AdoInptPt->m_SpeexWebRtcAecWebRtcAecEchoMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecDelay, AdoInptPt->m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_SpeexWebRtcAecIsUseSameRoomAec, AdoInptPt->m_SpeexWebRtcAecSameRoomEchoMinDelay, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化SpeexWebRtc三重声学回音消除器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化SpeexWebRtc三重声学回音消除器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
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
		case 1: //如果使用Speex声学回音消除器。
		{
			if( AdoInptPt->m_SpeexAecPt != NULL )
			{
				if( AdoInptPt->m_SpeexAecIsSaveMemFile != 0 ) //如果Speex声学回音消除器要保存内存块到文件。
				{
					if( SpeexAecSaveMemFile( AdoInptPt->m_SpeexAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_SpeexAecFilterLen, AdoInptPt->m_SpeexAecIsUseRec, AdoInptPt->m_SpeexAecEchoMutp, AdoInptPt->m_SpeexAecEchoCntu, AdoInptPt->m_SpeexAecEchoSupes, AdoInptPt->m_SpeexAecEchoSupesAct, AdoInptPt->m_SpeexAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %vs 成功。" ), AdoInptPt->m_SpeexAecMemFileFullPathVstrPt );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：将Speex声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), AdoInptPt->m_SpeexAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
				if( SpeexAecDstoy( AdoInptPt->m_SpeexAecPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex声学回音消除器失败。" ) );
				}
				AdoInptPt->m_SpeexAecPt = NULL;
			}
			break;
		}
		case 2: //如果使用WebRtc定点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAecmPt != NULL )
			{
				if( WebRtcAecmDstoy( AdoInptPt->m_WebRtcAecmPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc定点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc定点版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAecmPt = NULL;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版声学回音消除器。
		{
			if( AdoInptPt->m_WebRtcAecPt != NULL )
			{
				if( AdoInptPt->m_WebRtcAecIsSaveMemFile != 0 ) //如果WebRtc浮点版声学回音消除器要保存内存块到文件。
				{
					if( WebRtcAecSaveMemFile( AdoInptPt->m_WebRtcAecPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcAecEchoMode, AdoInptPt->m_WebRtcAecDelay, AdoInptPt->m_WebRtcAecIsUseDelayAgstcMode, AdoInptPt->m_WebRtcAecIsUseExtdFilterMode, AdoInptPt->m_WebRtcAecIsUseRefinedFilterAdaptAecMode, AdoInptPt->m_WebRtcAecIsUseAdaptAdjDelay, AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 成功。" ), AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：将WebRtc浮点版声学回音消除器内存块保存到指定的文件 %vs 失败。原因：%vs" ), AdoInptPt->m_WebRtcAecMemFileFullPathVstrPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				}
				if( WebRtcAecDstoy( AdoInptPt->m_WebRtcAecPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版声学回音消除器失败。" ) );
				}
				AdoInptPt->m_WebRtcAecPt = NULL;
			}
			break;
		}
		case 4: //如果使用SpeexWebRtc三重声学回音消除器。
		{
			if( AdoInptPt->m_SpeexWebRtcAecPt != NULL )
			{
				if( SpeexWebRtcAecDstoy( AdoInptPt->m_SpeexWebRtcAecPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁SpeexWebRtc三重声学回音消除器失败。" ) );
				}
				AdoInptPt->m_SpeexWebRtcAecPt = NULL;
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
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
		if( AdoInptPt->m_IsUseAdoInpt == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为不使用音频输入，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为不使用音频输出，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate != AdoInptPt->m_SmplRate )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为音频输出的采样频率与音频输入不一致，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLen != AdoInptPt->m_FrmLen )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为音频输出帧的长度与音频输入不一致，所以不可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为要使用声学回音消除器，所以可以使用声学回音消除器。" ) );
			AdoInptPt->m_IsCanUseAec = 1; //设置是否可以使用声学回音消除器为可以。
		}
	}
	else //如果不使用声学回音消除器。
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：因为不使用声学回音消除器，所以不可以使用声学回音消除器。" ) );
		AdoInptPt->m_IsCanUseAec = 0; //设置是否可以使用声学回音消除器为不可以。
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptNsInit
 * 功能说明：初始化音频输入的噪音抑制器。
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

int AdoInptNsInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatNs )
	{
		case 0: //如果不使用噪音抑制器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：不使用噪音抑制器。" ) );
			break;
		}
		case 1: //如果使用Speex预处理器的噪音抑制。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：在初始化Speex预处理器时一起初始化Speex预处理器的噪音抑制。" ) );
			break;
		}
		case 2: //如果使用WebRtc定点版噪音抑制器。
		{
			if( WebRtcNsxInit( &AdoInptPt->m_WebRtcNsxPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcNsxPolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化WebRtc定点版噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化WebRtc定点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版噪音抑制器。
		{
			if( WebRtcNsInit( &AdoInptPt->m_WebRtcNsPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_WebRtcNsPolicyMode, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化WebRtc浮点版噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化WebRtc浮点版噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 4: //如果使用RNNoise噪音抑制器。
		{
			if( RNNoiseInit( &AdoInptPt->m_RNNoisePt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化RNNoise噪音抑制器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化RNNoise噪音抑制器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
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
		case 1: //如果使用Speex预处理器的噪音抑制。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：在销毁Speex预处理器时一起销毁Speex预处理器的噪音抑制。" ) );
			break;
		}
		case 2: //如果使用WebRtc定点版噪音抑制器。
		{
			if( AdoInptPt->m_WebRtcNsxPt != NULL )
			{
				if( WebRtcNsxDstoy( AdoInptPt->m_WebRtcNsxPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc定点版噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc定点版噪音抑制器失败。" ) );
				}
				AdoInptPt->m_WebRtcNsxPt = NULL;
			}
			break;
		}
		case 3: //如果使用WebRtc浮点版噪音抑制器。
		{
			if( AdoInptPt->m_WebRtcNsPt != NULL )
			{
				if( WebRtcNsDstoy( AdoInptPt->m_WebRtcNsPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁WebRtc浮点版噪音抑制器失败。" ) );
				}
				AdoInptPt->m_WebRtcNsPt = NULL;
			}
			break;
		}
		case 4: //如果使用RNNoise噪音抑制器。
		{
			if( AdoInptPt->m_RNNoisePt != NULL )
			{
				if( RNNoiseDstoy( AdoInptPt->m_RNNoisePt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁RNNoise噪音抑制器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁RNNoise噪音抑制器失败。" ) );
				}
				AdoInptPt->m_RNNoisePt = NULL;
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int AdoInptSpeexPrpocsInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	if( ( AdoInptPt->m_UseWhatNs == 1 ) || ( AdoInptPt->m_IsUseSpeexPrpocsOther != 0 ) )
	{
		if( SpeexPrpocsInit( &AdoInptPt->m_SpeexPrpocsPt, AdoInptPt->m_SmplRate, AdoInptPt->m_FrmLen, ( AdoInptPt->m_UseWhatNs == 1 ) ? AdoInptPt->m_SpeexPrpocsIsUseNs : 0, AdoInptPt->m_SpeexPrpocsNoiseSupes, ( AdoInptPt->m_UseWhatNs == 1 ) ? AdoInptPt->m_SpeexPrpocsIsUseDereverb : 0, ( AdoInptPt->m_IsUseSpeexPrpocsOther != 0 ) ? AdoInptPt->m_SpeexPrpocsIsUseVad : 0, AdoInptPt->m_SpeexPrpocsVadProbStart, AdoInptPt->m_SpeexPrpocsVadProbCntu, ( AdoInptPt->m_IsUseSpeexPrpocsOther != 0 ) ? AdoInptPt->m_SpeexPrpocsIsUseAgc : 0, AdoInptPt->m_SpeexPrpocsAgcLevel, AdoInptPt->m_SpeexPrpocsAgcIncrement, AdoInptPt->m_SpeexPrpocsAgcDecrement, AdoInptPt->m_SpeexPrpocsAgcMaxGain, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化Speex预处理器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化Speex预处理器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoInptSpeexPrpocsDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_SpeexPrpocsPt != NULL )
	{
		if( SpeexPrpocsDstoy( AdoInptPt->m_SpeexPrpocsPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex预处理器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex预处理器失败。" ) );
		}
		AdoInptPt->m_SpeexPrpocsPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptEncdInit
 * 功能说明：初始化音频输入的编码器。
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

int AdoInptEncdInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	switch( AdoInptPt->m_UseWhatEncd )
	{
		case 0: //如果使用PCM原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化PCM原始数据成功。" ) );
			break;
		}
		case 1: //如果使用Speex编码器。
		{
			if( AdoInptPt->m_FrmLen != AdoInptPt->m_SmplRate / 1000 * 20 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：帧的长度不为20毫秒不能使用Speex编码器。" ) );
				goto Out;
			}
			if( SpeexEncdInit( &AdoInptPt->m_SpeexEncdPt, AdoInptPt->m_SmplRate, AdoInptPt->m_SpeexEncdUseCbrOrVbr, AdoInptPt->m_SpeexEncdQualt, AdoInptPt->m_SpeexEncdCmplxt, AdoInptPt->m_SpeexEncdPlcExptLossRate ) == 0 )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化Speex编码器成功。" ) );
			}
			else
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化Speex编码器失败。" ) );
				goto Out;
			}
			break;
		}
		case 2: //如果使用Opus编码器。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：暂不支持使用Opus编码器。" ) );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
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
		case 0: //如果使用PCM原始数据。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁PCM原始数据成功。" ) );
			break;
		}
		case 1: //如果使用Speex编码器。
		{
			if( AdoInptPt->m_SpeexEncdPt != NULL )
			{
				if( SpeexEncdDstoy( AdoInptPt->m_SpeexEncdPt ) == 0 )
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁Speex编码器成功。" ) );
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁Speex编码器失败。" ) );
				}
				AdoInptPt->m_SpeexEncdPt = NULL;
			}
			break;
		}
		case 2: //如果使用Opus编码器。
		{
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptWaveFileWriterInit
 * 功能说明：初始化音频输入的音频输入Wave文件写入器、音频结果Wave文件写入器。
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

int AdoInptWaveFileWriterInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_IsSaveAdoToFile != 0 )
	{
		if( WaveFileWriterInit( &AdoInptPt->m_AdoInptWaveFileWriterPt, AdoInptPt->m_AdoInptFileFullPathVstrPt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入文件 %vs 的Wave文件写入器成功。" ), AdoInptPt->m_AdoInptFileFullPathVstrPt );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输入文件 %vs 的Wave文件写入器失败。" ), AdoInptPt->m_AdoInptFileFullPathVstrPt );
			goto Out;
		}
		if( WaveFileWriterInit( &AdoInptPt->m_AdoRsltWaveFileWriterPt, AdoInptPt->m_AdoRsltFileFullPathVstrPt, 1, AdoInptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频结果文件 %vs 的Wave文件写入器成功。" ), AdoInptPt->m_AdoRsltFileFullPathVstrPt );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频结果文件 %vs 的Wave文件写入器失败。" ), AdoInptPt->m_AdoRsltFileFullPathVstrPt );
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
 * 功能说明：销毁音频输入的音频输入Wave文件写入器、音频结果Wave文件写入器。
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

void AdoInptWaveFileWriterDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_AdoInptWaveFileWriterPt != NULL )
	{
		if( WaveFileWriterDstoy( AdoInptPt->m_AdoInptWaveFileWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入Wave文件写入器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_AdoInptWaveFileWriterPt = NULL;
	}
	if( AdoInptPt->m_AdoRsltWaveFileWriterPt != NULL )
	{
		if( WaveFileWriterDstoy( AdoInptPt->m_AdoRsltWaveFileWriterPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频结果Wave文件写入器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频结果Wave文件写入器失败。" ) );
		}
		AdoInptPt->m_AdoRsltWaveFileWriterPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptWavfmInit
 * 功能说明：初始化音频输入的音频输入波形器、音频结果波形器。
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

int AdoInptWavfmInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	if( AdoInptPt->m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoWavfmInit( &AdoInptPt->m_AdoInptWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输入波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		if( AdoWavfmInit( &AdoInptPt->m_AdoRsltWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频结果波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频结果波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 功能说明：销毁音频输入的音频输入波形器、音频结果波形器。
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

void AdoInptWavfmDstoy( AdoInpt * AdoInptPt )
{
	if( AdoInptPt->m_AdoInptWavfmPt != NULL )
	{
		if( AdoWavfmDstoy( AdoInptPt->m_AdoInptWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_AdoInptWavfmPt = NULL;
	}
	if( AdoInptPt->m_AdoRsltWavfmPt != NULL )
	{
		if( AdoWavfmDstoy( AdoInptPt->m_AdoRsltWavfmPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频结果波形器成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频结果波形器失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		AdoInptPt->m_AdoRsltWavfmPt = NULL;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptDvcAndThrdInit
 * 功能说明：初始化音频输入的音频输入设备和线程。
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

int AdoInptDvcAndThrdInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//初始化音频输入设备。
	{
		HRESULT p_HRslt;

		p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&AdoInptPt->m_AdoInptDvcEnumPt ); //初始化音频输入设备枚举器。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备枚举器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		if( AdoInptPt->m_AdoInptDvcID == 0 ) //如果要使用默认的音频输入设备。
		{
			p_HRslt = AdoInptPt->m_AdoInptDvcEnumPt->GetDefaultAudioEndpoint( eCapture, eConsole, &AdoInptPt->m_AdoInptDvcPt ); //初始化音频输入设备。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：初始化音频输入设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}
		else //如果要使用指定的音频输入设备。
		{
			p_HRslt = AdoInptPt->m_AdoInptDvcEnumPt->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &AdoInptPt->m_AdoInptDvcClctPt ); //获取音频输入设备收集器。只收集激活的音频输入设备。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输入设备收集器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoInptPt->m_AdoInptDvcClctPt->Item( AdoInptPt->m_AdoInptDvcID - 1, &AdoInptPt->m_AdoInptDvcPt ); //初始化音频输入设备。
			if( p_HRslt != S_OK )
			{
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoInptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：初始化音频输入设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcPt->Activate( IID_IAudioClient, CLSCTX_INPROC_SERVER, NULL, ( void * * )&AdoInptPt->m_AdoInptDvcClntPt ); //激活音频输入设备的客户端接口。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "激活音频输入设备的客户端接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcClntPt->GetMixFormat( &AdoInptPt->m_AdoInptDvcWaveFmtExPt ); //获取音频输入设备的格式。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输入设备的格式失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		//修改音频格式为16位整型，不能修改采样频率和声道数。
		if( AdoInptPt->m_AdoInptDvcWaveFmtExPt->wFormatTag != WAVE_FORMAT_EXTENSIBLE )
		{
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->wFormatTag = WAVE_FORMAT_PCM;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->wBitsPerSample = 16;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->nBlockAlign = AdoInptPt->m_AdoInptDvcWaveFmtExPt->nChannels * AdoInptPt->m_AdoInptDvcWaveFmtExPt->wBitsPerSample / 8;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->nAvgBytesPerSec = AdoInptPt->m_AdoInptDvcWaveFmtExPt->nBlockAlign * AdoInptPt->m_AdoInptDvcWaveFmtExPt->nSamplesPerSec;
		}
		else
		{
			( ( WAVEFORMATEXTENSIBLE * )AdoInptPt->m_AdoInptDvcWaveFmtExPt )->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			( ( WAVEFORMATEXTENSIBLE * )AdoInptPt->m_AdoInptDvcWaveFmtExPt )->Samples.wValidBitsPerSample = 16;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->wBitsPerSample = 16;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->nBlockAlign = AdoInptPt->m_AdoInptDvcWaveFmtExPt->nChannels * AdoInptPt->m_AdoInptDvcWaveFmtExPt->wBitsPerSample / 8;
			AdoInptPt->m_AdoInptDvcWaveFmtExPt->nAvgBytesPerSec = AdoInptPt->m_AdoInptDvcWaveFmtExPt->nBlockAlign * AdoInptPt->m_AdoInptDvcWaveFmtExPt->nSamplesPerSec;
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcClntPt->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, 10 * 1000 * ( AdoInptPt->m_FrmLen * 1000 / AdoInptPt->m_SmplRate ), 0, AdoInptPt->m_AdoInptDvcWaveFmtExPt, NULL ); //初始化音频输入设备客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcClntPt->GetBufferSize( &AdoInptPt->m_AdoInptDvcBufSz ); //获取音频输入设备的缓冲区大小。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输入设备的缓冲区大小失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcClntPt->GetService( IID_IAudioCaptureClient, ( void * * )&AdoInptPt->m_AdoInptDvcCptrClntPt ); //获取音频输入设备的捕获客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输入设备的捕获客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoInptPt->m_AdoInptDvcGlblIntfcTablePt ); //初始化音频输入设备全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoInptPt->m_AdoInptDvcGlblIntfcTablePt->RegisterInterfaceInGlobal( AdoInptPt->m_AdoInptDvcCptrClntPt, IID_IAudioCaptureClient, &AdoInptPt->m_AdoInptDvcCptrClntCookie ); //注册音频输入设备的捕获客户端到全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "注册音频输入设备的捕获客户端到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		AdoInptPt->m_AdoInptDvcFrmSz = ( AdoInptPt->m_FrmLen * 1000 / AdoInptPt->m_SmplRate ) * AdoInptPt->m_AdoInptDvcWaveFmtExPt->nSamplesPerSec / 1000; //设置音频输入设备帧的大小。
		if( AdoInptPt->m_AdoInptDvcBufQueue.Init( BufAutoAdjMethFreeNumber, AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ), 0, AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ), AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ) * 50, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输入设备缓冲区队列失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}

		AdoInptPt->m_AdoInptDvcFrmPt = ( int16_t * )malloc( AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ) ); //创建音频输入设备帧的内存块。
		if( AdoInptPt->m_AdoInptDvcFrmPt == NULL )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "创建音频输入设备帧的内存块失败。原因：内存不足。" ) );
			goto Out;
		}

		if( SpeexResamplerInit( &AdoInptPt->m_AdoInptDvcSpeexResamplerPt, AdoInptPt->m_AdoInptDvcWaveFmtExPt->nSamplesPerSec, AdoInptPt->m_SmplRate, 3, NULL ) != 0 )
		{
			LOGE( Cu8vstr( "初始化音频输入设备Speex重采样器失败。" ) );
			goto Out;
		}

		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入设备成功。音频输入缓冲区大小：%uz32d。" ), AdoInptPt->m_AdoInptDvcBufSz );
	}

	//初始化音频输入帧链表。
	if( AdoInptPt->m_AdoInptFrmLnkLst.Init( sizeof( int16_t * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入帧链表成功。" ) );
	}
	else
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输入帧链表失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化音频输入空闲帧链表。
	if( AdoInptPt->m_AdoInptIdleFrmLnkLst.Init( sizeof( int16_t * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入空闲帧链表成功。" ) );
	}
	else
	{
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输入空闲帧链表失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化音频输入线程的临时变量。
	{
		AdoInptPt->m_IsInitAdoInptThrdTmpVar = 1; //设置已初始化音频输入线程的临时变量。
		AdoInptPt->m_AdoInptFrmPt = NULL; //初始化音频输入帧的指针。
		AdoInptPt->m_AdoInptFrmLnkLstElmTotal = 0; //初始化音频输入帧链表的元数总数。
		AdoInptPt->m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
		AdoInptPt->m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化音频输入线程的临时变量成功。" ) );
	}

	//初始化音频输入线程。
	{
		AdoInptPt->m_AdoInptThrdExitFlag = 0; //设置音频输入线程退出标记为0表示保持运行。
		if( ThrdInit( &AdoInptPt->m_AdoInptThrdInfoPt, ( LPTHREAD_START_ROUTINE )AdoInptThrdRun, AdoInptPt, AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输入线程失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入线程成功。" ) );
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
 * 功能说明：销毁音频输入的音频输入设备和线程。
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

void AdoInptDvcAndThrdDstoy( AdoInpt * AdoInptPt )
{
	//销毁音频输入线程。
	if( AdoInptPt->m_AdoInptThrdInfoPt != NULL )
	{
		AdoInptPt->m_AdoInptThrdExitFlag = 1; //请求音频输入线程退出。
		ThrdWaitDstoy( AdoInptPt->m_AdoInptThrdInfoPt, NULL ); //等待音频输入线程退出并销毁。
		AdoInptPt->m_AdoInptThrdInfoPt = NULL;
		AdoInptPt->m_AdoInptThrdExitFlag = 0;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入线程成功。" ) );
	}

	//销毁音频输入线程的临时变量。
	if( AdoInptPt->m_IsInitAdoInptThrdTmpVar != 0 )
	{
		AdoInptPt->m_IsInitAdoInptThrdTmpVar = 0; //设置未初始化音频输入线程的临时变量。
		AdoInptPt->m_AdoInptFrmPt = NULL; //销毁音频输入帧的指针。
		AdoInptPt->m_AdoInptFrmLnkLstElmTotal = 0; //销毁音频输入帧链表的元数总数。
		AdoInptPt->m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
		AdoInptPt->m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入线程的临时变量成功。" ) );
	}

	//销毁音频输入空闲帧链表。
	if( AdoInptPt->m_AdoInptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( AdoInptPt->m_AdoInptIdleFrmLnkLst.GetHead( NULL, &AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL ) == 0 )
		{
			free( AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt );
			AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
			AdoInptPt->m_AdoInptIdleFrmLnkLst.DelHead( 0, NULL );
		}
		if( AdoInptPt->m_AdoInptIdleFrmLnkLst.Dstoy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入空闲帧链表成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入空闲帧链表失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	//销毁音频输入帧链表。
	if( AdoInptPt->m_AdoInptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( AdoInptPt->m_AdoInptFrmLnkLst.GetHead( NULL, &AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt, NULL, 0, NULL ) == 0 )
		{
			free( AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt );
			AdoInptPt->m_MediaPocsThrdPt->m_PcmAdoInptFrmPt = NULL;
			AdoInptPt->m_AdoInptFrmLnkLst.DelHead( 0, NULL );
		}
		if( AdoInptPt->m_AdoInptFrmLnkLst.Dstoy( AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入帧链表成功。" ) );
		}
		else
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输入帧链表失败。原因：%vs" ), AdoInptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁音频输入设备。
	{
		if( AdoInptPt->m_AdoInptDvcSpeexResamplerPt != NULL ) //销毁音频输入设备Speex重采样器。
		{
			SpeexResamplerDstoy( AdoInptPt->m_AdoInptDvcSpeexResamplerPt );
			AdoInptPt->m_AdoInptDvcSpeexResamplerPt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcFrmPt != NULL ) //销毁音频输入设备帧。
		{
			free( AdoInptPt->m_AdoInptDvcFrmPt );
			AdoInptPt->m_AdoInptDvcFrmPt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcBufQueue.m_BufQueuePt != NULL ) //销毁音频输入设备缓冲区队列。
		{
			AdoInptPt->m_AdoInptDvcBufQueue.Dstoy( NULL );
		}
		AdoInptPt->m_AdoInptDvcFrmSz = 0;
		if( AdoInptPt->m_AdoInptDvcCptrClntCookie != 0 ) //销毁音频输入设备捕获客户端的Cookie。
		{
			AdoInptPt->m_AdoInptDvcGlblIntfcTablePt->RevokeInterfaceFromGlobal( AdoInptPt->m_AdoInptDvcCptrClntCookie );
			AdoInptPt->m_AdoInptDvcCptrClntCookie = 0;
		}
		if( AdoInptPt->m_AdoInptDvcGlblIntfcTablePt != NULL ) //销毁音频输入设备的全局接口表。
		{
			AdoInptPt->m_AdoInptDvcGlblIntfcTablePt->Release();
			AdoInptPt->m_AdoInptDvcGlblIntfcTablePt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcCptrClntPt != NULL ) //销毁音频输入设备的捕获客户端。
		{
			AdoInptPt->m_AdoInptDvcCptrClntPt->Release();
			AdoInptPt->m_AdoInptDvcCptrClntPt = NULL;
		}
		AdoInptPt->m_AdoInptDvcBufSz = 0; //销毁音频输入设备的缓冲区大小。
		AdoInptPt->m_AdoInptDvcWaveFmtExPt = NULL; //销毁音频输入设备的格式。
		if( AdoInptPt->m_AdoInptDvcClntPt != NULL ) //销毁音频输入设备的客户端。
		{
			AdoInptPt->m_AdoInptDvcClntPt->Stop();
			AdoInptPt->m_AdoInptDvcClntPt->Release();
			AdoInptPt->m_AdoInptDvcClntPt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcPt != NULL ) //销毁音频输入设备。
		{
			AdoInptPt->m_AdoInptDvcPt->Release();
			AdoInptPt->m_AdoInptDvcPt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcClctPt != NULL ) //销毁音频输入设备收集器。
		{
			AdoInptPt->m_AdoInptDvcClctPt->Release();
			AdoInptPt->m_AdoInptDvcClctPt = NULL;
		}
		if( AdoInptPt->m_AdoInptDvcEnumPt != NULL ) //销毁音频输入设备枚举器。
		{
			AdoInptPt->m_AdoInptDvcEnumPt->Release();
			AdoInptPt->m_AdoInptDvcEnumPt = NULL;
		}
		AdoInptPt->m_AdoInptDvcIsClos = 0;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输入设备成功。" ) );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoInptInit
 * 功能说明：初始化音频输入。
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

int AdoInptInit( AdoInpt * AdoInptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

	if( AdoInptAecInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptNsInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptSpeexPrpocsInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptEncdInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptWaveFileWriterInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptWavfmInit( AdoInptPt ) != 0 ) goto Out;
	if( AdoInptDvcAndThrdInit( AdoInptPt ) != 0 ) goto Out;

	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec ); //记录初始化结束的时间。
		LOGFI( Cu8vstr( "媒体处理线程：初始化音频输入耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的内存指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoInptDstoy( AdoInpt * AdoInptPt )
{
	AdoInptDvcAndThrdDstoy( AdoInptPt );
	AdoInptWavfmDstoy( AdoInptPt );
	AdoInptWaveFileWriterDstoy( AdoInptPt );
	AdoInptEncdDstoy( AdoInptPt );
	AdoInptSpeexPrpocsDstoy( AdoInptPt );
	AdoInptNsDstoy( AdoInptPt );
	AdoInptAecDstoy( AdoInptPt );
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
	IAudioCaptureClient * p_AdoInptDvcCptrClntPt; //存放音频输入设备捕获客户端的指针。
	int16_t * p_AdoInptDvcBufPt; //存放音频输入设备缓冲区的指针。
	UINT32 p_AdoInptDvcBufLen; //存放音频输入设备缓冲区的长度。
	DWORD p_AdoInptDvcBufFlag;
	size_t p_TmpSz;
	HRESULT p_HRslt;

	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

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

	//获取音频输入设备捕获客户端。
	{
		IGlobalInterfaceTable * p_AdoInptDvcGlblIntfcTablePt;
		CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&p_AdoInptDvcGlblIntfcTablePt );
		p_AdoInptDvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoInptPt->m_AdoInptDvcCptrClntCookie, IID_IAudioCaptureClient, ( void * * )&p_AdoInptDvcCptrClntPt );
		p_AdoInptDvcGlblIntfcTablePt->Release();
	}
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：开始准备音频输入。" ) );

	//自适应设置声学回音的延迟。
	if( AdoInptPt->m_IsCanUseAec != 0 ) //如果可以使用声学回音消除器，就自适应计算声学回音的延迟，并设置到声学回音消除器。
	{
		int32_t p_AdoInptDelay; //存放音频输入延迟。
		int32_t p_AdoOtptDelay; //存放音频输出延迟。
		int32_t p_Delay;
		int32_t p_TmpInt32;

		p_AdoInptDelay = AdoInptPt->m_AdoInptDvcBufSz * 1000 / AdoInptPt->m_AdoInptDvcWaveFmtExPt->nSamplesPerSec / ( AdoInptPt->m_AdoInptDvcWaveFmtExPt->wBitsPerSample / 8 );
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输入延迟 %z32d 毫秒。" ), p_AdoInptDelay );
		p_AdoOtptDelay = AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcBufSz * 1000 / AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcWaveFmtExPt->nSamplesPerSec / ( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcWaveFmtExPt->wBitsPerSample / 8 );
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出延迟 %z32d 毫秒。" ), p_AdoOtptDelay );
		p_Delay = p_AdoInptDelay + p_AdoOtptDelay;
		if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：声学回音延迟 %z32d 毫秒。" ), p_Delay );

		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt != NULL ) && ( WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc定点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			WebRtcAecmSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
			WebRtcAecmGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecmPt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt != NULL ) && ( WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用WebRtc浮点版声学回音消除器，且需要自适应设置回音的延迟。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecIsUseDelayAgstcMode == 0 ) //如果WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
			{
				WebRtcAecSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, p_Delay );
				WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt32 );
			}
			else //如果WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
			{
				WebRtcAecSetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, 20 );
				WebRtcAecGetDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_WebRtcAecPt, &p_TmpInt32 );
			}
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc定点版声学回音消除器需要自适应设置回音的延迟。
		{
			SpeexWebRtcAecSetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, p_Delay / 2 ); //WebRtc定点版声学回音消除器的回音延迟应为实际声学回音延迟的二分之一，这样效果最好。
			SpeexWebRtcAecGetWebRtcAecmDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt32 );
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc定点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
		if( ( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt != NULL ) && ( SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt32 ) == 0 ) && ( p_TmpInt32 == 0 ) ) //如果要使用SpeexWebRtc三重声学回音消除器，且WebRtc浮点版声学回音消除器需要自适应设置回音的延迟。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecWebRtcAecIsUseDelayAgstcMode == 0 ) //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器不使用回音延迟不可知模式。
			{
				SpeexWebRtcAecSetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, p_Delay );
				SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt32 );
			}
			else //如果SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器要使用回音延迟不可知模式。
			{
				SpeexWebRtcAecSetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, 20 );
				SpeexWebRtcAecGetWebRtcAecDelay( AdoInptPt->m_MediaPocsThrdPt->m_AdoInpt.m_SpeexWebRtcAecPt, &p_TmpInt32 );
			}
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：自适应设置SpeexWebRtc三重声学回音消除器的WebRtc浮点版声学回音消除器的回音延迟为 %z32d 毫秒。" ), p_TmpInt32 );
		}
	} //自适应设置声学回音的延迟完毕。
	{
		AdoInptPt->m_AdoInptDvcClntPt->Start(); //让音频输入设备开始录音。
		if( AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) //如果已初始化音频输出。
		{
			AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptDvcClntPt->Start(); //让音频输出设备开始播放。
			AdoInptPt->m_MediaPocsThrdPt->m_AdoOtpt.m_AdoOtptThrdIsStart = 1; //设置音频输出线程已开始。
		}
	}

	//音频输入循环开始。
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &AdoInptPt->m_LastTimeMsec );
	while( 1 )
	{
		//获取音频输入设备的缓冲区，并将音频输入数据放入音频输入设备缓冲区队列。
		p_HRslt = p_AdoInptDvcCptrClntPt->GetBuffer( ( BYTE * * )&p_AdoInptDvcBufPt, &p_AdoInptDvcBufLen, &p_AdoInptDvcBufFlag, NULL, NULL ); //获取音频输入设备的缓冲区。
		if( p_HRslt == S_OK ) //如果获取音频输入设备的缓冲区成功。
		{
			if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：AdoInptDvcBufLen：%uz32d，AdoInptDvcBufFlag：%uz32d。" ), p_AdoInptDvcBufLen, p_AdoInptDvcBufFlag );

			if( p_AdoInptDvcBufFlag & AUDCLNT_BUFFERFLAGS_SILENT ) //如果本次音频输入设备的缓冲区为静音。
			{
				//if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：AUDCLNT_BUFFERFLAGS_SILENT。" ) );
				memset( p_AdoInptDvcBufPt, 0, p_AdoInptDvcBufLen * sizeof( int16_t ) );
			}

			if( AdoInptPt->m_AdoInptDvcWaveFmtExPt->nChannels == 2 ) //如果音频输入设备为双声道。
			{
				for( UINT i = 0; i < p_AdoInptDvcBufLen; i++ ) //双声道合并为单声道。
					p_AdoInptDvcBufPt[ i ] = ( p_AdoInptDvcBufPt[ i * sizeof( int16_t ) ] + p_AdoInptDvcBufPt[ i * sizeof( int16_t ) + 1 ] ) / 2;
			}
			//fwrite( p_AdoInptDvcBufPt, p_AdoInptDvcBufLen * sizeof( int16_t ), 1, AdoInptFile1Pt );

			AdoInptPt->m_AdoInptDvcBufQueue.PutTail( p_AdoInptDvcBufPt, p_AdoInptDvcBufLen * sizeof( int16_t ), 0, NULL ); //将音频输入数据放入音频输入设备缓冲区队列。

			p_AdoInptDvcCptrClntPt->ReleaseBuffer( p_AdoInptDvcBufLen ); //释放音频输入设备的缓冲区。

		}
		else if( p_HRslt == AUDCLNT_E_DEVICE_INVALIDATED ) //如果音频输入设备已经关闭。
		{
			if( AdoInptPt->m_AdoInptDvcIsClos == 0 )
			{
				AdoInptPt->m_AdoInptDvcIsClos = 1; //设置音频输入设备已经关闭。
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到音频输入设备关闭消息。" ) );
			}
		}
		else if( p_HRslt == AUDCLNT_S_BUFFER_EMPTY ) //如果音频输入设备的缓冲区当前为空，表示还需要等待。
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

		AdoInptPt->m_AdoInptDvcBufQueue.GetLen( &p_TmpSz, 0, NULL ); //获取音频输入设备缓冲区队列的长度。
		if( p_TmpSz >= AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ) ) //如果音频输入设备缓冲区队列的长度够一个音频输入设备帧的长度。
		{
			//获取一个音频输入空闲帧。
			AdoInptPt->m_AdoInptIdleFrmLnkLst.GetTotal( &AdoInptPt->m_AdoInptFrmLnkLstElmTotal, 1, NULL ); //获取音频输入空闲帧链表的元素总数。
			if( AdoInptPt->m_AdoInptFrmLnkLstElmTotal > 0 ) //如果音频输入空闲帧链表中有音频输入空闲帧。
			{
				//从音频输入空闲帧链表中取出第一个音频输入空闲帧。
				{
					AdoInptPt->m_AdoInptIdleFrmLnkLst.Locked( NULL ); //音频输入空闲帧链表的互斥锁加锁。
					AdoInptPt->m_AdoInptIdleFrmLnkLst.GetHead( NULL, &AdoInptPt->m_AdoInptFrmPt, NULL, 0, NULL );
					AdoInptPt->m_AdoInptIdleFrmLnkLst.DelHead( 0, NULL );
					AdoInptPt->m_AdoInptIdleFrmLnkLst.Unlock( NULL ); //音频输入空闲帧链表的互斥锁解锁。
				}
				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：从音频输入空闲帧链表中取出第一个音频输入空闲帧，音频输入空闲帧链表元素个数：%uzd。" ), AdoInptPt->m_AdoInptFrmLnkLstElmTotal );
			}
			else //如果音频输入空闲帧链表中没有音频输入空闲帧。
			{
				AdoInptPt->m_AdoInptFrmLnkLst.GetTotal( &AdoInptPt->m_AdoInptFrmLnkLstElmTotal, 1, NULL ); //获取音频输入帧链表的元素总数。
				if( AdoInptPt->m_AdoInptFrmLnkLstElmTotal <= 50 )
				{
					AdoInptPt->m_AdoInptFrmPt = ( int16_t * )calloc( AdoInptPt->m_FrmLen * sizeof( int16_t ), 1 ); //创建一个音频输入空闲帧。
					if( AdoInptPt->m_AdoInptFrmPt == NULL )
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：音频输入空闲帧链表中没有音频输入空闲帧，创建一个音频输入空闲帧失败。" ) );
					}
					else
					{
						if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：音频输入空闲帧链表中没有音频输入空闲帧，创建一个音频输入空闲帧。" ) );
					}
				}
				else
				{
					if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输入线程：音频输入帧链表中音频输入帧数量为%uzd已经超过上限50，不再创建一个音频输入空闲帧。" ), AdoInptPt->m_AdoInptFrmLnkLstElmTotal );
					Sleep( 1 ); //暂停一下，避免CPU使用率过高。
				}
			}

			//追加本次音频输入帧到音频输入帧链表。
			if( AdoInptPt->m_AdoInptFrmPt != NULL ) //如果获取了一个音频输入空闲帧。
			{
				int32_t p_TmpInt;
				AdoInptPt->m_AdoInptDvcBufQueue.GetHead( AdoInptPt->m_AdoInptDvcFrmPt, AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ), NULL, 1, 0, NULL );
				//fwrite( AdoInptPt->m_AdoInptDvcFrmPt, AdoInptPt->m_AdoInptDvcFrmSz * sizeof( int16_t ), 1, AdoInptFile2Pt );
				SpeexResamplerPocs( AdoInptPt->m_AdoInptDvcSpeexResamplerPt, AdoInptPt->m_AdoInptDvcFrmPt, AdoInptPt->m_AdoInptDvcFrmSz, AdoInptPt->m_AdoInptFrmPt, AdoInptPt->m_FrmLen, &p_TmpInt );
				//fwrite( AdoInptPt->m_AdoInptFrmPt, p_TmpInt * 2, 1, AdoInptFile3Pt );
				AdoInptPt->m_AdoInptFrmLnkLst.PutTail( &AdoInptPt->m_AdoInptFrmPt, 1, NULL );
				AdoInptPt->m_AdoInptFrmPt = NULL;

				if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &AdoInptPt->m_NowTimeMsec );
					LOGFI( Cu8vstr( "音频输入线程：本次音频输入帧读取完毕，耗时 %uz64d 毫秒。" ), AdoInptPt->m_NowTimeMsec - AdoInptPt->m_LastTimeMsec );
					AdoInptPt->m_LastTimeMsec = AdoInptPt->m_NowTimeMsec;
				}
			}
		}

		if( AdoInptPt->m_AdoInptThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输入线程：本线程接收到退出请求，开始准备退出。" ) );
            break;
        }
	} //音频输入循环结束。

	CoUninitialize(); //销毁COM库。
	
	if( AdoInptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输入线程：本线程已退出。" ) );
	return 0;
}
