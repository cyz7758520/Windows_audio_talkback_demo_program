#include "ClntMediaPocsThrd.h"

const char * const g_TkbkModeU8strArrPt[ 17 ] = {
	"挂起", //0：挂起。
	"音入", //1：音频输入。
	"音出", //2：音频输出。
	"音入出", //3：音频输入、音频输出。
	"视入", //4：视频输入。
	"音入视入", //5：音频输入、视频输入。
	"音出视入", //6：音频输出、视频输入。
	"音入出视入", //7：音频输入、音频输出、视频输入。
	"视出", //8：视频输出。
	"音入视出", //9：音频输入、视频输出。
	"音出视出", //10：音频输出、视频输出。
	"音入出视出", //11：音频输入、音频输出、视频输出。
	"视入出", //12：视频输入、视频输出。
	"音入视入出", //13：音频输入、视频输入、视频输出。
	"音出视入出", //14：音频输出、视频输入、视频输出。
	"音入出视入出", //15：音频输入、音频输出、视频输入、视频输出。
	"不变", //16：不变。
};

//客户端媒体处理线程初始化。
int ClntMediaPocsThrdInit( ClntMediaPocsThrd * * ClntMediaPocsThrdPtPt, const void * LicnCodePt, void * UserDataPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserShowLogFuncPt UserShowLogFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserShowToastFuncPt UserShowToastFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserClntMediaPocsThrdInitFuncPt UserClntMediaPocsThrdInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserClntMediaPocsThrdDstoyFuncPt UserClntMediaPocsThrdDstoyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctInitFuncPt UserTkbkClntCnctInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctDstoyFuncPt UserTkbkClntCnctDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntCnctStsFuncPt UserTkbkClntCnctStsFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntMyTkbkIdxFuncPt UserTkbkClntMyTkbkIdxFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntLclTkbkModeFuncPt UserTkbkClntLclTkbkModeFuncPt, 
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoInitFuncPt UserTkbkClntTkbkInfoInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoDstoyFuncPt UserTkbkClntTkbkInfoDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTkbkInfoRmtTkbkModeFuncPt UserTkbkClntTkbkInfoRmtTkbkModeFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserTkbkClntTstNtwkDlyFuncPt UserTkbkClntTstNtwkDlyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntInitFuncPt UserBdctClntInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntDstoyFuncPt UserBdctClntDstoyFuncPt,
						   ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctInitFuncPt UserBdctClntCnctInitFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctDstoyFuncPt UserBdctClntCnctDstoyFuncPt, ClntMediaPocsThrd::ClntMediaPocsThrdUserBdctClntCnctStsFuncPt UserBdctClntCnctStsFuncPt,
						   Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = NULL; //存放客户端媒体处理线程的指针。

	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针的指针不正确。" ), , );
		goto Out;
	}
	
	//创建并清空客户端媒体处理线程内存块。
	p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )calloc( 1, sizeof( ClntMediaPocsThrd ) );
	if( p_ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空客户端媒体处理线程内存块失败。" ), , );
		goto Out;
	}
	
	//初始化媒体处理线程。
	if( MediaPocsThrdInit( &p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt, LicnCodePt, p_ClntMediaPocsThrdPt,
						   ClntMediaPocsThrdUserInit, ClntMediaPocsThrdUserPocs, ClntMediaPocsThrdUserDstoy, ClntMediaPocsThrdUserMsg,
						   ClntMediaPocsThrdUserReadAdoVdoInptFrm,
						   ClntMediaPocsThrdUserWriteAdoOtptFrm, ClntMediaPocsThrdUserGetAdoOtptFrm,
						   ClntMediaPocsThrdUserWriteVdoOtptFrm, ClntMediaPocsThrdUserGetVdoOtptFrm, ErrInfoVstrPt ) )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化媒体处理线程失败。原因：" ) );
		goto Out;
	}

	//初始化对讲客户端。
	p_ClntMediaPocsThrdPt->m_TkbkClnt.m_ClntMediaPocsThrdPt = p_ClntMediaPocsThrdPt;
	if( CQueueInit( &p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, sizeof( TkbkClnt::TkbkInfo ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化对讲客户端的对讲信息容器失败。原因" ) );
		goto Out;
	}
	p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCurMaxNum = -1; //设置对讲信息的当前最大序号。
	p_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode = ClntMediaPocsThrd::TkbkModeNone; //设置本端对讲模式为挂起。

	//初始化广播客户端。
	p_ClntMediaPocsThrdPt->m_BdctClnt.m_ClntMediaPocsThrdPt = p_ClntMediaPocsThrdPt;
	if( CQueueInit( &p_ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCntnrPt, sizeof( BdctClnt::CnctInfo ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化广播客户端的连接信息容器失败。原因" ) );
		goto Out;
	}
	p_ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCurMaxNum = -1; //设置连接信息的当前最大序号。
	p_ClntMediaPocsThrdPt->m_BdctClnt.m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	p_ClntMediaPocsThrdPt->m_BdctClnt.m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。

	//初始化客户端媒体处理线程。
	if( VstrInit( &p_ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( p_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt == NULL )
	{
		p_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( p_ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt == NULL )
		{
			VstrCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
			if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_ClntMediaPocsThrdPt->m_Thrd.m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据大小。
	}
	
	p_ClntMediaPocsThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt = UserShowLogFuncPt; //设置用户定义的显示日志函数的指针。
	p_ClntMediaPocsThrdPt->m_UserShowToastFuncPt = UserShowToastFuncPt; //设置用户定义的显示Toast函数的指针。
	p_ClntMediaPocsThrdPt->m_UserClntMediaPocsThrdInitFuncPt = UserClntMediaPocsThrdInitFuncPt; //设置用户定义的客户端媒体处理线程初始化函数的指针。
	p_ClntMediaPocsThrdPt->m_UserClntMediaPocsThrdDstoyFuncPt = UserClntMediaPocsThrdDstoyFuncPt; //设置用户定义的客户端媒体处理线程销毁函数的指针。
	
	p_ClntMediaPocsThrdPt->m_UserTkbkClntCnctInitFuncPt = UserTkbkClntCnctInitFuncPt; //设置用户定义的对讲客户端连接初始化函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntCnctDstoyFuncPt = UserTkbkClntCnctDstoyFuncPt; //设置用户定义的对讲客户端连接销毁函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntCnctStsFuncPt = UserTkbkClntCnctStsFuncPt; //设置用户定义的对讲客户端连接状态函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntMyTkbkIdxFuncPt = UserTkbkClntMyTkbkIdxFuncPt; //设置用户定义的对讲客户端我的对讲索引函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntLclTkbkModeFuncPt = UserTkbkClntLclTkbkModeFuncPt; //设置用户定义的对讲客户端本端对讲模式函数的指针。

	p_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoInitFuncPt = UserTkbkClntTkbkInfoInitFuncPt; //设置用户定义的对讲客户端对讲信息初始化函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoDstoyFuncPt = UserTkbkClntTkbkInfoDstoyFuncPt; //设置用户定义的对讲客户端对讲信息销毁函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntTkbkInfoRmtTkbkModeFuncPt = UserTkbkClntTkbkInfoRmtTkbkModeFuncPt; //设置用户定义的对讲客户端对讲信息远端对讲模式函数的指针。
	p_ClntMediaPocsThrdPt->m_UserTkbkClntTstNtwkDlyFuncPt = UserTkbkClntTstNtwkDlyFuncPt; //设置用户定义的对讲客户端测试网络延迟函数的指针。

	p_ClntMediaPocsThrdPt->m_UserBdctClntInitFuncPt = UserBdctClntInitFuncPt; //设置用户定义的广播客户端初始化函数的指针。
	p_ClntMediaPocsThrdPt->m_UserBdctClntDstoyFuncPt = UserBdctClntDstoyFuncPt; //设置用户定义的广播客户端销毁函数的指针。
	p_ClntMediaPocsThrdPt->m_UserBdctClntCnctInitFuncPt = UserBdctClntCnctInitFuncPt; //设置用户定义的广播客户端连接初始化函数的指针。
	p_ClntMediaPocsThrdPt->m_UserBdctClntCnctDstoyFuncPt = UserBdctClntCnctDstoyFuncPt; //设置用户定义的广播客户端连接销毁函数的指针。
	p_ClntMediaPocsThrdPt->m_UserBdctClntCnctStsFuncPt = UserBdctClntCnctStsFuncPt; //设置用户定义的广播客户端连接状态函数的指针。

	*ClntMediaPocsThrdPtPt = p_ClntMediaPocsThrdPt; //设置我的客户端媒体处理线程的指针。

	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//客户端媒体处理线程销毁。
int ClntMediaPocsThrdDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}
	
	//销毁媒体处理线程。因为要先让媒体处理线程停下来。
	MediaPocsThrdDstoy( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, NULL );

	//销毁客户端媒体处理线程。
	VstrDstoy( ClntMediaPocsThrdPt->m_Thrd.m_LclNodeAddrPt ); //销毁本端节点地址动态字符串。
	VstrDstoy( ClntMediaPocsThrdPt->m_Thrd.m_LclNodePortPt ); //销毁本端节点端口动态字符串。
	VstrDstoy( ClntMediaPocsThrdPt->m_Thrd.m_RmtNodeAddrPt ); //销毁远端节点地址动态字符串。
	VstrDstoy( ClntMediaPocsThrdPt->m_Thrd.m_RmtNodePortPt ); //销毁远端节点端口动态字符串。
	if( ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt != NULL ) free( ClntMediaPocsThrdPt->m_Thrd.m_TmpBytePt ); //销毁临时数据的内存块。
	
	//销毁广播客户端。
	CQueueDstoy( ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCntnrPt, NULL ); //销毁广播客户端的连接信息容器。
	
	//销毁对讲客户端。
	CQueueDstoy( ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, NULL ); //销毁对讲客户端的对讲信息容器。

	//销毁客户端媒体处理线程内存块。
	free( ClntMediaPocsThrdPt );
	
	p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//发送对讲客户端设置是否测试网络延迟消息。
int ClntMediaPocsThrdSendTkbkClntSetIsTstNtwkDlyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntSetIsTstNtwkDly p_UserMsgTkbkClntSetIsTstNtwkDly;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgTkbkClntSetIsTstNtwkDly.m_IsTstNtwkDly = IsTstNtwkDly;
	p_UserMsgTkbkClntSetIsTstNtwkDly.m_SendIntvlMsec = SendIntvlMsec;
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntSetIsTstNtwkDly, &p_UserMsgTkbkClntSetIsTstNtwkDly, sizeof( p_UserMsgTkbkClntSetIsTstNtwkDly ), ErrInfoVstrPt ) != 0 )
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

//发送对讲客户端的连接初始化消息。
int ClntMediaPocsThrdSendTkbkClntCnctInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntCnctInit p_UserMsgTkbkClntCnctInit;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgTkbkClntCnctInit.m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	if( VstrInit( &p_UserMsgTkbkClntCnctInit.m_RmtNodeNameVstrPt, Utf8, , RmtNodeNameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点名称动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_UserMsgTkbkClntCnctInit.m_RmtNodeSrvcVstrPt, Utf8, , RmtNodeSrvcVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点服务动态字符串失败。" ), , );
		goto Out;
	}
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntCnctInit, &p_UserMsgTkbkClntCnctInit, sizeof( p_UserMsgTkbkClntCnctInit ), ErrInfoVstrPt ) != 0 )
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

//发送对讲客户端的连接销毁消息。
int ClntMediaPocsThrdSendTkbkClntCnctDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntCnctDstoy p_UserMsgTkbkClntCnctDstoy;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntCnctDstoy, &p_UserMsgTkbkClntCnctDstoy, sizeof( p_UserMsgTkbkClntCnctDstoy ), ErrInfoVstrPt ) != 0 )
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

//发送对讲客户端的本端对讲模式消息。
int ClntMediaPocsThrdSendTkbkClntLclTkbkModeMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t LclTkbkMode, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntLclTkbkMode p_UserMsgTkbkClntLclTkbkMode;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgTkbkClntLclTkbkMode.m_LclTkbkMode = LclTkbkMode;
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntLclTkbkMode, &p_UserMsgTkbkClntLclTkbkMode, sizeof( p_UserMsgTkbkClntLclTkbkMode ), ErrInfoVstrPt ) != 0 )
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

