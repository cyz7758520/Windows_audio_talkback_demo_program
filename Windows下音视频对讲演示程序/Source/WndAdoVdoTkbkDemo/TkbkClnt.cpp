#include "TkbkClnt.h"
#include "ClntMediaPocsThrd.h"

//连接信息初始化。
int TkbkClntCnctInfoInit( TkbkClnt * TkbkClntPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
	TkbkClntPt->m_CnctIsInit = 1; //设置连接已初始化。
	TkbkClntPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl; //设置协议为Tcp协议或高级Udp协议。
	if( VstrInit( &TkbkClntPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt ) != 0 ) //设置远端套接字绑定的远端节点名称动态字符串的指针。
	{
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &TkbkClntPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt ) != 0 ) //设置远端套接字绑定的远端节点服务动态字符串的指针。
	{
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	TkbkClntPt->m_TcpClntSoktPt = NULL; //设置本端Tcp协议客户端套接字的指针。
	TkbkClntPt->m_AudpClntCnctIdx = SIZE_MAX; //设置本端高级Udp协议客户端连接索引。
	TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsWait; //设置当前连接状态。
	TkbkClntPt->m_IsRqstDstoy = 0; //设置是否请求销毁。

	TkbkClntPt->m_MyTkbkIdx = -1; //设置我的对讲索引。
	TkbkClntPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的音频输入帧为无语音活动。
	TkbkClntPt->m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
	TkbkClntPt->m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
	TkbkClntPt->m_IsRecvExitPkt = 0; //设置是否接收退出包。

	VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化与远端节点%vs[%vs:%vs]的连接成功。" ), ( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
	if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctInitFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt ); //调用用户定义的对讲客户端连接初始化函数。
	TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		TkbkClntCnctInfoDstoy( TkbkClntPt );
	}
	return p_Rslt;
}

