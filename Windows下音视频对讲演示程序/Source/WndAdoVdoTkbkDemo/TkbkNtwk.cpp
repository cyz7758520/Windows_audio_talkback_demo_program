#include "TkbkNtwk.h"
#include "NtwkMediaPocsThrd.h"

//连接信息初始化。
TkbkNtwk::CnctInfo * TkbkNtwkCnctInfoInit( TkbkNtwk * TkbkNtwkPt, int IsSrvrOrClntCnct, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int CurCnctSts )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt = NULL;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( TkbkNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
	if( p_ElmTotal < TkbkNtwkPt->m_MaxCnctNum ) //如果未达到最大连接数。
	{
		if( CQueuePutTail( TkbkNtwkPt->m_CnctInfoCntnrPt, NULL, ( void * * )&p_CnctInfoTmpPt, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：添加到连接信息容器失败。原因：%vs" ), TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_CnctInfoTmpPt->m_RmtNodeNameVstrPt = NULL;
		p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt = NULL;

		p_CnctInfoTmpPt->m_Num = p_ElmTotal; //设置序号。

		p_CnctInfoTmpPt->m_IsSrvrOrClntCnct = IsSrvrOrClntCnct; //设置是否是服务端或客户端的连接。
		p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl; //设置协议为Tcp协议或高级Udp协议。
		if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt ) != 0 ) //设置远端套接字绑定的远端节点名称动态字符串的指针。
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "网络媒体处理线程：对讲网络：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
			goto Out;
		}
		if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt ) != 0 ) //设置远端套接字绑定的远端节点服务动态字符串的指针。
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "网络媒体处理线程：对讲网络：创建远端套接字绑定的远端节点名称动态字符串失败。" ) );
			goto Out;
		}
		p_CnctInfoTmpPt->m_TcpClntSoktPt = TcpClntSoktPt; //设置本端Tcp协议客户端套接字的指针。
		p_CnctInfoTmpPt->m_AudpClntCnctIdx = AudpClntCnctIdx; //设置本端高级Udp协议客户端连接索引。
		p_CnctInfoTmpPt->m_IsRqstDstoy = 0; //设置是否请求销毁。

		p_CnctInfoTmpPt->m_CurCnctSts = CurCnctSts; //设置当前连接状态。
		p_CnctInfoTmpPt->m_LclTkbkMode = NtwkMediaPocsThrd::TkbkModeNone; //设置本端对讲模式。
		p_CnctInfoTmpPt->m_RmtTkbkMode = NtwkMediaPocsThrd::TkbkModeNone; //设置远端对讲模式。

		p_CnctInfoTmpPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
		p_CnctInfoTmpPt->m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
		p_CnctInfoTmpPt->m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
		p_CnctInfoTmpPt->m_IsRecvExitPkt = 0; //存放是否接收到退出包。

		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctInitFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt ); //调用用户定义的连接添加函数。
	}
	else //如果已达到最大连接数。
	{
		VstrCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：已达到最大连接数，无法添加连接信息。" ), , );
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ShowToastWndHdl, 3000, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_CnctInfoTmpPt != NULL )
		{
			if( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt );
			if( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			CQueueDelTail( TkbkNtwkPt->m_CnctInfoCntnrPt, NULL, 0, NULL );
			p_CnctInfoTmpPt = NULL;
		}
	}
	return p_CnctInfoTmpPt;
}

//连接信息发送数据包。
int TkbkNtwkCnctInfoSendPkt( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, const void * PktPt, size_t PktLenByt, uint32_t Times, int IsRlab, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( ( CnctInfoPt != NULL ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) ) //如果当前激活的连接信息的指针不为空，且当前连接状态为已连接。
	{
		//发送数据包。
		if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, PktPt, PktLenByt, 0, Times, 0, ErrInfoVstrPt ) == 0 ) ) ||
			( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( CnctInfoPt->m_IsSrvrOrClntCnct == 0 ) && ( AudpSendApkt( TkbkNtwkPt->m_AudpSrvrSoktPt, CnctInfoPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ) == 0 ) ) ||
			( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( CnctInfoPt->m_IsSrvrOrClntCnct == 1 ) && ( AudpSendApkt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, PktPt, PktLenByt, Times, IsRlab, ErrInfoVstrPt ) == 0 ) ) )
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
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) VstrCpy( ErrInfoVstrPt, Cu8vstr( "当前激活的连接信息的指针为空，或当前连接状态不为已连接。" ), , );
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
int TkbkNtwkCnctInfoSendTkbkModePkt( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = ( byte )NtwkMediaPocsThrd::PktTypTkbkMode;
	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( byte )LclTkbkMode;
	if( TkbkNtwkCnctInfoSendPkt( TkbkNtwkPt, CnctInfoPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 2, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
	{
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：发送一个对讲模式包成功。对讲模式：%z8s" ), CnctInfoPt, g_TkbkModeU8strArrPt[ LclTkbkMode ] );
	}
	else
	{
		VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：发送一个对讲模式包失败。原因：" ), CnctInfoPt );
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接信息更新本端对讲模式。
void TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt, int LclTkbkMode )
{
	CnctInfoPt->m_LclTkbkMode = LclTkbkMode;
	if( TkbkNtwkPt->m_CurActCnctInfoPt == CnctInfoPt ) //如果该连接信息为当前激活的连接信息。
	{
		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 0, CnctInfoPt->m_LclTkbkMode );
	}
	else //如果该连接信息不为当前激活的连接信息。
	{
		if( CnctInfoPt->m_LclTkbkMode != NtwkMediaPocsThrd::TkbkModeNone ) //如果该连接信息的本端对讲模式不为挂起。
		{
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 1, CnctInfoPt->m_LclTkbkMode );
		}
		else //如果该连接信息的本端对讲模式为挂起。
		{
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 0, CnctInfoPt->m_LclTkbkMode );
		}
	}
	TkbkNtwkCnctInfoSendTkbkModePkt( TkbkNtwkPt, CnctInfoPt, CnctInfoPt->m_LclTkbkMode ); //发送对讲模式包。
}

//连接信息全部更新本端对讲模式。
void TkbkNtwkCnctInfoAllUpdtLclTkbkMode( TkbkNtwk * TkbkNtwkPt, int LclTkbkMode )
{
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, LclTkbkMode );
	}
}

//连接信息更新远端对讲模式。
void TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt )
{
    if( TkbkNtwkPt->m_CurActCnctInfoPt == CnctInfoPt ) //如果该连接信息为当前激活的连接信息。
    {
        TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 0, CnctInfoPt->m_RmtTkbkMode );
    }
    else //如果该连接信息不为当前激活的连接信息。
    {
        if( CnctInfoPt->m_RmtTkbkMode != NtwkMediaPocsThrd::TkbkModeNone ) //如果该连接信息的远端对讲模式不为挂起。
        {
            TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 1, CnctInfoPt->m_RmtTkbkMode );
        }
        else //如果该连接信息的远端对讲模式为挂起。
        {
            TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, 0, CnctInfoPt->m_RmtTkbkMode );
        }
    }
}

//连接信息全部更新远端对讲模式。
void TkbkNtwkCnctInfoAllUpdtRmtTkbkMode( TkbkNtwk * TkbkNtwkPt )
{
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

    for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
    {
        TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt );
    }
}