//发送对讲客户端的一键即按即通按钮按下消息。
int ClntMediaPocsThrdSendTkbkClntPttBtnDownMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntPttBtnDown p_UserMsgTkbkClntPttBtnDown;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntPttBtnDown, &p_UserMsgTkbkClntPttBtnDown, sizeof( p_UserMsgTkbkClntPttBtnDown ), ErrInfoVstrPt ) != 0 )
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

//发送对讲客户端的一键即按即通按钮弹起消息。
int ClntMediaPocsThrdSendTkbkClntPttBtnUpMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgTkbkClntPttBtnUp p_UserMsgTkbkClntPttBtnUp;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypTkbkClntPttBtnUp, &p_UserMsgTkbkClntPttBtnUp, sizeof( p_UserMsgTkbkClntPttBtnUp ), ErrInfoVstrPt ) != 0 )
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

//发送广播客户端初始化消息。
int ClntMediaPocsThrdSendBdctClntInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t CnctNumIsDecr, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgBdctClntInit p_UserMsgBdctClntInit;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgBdctClntInit.m_CnctNumIsDecr = CnctNumIsDecr;
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypBdctClntInit, &p_UserMsgBdctClntInit, sizeof( p_UserMsgBdctClntInit ), ErrInfoVstrPt ) != 0 )
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

