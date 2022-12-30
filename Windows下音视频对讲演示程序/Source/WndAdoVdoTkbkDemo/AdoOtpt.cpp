#include "AdoOtpt.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptAdoOtptStrmInit
 * 功能说明：初始化音频输出的音频输出流。
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

int AdoOtptAdoOtptStrmInit( AdoOtpt * AdoOtptPt, AdoOtptStrm * AdoOtptStrmPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//初始化解码器。
	switch( AdoOtptStrmPt->m_UseWhatDecd )
	{
		case 0: //如果使用PCM原始数据。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化PCM原始数据成功。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
			break;
		}
		case 1: //如果使用Speex解码器。
		{
			if( AdoOtptPt->m_FrmLen != AdoOtptPt->m_SmplRate / 1000 * 20 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：帧的长度不为20毫秒不能使用Speex解码器。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
				goto Out;
			}
			if( SpeexDecdInit( &AdoOtptStrmPt->m_SpeexDecdPt, AdoOtptPt->m_SmplRate, AdoOtptStrmPt->m_SpeexDecdIsUsePrcplEnhsmt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化Speex解码器成功。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化Speex解码器失败。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
				goto Out;
			}
			break;
		}
		case 2: //如果使用Opus解码器。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
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
 * 函数名称：AdoOtptAdoOtptStrmDstoy
 * 功能说明：销毁音频输出的音频输出流。
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

void AdoOtptAdoOtptStrmDstoy( AdoOtpt * AdoOtptPt, AdoOtptStrm * AdoOtptStrmPt )
{
	//销毁解码器。
	switch( AdoOtptStrmPt->m_UseWhatDecd )
	{
		case 0: //如果使用PCM原始数据。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁PCM原始数据成功。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
			break;
		}
		case 1: //如果使用Speex解码器。
		{
			if( AdoOtptStrmPt->m_SpeexDecdPt != NULL )
			{
				if( SpeexDecdDstoy( AdoOtptStrmPt->m_SpeexDecdPt ) == 0 )
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Speex解码器成功。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
				}
				else
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Speex解码器失败。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
				}
				AdoOtptStrmPt->m_SpeexDecdPt = NULL;
			}
			break;
		}
		case 2: //如果使用Opus编码器。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Opus编码器成功。" ), AdoOtptStrmPt->m_AdoOtptStrmIdx );
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptAddAdoOtptStrm
 * 功能说明：添加音频输出的音频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             AdoOtptStrmIdx：[输入]，存放音频输出流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptAddAdoOtptStrm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			goto Out;
		}
	}

	if( AdoOtptPt->m_AdoOtptStrmLnkLst.PutTail( NULL, 0, NULL ) != 0 )
	{
		goto Out;
	}
	AdoOtptPt->m_AdoOtptStrmLnkLst.GetTail( NULL, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL );
	p_AdoOtptStrmPt->m_AdoOtptStrmIdx = AdoOtptStrmIdx;
	p_AdoOtptStrmPt->m_UseWhatDecd = 0;

	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptDelAdoOtptStrm
 * 功能说明：删除音频输出的音频输出流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             AdoOtptStrmIdx：[输入]，存放音频输出流索引。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptDelAdoOtptStrm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );
			AdoOtptPt->m_AdoOtptStrmLnkLst.DelByIdx( p_AdoOtptStrmLnkLstIdx, 0, NULL );
			goto Out;
		}
	}

	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetAdoOtptStrmUsePcm
 * 功能说明：设置音频输出的音频输出流要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetAdoOtptStrmUsePcm( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );

			p_AdoOtptStrmPt->m_UseWhatDecd = 0;

			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmInit( AdoOtptPt, p_AdoOtptStrmPt );
			goto Out;
		}
	}

	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetAdoOtptStrmUseSpeexDecd
 * 功能说明：设置音频输出的音频输出流要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             IsUsePrcplEnhsmt：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetAdoOtptStrmUseSpeexDecd( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx, int32_t IsUsePrcplEnhsmt )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );

			p_AdoOtptStrmPt->m_UseWhatDecd = 1;
			p_AdoOtptStrmPt->m_SpeexDecdIsUsePrcplEnhsmt = IsUsePrcplEnhsmt;

			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmInit( AdoOtptPt, p_AdoOtptStrmPt );
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetAdoOtptStrmUseOpusDecd
 * 功能说明：设置音频输出的音频输出流要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetAdoOtptStrmUseOpusDecd( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );

			p_AdoOtptStrmPt->m_UseWhatDecd = 2;

			if( ( AdoOtptPt->m_IsInitAdoOtpt != 0 ) && ( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) ) AdoOtptAdoOtptStrmInit( AdoOtptPt, p_AdoOtptStrmPt );
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetAdoOtptStrmIsUse
 * 功能说明：设置音频输出的音频输出流是否使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetAdoOtptStrmIsUse( AdoOtpt * AdoOtptPt, int32_t AdoOtptStrmIdx, int32_t IsUseAdoOtptStrm )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。
	
	AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

	//查找音频输出流索引。
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_AdoOtptStrmIdx == AdoOtptStrmIdx ) //如果音频输出流索引找到了。
		{
			if( IsUseAdoOtptStrm != 0 ) //如果要使用音频输出流。
			{
				if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm == 0 ) //如果当前不使用音频输出流。
				{
					if( AdoOtptPt->m_IsInitAdoOtpt != 0 ) //如果已初始化音频输出。
					{
						if( AdoOtptAdoOtptStrmInit( AdoOtptPt, p_AdoOtptStrmPt ) == 0 ) //如果初始化音频输出流成功。
						{
							p_AdoOtptStrmPt->m_IsUseAdoOtptStrm = 1;
							AdoOtptPt->m_AdoOtptStrmUseTotal++;
						}
					}
					else //如果未初始化音频输出。
					{
						p_AdoOtptStrmPt->m_IsUseAdoOtptStrm = 1;
						AdoOtptPt->m_AdoOtptStrmUseTotal++;
					}
				}
			}
			else //如果不使用音频输出流。
			{
				if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) //如果当前要使用音频输出流。
				{
					if( AdoOtptPt->m_IsInitAdoOtpt != 0 ) //如果已初始化音频输出。
					{
						AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );
						p_AdoOtptStrmPt->m_IsUseAdoOtptStrm = 0;
						AdoOtptPt->m_AdoOtptStrmUseTotal--;
					}
					else //如果未初始化音频输出。
					{
						p_AdoOtptStrmPt->m_IsUseAdoOtptStrm = 0;
						AdoOtptPt->m_AdoOtptStrmUseTotal--;
					}
				}
			}
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptAdoOtptStrmLnkLstInit
 * 功能说明：初始化音频输出的音频输出流链表。
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