//连接信息激活。
void TkbkNtwkCnctInfoAct( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt )
{
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

    if( TkbkNtwkPt->m_CurActCnctInfoPt != CnctInfoPt ) //如果当前激活的连接信息与要激活的连接信息不一致。
    {
        if( TkbkNtwkPt->m_CurActCnctInfoPt != NULL ) //如果当前激活的连接信息不为空。
        {
            p_CnctInfoTmpPt = TkbkNtwkPt->m_CurActCnctInfoPt;
            TkbkNtwkPt->m_CurActCnctInfoPt = NULL; //设置当前激活的连接信息。
            TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctActFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, 0 ); //调用用户定义的连接激活函数。
            if( p_CnctInfoTmpPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct )
            {
                TkbkNtwkCnctInfoSendTkbkModePkt( TkbkNtwkPt, p_CnctInfoTmpPt, NtwkMediaPocsThrd::TkbkModeNone ); //发送对讲模式包。
                TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt ); //连接信息更新远端对讲模式。
            }
        }

        if( CnctInfoPt != NULL ) //如果要激活的连接信息不为空。
        {
            TkbkNtwkPt->m_CurActCnctInfoPt = CnctInfoPt;
            TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctActFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_CurActCnctInfoPt, 1 ); //调用用户定义的连接激活函数。
            if( TkbkNtwkPt->m_CurActCnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, TkbkNtwkPt->m_CurActCnctInfoPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //如果连接状态为已连接，就连接信息更新本端对讲模式。
            TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, TkbkNtwkPt->m_CurActCnctInfoPt ); //连接信息更新远端对讲模式。
            NtwkMediaPocsThrdSetTkbkMode( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。

            TkbkNtwkRecvOtptFrmReset( TkbkNtwkPt ); //接收输出帧重置。

			VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：连接激活。" ), CnctInfoPt );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

            //挂起全部非激活连接信息。
			for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
			{
				if( p_CnctInfoTmpPt != TkbkNtwkPt->m_CurActCnctInfoPt )
                {
                    TkbkNtwkCnctInfoPend( TkbkNtwkPt, p_CnctInfoTmpPt );
                }
			}
        }
        else //如果要激活的连接信息为空。
        {
            NtwkMediaPocsThrdSetTkbkMode( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。

			VstrCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：无连接激活。" ), , );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
        }
    }
}

//连接信息挂起。
void TkbkNtwkCnctInfoPend( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt )
{
	if( CnctInfoPt->m_LclTkbkMode != NtwkMediaPocsThrd::TkbkModeNone ) //如果连接信息当前不为挂起。
	{
		TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, CnctInfoPt, NtwkMediaPocsThrd::TkbkModeNone );
	}
}