//发送广播客户端销毁消息。
int ClntMediaPocsThrdSendBdctClntDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgBdctClntDstoy p_UserMsgBdctClntDstoy;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypBdctClntDstoy, &p_UserMsgBdctClntDstoy, sizeof( p_UserMsgBdctClntDstoy ), ErrInfoVstrPt ) != 0 )
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

//发送广播客户端的连接初始化消息。
int ClntMediaPocsThrdSendBdctClntCnctInitMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgBdctClntCnctInit p_UserMsgBdctClntCnctInit;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgBdctClntCnctInit.m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	if( VstrInit( &p_UserMsgBdctClntCnctInit.m_RmtNodeNameVstrPt, Utf8, , RmtNodeNameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点名称动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_UserMsgBdctClntCnctInit.m_RmtNodeSrvcVstrPt, Utf8, , RmtNodeSrvcVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置远端套接字绑定的远端节点服务动态字符串失败。" ), , );
		goto Out;
	}
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypBdctClntCnctInit, &p_UserMsgBdctClntCnctInit, sizeof( p_UserMsgBdctClntCnctInit ), ErrInfoVstrPt ) != 0 )
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

//发送广播客户端的连接销毁消息。
int ClntMediaPocsThrdSendBdctClntCnctDstoyMsg( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int32_t CnctNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd::UserMsgBdctClntCnctDstoy p_UserMsgBdctClntCnctDstoy;
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	p_UserMsgBdctClntCnctDstoy.m_CnctNum = CnctNum;
	if( MediaPocsThrdSendUserMsg( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ClntMediaPocsThrd::UserMsgTypBdctClntCnctDstoy, &p_UserMsgBdctClntCnctDstoy, sizeof( p_UserMsgBdctClntCnctDstoy ), ErrInfoVstrPt ) != 0 )
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
void ClntMediaPocsThrdIsAutoRqirExit( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	size_t p_ElmTotal; //存放元素总数。
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程的指针不正确。" ), , );
		goto Out;
	}

	if( ClntMediaPocsThrdPt->m_IsAutoRqirExit == 0 )
	{

	}
	else if( ClntMediaPocsThrdPt->m_IsAutoRqirExit == 1 )
	{
		if( ( ClntMediaPocsThrdPt->m_TkbkClnt.m_CnctIsInit == 0 ) && ( ClntMediaPocsThrdPt->m_BdctClnt.m_IsInit == 0 ) )
		{
			MediaPocsThrdRqirExit( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, 0, 1, NULL );

			VstrCpy( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：所有连接已销毁，自动请求退出。" ), , );
			if( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			ClntMediaPocsThrdPt->m_UserShowLogFuncPt( ClntMediaPocsThrdPt, ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
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
void ClntMediaPocsThrdSetTkbkMode( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int IsBlockWait, int SetMode )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	int p_RealTkbkMode; //存放实际对讲模式。
	int p_IsUseAdoInpt = 0;
	int p_IsUseAdoOtpt = 0;
	int p_IsUseVdoInpt = 0;
	int p_IsUseVdoOtpt = 0;
	size_t p_ElmTotal; //存放元素总数。
	
	//判断各个变量是否正确。
	if( ClntMediaPocsThrdPt == NULL )
	{
		goto Out;
	}

	//设置实际对讲模式。
	p_RealTkbkMode = ClntMediaPocsThrd::TkbkModeNone; //设置实际对讲模式为挂起。
	if( ClntMediaPocsThrdPt->m_TkbkClnt.m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) //如果对讲客户端已连接。
	{
		if( ClntMediaPocsThrdPt->m_IsReferRmtTkbkModeSetTkbkMode != 0 ) //如果要参考远端对讲模式来设置对讲模式。
		{
			TkbkClnt::TkbkInfo * p_TkbkClntTkbkInfoTmpPt;
			int p_TmpTkbkMode = 0; //存放临时对讲模式。

			//综合全部的远端对讲模式。
			for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkClntTkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
			{
				if( p_TkbkClntTkbkInfoTmpPt->m_IsInit != 0 )
				{
					p_TmpTkbkMode |= p_TkbkClntTkbkInfoTmpPt->m_RmtTkbkMode;
				}
			}

			if( ( ( ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) && ( ( p_TmpTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) )
			{
				p_RealTkbkMode |= ClntMediaPocsThrd::TkbkModeAdoInpt;
			}
			if( ( ( ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) && ( ( p_TmpTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) )
			{
				p_RealTkbkMode |= ClntMediaPocsThrd::TkbkModeAdoOtpt;
			}
			if( ( ( ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) && ( ( p_TmpTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) )
			{
				p_RealTkbkMode |= ClntMediaPocsThrd::TkbkModeVdoInpt;
			}
			if( ( ( ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) && ( ( p_TmpTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) )
			{
				p_RealTkbkMode |= ClntMediaPocsThrd::TkbkModeVdoOtpt;
			}
		}
		else //如果不参考远端对讲模式来设置对讲模式。
		{
			p_RealTkbkMode = ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode;
		}
	}

	if( ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		if( ClntMediaPocsThrdPt->m_TkbkClnt.m_PttBtnIsDown != 0 ) //如果一键即按即通按钮为按下。
		{
			if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) p_IsUseAdoInpt = 1;
			if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) p_IsUseVdoInpt = 1;
		}
		else //如果一键即按即通按钮为弹起。
		{
			if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) p_IsUseAdoOtpt = 1;
			if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) p_IsUseVdoOtpt = 1;
		}
	}
	else //如果传输模式为实时全双工。
	{
		if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) p_IsUseAdoInpt = 1;
		if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) p_IsUseAdoOtpt = 1;
		if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) p_IsUseVdoInpt = 1;
		if( ( p_RealTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) p_IsUseVdoOtpt = 1;
	}

	if( ClntMediaPocsThrdPt->m_BdctClnt.m_IsInit != 0 ) p_IsUseAdoInpt = 1; //如果广播客户端已初始化，就要使用音频输入。
	
	//设置是否使用音视频输入输出。
	if( SetMode == 0 ) //如果同时设置不使用和要使用。
	{
		if( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：设置对讲模式：IsUseAdoInpt：%d，IsUseAdoOtpt：%d，IsUseVdoInpt：%d，IsUseVdoOtpt：%d。"), p_IsUseAdoInpt, p_IsUseAdoOtpt, p_IsUseVdoInpt, p_IsUseVdoOtpt );
		MediaPocsThrdSetIsUseAdoVdoInptOtpt( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, p_IsUseAdoInpt, p_IsUseAdoOtpt, p_IsUseVdoInpt, p_IsUseVdoOtpt, NULL );
	}
	else if( SetMode == 1 ) //如果只设置不使用。
	{
		if( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：设置对讲模式：IsUseAdoInpt：%d，IsUseAdoOtpt：%d，IsUseVdoInpt：%d，IsUseVdoOtpt：%d。"), ( ( p_IsUseAdoInpt == 0 ) ? 0 : -1 ), ( ( p_IsUseAdoOtpt == 0 ) ? 0 : -1 ), ( ( p_IsUseVdoInpt == 0 ) ? 0 : -1 ), ( ( p_IsUseVdoOtpt == 0 ) ? 0 : -1 ) );
		MediaPocsThrdSetIsUseAdoVdoInptOtpt( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ( p_IsUseAdoInpt == 0 ) ? 0 : -1, ( p_IsUseAdoOtpt == 0 ) ? 0 : -1, ( p_IsUseVdoInpt == 0 ) ? 0 : -1, ( p_IsUseVdoOtpt == 0 ) ? 0 : -1, NULL );
	}
	else if( SetMode == 2 ) //如果只设置要使用。
	{
		if( ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "客户端媒体处理线程：设置对讲模式：IsUseAdoInpt：%d，IsUseAdoOtpt：%d，IsUseVdoInpt：%d，IsUseVdoOtpt：%d。" ), ( ( p_IsUseAdoInpt != 0 ) ? 1 : -1 ), ( ( p_IsUseAdoOtpt != 0 ) ? 1 : -1 ), ( ( p_IsUseVdoInpt != 0 ) ? 1 : -1 ), ( ( p_IsUseVdoOtpt != 0 ) ? 1 : -1 ) );
		MediaPocsThrdSetIsUseAdoVdoInptOtpt( ClntMediaPocsThrdPt->m_MediaPocsThrdPt, IsBlockWait, ( p_IsUseAdoInpt != 0 ) ? 1 : -1, ( p_IsUseAdoOtpt != 0 ) ? 1 : -1, ( p_IsUseVdoInpt != 0 ) ? 1 : -1, ( p_IsUseVdoOtpt != 0 ) ? 1 : -1, NULL );
	}

	//if( ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode == 0 ) TkbkClntRecvOtptFrmReset( &ClntMediaPocsThrdPt->m_TkbkClnt ); //接收输出帧重置。防止在实时半双工（一键通）模式下每次按下PTT时还有点点播放上次按下的声音。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//用户定义的初始化函数。
int __cdecl ClntMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	p_ClntMediaPocsThrdPt->m_UserClntMediaPocsThrdInitFuncPt( p_ClntMediaPocsThrdPt ); //调用用户定义的客户端媒体处理线程初始化函数。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//用户定义的处理函数。
int __cdecl ClntMediaPocsThrdUserPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	TkbkClntCnctPocs( &p_ClntMediaPocsThrdPt->m_TkbkClnt );
	BdctClntCnctPocs( &p_ClntMediaPocsThrdPt->m_BdctClnt );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//用户定义的销毁函数。
void __cdecl ClntMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	TkbkClntCnctInfoDstoy( &p_ClntMediaPocsThrdPt->m_TkbkClnt ); //对讲客户端的连接销毁。
	BdctClntDstoy( &p_ClntMediaPocsThrdPt->m_BdctClnt ); //广播客户端销毁。

	//销毁本端高级Udp协议客户端套接字。
	if( p_ClntMediaPocsThrdPt->m_AudpClntSoktPt != NULL )
	{
		AudpDstoy( p_ClntMediaPocsThrdPt->m_AudpClntSoktPt, NULL ); //关闭并销毁本端高级Udp协议客户端套接字。
		p_ClntMediaPocsThrdPt->m_AudpClntSoktPt = NULL;

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：关闭并销毁本端高级Udp协议客户端套接字成功。" ), , );
		if( MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	p_ClntMediaPocsThrdPt->m_UserClntMediaPocsThrdDstoyFuncPt( p_ClntMediaPocsThrdPt ); //调用用户定义的客户端媒体处理线程销毁函数。
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//用户定义的消息函数。
int __cdecl ClntMediaPocsThrdUserMsg( MediaPocsThrd * MediaPocsThrdPt, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	switch( MsgTyp )
	{
		case ClntMediaPocsThrd::UserMsgTypTkbkClntSetIsTstNtwkDly:
		{
			p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TstNtwkDly.m_IsTstNtwkDly = ( ( ClntMediaPocsThrd::UserMsgTkbkClntSetIsTstNtwkDly * )MsgPt )->m_IsTstNtwkDly; //设置是否测试网络延迟。
			p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TstNtwkDly.m_SendIntvlMsec = ( ( ClntMediaPocsThrd::UserMsgTkbkClntSetIsTstNtwkDly * )MsgPt )->m_SendIntvlMsec; //设置测试网络延迟包的发送间隔。

			if( p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TstNtwkDly.m_IsTstNtwkDly != 0 )
			{
				p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TstNtwkDly.m_LastSendTickMsec = 0; //设置测试网络延迟包最后发送的嘀嗒钟为0，这样可以立即开始发送。
				p_ClntMediaPocsThrdPt->m_TkbkClnt.m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包，这样可以立即开始发送。
			}
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypTkbkClntCnctInit:
		{
			int32_t p_IsTcpOrAudpPrtcl = ( ( ClntMediaPocsThrd::UserMsgTkbkClntCnctInit * )MsgPt )->m_IsTcpOrAudpPrtcl;
			Vstr * p_RmtNodeNameVstrPt = ( ( ClntMediaPocsThrd::UserMsgTkbkClntCnctInit * )MsgPt )->m_RmtNodeNameVstrPt;
			Vstr * p_RmtNodeSrvcVstrPt = ( ( ClntMediaPocsThrd::UserMsgTkbkClntCnctInit * )MsgPt )->m_RmtNodeSrvcVstrPt;

			if( p_ClntMediaPocsThrdPt->m_TkbkClnt.m_CnctIsInit != 0 )
			{
				VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：对讲客户端：已初始化与远端节点%vs[%vs:%vs]的服务端连接，请先销毁再初始化。" ), ( ( p_ClntMediaPocsThrdPt->m_TkbkClnt.m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_ClntMediaPocsThrdPt->m_TkbkClnt.m_RmtNodeNameVstrPt, p_ClntMediaPocsThrdPt->m_TkbkClnt.m_RmtNodeSrvcVstrPt );
				if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto OutTkbkClntCnctInit;
			}

			//Ping一下远程节点名称，这样可以快速获取ARP条目。
			VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), p_RmtNodeNameVstrPt );
			WinExec( ( char * )p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt->m_Pt, SW_HIDE );

			if( TkbkClntCnctInfoInit( &p_ClntMediaPocsThrdPt->m_TkbkClnt, p_IsTcpOrAudpPrtcl, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt ) != 0 ) goto Out;

			OutTkbkClntCnctInit:
			VstrDstoy( p_RmtNodeNameVstrPt );
			VstrDstoy( p_RmtNodeSrvcVstrPt );
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypTkbkClntCnctDstoy:
		{
			TkbkClntCnctInfoDstoy( &p_ClntMediaPocsThrdPt->m_TkbkClnt );
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypTkbkClntLclTkbkMode:
		{
			int32_t p_LclTkbkMode = ( ( ClntMediaPocsThrd::UserMsgTkbkClntLclTkbkMode * )MsgPt )->m_LclTkbkMode; //设置本端对讲模式。
			int32_t p_OldLclTkbkMode = p_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode; //设置旧本端对讲模式。

			if( p_LclTkbkMode != ClntMediaPocsThrd::TkbkModeNoChg ) p_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode = p_LclTkbkMode; //设置本端对讲模式。
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 1 ); //只设置不使用的对讲模式。
			p_ClntMediaPocsThrdPt->m_UserTkbkClntLclTkbkModeFuncPt( p_ClntMediaPocsThrdPt, p_OldLclTkbkMode, p_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode ); //调用用户定义的对讲客户端本端对讲模式函数。
			if( p_ClntMediaPocsThrdPt->m_TkbkClnt.m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct ) TkbkClntCnctSendTkbkModePkt( &p_ClntMediaPocsThrdPt->m_TkbkClnt, p_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode ); //发送对讲模式包。
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 2 ); //只设置要使用的对讲模式。
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypTkbkClntPttBtnDown:
		{
			p_ClntMediaPocsThrdPt->m_TkbkClnt.m_PttBtnIsDown = 1; //设置一键即按即通按钮为按下。
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 0 ); //设置对讲模式。
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypTkbkClntPttBtnUp:
		{
			p_ClntMediaPocsThrdPt->m_TkbkClnt.m_PttBtnIsDown = 0; //设置一键即按即通按钮为弹起。
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 0 ); //设置对讲模式。
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypBdctClntInit:
		{
			BdctClntInit( &p_ClntMediaPocsThrdPt->m_BdctClnt, ( ( ClntMediaPocsThrd::UserMsgBdctClntInit * )MsgPt )->m_CnctNumIsDecr );
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 0 ); //设置对讲模式。
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypBdctClntDstoy:
		{
			BdctClntDstoy( &p_ClntMediaPocsThrdPt->m_BdctClnt );
			ClntMediaPocsThrdSetTkbkMode( p_ClntMediaPocsThrdPt, 1, 0 ); //设置对讲模式。
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypBdctClntCnctInit:
		{
			int32_t p_IsTcpOrAudpPrtcl = ( ( ClntMediaPocsThrd::UserMsgBdctClntCnctInit * )MsgPt )->m_IsTcpOrAudpPrtcl;
			Vstr * p_RmtNodeNameVstrPt = ( ( ClntMediaPocsThrd::UserMsgBdctClntCnctInit * )MsgPt )->m_RmtNodeNameVstrPt;
			Vstr * p_RmtNodeSrvcVstrPt = ( ( ClntMediaPocsThrd::UserMsgBdctClntCnctInit * )MsgPt )->m_RmtNodeSrvcVstrPt;
			BdctClnt::CnctInfo * p_BdctClntCnctInfoTmpPt;
			int p_CmpRslt; //存放比较结果。

			for( int p_CnctInfoLstNum = 0; CQueueGetByNum( p_ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCntnrPt, p_CnctInfoLstNum, NULL, ( void * * )&p_BdctClntCnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstNum++ )
			{
				if( ( p_BdctClntCnctInfoTmpPt->m_IsInit != 0 ) &&
					( p_BdctClntCnctInfoTmpPt->m_IsTcpOrAudpPrtcl == p_IsTcpOrAudpPrtcl ) &&
					( VstrCmp( p_BdctClntCnctInfoTmpPt->m_RmtNodeNameVstrPt, p_RmtNodeNameVstrPt, &p_CmpRslt ), p_CmpRslt == 0 ) &&
					( VstrCmp( p_BdctClntCnctInfoTmpPt->m_RmtNodeSrvcVstrPt, p_RmtNodeSrvcVstrPt, &p_CmpRslt ), p_CmpRslt == 0 ) )
				{
					VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：已存在与远端节点%vs[%vs:%vs]的连接，无需重复连接。" ), ( ( p_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt );
					if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto OutBdctClntCnctInit;
				}
			}
			
			//Ping一下远程节点名称，这样可以快速获取ARP条目。
			VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), p_RmtNodeNameVstrPt );
			WinExec( ( char * )p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt->m_Pt, SW_HIDE );

			if( ( BdctClntCnctInfoInit( &p_ClntMediaPocsThrdPt->m_BdctClnt, p_IsTcpOrAudpPrtcl, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt ) ) == NULL ) goto Out; //如果连接信息初始化失败。

			OutBdctClntCnctInit:
			VstrDstoy( p_RmtNodeNameVstrPt );
			VstrDstoy( p_RmtNodeSrvcVstrPt );
			break;
		}
		case ClntMediaPocsThrd::UserMsgTypBdctClntCnctDstoy:
		{
			int p_CnctNum = ( ( ClntMediaPocsThrd::UserMsgBdctClntCnctDstoy * )MsgPt )->m_CnctNum;
			BdctClnt::CnctInfo * p_BdctClntCnctInfoTmpPt = NULL;

			if( ( p_CnctNum > p_ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCurMaxNum ) || ( p_CnctNum < 0 ) )
			{
				VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：没有序号为%d的连接，无法删除。" ), p_CnctNum );
				if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				goto OutBdctClntCnctDstoy;
			}

			for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( p_ClntMediaPocsThrdPt->m_BdctClnt.m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_BdctClntCnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
			{
				if( ( p_BdctClntCnctInfoTmpPt->m_IsInit != 0 ) && ( p_BdctClntCnctInfoTmpPt->m_Num == p_CnctNum ) )
				{
					p_BdctClntCnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
					break;
				}
			}

			VstrFmtCpy( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "客户端媒体处理线程：广播客户端：连接%uzd：请求销毁远端节点%vs[%vs:%vs]的连接。" ), p_BdctClntCnctInfoTmpPt->m_Idx, ( ( p_BdctClntCnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_BdctClntCnctInfoTmpPt->m_RmtNodeNameVstrPt, p_BdctClntCnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			if( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			p_ClntMediaPocsThrdPt->m_UserShowLogFuncPt( p_ClntMediaPocsThrdPt, p_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

			p_Rslt = 0; //设置本函数执行成功。

			OutBdctClntCnctDstoy:
			if( p_Rslt != 0 ) //如果本函数执行失败。
			{

			}
			break;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//用户定义的读取音视频输入帧函数。
void __cdecl ClntMediaPocsThrdUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt,
													 int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
													 uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
													 uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
													 uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
													 uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt )
{
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。
	size_t p_ElmTotal; //存放元素总数。

	if( p_ClntMediaPocsThrdPt->m_TkbkClnt.m_CurCnctSts == ClntMediaPocsThrd::CnctStsCnct )
	{
		TkbkClntUserReadAdoVdoInptFrm( &p_ClntMediaPocsThrdPt->m_TkbkClnt, AdoInptPcmSrcFrmPt,
									   AdoInptPcmRsltFrmPt, AdoInptPcmFrmLenUnit, AdoInptPcmRsltFrmVoiceActSts,
									   AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt, AdoInptEncdRsltFrmIsNeedTrans,
									   VdoInptBgraSrcFrmPt, VdoInptBgraSrcFrmWidth, VdoInptBgraSrcFrmHeight, VdoInptBgraSrcFrmLenByt,
									   VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth, VdoInptYu12RsltFrmHeight, VdoInptYu12RsltFrmLenByt,
									   VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt );
	}

	if( ( AdoInptPcmSrcFrmPt != NULL ) && ( p_ClntMediaPocsThrdPt->m_BdctClnt.m_IsInit != 0 ) )
	{
		BdctClntUserReadAdoVdoInptFrm( &p_ClntMediaPocsThrdPt->m_BdctClnt,
									   AdoInptPcmSrcFrmPt, AdoInptPcmRsltFrmPt, AdoInptPcmFrmLenUnit, AdoInptPcmRsltFrmVoiceActSts,
									   AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt, AdoInptEncdRsltFrmIsNeedTrans,
									   VdoInptBgraSrcFrmPt, VdoInptBgraSrcFrmWidth, VdoInptBgraSrcFrmHeight, VdoInptBgraSrcFrmLenByt,
									   VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth, VdoInptYu12RsltFrmHeight, VdoInptYu12RsltFrmLenByt,
									   VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt );
	}
}

//用户定义的写入音频输出帧函数。
void __cdecl ClntMediaPocsThrdUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
												   int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
												   uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	TkbkClntUserWriteAdoOtptFrm( &p_ClntMediaPocsThrdPt->m_TkbkClnt, AdoOtptStrmIdx,
								 AdoOtptPcmSrcFrmPt, AdoOtptPcmFrmLenUnit,
								 AdoOtptEncdSrcFrmPt, AdoOtptEncdSrcFrmSzByt, AdoOtptEncdSrcFrmLenBytPt );
}

//用户定义的获取音频输出帧函数。
void __cdecl ClntMediaPocsThrdUserGetAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx,
												 int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
												 uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{

}

//用户定义的写入视频输出帧函数。
void __cdecl ClntMediaPocsThrdUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
												   uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
												   uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	ClntMediaPocsThrd * p_ClntMediaPocsThrdPt = ( ClntMediaPocsThrd * )MediaPocsThrdPt->m_UserDataPt; //存放客户端媒体处理线程的指针。

	TkbkClntUserWriteVdoOtptFrm( &p_ClntMediaPocsThrdPt->m_TkbkClnt, VdoOtptStrmIdx,
								 VdoOtptYu12SrcFrmPt, VdoOtptYu12SrcFrmWidthPt, VdoOtptYu12SrcFrmHeightPt,
								 VdoOtptEncdSrcFrmPt, VdoOtptEncdSrcFrmSzByt, VdoOtptEncdSrcFrmLenBytPt );
}

//用户定义的获取视频输出帧函数。
void __cdecl ClntMediaPocsThrdUserGetVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint32_t VdoOtptStrmIdx,
												 uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
												 uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{

}

//回调ClntMediaPocsThrdCls类的用户定义的显示日志函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserShowLog( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * InfoVstrPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserShowLog( InfoVstrPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的显示Toast函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserShowToast( ClntMediaPocsThrd * ClntMediaPocsThrdPt, Vstr * InfoVstrPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserShowToast( InfoVstrPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的客户端媒体处理线程初始化函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserClntMediaPocsThrdInit( ClntMediaPocsThrd * ClntMediaPocsThrdPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserClntMediaPocsThrdInit();
}

//回调ClntMediaPocsThrdCls类的用户定义的客户端媒体处理线程销毁函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserClntMediaPocsThrdDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserClntMediaPocsThrdDstoy();
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端连接初始化函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntCnctInit( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntCnctInit( IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端连接销毁函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntCnctDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntCnctDstoy();
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端连接状态函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntCnctSts( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t CurCnctSts )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntCnctSts( CurCnctSts );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端我的对讲索引函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntMyTkbkIdx( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t MyTkbkIdx )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntMyTkbkIdx( MyTkbkIdx );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端本端对讲模式函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntLclTkbkMode( ClntMediaPocsThrd * ClntMediaPocsThrdPt, int32_t OldLclTkbkMode, int32_t NewLclTkbkMode )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntLclTkbkMode( OldLclTkbkMode, NewLclTkbkMode );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端对讲信息初始化函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntTkbkInfoInit( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntTkbkInfoInit( TkbkInfoPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端对讲信息销毁函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntTkbkInfoDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntTkbkInfoDstoy( TkbkInfoPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端对讲信息远端对讲模式函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntTkbkInfoRmtTkbkMode( ClntMediaPocsThrd * ClntMediaPocsThrdPt, TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntTkbkInfoRmtTkbkMode( TkbkInfoPt, OldRmtTkbkMode, NewRmtTkbkMode );
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端测试网络延迟函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserTkbkClntTstNtwkDly( ClntMediaPocsThrd * ClntMediaPocsThrdPt, uint64_t NtwkDlyMsec )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserTkbkClntTstNtwkDly( NtwkDlyMsec );
}

//回调ClntMediaPocsThrdCls类的用户定义的广播客户端初始化函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserBdctClntInit( ClntMediaPocsThrd * ClntMediaPocsThrdPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserBdctClntInit();
}

//回调ClntMediaPocsThrdCls类的用户定义的广播客户端销毁函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserBdctClntDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserBdctClntDstoy();
}

//回调ClntMediaPocsThrdCls类的用户定义的对讲客户端连接初始化函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserBdctClntCnctInit( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserBdctClntCnctInit( CnctInfoPt, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的广播客户端连接销毁函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserBdctClntCnctDstoy( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserBdctClntCnctDstoy( CnctInfoPt );
}

//回调ClntMediaPocsThrdCls类的用户定义的广播客户端连接状态函数。
extern "C" void __cdecl ClntMediaPocsThrdClsUserBdctClntCnctSts( ClntMediaPocsThrd * ClntMediaPocsThrdPt, BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts )
{
	( ( ClntMediaPocsThrdCls * )ClntMediaPocsThrdPt->m_UserDataPt )->UserBdctClntCnctSts( CnctInfoPt, CurCnctSts );
}

int ClntMediaPocsThrdCls::Init( const void * LicnCodePt, VstrCls * ErrInfoVstrPt )
{
	int p_Rslt = ClntMediaPocsThrdInit( &m_ClntMediaPocsThrdPt, LicnCodePt, this,
										ClntMediaPocsThrdClsUserShowLog, ClntMediaPocsThrdClsUserShowToast,
										ClntMediaPocsThrdClsUserClntMediaPocsThrdInit, ClntMediaPocsThrdClsUserClntMediaPocsThrdDstoy,
										ClntMediaPocsThrdClsUserTkbkClntCnctInit, ClntMediaPocsThrdClsUserTkbkClntCnctDstoy, ClntMediaPocsThrdClsUserTkbkClntCnctSts, ClntMediaPocsThrdClsUserTkbkClntMyTkbkIdx, ClntMediaPocsThrdClsUserTkbkClntLclTkbkMode,
										ClntMediaPocsThrdClsUserTkbkClntTkbkInfoInit, ClntMediaPocsThrdClsUserTkbkClntTkbkInfoDstoy, ClntMediaPocsThrdClsUserTkbkClntTkbkInfoRmtTkbkMode, ClntMediaPocsThrdClsUserTkbkClntTstNtwkDly,
										ClntMediaPocsThrdClsUserBdctClntInit, ClntMediaPocsThrdClsUserBdctClntDstoy, ClntMediaPocsThrdClsUserBdctClntCnctInit, ClntMediaPocsThrdClsUserBdctClntCnctDstoy, ClntMediaPocsThrdClsUserBdctClntCnctSts,
										( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL );
	if( p_Rslt == 0 )
	{
		m_MediaPocsThrdClsPt = ( MediaPocsThrdCls * )( &m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt - 1 );
	}
	return p_Rslt;
}