//连接信息销毁。
void TkbkClntCnctInfoDstoy( TkbkClnt * TkbkClntPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	if( TkbkClntPt->m_CnctIsInit != 0 )
	{
		//发送退出包。
		if( ( TkbkClntPt->m_IsRecvExitPkt == 0 ) && ( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果未接收退出包，且当前连接状态为已连接。
		{
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypExit; //设置退出包。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( int8_t )TkbkClntPt->m_MyTkbkIdx; //设置退出包。
			if( ( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( TkbkClntPt->m_TcpClntSoktPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送退出包成功。对讲索引：%z32d。总长度：2。" ), TkbkClntPt->m_MyTkbkIdx );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else
			{
				VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送退出包失败。对讲索引：%z32d。总长度：2。原因：" ), TkbkClntPt->m_MyTkbkIdx );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		TkbkClntTkbkInfoAllDstoy( TkbkClntPt ); //对讲信息全部销毁。

		//销毁本端Tcp协议客户端套接字。
		if( TkbkClntPt->m_TcpClntSoktPt != NULL )
		{
			TcpClntDstoy( TkbkClntPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
			TkbkClntPt->m_TcpClntSoktPt = NULL;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁本端Tcp协议客户端套接字成功。" ) );
		}

		//销毁本端高级Udp协议客户端连接。
		if( TkbkClntPt->m_AudpClntCnctIdx != -1 )
		{
			AudpClosCnct( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, NULL );
			TkbkClntPt->m_AudpClntCnctIdx = -1;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁本端高级Udp协议客户端连接成功。" ) );
		}

		if( ( TkbkClntPt->m_IsRecvExitPkt == 0 ) && ( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsTmot ) ) //如果未接收退出包，且当前连接状态为异常断开，就重连。
		{
			TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsWait; //设置当前连接状态。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
			TkbkClntPt->m_IsRqstDstoy = 0; //设置未请求销毁。

			TkbkClntPt->m_MyTkbkIdx = -1; //设置我的对讲索引。
			TkbkClntPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的音频输入帧为无语音活动。
			TkbkClntPt->m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
			TkbkClntPt->m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
			TkbkClntPt->m_IsRecvExitPkt = 0; //设置未接收退出包。

			VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：连接异常断开，准备重连。" ) );
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		else //如果已接收退出包，或当前连接状态不为异常断开，就销毁。
		{
			//设置当前连接状态。
			if( TkbkClntPt->m_CurCnctSts <= ClntMediaPocsThrd::CnctStsWait )
			{
				TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsTmot;
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
			}
			else if( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct )
			{
				TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsDsct;
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
			}

			TkbkClntPt->m_CnctIsInit = 0; //设置连接未初始化。

			VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁与远端节点%vs[%vs:%vs]的连接成功。" ), ( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

			TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctDstoyFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt ); //调用用户定义的对讲客户端连接销毁函数。
			VstrDstoy( TkbkClntPt->m_RmtNodeNameVstrPt ); //销毁远端套接字绑定的远端节点名称动态字符串。
			VstrDstoy( TkbkClntPt->m_RmtNodeSrvcVstrPt ); //销毁远端套接字绑定的远端节点服务动态字符串。
		}

		ClntMediaPocsThrdSetTkbkMode( TkbkClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。
	}

	ClntMediaPocsThrdIsAutoRqirExit( TkbkClntPt->m_ClntMediaPocsThrdPt, NULL ); //判断是否自动请求退出。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接发送数据包。
int TkbkClntCnctSendPkt( TkbkClnt * TkbkClntPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( TkbkClntPt->m_CnctIsInit != 0 ) && ( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果连接已初始化，且当前连接状态为已连接。
	{
		//发送数据包。
		if( ( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( TkbkClntPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, ErrInfoVstrPt ) == 0 ) ) ||
			( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ) == 0 ) ) )
		{
			//发送数据包成功。
		}
		else
		{
			goto Out; //发送数据包失败。
		}
	}
	else
	{
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) VstrCpy( ErrInfoVstrPt, Cu8vstr( "连接未初始化，或当前连接状态不为已连接。" ), , );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接发送对讲模式包。
int TkbkClntCnctSendTkbkModePkt( TkbkClnt * TkbkClntPt, int LclTkbkMode )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ( int8_t )ClntMediaPocsThrd::PktTypTkbkMode; //设置对讲模式包。
	TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( int8_t )TkbkClntPt->m_MyTkbkIdx; //设置对讲索引。
	TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )LclTkbkMode; //设置对讲模式。
	if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 3, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送对讲模式包成功。对讲索引：%z32d。对讲模式：%z8s。" ), TkbkClntPt->m_MyTkbkIdx, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
	}
	else
	{
		VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送对讲模式包失败。对讲索引：%z32d。对讲模式：%z8s。原因：" ), TkbkClntPt->m_MyTkbkIdx, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//对讲信息初始化。
TkbkClnt::TkbkInfo * TkbkClntTkbkInfoInit( TkbkClnt * TkbkClntPt, int32_t TkbkIdx )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt = NULL;
	size_t p_ElmTotal; //存放元素总数。

	//添加空对讲信息到对讲信息容器，直到对讲信息容器的元素总数达到对讲索引。
	while( CQueueGetTotal( TkbkClntPt->m_TkbkInfoCntnrPt, &p_ElmTotal, 0, NULL ), TkbkIdx >= p_ElmTotal )
	{
		if( CQueuePutTail( TkbkClntPt->m_TkbkInfoCntnrPt, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：添加到对讲信息容器失败。原因：%vs" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		memset( p_TkbkInfoTmpPt, 0, sizeof( TkbkClnt::TkbkInfo ) );
		p_TkbkInfoTmpPt->m_TkbkIdx = p_ElmTotal;
	}

	if( CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, TkbkIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) != 0 ) goto Out;
	p_TkbkInfoTmpPt->m_IsInit = 1; //设置对讲信息已初始化。
	TkbkClntPt->m_TkbkInfoCurMaxNum++; //递增对讲信息的当前最大序号。
	p_TkbkInfoTmpPt->m_Num = TkbkClntPt->m_TkbkInfoCurMaxNum; //设置序号。
	
	p_TkbkInfoTmpPt->m_RmtTkbkMode = ClntMediaPocsThrd::TkbkModeNone; //设置远端对讲模式。

	//接收输出帧初始化。
	switch( TkbkClntPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果要使用容器。
		{
			//初始化接收音频输出帧容器。
			if( VQueueInit( &p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收音频输出帧容器成功。" ) );
			}
			else
			{
				VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化接收音频输出帧容器失败。原因：" ) );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			//初始化接收视频输出帧容器。
			if( VQueueInit( &p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收视频输出帧容器成功。" ) );
			}
			else
			{
				VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化接收视频输出帧容器失败。原因：" ) );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
		case 1: //如果要使用自适应抖动缓冲器。
		{
			//初始化音频自适应抖动缓冲器。
			#if IsIcludAjb
			if( AAjbInit( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_LicnCodePt, &p_TkbkInfoTmpPt->m_AAjbPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, 1, 1, 0, TkbkClntPt->m_AAjbParm.m_MinNeedBufFrmCnt, TkbkClntPt->m_AAjbParm.m_MaxNeedBufFrmCnt, TkbkClntPt->m_AAjbParm.m_MaxCntuLostFrmCnt, TkbkClntPt->m_AAjbParm.m_AdaptSensitivity, ( TkbkClntPt->m_XfrMode == 0 ) ? 0 : 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化音频自适应抖动缓冲器成功。" ) );
			}
			else
			#else
			VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含Ajb。" ), , );
			#endif
			{
				VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化音频自适应抖动缓冲器失败。原因：" ) );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			//初始化视频自适应抖动缓冲器。
			#if IsIcludAjb
			if( VAjbInit( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_LicnCodePt, &p_TkbkInfoTmpPt->m_VAjbPt, 1, TkbkClntPt->m_VAjbParm.m_MinNeedBufFrmCnt, TkbkClntPt->m_VAjbParm.m_MaxNeedBufFrmCnt, TkbkClntPt->m_VAjbParm.m_AdaptSensitivity, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化视频自适应抖动缓冲器成功。" ) );
			}
			else
			#else
			VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含Ajb。" ), , );
			#endif
			{
				VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化视频自适应抖动缓冲器失败。原因：" ) );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
			break;
		}
	}

	//临时数据初始化。
	p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
	if( p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt == NULL )
	{
		VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpByteSz = 1024 * 1024; //设置临时数据大小。
	p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
	if( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt == NULL )
	{
		VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
		if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpByteSz = 1024 * 1024; //设置临时数据大小。

	TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoInitFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, p_TkbkInfoTmpPt ); //调用用户定义的对讲客户端对讲信息初始化函数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_TkbkInfoTmpPt != NULL )
		{
			TkbkClntTkbkInfoDstoy( TkbkClntPt, TkbkIdx );
			p_TkbkInfoTmpPt = NULL;
		}
	}
	return p_TkbkInfoTmpPt;
}

//对讲信息销毁。
void TkbkClntTkbkInfoDstoy( TkbkClnt * TkbkClntPt, int32_t TkbkIdx )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;
	TkbkClnt::TkbkInfo * p_TkbkInfoTmp2Pt;
	
	if( CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, TkbkIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) != 0 ) goto Out;
	if( p_TkbkInfoTmpPt->m_IsInit != 0 )
	{
		TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoDstoyFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, p_TkbkInfoTmpPt ); //调用用户定义的对讲客户端对讲信息销毁函数。
		
		//销毁临时数据。
		if( p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt != NULL )
		{
			free( p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt );
			p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt = NULL;
			p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpByteSz = 0;
		}
		if( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt != NULL )
		{
			free( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt );
			p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt = NULL;
			p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpByteSz = 0;
		}

		//销毁接收音频输出帧容器。
		if( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt != NULL )
		{
			VQueueDstoy( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, NULL );
			p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt = NULL;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁接收音频输出帧容器成功。" ) );
		}

		//销毁接收视频输出帧容器。
		if( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt != NULL )
		{
			VQueueDstoy( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, NULL );
			p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt = NULL;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁接收视频输出帧容器成功。" ) );
		}

		//销毁音频自适应抖动缓冲器。
		if( p_TkbkInfoTmpPt->m_AAjbPt != NULL )
		{
			#if IsIcludAjb
			AAjbDstoy( p_TkbkInfoTmpPt->m_AAjbPt, NULL );
			#endif
			p_TkbkInfoTmpPt->m_AAjbPt = NULL;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁音频自适应抖动缓冲器成功。" ) );
		}

		//销毁视频自适应抖动缓冲器。
		if( p_TkbkInfoTmpPt->m_VAjbPt != NULL )
		{
			#if IsIcludAjb
			VAjbDstoy( p_TkbkInfoTmpPt->m_VAjbPt, NULL );
			#endif
			p_TkbkInfoTmpPt->m_VAjbPt = NULL;
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：销毁视频自适应抖动缓冲器成功。" ) );
		}

		//递减后面对讲信息的序号。
		for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmp2Pt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
		{
			if( ( p_TkbkInfoTmp2Pt->m_IsInit != 0 ) && ( p_TkbkInfoTmp2Pt->m_Num > p_TkbkInfoTmpPt->m_Num ) )
			{
				p_TkbkInfoTmp2Pt->m_Num--; //设置后面对讲信息的序号全部递减1。
			}
		}
		TkbkClntPt->m_TkbkInfoCurMaxNum--; //递减对讲信息的当前最大序号。
		p_TkbkInfoTmpPt->m_IsInit = 0; //设置对讲信息未初始化。

		ClntMediaPocsThrdSetTkbkMode( TkbkClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//对讲信息全部销毁。
void TkbkClntTkbkInfoAllDstoy( TkbkClnt * TkbkClntPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
	{
		if( p_TkbkInfoTmpPt->m_IsInit != 0 )
		{
			TkbkClntTkbkInfoDstoy( TkbkClntPt, p_TkbkInfoTmpPt->m_TkbkIdx );
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接处理，包括连接服务端、接收数据包。
void TkbkClntCnctPocs( TkbkClnt * TkbkClntPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	if( TkbkClntPt->m_CnctIsInit != 0 )
	{
		//用本端客户端套接字连接远端服务端套接字。
		if( TkbkClntPt->m_CurCnctSts <= ClntMediaPocsThrd::CnctStsWait ) //如果当前连接状态为等待远端接受连接。
		{
			int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。

			{ //设置远端节点的地址族。
				ADDRINFOW * p_AddrInfoListPt;
				{ WSADATA p_WsaData; WSAStartup( MAKEWORD( 2, 2 ), &p_WsaData ); } //初始化套接字库。
				if( GetAddrInfo( ( PCWSTR )TkbkClntPt->m_RmtNodeNameVstrPt->m_StrPt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
				{
					p_RmtNodeAddrFamly = ( p_AddrInfoListPt->ai_family == AF_INET ) ? 4 : ( p_AddrInfoListPt->ai_family == AF_INET6 ) ? 6 : 0;
					FreeAddrInfo( p_AddrInfoListPt );
				}
				else p_RmtNodeAddrFamly = 0;
			}

			if( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) //用本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
			{
				int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
				TcpCnctSts p_TcpCnctSts;

				if( TkbkClntPt->m_TcpClntSoktPt == NULL ) //如果未初始化本端Tcp协议客户端套接字。
				{
					if( -TkbkClntPt->m_CurCnctSts >= TkbkClntPt->m_ClntMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
					{
						TkbkClntPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在对讲信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：已达到最大连接次数，中断连接。" ) );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpClntSoktCnctOut;
					}

					TkbkClntPt->m_CurCnctSts--; //递增当前连接次数。
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。

					VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：开始第 %d 次连接。" ), -TkbkClntPt->m_CurCnctSts );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

					if( TcpClntInit( &TkbkClntPt->m_TcpClntSoktPt, p_RmtNodeAddrFamly, TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt, NULL, NULL, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字失败。
					{
						VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpClntSoktCnctOut;
					}
				}

				if( TcpClntWaitCnct( TkbkClntPt->m_TcpClntSoktPt, 0, &p_TcpCnctSts, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字成功。
				{
					if( p_TcpCnctSts == TcpCnctStsWait ) //如果等待远端接受连接。
					{
						//继续等待本端本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
					}
					else if( p_TcpCnctSts == TcpCnctStsCnct ) //如果连接成功。
					{
						if( TcpClntSetNoDelay( TkbkClntPt->m_TcpClntSoktPt, 1, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetSendBufSz( TkbkClntPt->m_TcpClntSoktPt, 1024 * 1024, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetRecvBufSz( TkbkClntPt->m_TcpClntSoktPt, 1024 * 1024 * 3, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetKeepAlive( TkbkClntPt->m_TcpClntSoktPt, 1, 1, 1, 5, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntGetLclAddr( TkbkClntPt->m_TcpClntSoktPt, NULL, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：获取本端Tcp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntGetRmtAddr( TkbkClntPt->m_TcpClntSoktPt, NULL, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：获取本端Tcp协议客户端套接字连接的远端Tcp协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
						TkbkClntPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包，这样可以立即开始发送。
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
						
						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端Tcp协议客户端套接字[%vs:%vs]，并连接远端Tcp协议服务端套接字[%vs:%vs]成功。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
					else //如果连接失败。
					{
						VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpClntSoktCnctOut;
					}
				}
				else //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。
				{
					VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。原因：" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto TcpClntSoktCnctOut;
				}

				p_PocsRslt = 0; //设置本处理段执行成功。
				
				TcpClntSoktCnctOut:
				if( p_PocsRslt != 0 ) //如果本处理段执行失败。
				{
					if( TkbkClntPt->m_TcpClntSoktPt != NULL )
					{
						TcpClntDstoy( TkbkClntPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
						TkbkClntPt->m_TcpClntSoktPt = NULL;
					}
				}
			}
			else //用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字。
			{
				int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
				AudpCnctSts p_AudpCnctSts;

				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt == NULL ) //如果未初始化本端高级Udp协议客户端套接字。
				{
					if( AudpInit( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_LicnCodePt, &TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, NULL, NULL, 0, 5000, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议客户端套接字成功。
					{
						if( AudpGetLclAddr( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, NULL, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
						{
							VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：获取本端高级Udp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}

						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端高级Udp协议客户端套接字[%vs:%vs]成功。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
					else //如果初始化本端高级Udp协议客户端套接字失败。
					{
						VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：初始化本端高级Udp协议客户端套接字失败。原因：" ) );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				if( TkbkClntPt->m_AudpClntCnctIdx == SIZE_MAX )
				{
					if( -TkbkClntPt->m_CurCnctSts >= TkbkClntPt->m_ClntMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
					{
						TkbkClntPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在对讲信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：已达到最大连接次数，中断连接。" ) );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}

					TkbkClntPt->m_CurCnctSts--; //递增当前连接次数。
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。

					VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：开始第 %d 次连接。" ), -TkbkClntPt->m_CurCnctSts );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

					if( AudpCnct( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt, &TkbkClntPt->m_AudpClntCnctIdx, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字失败。
					{
						VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				if( AudpWaitCnct( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, 0, &p_AudpCnctSts, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //循环等待本端高级Udp协议套接字连接远端成功。
				{
					if( p_AudpCnctSts == AudpCnctStsWait ) //如果等待远端接受连接。
					{
						//重新循环，继续等待本端高级Udp协议套接字连接远端。
					}
					else if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
					{
						if( AudpGetRmtAddr( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, NULL, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：获取本端高级Udp协议客户端套接字连接的远端高级Udp协议服务端套接字绑定的远程节点地址和端口失败。原因：" ) );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}

						TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
						TkbkClntPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包，这样可以立即开始发送。
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
						
						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]成功。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
					else if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
					{
						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接超时。" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
					else if( p_AudpCnctSts == AudpCnctStsDsct ) //如果连接断开。
					{
						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接断开。" ), TkbkClntPt->m_RmtNodeNameVstrPt, TkbkClntPt->m_RmtNodeSrvcVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				p_PocsRslt = 0; //设置本处理段执行成功。

				AudpClntSoktCnctOut:
				if( p_PocsRslt != 0 ) //如果本处理段执行失败。
				{
					if( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt != NULL ) && ( TkbkClntPt->m_AudpClntCnctIdx != SIZE_MAX ) )
					{
						AudpClosCnct( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, NULL );
						TkbkClntPt->m_AudpClntCnctIdx = SIZE_MAX;
					}
				}
			}
		}

		//用本端客户端套接字接收远端服务端套接字发送的数据包。
		if( ( TkbkClntPt->m_IsRqstDstoy == 0 ) && ( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果连接未请求销毁，且当前连接状态为已连接。
		{
			size_t p_PktLenByt;
			uint32_t p_TmpUint32;
			size_t p_TmpElmTotal;

			if( ( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntRecvApkt( TkbkClntPt->m_TcpClntSoktPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, 0, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( TkbkClntPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpRecvApkt( TkbkClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, TkbkClntPt->m_AudpClntCnctIdx, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, NULL, ( short )0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				if( p_PktLenByt != -1 ) //如果用本端套接字接收连接的远端套接字发送的数据包成功。
				{
					if( p_PktLenByt == 0 ) //如果数据包的长度为0。
					{
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收数据包。长度为%uzd，表示没有数据，无法继续接收。" ), p_PktLenByt );
						goto RecvPktOut;
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ( int8_t )ClntMediaPocsThrd::PktTypTkbkIdx ) //如果是对讲索引包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果对讲索引包的长度小于1 + 1，表示没有对讲索引。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收对讲索引包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}

						if( TkbkClntPt->m_MyTkbkIdx == -1 ) //如果未设置我的对讲索引。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收我的对讲索引包。对讲索引：%z8d。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );

							TkbkClntPt->m_MyTkbkIdx = TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ]; //设置我的对讲索引。
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntMyTkbkIdxFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_MyTkbkIdx ); //调用用户定义的对讲客户端我的对讲索引函数。
							TkbkClntCnctSendTkbkModePkt( TkbkClntPt, TkbkClntPt->m_LclTkbkMode ); //发送对讲模式包。
							ClntMediaPocsThrdSetTkbkMode( TkbkClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。如果不参考远端对讲模式来设置对讲模式需要这步。
						}
						else
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收其他对讲索引包。对讲索引：%z8d。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );

							TkbkClntTkbkInfoInit( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] ); //对讲信息初始化。
						}
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypTkbkMode ) //如果是对讲模式包。
					{
						if( p_PktLenByt < 1 + 1 + 1 ) //如果对讲模式包的长度小于1 + 1 + 1，表示没有对讲索引和对讲模式。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收对讲模式包。长度为%uzd小于1 + 1 + 1，表示没有对讲索引和对讲模式，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}
						if( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] < ( int8_t )ClntMediaPocsThrd::TkbkModeNone ) || ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] >= ( int8_t )ClntMediaPocsThrd::TkbkModeNoChg ) )
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收对讲模式包。对讲模式为%z8d不正确，无法继续接收。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] );
							goto RecvPktOut;
						}

						if( CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) != 0 ) goto RecvPktOut;
						if( p_TkbkInfoTmpPt->m_IsInit == 0 ) goto RecvPktOut;
						int p_OldRmtTkbkMode = p_TkbkInfoTmpPt->m_RmtTkbkMode; //设置旧远端对讲模式。
						p_TkbkInfoTmpPt->m_RmtTkbkMode = TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ]; //设置远端对讲模式。
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收对讲模式包。对讲索引：%z32d。对讲模式：%z8s。" ), p_TkbkInfoTmpPt->m_TkbkIdx, g_TkbkModeU8strArrPt[ p_TkbkInfoTmpPt->m_RmtTkbkMode ] );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoRmtTkbkModeFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, p_TkbkInfoTmpPt, p_OldRmtTkbkMode, p_TkbkInfoTmpPt->m_RmtTkbkMode ); //调用用户定义的对讲客户端对讲信息远端对讲模式函数。

						ClntMediaPocsThrdSetTkbkMode( TkbkClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypAdoFrm ) //如果是音频输出帧包。
					{
						if( p_PktLenByt < 1 + 1 + 4 ) //如果音频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收音频输出帧包。长度为%uzd小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}

						//读取音频输出帧时间戳。
						p_TmpUint32 = ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

						//将音频输出帧放入容器或自适应抖动缓冲器。
						if( CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) != 0 ) goto RecvPktOut;
						if( p_TkbkInfoTmpPt->m_IsInit == 0 ) goto RecvPktOut;
						if( ( TkbkClntPt->m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) //如果本端对讲模式有音频输出。
						{
							switch( TkbkClntPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
							{
								case 0: //如果要使用容器。
								{
									if( p_PktLenByt > 1 + 1 + 4 ) //如果该音频输出帧为有语音活动。
									{
										VQueueGetTotal( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, &p_TmpElmTotal, 1, NULL ); //获取接收音频输出帧容器的元素总数。
										if( p_TmpElmTotal <= 50 )
										{
											if( VQueuePutTail( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, p_PktLenByt - 1 - 1 - 4, NULL, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
											{
												if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。放入接收音频输出帧容器成功。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
											}
											else
											{
												if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。放入接收音频输出帧容器失败。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。原因：%vs" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
											}
										}
										else
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。接收音频输出帧容器中帧总数为%uzd已经超过上限50，不再放入接收音频输出帧容器。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpElmTotal, p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
										}
									}
									else //如果该音频输出帧为无语音活动。
									{
										if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无语音活动的音频输出帧包。无需放入接收音频输出帧容器。对讲索引：%z32d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
									}
									break;
								}
								case 1: //如果要使用自适应抖动缓冲器。
								{
									if( p_PktLenByt > 1 + 1 + 4 ) //如果该音频输出帧为有语音活动。
									{
										#if IsIcludAjb
										if( AAjbPutFrm( p_TkbkInfoTmpPt->m_AAjbPt, p_TmpUint32, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, p_PktLenByt - 1 - 1 - 4, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。放入音频自适应抖动缓冲器成功。对讲索引：%z32d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
										}
										else
										#else
										VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含Ajb。" ), , );
										#endif
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。放入音频自适应抖动缓冲器失败。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。原因：%vs" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										}
									}
									else //如果该音频输出帧为无语音活动。
									{
										#if IsIcludAjb
										if( AAjbPutFrm( p_TkbkInfoTmpPt->m_AAjbPt, p_TmpUint32, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, 0, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无语音活动的音频输出帧包。放入音频自适应抖动缓冲器成功。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
										}
										else
										#else
										VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含Ajb。" ), , );
										#endif
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无语音活动的音频输出帧包。放入音频自适应抖动缓冲器失败。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。原因：%vs" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										}
									}

									int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
									int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
									int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
									int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
									int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
									int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
									int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
									#if IsIcludAjb
									AAjbGetBufFrmCnt( p_TkbkInfoTmpPt->m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
									#else
									p_CurHaveBufActFrmCnt = -1;
									p_CurHaveBufInactFrmCnt = -1;
									p_CurHaveBufFrmCnt = -1;
									p_MinNeedBufFrmCnt = -1;
									p_MaxNeedBufFrmCnt = -1;
									p_MaxCntuLostFrmCnt = -1;
									p_CurNeedBufFrmCnt = -1;
									#endif
									if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );
									break;
								}
							}
						}
						else //如果本端对讲模式无音频输出。
						{
							if( p_PktLenByt > 1 + 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有语音活动的音频输出帧包。但本端对讲模式无音频。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
							}
							else //如果该音频输出帧为无语音活动。
							{
								if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无语音活动的音频输出帧包。但本端对讲模式无音频。对讲索引：%z32d。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
							}
						}
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypVdoFrm ) //如果是视频输出帧包。
					{
						if( p_PktLenByt < 1 + 1 + 4 ) //如果视频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收视频输出帧包。长度为%uzd小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}

						//读取视频输出帧时间戳。
						p_TmpUint32 = ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

						//将视频输出帧放入容器或自适应抖动缓冲器。
						if( CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) != 0 ) goto RecvPktOut;
						if( p_TkbkInfoTmpPt->m_IsInit == 0 ) goto RecvPktOut;
						if( ( TkbkClntPt->m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) //如果本端对讲模式有视频输出。
						{
							switch( TkbkClntPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
							{
								case 0: //如果要使用容器。
								{
									if( p_PktLenByt > 1 + 1 + 4 ) //如果该视频输出帧为有图像活动。
									{
										VQueueGetTotal( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, &p_TmpElmTotal, 1, NULL ); //获取接收视频输出帧容器的元素总数。
										if( p_TmpElmTotal <= 20 )
										{
											if( VQueuePutTail( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, p_PktLenByt - 1 - 1 - 4, NULL, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
											{
												if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。放入接收视频输出帧容器成功。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
											}
											else
											{
												if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。放入接收视频输出帧容器失败。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。原因：%vs" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
											}
										}
										else
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。接收视频输出帧容器中帧总数为%uzd已经超过上限20，不再放入接收视频输出帧容器。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_TmpElmTotal, p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
										}
									}
									else //如果该视频输出帧为无图像活动。
									{
										if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无图像活动的视频输出帧包。无需放入接收视频输出帧容器。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
									}
									break;
								}
								case 1: //如果要使用自适应抖动缓冲器。
								{
									if( p_PktLenByt > 1 + 1 + 4 ) //如果该视频输出帧为有图像活动。
									{
										#if IsIcludAjb
										if( VAjbPutFrm( p_TkbkInfoTmpPt->m_VAjbPt, FuncGetTickAsMsec(), p_TmpUint32, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, p_PktLenByt - 1 - 1 - 4, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。放入视频自适应抖动缓冲器成功。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
										}
										else
										#else
										VstrCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "未包含Ajb。" ), , );
										#endif
										{
											if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。放入视频自适应抖动缓冲器失败。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。原因：%vs" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										}
									}
									else //如果该视频输出帧为无图像活动。
									{
										if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无图像活动的视频输出帧包。无需放入视频自适应抖动缓冲器。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
									}

									int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
									int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
									int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
									int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
									#if IsIcludAjb
									VAjbGetBufFrmCnt( p_TkbkInfoTmpPt->m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
									#else
									p_CurHaveBufFrmCnt = -1;
									p_MinNeedBufFrmCnt = -1;
									p_MaxNeedBufFrmCnt = -1;
									p_CurNeedBufFrmCnt = -1;
									#endif
									if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );
									break;
								}
							}
						}
						else //如果未初始化视频输出。
						{
							if( p_PktLenByt > 1 + 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收有图像活动的视频输出帧包。但本端对讲模式无视频。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收无图像活动的视频输出帧包。但本端对讲模式无视频。对讲索引：%z32d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_TkbkInfoTmpPt->m_TkbkIdx, p_TmpUint32, p_PktLenByt );
							}
						}
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypTstNtwkDly ) //如果是测试网络延迟包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果测试网络延迟包的长度小于1 + 1，表示没有对讲索引。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收测试网络延迟包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != TkbkClntPt->m_MyTkbkIdx )
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收测试网络延迟包。索引为%z8d与我的对讲索引%z32d不一致，无法继续接收。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], TkbkClntPt->m_MyTkbkIdx );
							goto RecvPktOut;
						}

						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收测试网络延迟包。对讲索引：%z8d。总长度：%uzd。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_PktLenByt );

						TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ( int8_t )ClntMediaPocsThrd::PktTypTstNtwkDlyRply; //设置测试网络延迟应答包。
						if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送测试网络延迟应答包成功。对讲索引：%z32d。总长度：2。" ), TkbkClntPt->m_MyTkbkIdx );
						}
						else
						{
							VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送测试网络延迟应答包失败。对讲索引：%z32d。总长度：2。原因：" ), TkbkClntPt->m_MyTkbkIdx );
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto Out;
						}
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypTstNtwkDlyRply ) //如果是测试网络延迟应答包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果退出包的长度小于1 + 1，表示没有对讲索引。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收测试网络延迟应答包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}

						uint64_t p_NtwkDlyMsec = FuncGetTickAsMsec() - TkbkClntPt->m_TstNtwkDly.m_LastSendTickMsec; //存放网络延迟，单位为毫秒。

						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收测试网络延迟应答包。对讲索引：%z8d。延迟：%uz64d。总长度：%uzd。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_NtwkDlyMsec, p_PktLenByt );

						if( TkbkClntPt->m_TstNtwkDly.m_IsTstNtwkDly != 0 ) //如果要测试网络延迟。
						{
							TkbkClntPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包。
							TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntTstNtwkDlyFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, p_NtwkDlyMsec ); //调用用户定义的对讲客户端测试网络延迟函数。
						}
					}
					else if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypExit ) //如果是退出包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果退出包的长度小于1 + 1，表示没有对讲索引。
						{
							if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：接收退出包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_PktLenByt );
							goto RecvPktOut;
						}

						VstrFmtCpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：接收退出包。对讲索引：%z8d。" ), TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowToastFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						
						if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] == TkbkClntPt->m_MyTkbkIdx ) //如果对讲索引是我的对讲索引。
						{
							TkbkClntPt->m_IsRecvExitPkt = 1; //设置已接收退出包。
							TkbkClntPt->m_IsRqstDstoy = 1; //设置已请求销毁。
						}
						else
						{
							TkbkClntTkbkInfoDstoy( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] ); //对讲信息销毁。
						}

					}
				} //如果用本端套接字接收连接的远端套接字发送的数据包超时，就重新接收。
			}
			else //如果用本端套接字接收连接的远端套接字发送的数据包失败。
			{
				VstrIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：用本端套接字接收连接的远端套接字发送的数据包失败。原因：" ) );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				
				TkbkClntPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsTmot; //设置当前连接状态为异常断开。
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_CurCnctSts ); //调用用户定义的对讲客户端连接状态函数。
				TkbkClntPt->m_IsRqstDstoy = 1; //设置已请求销毁。
				goto RecvPktOut;
			}
			RecvPktOut:;
		}

		//用本端客户端套接字测试网络延迟。
		if( ( TkbkClntPt->m_TstNtwkDly.m_IsTstNtwkDly != 0 ) && ( TkbkClntPt->m_IsRqstDstoy == 0 ) && ( TkbkClntPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) && ( TkbkClntPt->m_MyTkbkIdx != -1 ) ) //如果要测试网络延迟，且连接未请求销毁，且当前连接状态为已连接，且已设置我的对讲索引。
		{
			uint64_t p_CurTickMsec = FuncGetTickAsMsec(); //存放当前嘀嗒钟，单位为毫秒。

			if( ( TkbkClntPt->m_TstNtwkDly.m_IsRecvRplyPkt != 0 ) && ( p_CurTickMsec - TkbkClntPt->m_TstNtwkDly.m_LastSendTickMsec >= TkbkClntPt->m_TstNtwkDly.m_SendIntvlMsec ) ) //如果已接收测试网络延迟应答包，且最后发送测试网络延迟包已超过间隔时间。
			{
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypTstNtwkDly; //设置数据包类型为测试网络延迟包。
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = TkbkClntPt->m_MyTkbkIdx; //设置对讲索引。
				if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送测试网络延迟包成功。对讲索引：%z32d。总长度：2。" ), TkbkClntPt->m_MyTkbkIdx );

					TkbkClntPt->m_TstNtwkDly.m_LastSendTickMsec = p_CurTickMsec; //设置测试网络延迟包最后发送的嘀嗒钟。
					TkbkClntPt->m_TstNtwkDly.m_IsRecvRplyPkt = 0; //设置未接收测试网络延迟应答包。
				}
				else
				{
					VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送测试网络延迟包失败。对讲索引：%z32d。总长度：2。原因：" ), TkbkClntPt->m_MyTkbkIdx );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
			}
		}

		//连接销毁。
		if( TkbkClntPt->m_IsRqstDstoy == 1 ) //如果已请求销毁。
		{
			TkbkClntCnctInfoDstoy( TkbkClntPt );
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//用户定义的读取音视频输入帧函数。
void TkbkClntUserReadAdoVdoInptFrm( TkbkClnt * TkbkClntPt,
									int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
									uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
									uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
									uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
									uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt )
{
	size_t p_PktLenByt = 0; //存放数据包的长度，单位为字节。

	//发送音频输入帧。
	if( ( ( TkbkClntPt->m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) && ( AdoInptPcmSrcFrmPt != NULL ) ) //如果本端对讲模式有音频输入，且有音频输入Pcm格式原始帧。
	{
		if( AdoInptEncdRsltFrmPt == NULL ) //如果没有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 ) //如果音频输入Pcm格式结果帧为有语音活动。
			{
				memcpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, AdoInptPcmRsltFrmPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 1 + 4 + TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳 + 音频输入Pcm格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动。
			{
				p_PktLenByt = 1 + 1 + 4; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳。
			}
		}
		else //如果有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 && AdoInptEncdRsltFrmIsNeedTrans != 0 ) //如果音频输入Pcm格式结果帧为有语音活动，且音频输入已编码格式结果帧需要传输。
			{
				memcpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 1 + 4 + AdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳 + 音频输入已编码格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 1 + 4; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			TkbkClntPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypAdoFrm;
			//设置数据包类型为音频输入帧包。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( int8_t )TkbkClntPt->m_MyTkbkIdx;
			//设置音频输入帧时间戳。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送有语音活动的音频输入帧包成功。对讲索引：%z32d。音频输入帧时间戳：%uz32d。总长度：%uzd。" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
			}
			else
			{
				VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送有语音活动的音频输入帧包失败。对讲索引：%z32d。音频输入帧时间戳：%uz32d。总长度：%uzd。原因：" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}

			TkbkClntPt->m_LastSendAdoInptFrmIsAct = 1; //设置最后发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( TkbkClntPt->m_LastSendAdoInptFrmIsAct != 0 ) //如果最后发送的音频输入帧为有语音活动，就发送。
			{
				TkbkClntPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypAdoFrm;
				//设置对讲索引。
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( int8_t )TkbkClntPt->m_MyTkbkIdx;
				//设置音频输入帧时间戳。
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] = ( int8_t )( ( TkbkClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 0, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送无语音活动的音频输入帧包成功。对讲索引：%z32d。音频输入帧时间戳：%uz32d，总长度：%uzd。" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
				}
				else
				{
					VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送无语音活动的音频输入帧包失败。对讲索引：%z32d。音频输入帧时间戳：%uz32d，总长度：%uzd。原因：" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}

				TkbkClntPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的音频输入帧无语音活动。
			}
			else //如果最后发送的音频输入帧为无语音活动，无需发送。
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：本次音频输入帧为无语音活动，且最后发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}

	//发送视频输入帧。
	if( ( ( TkbkClntPt->m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) && ( VdoInptYu12RsltFrmPt != NULL ) ) //如果本端对讲模式有视频输入，且有视频输入Yu12格式结果帧。
	{
		if( VdoInptEncdRsltFrmPt == NULL ) //如果没有视频输入已编码格式结果帧。
		{
			//设置视频输入帧宽度。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 6 ] = ( int8_t )( VdoInptYu12RsltFrmWidth & 0xFF );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 7 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF00 ) >> 8 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 8 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF0000 ) >> 16 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 9 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF000000 ) >> 24 );
			//设置视频输入帧高度。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] = ( int8_t )( VdoInptYu12RsltFrmHeight & 0xFF );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 11 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF00 ) >> 8 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 12 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF0000 ) >> 16 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 13 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF000000 ) >> 24 );

			memcpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4 + 4 + 4, VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2 ); //设置视频输入帧。
			p_PktLenByt = 1 + 1 + 4 + 4 + 4 + VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2; //数据包长度 = 数据包类型 + 对讲索引 + 视频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + 视频输入Yu12格式结果帧。
		}
		else //如果有视频输入已编码格式结果帧。
		{
			if( VdoInptEncdRsltFrmLenByt != 0 ) //如果本次视频输入已编码格式结果帧为有图像活动。
			{
				memcpy( TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt ); //设置视频输入帧。
				p_PktLenByt = 1 + 1 + 4 + VdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 对讲索引 + 视频输入帧时间戳 + 视频输入已编码格式结果帧。
			}
			else
			{
				p_PktLenByt = 1 + 1 + 4; //数据包长度 = 数据包类型 + 对讲索引 + 视频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 1 + 4 ) //如果本次视频输入帧为有图像活动，就发送。
		{
			TkbkClntPt->m_LastSendVdoInptFrmTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

			//设置数据包类型为视频输入帧包。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypVdoFrm;
			//设置对讲索引。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( int8_t )TkbkClntPt->m_MyTkbkIdx;
			//设置视频输入帧时间戳。
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )( TkbkClntPt->m_LastSendVdoInptFrmTimeStamp & 0xFF );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkClntPt->m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkClntPt->m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] = ( int8_t )( ( TkbkClntPt->m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( TkbkClntCnctSendPkt( TkbkClntPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：发送有图像活动的视频输入帧包成功。对讲索引：%z32d。视频输入帧时间戳：%uz32d。总长度：%d。类型：%d。" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
			}
			else
			{
				VstrFmtIns( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：对讲客户端：发送有图像活动的视频输入帧包失败。对讲索引：%z32d。视频输入帧时间戳：%uz32d。总长度：%d。类型：%d。原因：" ), TkbkClntPt->m_MyTkbkIdx, TkbkClntPt->m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkClntPt->m_ClntMediaPocsThrdPt, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}
		else //如果本次视频输入帧为无图像活动，无需发送。
		{
			if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "客户端媒体处理线程：对讲客户端：本次视频输入帧为无图像活动，无需发送。" ) );
		}
	}
}

//用户定义的写入音频输出帧函数。
void TkbkClntUserWriteAdoOtptFrm( TkbkClnt * TkbkClntPt, int32_t AdoOtptStrmIdx,
								  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;
	uint32_t p_AdoOtptFrmTimeStamp;
	size_t m_TmpSz = 0;
	
	CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, AdoOtptStrmIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL );
	
	//取出并写入音频输出帧。
	{
		//从容器或自适应抖动缓冲器取出音频输出帧。
		switch( TkbkClntPt->m_UseWhatRecvOtptFrm ) //使用什么接收音频输出帧。
		{
			case 0: //如果要使用容器。
			{
				VQueueGetTotal( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, &m_TmpSz, 1, NULL ); //获取接收音频输出帧容器的元素总数。
				if( m_TmpSz != 0 )
				{
					VQueueGetHead( p_TkbkInfoTmpPt->m_RecvAdoOtptFrmCntnrPt, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpByteSz, NULL, &m_TmpSz, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //从接收音频输出帧容器中取出并删除第一个帧。
				}

				if( m_TmpSz != 0 ) //如果音频输出帧为有语音活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从接收音频输出帧容器取出有语音活动的音频输出帧。长度：%uzd。" ), AdoOtptStrmIdx, m_TmpSz );
				}
				else //如果接收音频输出帧容器为空，或音频输出帧为无语音活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从接收音频输出帧容器取出无语音活动的音频输出帧。长度：%uzd。" ), AdoOtptStrmIdx, m_TmpSz );
				}

				break;
			}
			case 1: //如果要使用自适应抖动缓冲器。
			{
				int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
				int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
				int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
				int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
				int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
				int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
				int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
				#if IsIcludAjb
				AAjbGetBufFrmCnt( p_TkbkInfoTmpPt->m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
				#else
				p_CurHaveBufActFrmCnt = -1;
				p_CurHaveBufInactFrmCnt = -1;
				p_CurHaveBufFrmCnt = -1;
				p_MinNeedBufFrmCnt = -1;
				p_MaxNeedBufFrmCnt = -1;
				p_MaxCntuLostFrmCnt = -1;
				p_CurNeedBufFrmCnt = -1;
				#endif
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), AdoOtptStrmIdx, p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

				//从音频自适应抖动缓冲器取出音频输出帧。
				#if IsIcludAjb
				AAjbGetFrm( p_TkbkInfoTmpPt->m_AAjbPt, &p_AdoOtptFrmTimeStamp, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpByteSz, &m_TmpSz, 1, NULL );
				#endif

				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从音频自适应抖动缓冲器取出有语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), AdoOtptStrmIdx, p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从音频自适应抖动缓冲器取出无语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), AdoOtptStrmIdx, p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从音频自适应抖动缓冲器取出丢失的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), AdoOtptStrmIdx, p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				break;
			}
		}

		//写入音频输出帧。
		if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
		{
			if( AdoOtptPcmSrcFrmPt != NULL ) //如果要使用音频输出Pcm格式原始帧。
			{
				if( m_TmpSz != AdoOtptPcmFrmLenUnit * sizeof( int16_t ) )
				{
					memset( AdoOtptPcmSrcFrmPt, 0, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。音频输出帧的长度不等于Pcm格式的长度。音频输出帧：%uzd。Pcm格式：%z32d。" ), AdoOtptStrmIdx, m_TmpSz, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
				else
				{
					//写入音频输出Pcm格式原始帧。
					memcpy( AdoOtptPcmSrcFrmPt, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
			}
			else //如果要使用音频输出已编码格式原始帧。
			{
				if( m_TmpSz > AdoOtptEncdSrcFrmSzByt )
				{
					*AdoOtptEncdSrcFrmLenBytPt = 0;
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。音频输出帧的长度已超过已编码格式的长度。音频输出帧：%uzd，已编码格式：%uzd。" ), AdoOtptStrmIdx, m_TmpSz, AdoOtptEncdSrcFrmSzByt );
				}
				else
				{
					//写入音频输出已编码格式原始帧。
					*AdoOtptEncdSrcFrmLenBytPt = m_TmpSz;
					memcpy( AdoOtptEncdSrcFrmPt, p_TkbkInfoTmpPt->m_AdoOtptTmpVar.m_TmpBytePt, m_TmpSz );
				}
			}
		}
		else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
		{
			if( AdoOtptPcmSrcFrmPt != NULL ) //如果要使用音频输出Pcm格式原始帧。
			{
				memset( AdoOtptPcmSrcFrmPt, 0, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
			}
			else //如果要使用音频输出已编码格式原始帧。
			{
				*AdoOtptEncdSrcFrmLenBytPt = 0;
			}
		}
		else //如果音频输出帧为丢失。
		{
			if( AdoOtptPcmSrcFrmPt != NULL ) //如果要使用音频输出Pcm格式原始帧。
			{
				memset( AdoOtptPcmSrcFrmPt, 0, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
			}
			else //如果要使用音频输出已编码格式原始帧。
			{
				*AdoOtptEncdSrcFrmLenBytPt = m_TmpSz;
			}
		}
	}
}

//用户定义的获取音频输出帧函数。
void TkbkClntUserGetAdoOtptFrm( TkbkClnt * TkbkClntPt, int32_t AdoOtptStrmIdx,
								int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{
		
}

//用户定义的写入视频输出帧函数。
void TkbkClntUserWriteVdoOtptFrm( TkbkClnt * TkbkClntPt, uint32_t VdoOtptStrmIdx,
								  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
								  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;
	uint32_t p_VdoOtptFrmTimeStamp;
	size_t m_TmpSz = 0;

	CQueueGetByNum( TkbkClntPt->m_TkbkInfoCntnrPt, VdoOtptStrmIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL );

	//从容器或自适应抖动缓冲器取出一个视频输出帧。
	switch( TkbkClntPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果要使用容器。
		{
			VQueueGetTotal( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, &m_TmpSz, 1, NULL );
			if( m_TmpSz != 0 ) //如果接收视频输出帧容器不为空。
			{
				VQueueGetHead( p_TkbkInfoTmpPt->m_RecvVdoOtptFrmCntnrPt, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpByteSz, NULL, &m_TmpSz, 1, 1, TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //从接收视频输出帧容器中取出并删除第一个帧。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从接收视频输出帧容器取出有图像活动的视频输出帧。长度：%uzd。" ), VdoOtptStrmIdx, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从接收视频输出帧容器取出无图像活动的视频输出帧。长度：%uzd。" ), VdoOtptStrmIdx, m_TmpSz );
				}
			}
			break;
		}
		case 1: //如果要使用自适应抖动缓冲器。
		{
			int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
			int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
			int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
			int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
			#if IsIcludAjb
			VAjbGetBufFrmCnt( p_TkbkInfoTmpPt->m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
			#else
			p_CurHaveBufFrmCnt = -1;
			p_MinNeedBufFrmCnt = -1;
			p_MaxNeedBufFrmCnt = -1;
			p_CurNeedBufFrmCnt = -1;
			#endif

			if( p_CurHaveBufFrmCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), VdoOtptStrmIdx, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );

				//从视频自适应抖动缓冲器取出视频输出帧。
				#if IsIcludAjb
				VAjbGetFrm( p_TkbkInfoTmpPt->m_VAjbPt, FuncGetTickAsMsec(), &p_VdoOtptFrmTimeStamp, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpByteSz, &m_TmpSz, 1, NULL );
				#endif

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从视频自适应抖动缓冲器取出有图像活动的视频输出帧。视频输出帧时间戳：%uz32d。长度：%uzd。" ), VdoOtptStrmIdx, p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。从视频自适应抖动缓冲器取出无图像活动的视频输出帧。视频输出帧时间戳：%uz32d。长度：%uzd。" ), VdoOtptStrmIdx, p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
			}
			break;
		}
	}

	//写入视频输出帧。
	if( m_TmpSz > 0 ) //如果视频输出帧为有图像活动。
	{
		if( VdoOtptYu12SrcFrmPt != NULL ) //如果要使用视频输出Yu12格式原始帧。
		{
			//读取视频输出帧宽度。
			*VdoOtptYu12SrcFrmWidthPt = ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 0 ] & 0xFF ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 1 ] & 0xFF ) << 8 ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 2 ] & 0xFF ) << 16 ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 3 ] & 0xFF ) << 24 );
			//读取视频输出帧高度。
			*VdoOtptYu12SrcFrmHeightPt = ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 4 ] & 0xFF ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 5 ] & 0xFF ) << 8 ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 6 ] & 0xFF ) << 16 ) + ( ( p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt[ 7 ] & 0xFF ) << 24 );

			if( m_TmpSz - 4 - 4 != *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 )
			{
				*VdoOtptYu12SrcFrmWidthPt = 0;
				*VdoOtptYu12SrcFrmHeightPt = 0;
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。视频输出帧的长度不等于Yu12格式的长度。视频输出帧：%uzd。Yu12格式：%z32d。" ), VdoOtptStrmIdx, m_TmpSz - 4 - 4, *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 );
				return;
			}

			//写入视频输出Yu12格式原始帧。
			memcpy( VdoOtptYu12SrcFrmPt, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt + 4 + 4, m_TmpSz - 4 - 4 );
		}
		else //如果要使用视频输出已编码格式帧。
		{
			if( m_TmpSz > VdoOtptEncdSrcFrmSzByt )
			{
				*VdoOtptEncdSrcFrmLenBytPt = 0;
				if( TkbkClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：对讲客户端：对讲索引：%z32d。视频输出帧的长度已超过已编码格式的长度。视频输出帧：%uzd。已编码格式：%z32d。" ), VdoOtptStrmIdx, m_TmpSz, VdoOtptEncdSrcFrmSzByt );
				return;
			}

			//写入视频输出已编码格式帧。
			memcpy( VdoOtptEncdSrcFrmPt, p_TkbkInfoTmpPt->m_VdoOtptTmpVar.m_TmpBytePt, m_TmpSz );
			*VdoOtptEncdSrcFrmLenBytPt = m_TmpSz;
		}
	}
	else if( m_TmpSz == 0 ) //如果视频输出帧为无图像活动。
	{
		if( VdoOtptYu12SrcFrmPt != NULL ) //如果要使用视频输出Yu12格式原始帧。
		{
			
		}
		else //如果要使用视频输出已编码格式原始帧。
		{
			
		}
	}
}

//用户定义的获取视频输出帧函数。
void TkbkClntUserGetVdoOtptFrm( TkbkClnt * TkbkClntPt, uint32_t VdoOtptStrmIdx,
								uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
								uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{
	
}