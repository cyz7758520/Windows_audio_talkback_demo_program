#include "BdctClnt.h"
#include "ClntMediaPocsThrd.h"

//广播客户端初始化。
int BdctClntInit( BdctClnt * BdctClntPt, int32_t CnctNumIsDecr )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	
	BdctClntPt->m_IsInit = 1; //设置已初始化广播客户端。
	BdctClntPt->m_CnctNumIsDecr = CnctNumIsDecr; //设置连接序号是否递减。

	BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntInitFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt ); //调用用户定义的广播客户端初始化函数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		BdctClntDstoy( BdctClntPt );
	}
	return p_Rslt;
}

//广播客户端销毁。
void BdctClntDstoy( BdctClnt * BdctClntPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctClnt::CnctInfo * p_CnctInfoTmpPt;

	//连接信息全部销毁。
	for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( BdctClntPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
	{
		if( p_CnctInfoTmpPt->m_IsInit != 0 )
		{
			BdctClntCnctInfoDstoy( BdctClntPt, p_CnctInfoTmpPt );
		}
	}

	BdctClntPt->m_IsInit = 0; //设置未初始化广播客户端。

	BdctClntPt->m_CnctInfoCurMaxNum = -1; //设置连接信息的当前最大序号。

	BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntDstoyFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt ); //调用用户定义的广播客户端销毁函数。

	ClntMediaPocsThrdSetTkbkMode( BdctClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。

	ClntMediaPocsThrdIsAutoRqirExit( BdctClntPt->m_ClntMediaPocsThrdPt, NULL ); //判断是否自动请求退出。在没有广播连接时需要这一步判断。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接信息初始化。
BdctClnt::CnctInfo * BdctClntCnctInfoInit( BdctClnt * BdctClntPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	BdctClnt::CnctInfo * p_CnctInfoTmpPt = NULL;
	size_t p_ElmTotal; //存放元素总数。
	
	if( BdctClntPt->m_IsInit == 0 ) //如果广播客户端未初始化。
	{
		VstrCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：广播客户端未初始化，无法继续连接信息初始化。" ), ,  );
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	{
		//查找是否有未初始化的连接信息。
		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( BdctClntPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( p_CnctInfoTmpPt->m_IsInit == 0 ) goto CnctInfoFindOut; //如果找到了未初始化的连接信息。
		}
		p_CnctInfoTmpPt = NULL;

		//如果没找到未初始化的连接信息。
		CQueueGetTotal( BdctClntPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
		if( CQueuePutTail( BdctClntPt->m_CnctInfoCntnrPt, NULL, ( void * * )&p_CnctInfoTmpPt, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：添加到连接信息容器失败。原因：%vs" ), BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_CnctInfoTmpPt->m_Idx = p_ElmTotal;
		p_CnctInfoTmpPt->m_RmtNodeNameVstrPt = NULL;
		p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt = NULL;
	}
	CnctInfoFindOut:;
	p_CnctInfoTmpPt->m_IsInit = 1; //设置连接信息已初始化。
	BdctClntPt->m_CnctInfoCurMaxNum++; //递增连接信息的当前最大序号。
	p_CnctInfoTmpPt->m_Num = BdctClntPt->m_CnctInfoCurMaxNum; //设置序号。

	p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl; //设置协议为Tcp协议或高级Udp协议。
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt ) != 0 ) //设置远端套接字绑定的远端节点名称动态字符串的指针。
	{
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "客户端媒体处理线程：广播客户端：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt ) != 0 ) //设置远端套接字绑定的远端节点服务动态字符串的指针。
	{
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "客户端媒体处理线程：广播客户端：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	p_CnctInfoTmpPt->m_TcpClntSoktPt = NULL; //设置本端Tcp协议客户端套接字的指针。
	p_CnctInfoTmpPt->m_AudpClntCnctIdx = SIZE_MAX; //设置本端高级Udp协议客户端连接索引。
	p_CnctInfoTmpPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsWait; //设置当前连接状态。
	p_CnctInfoTmpPt->m_IsRqstDstoy = 0; //设置是否请求销毁。

	p_CnctInfoTmpPt->m_MyTkbkIdx = -1; //设置我的对讲索引。
	p_CnctInfoTmpPt->m_IsRecvExitPkt = 0; //设置是否接收退出包。
	
	VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：初始化与远端节点%vs[%vs:%vs]的连接成功。" ), ( ( IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
	if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctInitFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt ); //调用用户定义的广播客户端连接初始化函数。
	BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_CnctInfoTmpPt != NULL )
		{
			if( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt );
			if( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			CQueueDelTail( BdctClntPt->m_CnctInfoCntnrPt, NULL, 0, NULL );
			p_CnctInfoTmpPt = NULL;
		}
	}
	return p_CnctInfoTmpPt;
}

//连接信息销毁。
void BdctClntCnctInfoDstoy( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctClnt::CnctInfo * p_CnctInfoTmpPt;

	if( CnctInfoPt->m_IsInit != 0 )
	{
		//发送退出包。
		if( ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果未接收退出包，且当前连接状态为已连接。
		{
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypExit; //设置退出包。
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = CnctInfoPt->m_MyTkbkIdx; //设置对讲索引。
			if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：发送退出包成功。对讲索引：%uzd。总长度：2。" ), CnctInfoPt->m_Idx, CnctInfoPt->m_MyTkbkIdx );
				if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else
			{
				VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：发送退出包失败。对讲索引：%uzd。总长度：2。原因：" ), CnctInfoPt->m_Idx, CnctInfoPt->m_MyTkbkIdx );
				if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		//销毁本端Tcp协议客户端套接字。
		if( CnctInfoPt->m_TcpClntSoktPt != NULL )
		{
			TcpClntDstoy( CnctInfoPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
			CnctInfoPt->m_TcpClntSoktPt = NULL;
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：销毁本端Tcp协议客户端套接字成功。" ), CnctInfoPt->m_Idx );
		}

		//销毁本端高级Udp协议客户端连接。
		if( CnctInfoPt->m_AudpClntCnctIdx != -1 )
		{
			AudpClosCnct( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, NULL );
			CnctInfoPt->m_AudpClntCnctIdx = -1;
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：销毁本端高级Udp协议客户端连接成功。" ), CnctInfoPt->m_Idx );
		}

		if( ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsTmot ) ) //如果未接收退出包，且当前连接状态为异常断开，就重连。
		{
			CnctInfoPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsWait; //设置当前连接状态。
			BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。
			CnctInfoPt->m_IsRqstDstoy = 0; //设置未请求销毁。

			CnctInfoPt->m_MyTkbkIdx = -1; //设置我的对讲索引。
			CnctInfoPt->m_IsRecvExitPkt = 0; //设置未接收退出包。

			VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：连接异常断开，准备重连。" ), CnctInfoPt->m_Idx );
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		else //如果已接收退出包，或当前连接状态不为异常断开，就销毁。
		{
			//设置当前连接状态。
			if( CnctInfoPt->m_CurCnctSts <= ClntMediaPocsThrd::CnctStsWait )
			{
				CnctInfoPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsTmot;
				BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。
			}
			else if( CnctInfoPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct )
			{
				CnctInfoPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsDsct;
				BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。
			}

			CnctInfoPt->m_IsInit = 0; //设置连接信息未初始化。
			
			//递减后面连接信息的序号。
			if( BdctClntPt->m_CnctNumIsDecr != 0 )
			{
				for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( BdctClntPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
				{
					if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) && ( p_CnctInfoTmpPt->m_Num > CnctInfoPt->m_Num ) )
					{
						p_CnctInfoTmpPt->m_Num--; //设置后面连接信息的序号全部递减1。
					}
				}
				BdctClntPt->m_CnctInfoCurMaxNum--; //递减连接信息的当前最大序号。
			}

			VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：已销毁。" ), CnctInfoPt->m_Idx );
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

			BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctDstoyFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, CnctInfoPt ); //调用用户定义的广播客户端连接销毁函数。
			VstrDstoy( CnctInfoPt->m_RmtNodeNameVstrPt ); //销毁远端套接字绑定的远端节点名称动态字符串。
			VstrDstoy( CnctInfoPt->m_RmtNodeSrvcVstrPt ); //销毁远端套接字绑定的远端节点服务动态字符串。
		}

		//ClntMediaPocsThrdIsAutoRqirExit( TkbkClntPt->m_ClntMediaPocsThrdPt, NULL ); //判断是否自动请求退出。这里不需要判断，因为有多少个广播连接就会请求多少次，没有必要，只需要在连接信息全部销毁时请求一次即可。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接信息发送数据包。
int BdctClntCnctInfoSendPkt( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( CnctInfoPt->m_IsInit != 0 ) && ( CnctInfoPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果连接信息已初始化，且当前连接状态为已连接。
	{
		//发送数据包。
		if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, ErrInfoVstrPt ) == 0 ) ) ||
			( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ) == 0 ) ) )
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
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) VstrCpy( ErrInfoVstrPt, Cu8vstr( "当前激活的连接信息的指针为空，或当前连接状态不为已连接。" ), , );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接信息发送对讲模式包。
int BdctClntCnctInfoSendTkbkModePkt( BdctClnt * BdctClntPt, BdctClnt::CnctInfo * CnctInfoPt, int LclTkbkMode )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ( byte )ClntMediaPocsThrd::PktTypTkbkMode;
	BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = ( byte )CnctInfoPt->m_MyTkbkIdx;
	BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( byte )LclTkbkMode;
	if( BdctClntCnctInfoSendPkt( BdctClntPt, CnctInfoPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, 3, 1, 1, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：发送对讲模式包成功。对讲索引：%z32d。对讲模式：%z8s。" ), CnctInfoPt->m_Idx, CnctInfoPt->m_MyTkbkIdx, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
	}
	else
	{
		VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：发送对讲模式包失败。对讲索引：%z32d。对讲模式：%z8s。原因：" ), CnctInfoPt->m_Idx, CnctInfoPt->m_MyTkbkIdx, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
		if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//所有连接发送音频数据包。
void BdctClntAllCnctSendAdoPkt( BdctClnt * BdctClntPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctClnt::CnctInfo * p_CnctInfoTmpPt;

	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( BdctClntPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) &&
			( p_CnctInfoTmpPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) &&
			( p_CnctInfoTmpPt->m_MyTkbkIdx != SIZE_MAX ) )
		{
			( ( int8_t * )PktPt )[ 1 ] = ( int8_t ) p_CnctInfoTmpPt->m_MyTkbkIdx; //设置对讲索引。

			//发送数据包。
			if( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, NULL );
			else AudpSendApkt( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, NULL );
		}
	}
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接处理，包括连接服务端、接收数据包、删除连接信息。
void BdctClntCnctPocs( BdctClnt * BdctClntPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctClnt::CnctInfo * p_CnctInfoTmpPt;
	size_t p_PktLenByt;
	size_t p_TmpElmTotal;
	uint32_t p_TmpUint32;

	//遍历连接信息容器。
	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( BdctClntPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		if( p_CnctInfoTmpPt->m_IsInit != 0 )
		{
			//用本端客户端套接字连接远端服务端套接字。
			if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts <= ClntMediaPocsThrd::CnctStsWait ) ) //如果该连接未请求销毁，且当前连接状态为等待远端接受连接。
			{
				int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。

				{ //设置远端节点的地址族。
					ADDRINFOW * p_AddrInfoListPt;
					if( GetAddrInfo( ( PCWSTR )p_CnctInfoTmpPt->m_RmtNodeNameVstrPt->m_StrPt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
					{
						p_RmtNodeAddrFamly = ( p_AddrInfoListPt->ai_family == AF_INET ) ? 4 : ( p_AddrInfoListPt->ai_family == AF_INET6 ) ? 6 : 0;
						FreeAddrInfo( p_AddrInfoListPt );
					}
					else p_RmtNodeAddrFamly = 0;
				}

				if( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) //用本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
				{
					int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
					TcpCnctSts p_TcpCnctSts;

					if( p_CnctInfoTmpPt->m_TcpClntSoktPt == NULL ) //如果未初始化本端Tcp协议客户端套接字。
					{
						if( -p_CnctInfoTmpPt->m_CurCnctSts >= BdctClntPt->m_ClntMediaPocsThrdPt->m_MaxCnctTimes ) //如果未达到最大连接次数。
						{
							p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt->m_Idx );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
						BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。

						VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：开始第 %d 次连接。" ), p_CnctInfoTmpPt->m_Idx, -p_CnctInfoTmpPt->m_CurCnctSts );
						if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

						if( TcpClntInit( &p_CnctInfoTmpPt->m_TcpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, NULL, NULL, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字失败。
						{
							VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}
					}

					if( TcpClntWaitCnct( p_CnctInfoTmpPt->m_TcpClntSoktPt, 0, &p_TcpCnctSts, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字成功。
					{
						if( p_TcpCnctSts == TcpCnctStsWait ) //如果等待远端接受连接。
						{
							//继续等待本端本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
						}
						else if( p_TcpCnctSts == TcpCnctStsCnct ) //如果连接成功。
						{
							if( TcpClntSetNoDelay( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							if( TcpClntSetSendBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							if( TcpClntSetRecvBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024 * 3, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							if( TcpClntSetKeepAlive( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 1, 1, 5, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							if( TcpClntGetLclAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：获取本端Tcp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							if( TcpClntGetRmtAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：获取本端Tcp协议客户端套接字连接的远端Tcp协议客户端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto TcpClntSoktCnctOut;
							}

							p_CnctInfoTmpPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。

							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：初始化本端Tcp协议客户端套接字[%vs:%vs]，并连接远端Tcp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						else //如果连接失败。
						{
							VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}
					}
					else //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。
					{
						VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。原因：" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
						if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpClntSoktCnctOut;
					}

					p_PocsRslt = 0; //设置本处理段执行成功。
				
					TcpClntSoktCnctOut:
					if( p_PocsRslt != 0 ) //如果本处理段执行失败。
					{
						if( p_CnctInfoTmpPt->m_TcpClntSoktPt != NULL )
						{
							TcpClntDstoy( p_CnctInfoTmpPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
							p_CnctInfoTmpPt->m_TcpClntSoktPt = NULL;
						}
					}
				}
				else //用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字。
				{
					int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
					AudpCnctSts p_AudpCnctSts;

					if( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt == NULL ) //如果未初始化本端高级Udp协议客户端套接字。
					{
						if( AudpInit( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_LicnCodePt, &BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, NULL, NULL, 0, 5000, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议客户端套接字成功。
						{
							if( AudpSetSendBufSz( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, 1024 * 1024, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto AudpClntSoktCnctOut;
							}

							if( AudpSetRecvBufSz( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, 1024 * 1024 * 3, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto AudpClntSoktCnctOut;
							}

							if( AudpGetLclAddr( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, NULL, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
							{
								VstrIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：获取本端高级Udp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto AudpClntSoktCnctOut;
							}

							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：初始化本端高级Udp协议客户端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						else //如果初始化本端高级Udp协议客户端套接字失败。
						{
							VstrIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：初始化本端高级Udp协议客户端套接字失败。原因：" ) );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}
					}

					if( p_CnctInfoTmpPt->m_AudpClntCnctIdx == SIZE_MAX )
					{
						if( -p_CnctInfoTmpPt->m_CurCnctSts >= BdctClntPt->m_ClntMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
						{
							p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

							VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt->m_Idx );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}

						p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
						BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。
					
						VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：开始第 %d 次连接。" ), p_CnctInfoTmpPt->m_Idx, -p_CnctInfoTmpPt->m_CurCnctSts );
						if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

						if( AudpCnct( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, &p_CnctInfoTmpPt->m_AudpClntCnctIdx, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字失败。
						{
							VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}
					}

					if( AudpWaitCnct( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, 0, &p_AudpCnctSts, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //循环等待本端高级Udp协议套接字连接远端成功。
					{
						if( p_AudpCnctSts == AudpCnctStsWait ) //如果等待远端接受连接。
						{
							//重新循环，继续等待本端高级Udp协议套接字连接远端。
						}
						else if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
						{
							if( AudpGetRmtAddr( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
							{
								VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：获取本端高级Udp协议客户端套接字连接的远端高级Udp协议服务端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								goto AudpClntSoktCnctOut;
							}

							p_CnctInfoTmpPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。

							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						}
						else if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
						{
							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接超时。" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}
						else if( p_AudpCnctSts == AudpCnctStsDsct ) //如果连接断开。
						{
							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接断开。" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}
					}

					p_PocsRslt = 0; //设置本处理段执行成功。

					AudpClntSoktCnctOut:
					if( p_PocsRslt != 0 ) //如果本处理段执行失败。
					{
						if( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt != NULL ) && ( p_CnctInfoTmpPt->m_AudpClntCnctIdx != SIZE_MAX ) )
						{
							AudpClosCnct( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL );
							p_CnctInfoTmpPt->m_AudpClntCnctIdx = SIZE_MAX;
						}
					}
				}
			}

			//用本端客户端套接字接收远端服务端套接字发送的数据包。
			if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) ) //如果该连接未请求销毁，且当前连接状态为已连接。
			{
				if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntRecvApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, 0, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpRecvApkt( BdctClntPt->m_ClntMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, NULL, 0, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
					{
						if( p_PktLenByt == 0 ) //如果数据包的数据长度为0。
						{
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收数据包。长度为%uzd，表示没有数据，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
							goto RecvPktOut;
						}
						else if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypTkbkIdx ) //如果是对讲索引包。
						{
							if( p_PktLenByt < 1 + 1 ) //如果对讲模式包的长度小于1 + 1，表示没有对讲索引。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收对讲索引包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}

							if( p_CnctInfoTmpPt->m_MyTkbkIdx == -1 ) //如果未设置我的对讲索引。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收我的对讲索引包。对讲索引：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );

								p_CnctInfoTmpPt->m_MyTkbkIdx = BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ]; //设置我的对讲索引。
								BdctClntCnctInfoSendTkbkModePkt( BdctClntPt, p_CnctInfoTmpPt, ClntMediaPocsThrd::TkbkModeAdoInpt ); //发送对讲模式包。
								ClntMediaPocsThrdSetTkbkMode( BdctClntPt->m_ClntMediaPocsThrdPt, 0, 0 ); //设置对讲模式。
							}
							else
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收其他对讲索引包。对讲索引：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
							}
						}
						else if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypTkbkMode ) //如果是对讲模式包。
						{
							if( p_PktLenByt < 1 + 1 + 1 ) //如果对讲模式包的长度小于1 + 1 + 1，表示没有对讲索引和对讲模式。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收对讲模式包。长度为%uzd小于1 + 1 + 1，表示没有对讲索引和对讲模式，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] < ( int8_t )ClntMediaPocsThrd::TkbkModeNone ) || ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] >= ( int8_t )ClntMediaPocsThrd::TkbkModeNoChg ) )
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收对讲模式包。对讲模式为%uz8d不正确，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] );
								goto RecvPktOut;
							}

							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收对讲模式包。对讲索引：%uz8d。对讲模式：%z8s。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], g_TkbkModeU8strArrPt[ BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] ] );
						}
						else if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypAdoFrm ) //如果是音频输出帧包。
						{
							if( p_PktLenByt < 1 + 1 + 4 ) //如果音频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收音频输出帧包。长度为%uzd小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}

							//读取音频输出帧时间戳。
							p_TmpUint32 = ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

							if( p_PktLenByt > 1 + 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收有语音活动的音频输出帧包。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
							else //如果该音频输出帧为无语音活动。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收无语音活动的音频输出帧包。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
						}
						else if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypVdoFrm ) //如果是视频输出帧包。
						{
							if( p_PktLenByt < 1 + 1 + 4 ) //如果视频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收视频输出帧包。长度为%uzd小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}

							//读取视频输出帧时间戳。
							p_TmpUint32 = ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

							if( p_PktLenByt > 1 + 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收有图像活动的视频输出帧包。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收无图像活动的视频输出帧包。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
						}
						else if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == ClntMediaPocsThrd::PktTypExit ) //如果是退出包。
						{
							if( p_PktLenByt < 1 + 1 ) //如果退出包的长度小于1 + 1，表示没有对讲索引。
							{
								if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收退出包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}

							VstrFmtCpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：接收退出包。对讲索引：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowToastFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

							if( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 1 ] == p_CnctInfoTmpPt->m_MyTkbkIdx ) //如果对讲索引是我的对讲索引。
							{
								p_CnctInfoTmpPt->m_IsRecvExitPkt = 1; //设置已接收退出包。
								p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
							}
						}
					} //如果用本端客户端套接字接收远端服务端套接字发送的数据包超时，就重新接收。
				}
				else //如果用本端客户端套接字接收远端服务端套接字发送的数据包失败。
				{
					VstrFmtIns( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：用本端客户端套接字接收远端服务端套接字发送的数据包失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
					if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					BdctClntPt->m_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					
					p_CnctInfoTmpPt->m_CurCnctSts = ClntMediaPocsThrd::CnctStsTmot; //设置当前连接状态为异常断开。
					BdctClntPt->m_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt( BdctClntPt->m_ClntMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的广播客户端连接状态函数。
					p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
					goto RecvPktOut;
				}
				RecvPktOut:;
			}

			//销毁连接信息。
			if( p_CnctInfoTmpPt->m_IsRqstDstoy == 1 ) //如果该连接已请求销毁。
			{
				BdctClntCnctInfoDstoy( BdctClntPt, p_CnctInfoTmpPt );
			}
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//用户定义的读取音视频输入帧函数。
void BdctClntUserReadAdoVdoInptFrm( BdctClnt * BdctClntPt,
									short AdoInptPcmSrcFrmPt[], short AdoInptPcmRsltFrmPt[], long AdoInptPcmFrmLenUnit, int AdoInptPcmRsltFrmVoiceActSts,
									byte AdoInptEncdRsltFrmPt[], long AdoInptEncdRsltFrmLenByt, int AdoInptEncdRsltFrmIsNeedTrans,
									byte VdoInptNv21SrcFrmPt[], int VdoInptNv21SrcFrmWidthPt, int VdoInptNv21SrcFrmHeightPt, long VdoInptNv21SrcFrmLenByt,
									byte VdoInptYu12RsltFrmPt[], int VdoInptYu12RsltFrmWidth, int VdoInptYu12RsltFrmHeight, long VdoInptYu12RsltFrmLenByt,
									byte VdoInptEncdRsltFrmPt[], long VdoInptEncdRsltFrmLenByt )
{
	size_t p_PktLenByt = 0; //存放输入输出帧数据包的数据长度，单位字节。

	//发送音频输入帧。
	if( AdoInptPcmSrcFrmPt != NULL ) //如果有音频输入Pcm格式原始帧。
	{
		if( AdoInptEncdRsltFrmPt == NULL ) //如果没有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 ) //如果音频输入Pcm格式结果帧为有语音活动。
			{
				memcpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, AdoInptPcmRsltFrmPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 1 + 4 + BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳 + 音频输入Pcm格式帧。
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
				memcpy( BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt + 1 + 1 + 4, AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 1 + 4 + AdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳 + 音频输入已编码格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 1 + 4; //数据包长度 = 数据包类型 + 对讲索引 + 音频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			BdctClntPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypAdoFrm;
			//设置音频输入帧时间戳。
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			BdctClntAllCnctSendAdoPkt( BdctClntPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 0 );
			if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：发送有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d。总长度：%uzd。" ), BdctClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );

			BdctClntPt->m_LastSendAdoInptFrmIsAct = 1; //设置最后发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( BdctClntPt->m_LastSendAdoInptFrmIsAct != 0 ) //如果最后发送的音频输入帧为有语音活动，就发送。
			{
				BdctClntPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = ClntMediaPocsThrd::PktTypAdoFrm;
				//设置音频输入帧时间戳。
				BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = ( int8_t )( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
				BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 3 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 4 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt[ 5 ] = ( int8_t )( ( BdctClntPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				BdctClntAllCnctSendAdoPkt( BdctClntPt, BdctClntPt->m_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 0 );
				if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：发送无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d。总长度：%uzd。" ), BdctClntPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );

				BdctClntPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的音频输入帧无语音活动。
			}
			else //如果最后发送的音频输入帧为无语音活动，无需发送。
			{
				if( BdctClntPt->m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：广播客户端：本次音频输入帧为无语音活动，且最后发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}
}
