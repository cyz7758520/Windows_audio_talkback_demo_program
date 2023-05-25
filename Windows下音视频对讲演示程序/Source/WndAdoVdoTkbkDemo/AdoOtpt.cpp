#include "AdoOtpt.h"

DWORD WINAPI AdoOtptThrdRun( AdoOtpt * AdoOtptPt );

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptStrmInit
 * 功能说明：初始化音频输出的流。
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

int AdoOtptStrmInit( AdoOtpt * AdoOtptPt, AdoOtpt::Strm * StrmPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//初始化解码器。
	switch( StrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用PCM原始数据。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化PCM原始数据成功。" ), StrmPt->m_Idx );
			break;
		}
		case 1: //如果要使用Speex解码器。
		{
			if( AdoOtptPt->m_FrmLenMsec != 20 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：帧的长度不为20毫秒不能使用Speex解码器。" ), StrmPt->m_Idx );
				goto Out;
			}
			if( SpeexDecdInit( &StrmPt->m_SpeexDecd.m_Pt, AdoOtptPt->m_SmplRate, StrmPt->m_SpeexDecd.m_IsUsePrcplEnhsmt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化Speex解码器成功。" ), StrmPt->m_Idx );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：初始化Speex解码器失败。" ), StrmPt->m_Idx );
				goto Out;
			}
			break;
		}
		case 2: //如果要使用Opus解码器。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), StrmPt->m_Idx );
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
 * 函数名称：AdoOtptStrmDstoy
 * 功能说明：销毁音频输出的流。
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

void AdoOtptStrmDstoy( AdoOtpt * AdoOtptPt, AdoOtpt::Strm * StrmPt )
{
	//销毁解码器。
	switch( StrmPt->m_UseWhatDecd )
	{
		case 0: //如果要使用PCM原始数据。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁PCM原始数据成功。" ), StrmPt->m_Idx );
			break;
		}
		case 1: //如果要使用Speex解码器。
		{
			if( StrmPt->m_SpeexDecd.m_Pt != NULL )
			{
				if( SpeexDecdDstoy( StrmPt->m_SpeexDecd.m_Pt ) == 0 )
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Speex解码器成功。" ), StrmPt->m_Idx );
				}
				else
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Speex解码器失败。" ), StrmPt->m_Idx );
				}
				StrmPt->m_SpeexDecd.m_Pt = NULL;
			}
			break;
		}
		case 2: //如果要使用Opus编码器。
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出流索引 %z32d：销毁Opus编码器成功。" ), StrmPt->m_Idx );
			break;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptAddStrm
 * 功能说明：添加音频输出的流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptAddStrm( AdoOtpt * AdoOtptPt, int32_t StrmIdx )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			goto Out;
		}
	}

	//添加到流容器。
	if( AdoOtptPt->m_StrmCntnr.PutTail( NULL, ( void * * )&p_StrmPt, 0, NULL ) != 0 )
	{
		goto Out;
	}
	p_StrmPt->m_Idx = StrmIdx;
	p_StrmPt->m_IsUse = 0;
	p_StrmPt->m_UseWhatDecd = 0;

	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptDelStrm
 * 功能说明：删除音频输出的流。
 * 参数说明：MediaPocsThrdPt：[输入]，存放媒体处理线程的指针，不能为NULL。
             StrmIdx：[输入]，存放流索引。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptDelStrm( AdoOtpt * AdoOtptPt, int32_t StrmIdx )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			//从流容器删除。
			AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );
			AdoOtptPt->m_StrmCntnr.DelByNum( p_StrmNum, 0, NULL );
			goto Out;
		}
	}

	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetStrmUsePcm
 * 功能说明：设置音频输出的流要使用PCM原始数据。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetStrmUsePcm( AdoOtpt * AdoOtptPt, int32_t StrmIdx )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );

			p_StrmPt->m_UseWhatDecd = 0;

			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmInit( AdoOtptPt, p_StrmPt );
			goto Out;
		}
	}

	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetStrmUseSpeexDecd
 * 功能说明：设置音频输出的流要使用Speex解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
             IsUsePrcplEnhsmt：[输出]，存放Speex解码器是否使用知觉增强，为非0表示要使用，为0表示不使用。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetStrmUseSpeexDecd( AdoOtpt * AdoOtptPt, int32_t StrmIdx, int32_t IsUsePrcplEnhsmt )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );

			p_StrmPt->m_UseWhatDecd = 1;
			p_StrmPt->m_SpeexDecd.m_IsUsePrcplEnhsmt = IsUsePrcplEnhsmt;

			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmInit( AdoOtptPt, p_StrmPt );
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetStrmUseOpusDecd
 * 功能说明：设置音频输出的流要使用Opus解码器。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetStrmUseOpusDecd( AdoOtpt * AdoOtptPt, int32_t StrmIdx )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );

			p_StrmPt->m_UseWhatDecd = 2;

			if( ( AdoOtptPt->m_IsInit != 0 ) && ( p_StrmPt->m_IsUse != 0 ) ) AdoOtptStrmInit( AdoOtptPt, p_StrmPt );
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptSetStrmIsUse
 * 功能说明：设置音频输出的流是否使用。
 * 参数说明：MediaPocsThrdPt：[输出]，存放媒体处理线程的指针，不能为NULL。
			 ErrInfoVstrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
			 非0：失败。
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptSetStrmIsUse( AdoOtpt * AdoOtptPt, int32_t StrmIdx, int32_t IsUseStrm )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。
	
	AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

	//查找流索引。
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == StrmIdx ) //如果流索引找到了。
		{
			if( IsUseStrm != 0 ) //如果要使用流。
			{
				if( p_StrmPt->m_IsUse == 0 ) //如果当前不使用流。
				{
					if( AdoOtptPt->m_IsInit != 0 ) //如果已初始化音频输出。
					{
						if( AdoOtptStrmInit( AdoOtptPt, p_StrmPt ) == 0 ) //如果初始化音频输出的流成功。
						{
							p_StrmPt->m_IsUse = 1;
							AdoOtptPt->m_StrmUseTotal++;
						}
					}
					else //如果未初始化音频输出。
					{
						p_StrmPt->m_IsUse = 1;
						AdoOtptPt->m_StrmUseTotal++;
					}
				}
			}
			else //如果不使用流。
			{
				if( p_StrmPt->m_IsUse != 0 ) //如果当前要使用流。
				{
					if( AdoOtptPt->m_IsInit != 0 ) //如果已初始化音频输出。
					{
						AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );
						p_StrmPt->m_IsUse = 0;
						AdoOtptPt->m_StrmUseTotal--;
					}
					else //如果未初始化音频输出。
					{
						p_StrmPt->m_IsUse = 0;
						AdoOtptPt->m_StrmUseTotal--;
					}
				}
			}
			goto Out;
		}
	}
	
	Out:
	AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptStrmCntnrInit
 * 功能说明：初始化音频输出的流容器。
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

int AdoOtptStrmCntnrInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。

	AdoOtptPt->m_StrmUseTotal = 0;
	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_IsUse != 0 )
		{
			if( AdoOtptStrmInit( AdoOtptPt, p_StrmPt ) != 0 ) goto Out;
			AdoOtptPt->m_StrmUseTotal++;
		}
	}
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：初始化流容器成功。" ) );

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	//if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptStrmCntnrDstoy
 * 功能说明：销毁音频输出的流容器。
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

void AdoOtptStrmCntnrDstoy( AdoOtpt * AdoOtptPt )
{
	AdoOtpt::Strm * p_StrmPt; //存放流的指针。

	for( size_t p_StrmNum = 0; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_IsUse != 0 )
		{
			AdoOtptStrmDstoy( AdoOtptPt, p_StrmPt );
		}
	}
	AdoOtptPt->m_StrmUseTotal = 0;
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁流容器成功。" ) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptWavfmInit
 * 功能说明：初始化音频输出的波形器。
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

int AdoOtptWavfmInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoOtptPt->m_Wavfm.m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoWavfmInit( &AdoOtptPt->m_Wavfm.m_SrcWavfmPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化原始波形器成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化原始波形器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 功能说明：销毁音频输出的波形器。
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

void AdoOtptWavfmDstoy( AdoOtpt * AdoOtptPt )
{
	if( AdoOtptPt->m_Wavfm.m_IsDrawAdoWavfmToWnd != 0 )
	{
		if( AdoOtptPt->m_Wavfm.m_SrcWavfmPt != NULL )
		{
			if( AdoWavfmDstoy( AdoOtptPt->m_Wavfm.m_SrcWavfmPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁原始波形器成功。" ) );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：销毁原始波形器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			AdoOtptPt->m_Wavfm.m_SrcWavfmPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptWaveFileWriterInit
 * 功能说明：初始化音频输出的Wave文件写入器。
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

int AdoOtptWaveFileWriterInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( AdoOtptPt->m_WaveFile.m_IsSaveAdoToWaveFile != 0 )
	{
		if( WaveFileWriterInit( &AdoOtptPt->m_WaveFile.m_SrcWaveFileWriterPt, AdoOtptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt, AdoOtptPt->m_WaveFile.m_WaveFileWrBufSzByt, 1, AdoOtptPt->m_SmplRate, 16 ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化原始Wave文件 %vs 的Wave文件写入器成功。" ), AdoOtptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化原始Wave文件 %vs 的Wave文件写入器失败。" ), AdoOtptPt->m_WaveFile.m_SrcWaveFileFullPathVstrPt );
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
 * 功能说明：销毁音频输出的Wave文件写入器。
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

void AdoOtptWaveFileWriterDstoy( AdoOtpt * AdoOtptPt )
{
	if( AdoOtptPt->m_WaveFile.m_IsSaveAdoToWaveFile != 0 )
	{
		if( AdoOtptPt->m_WaveFile.m_SrcWaveFileWriterPt != NULL )
		{
			if( WaveFileWriterDstoy( AdoOtptPt->m_WaveFile.m_SrcWaveFileWriterPt ) == 0 )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁原始Wave文件写入器成功。" ) );
			}
			else
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：销毁原始Wave文件写入器失败。" ) );
			}
			AdoOtptPt->m_WaveFile.m_SrcWaveFileWriterPt = NULL;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptDvcAndThrdInit
 * 功能说明：初始化音频输出的设备和线程。
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

int AdoOtptDvcAndThrdInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//初始化设备。
	{
		HRESULT p_HRslt;

		p_HRslt = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, ( void * * )&AdoOtptPt->m_Dvc.m_EnumPt ); //初始化设备枚举器。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化设备枚举器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		if( AdoOtptPt->m_Dvc.m_ID == 0 ) //如果要使用默认的设备。
		{
			p_HRslt = AdoOtptPt->m_Dvc.m_EnumPt->GetDefaultAudioEndpoint( eRender, eConsole, &AdoOtptPt->m_Dvc.m_Pt ); //初始化设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoOtptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：音频输出：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}
		else //如果要使用指定的设备。
		{
			p_HRslt = AdoOtptPt->m_Dvc.m_EnumPt->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, &AdoOtptPt->m_Dvc.m_ClctPt ); //获取设备收集器。只收集激活的设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：获取设备收集器失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}

			p_HRslt = AdoOtptPt->m_Dvc.m_ClctPt->Item( AdoOtptPt->m_Dvc.m_ID - 1, &AdoOtptPt->m_Dvc.m_Pt ); //初始化设备。
			if( p_HRslt != S_OK )
			{
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) ToastFmt( AdoOtptPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, Cu8vstr( "媒体处理线程：音频输出：初始化设备失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
				goto Out;
			}
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_Pt->Activate( IID_IAudioClient, CLSCTX_INPROC_SERVER, NULL, ( void * * )&AdoOtptPt->m_Dvc.m_ClntPt ); //激活设备的客户端接口。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：激活设备的客户端接口失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_ClntPt->GetMixFormat( &AdoOtptPt->m_Dvc.m_WaveFmtExPt ); //获取设备的格式。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：获取设备的格式失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		//修改格式为16位整型，不能修改采样频率和声道数。
		if( AdoOtptPt->m_Dvc.m_WaveFmtExPt->wFormatTag != WAVE_FORMAT_EXTENSIBLE )
		{
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->wFormatTag = WAVE_FORMAT_PCM;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample = 16;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign = AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels * AdoOtptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample / 8;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->nAvgBytesPerSec = AdoOtptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign * AdoOtptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
		}
		else
		{
			( ( WAVEFORMATEXTENSIBLE * )AdoOtptPt->m_Dvc.m_WaveFmtExPt )->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			( ( WAVEFORMATEXTENSIBLE * )AdoOtptPt->m_Dvc.m_WaveFmtExPt )->Samples.wValidBitsPerSample = 16;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample = 16;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign = AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels * AdoOtptPt->m_Dvc.m_WaveFmtExPt->wBitsPerSample / 8;
			AdoOtptPt->m_Dvc.m_WaveFmtExPt->nAvgBytesPerSec = AdoOtptPt->m_Dvc.m_WaveFmtExPt->nBlockAlign * AdoOtptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec;
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_ClntPt->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, AdoOtptPt->m_FrmLenMsec * 10 * 1000, 0, AdoOtptPt->m_Dvc.m_WaveFmtExPt, NULL ); //初始化设备客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化设备客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_ClntPt->GetBufferSize( &AdoOtptPt->m_Dvc.m_BufSzUnit ); //获取设备的缓冲区大小。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：获取设备的缓冲区大小失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_ClntPt->GetService( IID_IAudioRenderClient, ( void * * )&AdoOtptPt->m_Dvc.m_RndrClntPt ); //获取设备的渲染客户端。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：获取设备的渲染客户端失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&AdoOtptPt->m_Dvc.m_GlblIntfcTablePt ); //初始化设备全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化设备的全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		p_HRslt = AdoOtptPt->m_Dvc.m_GlblIntfcTablePt->RegisterInterfaceInGlobal( AdoOtptPt->m_Dvc.m_RndrClntPt, IID_IAudioRenderClient, &AdoOtptPt->m_Dvc.m_RndrClntCookie ); //注册设备的渲染客户端到全局接口表。
		if( p_HRslt != S_OK )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：注册设备的渲染客户端到全局接口表失败。原因：%vs" ), GetWinSysErrInfo( p_HRslt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) );
			goto Out;
		}

		AdoOtptPt->m_Dvc.m_PcmBufFrmPt = NULL; //设置Pcm格式缓冲区帧的指针。
		AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit = AdoOtptPt->m_FrmLenMsec * AdoOtptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec / 1000; //设置Pcm格式缓冲区帧的长度。
		AdoOtptPt->m_Dvc.m_PcmBufFrmLenByt = AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit * sizeof( int16_t ); //设置Pcm格式缓冲区帧的长度。

		if( SpeexResamplerInit( &AdoOtptPt->m_Dvc.m_PcmBufFrmSpeexResamplerPt, AdoOtptPt->m_SmplRate, AdoOtptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec, 3, NULL ) != 0 )
		{
			LOGE( Cu8vstr( "媒体处理线程：音频输出：初始化Pcm格式缓冲区帧Speex重采样器失败。" ) );
			goto Out;
		}

		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化设备成功。采样频率：%z32d, 声道数：%z16d，缓冲区大小：%uz32d。" ), AdoOtptPt->m_Dvc.m_WaveFmtExPt->nSamplesPerSec, AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels, AdoOtptPt->m_Dvc.m_BufSzUnit );
	}

	//初始化Pcm格式原始帧容器。
	if( AdoOtptPt->m_PcmSrcFrmCntnr.Init( sizeof( int16_t * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化Pcm格式原始帧容器成功。" ) );
	}
	else
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化Pcm格式原始帧容器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	
	//初始化Pcm格式空闲帧容器。
	if( AdoOtptPt->m_PcmIdleFrmCntnr.Init( sizeof( int16_t * ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化Pcm格式空闲帧容器成功。" ) );
	}
	else
	{
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化Pcm格式空闲帧容器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//初始化线程的临时变量。
	{
		AdoOtptPt->m_Thrd.m_IsInitThrdTmpVar = 1; //设置已初始化线程的临时变量。
		AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = NULL; //初始化Pcm格式原始帧的指针。
		AdoOtptPt->m_Thrd.m_EncdSrcFrmPt = ( uint8_t * )malloc( AdoOtptPt->m_FrmLenByt ); //初始化已编码格式原始帧的指针。
		if( AdoOtptPt->m_Thrd.m_EncdSrcFrmPt == NULL )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：创建已编码格式原始帧的内存块失败。" ) );
			goto Out;
		}
		AdoOtptPt->m_Thrd.m_EncdSrcFrmSzByt = AdoOtptPt->m_FrmLenByt; //初始化已编码格式原始帧的大小，单位为字节。
		AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt = 0; //初始化已编码格式原始帧的长度，单位为字节。
		AdoOtptPt->m_Thrd.m_PcmMixFrmPt = ( int32_t * )malloc( AdoOtptPt->m_FrmLenByt ); //初始化Pcm格式混音帧的指针。
		if( AdoOtptPt->m_Thrd.m_PcmMixFrmPt == NULL )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：创建Pcm格式混音帧的内存块失败。" ) );
			goto Out;
		}
		AdoOtptPt->m_Thrd.m_ElmTotal = 0; //初始化元素总数。
		AdoOtptPt->m_Thrd.m_LastTickMsec = 0; //初始化上次的嘀嗒钟。
		AdoOtptPt->m_Thrd.m_NowTickMsec = 0; //初始化本次的嘀嗒钟。
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：初始化线程的临时变量成功。" ) );
	}

	//初始化线程。
	{
		AdoOtptPt->m_Thrd.m_ThrdIsStart = 0; //设置线程为未开始。
		AdoOtptPt->m_Thrd.m_ExitFlag = 0; //设置退出标记为0表示保持运行。
		if( ThrdInit( &AdoOtptPt->m_Thrd.m_ThrdInfoPt, ( LPTHREAD_START_ROUTINE )AdoOtptThrdRun, AdoOtptPt, AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化线程成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：初始化线程失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
 * 功能说明：销毁音频输出的设备和线程。
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

void AdoOtptDvcAndThrdDstoy( AdoOtpt * AdoOtptPt )
{
	//销毁线程。
	if( AdoOtptPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		AdoOtptPt->m_Thrd.m_ExitFlag = 1; //请求线程退出。
		ThrdWaitDstoy( AdoOtptPt->m_Thrd.m_ThrdInfoPt, NULL ); //等待线程退出并销毁。
		AdoOtptPt->m_Thrd.m_ThrdInfoPt = NULL;
		AdoOtptPt->m_Thrd.m_ThrdIsStart = 0;
		AdoOtptPt->m_Thrd.m_ExitFlag = 0;
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁线程成功。" ) );
	}

	//销毁线程的临时变量。
	if( AdoOtptPt->m_Thrd.m_IsInitThrdTmpVar != 0 )
	{
		AdoOtptPt->m_Thrd.m_IsInitThrdTmpVar = 0; //设置未初始化线程的临时变量。
		AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = NULL; //销毁Pcm格式原始帧的指针。
		if( AdoOtptPt->m_Thrd.m_EncdSrcFrmPt != NULL ) //销毁已编码格式原始帧的指针。
		{
			free( AdoOtptPt->m_Thrd.m_EncdSrcFrmPt );
			AdoOtptPt->m_Thrd.m_EncdSrcFrmPt = NULL;
		}
		AdoOtptPt->m_Thrd.m_EncdSrcFrmSzByt = 0; //销毁已编码格式原始帧的大小，单位为字节。
		AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt = 0; //销毁已编码格式原始帧的长度，单位为字节。
		if( AdoOtptPt->m_Thrd.m_PcmMixFrmPt != NULL ) //销毁Pcm格式混音帧的指针。
		{
			free( AdoOtptPt->m_Thrd.m_PcmMixFrmPt );
			AdoOtptPt->m_Thrd.m_PcmMixFrmPt = NULL;
		}
		AdoOtptPt->m_Thrd.m_ElmTotal = 0; //销毁元素总数。
		AdoOtptPt->m_Thrd.m_LastTickMsec = 0; //销毁上次的嘀嗒钟。
		AdoOtptPt->m_Thrd.m_NowTickMsec = 0; //销毁本次的嘀嗒钟。
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁线程的临时变量成功。" ) );
	}

	//销毁Pcm格式空闲帧容器。
	if( AdoOtptPt->m_PcmIdleFrmCntnr.m_CQueuePt != NULL )
	{
		while( AdoOtptPt->m_PcmIdleFrmCntnr.GetHead( &AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			free( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt );
			AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
		}
		if( AdoOtptPt->m_PcmIdleFrmCntnr.Dstoy( AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁Pcm格式空闲帧容器成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：销毁Pcm格式空闲帧容器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	//销毁Pcm格式原始帧容器。
	if( AdoOtptPt->m_PcmSrcFrmCntnr.m_CQueuePt != NULL )
	{
		while( AdoOtptPt->m_PcmSrcFrmCntnr.GetHead( &AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 0, NULL ) == 0 )
		{
			free( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt );
			AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
		}
		if( AdoOtptPt->m_PcmSrcFrmCntnr.Dstoy( AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁Pcm格式原始帧容器成功。" ) );
		}
		else
		{
			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "媒体处理线程：音频输出：销毁Pcm格式原始帧容器失败。原因：%vs" ), AdoOtptPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//销毁设备。
	{
		if( AdoOtptPt->m_Dvc.m_PcmBufFrmSpeexResamplerPt != NULL ) //销毁Pcm格式设备缓冲区帧Speex重采样器。
		{
			SpeexResamplerDstoy( AdoOtptPt->m_Dvc.m_PcmBufFrmSpeexResamplerPt );
			AdoOtptPt->m_Dvc.m_PcmBufFrmSpeexResamplerPt = NULL;
		}
		AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit = 0;
		AdoOtptPt->m_Dvc.m_PcmBufFrmLenByt = 0;
		AdoOtptPt->m_Dvc.m_PcmBufFrmPt = NULL;
		if( AdoOtptPt->m_Dvc.m_RndrClntCookie != 0 ) //销毁设备渲染客户端的Cookie。
		{
			AdoOtptPt->m_Dvc.m_GlblIntfcTablePt->RevokeInterfaceFromGlobal( AdoOtptPt->m_Dvc.m_RndrClntCookie );
			AdoOtptPt->m_Dvc.m_RndrClntCookie = 0;
		}
		if( AdoOtptPt->m_Dvc.m_GlblIntfcTablePt != NULL ) //销毁设备的全局接口表。
		{
			AdoOtptPt->m_Dvc.m_GlblIntfcTablePt->Release();
			AdoOtptPt->m_Dvc.m_GlblIntfcTablePt = NULL;
		}
		if( AdoOtptPt->m_Dvc.m_RndrClntPt != NULL ) //销毁设备的渲染客户端。
		{
			AdoOtptPt->m_Dvc.m_RndrClntPt->Release();
			AdoOtptPt->m_Dvc.m_RndrClntPt = NULL;
		}
		AdoOtptPt->m_Dvc.m_BufSzUnit = 0; //销毁设备的缓冲区大小。
		if( AdoOtptPt->m_Dvc.m_WaveFmtExPt != NULL ) //销毁设备的格式。
		{
			CoTaskMemFree( AdoOtptPt->m_Dvc.m_WaveFmtExPt );
			AdoOtptPt->m_Dvc.m_WaveFmtExPt = NULL;
		}
		if( AdoOtptPt->m_Dvc.m_ClntPt != NULL ) //销毁设备的客户端。
		{
			AdoOtptPt->m_Dvc.m_ClntPt->Stop();
			AdoOtptPt->m_Dvc.m_ClntPt->Release();
			AdoOtptPt->m_Dvc.m_ClntPt = NULL;
		}
		if( AdoOtptPt->m_Dvc.m_Pt != NULL ) //销毁设备。
		{
			AdoOtptPt->m_Dvc.m_Pt->Release();
			AdoOtptPt->m_Dvc.m_Pt = NULL;
		}
		if( AdoOtptPt->m_Dvc.m_ClctPt != NULL ) //销毁设备收集器。
		{
			AdoOtptPt->m_Dvc.m_ClctPt->Release();
			AdoOtptPt->m_Dvc.m_ClctPt = NULL;
		}
		if( AdoOtptPt->m_Dvc.m_EnumPt != NULL ) //销毁设备枚举器。
		{
			AdoOtptPt->m_Dvc.m_EnumPt->Release();
			AdoOtptPt->m_Dvc.m_EnumPt = NULL;
		}
		AdoOtptPt->m_Dvc.m_IsClos = 0;
		if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "媒体处理线程：音频输出：销毁设备成功。" ) );
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 函数名称：AdoOtptInit
 * 功能说明：初始化音频输出。
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

int AdoOtptInit( AdoOtpt * AdoOtptPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录初始化开始的嘀嗒钟。

	if( AdoOtptStrmCntnrInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptWavfmInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptWaveFileWriterInit( AdoOtptPt ) != 0 ) goto Out;
	if( AdoOtptDvcAndThrdInit( AdoOtptPt ) != 0 ) goto Out;

	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录初始化结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：音频输出：初始化耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
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
             ErrInfoVarStrPt：[输出]，存放错误信息动态字符串的指针，可以为NULL。
 * 返回说明：0：成功。
             非0：失败。
             ……
 * 线程安全：是 或 否
 * 调用样例：填写调用此函数的样例，并解释函数参数和返回值。
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void AdoOtptDstoy( AdoOtpt * AdoOtptPt )
{
	uint64_t p_LastTickMsec = 0;
	uint64_t p_NowTickMsec = 0;
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec(); //记录销毁开始的嘀嗒钟。

	AdoOtptDvcAndThrdDstoy( AdoOtptPt );
	AdoOtptWaveFileWriterDstoy( AdoOtptPt );
	AdoOtptWavfmDstoy( AdoOtptPt );
	AdoOtptStrmCntnrDstoy( AdoOtptPt );
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
	{
		p_NowTickMsec = FuncGetTickAsMsec(); //记录销毁结束的嘀嗒钟。
		LOGFI( Cu8vstr( "媒体处理线程：音频输出：销毁耗时 %uz64d 毫秒。" ), p_NowTickMsec - p_LastTickMsec );
	}
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
	IAudioRenderClient * p_DvcRndrClntPt; //存放设备渲染客户端的指针。
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

	//获取设备渲染客户端。
	{
		IGlobalInterfaceTable * p_DvcGlblIntfcTablePt;
		CoCreateInstance( CLSID_StdGlobalInterfaceTable, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalInterfaceTable, ( void * * )&p_DvcGlblIntfcTablePt );
		p_DvcGlblIntfcTablePt->GetInterfaceFromGlobal( AdoOtptPt->m_Dvc.m_RndrClntCookie, IID_IAudioRenderClient, ( void * * )&p_DvcRndrClntPt );
		p_DvcGlblIntfcTablePt->Release();
	}
	
	while( AdoOtptPt->m_Thrd.m_ThrdIsStart == 0 ) Sleep( 1 ); //等待线程开始。

	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：开始准备音频输出。" ) );

	//音频输出循环开始。
	while( 1 )
	{
		{
			if( AdoOtptPt->m_Dvc.m_PcmBufFrmPt == NULL ) //如果还没有获取设备的Pcm格式缓冲区。
			{
				p_HRslt = p_DvcRndrClntPt->GetBuffer( AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit, ( BYTE * * )&AdoOtptPt->m_Dvc.m_PcmBufFrmPt ); //获取设备的Pcm格式缓冲区。
				if( p_HRslt == S_OK ) //如果获取设备的Pcm格式缓冲区成功。
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit：%uzd。" ), AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit );
				}
				else //如果获取设备的Pcm格式缓冲区失败。
				{
					if( p_HRslt == AUDCLNT_E_DEVICE_INVALIDATED ) //如果设备已经关闭。
					{
						if( AdoOtptPt->m_Dvc.m_IsClos == 0 )
						{
							AdoOtptPt->m_Dvc.m_IsClos = 1; //设置设备已经关闭。
							if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到设备关闭消息。" ) );
						}
					}
					else if( p_HRslt == AUDCLNT_E_BUFFER_ERROR )
					{
						if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_ERROR。" ) );
					}
					else if( p_HRslt == AUDCLNT_E_BUFFER_TOO_LARGE ) //如果获取设备的Pcm格式缓冲区还不够一帧。
					{
						//if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：AUDCLNT_E_BUFFER_TOO_LARGE。" ) );
						Sleep( 1 ); //暂停一下，避免CPU使用率过高。
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
					goto OutPocs;
				}
			}

			//获取一个Pcm格式空闲帧。
			AdoOtptPt->m_PcmIdleFrmCntnr.GetTotal( &AdoOtptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取Pcm格式空闲帧容器的元素总数。
			if( AdoOtptPt->m_Thrd.m_ElmTotal > 0 ) //如果Pcm格式空闲帧容器中有帧。
			{
				AdoOtptPt->m_PcmIdleFrmCntnr.GetHead( &AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, 1, NULL ); //从Pcm格式空闲帧容器中取出并删除第一个帧。
				if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：从Pcm格式空闲帧容器中取出并删除第一个帧，Pcm格式空闲帧容器元素总数：%uzd。" ), AdoOtptPt->m_Thrd.m_ElmTotal );
			}
			else //如果Pcm格式空闲帧容器中没有帧。
			{
				AdoOtptPt->m_PcmSrcFrmCntnr.GetTotal( &AdoOtptPt->m_Thrd.m_ElmTotal, 1, NULL ); //获取Pcm格式原始帧容器的元素总数。
				if( AdoOtptPt->m_Thrd.m_ElmTotal <= 50 )
				{
					AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = ( int16_t * )calloc( AdoOtptPt->m_FrmLenByt, 1 ); //创建一个Pcm格式空闲帧。
					if( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt != NULL )
					{
						if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：Pcm格式空闲帧容器中没有帧，创建一个Pcm格式空闲帧成功。" ) );
					}
					else
					{
						if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "音频输出线程：Pcm格式空闲帧容器中没有帧，创建一个Pcm格式空闲帧失败。原因：内存不足。" ) );
						Sleep( 1 ); //暂停一下，避免CPU使用率过高。
						goto OutPocs;
					}
				}
				else
				{
					if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：Pcm格式原始帧容器中帧总数为%uzd已经超过上限50，不再创建Pcm格式空闲帧。" ), AdoOtptPt->m_Thrd.m_ElmTotal );
					Sleep( 1 ); //暂停一下，避免CPU使用率过高。
					goto OutPocs;
				}
			}

			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) AdoOtptPt->m_Thrd.m_LastTickMsec = FuncGetTickAsMsec();

			//调用用户定义的写入音频输出帧函数，并解码成PCM原始数据，最后混音。
			{
				AdoOtptPt->m_StrmCntnr.Locked( NULL ); //流容器的互斥锁加锁。

				if( AdoOtptPt->m_StrmUseTotal > 0 ) //如果有流要使用。
				{
					size_t p_StrmNum = 0;
					AdoOtpt::Strm * p_StrmPt; //存放流的指针。

					for( ; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ ) //查找第一条要使用的流。
					{
						if( p_StrmPt->m_IsUse != 0 ) //如果该流为要使用。
						{
							switch( p_StrmPt->m_UseWhatDecd ) //使用什么解码器。
							{
								case 0: //如果要使用PCM原始数据。
								{
									//调用用户定义的写入音频输出帧函数。
									AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																							   AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																							   NULL, 0, NULL );

									//调用用户定义的获取音频输出帧函数。
									AdoOtptPt->m_MediaPocsThrdPt->m_UserGetAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																							 AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																							 NULL, 0 );
									break;
								}
								case 1: //如果要使用Speex编解码器。
								{
									//调用用户定义的写入音频输出帧函数。
									AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																							   NULL, 0,
																							   AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmSzByt, &AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt );

									//使用Speex解码器。
									if( SpeexDecdPocs( p_StrmPt->m_SpeexDecd.m_Pt, AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt, AdoOtptPt->m_Thrd.m_PcmSrcFrmPt ) == 0 )
									{
										if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器成功。" ), p_StrmPt->m_Idx );
									}
									else
									{
										if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器失败。" ), p_StrmPt->m_Idx );
									}

									//调用用户定义的获取音频输出帧函数。
									AdoOtptPt->m_MediaPocsThrdPt->m_UserGetAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																							 AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																							 AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt );
									break;
								}
								case 2: //如果要使用Opus编解码器。
								{
									if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), p_StrmPt->m_Idx );
								}
							}

							break;
						}
					}

					if( AdoOtptPt->m_StrmUseTotal > 1 ) //如果有超过1条流要使用。
					{
						for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLenUnit; p_TmpInt++ ) //将第一个流的Pcm格式原始帧复制到Pcm格式混音帧。
						{
							AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] = AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ];
						}

						for( ; AdoOtptPt->m_StrmCntnr.GetByNum( p_StrmNum, NULL, ( void * * )&p_StrmPt, 0, 0, NULL ) == 0; p_StrmNum++ ) //查找其他要使用的流。
						{
							if( p_StrmPt->m_IsUse != 0 ) //如果该流为要使用。
							{
								switch( p_StrmPt->m_UseWhatDecd ) //使用什么解码器。
								{
									case 0: //如果要使用PCM原始数据。
									{
										//调用用户定义的写入音频输出帧函数。
										AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																								   AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																								   NULL, 0, NULL );

										//调用用户定义的获取音频输出帧函数。
										AdoOtptPt->m_MediaPocsThrdPt->m_UserGetAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																								 AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																								 NULL, 0 );
										break;
									}
									case 1: //如果要使用Speex编解码器。
									{
										//调用用户定义的写入音频输出帧函数。
										AdoOtptPt->m_MediaPocsThrdPt->m_UserWriteAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																								   NULL, 0,
																								   AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmSzByt, &AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt );

										//使用Speex解码器。
										if( SpeexDecdPocs( p_StrmPt->m_SpeexDecd.m_Pt, AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt, AdoOtptPt->m_Thrd.m_PcmSrcFrmPt ) == 0 )
										{
											if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器成功。" ), p_StrmPt->m_Idx );
										}
										else
										{
											if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：使用Speex解码器失败。" ), p_StrmPt->m_Idx );
										}

										//调用用户定义的获取音频输出帧函数。
										AdoOtptPt->m_MediaPocsThrdPt->m_UserGetAdoOtptFrmFuncPt( AdoOtptPt->m_MediaPocsThrdPt, p_StrmPt->m_Idx,
																								 AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit,
																								 AdoOtptPt->m_Thrd.m_EncdSrcFrmPt, AdoOtptPt->m_Thrd.m_EncdSrcFrmLenByt );
										break;
									}
									case 2: //如果要使用Opus编解码器。
									{
										if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出线程：音频输出流索引 %z32d：暂不支持使用Opus解码器。" ), p_StrmPt->m_Idx );
									}
								}

								for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLenUnit; p_TmpInt++ ) //混音。
								{
									AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] = AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ] + AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] - ( ( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ] * AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] ) >> 0x10 );
								}
							}
						}

						for( int p_TmpInt = 0; p_TmpInt < AdoOtptPt->m_FrmLenUnit; p_TmpInt++ ) //将Pcm格式混音帧复制到Pcm格式原始帧。
						{
							if( AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] > 32767 ) AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ] = 32767;
							else if( AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ] < -32768 ) AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ] = -32768;
							else AdoOtptPt->m_Thrd.m_PcmSrcFrmPt[ p_TmpInt ] = ( int16_t )AdoOtptPt->m_Thrd.m_PcmMixFrmPt[ p_TmpInt ];
						}
					}
				}

				AdoOtptPt->m_StrmCntnr.Unlock( NULL ); //流容器的互斥锁解锁。
			}

			//判断设备是否静音。在音频处理完后再设置静音，这样可以保证音频处理器的连续性。
			if( AdoOtptPt->m_Dvc.m_IsMute != 0 )
			{
				memset( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, 0, AdoOtptPt->m_FrmLenByt );
			}

			//将Pcm格式原始帧转换为Pcm格式缓冲区帧。
			{
				//fwrite( AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenByt, 1, AdoOtptFile1Pt );
				SpeexResamplerPocs( AdoOtptPt->m_Dvc.m_PcmBufFrmSpeexResamplerPt, AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, AdoOtptPt->m_FrmLenUnit, AdoOtptPt->m_Dvc.m_PcmBufFrmPt, AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit, NULL );
				//fwrite( AdoOtptPt->m_Dvc.m_PcmBufFrmPt, AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit * sizeof( int16_t ), 1, AdoOtptFile2Pt );
				if( AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels > 1 ) //如果Pcm格式缓冲区帧为多声道。
				{
					for( int i = AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit - 1; i >= 0; i-- ) //单声道复制为多声道。
						for( int j = AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels - 1; j >= 0; j-- )
							AdoOtptPt->m_Dvc.m_PcmBufFrmPt[ i * AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels + j ] = AdoOtptPt->m_Dvc.m_PcmBufFrmPt[ i ];
					//fwrite( AdoOtptPt->m_Dvc.m_PcmBufFrmPt, AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit * AdoOtptPt->m_Dvc.m_WaveFmtExPt->nChannels * sizeof( int16_t ), 1, AdoOtptFile3Pt );
				}
			}

			p_DvcRndrClntPt->ReleaseBuffer( AdoOtptPt->m_Dvc.m_PcmBufFrmLenUnit, 0 ); //释放设备的Pcm格式缓冲区。
			AdoOtptPt->m_Dvc.m_PcmBufFrmPt = NULL;

			//放入本次Pcm格式原始帧到Pcm格式原始帧容器。注意：从取出到放入过程中不能跳出，否则会内存泄露。
			{
				AdoOtptPt->m_PcmSrcFrmCntnr.PutTail( &AdoOtptPt->m_Thrd.m_PcmSrcFrmPt, NULL, 1, NULL );
				AdoOtptPt->m_Thrd.m_PcmSrcFrmPt = NULL;
			}

			if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 )
			{
				AdoOtptPt->m_Thrd.m_NowTickMsec = FuncGetTickAsMsec();
				LOGFI( Cu8vstr( "音频输出线程：本次帧处理完毕，耗时 %uz64d 毫秒。" ), AdoOtptPt->m_Thrd.m_NowTickMsec - AdoOtptPt->m_Thrd.m_LastTickMsec );
				AdoOtptPt->m_Thrd.m_LastTickMsec = AdoOtptPt->m_Thrd.m_NowTickMsec;
			}
		}
		OutPocs:;

		if( AdoOtptPt->m_Thrd.m_ExitFlag == 1 ) //如果退出标记为请求退出。
        {
            if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "音频输出线程：本线程接收到退出请求，开始准备退出。" ) );
            break;
        }
	} //音频输出循环结束。

	CoUninitialize(); //销毁COM库。
	
	if( AdoOtptPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频输出线程：本线程已退出。" ) );
	return 0;
}
