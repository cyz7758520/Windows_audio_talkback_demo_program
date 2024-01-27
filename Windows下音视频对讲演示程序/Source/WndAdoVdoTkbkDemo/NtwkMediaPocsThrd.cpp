#include "NtwkMediaPocsThrd.h"

const char * const g_TkbkModeU8strArrPt[ 5 ] = { "挂起", "音频", "视频", "音视频", "不变" };

//网络媒体处理线程初始化。
int NtwkMediaPocsThrdInit( NtwkMediaPocsThrd * * NtwkMediaPocsThrdPtPt, void * UserDataPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserNtwkMediaPocsThrdInitFuncPt UserNtwkMediaPocsThrdInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserNtwkMediaPocsThrdDstoyFuncPt UserNtwkMediaPocsThrdDstoyFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserSrvrInitFuncPt UserSrvrInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserSrvrDstoyFuncPt UserSrvrDstoyFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserShowLogFuncPt UserShowLogFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserShowToastFuncPt UserShowToastFuncPt,
						   NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctInitFuncPt UserCnctInitFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctStsFuncPt UserCnctStsFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctActFuncPt UserCnctActFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctLclTkbkModeFuncPt UserCnctLclTkbkModeFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctRmtTkbkModeFuncPt UserCnctRmtTkbkModeFuncPt, NtwkMediaPocsThrd::NtwkMediaPocsThrdUserCnctDstoyFuncPt UserCnctDstoyFuncPt,
						   Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = NULL; //存放网络媒体处理线程的指针。

	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针的指针不正确。" ), , );
		goto Out;
	}
	
	//创建并清空网络媒体处理线程内存块。
	p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )calloc( 1, sizeof( NtwkMediaPocsThrd ) );
	if( p_NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空网络媒体处理线程内存块失败。" ), , );
		goto Out;
	}
	
	//初始化媒体处理线程。
	if( MediaPocsThrdInit( &p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, p_NtwkMediaPocsThrdPt,
						   NtwkMediaPocsThrdUserInit, NtwkMediaPocsThrdUserPocs, NtwkMediaPocsThrdUserDstoy, NtwkMediaPocsThrdUserMsg,
						   NtwkMediaPocsThrdUserReadAdoVdoInptFrm,
						   NtwkMediaPocsThrdUserWriteAdoOtptFrm, NtwkMediaPocsThrdUserGetAdoOtptFrm,
						   NtwkMediaPocsThrdUserWriteVdoOtptFrm, NtwkMediaPocsThrdUserGetVdoOtptFrm, ErrInfoVstrPt ) )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化媒体处理线程失败。原因：" ) );
		goto Out;
	}

	//初始化对讲网络。
	p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_NtwkMediaPocsThrdPt = p_NtwkMediaPocsThrdPt;
	if( CQueueInit( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CnctInfoCntnrPt, sizeof( TkbkNtwk::CnctInfo ), 10, BufAutoAdjMethFreeNumber, 10, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化对讲网络的连接信息容器失败。原因" ) );
		goto Out;
	}

	//初始化广播网络。
	p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_NtwkMediaPocsThrdPt = p_NtwkMediaPocsThrdPt;
	if( CQueueInit( &p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, sizeof( TkbkNtwk::CnctInfo ), 10, BufAutoAdjMethFreeNumber, 10, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化广播网络的连接信息容器失败。原因" ) );
		goto Out;
	}
	p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
	p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。

	//初始化网络媒体处理线程。
	p_NtwkMediaPocsThrdPt->m_LclTkbkMode = NtwkMediaPocsThrd::TkbkModeNone;
	if( VstrInit( &p_NtwkMediaPocsThrdPt->m_LclNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_NtwkMediaPocsThrdPt->m_LclNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_NtwkMediaPocsThrdPt->m_RmtNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_NtwkMediaPocsThrdPt->m_RmtNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( p_NtwkMediaPocsThrdPt->m_TmpBytePt == NULL )
	{
		p_NtwkMediaPocsThrdPt->m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( p_NtwkMediaPocsThrdPt->m_TmpBytePt == NULL )
		{
			VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
			if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_NtwkMediaPocsThrdPt->m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据大小。
	}
	if( p_NtwkMediaPocsThrdPt->m_TmpByte2Pt == NULL )
	{
		p_NtwkMediaPocsThrdPt->m_TmpByte2Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( p_NtwkMediaPocsThrdPt->m_TmpByte2Pt == NULL )
		{
			VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
			if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_NtwkMediaPocsThrdPt->m_TmpByte2Sz = 1024 * 1024; //设置临时数据大小。
	}
	if( p_NtwkMediaPocsThrdPt->m_TmpByte3Pt == NULL )
	{
		p_NtwkMediaPocsThrdPt->m_TmpByte3Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( p_NtwkMediaPocsThrdPt->m_TmpByte3Pt == NULL )
		{
			VstrCpy( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
			if( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_NtwkMediaPocsThrdPt->m_TmpByte3Sz = 1024 * 1024; //设置临时数据大小。
	}
	
	p_NtwkMediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_NtwkMediaPocsThrdPt->m_UserNtwkMediaPocsThrdInitFuncPt = UserNtwkMediaPocsThrdInitFuncPt; //设置用户定义的网络媒体处理线程初始化函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserNtwkMediaPocsThrdDstoyFuncPt = UserNtwkMediaPocsThrdDstoyFuncPt; //设置用户定义的网络媒体处理线程销毁函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserSrvrInitFuncPt = UserSrvrInitFuncPt; //设置用户定义的服务端初始化函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserSrvrDstoyFuncPt = UserSrvrDstoyFuncPt; //设置用户定义的服务端销毁函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt = UserShowLogFuncPt; //设置用户定义的显示日志函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserShowToastFuncPt = UserShowToastFuncPt; //设置用户定义的显示Toast函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctInitFuncPt = UserCnctInitFuncPt; //设置用户定义的连接初始化函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctStsFuncPt = UserCnctStsFuncPt; //设置用户定义的连接状态函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctActFuncPt = UserCnctActFuncPt; //设置用户定义的连接激活函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctLclTkbkModeFuncPt = UserCnctLclTkbkModeFuncPt; //设置用户定义的连接本端对讲模式函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctRmtTkbkModeFuncPt = UserCnctRmtTkbkModeFuncPt; //设置用户定义的连接远端对讲模式函数的指针。
	p_NtwkMediaPocsThrdPt->m_UserCnctDstoyFuncPt = UserCnctDstoyFuncPt; //设置用户定义的连接销毁函数的指针。

	*NtwkMediaPocsThrdPtPt = p_NtwkMediaPocsThrdPt; //设置我的网络媒体处理线程的指针。

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//网络媒体处理线程销毁。
int NtwkMediaPocsThrdDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	//销毁媒体处理线程。因为要先让媒体处理线程停下来。
	MediaPocsThrdDstoy( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, NULL );

	//销毁网络媒体处理线程。
	VstrDstoy( NtwkMediaPocsThrdPt->m_LclNodeAddrPt ); //销毁本端节点地址动态字符串。
	VstrDstoy( NtwkMediaPocsThrdPt->m_LclNodePortPt ); //销毁本端节点端口动态字符串。
	VstrDstoy( NtwkMediaPocsThrdPt->m_RmtNodeAddrPt ); //销毁远端节点地址动态字符串。
	VstrDstoy( NtwkMediaPocsThrdPt->m_RmtNodePortPt ); //销毁远端节点端口动态字符串。
	if( NtwkMediaPocsThrdPt->m_TmpBytePt != NULL ) free( NtwkMediaPocsThrdPt->m_TmpBytePt ); //销毁临时数据的内存块。
	if( NtwkMediaPocsThrdPt->m_TmpByte2Pt != NULL ) free( NtwkMediaPocsThrdPt->m_TmpByte2Pt ); //销毁临时数据的内存块。
	if( NtwkMediaPocsThrdPt->m_TmpByte3Pt != NULL ) free( NtwkMediaPocsThrdPt->m_TmpByte3Pt ); //销毁临时数据的内存块。
	
	//销毁广播网络。
	CQueueDstoy( NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, NULL ); //销毁广播网络的连接信息容器。
	
	//销毁对讲网络。
	CQueueDstoy( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CnctInfoCntnrPt, NULL ); //销毁对讲网络的连接信息容器。

	//销毁网络媒体处理线程内存块。
	free( NtwkMediaPocsThrdPt );
	
	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//服务端初始化。
int NtwkMediaPocsThrdSrvrInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * SrvrUrlVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgSrvrInit p_UserMsgSrvrInit;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgSrvrInit.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypSrvrInit;
	if( VstrInit( &p_UserMsgSrvrInit.m_SrvrUrlVstrPt, Utf8, , SrvrUrlVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置服务端Url动态字符串失败。" ), , );
		goto Out;
	}
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgSrvrInit, sizeof( p_UserMsgSrvrInit ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//服务端销毁。
int NtwkMediaPocsThrdSrvrDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgSrvrDstoy p_UserMsgSrvrDstoy;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgSrvrDstoy.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypSrvrDstoy;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgSrvrDstoy, sizeof( p_UserMsgSrvrDstoy ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接初始化。
int NtwkMediaPocsThrdCnctInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgCnctInit p_UserMsgCnctInit;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgCnctInit.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypCnctInit;
	p_UserMsgCnctInit.m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	if( VstrInit( &p_UserMsgCnctInit.m_RmtNodeNameVstrPt, Utf8, , RmtNodeNameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点名称动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_UserMsgCnctInit.m_RmtNodeSrvcVstrPt, Utf8, , RmtNodeSrvcVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点服务动态字符串失败。" ), , );
		goto Out;
	}
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgCnctInit, sizeof( p_UserMsgCnctInit ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接激活。
int NtwkMediaPocsThrdCnctAct( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int CnctNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgCnctAct p_UserMsgCnctAct;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgCnctAct.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypCnctAct;
	p_UserMsgCnctAct.m_CnctNum = CnctNum;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgCnctAct, sizeof( p_UserMsgCnctAct ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//连接销毁。
int NtwkMediaPocsThrdCnctDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int CnctNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgCnctDstoy p_UserMsgCnctDstoy;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgCnctDstoy.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypCnctDstoy;
	p_UserMsgCnctDstoy.m_CnctNum = CnctNum;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgCnctDstoy, sizeof( p_UserMsgCnctDstoy ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//本端对讲模式。
int NtwkMediaPocsThrdLclTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int LclTkbkMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgLclTkbkMode p_UserMsgLclTkbkMode;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgLclTkbkMode.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypLclTkbkMode;
	p_UserMsgLclTkbkMode.m_LclTkbkMode = LclTkbkMode;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgLclTkbkMode, sizeof( p_UserMsgLclTkbkMode ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//一键即按即通按钮按下。
int NtwkMediaPocsThrdPttBtnDown( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgPttBtnDown p_UserMsgPttBtnDown;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgPttBtnDown.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypPttBtnDown;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgPttBtnDown, sizeof( p_UserMsgPttBtnDown ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//一键即按即通按钮弹起。
int NtwkMediaPocsThrdPttBtnUp( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgPttBtnUp p_UserMsgPttBtnUp;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgPttBtnUp.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypPttBtnUp;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgPttBtnUp, sizeof( p_UserMsgPttBtnUp ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//广播连接初始化。
int NtwkMediaPocsThrdBdctCnctInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, int IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgBdctCnctInit p_UserMsgBdctCnctInit;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgBdctCnctInit.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypBdctCnctInit;
	p_UserMsgBdctCnctInit.m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	if( VstrInit( &p_UserMsgBdctCnctInit.m_RmtNodeNameVstrPt, Utf8, , RmtNodeNameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点名称动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_UserMsgBdctCnctInit.m_RmtNodeSrvcVstrPt, Utf8, , RmtNodeSrvcVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点服务动态字符串失败。" ), , );
		goto Out;
	}
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgBdctCnctInit, sizeof( p_UserMsgBdctCnctInit ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//广播连接全部销毁。
int NtwkMediaPocsThrdBdctCnctAllDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd::UserMsgBdctCnctAllDstoy p_UserMsgBdctCnctAllDstoy;
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgBdctCnctAllDstoy.m_UserMsgTyp = NtwkMediaPocsThrd::UserMsgTypBdctCnctAllDstoy;
	if( MediaPocsThrdSendUserMsg( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, &p_UserMsgBdctCnctAllDstoy, sizeof( p_UserMsgBdctCnctAllDstoy ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送用户消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//判断是否自动请求退出。
void NtwkMediaPocsThrdIsAutoRqirExit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	size_t p_ElmTotal; //存放元素总数。
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( NtwkMediaPocsThrdPt->m_IsAutoRqirExit == 0 )
	{

	}
	else if( NtwkMediaPocsThrdPt->m_IsAutoRqirExit == 1 )
	{
		if( ( CQueueGetTotal( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ), p_ElmTotal == 0 ) &&
			( CQueueGetTotal( NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ), p_ElmTotal == 0 ) )
		{
			MediaPocsThrdRqirExit( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, 1, 0, NULL );

			VstrCpy( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：所有连接已销毁，自动请求退出。" ), , );
			if( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( NtwkMediaPocsThrdPt, NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	else if( NtwkMediaPocsThrdPt->m_IsAutoRqirExit == 2 )
	{
		if( ( CQueueGetTotal( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ), p_ElmTotal == 0 ) &&
			( CQueueGetTotal( NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ), p_ElmTotal == 0 ) &&
			( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_SrvrIsInit == 0 ) )
		{
			MediaPocsThrdRqirExit( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, 1, 0, NULL );

			VstrCpy( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：所有连接和服务端已销毁，自动请求退出。" ), , );
			if( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( NtwkMediaPocsThrdPt, NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//设置对讲模式。
void NtwkMediaPocsThrdSetTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	int p_RmtTkbkMode; //存放远端对讲模式。
	int p_RealTkbkMode; //存放实际对讲模式。
	int p_IsUseAdoInpt = 0;
	int p_IsUseAdoOtpt = 0;
	int p_IsUseVdoInpt = 0;
	int p_IsUseVdoOtpt = 0;
	size_t p_ElmTotal; //存放元素总数。
	
	//判断各个变量是否正确。
	if( NtwkMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	//设置远端对讲模式。
	if( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt != NULL ) p_RmtTkbkMode = NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt->m_RmtTkbkMode;
	else p_RmtTkbkMode = NtwkMediaPocsThrd::TkbkModeNone;

	//设置实际对讲模式。
	p_RealTkbkMode = NtwkMediaPocsThrdPt->m_LclTkbkMode & p_RmtTkbkMode;

	if( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		if( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_PttBtnIsDown != 0 ) //如果一键即按即通按钮为按下。
		{
			if( ( NtwkMediaPocsThrdPt->m_LclTkbkMode & NtwkMediaPocsThrd::TkbkModeAdo ) != 0 )
			{
				p_IsUseAdoInpt = 1;
			}
			if( ( NtwkMediaPocsThrdPt->m_LclTkbkMode & NtwkMediaPocsThrd::TkbkModeVdo ) != 0 )
			{
				p_IsUseVdoInpt = 1;
			}
		}
		else //如果一键即按即通按钮为弹起。
		{
			if( ( p_RealTkbkMode & NtwkMediaPocsThrd::TkbkModeAdo ) != 0 )
			{
				p_IsUseAdoOtpt = 1;
			}
			if( ( p_RealTkbkMode & NtwkMediaPocsThrd::TkbkModeVdo ) != 0 )
			{
				p_IsUseVdoOtpt = 1;
			}
		}
	}
	else //如果传输模式为实时全双工。
	{
		if( ( p_RealTkbkMode & NtwkMediaPocsThrd::TkbkModeAdo ) != 0 )
		{
			p_IsUseAdoInpt = 1;
			p_IsUseAdoOtpt = 1;
		}
		if( ( p_RealTkbkMode & NtwkMediaPocsThrd::TkbkModeVdo ) != 0 )
		{
			p_IsUseVdoInpt = 1;
			p_IsUseVdoOtpt = 1;
		}
	}

	if( CQueueGetTotal( NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ), p_ElmTotal != 0 ) //如果有广播连接。
	{
		p_IsUseAdoInpt = 1;
	}

	MediaPocsThrdSetIsUseAdoVdoInptOtpt( NtwkMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, p_IsUseAdoInpt, p_IsUseAdoOtpt, p_IsUseVdoInpt, p_IsUseVdoOtpt, NULL ); //设置是否使用音视频输入输出。

	if( NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode == 0 ) TkbkNtwkRecvOtptFrmReset( &NtwkMediaPocsThrdPt->m_TkbkNtwk ); //接收输出帧重置。防止在实时半双工（一键通）模式下每次按下PTT时还有点点播放上次按下的声音。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//用户定义的初始化函数。
int __cdecl NtwkMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	p_NtwkMediaPocsThrdPt->m_UserNtwkMediaPocsThrdInitFuncPt( p_NtwkMediaPocsThrdPt ); //网络媒体处理线程初始化。

	TkbkNtwkRecvOtptFrmInit( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk ); //接收输出帧初始化。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//用户定义的处理函数。
int __cdecl NtwkMediaPocsThrdUserPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	TkbkNtwkCnctPocs( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk );

	BdctNtwkCnctPocs( &p_NtwkMediaPocsThrdPt->m_BdctNtwk );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//用户定义的销毁函数。
void __cdecl NtwkMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	BdctNtwkCnctInfoAllDstoy( &p_NtwkMediaPocsThrdPt->m_BdctNtwk ); //连接信息全部销毁。

	TkbkNtwkCnctInfoAllDstoy( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk ); //连接信息全部销毁。
	TkbkNtwkSrvrDstoy( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk ); //服务端销毁。
	TkbkNtwkRecvOtptFrmDstoy( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk ); //接收输出帧销毁。

	//销毁本端高级Udp协议客户端套接字。
	if( p_NtwkMediaPocsThrdPt->m_AudpClntSoktPt != NULL )
	{
		AudpDstoy( p_NtwkMediaPocsThrdPt->m_AudpClntSoktPt, NULL ); //关闭并销毁本端高级Udp协议客户端套接字。
		p_NtwkMediaPocsThrdPt->m_AudpClntSoktPt = NULL;

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "网络媒体处理线程：关闭并销毁本端高级Udp协议客户端套接字成功。" ), , );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_NtwkMediaPocsThrdPt->m_UserShowLogFuncPt( p_NtwkMediaPocsThrdPt, p_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	p_NtwkMediaPocsThrdPt->m_UserNtwkMediaPocsThrdDstoyFuncPt( p_NtwkMediaPocsThrdPt ); //调用用户定义的网络媒体处理线程销毁函数。
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//用户定义的消息函数。
int __cdecl NtwkMediaPocsThrdUserMsg( MediaPocsThrd * MediaPocsThrdPt, void * UserMsgPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	switch( ( ( NtwkMediaPocsThrd::UserMsg * )UserMsgPt )->m_UserMsgTyp )
	{
		case NtwkMediaPocsThrd::UserMsgTypSrvrInit:
		{
			TkbkNtwkSrvrInit( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, ( ( NtwkMediaPocsThrd::UserMsgSrvrInit * )UserMsgPt )->m_SrvrUrlVstrPt );
			VstrDstoy( ( ( NtwkMediaPocsThrd::UserMsgSrvrInit * )UserMsgPt )->m_SrvrUrlVstrPt );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypSrvrDstoy:
		{
			TkbkNtwkSrvrDstoy( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypCnctInit:
		{
			TkbkNtwkCnctInit( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, ( ( NtwkMediaPocsThrd::UserMsgCnctInit * )UserMsgPt )->m_IsTcpOrAudpPrtcl, ( ( NtwkMediaPocsThrd::UserMsgCnctInit * )UserMsgPt )->m_RmtNodeNameVstrPt, ( ( NtwkMediaPocsThrd::UserMsgCnctInit * )UserMsgPt )->m_RmtNodeSrvcVstrPt );
			VstrDstoy( ( ( NtwkMediaPocsThrd::UserMsgCnctInit * )UserMsgPt )->m_RmtNodeNameVstrPt );
			VstrDstoy( ( ( NtwkMediaPocsThrd::UserMsgCnctInit * )UserMsgPt )->m_RmtNodeSrvcVstrPt );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypCnctAct:
		{
			TkbkNtwkCnctAct( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, ( ( NtwkMediaPocsThrd::UserMsgCnctAct * )UserMsgPt )->m_CnctNum );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypCnctDstoy:
		{
			TkbkNtwkCnctDstoy( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, ( ( NtwkMediaPocsThrd::UserMsgCnctDstoy * )UserMsgPt )->m_CnctNum );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypLclTkbkMode:
		{
			if( ( ( NtwkMediaPocsThrd::UserMsgLclTkbkMode * )UserMsgPt )->m_LclTkbkMode != NtwkMediaPocsThrd::TkbkModeNoChg ) p_NtwkMediaPocsThrdPt->m_LclTkbkMode = ( ( NtwkMediaPocsThrd::UserMsgLclTkbkMode * )UserMsgPt )->m_LclTkbkMode; //设置本端对讲模式。
			if( p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt != NULL ) //如果当前激活的连接信息的指针不为空。
			{
				TkbkNtwkCnctInfoUpdtLclTkbkMode( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt, p_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //当前激活的连接信息更新本端对讲模式。
			}
			else
			{
				TkbkNtwkCnctInfoAllUpdtLclTkbkMode( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, p_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //连接信息全部更新本端对讲模式。
			}
			NtwkMediaPocsThrdSetTkbkMode( p_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypPttBtnDown:
		{
			p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_PttBtnIsDown = 1; //设置一键即按即通按钮为按下。
			if( p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt != NULL ) //如果当前激活的连接信息的指针不为空。
			{
				TkbkNtwkCnctInfoSendTkbkModePkt( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt, p_NtwkMediaPocsThrdPt->m_LclTkbkMode ); //发送对讲模式包。
				NtwkMediaPocsThrdSetTkbkMode( p_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。
			}
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypPttBtnUp:
		{
			p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_PttBtnIsDown = 0; //设置一键即按即通按钮为弹起。
			if( p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt != NULL ) //如果当前激活的连接信息的指针不为空。
			{
				TkbkNtwkCnctInfoSendTkbkModePkt( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt, NtwkMediaPocsThrd::TkbkModeNone ); //发送对讲模式包。
				NtwkMediaPocsThrdSetTkbkMode( p_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。
			}
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypBdctCnctInit:
		{
			BdctNtwkCnctInit( &p_NtwkMediaPocsThrdPt->m_BdctNtwk, ( ( NtwkMediaPocsThrd::UserMsgBdctCnctInit * )UserMsgPt )->m_IsTcpOrAudpPrtcl, ( ( NtwkMediaPocsThrd::UserMsgBdctCnctInit * )UserMsgPt )->m_RmtNodeNameVstrPt, ( ( NtwkMediaPocsThrd::UserMsgBdctCnctInit * )UserMsgPt )->m_RmtNodeSrvcVstrPt );
			VstrDstoy( ( ( NtwkMediaPocsThrd::UserMsgBdctCnctInit * )UserMsgPt )->m_RmtNodeNameVstrPt );
			VstrDstoy( ( ( NtwkMediaPocsThrd::UserMsgBdctCnctInit * )UserMsgPt )->m_RmtNodeSrvcVstrPt );
			break;
		}
		case NtwkMediaPocsThrd::UserMsgTypBdctCnctAllDstoy:
		{
			BdctNtwkCnctInfoAllDstoy( &p_NtwkMediaPocsThrdPt->m_BdctNtwk );
			NtwkMediaPocsThrdSetTkbkMode( p_NtwkMediaPocsThrdPt, 1, NULL ); //设置对讲模式。
			break;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//用户定义的读取音视频输入帧函数。
void __cdecl NtwkMediaPocsThrdUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt,
											 int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
											 uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
											 uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
											 uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
											 uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt )
{
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。
	size_t p_ElmTotal; //存放元素总数。

	if( p_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_CurActCnctInfoPt != NULL )
	{
		TkbkNtwkUserReadAdoVdoInptFrm( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, AdoInptPcmSrcFrmPt,
									   AdoInptPcmRsltFrmPt, AdoInptPcmFrmLenUnit, AdoInptPcmRsltFrmVoiceActSts,
									   AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt, AdoInptEncdRsltFrmIsNeedTrans,
									   VdoInptBgraSrcFrmPt, VdoInptBgraSrcFrmWidth, VdoInptBgraSrcFrmHeight, VdoInptBgraSrcFrmLenByt,
									   VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth, VdoInptYu12RsltFrmHeight, VdoInptYu12RsltFrmLenByt,
									   VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt );
	}

	if( ( AdoInptPcmSrcFrmPt != NULL ) && ( CQueueGetTotal( p_NtwkMediaPocsThrdPt->m_BdctNtwk.m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL ),p_ElmTotal != 0 ) )
	{
		BdctNtwkUserReadAdoVdoInptFrm( &p_NtwkMediaPocsThrdPt->m_BdctNtwk,
									   AdoInptPcmSrcFrmPt, AdoInptPcmRsltFrmPt, AdoInptPcmFrmLenUnit, AdoInptPcmRsltFrmVoiceActSts,
									   AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt, AdoInptEncdRsltFrmIsNeedTrans,
									   VdoInptBgraSrcFrmPt, VdoInptBgraSrcFrmWidth, VdoInptBgraSrcFrmHeight, VdoInptBgraSrcFrmLenByt,
									   VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth, VdoInptYu12RsltFrmHeight, VdoInptYu12RsltFrmLenByt,
									   VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt );
	}
}

//用户定义的写入音频输出帧函数。
void __cdecl NtwkMediaPocsThrdUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										   int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										   uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	TkbkNtwkUserWriteAdoOtptFrm( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, AdoOtptStrmIdx,
								 AdoOtptPcmSrcFrmPt, AdoOtptPcmFrmLenUnit,
								 AdoOtptEncdSrcFrmPt, AdoOtptEncdSrcFrmSzByt, AdoOtptEncdSrcFrmLenBytPt );
}

//用户定义的获取音频输出帧函数。
void __cdecl NtwkMediaPocsThrdUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
										 int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
										 uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{

}

//用户定义的写入视频输出帧函数。
void __cdecl NtwkMediaPocsThrdUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										   uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
										   uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	NtwkMediaPocsThrd * p_NtwkMediaPocsThrdPt = ( NtwkMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放网络媒体处理线程的指针。

	TkbkNtwkUserWriteVdoOtptFrm( &p_NtwkMediaPocsThrdPt->m_TkbkNtwk, VdoOtptStrmIdx,
								 VdoOtptYu12SrcFrmPt, VdoOtptYu12SrcFrmWidthPt, VdoOtptYu12SrcFrmHeightPt,
								 VdoOtptEncdSrcFrmPt, VdoOtptEncdSrcFrmSzByt, VdoOtptEncdSrcFrmLenBytPt );
}

//用户定义的获取视频输出帧函数。
void __cdecl NtwkMediaPocsThrdUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
										 uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
										 uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{

}

//回调NtwkMediaPocsThrdCls类的用户定义的网络媒体处理线程初始化函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserNtwkMediaPocsThrdInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserNtwkMediaPocsThrdInit();
}

//回调NtwkMediaPocsThrdCls类的用户定义的网络媒体处理线程销毁函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserNtwkMediaPocsThrdDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserNtwkMediaPocsThrdDstoy();
}

//回调NtwkMediaPocsThrdCls类的用户定义的服务端初始化函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserSrvrInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserSrvrInit();
}

//回调NtwkMediaPocsThrdCls类的用户定义的服务端销毁函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserSrvrDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserSrvrDstoy();
}

//回调NtwkMediaPocsThrdCls类的用户定义的显示日志函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserShowLog( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * InfoVstrPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserShowLog( InfoVstrPt );
}

//回调NtwkMediaPocsThrdCls类的用户定义的显示Toast函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserShowToast( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, Vstr * InfoVstrPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserShowToast( InfoVstrPt );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接初始化函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctInit( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctInit( CnctInfoPt, IsSrvrOrClntCnct, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接状态函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctSts( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int CurCnctSts )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctSts( CnctInfoPt, CurCnctSts );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接激活函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctAct( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsAct )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctAct( CnctInfoPt, IsAct );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接本端对讲模式函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctLclTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int LclTkbkMode )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctLclTkbkMode( CnctInfoPt, IsRqirAct, LclTkbkMode );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接远端对讲模式函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctRmtTkbkMode( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int RmtTkbkMode )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctRmtTkbkMode( CnctInfoPt, IsRqirAct, RmtTkbkMode );
}

//回调NtwkMediaPocsThrdCls类的用户定义的连接远端对讲模式函数。
extern "C" void __cdecl NtwkMediaPocsThrdClsUserCnctDstoy( NtwkMediaPocsThrd * NtwkMediaPocsThrdPt, TkbkNtwk::CnctInfo * CnctInfoPt )
{
	( ( NtwkMediaPocsThrdCls * )NtwkMediaPocsThrdPt->m_UserDataPt )->UserCnctDstoy( CnctInfoPt );
}

class asd
{
	virtual int UserInit() = 0;
};

int NtwkMediaPocsThrdCls::Init( VstrCls * ErrInfoVstrPt )
{
	int p_Rslt = NtwkMediaPocsThrdInit( &m_NtwkMediaPocsThrdPt, this,
										NtwkMediaPocsThrdClsUserNtwkMediaPocsThrdInit, NtwkMediaPocsThrdClsUserNtwkMediaPocsThrdDstoy,
										NtwkMediaPocsThrdClsUserSrvrInit, NtwkMediaPocsThrdClsUserSrvrDstoy,
										NtwkMediaPocsThrdClsUserShowLog, NtwkMediaPocsThrdClsUserShowToast,
										NtwkMediaPocsThrdClsUserCnctInit, NtwkMediaPocsThrdClsUserCnctSts, NtwkMediaPocsThrdClsUserCnctAct, NtwkMediaPocsThrdClsUserCnctLclTkbkMode, NtwkMediaPocsThrdClsUserCnctRmtTkbkMode, NtwkMediaPocsThrdClsUserCnctDstoy,
										( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL );
	if( p_Rslt == 0 )
	{
		m_MediaPocsThrdClsPt = ( MediaPocsThrdCls * )( &m_NtwkMediaPocsThrdPt->m_MediaPocsThrdPt - 1 );
	}
	return p_Rslt;
}