int AdoOtptAdoOtptStrmLnkLstInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。

	AdoOtptPt->m_AdoOtptStrmUseTotal = 0;
	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 )
		{
			if( AdoOtptAdoOtptStrmInit( AdoOtptPt, p_AdoOtptStrmPt ) != 0 ) goto Out;
			AdoOtptPt->m_AdoOtptStrmUseTotal++;
		}
	}
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化音频输出流链表成功。" ) );

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptAdoOtptStrmLnkLstDstoy
 * 功能说明：销毁音频输出的音频输出流链表。
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

void AdoOtptAdoOtptStrmLnkLstDstoy( AdoOtpt * AdoOtptPt )
{
	AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流的指针。

	for( size_t p_AdoOtptStrmLnkLstIdx = SIZE_MAX; AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmLnkLstIdx, &p_AdoOtptStrmLnkLstIdx, NULL, ( void * * )&p_AdoOtptStrmPt, 0, NULL ) == 0; )
	{
		if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 )
		{
			AdoOtptAdoOtptStrmDstoy( AdoOtptPt, p_AdoOtptStrmPt );
		}
	}
	AdoOtptPt->m_AdoOtptStrmUseTotal = 0;
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出流链表成功。" ) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptWaveFileWriterInit
 * 功能说明：初始化音频输出的音频输出Wave文件写入器。
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

int AdoOtptWaveFileWriterInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	if( AdoOtptPt->m_IsSaveAdoToFile != 0 )
	{
		if( WaveFileWriterInit( &AdoOtptPt->m_AdoOtptWaveFileWriterPt, AdoOtptPt->m_AdoOtptFileFullPathVstrPt, 1, AdoOtptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出文件 %vs 的Wave文件写入器成功。" ), AdoOtptPt->m_AdoOtptFileFullPathVstrPt );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输出文件 %vs 的Wave文件写入器失败。" ), AdoOtptPt->m_AdoOtptFileFullPathVstrPt );
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
 * 函数名称：AdoOtptWaveFileWriterDstoy
 * 功能说明：销毁音频输出的音频输出Wave文件写入器。
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

void AdoOtptWaveFileWriterDstoy( AdoOtpt * AdoOtptPt )
{
	if( AdoOtptPt->m_IsSaveAdoToFile != 0 )
	{
		if( AdoOtptPt->m_AdoOtptWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDstoy( AdoOtptPt->m_AdoOtptWaveFileWriterPt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出Wave文件写入器成功。" ) );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出Wave文件写入器失败。" ) );
			}
			AdoOtptPt->m_AdoOtptWaveFileWriterPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptWavfmInit
 * 功能说明：初始化音频输出的音频输出波形器。
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

int AdoOtptWavfmInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	if( AdoOtptPt->m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoWavfmInit( &AdoOtptPt->m_AdoOtptWavfmPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出波形器成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输出波形器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoOtptWavfmDstoy
 * 功能说明：销毁音频输出的音频输出波形器。
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

void AdoOtptWavfmDstoy( AdoOtpt * AdoOtptPt )
{
	if( AdoOtptPt->m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoOtptPt->m_AdoOtptWavfmPt != NULL )
		{
			if( AdoWavfmDstoy( AdoOtptPt->m_AdoOtptWavfmPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出波形器成功。" ) );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出波形器失败。" ) );
			}
			AdoOtptPt->m_AdoOtptWavfmPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptDvcAndThrdInit
 * 功能说明：初始化音频输出的音频输出设备和线程。
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

int AdoOtptDvcAndThrdInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。

	//初始化音频输出设备。
	{
		HRESULT p_HRslt;

		p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&AdoOtptPt->m_AdoOtptDvcEnumPt ); //初始化音频输出设备枚举器。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输出设备枚举器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		if( AdoOtptPt->m_AdoOtptDvcID == 0 ) //如果要使用默认的音频输出设备。
		{
			p_HRslt = AdoOtptPt->m_AdoOtptDvcEnumPt->GetDefaultAudioEndpoint( eRender, eConsole, &AdoOtptPt->m_AdoOtptDvcPt ); //初始化音频输出设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输出设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoOtptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：初始化音频输出设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}
		else //如果要使用指定的音频输出设备。
		{
			p_HRslt = AdoOtptPt->m_AdoOtptDvcEnumPt->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, &AdoOtptPt->m_AdoOtptDvcClctPt ); //获取音频输出设备收集器。只收集激活的音频输出设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输出设备收集器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoOtptPt->m_AdoOtptDvcClctPt->Item( AdoOtptPt->m_AdoOtptDvcID - 1, &AdoOtptPt->m_AdoOtptDvcPt ); //初始化音频输出设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输出设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoOtptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：初始化音频输出设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcPt->Activate( IID_IAudioClient, CLSCTX_INPROC_SERVER, NULL, ( void * * )&AdoOtptPt->m_AdoOtptDvcClntPt ); //激活音频输出设备的客户端接口。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "激活音频输出设备的客户端接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcClntPt->GetMixFormat( &AdoOtptPt->m_AdoOtptDvcWaveFmtExPt ); //获取音频输出设备的格式。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输出设备的格式失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		//修改音频格式为16位整型，不能修改采样频率和声道数。
		if( AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wFormatTag != WAVE_FORMAT_EXTENSIBLE )
		{
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wFormatTag = WAVE_FORMAT_PCM;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wBitsPerSample = 16;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nBlockAlign = AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nChannels * AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wBitsPerSample / 8;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nAvgBytesPerSec = AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nBlockAlign * AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nSamplesPerSec;
		}
		else
		{
			( ( WAVEFORMATEXTENSIBLE * )AdoOtptPt->m_AdoOtptDvcWaveFmtExPt )->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			( ( WAVEFORMATEXTENSIBLE * )AdoOtptPt->m_AdoOtptDvcWaveFmtExPt )->Samples.wValidBitsPerSample = 16;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wBitsPerSample = 16;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nBlockAlign = AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nChannels * AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->wBitsPerSample / 8;
			AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nAvgBytesPerSec = AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nBlockAlign * AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nSamplesPerSec;
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcClntPt->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, 10 * 1000 * ( AdoOtptPt->m_FrmLen * 1000 / AdoOtptPt->m_SmplRate ), 0, AdoOtptPt->m_AdoOtptDvcWaveFmtExPt, NULL ); //初始化音频输出设备客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输出设备客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcClntPt->GetBufferSize( &AdoOtptPt->m_AdoOtptDvcBufSz ); //获取音频输出设备的缓冲区大小。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输出设备的缓冲区大小失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcClntPt->GetService( IID_IAudioRenderClient, ( void * * )&AdoOtptPt->m_AdoOtptDvcRndrClntPt ); //获取音频输出设备的渲染客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "获取音频输出设备的渲染客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt ); //初始化音频输出设备全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "初始化音频输出设备全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt->RegisterInterfaceInGlobal( AdoOtptPt->m_AdoOtptDvcRndrClntPt, IID_IAudioRenderClient, &AdoOtptPt->m_AdoOtptDvcRndrClntCookie ); //注册音频输出设备的渲染客户端到全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "注册音频输出设备的渲染客户端到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		AdoOtptPt->m_AdoOtptDvcFrmSz = ( AdoOtptPt->m_FrmLen * 1000 / AdoOtptPt->m_SmplRate ) * AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nSamplesPerSec / 1000; //设置音频输出设备帧的大小。

		if( SpeexResamplerInit( &AdoOtptPt->m_AdoOtptDvcSpeexResamplerPt, AdoOtptPt->m_SmplRate, AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nSamplesPerSec, 3, NULL ) != 0 )
		{
			LOGE( Cu8vstr( "初始化音频输出设备Speex重采样器失败。" ) );
			goto Out;
		}

		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出设备成功。音频输出缓冲区大小：%uz32d。" ), AdoOtptPt->m_AdoOtptDvcBufSz );
	}

	//初始化音频输出帧链表。
	if( AdoOtptPt->m_AdoOtptFrmLnkLst.Init( sizeof( int16_t * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出帧链表成功。" ) );
	}
	else
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输出帧链表失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	
	//初始化音频输出空闲帧链表。
	if( AdoOtptPt->m_AdoOtptIdleFrmLnkLst.Init( sizeof( int16_t * ), BufAutoAdjMethFreeNumber, 1, 0, 1, SIZE_MAX, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出空闲帧链表成功。" ) );
	}
	else
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输出空闲帧链表失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化音频输出线程的临时变量。
	{
		AdoOtptPt->m_IsInitAdoOtptThrdTmpVar = 1; //设置已初始化音频输出线程的临时变量。
		AdoOtptPt->m_AdoOtptFrmPt = NULL; //初始化音频输出帧的指针。
		AdoOtptPt->m_AdoOtptMixFrmPt = ( int32_t * )malloc( AdoOtptPt->m_FrmLen * sizeof( int32_t ) ); //初始化音频输出混音帧的指针。
		if( AdoOtptPt->m_AdoOtptMixFrmPt == NULL )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建音频输出混音帧的内存块失败。" ) );
			goto Out;
		}
		AdoOtptPt->m_EncdAdoOtptFrmPt = ( uint8_t * )malloc( AdoOtptPt->m_FrmLen * sizeof( int8_t ) ); //初始化已编码格式音频输出帧的指针。
		if( AdoOtptPt->m_EncdAdoOtptFrmPt == NULL )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：创建已编码格式音频输出帧的内存块失败。" ) );
			goto Out;
		}
		AdoOtptPt->m_EncdAdoOtptFrmSz = AdoOtptPt->m_FrmLen * sizeof( int8_t ); //初始化已编码格式音频输出帧的内存大小，单位为字节。
		AdoOtptPt->m_AdoOtptFrmLen = 0; //初始化音频输出帧的长度，单位为字节。
		AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal = 0; //初始化音频输出帧链表的元数总数。
		AdoOtptPt->m_LastTimeMsec = 0; //初始化上次时间的毫秒数。
		AdoOtptPt->m_NowTimeMsec = 0; //初始化本次时间的毫秒数。
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：初始化音频输出线程的临时变量成功。" ) );
	}

	//初始化音频输出线程。
	{
		AdoOtptPt->m_AdoOtptThrdIsStart = 0; //设置音频输出线程为未开始。
		AdoOtptPt->m_AdoOtptThrdExitFlag = 0; //设置音频输出线程退出标记为0表示保持运行。
		if( ThrdInit( &AdoOtptPt->m_AdoOtptThrdInfoPt, ( LPTHREAD_START_ROUTINE )AdoOtptThrdRun, AdoOtptPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出线程成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：初始化音频输出线程失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 函数名称：AdoOtptDvcAndThrdDstoy
 * 功能说明：销毁音频输出的音频输出设备和线程。
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

void AdoOtptDvcAndThrdDstoy( AdoOtpt * AdoOtptPt )
{
	//销毁音频输出线程。
	if( AdoOtptPt->m_AdoOtptThrdInfoPt != NULL )
	{
		AdoOtptPt->m_AdoOtptThrdExitFlag = 1; //请求音频输出线程退出。
		ThrdWaitDstoy( AdoOtptPt->m_AdoOtptThrdInfoPt, NULL ); //等待音频输出线程退出并销毁。
		AdoOtptPt->m_AdoOtptThrdInfoPt = NULL;
		AdoOtptPt->m_AdoOtptThrdIsStart = 0;
		AdoOtptPt->m_AdoOtptThrdExitFlag = 0;
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出线程成功。" ) );
	}

	//销毁音频输出线程的临时变量。
	if( AdoOtptPt->m_IsInitAdoOtptThrdTmpVar != 0 )
	{
		AdoOtptPt->m_IsInitAdoOtptThrdTmpVar = 0; //设置未初始化音频输出线程的临时变量。
		AdoOtptPt->m_AdoOtptFrmPt = NULL; //销毁音频输出帧的指针。
		if( AdoOtptPt->m_AdoOtptMixFrmPt != NULL ) //销毁音频输出混音帧的指针。
		{
			free( AdoOtptPt->m_AdoOtptMixFrmPt );
			AdoOtptPt->m_AdoOtptMixFrmPt = NULL;
		}
		if( AdoOtptPt->m_EncdAdoOtptFrmPt != NULL ) //销毁已编码格式音频输出帧的指针。
		{
			free( AdoOtptPt->m_EncdAdoOtptFrmPt );
			AdoOtptPt->m_EncdAdoOtptFrmPt = NULL;
		}
		AdoOtptPt->m_EncdAdoOtptFrmSz = 0; //销毁已编码格式音频输出帧的内存大小，单位为字节。
		AdoOtptPt->m_AdoOtptFrmLen = 0; //销毁音频输出帧的长度，单位为字节。
		AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal = 0; //销毁音频输出帧链表的元数总数。
		AdoOtptPt->m_LastTimeMsec = 0; //销毁上次时间的毫秒数。
		AdoOtptPt->m_NowTimeMsec = 0; //销毁本次时间的毫秒数。
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出线程的临时变量成功。" ) );
	}

	//销毁音频输出空闲帧链表。
	if( AdoOtptPt->m_AdoOtptIdleFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( AdoOtptPt->m_AdoOtptIdleFrmLnkLst.GetHead( NULL, &AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL ) == 0 )
		{
			free( AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
			AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
			AdoOtptPt->m_AdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
		}
		if( AdoOtptPt->m_AdoOtptIdleFrmLnkLst.Dstoy( AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出空闲帧链表成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出空闲帧链表失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	//销毁音频输出帧链表。
	if( AdoOtptPt->m_AdoOtptFrmLnkLst.m_ConstLenLnkLstPt != NULL )
	{
		while( AdoOtptPt->m_AdoOtptFrmLnkLst.GetHead( NULL, &AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt, NULL, 0, NULL ) == 0 )
		{
			free( AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt );
			AdoOtptPt->m_MediaPocsThrdPt->m_PcmAdoOtptFrmPt = NULL;
			AdoOtptPt->m_AdoOtptFrmLnkLst.DelHead( 0, NULL );
		}
		if( AdoOtptPt->m_AdoOtptFrmLnkLst.Dstoy( AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出帧链表成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：销毁音频输出帧链表失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁音频输出设备。
	{
		if( AdoOtptPt->m_AdoOtptDvcSpeexResamplerPt != NULL ) //销毁音频输出设备Speex重采样器。
		{
			SpeexResamplerDstoy( AdoOtptPt->m_AdoOtptDvcSpeexResamplerPt );
			AdoOtptPt->m_AdoOtptDvcSpeexResamplerPt = NULL;
		}
		AdoOtptPt->m_AdoOtptDvcFrmSz = 0;
		if( AdoOtptPt->m_AdoOtptDvcRndrClntCookie != 0 ) //销毁音频输出设备渲染客户端的Cookie。
		{
			AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt->RevokeInterfaceFromGlobal( AdoOtptPt->m_AdoOtptDvcRndrClntCookie );
			AdoOtptPt->m_AdoOtptDvcRndrClntCookie = 0;
		}
		if( AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt != NULL ) //销毁音频输出设备的全局接口表。
		{
			AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt->Release();
			AdoOtptPt->m_AdoOtptDvcGlblIntfcTablePt = NULL;
		}
		if( AdoOtptPt->m_AdoOtptDvcRndrClntPt != NULL ) //销毁音频输出设备的渲染客户端。
		{
			AdoOtptPt->m_AdoOtptDvcRndrClntPt->Release();
			AdoOtptPt->m_AdoOtptDvcRndrClntPt = NULL;
		}
		AdoOtptPt->m_AdoOtptDvcBufSz = 0; //销毁音频输出设备的缓冲区大小。
		AdoOtptPt->m_AdoOtptDvcWaveFmtExPt = NULL; //销毁音频输出设备的格式。
		if( AdoOtptPt->m_AdoOtptDvcClntPt != NULL ) //销毁音频输出设备的客户端。
		{
			AdoOtptPt->m_AdoOtptDvcClntPt->Stop();
			AdoOtptPt->m_AdoOtptDvcClntPt->Release();
			AdoOtptPt->m_AdoOtptDvcClntPt = NULL;
		}
		if( AdoOtptPt->m_AdoOtptDvcPt != NULL ) //销毁音频输出设备。
		{
			AdoOtptPt->m_AdoOtptDvcPt->Release();
			AdoOtptPt->m_AdoOtptDvcPt = NULL;
		}
		if( AdoOtptPt->m_AdoOtptDvcClctPt != NULL ) //销毁音频输出设备收集器。
		{
			AdoOtptPt->m_AdoOtptDvcClctPt->Release();
			AdoOtptPt->m_AdoOtptDvcClctPt = NULL;
		}
		if( AdoOtptPt->m_AdoOtptDvcEnumPt != NULL ) //销毁音频输出设备枚举器。
		{
			AdoOtptPt->m_AdoOtptDvcEnumPt->Release();
			AdoOtptPt->m_AdoOtptDvcEnumPt = NULL;
		}
		AdoOtptPt->m_AdoOtptDvcIsClos = 0;
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：销毁音频输出设备成功。" ) );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptInit
 * 功能说明：初始化音频输出。
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

int AdoOtptInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	uint64_t p_LastMsec = 0;
	uint64_t p_NowMsec = 0;
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &p_LastMsec ); //记录初始化开始的时间。

	if( AdoOtptAdoOtptStrmLnkLstInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptWaveFileWriterInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptWavfmInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptDvcAndThrdInit( AdoOtptPt ) != 0 ) goto Out;

	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		FuncGetTimeAsMsec( &p_NowMsec ); //记录初始化结束的时间。
		LOGFI( Cu8vstr( "媒体处理线程：初始化音频输出耗时 %uz64d 毫秒。" ), p_NowMsec - p_LastMsec );
	}

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		AdoOtptDstoy( AdoOtptPt );
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptDstoy
 * 功能说明：销毁音频输出。
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

void AdoOtptDstoy( AdoOtpt * AdoOtptPt )
{
	AdoOtptDvcAndThrdDstoy( AdoOtptPt );
	AdoOtptWavfmDstoy( AdoOtptPt );
	AdoOtptWaveFileWriterDstoy( AdoOtptPt );
	AdoOtptAdoOtptStrmLnkLstDstoy( AdoOtptPt );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptThrdRun
 * 功能说明：音频输出线程主函数。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DWORD WINAPI AdoOtptThrdRun( AdoOtpt * AdoOtptPt )
{
	IAudioRenderClient * p_AdoOtptDvcRndrClntPt; //存放音频输出设备渲染客户端的指针。
	int16_t * p_AdoOtptDvcBufPt = NULL; //存放音频输出设备缓冲区的指针。
	size_t p_TmpSz;
	HRESULT p_HRslt;

	CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	/*FILE * AdoOtptFile1Pt = NULL;
	FILE * AdoOtptFile2Pt = NULL;
	FILE * AdoOtptFile3Pt = NULL;
	char path[ 1024 ];
	sprintf( path, "F:\\%lld-AdoOtptAndOtpt-Otpt1.pcm", time( NULL ) );
	AdoOtptFile1Pt = fopen( path, "wb+" );
	sprintf( path, "F:\\%lld-AdoOtptAndOtpt-Otpt2.pcm", time( NULL ) );
	AdoOtptFile2Pt = fopen( path, "wb+" );
	sprintf( path, "F:\\%lld-AdoOtptAndOtpt-Otpt3.pcm", time( NULL ) );
	AdoOtptFile3Pt = fopen( path, "wb+" );*/

	//获取音频输出设备渲染客户端。
	{
		IGlobalInterfaceTable * p_AdoOtptDvcGlblIntfcTablePt;
		CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&p_AdoOtptDvcGlblIntfcTablePt );
		p_AdoOtptDvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoOtptPt->m_AdoOtptDvcRndrClntCookie, IID_IAudioRenderClient, ( void * * )&p_AdoOtptDvcRndrClntPt );
		p_AdoOtptDvcGlblIntfcTablePt->Release();
	}
	
	while( AdoOtptPt->m_AdoOtptThrdIsStart == 0 ) Sleep( 1 ); //等待音频输出线程开始。

	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：开始准备音频输出。" ) );

	//音频输出循环开始。
	while( 1 )
	{
		if( p_AdoOtptDvcBufPt == NULL ) //如果还没有获取音频输出设备的缓冲区。
		{
			p_HRslt = p_AdoOtptDvcRndrClntPt->GetBuffer( AdoOtptPt->m_AdoOtptDvcFrmSz, ( BYTE * * )&p_AdoOtptDvcBufPt ); //获取音频输出设备的缓冲区。
			if( p_HRslt == S_OK ) //如果获取音频输出设备的缓冲区成功。
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：AdoOtptDvcFrmSz：%uzd。" ), AdoOtptPt->m_AdoOtptDvcFrmSz );
			}
			else if( p_HRslt == AUDCLNT_E_DEVICE_INVALIDATED ) //如果音频输出设备已经关闭。
			{
				if( AdoOtptPt->m_AdoOtptDvcIsClos == 0 )
				{
					AdoOtptPt->m_AdoOtptDvcIsClos = 1; //设置音频输出设备已经关闭。
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到音频输出设备关闭消息。" ) );
				}
			}
			else if( p_HRslt == AUDCLNT_E_BUFFER_ERROR )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_ERROR。" ) );
			}
			else if( p_HRslt == AUDCLNT_E_BUFFER_TOO_LARGE ) //如果获取音频输出设备的缓冲区还不够。
			{
				//if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_TOO_LARGE。" ) );
			}
			else if( p_HRslt == AUDCLNT_E_BUFFER_SIZE_ERROR )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_SIZE_ERROR。" ) );
			}
			else if( p_HRslt == AUDCLNT_E_OUT_OF_ORDER )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_OUT_OF_ORDER。" ) );
			}
			else if( p_HRslt == AUDCLNT_E_BUFFER_OPERATION_PENDING )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_OPERATION_PENDING。" ) );
			}
			else if( p_HRslt == AUDCLNT_E_SERVICE_NOT_RUNNING )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_SERVICE_NOT_RUNNING。" ) );
			}
			else if( p_HRslt == E_POINTER )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：E_POINTER。" ) );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：p_HRslt：%ld。" ), p_HRslt );
			}
		}

		if( p_AdoOtptDvcBufPt != NULL ) //如果已经获取音频输出设备的缓冲区。
		{
			//获取一个音频输出空闲帧。
			AdoOtptPt->m_AdoOtptIdleFrmLnkLst.GetTotal( &AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal, 1, NULL ); //获取音频输出空闲帧链表的元素总数。
			if( AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal > 0 ) //如果音频输出空闲帧链表中有音频输出空闲帧。
			{
				//从音频输出空闲帧链表中取出第一个音频输出空闲帧。
				{
					AdoOtptPt->m_AdoOtptIdleFrmLnkLst.Locked( NULL ); //音频输出空闲帧链表的互斥锁加锁。
					AdoOtptPt->m_AdoOtptIdleFrmLnkLst.GetHead( NULL, &AdoOtptPt->m_AdoOtptFrmPt, NULL, 0, NULL );
					AdoOtptPt->m_AdoOtptIdleFrmLnkLst.DelHead( 0, NULL );
					AdoOtptPt->m_AdoOtptIdleFrmLnkLst.Unlock( NULL ); //音频输出空闲帧链表的互斥锁解锁。
				}
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：从音频输出空闲帧链表中取出第一个音频输出空闲帧，音频输出空闲帧链表元素个数：%uzd。" ), AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal );
			}
			else //如果音频输出空闲帧链表中没有音频输出空闲帧。
			{
				AdoOtptPt->m_AdoOtptFrmLnkLst.GetTotal( &AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal, 1, NULL ); //获取音频输出帧链表的元素总数。
				if( AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal <= 50 )
				{
					AdoOtptPt->m_AdoOtptFrmPt = ( int16_t * )calloc( AdoOtptPt->m_FrmLen * sizeof( int16_t ), 1 ); //创建一个音频输出空闲帧。
					if( AdoOtptPt->m_AdoOtptFrmPt == NULL )
					{
						if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "媒体处理线程：音频输出空闲帧链表中没有音频输出空闲帧，创建一个音频输出空闲帧失败。" ) );
					}
					else
					{
						if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：音频输出空闲帧链表中没有音频输出空闲帧，创建一个音频输出空闲帧。" ) );
					}
				}
				else
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出帧链表中音频输出帧数量为%uzd已经超过上限50，不再创建一个音频输出空闲帧。" ), AdoOtptPt->m_AdoOtptFrmLnkLstElmTotal );
					Sleep( 1 ); //暂停一下，避免CPU使用率过高。
				}
			}

			if( AdoOtptPt->m_AdoOtptFrmPt != NULL ) //如果获取了一个音频输出空闲帧。
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) FuncGetTimeAsMsec( &AdoOtptPt->m_LastTimeMsec );

				//调用用户定义的写入音频输出帧函数，并解码成PCM原始数据，最后混音。
				{
					AdoOtptPt->m_AdoOtptStrmLnkLst.Locked( NULL ); //音频输出流链表的互斥锁加锁。

					if( AdoOtptPt->m_AdoOtptStrmUseTotal > 0 )
					{
						size_t p_AdoOtptStrmDataIdx = SIZE_MAX;
						AdoOtptStrm * p_AdoOtptStrmPt; //存放音频输出流数据的指针。

						while( AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmDataIdx, &p_AdoOtptStrmDataIdx, NULL, ( void ** )&p_AdoOtptStrmPt, 0, NULL ) == 0 ) //查找第一条要使用的音频输出流。
						{
							if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) //如果该音频输出流为要使用。
							{
								switch( p_AdoOtptStrmPt->m_UseWhatDecd ) //使用什么解码器。
								{
									case 0: //如果使用PCM原始数据。
									{
										//调用用户定义的写入音频输出帧函数。
										AdoOtptPt->m_AdoOtptFrmLen = AdoOtptPt->m_FrmLen;
										AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, AdoOtptPt->m_AdoOtptFrmPt, NULL, &AdoOtptPt->m_AdoOtptFrmLen );
										break;
									}
									case 1: //如果使用Speex编解码器。
									{
										//调用用户定义的写入音频输出帧函数。
										AdoOtptPt->m_AdoOtptFrmLen = AdoOtptPt->m_EncdAdoOtptFrmSz;
										AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, NULL, AdoOtptPt->m_EncdAdoOtptFrmPt, &AdoOtptPt->m_AdoOtptFrmLen );

										//使用Speex解码器。
										if( SpeexDecdPocs( p_AdoOtptStrmPt->m_SpeexDecdPt, AdoOtptPt->m_EncdAdoOtptFrmPt, AdoOtptPt->m_AdoOtptFrmLen, AdoOtptPt->m_AdoOtptFrmPt ) == 0 )
										{
											if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器成功。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
										}
										else
										{
											if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器失败。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
										}
										break;
									}
									case 2: //如果使用Opus编解码器。
									{
										if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
									}
								}

								//调用用户定义的获取PCM格式音频输出帧函数。
								AdoOtptPt->m_MediaPocsThrdPt->m_UserGetPcmAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, AdoOtptPt->m_AdoOtptFrmPt, AdoOtptPt->m_FrmLen );
								break;
							}
						}

						if( AdoOtptPt->m_AdoOtptStrmUseTotal > 1 ) //查找其他要使用的音频输出流。
						{
							for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLen; p_TmpInt++ ) //将第一个音频输出流的音频输出帧复制到音频输出混音帧。
							{
								AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] = AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ];
							}

							while( AdoOtptPt->m_AdoOtptStrmLnkLst.GetNextByIdx( p_AdoOtptStrmDataIdx, &p_AdoOtptStrmDataIdx, NULL, ( void ** )&p_AdoOtptStrmPt, 0, NULL ) == 0 )
							{
								if( p_AdoOtptStrmPt->m_IsUseAdoOtptStrm != 0 ) //如果该音频输出流为要使用。
								{
									switch( p_AdoOtptStrmPt->m_UseWhatDecd ) //使用什么解码器。
									{
										case 0: //如果使用PCM原始数据。
										{
											//调用用户定义的写入音频输出帧函数。
											AdoOtptPt->m_AdoOtptFrmLen = AdoOtptPt->m_FrmLen;
											AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, AdoOtptPt->m_AdoOtptFrmPt, NULL, &AdoOtptPt->m_AdoOtptFrmLen );
											break;
										}
										case 1: //如果使用Speex编解码器。
										{
											//调用用户定义的写入音频输出帧函数。
											AdoOtptPt->m_AdoOtptFrmLen = AdoOtptPt->m_EncdAdoOtptFrmSz;
											AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, NULL, AdoOtptPt->m_EncdAdoOtptFrmPt, &AdoOtptPt->m_AdoOtptFrmLen );

											//使用Speex解码器。
											if( SpeexDecdPocs( p_AdoOtptStrmPt->m_SpeexDecdPt, AdoOtptPt->m_EncdAdoOtptFrmPt, AdoOtptPt->m_AdoOtptFrmLen, AdoOtptPt->m_AdoOtptFrmPt ) == 0 )
											{
												if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器成功。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
											}
											else
											{
												if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器失败。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
											}
											break;
										}
										case 2: //如果使用Opus编解码器。
										{
											if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), p_AdoOtptStrmPt->m_AdoOtptStrmIdx );
										}
									}

									//调用用户定义的获取PCM格式音频输出帧函数。
									AdoOtptPt->m_MediaPocsThrdPt->m_UserGetPcmAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_AdoOtptStrmPt->m_AdoOtptStrmIdx, AdoOtptPt->m_AdoOtptFrmPt, AdoOtptPt->m_FrmLen );

									for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLen; p_TmpInt++ ) //混音。
									{
										AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] = AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ] + AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] - ( ( AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ] * AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] ) >> 0x10 );
									}
								}

								for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLen; p_TmpInt++ ) //将第一个音频输出流的音频输出帧复制到音频输出混音帧。
								{
									if( AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] > 32767 ) AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ] = 32767;
									else if( AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ] < -32768 ) AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ] = -32768;
									else AdoOtptPt->m_AdoOtptFrmPt[ p_TmpInt ] = ( int16_t )AdoOtptPt->m_AdoOtptMixFrmPt[ p_TmpInt ];
								}
							}
						}
					}

					AdoOtptPt->m_AdoOtptStrmLnkLst.Unlock( NULL ); //音频输出流链表的互斥锁解锁。
				}

				//判断音频输出是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
				if( AdoOtptPt->m_AdoOtptIsMute != 0 )
				{
					memset( AdoOtptPt->m_AdoOtptFrmPt, 0, AdoOtptPt->m_FrmLen * sizeof( int16_t ) );
				}

				{
					int32_t p_TmpInt;
					//fwrite( AdoOtptPt->m_AdoOtptFrmPt, AdoOtptPt->m_FrmLen * sizeof( int16_t ), 1, AdoOtptFile1Pt );
					SpeexResamplerPocs( AdoOtptPt->m_AdoOtptDvcSpeexResamplerPt, AdoOtptPt->m_AdoOtptFrmPt, AdoOtptPt->m_FrmLen, p_AdoOtptDvcBufPt, AdoOtptPt->m_AdoOtptDvcFrmSz, &p_TmpInt );
					//fwrite( p_AdoOtptDvcBufPt, AdoOtptPt->m_AdoOtptDvcFrmSz * sizeof( int16_t ), 1, AdoOtptFile2Pt );
					if( AdoOtptPt->m_AdoOtptDvcWaveFmtExPt->nChannels == 2 ) //如果音频输出设备为双声道。
					{
						for( int i = AdoOtptPt->m_AdoOtptDvcFrmSz - 1; i >= 0; i-- ) //单声道分离为双声道。
							p_AdoOtptDvcBufPt[ i * 2 ] = p_AdoOtptDvcBufPt[ i ],
							p_AdoOtptDvcBufPt[ i * 2 + 1 ] = p_AdoOtptDvcBufPt[ i ];
						//fwrite( p_AdoOtptDvcBufPt, AdoOtptPt->m_AdoOtptDvcFrmSz * 2 * sizeof( int16_t ), 1, AdoOtptFile3Pt );
					}
				}

				p_AdoOtptDvcRndrClntPt->ReleaseBuffer( AdoOtptPt->m_AdoOtptDvcFrmSz, 0 ); //释放音频输出设备的缓冲区。
				p_AdoOtptDvcBufPt = NULL;

				//追加本次音频输出帧到音频输出帧链表。
				{
					AdoOtptPt->m_AdoOtptFrmLnkLst.PutTail( &AdoOtptPt->m_AdoOtptFrmPt, 1, NULL );
					AdoOtptPt->m_AdoOtptFrmPt = NULL;
				}

				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
				{
					FuncGetTimeAsMsec( &AdoOtptPt->m_NowTimeMsec );
					LOGFI( Cu8vstr( "音频输出线程：本次音频输出帧读取完毕，耗时 %uz64d 毫秒。" ), AdoOtptPt->m_NowTimeMsec - AdoOtptPt->m_LastTimeMsec );
					AdoOtptPt->m_LastTimeMsec = AdoOtptPt->m_NowTimeMsec;
				}
			}
		}

		if( AdoOtptPt->m_AdoOtptThrdExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到退出请求，开始准备退出。" ) );
            break;
        }
	} //音频输出循环完毕。

	CoUninitialize(); //销毁COM库。
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：本线程已退出。" ) );
	return 0;
}