//连接信息销毁。
void TkbkNtwkCnctInfoDstoy( TkbkNtwk * TkbkNtwkPt, TkbkNtwk::CnctInfo * CnctInfoPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	if( CnctInfoPt != NULL )
	{
		//发送退出包。
		if( ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct ) ) //如果未接收到退出包，且当前连接状态为已连接。
		{
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypExit; //设置退出包。
			if( ( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( CnctInfoPt->m_TcpClntSoktPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 0, 1, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( CnctInfoPt->m_IsSrvrOrClntCnct == 0 ) && ( AudpSendApkt( TkbkNtwkPt->m_AudpSrvrSoktPt, CnctInfoPt->m_AudpClntCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( CnctInfoPt->m_IsTcpOrAudpPrtcl == 1 ) && ( CnctInfoPt->m_IsSrvrOrClntCnct == 1 ) && ( AudpSendApkt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：发送一个退出包成功。" ), CnctInfoPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else
			{
				VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：发送一个退出包失败。原因：" ), CnctInfoPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}

		//销毁本端Tcp协议客户端套接字。
		if( CnctInfoPt->m_TcpClntSoktPt != NULL )
		{
			TcpClntDstoy( CnctInfoPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
			CnctInfoPt->m_TcpClntSoktPt = NULL;
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：销毁本端Tcp协议客户端套接字成功。" ), CnctInfoPt );
		}

		//销毁本端高级Udp协议客户端连接。
		if( CnctInfoPt->m_AudpClntCnctIdx != -1 )
		{
			if( CnctInfoPt->m_IsSrvrOrClntCnct == 0 ) AudpClosCnct( TkbkNtwkPt->m_AudpSrvrSoktPt, CnctInfoPt->m_AudpClntCnctIdx, NULL );
			else AudpClosCnct( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, CnctInfoPt->m_AudpClntCnctIdx, NULL );
			CnctInfoPt->m_AudpClntCnctIdx = -1;
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：销毁本端高级Udp协议客户端连接成功。" ), CnctInfoPt );
		}

		if( ( CnctInfoPt->m_IsSrvrOrClntCnct == 1 ) && ( CnctInfoPt->m_IsRecvExitPkt == 0 ) && ( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsTmot ) ) //如果为客户端的连接，且未接收到退出包，且当前连接状态为异常断开，就重连。
		{
			CnctInfoPt->m_IsRqstDstoy = 0; //设置未请求销毁。

			CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsWait; //设置当前连接状态。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
			CnctInfoPt->m_RmtTkbkMode = NtwkMediaPocsThrd::TkbkModeNone; //设置远端对讲模式。

			CnctInfoPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
			CnctInfoPt->m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
			CnctInfoPt->m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
			CnctInfoPt->m_IsRecvExitPkt = 0; //设置未接收到退出包。

			if( CnctInfoPt == TkbkNtwkPt->m_CurActCnctInfoPt ) TkbkNtwkRecvOtptFrmReset( TkbkNtwkPt ); //如果为当前激活的连接信息，接收输出帧重置。

			VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：连接异常断开，准备重连。" ), CnctInfoPt );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		else //如果不为客户端的连接，或已接收到退出包，或当前连接状态不为异常断开，就销毁。
		{
			int p_IsCurActCnctInfo = ( CnctInfoPt == TkbkNtwkPt->m_CurActCnctInfoPt ) ? 1 : 0; //存放是否是当前激活的连接信息。

			//设置当前连接状态。
			if( CnctInfoPt->m_CurCnctSts <= NtwkMediaPocsThrd::CnctStsWait )
			{
				CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsTmot;
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
			}
			else if( CnctInfoPt->m_CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct )
			{
				CnctInfoPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsDsct;
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
			}

			if( p_IsCurActCnctInfo != 0 ) TkbkNtwkCnctInfoAct( TkbkNtwkPt, NULL ); //如果是当前激活的连接信息，就激活空连接信息。

			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctDstoyFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, CnctInfoPt ); //调用用户定义的连接销毁函数。

			VstrDstoy( CnctInfoPt->m_RmtNodeNameVstrPt ); //销毁远端套接字绑定的远端节点名称动态字符串。
			VstrDstoy( CnctInfoPt->m_RmtNodeSrvcVstrPt ); //销毁远端套接字绑定的远端节点服务动态字符串。

			//从连接信息容器删除。
			for( int p_CnctInfoLstNum = CnctInfoPt->m_Num + 1; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
			{
				p_CnctInfoTmpPt->m_Num--; //设置后面的连接信息的序号全部递减1。
			}
			CQueueDelByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, CnctInfoPt->m_Num, 0, NULL );

			VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：已销毁。" ), CnctInfoPt );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

			//连接信息激活。
			CQueueGetTotal( TkbkNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
			if( ( p_IsCurActCnctInfo != 0 ) && //如果是当前激活的连接信息。
				( ( TkbkNtwkPt->m_IsAutoActCnct == 1 ) && ( p_ElmTotal > 0 ) ) ) //如果是自动激活连接，且还有其他连接信息。
			{
				{
					//激活请求激活的连接信息。
					for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
					{
						if( p_CnctInfoTmpPt->m_RmtTkbkMode != NtwkMediaPocsThrd::TkbkModeNone )
						{
							TkbkNtwkCnctInfoAct( TkbkNtwkPt, p_CnctInfoTmpPt );
							goto ActRqirActCnctInfoOut;
						}
					}

					TkbkNtwkCnctInfoAllUpdtLclTkbkMode( TkbkNtwkPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //如果没有请求激活的连接信息，就连接信息全部更新本端对讲模式。
				}
				ActRqirActCnctInfoOut:;
			}

			NtwkMediaPocsThrdIsAutoRqirExit( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, NULL ); //判断是否自动请求退出。
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
}

//连接信息全部销毁。
void TkbkNtwkCnctInfoAllDstoy( TkbkNtwk * TkbkNtwkPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

	while( CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, 0, NULL, ( void ** )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0 ) TkbkNtwkCnctInfoDstoy( TkbkNtwkPt, p_CnctInfoTmpPt );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
    if( p_Rslt != 0 ) //如果本函数执行失败。
    {

    }
	return;
}

//服务端初始化。
int TkbkNtwkSrvrInit( TkbkNtwk * TkbkNtwkPt, Vstr * SrvrUrlVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * p_SrvrPrtclVstrPt = NULL; //存放服务端协议动态字符串的指针。
	Vstr * p_SrvrNodeNameVstrPt = NULL; //存放服务端节点名称动态字符串的指针。
	Vstr * p_SrvrNodeSrvcVstrPt = NULL; //存放服务端节点服务动态字符串的指针。
	int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。
	int p_CmpRslt; //存放比较结果。

	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserSrvrInitFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt ); //调用用户定义的服务端初始化函数。

	if( TkbkNtwkPt->m_SrvrIsInit == 0 ) //如果服务端未初始化。
	{
		if( VstrInit( &p_SrvrPrtclVstrPt, Utf16, , ) != 0 )
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "创建服务端协议动态字符串失败。" ) );
			goto Out;
		}
		if( VstrInit( &p_SrvrNodeNameVstrPt, Utf16, , ) != 0 )
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "创建服务端节点服务动态字符串失败。" ) );
			goto Out;
		}
		if( VstrInit( &p_SrvrNodeSrvcVstrPt, Utf16, , ) != 0 )
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "创建服务端节点服务动态字符串失败。" ) );
			goto Out;
		}

		TkbkNtwkPt->m_SrvrIsInit = 1; //设置服务端已初始化。

		//解析服务端Url动态字符串。
		if( VstrUrlParse( SrvrUrlVstrPt, p_SrvrPrtclVstrPt, NULL, NULL, p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt, NULL, NULL, NULL, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：解析服务端Url动态字符串失败。原因：" ) );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		if( ( VstrCmp( p_SrvrPrtclVstrPt, Cu8vstr( "Tcp" ), &p_CmpRslt ),p_CmpRslt != 0 ) && ( VstrCmp( p_SrvrPrtclVstrPt, Cu8vstr( "Audp" ), &p_CmpRslt ),p_CmpRslt != 0 ) )
		{
			VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：服务端Url动态字符串的协议不正确。" ) );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		if( VstrCmp( p_SrvrNodeSrvcVstrPt, Cu8vstr( "" ), &p_CmpRslt ),p_CmpRslt == 0 )
		{
			VstrCpy( p_SrvrNodeSrvcVstrPt, Cu8vstr( "12345" ), , );
		}

		{ //设置远端节点的地址族。
			ADDRINFOW * p_AddrInfoListPt;
			if( GetAddrInfo( ( PCWSTR )p_SrvrNodeNameVstrPt->m_Pt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
			{
				p_RmtNodeAddrFamly = ( p_AddrInfoListPt->ai_family == AF_INET ) ? 4 : ( p_AddrInfoListPt->ai_family == AF_INET6 ) ? 6 : 0;
				FreeAddrInfo( p_AddrInfoListPt );
			}
			else p_RmtNodeAddrFamly = 0;
		}

		if( VstrCmp( p_SrvrPrtclVstrPt, Cu8vstr( "Tcp" ), &p_CmpRslt ),p_CmpRslt == 0 ) //如果要使用Tcp协议。
		{
			if( TcpSrvrInit( &TkbkNtwkPt->m_TcpSrvrSoktPt, p_RmtNodeAddrFamly, p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议服务端套接字成功。
			{
				if( TcpSrvrGetLclAddr( TkbkNtwkPt->m_TcpSrvrSoktPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端TCP协议服务端套接字[%vs:%vs]成功。" ), TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else
			{
				VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
		}
		else //如果要使用高级Udp协议。
		{
			if( AudpInit( &TkbkNtwkPt->m_AudpSrvrSoktPt, p_RmtNodeAddrFamly, p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt, ( short )1, ( short )5000, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议服务端套接字成功。
			{
				if( AudpSetSendBufSz( TkbkNtwkPt->m_AudpSrvrSoktPt, 1024 * 1024, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
				{
					VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				if( AudpSetRecvBufSz( TkbkNtwkPt->m_AudpSrvrSoktPt, 1024 * 1024, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
				{
					VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				if( AudpGetLclAddr( TkbkNtwkPt->m_AudpSrvrSoktPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：获取本端高级Udp协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端高级Udp协议服务端套接字[%vs:%vs]成功。" ), p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
			else //如果初始化本端高级Udp协议服务端套接字失败。
			{
				VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), p_SrvrNodeNameVstrPt, p_SrvrNodeSrvcVstrPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_SrvrPrtclVstrPt != NULL ) VstrDstoy( p_SrvrPrtclVstrPt );
	if( p_SrvrNodeNameVstrPt != NULL ) VstrDstoy( p_SrvrNodeNameVstrPt );
	if( p_SrvrNodeSrvcVstrPt != NULL ) VstrDstoy( p_SrvrNodeSrvcVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		TkbkNtwkSrvrDstoy( TkbkNtwkPt );
	}
	return p_Rslt;
}

//服务端销毁。
void TkbkNtwkSrvrDstoy( TkbkNtwk * TkbkNtwkPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserSrvrDstoyFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt ); //调用用户定义的服务端销毁函数。

	if( TkbkNtwkPt->m_SrvrIsInit != 0 ) //如果服务端已初始化。
	{
		//删除所有的服务端连接。
		for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
		{
			if( p_CnctInfoTmpPt->m_IsSrvrOrClntCnct == 0 )
			{
				TkbkNtwkCnctInfoDstoy( TkbkNtwkPt, p_CnctInfoTmpPt );
				p_CnctInfoLstNum--;
			}
		}

		//销毁本端Tcp协议服务端套接字。
		if( TkbkNtwkPt->m_TcpSrvrSoktPt != NULL )
		{
			TcpSrvrDstoy( TkbkNtwkPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁本端Tcp协议服务端套接字。
			TkbkNtwkPt->m_TcpSrvrSoktPt = NULL;

			VstrCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：关闭并销毁本端Tcp协议服务端套接字成功。" ), , );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//销毁本端高级Udp协议服务端套接字。
		if( TkbkNtwkPt->m_AudpSrvrSoktPt != NULL )
		{
			AudpDstoy( TkbkNtwkPt->m_AudpSrvrSoktPt, NULL ); //关闭并销毁本端高级Udp协议服务端套接字。
			TkbkNtwkPt->m_AudpSrvrSoktPt = NULL;

			VstrCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：关闭并销毁本端高级Udp协议服务端套接字成功。" ), , );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		TkbkNtwkPt->m_SrvrIsInit = 0; //设置服务端未初始化。
	}

	NtwkMediaPocsThrdIsAutoRqirExit( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, NULL ); //判断是否自动请求退出。
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接初始化。
void TkbkNtwkCnctInit( TkbkNtwk * TkbkNtwkPt, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;
	int p_CmpRslt; //存放比较结果。

	for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
	{
		if( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == IsTcpOrAudpPrtcl ) &&
			( VstrCmp( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, RmtNodeNameVstrPt, &p_CmpRslt ),p_CmpRslt == 0 ) &&
			( VstrCmp( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, RmtNodeSrvcVstrPt, &p_CmpRslt ),p_CmpRslt == 0 ) )
		{
			VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：已存在与远端节点%vs[%vs:%vs]的连接，无需重复连接。" ), ( ( IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
	}

	if( ( p_CnctInfoTmpPt = TkbkNtwkCnctInfoInit( TkbkNtwkPt, 1, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt, NULL, -1, NtwkMediaPocsThrd::CnctStsWait ) ) == NULL ) goto Out; //如果连接信息初始化失败。
	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。

	//Ping一下远程节点名称，这样可以快速获取ARP条目。
	VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), RmtNodeNameVstrPt );
	WinExec( ( char * )TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt->m_Pt, SW_HIDE );

	VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：初始化与远端节点%vs[%vs:%vs]的连接成功。" ), ( ( IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
	if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接激活。
void TkbkNtwkCnctAct( TkbkNtwk * TkbkNtwkPt, int CnctNum )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( TkbkNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
	if( ( CnctNum >= p_ElmTotal ) || ( CnctNum < 0 ) )
	{
		VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：没有序号为%d的连接，无法激活。" ), CnctNum );
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, CnctNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL );
	TkbkNtwkCnctInfoAct( TkbkNtwkPt, p_CnctInfoTmpPt ); //连接信息激活。
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接销毁。
void TkbkNtwkCnctDstoy( TkbkNtwk * TkbkNtwkPt, int CnctNum )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( TkbkNtwkPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
	if( ( CnctNum >= p_ElmTotal ) || ( CnctNum < 0 ) )
	{
		VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：没有序号为%d的连接，无法删除。" ), CnctNum );
		if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	
	CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, CnctNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL );
	p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
	
	VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：请求销毁远端节点%vs[%vs:%vs]的连接。" ), p_CnctInfoTmpPt, ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
	if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//接收输出帧初始化。
int TkbkNtwkRecvOtptFrmInit( TkbkNtwk * TkbkNtwkPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

    if( TkbkNtwkPt->m_RecvOtptFrmIsInit == 0 )
    {
		TkbkNtwkPt->m_RecvOtptFrmIsInit = 1; //设置接收输出帧已初始化。

		switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
		{
			case 0: //如果要使用容器。
			{
				//初始化接收音频输出帧容器。
				if( VQueueInit( &TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收音频输出帧容器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化接收音频输出帧容器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				//初始化接收视频输出帧容器。
				if( VQueueInit( &TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收视频输出帧容器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化接收视频输出帧容器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				break;
			}
			case 1: //如果要使用自适应抖动缓冲器。
			{
				//初始化音频自适应抖动缓冲器。
				if( AAjbInit( &TkbkNtwkPt->m_AAjb.m_Pt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, 1, 1, 0, TkbkNtwkPt->m_AAjb.m_MinNeedBufFrmCnt, TkbkNtwkPt->m_AAjb.m_MaxNeedBufFrmCnt, TkbkNtwkPt->m_AAjb.m_MaxCntuLostFrmCnt, TkbkNtwkPt->m_AAjb.m_AdaptSensitivity, ( TkbkNtwkPt->m_XfrMode == 0 ) ? 0 : 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化音频自适应抖动缓冲器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化音频自适应抖动缓冲器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

				//初始化视频自适应抖动缓冲器。
				if( VAjbInit( &TkbkNtwkPt->m_VAjb.m_Pt, 1, TkbkNtwkPt->m_VAjb.m_MinNeedBufFrmCnt, TkbkNtwkPt->m_VAjb.m_MaxNeedBufFrmCnt, TkbkNtwkPt->m_VAjb.m_AdaptSensitivity, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化视频自适应抖动缓冲器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化视频自适应抖动缓冲器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				break;
			}
		}
    }

    p_Rslt = 0; //设置本函数执行成功。

	Out:
    if( p_Rslt != 0 ) //如果本函数执行失败。
    {
		TkbkNtwkRecvOtptFrmDstoy( TkbkNtwkPt );
    }
    return p_Rslt;
}

//接收输出帧重置。
void TkbkNtwkRecvOtptFrmReset( TkbkNtwk * TkbkNtwkPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

    if( TkbkNtwkPt->m_RecvOtptFrmIsInit != 0 )
    {
        switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
        {
            case 0: //如果要使用容器。
            {
                //清空接收音频输出帧容器。
				if( VQueueDelAll( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：清空接收音频输出帧容器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：清空接收音频输出帧容器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}

                //清空接收视频输出帧容器。
				if( VQueueDelAll( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, 1, NULL ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：清空接收视频输出帧容器成功。" ) );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：清空接收视频输出帧容器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
                break;
            }
            case 1: //如果要使用自适应抖动缓冲器。
            {
                //清空并重置音频自适应抖动缓冲器。
                if( AAjbReset( TkbkNtwkPt->m_AAjb.m_Pt, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
                {
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：清空并重置音频自适应抖动缓冲器成功。" ) );
                }
                else
                {
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：清空并重置音频自适应抖动缓冲器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
                }

                //清空并重置视频自适应抖动缓冲器。
                if( VAjbReset( TkbkNtwkPt->m_VAjb.m_Pt, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
                {
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：清空并重置视频自适应抖动缓冲器成功。" ) );
                }
                else
                {
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：清空并重置视频自适应抖动缓冲器失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
                }
                break;
            }
        }
    }
    else
    {
		if( TkbkNtwkRecvOtptFrmInit( TkbkNtwkPt ) != 0 )
		{
			goto Out;
		}
    }

    p_Rslt = 0; //设置本函数执行成功。

	Out:
    if( p_Rslt != 0 ) //如果本函数执行失败。
    {

    }
    return;
}

//接收输出帧销毁。
void TkbkNtwkRecvOtptFrmDstoy( TkbkNtwk * TkbkNtwkPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

    if( TkbkNtwkPt->m_RecvOtptFrmIsInit != 0 )
    {
        //销毁接收音频输出帧容器。
        if( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt != NULL )
        {
			VQueueDstoy( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, NULL );
            TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt = NULL;
            if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：销毁接收音频输出帧容器成功。" ) );
        }

        //销毁接收视频输出帧容器。
        if( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt != NULL )
        {
			VQueueDstoy( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, NULL );
            TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt = NULL;
            if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：销毁接收视频输出帧容器成功。" ) );
        }

        //销毁音频自适应抖动缓冲器。
        if( TkbkNtwkPt->m_AAjb.m_Pt != NULL )
        {
			AAjbDstoy( TkbkNtwkPt->m_AAjb.m_Pt, NULL );
            TkbkNtwkPt->m_AAjb.m_Pt = NULL;
            if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：销毁音频自适应抖动缓冲器成功。" ) );
        }

        //销毁视频自适应抖动缓冲器。
        if( TkbkNtwkPt->m_VAjb.m_Pt != NULL )
        {
            VAjbDstoy( TkbkNtwkPt->m_VAjb.m_Pt, NULL );
            TkbkNtwkPt->m_VAjb.m_Pt = NULL;
            if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "网络媒体处理线程：对讲网络：销毁视频自适应抖动缓冲器成功。" ) );
        }
    }

    p_Rslt = 0; //设置本函数执行成功。

	Out:
    if( p_Rslt != 0 ) //如果本函数执行失败。
    {

    }
    return;
}

//连接处理，包括接受连接、连接服务端、接收数据包、删除连接。
void TkbkNtwkCnctPocs( TkbkNtwk * TkbkNtwkPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
    TkbkNtwk::CnctInfo * p_CnctInfoTmpPt;

	//用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接。
	if( TkbkNtwkPt->m_TcpSrvrSoktPt != NULL )
	{
		int p_PocsRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。
		TcpClntSokt * p_TcpClntSoktTmpPt = NULL;

		while( true )
		{
			if( TcpSrvrAcpt( TkbkNtwkPt->m_TcpSrvrSoktPt, &p_TcpClntSoktTmpPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, ( short )0, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( p_TcpClntSoktTmpPt != NULL ) //如果用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接成功。
				{
					if( TcpClntSetNoDelay( p_TcpClntSoktTmpPt, 1, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
					{
						VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetSendBufSz( p_TcpClntSoktTmpPt, 1024 * 1024, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ) );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetRecvBufSz( p_TcpClntSoktTmpPt, 1024 * 1024, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ) );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetKeepAlive( p_TcpClntSoktTmpPt, 1, 1, 1, 5, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ) );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( ( p_CnctInfoTmpPt = TkbkNtwkCnctInfoInit( TkbkNtwkPt, 0, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, p_TcpClntSoktTmpPt, -1, NtwkMediaPocsThrd::CnctStsCnct ) ) == NULL ) goto TcpSrvrSoktAcptOut; //如果连接信息初始化失败。
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, 0, p_CnctInfoTmpPt->m_LclTkbkMode );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, 0, p_CnctInfoTmpPt->m_RmtTkbkMode );
					if( ( TkbkNtwkPt->m_IsAutoActCnct != 0 ) && ( TkbkNtwkPt->m_CurActCnctInfoPt == NULL ) ) TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode );

					VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字[%vs:%vs]的连接成功。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					break;
				}
				else //如果用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接超时，就跳出接受。
				{
					goto TcpSrvrSoktAcptOut;
				}
			}
			else
			{
				VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接失败。原因：" ) );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto TcpSrvrSoktAcptOut;
			}
		}

		p_PocsRslt = 0; //设置本处理段执行成功。

		TcpSrvrSoktAcptOut:
		if( p_PocsRslt != 0 ) //如果本处理段执行失败。
		{
			if( p_TcpClntSoktTmpPt != NULL )
			{
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypExit; //设置退出包。
				TcpClntSendApkt( p_TcpClntSoktTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 0, 1, 0, NULL ); //发送退出包。
				TcpClntDstoy( p_TcpClntSoktTmpPt, UINT16_MAX, NULL );
			}
		}
	}

	//用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接。
	if( TkbkNtwkPt->m_AudpSrvrSoktPt != NULL )
	{
		int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
		size_t p_AudpCnctIdx = SIZE_MAX; //存放本端高级UDP协议连接索引。

		while( true ) //循环接受远端高级Udp协议套接字的连接。
		{
			if( AudpAcpt( TkbkNtwkPt->m_AudpSrvrSoktPt, &p_AudpCnctIdx, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, ( short )0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( p_AudpCnctIdx != SIZE_MAX ) //如果用本端高级Udp协议套接字接受远端高级Udp协议套接字的连接成功。
				{
					if( ( p_CnctInfoTmpPt = TkbkNtwkCnctInfoInit( TkbkNtwkPt, 0, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, NULL, p_AudpCnctIdx, NtwkMediaPocsThrd::CnctStsCnct ) ) == NULL ) goto AudpSrvrSoktAcptOut; //如果连接信息初始化失败。
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, 0, p_CnctInfoTmpPt->m_LclTkbkMode );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, 0, p_CnctInfoTmpPt->m_RmtTkbkMode );
					if( ( TkbkNtwkPt->m_IsAutoActCnct != 0 ) && ( TkbkNtwkPt->m_CurActCnctInfoPt == NULL ) ) TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode );

					VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字[%vs:%vs]的连接成功。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					break;
				}
				else //如果用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接超时，就跳出接受。
				{
					goto AudpSrvrSoktAcptOut;
				}
			}
			else
			{
				VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接失败。原因：" ) );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto AudpSrvrSoktAcptOut;
			}
		}

		p_PocsRslt = 0; //设置本处理段执行成功。

		AudpSrvrSoktAcptOut:
		if( p_PocsRslt != 0 ) //如果本处理段执行失败。
		{
			if( p_AudpCnctIdx != SIZE_MAX )
			{
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypExit; //设置退出包。
				AudpSendApkt( TkbkNtwkPt->m_AudpSrvrSoktPt, p_AudpCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, 1, 1, 1, NULL ); //发送退出包。
				AudpClosCnct( TkbkNtwkPt->m_AudpSrvrSoktPt, p_AudpCnctIdx, NULL );
			}
		}
	}

    //遍历连接信息容器。
    for( int p_CnctInfoLstNum = 0; CQueueGetByNum( TkbkNtwkPt->m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
    {
        //用本端客户端套接字连接远端服务端套接字。
        if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_IsSrvrOrClntCnct == 1 ) && ( p_CnctInfoTmpPt->m_CurCnctSts <= NtwkMediaPocsThrd::CnctStsWait ) ) //如果该连接未请求销毁，且为客户端的连接，且当前连接状态为等待远端接受连接。
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
                    if( -p_CnctInfoTmpPt->m_CurCnctSts >= TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
                    {
                        p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                        goto TcpClntSoktCnctOut;
                    }

                    p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
                    TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。

					VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：开始第 %d 次连接。" ), p_CnctInfoTmpPt, -p_CnctInfoTmpPt->m_CurCnctSts );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

                    if( TcpClntInit( &p_CnctInfoTmpPt->m_TcpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, NULL, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字失败。
                    {
						VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                        goto TcpClntSoktCnctOut;
                    }
                }

                if( TcpClntWaitCnct( p_CnctInfoTmpPt->m_TcpClntSoktPt, 0, &p_TcpCnctSts, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字成功。
                {
                    if( p_TcpCnctSts == TcpCnctStsWait ) //如果等待远端接受连接。
                    {
                        //继续等待本端本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字。
                    }
                    else if( p_TcpCnctSts == TcpCnctStsCnct ) //如果连接成功。
                    {
						if( TcpClntSetNoDelay( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
						{
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetSendBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetRecvBufSz( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1024 * 1024, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

						if( TcpClntSetKeepAlive( p_CnctInfoTmpPt->m_TcpClntSoktPt, 1, 1, 1, 5, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
						}

                        if( TcpClntGetLclAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
                        {
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：获取本端Tcp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
                        }

                        if( TcpClntGetRmtAddr( p_CnctInfoTmpPt->m_TcpClntSoktPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
                        {
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：获取本端Tcp协议客户端套接字连接的远端Tcp协议客户端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto TcpClntSoktCnctOut;
                        }

                        p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
                        TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
                        if( ( TkbkNtwkPt->m_IsAutoActCnct != 0 ) && ( TkbkNtwkPt->m_CurActCnctInfoPt == NULL ) ) TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //如果自动激活连接，且当前激活的连接信息为空，就连接信息更新本端对讲模式为实际的。
                        else TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_LclTkbkMode ); //如果手动激活连接，或当前激活的连接信息不为空，就连接信息更新本端对讲模式为该连接信息的本端对讲模式，可能为挂起，也可能在重连时为其他状态。
                        TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt );

						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：初始化本端Tcp协议客户端套接字[%vs:%vs]，并连接远端Tcp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                    }
                    else //如果连接失败。
                    {
						VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                        goto TcpClntSoktCnctOut;
                    }
                }
                else //如果等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。
                {
					VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：初始化本端Tcp协议客户端套接字，并连接远端Tcp协议服务端套接字[%vs:%vs]失败。原因：等待本端Tcp协议客户端套接字连接远端Tcp协议服务端套接字失败。原因：" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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

                if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt == NULL ) //如果未初始化本端高级Udp协议客户端套接字。
                {
                    if( AudpInit( &TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, NULL, NULL, 0, 5000, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议客户端套接字成功。
                    {
                        if( AudpGetLclAddr( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
                        {
							VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：获取本端高级Udp协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
                        }

						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端高级Udp协议客户端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclNodePortPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
                    }
                    else //如果初始化本端高级Udp协议客户端套接字失败。
                    {
						VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：初始化本端高级Udp协议客户端套接字失败。原因：" ) );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
                    }
                }

				if( p_CnctInfoTmpPt->m_AudpClntCnctIdx == SIZE_MAX )
				{
					if( -p_CnctInfoTmpPt->m_CurCnctSts >= TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MaxCnctTimes ) //如果已达到最大连接次数。
					{
						p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。这里只设置请求销毁，不设置当前连接状态，因为在连接信息销毁函数里要根据当前连接状态判断是否重连。

						VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：已达到最大连接次数，中断连接。" ), p_CnctInfoTmpPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}

					p_CnctInfoTmpPt->m_CurCnctSts--; //递增当前连接次数。
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。

					VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：开始第 %d 次连接。" ), p_CnctInfoTmpPt, -p_CnctInfoTmpPt->m_CurCnctSts );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

					if( AudpCnct( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_RmtNodeAddrFamly, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, &p_CnctInfoTmpPt->m_AudpClntCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字失败。
					{
						VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				if( AudpWaitCnct( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, 0, &p_AudpCnctSts, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //循环等待本端高级Udp协议套接字连接远端成功。
				{
					if( p_AudpCnctSts == AudpCnctStsWait ) //如果等待远端接受连接。
					{
						//重新循环，继续等待本端高级Udp协议套接字连接远端。
					}
					else if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
					{
						if( AudpGetRmtAddr( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：获取本端高级Udp协议客户端套接字连接的远端高级Udp协议服务端套接字绑定的远程节点地址和端口失败。原因：" ), p_CnctInfoTmpPt );
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							goto AudpClntSoktCnctOut;
						}

						p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsCnct; //设置当前连接状态为已连接。
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
						if( ( TkbkNtwkPt->m_IsAutoActCnct != 0 ) && ( TkbkNtwkPt->m_CurActCnctInfoPt == NULL ) ) TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //如果自动激活连接，且当前激活的连接信息为空，就连接信息更新本端对讲模式为实际的。
						else TkbkNtwkCnctInfoUpdtLclTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_LclTkbkMode ); //如果手动激活连接，或当前激活的连接信息不为空，就连接信息更新本端对讲模式为该连接信息的本端对讲模式，可能为挂起，也可能在重连时为其他状态。
						TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt );

						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]成功。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_RmtNodePortPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
					else if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
					{
						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接超时。" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
					else if( p_AudpCnctSts == AudpCnctStsDsct ) //如果连接断开。
					{
						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端高级Udp协议客户端套接字连接远端高级Udp协议服务端套接字[%vs:%vs]失败。原因：连接断开。" ), p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto AudpClntSoktCnctOut;
					}
				}

				p_PocsRslt = 0; //设置本处理段执行成功。

				AudpClntSoktCnctOut:
				if( p_PocsRslt != 0 ) //如果本处理段执行失败。
				{
					if( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt != NULL ) && ( p_CnctInfoTmpPt->m_AudpClntCnctIdx != SIZE_MAX ) )
					{
						AudpClosCnct( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, NULL );
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

			if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntRecvApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByteSz, &p_PktLenByt, 0, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( p_CnctInfoTmpPt->m_IsSrvrOrClntCnct == 0 ) && ( AudpRecvApkt( TkbkNtwkPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByteSz, &p_PktLenByt, NULL, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( p_CnctInfoTmpPt->m_IsSrvrOrClntCnct == 1 ) && ( AudpRecvApkt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByteSz, &p_PktLenByt, NULL, ( short )0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
				{
					if( p_PktLenByt == 0 ) //如果数据包的数据长度为0。
					{
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个数据包的数据长度为%uzd，表示没有数据，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
						goto RecvPktOut;
					}
					else if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypTkbkMode ) //如果是对讲模式包。
					{
						if( p_PktLenByt < 1 + 1 ) //如果音频输出帧包的数据长度小于1 + 1，表示没有对讲模式。
						{
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个对讲模式包的数据长度为%uzd小于1 + 1，表示没有对讲模式，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] >= NtwkMediaPocsThrd::TkbkModeNoChg )
						{
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个对讲模式包的对讲模式为%z8d不正确，无法继续接收。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] );
							goto RecvPktOut;
						}

						p_CnctInfoTmpPt->m_RmtTkbkMode = TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ]; //设置远端对讲模式。
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个对讲模式包。对讲模式：%z8s" ), p_CnctInfoTmpPt, g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ] );

						if( ( TkbkNtwkPt->m_IsAutoActCnct != 0 ) && ( TkbkNtwkPt->m_CurActCnctInfoPt == NULL ) && ( p_CnctInfoTmpPt->m_LclTkbkMode != NtwkMediaPocsThrd::TkbkModeNone ) && ( p_CnctInfoTmpPt->m_RmtTkbkMode != NtwkMediaPocsThrd::TkbkModeNone ) ) TkbkNtwkCnctInfoAct( TkbkNtwkPt, p_CnctInfoTmpPt ); //连接信息激活。
						TkbkNtwkCnctInfoUpdtRmtTkbkMode( TkbkNtwkPt, p_CnctInfoTmpPt );
						if( TkbkNtwkPt->m_CurActCnctInfoPt == p_CnctInfoTmpPt ) NtwkMediaPocsThrdSetTkbkMode( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。
					}
					else if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypAdoFrm ) //如果是音频输出帧包。
					{
						if( p_PktLenByt < 1 + 4 ) //如果音频输出帧包的数据长度小于1 + 4，表示没有音频输出帧时间戳。
						{
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个音频输出帧包的数据长度为%uzd小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						//读取音频输出帧时间戳。
						p_TmpUint32 = ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] & 0xFF ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] & 0xFF ) << 8 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] & 0xFF ) << 16 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] & 0xFF ) << 24 );

						if( ( TkbkNtwkPt->m_CurActCnctInfoPt == p_CnctInfoTmpPt ) && //如果该连接为当前激活连接。
							( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) || //如果已初始化音频输出。
								( ( TkbkNtwkPt->m_XfrMode == 0 ) && ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode == NtwkMediaPocsThrd::TkbkModeAdo ) || ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode == NtwkMediaPocsThrd::TkbkModeAdoVdo ) ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为音频或音视频。
						{
							//将音频输出帧放入容器或自适应抖动缓冲器。
							switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
							{
								case 0: //如果要使用容器。
								{
									if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
									{
										VQueueGetTotal( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, &p_TmpElmTotal, 1, NULL ); //获取接收音频输出帧容器的元素总数。
										if( p_TmpElmTotal <= 50 )
										{
											if( VQueuePutTail( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, p_PktLenByt, NULL, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
											{
												if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧容器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
											}
											else
											{
												if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧容器失败。原因：%vs音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, p_TmpUint32, p_PktLenByt );
											}
										}
										else
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包，但接收音频输出帧容器中帧总数为%uzd已经超过上限50，不再放入接收音频输出帧容器。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpElmTotal, p_TmpUint32, p_PktLenByt );
										}
									}
									else //如果该音频输出帧为无语音活动。
									{
										if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无语音活动的音频输出帧包，无需放入接收音频输出帧容器。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
									}
									break;
								}
								case 1: //如果要使用自适应抖动缓冲器。
								{
									if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
									{
										if( AAjbPutFrm( TkbkNtwkPt->m_AAjb.m_Pt, p_TmpUint32, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
										}
										else
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器失败。原因：%vs音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, p_TmpUint32, p_PktLenByt );
										}
									}
									else //如果该音频输出帧为无语音活动。
									{
										if( AAjbPutFrm( TkbkNtwkPt->m_AAjb.m_Pt, p_TmpUint32, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, 0, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
										}
										else
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器失败。原因：%vs音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, p_TmpUint32, p_PktLenByt );
										}
									}

									int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
									int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
									int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
									int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
									int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
									int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
									int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
									AAjbGetBufFrmCnt( TkbkNtwkPt->m_AAjb.m_Pt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
									if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CnctInfoTmpPt, p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );
									break;
								}
							}
						}
						else //如果未初始化音频输出。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有语音活动的音频输出帧包成功，但不是当前激活连接或未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
							}
							else //如果该音频输出帧为无语音活动。
							{
								if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无语音活动的音频输出帧包成功，但不是当前激活连接或未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
							}
						}
					}
					else if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypVdoFrm ) //如果是视频输出帧包。
					{
						if( p_PktLenByt < 1 + 4 ) //如果视频输出帧包的数据长度小于1 + 4，表示没有视频输出帧时间戳。
						{
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个视频输出帧包的数据长度为%uzd小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						//读取视频输出帧时间戳。
						p_TmpUint32 = ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] & 0xFF ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] & 0xFF ) << 8 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] & 0xFF ) << 16 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] & 0xFF ) << 24 );

						if( ( TkbkNtwkPt->m_CurActCnctInfoPt == p_CnctInfoTmpPt ) && //如果该连接为当前激活连接。
							( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_VdoOtpt.m_IsInit != 0 ) || //如果已初始化视频输出。
								( ( TkbkNtwkPt->m_XfrMode == 0 ) && ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode == NtwkMediaPocsThrd::TkbkModeVdo ) || ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_LclTkbkMode == NtwkMediaPocsThrd::TkbkModeAdoVdo ) ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为视频或音视频。
						{
							//将视频输出帧放入容器或自适应抖动缓冲器。
							switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
							{
								case 0: //如果要使用容器。
								{
									if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
									{
										VQueueGetTotal( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, &p_TmpElmTotal, 1, NULL ); //获取接收视频输出帧容器的元素总数。
										if( p_TmpElmTotal <= 20 )
										{
											if( VQueuePutTail( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, p_PktLenByt, NULL, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
											{
												if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧容器成功。视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
											}
											else
											{
												if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧容器失败。原因：%vs视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, p_TmpUint32, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
											}
										}
										else
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包，但接收视频输出帧容器中帧总数为%uzd已经超过上限20，不再放入接收视频输出帧容器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpElmTotal, p_TmpUint32, p_PktLenByt );
										}
									}
									else //如果该视频输出帧为无图像活动。
									{
										if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无图像活动的视频输出帧包，无需放入接收视频输出帧容器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
									}
									break;
								}
								case 1: //如果要使用自适应抖动缓冲器。
								{
									if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
									{
										if( VAjbPutFrm( TkbkNtwkPt->m_VAjb.m_Pt, FuncGetTickAsMsec(), p_TmpUint32, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
										}
										else
										{
											if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器失败。原因：%vs视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, p_TmpUint32, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
										}
									}
									else //如果该视频输出帧为无图像活动。
									{
										if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
									}

									int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
									int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
									int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
									int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
									VAjbGetBufFrmCnt( TkbkNtwkPt->m_VAjb.m_Pt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
									if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CnctInfoTmpPt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );
									break;
								}
							}
						}
						else //如果未初始化视频输出。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个有图像活动的视频输出帧包成功，但不是当前激活连接或未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd，类型：%uz8d。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] & 0xff );
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个无图像活动的视频输出帧包成功，但不是当前激活连接或未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_CnctInfoTmpPt, p_TmpUint32, p_PktLenByt );
							}
						}
					}
					else if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] == NtwkMediaPocsThrd::PktTypExit ) //如果是退出包。
					{
						if( p_PktLenByt > 1 ) //如果退出包的数据长度大于1。
						{
							if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个退出包的数据长度为%uzd大于1，表示还有其他数据，无法继续接收。" ), p_CnctInfoTmpPt, p_PktLenByt );
							goto RecvPktOut;
						}

						p_CnctInfoTmpPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
						p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

						VstrFmtCpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：接收到一个退出包。" ), p_CnctInfoTmpPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsShowToast != 0 ) TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowToastFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					}
				} //如果用本端套接字接收一个连接的远端套接字发送的数据包超时，就重新接收。
			}
			else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
			{
				p_CnctInfoTmpPt->m_CurCnctSts = NtwkMediaPocsThrd::CnctStsTmot; //设置当前连接状态为异常断开。
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
				p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

				VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "网络媒体处理线程：对讲网络：连接%P：用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ), p_CnctInfoTmpPt );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto RecvPktOut;
			}
			RecvPktOut:;
		}

		//销毁连接信息。
		if( p_CnctInfoTmpPt->m_IsRqstDstoy == 1 ) //如果该连接已请求销毁。
		{
			TkbkNtwkCnctInfoDstoy( TkbkNtwkPt, p_CnctInfoTmpPt );
			p_CnctInfoLstNum--;
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
void TkbkNtwkUserReadAdoVdoInptFrm( TkbkNtwk * TkbkNtwkPt,
									int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
									uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
									uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
									uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
									uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt )
{
	size_t p_PktLenByt = 0; //存放数据包的长度，单位为字节。

	//发送音频输入帧。
	if( AdoInptPcmSrcFrmPt != NULL ) //如果有音频输入Pcm格式原始帧。
	{
		if( AdoInptEncdRsltFrmPt == NULL ) //如果没有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 ) //如果音频输入Pcm格式结果帧为有语音活动。
			{
				memcpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, AdoInptPcmRsltFrmPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入Pcm格式帧。
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
				memcpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + AdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入已编码格式帧。
			}
			else //如果音频输入Pcm格式结果帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypAdoFrm;
			//设置音频输入帧时间戳。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( int8_t )( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( TkbkNtwkCnctInfoSendPkt( TkbkNtwkPt, TkbkNtwkPt->m_CurActCnctInfoPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, p_PktLenByt, 1, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
			}
			else
			{
				VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有语音活动的音频输入帧包失败。原因：" ) );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}

			TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmIsAct != 0 ) //如果最后一个发送的音频输入帧为有语音活动，就发送。
			{
				TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypAdoFrm;
				//设置音频输入帧时间戳。
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( int8_t )( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				if( TkbkNtwkCnctInfoSendPkt( TkbkNtwkPt, TkbkNtwkPt->m_CurActCnctInfoPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, p_PktLenByt, 1, 0, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
				}
				else
				{
					VstrIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个无语音活动的音频输入帧包失败。原因：" ) );
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				}

				TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
			}
			else //如果最后一个发送的音频输入帧为无语音活动，无需发送。
			{
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本次音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}

	//发送视频输入帧。
	if( VdoInptYu12RsltFrmPt != NULL ) //如果有视频输入Yu12格式结果帧。
	{
		if( VdoInptEncdRsltFrmPt == NULL ) //如果没有视频输入已编码格式结果帧。
		{
			//设置视频输入帧宽度。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 5 ] = ( int8_t )( VdoInptYu12RsltFrmWidth & 0xFF );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 6 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF00 ) >> 8 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 7 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF0000 ) >> 16 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 8 ] = ( int8_t )( ( VdoInptYu12RsltFrmWidth & 0xFF000000 ) >> 24 );
			//设置视频输入帧高度。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 9 ] = ( int8_t )( VdoInptYu12RsltFrmHeight & 0xFF );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 10 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF00 ) >> 8 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 11 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF0000 ) >> 16 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 12 ] = ( int8_t )( ( VdoInptYu12RsltFrmHeight & 0xFF000000 ) >> 24 );

			memcpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4 + 4 + 4, VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2 ); //设置视频输入帧。
			p_PktLenByt = 1 + 4 + 4 + 4 + VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + 视频输入Yu12格式结果帧。
		}
		else //如果有视频输入已编码格式结果帧。
		{
			if( VdoInptEncdRsltFrmLenByt != 0 ) //如果本次视频输入已编码格式结果帧为有图像活动。
			{
				memcpy( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt + 1 + 4, VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt ); //设置视频输入帧。
				p_PktLenByt = 1 + 4 + VdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 视频输入已编码格式结果帧。
			}
			else
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 视频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 4 ) //如果本次视频输入帧为有图像活动，就发送。
		{
			TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

			//设置数据包类型为视频输入帧包。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 0 ] = NtwkMediaPocsThrd::PktTypVdoFrm;
			//设置视频输入帧时间戳。
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 1 ] = ( int8_t )( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 2 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 3 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 4 ] = ( int8_t )( ( TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( TkbkNtwkCnctInfoSendPkt( TkbkNtwkPt, TkbkNtwkPt->m_CurActCnctInfoPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt, p_PktLenByt, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。" ), TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 13 ] & 0xff );
			}
			else
			{
				VstrFmtIns( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有图像活动的视频输入帧包失败。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。原因：" ), TkbkNtwkPt->m_CurActCnctInfoPt->m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpBytePt[ 13 ] & 0xff );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( TkbkNtwkPt->m_NtwkMediaPocsThrdPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}
		else //如果本次视频输入帧为无图像活动，无需发送。
		{
			if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本次视频输入帧为无图像活动，无需发送。" ) );
		}
	}
}

//用户定义的写入音频输出帧函数。
void TkbkNtwkUserWriteAdoOtptFrm( TkbkNtwk * TkbkNtwkPt, int32_t AdoOtptStrmIdx,
								  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//取出并写入音频输出帧。
	{
		//从容器或音频自适应抖动缓冲器取出一个音频输出帧。
		switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收音频输出帧。
		{
			case 0: //如果要使用容器。
			{
				VQueueGetTotal( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, &m_TmpSz, 1, NULL ); //获取接收音频输出帧容器的元素总数。
				if( m_TmpSz != 0 )
				{
					VQueueGetHead( TkbkNtwkPt->m_RecvAdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Pt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Sz, NULL, &m_TmpSz, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取并删除接收音频输出帧容器的第一个音频输出帧。
				}

				if( m_TmpSz != 0 ) //如果接收音频输出帧容器的第一个输出帧为有语音活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收音频输出帧容器取出一个有语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}
				else //如果接收音频输出帧容器为空，或第一个音频输出帧为无语音活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收音频输出帧容器取出一个无语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
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
				AAjbGetBufFrmCnt( TkbkNtwkPt->m_AAjb.m_Pt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_AdoOtptFrmTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出音频输出帧。
				AAjbGetFrm( TkbkNtwkPt->m_AAjb.m_Pt, &p_AdoOtptFrmTimeStamp, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Pt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Sz, &m_TmpSz, 1, NULL );

				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
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
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出帧的长度不等于Pcm格式的长度。音频输出帧：%uzd，Pcm格式：%z32d。" ), m_TmpSz, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
				else
				{
					//写入音频输出Pcm格式原始帧。
					memcpy( AdoOtptPcmSrcFrmPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Pt, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
			}
			else //如果要使用音频输出已编码格式原始帧。
			{
				if( m_TmpSz > AdoOtptEncdSrcFrmSzByt )
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出帧的长度已超过已编码格式的长度。音频输出帧：%uzd，已编码格式：%uzd。" ), m_TmpSz, AdoOtptEncdSrcFrmSzByt );
					*AdoOtptEncdSrcFrmLenBytPt = 0;
				}
				else
				{
					//写入音频输出已编码格式原始帧。
					*AdoOtptEncdSrcFrmLenBytPt = m_TmpSz;
					memcpy( AdoOtptEncdSrcFrmPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte2Pt, m_TmpSz );
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
void TkbkNtwkUserGetAdoOtptFrm( TkbkNtwk * TkbkNtwkPt, int32_t AdoOtptStrmIdx,
								int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
								uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{
		
}

//用户定义的写入视频输出帧函数。
void TkbkNtwkUserWriteVdoOtptFrm( TkbkNtwk * TkbkNtwkPt, uint32_t VdoOtptStrmIdx,
								  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
								  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//从容器或自适应抖动缓冲器取出一个视频输出帧。
	switch( TkbkNtwkPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果要使用容器。
		{
			VQueueGetTotal( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, &m_TmpSz, 1, NULL );
			if( m_TmpSz != 0 ) //如果接收视频输出帧容器不为空。
			{
				VQueueGetHead( TkbkNtwkPt->m_RecvVdoOtptFrmCntnrPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Sz, NULL, &m_TmpSz, 1, 1, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取并删除接收视频输出帧容器的第一个视频输出帧。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收视频输出帧容器取出一个有图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收视频输出帧容器取出一个无图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
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
			VAjbGetBufFrmCnt( TkbkNtwkPt->m_VAjb.m_Pt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );

			if( p_CurHaveBufFrmCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_VdoOtptFrmTimeStamp;

				//从视频自适应抖动缓冲器取出视频输出帧。
				VAjbGetFrm( TkbkNtwkPt->m_VAjb.m_Pt, FuncGetTickAsMsec(), &p_VdoOtptFrmTimeStamp, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Sz, &m_TmpSz, 1, NULL );

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
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
			*VdoOtptYu12SrcFrmWidthPt = ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[0] & 0xFF ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[1] & 0xFF ) << 8 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[2] & 0xFF ) << 16 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[3] & 0xFF ) << 24 );
			//读取视频输出帧高度。
			*VdoOtptYu12SrcFrmHeightPt = ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[4] & 0xFF ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[5] & 0xFF ) << 8 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[6] & 0xFF ) << 16 ) + ( ( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt[7] & 0xFF ) << 24 );

			if( m_TmpSz - 4 - 4 != *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 )
			{
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出帧的长度不等于Yu12格式的长度。视频输出帧：%uzd，Yu12格式：%z32d。" ), m_TmpSz - 4 - 4, *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 );
				*VdoOtptYu12SrcFrmWidthPt = 0;
				*VdoOtptYu12SrcFrmHeightPt = 0;
				return;
			}

			//写入视频输出Yu12格式原始帧。
			memcpy( VdoOtptYu12SrcFrmPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt + 4 + 4, m_TmpSz - 4 - 4 );
		}
		else //如果要使用视频输出已编码格式帧。
		{
			if( m_TmpSz > VdoOtptEncdSrcFrmSzByt )
			{
				*VdoOtptEncdSrcFrmLenBytPt = 0;
				if( TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出帧的长度已超过已编码格式的长度。视频输出帧：%uzd，已编码格式：%z32d。" ), m_TmpSz, VdoOtptEncdSrcFrmSzByt );
				return;
			}

			//写入视频输出已编码格式帧。
			memcpy( VdoOtptEncdSrcFrmPt, TkbkNtwkPt->m_NtwkMediaPocsThrdPt->m_TmpByte3Pt, m_TmpSz );
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
void TkbkNtwkUserGetVdoOtptFrm( TkbkNtwk * TkbkNtwkPt, uint32_t VdoOtptStrmIdx,
								uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
								uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{
	
}