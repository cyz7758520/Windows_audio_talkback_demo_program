#include "BdctNtwk.h"
#include "NtwkMediaPocsThrd.h"

//连接信息初始化。
BdctNtwk::CnctInfo * BdctNtwkCnctInfoInit( BdctNtwk * BdctNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int CurCnctSts )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt = NULL;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( BdctNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );

	if( CQueuePutTail( BdctNtwkPt->m_CnctInfoCntnrPt, NULL, ( void * * )&p_CnctInfoTmpPt, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
	{
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：添加到连接信息容器失败。原因：%vs" ), BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	p_CnctInfoTmpPt->m_RmtNodeNameVstrPt = NULL;
	p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt = NULL;

	p_CnctInfoTmpPt->m_Num = p_ElmTotal; //设置序号。

	p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl; //设置协议为Tcp协议或高级Udp协议。
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt ) != 0 ) //设置远端套接字绑定的远端节点名称动态字符串的指针。
	{
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "网络媒体处理线程：广播网络：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt ) != 0 ) //设置远端套接字绑定的远端节点服务动态字符串的指针。
	{
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "网络媒体处理线程：广播网络：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	p_CnctInfoTmpPt->m_TcpClntSoktPt = TcpClntSoktPt; //设置本端Tcp协议客户端套接字的指针。
	p_CnctInfoTmpPt->m_AudpClntCnctIdx = AudpClntCnctIdx; //设置本端高级Udp协议客户端连接索引。
	p_CnctInfoTmpPt->m_IsRqstDstoy = 0; //设置是否请求销毁。

	p_CnctInfoTmpPt->m_CurCnctSts = CurCnctSts; //设置当前连接状态。
	p_CnctInfoTmpPt->m_RmtTkbkMode = NtwkMediaPocsThrd::TkbkModeNone; //设置远端对讲模式。

	p_CnctInfoTmpPt->m_IsRecvExitPkt = 0; //存放是否接收到退出包。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_CnctInfoTmpPt != NULL )
		{
			if( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt );
			if( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			CQueueDelTail( BdctNtwkPt->m_CnctInfoCntnrPt, NULL, 0, NULL );
			p_CnctInfoTmpPt = NULL;
		}
	}
	return p_CnctInfoTmpPt;
}

//连接信息发送数据包。
int BdctNtwkCnctInfoSendPkt( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( CnctInfoPt != NULL ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) ) //如果当前激活的连接信息的指针不为空，且当前连接状态为已连接。
	{
		//发送数据包。
		if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, ErrInfoVstrPt ) == 0 ) ) ||
			( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ) == 0 ) ) )
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
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) VstrCpy( ErrInfoVstrPt, Cu8vstr( "当前激活的连接信息的指针为空，或当前连接状态不为已连接。" ), , );
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
int BdctNtwkCnctInfoSendTkbkModePkt( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = ( byte )NtwkMediaPocsThrd::PktTypTkbkMode;
	BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( byte )LclTkbkMode;
	if( BdctNtwkCnctInfoSendPkt( BdctNtwkPt, CnctInfoPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 2, 1, 1, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：发送一个对讲模式包成功。对讲模式：%z8s" ), CnctInfoPt, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
	}
	else
	{
		VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：发送一个对讲模式包失败。原因：" ), CnctInfoPt );
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接信息销毁。
void BdctNtwkCnctInfoDstoy( BdctNtwk * BdctNtwkPt, BdctNtwk::CnctInfo * CnctInfoPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;

	if( CnctInfoPt != NULL )
	{
		//发送退出包。
		if( ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) ) //如果未接收到退出包，且当前连接状态为已连接。
		{
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypExit; //设置退出包。
			if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 0, 1, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 1, 1, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：发送一个退出包成功。" ), CnctInfoPt );
				if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else
			{
				VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：发送一个退出包失败。原因：" ), CnctInfoPt );
				if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		//销毁本端Tcp协议客户端套接字。
		if( CnctInfoPt->m_TcpClntSoktPt != NULL )
		{
			TcpClntDstoy( CnctInfoPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
			CnctInfoPt->m_TcpClntSoktPt = NULL;
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：销毁本端Tcp协议客户端套接字成功。" ), CnctInfoPt );
		}

		//销毁本端高级Udp协议客户端连接。
		if( CnctInfoPt->m_AudpClntCnctIdx != -1 )
		{
			AudpClosCnct( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, NULL );
			CnctInfoPt->m_AudpClntCnctIdx = -1;
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：销毁本端高级Udp协议客户端连接成功。" ), CnctInfoPt );
		}

		if( ( CnctInfoPt->m_IsSrvrOrClntCnct == 1 ) && ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsTmot ) ) //如果为客户端的连接，且未接收到退出包，且当前连接状态为异常断开，就重连。
		{
			CnctInfoPt->m_IsRqstDstoy = 0; //设置未请求销毁。

			CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsWait; //设置当前连接状态。
			CnctInfoPt->m_RmtTkbkMode = NtwkMediaPocsThrd::TkbkModeNone; //设置远端对讲模式。

			CnctInfoPt->m_IsRecvExitPkt = 0; //设置未接收到退出包。

			VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：连接异常断开，准备重连。" ), CnctInfoPt );
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		else //如果不为客户端的连接，或已接收到退出包，或当前连接状态不为异常断开，就销毁。
		{
			//设置当前连接状态。
			if( CnctInfoPt->m_CurCnctSts <= NtwkMediaPocsThrd::CnctStsWait )
			{
				CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsTmot;
			}
			else if( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct )
			{
				CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsDsct;
			}
			
			VstrDstoy( CnctInfoPt->m_RmtNodeNameVstrPt ); //销毁远端套接字绑定的远端节点名称动态字符串。
			VstrDstoy( CnctInfoPt->m_RmtNodeSrvcVstrPt ); //销毁远端套接字绑定的远端节点服务动态字符串。

			//从连接信息容器删除。
			for( int p_CnctInfoLstNum = CnctInfoPt->m_Num + 1; CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
			{
				p_CnctInfoTmpPt->m_Num--; //设置后面的连接信息的序号全部递减1。
			}
			CQueueDelByNum( BdctNtwkPt->m_CnctInfoCntnrPt, CnctInfoPt->m_Num, 0, NULL );

			VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：已销毁。" ), CnctInfoPt );
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//NtwkMediaPocsThrdIsAutoRqirExit( BdctNtwkPt->m_NtwkMediaPocsThrdPt, NULL ); //判断是否自动请求退出。这里不需要判断，因为有多少个广播连接就会请求多少次，没有必要，只需要在连接信息全部销毁时请求一次即可。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接信息全部销毁。
void BdctNtwkCnctInfoAllDstoy( BdctNtwk * BdctNtwkPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;

	while( CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, 0, NULL, ( void ** )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0 ) BdctNtwkCnctInfoDstoy( BdctNtwkPt, p_CnctInfoTmpPt );

	NtwkMediaPocsThrdIsAutoRqirExit( BdctNtwkPt->m_NtwkMediaPocsThrdPt, NULL ); //判断是否自动请求退出。在没有广播连接时需要这一步判断。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//连接初始化。
void BdctNtwkCnctInit( BdctNtwk * BdctNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;
	int p_CmpRslt; //存放比较结果。

	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		if( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == IsTcpOrAudpPrtcl ) &&
			( VstrCmp( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, RmtNodeNameVstrPt, &p_CmpRslt ),p_CmpRslt == 0 ) &&
			( VstrCmp( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, RmtNodeSrvcVstrPt, &p_CmpRslt ),p_CmpRslt == 0 ) )
		{
			VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：已存在与远端节点%vs[%vs:%vs]的连接，无需重复连接。" ), ( ( IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
	}

	if( ( p_CnctInfoTmpPt = BdctNtwkCnctInfoInit( BdctNtwkPt, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, NULL, -1, NtwkMediaPocsThrd::CnctStsWait ) ) == NULL ) goto Out; //如果连接信息初始化失败。

	//Ping一下远程节点名称，这样可以快速获取ARP条目。
	VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), RmtNodeNameVstrPt );
	WinExec( ( char * )BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt->m_Pt, SW_HIDE );

	VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：初始化与远端节点%vs[%vs:%vs]的连接成功。" ), ( ( IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
	if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//所有连接发送音频数据包。
void BdctNtwkAllCnctSendAdoPkt( BdctNtwk * BdctNtwkPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;

	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		if( ( p_CnctInfoTmpPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) &&
			( ( p_CnctInfoTmpPt->m_RmtTkbkMode == NtwkMediaPocsThrd::TkbkModeAdo ) || ( p_CnctInfoTmpPt->m_RmtTkbkMode == NtwkMediaPocsThrd::TkbkModeAdoVdo ) ) )
		{
			//发送数据包。
			if( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, NULL );
			else AudpSendApkt( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, NULL );
		}
	}
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接销毁。
void BdctNtwkCnctDstoy( BdctNtwk * BdctNtwkPt, int CnctNum )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( BdctNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
	if( ( CnctNum >= p_ElmTotal ) || ( CnctNum < 0 ) )
	{
		VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：没有序号为%d的连接，无法删除。" ), CnctNum );
		if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	
	CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, CnctNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL );
	p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
	
	VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：请求销毁远端节点%vs[%vs:%vs]的连接。" ), p_CnctInfoTmpPt, ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
	if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接处理，包括接受连接、连接服务端、接收数据包、删除连接。
void BdctNtwkCnctPocs( BdctNtwk * BdctNtwkPt )
{
	BdctNtwk::CnctInfo * p_CnctInfoTmpPt;
	uint32_t p_TmpUint32;

	//遍历连接信息容器。
	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( BdctNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		//用本端客户端套接字连接远端服务端套接字。
		if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts <= NtwkMediaPocsThrd::CnctStsWait ) ) //如果该连接未请求销毁，且当前连接状态为等待远端接受连接。
		{
			int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。

			{ //设置远端节点的地址族。
				ADDRINFOW * p_AddrInfoListPt;
				if( GetAddrInfo( ( PCWSTR )p_CnctInfoTmpPt->m_RmtNodeNameVstrPt->m_Pt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
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
					if( -p_CnctInfoTmpPt->m_CurCnctSts >= BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MaxCnctTimes ) //如果未达到最大连接次数。
					{
						p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpClntSoktCnctOut;
					}

					p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
					
					VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：开始第 %d 次连接。" ), p_CnctInfoTmpPt, -p_CnctInfoTmpPt->m_CurCnctSts );
					if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

					if( TcpClntInit( &p_CnctInfoTmpPt->m_TcpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, NULL, NULL, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字失败。
                    {
						VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                        goto TcpClntSoktCnctOut;
                    }
				}
				
                if( TcpClntWaitCnct( p_CnctInfoTmpPt->m_TcpClntSoktPt, 0, &p_TcpCnctSts, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字成功。
                {
                    if( p_TcpCnctSts == TcpCnctStsWait ) //如果等待远端接受连接。
                    {
                        //继续等待本端本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
                    }
                    else if( p_TcpCnctSts == TcpCnctStsCnct ) //如果连接成功。
                    {
						if( TcpClntSetNoDelay( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
						{
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetSendBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetRecvBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetKeepAlive( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 1, 1, 5, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

                        if( TcpClntGetLclAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
                        {
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：获取本端Tcp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
                        }

                        if( TcpClntGetRmtAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
                        {
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：获取本端Tcp协议客户端套接字连接的远端Tcp协议客户端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
                        }

                        p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
                        BdctNtwkCnctInfoSendTkbkModePkt( BdctNtwkPt, p_CnctInfoTmpPt, NtwkMediaPocsThrd::TkbkModeAdo ); //发送对讲模式包。

						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：初始化本端Tcp协议客户端套接字[%vs:%vs]，并连接远端Tcp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                    }
                    else //如果连接失败。
                    {
						VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                        goto TcpClntSoktCnctOut;
                    }
                }
                else //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。
                {
					VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。原因：" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
					if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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

                if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt == NULL ) //如果未初始化本端高级Udp协议客户端套接字。
                {
                    if( AudpInit( &BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, NULL, NULL, 0, 5000, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议客户端套接字成功。
                    {
                        if( AudpGetLclAddr( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, NULL, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
                        {
							VstrIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：获取本端高级Udp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
                        }

						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：初始化本端高级Udp协议客户端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                    }
                    else //如果初始化本端高级Udp协议客户端套接字失败。
                    {
						VstrIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：初始化本端高级Udp协议客户端套接字失败。原因：" ) );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
                    }
                }

				if( p_CnctInfoTmpPt->m_AudpClntCnctIdx == SIZE_MAX )
				{
					if( -p_CnctInfoTmpPt->m_CurCnctSts >= BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
					{
						p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}

					p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
					
					VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：开始第 %d 次连接。" ), p_CnctInfoTmpPt, -p_CnctInfoTmpPt->m_CurCnctSts );
					if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

					if( AudpCnct( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, &p_CnctInfoTmpPt->m_AudpClntCnctIdx, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字失败。
					{
						VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				if( AudpWaitCnct( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, 0, &p_AudpCnctSts, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //循环等待本端高级Udp协议套接字连接远端成功。
				{
					if( p_AudpCnctSts == AudpCnctStsWait ) //如果等待远端接受连接。
					{
						//重新循环，继续等待本端高级Udp协议套接字连接远端。
					}
					else if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
					{
						if( AudpGetRmtAddr( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：获取本端高级Udp协议客户端套接字连接的远端高级Udp协议服务端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}

						p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
                        BdctNtwkCnctInfoSendTkbkModePkt( BdctNtwkPt, p_CnctInfoTmpPt, NtwkMediaPocsThrd::TkbkModeAdo ); //发送对讲模式包。

						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
					else if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
					{
						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接超时。" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
					else if( p_AudpCnctSts == AudpCnctStsDsct ) //如果连接断开。
					{
						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接断开。" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				p_PocsRslt = 0; //设置本处理段执行成功。

				AudpClntSoktCnctOut:
				if( p_PocsRslt != 0 ) //如果本处理段执行失败。
				{
					if( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt != NULL ) && ( p_CnctInfoTmpPt->m_AudpClntCnctIdx != SIZE_MAX ) )
					{
						AudpClosCnct( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL );
						p_CnctInfoTmpPt->m_AudpClntCnctIdx = SIZE_MAX;
					}
				}
			}
		}

		//用本端客户端套接字接收远端服务端套接字发送过来的一个数据包。
		if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) ) //如果该连接未请求销毁，且当前连接状态为已连接。
		{
			size_t p_PktLenByt;
			uint32_t p_TmpUint32;
			size_t p_TmpElmTotal;

			if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntRecvApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByteSz, &p_PktLenByt, 0, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpRecvApkt( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByteSz, &p_PktLenByt, NULL, 0, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
				{
					if( p_PktLenByt == 0 ) //如果数据包的数据长度为0。
					{
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个数据包的数据长度为%uzd，表示没有数据，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
						goto RecvPktOut;
					}
					else if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypTkbkMode ) //如果是对讲模式包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果音频输出帧包的数据长度小于1 + 1，表示没有对讲模式。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个对讲模式包的数据长度为%uzd小于1 + 1，表示没有对讲模式，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] >= NtwkMediaPocsThrd::TkbkModeNoChg )
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个对讲模式包的对讲模式为%z8d不正确，无法继续接收。" ), p_CnctInfoTmpPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] );
							goto RecvPktOut;
						}

						p_CnctInfoTmpPt->m_RmtTkbkMode = BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ]; //设置远端对讲模式。
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个对讲模式包。对讲模式：%z8s" ), p_CnctInfoTmpPt, g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ] );
					}
					else if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypAdoFrm ) //如果是音频输出帧包。
					{
						if( p_PktLenByt < 1 + 4 ) //如果音频输出帧包的数据长度小于1 + 4，表示没有音频输出帧时间戳。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个音频输出帧包的数据长度为%uzd小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						//读取音频输出帧时间戳。
						p_TmpUint32 = ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] & 0xFF ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] & 0xFF ) << 8 ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] & 0xFF ) << 16 ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] & 0xFF ) << 24 );

						if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个有语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
						}
						else //如果该音频输出帧为无语音活动。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个无语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
						}
					}
					else if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypVdoFrm ) //如果是视频输出帧包。
					{
						if( p_PktLenByt < 1 + 4 ) //如果视频输出帧包的数据长度小于1 + 4，表示没有视频输出帧时间戳。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个视频输出帧包的数据长度为%uzd小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						//读取视频输出帧时间戳。
						p_TmpUint32 = ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] & 0xFF ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] & 0xFF ) << 8 ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] & 0xFF ) << 16 ) + ( ( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] & 0xFF ) << 24 );

						if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个有图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
						}
						else //如果该视频输出帧为无图像活动。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个无图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
						}
					}
					else if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypExit ) //如果是退出包。
					{
						if( p_PktLenByt > 1 ) //如果退出包的数据长度大于1。
						{
							if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个退出包的数据长度为%uzd大于1，表示还有其他数据，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						p_CnctInfoTmpPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
						p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

						VstrFmtCpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：接收到一个退出包。" ), p_CnctInfoTmpPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowToastFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				} //如果用本端套接字接收一个连接的远端套接字发送的数据包超时，就重新接收。
			}
			else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
			{
				p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsTmot; //设置当前连接状态为异常断开。
				p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

				VstrFmtIns( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：广播网络：连接%P：用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ), p_CnctInfoTmpPt );
				if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( BdctNtwkPt->m_NtwkMediaPocsThrdPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto RecvPktOut;
			}
			RecvPktOut:;
		}

		//销毁连接信息。
		if( p_CnctInfoTmpPt->m_IsRqstDstoy == 1 ) //如果该连接已请求销毁。
		{
			BdctNtwkCnctInfoDstoy( BdctNtwkPt, p_CnctInfoTmpPt );
			p_CnctInfoLstNum--;
		}
	}
}

//用户定义的读取音视频输入帧函数。
void BdctNtwkUserReadAdoVdoInptFrm( BdctNtwk * BdctNtwkPt,
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
				memcpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, AdoInptPcmRsltFrmPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入Pcm格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动。
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}
		else //如果有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 && AdoInptEncdRsltFrmIsNeedTrans != 0 ) //如果音频输入Pcm格式结果帧为有语音活动，且音频输入已编码格式结果帧需要传输。
			{
				memcpy( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + AdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入已编码格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypAdoFrm;
			//设置音频输入帧时间戳。
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( int8_t )( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			BdctNtwkAllCnctSendAdoPkt( BdctNtwkPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, p_PktLenByt, 1, 0 );
			if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%uzd。" ), BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );

			BdctNtwkPt->m_LastSendAdoInptFrmIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( BdctNtwkPt->m_LastSendAdoInptFrmIsAct != 0 ) //如果最后一个发送的音频输入帧为有语音活动，就发送。
			{
				BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypAdoFrm;
				//设置音频输入帧时间戳。
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( int8_t )( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] = ( int8_t )( ( BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				BdctNtwkAllCnctSendAdoPkt( BdctNtwkPt, BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, p_PktLenByt, 1, 0 );
				if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%uzd。" ), BdctNtwkPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );

				BdctNtwkPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
			}
			else //如果最后一个发送的音频输入帧为无语音活动，无需发送。
			{
				if( BdctNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：广播网络：本次音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}
}
