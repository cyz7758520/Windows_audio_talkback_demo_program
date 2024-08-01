#include "SrvrThrd.h"

extern "C" void SrvrThrdPrvntSysSleepInitOrDstoy( SrvrThrd * SrvrThrdPt, int32_t IsInitPrvntSysSleep );
extern "C" int SrvrThrdSrvrInit( SrvrThrd * SrvrThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * LclNodeNameVstrPt, Vstr * LclNodeSrvcVstrPt, int32_t MaxCnctNum, int32_t IsAutoRqirExit );
extern "C" void SrvrThrdSrvrDstoy( SrvrThrd * SrvrThrdPt );
extern "C" SrvrThrd::CnctInfo * SrvrThrdCnctInfoInit( SrvrThrd * SrvrThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int32_t CurCnctSts );
extern "C" void SrvrThrdCnctInfoDstoy( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt );
extern "C" void SrvrThrdIsAutoRqirExit( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt );
extern "C" void SrvrThrdCnctPocs( SrvrThrd * SrvrThrdPt );
extern "C" int SrvrThrdThrdMsgPocs( MsgQueue * MsgQueuePt, SrvrThrd * SrvrThrdPt, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt );
extern "C" DWORD WINAPI SrvrThrdRun( SrvrThrd * SrvrThrdPt );

//服务端线程初始化。
int SrvrThrdInit( SrvrThrd * * SrvrThrdPtPt, void * UserDataPt,
				  SrvrThrd::SrvrThrdUserShowLogFuncPt UserShowLogFuncPt, SrvrThrd::SrvrThrdUserShowToastFuncPt UserShowToastFuncPt, SrvrThrd::SrvrThrdUserMsgFuncPt UserMsgFuncPt,
				  SrvrThrd::SrvrThrdUserSrvrThrdInitFuncPt UserSrvrThrdInitFuncPt, SrvrThrd::SrvrThrdUserSrvrThrdDstoyFuncPt UserSrvrThrdDstoyFuncPt,
				  SrvrThrd::SrvrThrdUserSrvrInitFuncPt UserSrvrInitFuncPt, SrvrThrd::SrvrThrdUserSrvrDstoyFuncPt UserSrvrDstoyFuncPt,
				  SrvrThrd::SrvrThrdUserCnctInitFuncPt UserCnctInitFuncPt, SrvrThrd::SrvrThrdUserCnctDstoyFuncPt UserCnctDstoyFuncPt, SrvrThrd::SrvrThrdUserCnctStsFuncPt UserCnctStsFuncPt, SrvrThrd::SrvrThrdUserCnctRmtTkbkModeFuncPt UserCnctRmtTkbkModeFuncPt, SrvrThrd::SrvrThrdUserCnctTstNtwkDlyFuncPt UserCnctTstNtwkDlyFuncPt,
				  Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	SrvrThrd * p_SrvrThrdPt = NULL; //存放服务端线程的指针。
	
	//判断各个变量是否正确。
	if( SrvrThrdPtPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针的指针不正确。" ), , );
		goto Out;
	}
	if( UserShowLogFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的显示日志函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserShowToastFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的显示Toast函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserMsgFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的消息函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserSrvrThrdInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的服务端线程初始化函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserSrvrThrdDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的服务端线程销毁函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserSrvrInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的服务端初始化函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserSrvrDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的服务端销毁函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserCnctInitFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的连接初始化函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserCnctDstoyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的连接销毁函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserCnctStsFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的连接状态函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserCnctRmtTkbkModeFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的连接远端对讲模式函数的指针不正确。" ), , );
		goto Out;
	}
	if( UserCnctTstNtwkDlyFuncPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "用户定义的连接测试网络延迟函数的指针不正确。" ), , );
		goto Out;
	}

	//创建并清空服务端线程内存块。
	p_SrvrThrdPt = ( SrvrThrd * )calloc( 1, sizeof( SrvrThrd ) );
	if( p_SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "创建并清空服务端线程内存块失败。" ), , );
		goto Out;
	}
	
	//设置最小计时器分辨率为1毫秒，可以防止Sleep()函数休眠时间过长。放在这里是防止下面的代码初始化失败导致跳过设置最小计时器分辨率，从而在调用销毁函数时多取消了一次最小计时器分辨率。
	if( timeBeginPeriod( 1 ) != TIMERR_NOERROR )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置最小计时器分辨率失败。" ), , );
		goto Out;
	}
	
	if( MsgQueueInit( &p_SrvrThrdPt->m_ThrdMsgQueuePt, NULL, p_SrvrThrdPt, ( MsgQueue::MsgQueueUserMsgPocsFuncPt )SrvrThrdThrdMsgPocs, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化线程消息队列失败。原因：" ) );
		goto Out;
	}

	if( CQueueInit( &p_SrvrThrdPt->m_CnctInfoCntnrPt, sizeof( SrvrThrd::CnctInfo ), 10, BufAutoAdjMethFreeNumber, 10, SIZE_MAX, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "初始化连接信息容器失败。原因" ) );
		goto Out;
	}
	p_SrvrThrdPt->m_CnctInfoCurMaxNum = -1; //设置连接信息的当前最大序号。

	//初始化服务端线程。
	if( VstrInit( &p_SrvrThrdPt->m_Thrd.m_LclNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( p_SrvrThrdPt->m_ErrInfoVstrPt );
		p_SrvrThrdPt->m_UserShowLogFuncPt( p_SrvrThrdPt, p_SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_SrvrThrdPt->m_Thrd.m_LclNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( p_SrvrThrdPt->m_ErrInfoVstrPt );
		p_SrvrThrdPt->m_UserShowLogFuncPt( p_SrvrThrdPt, p_SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( p_SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ), , );
		if( p_SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( p_SrvrThrdPt->m_ErrInfoVstrPt );
		p_SrvrThrdPt->m_UserShowLogFuncPt( p_SrvrThrdPt, p_SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( VstrInit( &p_SrvrThrdPt->m_Thrd.m_RmtNodePortPt, , 6,  ) != 0 )
	{
		VstrCpy( p_SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ), , );
		if( p_SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( p_SrvrThrdPt->m_ErrInfoVstrPt );
		p_SrvrThrdPt->m_UserShowLogFuncPt( p_SrvrThrdPt, p_SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( p_SrvrThrdPt->m_Thrd.m_TmpBytePt == NULL )
	{
		p_SrvrThrdPt->m_Thrd.m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( p_SrvrThrdPt->m_Thrd.m_TmpBytePt == NULL )
		{
			VstrCpy( p_SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ), , );
			if( p_SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( p_SrvrThrdPt->m_ErrInfoVstrPt );
			p_SrvrThrdPt->m_UserShowLogFuncPt( p_SrvrThrdPt, p_SrvrThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_SrvrThrdPt->m_Thrd.m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据大小。
	}
	
	//初始化错误信息动态字符串。
	if( VstrInit( &p_SrvrThrdPt->m_ErrInfoVstrPt, , , ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "初始化错误信息动态字符串失败。" ), , );
		goto Out;
	}

	p_SrvrThrdPt->m_UserDataPt = UserDataPt; //设置用户数据的指针。
	p_SrvrThrdPt->m_UserShowLogFuncPt = UserShowLogFuncPt; //设置用户定义的显示日志函数的指针。
	p_SrvrThrdPt->m_UserShowToastFuncPt = UserShowToastFuncPt; //设置用户定义的显示Toast函数的指针。
	p_SrvrThrdPt->m_UserMsgFuncPt = UserMsgFuncPt; //设置用户定义的消息函数的指针。
	p_SrvrThrdPt->m_UserSrvrThrdInitFuncPt = UserSrvrThrdInitFuncPt; //设置用户定义的服务端线程初始化函数的指针。
	p_SrvrThrdPt->m_UserSrvrThrdDstoyFuncPt = UserSrvrThrdDstoyFuncPt; //设置用户定义的服务端线程销毁函数的指针。
	p_SrvrThrdPt->m_UserSrvrInitFuncPt = UserSrvrInitFuncPt; //设置用户定义的服务端初始化函数的指针。
	p_SrvrThrdPt->m_UserSrvrDstoyFuncPt = UserSrvrDstoyFuncPt; //设置用户定义的服务端销毁函数的指针。
	p_SrvrThrdPt->m_UserCnctInitFuncPt = UserCnctInitFuncPt; //设置用户定义的连接初始化函数的指针。
	p_SrvrThrdPt->m_UserCnctDstoyFuncPt = UserCnctDstoyFuncPt; //设置用户定义的连接销毁函数的指针。
	p_SrvrThrdPt->m_UserCnctStsFuncPt = UserCnctStsFuncPt; //设置用户定义的连接状态函数的指针。
	p_SrvrThrdPt->m_UserCnctRmtTkbkModeFuncPt = UserCnctRmtTkbkModeFuncPt; //设置用户定义的连接远端对讲模式函数的指针。
	p_SrvrThrdPt->m_UserCnctTstNtwkDlyFuncPt = UserCnctTstNtwkDlyFuncPt; //设置用户定义的连接测试网络延迟函数的指针。

	*SrvrThrdPtPt = p_SrvrThrdPt; //设置服务端线程的指针。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		SrvrThrdDstoy( p_SrvrThrdPt, NULL );
	}
	return p_Rslt;
}

//服务端线程销毁。
int SrvrThrdDstoy( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	SrvrThrdSendRqirExitMsg( SrvrThrdPt, 1, NULL ); //请求服务端线程退出，并阻塞等待。
	
	//销毁错误信息动态字符串。
	if( SrvrThrdPt->m_ErrInfoVstrPt != NULL )
	{
		VstrDstoy( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_ErrInfoVstrPt = NULL;
	}
	
	//销毁服务端线程。
	if( SrvrThrdPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		ThrdWaitDstoy( SrvrThrdPt->m_Thrd.m_ThrdInfoPt, NULL );
		SrvrThrdPt->m_Thrd.m_ThrdInfoPt = NULL;
	}
	VstrDstoy( SrvrThrdPt->m_Thrd.m_LclNodeAddrPt ); //销毁本端节点地址动态字符串。
	VstrDstoy( SrvrThrdPt->m_Thrd.m_LclNodePortPt ); //销毁本端节点端口动态字符串。
	VstrDstoy( SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt ); //销毁远端节点地址动态字符串。
	VstrDstoy( SrvrThrdPt->m_Thrd.m_RmtNodePortPt ); //销毁远端节点端口动态字符串。
	if( SrvrThrdPt->m_Thrd.m_TmpBytePt != NULL )
	{
		free( SrvrThrdPt->m_Thrd.m_TmpBytePt );
		SrvrThrdPt->m_Thrd.m_TmpBytePt = NULL;
	}
	
	//销毁连接消息容器。
	if( SrvrThrdPt->m_CnctInfoCntnrPt != NULL )
	{
		CQueueDstoy( SrvrThrdPt->m_CnctInfoCntnrPt, NULL );
	}

	//销毁线程消息容器。
	if( SrvrThrdPt->m_ThrdMsgQueuePt != NULL )
	{
		MsgQueueDstoy( SrvrThrdPt->m_ThrdMsgQueuePt, NULL );
	}

	//取消最小计时器分辨率。
	timeEndPeriod( 1 );

	//销毁服务端线程内存块。
	free( SrvrThrdPt );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

typedef enum ThrdMsgTyp
{
	ThrdMsgTypSetIsUsePrvntSysSleep,
	ThrdMsgTypSetIsTstNtwkDly,

	ThrdMsgTypSrvrInit,
	ThrdMsgTypSrvrDstoy,

	ThrdMsgTypCnctDstoy,
	
	ThrdMsgTypRqirExit,

	ThrdMsgTypUserMsgMinVal = 100, //用户消息的最小值。
} ThrdMsgTyp;
typedef struct
{
	int32_t m_IsUsePrvntSysSleep;
} ThrdMsgSetIsUsePrvntSysSleep;
typedef struct
{
	int32_t m_IsTcpOrAudpPrtcl;
	Vstr * m_LclNodeNameVstrPt;
	Vstr * m_LclNodeSrvcVstrPt;
	int32_t m_MaxCnctNum;
	int32_t m_IsAutoRqirExit;
} ThrdMsgSrvrInit;
typedef struct
{
} ThrdMsgSrvrDstoy;
typedef struct
{
	int32_t m_CnctNum;
} ThrdMsgCnctDstoy;
typedef struct
{
	int32_t m_IsTstNtwkDly;
	uint64_t m_SendIntvlMsec;
} ThrdMsgSetIsTstNtwkDly;
typedef struct
{
} ThrdMsgRqirExit;
typedef struct
{
	uint8_t m_MsgArgArr[ sizeof( ThrdMsgSrvrInit ) ];
} ThrdMsg;

//设置是否打印Log日志、显示Toast。
int SrvrThrdSetIsPrintLogShowToast( SrvrThrd * SrvrThrdPt, int32_t IsPrintLog, int32_t IsShowToast, HWND ShowToastWndHdl, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	SrvrThrdPt->m_IsPrintLog = IsPrintLog;
	SrvrThrdPt->m_IsShowToast = IsShowToast;
	SrvrThrdPt->m_ShowToastWndHdl = ShowToastWndHdl;

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//发送设置是否使用阻止系统睡眠消息。
int SrvrThrdSendSetIsUsePrvntSysSleepMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsUsePrvntSysSleep, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgSetIsUsePrvntSysSleep p_ThrdMsgSetIsUsePrvntSysSleep;

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgSetIsUsePrvntSysSleep.m_IsUsePrvntSysSleep = IsUsePrvntSysSleep;
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypSetIsUsePrvntSysSleep, &p_ThrdMsgSetIsUsePrvntSysSleep, sizeof( p_ThrdMsgSetIsUsePrvntSysSleep ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//发送设置是否测试网络延迟消息。
int SrvrThrdSendSetIsTstNtwkDlyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsTstNtwkDly, uint64_t SendIntvlMsec, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgSetIsTstNtwkDly p_ThrdMsgSetIsTstNtwkDly;
	
	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgSetIsTstNtwkDly.m_IsTstNtwkDly = IsTstNtwkDly;
	p_ThrdMsgSetIsTstNtwkDly.m_SendIntvlMsec = SendIntvlMsec;
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypSetIsTstNtwkDly, &p_ThrdMsgSetIsTstNtwkDly, sizeof( p_ThrdMsgSetIsTstNtwkDly ), ErrInfoVstrPt ) != 0 )
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

//阻止系统睡眠初始化或销毁。
void SrvrThrdPrvntSysSleepInitOrDstoy( SrvrThrd * SrvrThrdPt, int32_t IsInitPrvntSysSleep )
{
	if( IsInitPrvntSysSleep != 0 ) //如果要初始化阻止系统睡眠。
	{
		SetThreadExecutionState( ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED | ES_AWAYMODE_REQUIRED );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "服务端线程：初始化阻止系统睡眠成功。" ) );
	}
	else //如果要销毁阻止系统睡眠。
	{
		SetThreadExecutionState( ES_CONTINUOUS );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "服务端线程：销毁阻止系统睡眠成功。" ) );
	}
}

//服务端线程启动。
int SrvrThrdStart( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}
	if( SrvrThrdPt->m_Thrd.m_ThrdInfoPt != NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程已经启动。" ), , );
		goto Out;
	}

	if( ThrdInit( &SrvrThrdPt->m_Thrd.m_ThrdInfoPt, ( LPTHREAD_START_ROUTINE )SrvrThrdRun, SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 )
	{
		if( SrvrThrdPt->m_IsPrintLog != NULL ) LOGFE( Cu8vstr( "服务端线程：初始化服务端线程失败。原因：%vs" ), SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}
	if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：初始化服务端线程成功。" ) );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//发送请求退出消息。
int SrvrThrdSendRqirExitMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgRqirExit p_ThrdMsgRqirExit;

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypRqirExit, &p_ThrdMsgRqirExit, sizeof( p_ThrdMsgRqirExit ), ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//发送用户消息。
int SrvrThrdSendUserMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}
	
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypUserMsgMinVal + MsgTyp, MsgPt, MsgLenByt, ErrInfoVstrPt ) != 0 )
	{
		VstrIns( ErrInfoVstrPt, 0, Cu8vstr( "发送线程消息失败。原因：" ) );
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return p_Rslt;
}

//发送服务端初始化消息。
int SrvrThrdSendSrvrInitMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t IsTcpOrAudpPrtcl, Vstr * LclNodeNameVstrPt, Vstr * LclNodeSrvcVstrPt, int32_t MaxCnctNum, int32_t IsAutoRqirExit, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgSrvrInit p_ThrdMsgSrvrInit;
	
	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgSrvrInit.m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	if( VstrInit( &p_ThrdMsgSrvrInit.m_LclNodeNameVstrPt, Utf8, , LclNodeNameVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置本端套接字绑定的名称动态字符串失败。" ), , );
		goto Out;
	}
	if( VstrInit( &p_ThrdMsgSrvrInit.m_LclNodeSrvcVstrPt, Utf8, , LclNodeSrvcVstrPt ) != 0 )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "设置本端套接字绑定的服务动态字符串失败。" ), , );
		goto Out;
	}
	p_ThrdMsgSrvrInit.m_MaxCnctNum = MaxCnctNum;
	p_ThrdMsgSrvrInit.m_IsAutoRqirExit = IsAutoRqirExit;
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypSrvrInit, &p_ThrdMsgSrvrInit, sizeof( p_ThrdMsgSrvrInit ), ErrInfoVstrPt ) != 0 )
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

//发送服务端销毁消息。
int SrvrThrdSendSrvrDstoyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgSrvrDstoy p_ThrdMsgSrvrDstoy;
	
	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypSrvrDstoy, &p_ThrdMsgSrvrDstoy, sizeof( p_ThrdMsgSrvrDstoy ), ErrInfoVstrPt ) != 0 )
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

//发送连接销毁消息。
int SrvrThrdSendCnctDstoyMsg( SrvrThrd * SrvrThrdPt, int IsBlockWait, int32_t CnctNum, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	ThrdMsgCnctDstoy p_ThrdMsgCnctDstoy;
	
	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	p_ThrdMsgCnctDstoy.m_CnctNum = CnctNum;
	if( MsgQueueSendMsg( SrvrThrdPt->m_ThrdMsgQueuePt, IsBlockWait, 1, ThrdMsgTypCnctDstoy, &p_ThrdMsgCnctDstoy, sizeof( p_ThrdMsgCnctDstoy ), ErrInfoVstrPt ) != 0 )
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

//服务端初始化。
int SrvrThrdSrvrInit( SrvrThrd * SrvrThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * LclNodeNameVstrPt, Vstr * LclNodeSrvcVstrPt, int32_t MaxCnctNum, int32_t IsAutoRqirExit )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。

	if( SrvrThrdPt->m_SrvrIsInit != 0 ) //如果服务端已初始化。
	{
		VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：服务端已初始化，无法再次初始化。" ), ,  );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	SrvrThrdPt->m_UserSrvrInitFuncPt( SrvrThrdPt ); //调用用户定义的服务端初始化函数。

	SrvrThrdPt->m_SrvrIsInit = 1; //设置服务端已初始化。

	{ //设置远端节点的地址族。
		ADDRINFOW * p_AddrInfoListPt;
		if( GetAddrInfo( ( PCWSTR )LclNodeNameVstrPt->m_Pt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
		{
			p_RmtNodeAddrFamly = ( p_AddrInfoListPt->ai_family == AF_INET ) ? 4 : ( p_AddrInfoListPt->ai_family == AF_INET6 ) ? 6 : 0;
			FreeAddrInfo( p_AddrInfoListPt );
		}
		else p_RmtNodeAddrFamly = 0;
	}

	if( IsTcpOrAudpPrtcl == 0 ) //如果要使用Tcp协议。
	{
		if( TcpSrvrInit( &SrvrThrdPt->m_TcpSrvrSoktPt, p_RmtNodeAddrFamly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议服务端套接字成功。
		{
			if( TcpSrvrGetLclAddr( SrvrThrdPt->m_TcpSrvrSoktPt, NULL, SrvrThrdPt->m_Thrd.m_LclNodeAddrPt, SrvrThrdPt->m_Thrd.m_LclNodePortPt, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
			{
				VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：初始化本端TCP协议服务端套接字[%vs:%vs]成功。" ), SrvrThrdPt->m_Thrd.m_LclNodeAddrPt, SrvrThrdPt->m_Thrd.m_LclNodePortPt );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		}
		else
		{
			VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：初始化本端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), LclNodeNameVstrPt, LclNodeSrvcVstrPt );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
	}
	else //如果要使用高级Udp协议。
	{
		if( AudpInit( &SrvrThrdPt->m_AudpSrvrSoktPt, p_RmtNodeAddrFamly, LclNodeNameVstrPt, LclNodeSrvcVstrPt, ( short )1, ( short )5000, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级Udp协议服务端套接字成功。
		{
			if( AudpSetSendBufSz( SrvrThrdPt->m_AudpSrvrSoktPt, 1024 * 1024, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
			{
				VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			if( AudpSetRecvBufSz( SrvrThrdPt->m_AudpSrvrSoktPt, 1024 * 1024 * 3, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
			{
				VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			if( AudpGetLclAddr( SrvrThrdPt->m_AudpSrvrSoktPt, NULL, SrvrThrdPt->m_Thrd.m_LclNodeAddrPt, SrvrThrdPt->m_Thrd.m_LclNodePortPt, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级Udp协议套接字绑定的本地节点地址和端口失败。
			{
				VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：获取本端高级Udp协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto Out;
			}

			VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：初始化本端高级Udp协议服务端套接字[%vs:%vs]成功。" ), LclNodeNameVstrPt, LclNodeSrvcVstrPt );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		}
		else //如果初始化本端高级Udp协议服务端套接字失败。
		{
			VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：初始化本端高级Udp协议服务端套接字[%vs:%vs]失败。原因：" ), LclNodeNameVstrPt, LclNodeSrvcVstrPt );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
	}

	SrvrThrdPt->m_MaxCnctNum = MaxCnctNum; //设置最大连接数。
	SrvrThrdPt->m_IsAutoRqirExit = IsAutoRqirExit; //设置是否自动请求退出。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		SrvrThrdSrvrDstoy( SrvrThrdPt );
	}
	return p_Rslt;
}

//服务端销毁。
void SrvrThrdSrvrDstoy( SrvrThrd * SrvrThrdPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	SrvrThrd::CnctInfo * p_CnctInfoTmpPt;

	if( SrvrThrdPt->m_SrvrIsInit == 0 ) //如果服务端未初始化。
	{
		VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：服务端未初始化，无法继续销毁。" ), ,  );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	//连接信息全部销毁。
	for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
	{
		if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) )
		{
			SrvrThrdCnctInfoDstoy( SrvrThrdPt, p_CnctInfoTmpPt );
		}
	}

	//销毁本端Tcp协议服务端套接字。
	if( SrvrThrdPt->m_TcpSrvrSoktPt != NULL )
	{
		TcpSrvrDstoy( SrvrThrdPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁本端Tcp协议服务端套接字。
		SrvrThrdPt->m_TcpSrvrSoktPt = NULL;

		VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：关闭并销毁本端Tcp协议服务端套接字成功。" ), , );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
	}

	//销毁本端高级Udp协议服务端套接字。
	if( SrvrThrdPt->m_AudpSrvrSoktPt != NULL )
	{
		AudpDstoy( SrvrThrdPt->m_AudpSrvrSoktPt, NULL ); //关闭并销毁本端高级Udp协议服务端套接字。
		SrvrThrdPt->m_AudpSrvrSoktPt = NULL;

		VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：关闭并销毁本端高级Udp协议服务端套接字成功。" ), , );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
	}

	SrvrThrdPt->m_SrvrIsInit = 0; //设置服务端未初始化。

	SrvrThrdPt->m_UserSrvrDstoyFuncPt( SrvrThrdPt ); //调用用户定义的服务端销毁函数。

	SrvrThrdIsAutoRqirExit( SrvrThrdPt, NULL ); //判断是否自动请求退出。
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接信息初始化。
SrvrThrd::CnctInfo * SrvrThrdCnctInfoInit( SrvrThrd * SrvrThrdPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt, TcpClntSokt * TcpClntSoktPt, size_t AudpClntCnctIdx, int32_t CurCnctSts )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	SrvrThrd::CnctInfo * p_CnctInfoTmpPt = NULL;
	SrvrThrd::CnctInfo * p_CnctInfoTmp2Pt;
	size_t p_ElmTotal; //存放元素总数。

	CQueueGetTotal( SrvrThrdPt->m_CnctInfoCntnrPt, &p_ElmTotal, 0, NULL );
	if( p_ElmTotal >= SrvrThrdPt->m_MaxCnctNum ) //如果已达到最大连接数。
	{
		VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：已达到最大连接数，无法进行连接信息初始化。" ), , );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		if( SrvrThrdPt->m_IsShowToast != 0 ) SrvrThrdPt->m_UserShowToastFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		goto Out;
	}

	{
		//查找是否有未初始化的连接信息。
		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( p_CnctInfoTmpPt->m_IsInit == 0 ) goto CnctInfoFindOut; //如果找到了未初始化的连接信息。
		}

		//如果没找到未初始化的连接信息。
		if( CQueuePutTail( SrvrThrdPt->m_CnctInfoCntnrPt, NULL, ( void * * )&p_CnctInfoTmpPt, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 )
		{
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：添加到连接信息容器失败。原因：%vs" ), SrvrThrdPt->m_ErrInfoVstrPt );
			goto Out;
		}
		p_CnctInfoTmpPt->m_Idx = p_ElmTotal;
		p_CnctInfoTmpPt->m_RmtNodeNameVstrPt = NULL;
		p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt = NULL;
	}
	CnctInfoFindOut:;
	p_CnctInfoTmpPt->m_IsInit = 1; //设置连接信息已初始化。
	p_CnctInfoTmpPt->m_TstNtwkDly.m_LastSendTickMsec = 0; //设置测试网络延迟包最后发送的嘀嗒钟为0，这样可以立即开始发送。
	p_CnctInfoTmpPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包，这样可以立即开始发送。
	SrvrThrdPt->m_CnctInfoCurMaxNum++; //递增连接信息的当前最大序号。
	p_CnctInfoTmpPt->m_Num = SrvrThrdPt->m_CnctInfoCurMaxNum; //设置序号。

	p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl; //设置协议为Tcp协议或高级Udp协议。
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt ) != 0 ) //设置远端套接字绑定的远端节点名称动态字符串的指针。
	{
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "服务端线程：初始化远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt ) != 0 ) //设置远端套接字绑定的远端节点服务动态字符串的指针。
	{
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( Cu8vstr( "服务端线程：初始化远端套接字绑定的远端节点名称动态字符串失败。" ) );
		goto Out;
	}
	p_CnctInfoTmpPt->m_TcpClntSoktPt = TcpClntSoktPt; //设置本端Tcp协议客户端套接字的指针。
	p_CnctInfoTmpPt->m_AudpClntCnctIdx = AudpClntCnctIdx; //设置本端高级Udp协议客户端连接索引。
	p_CnctInfoTmpPt->m_IsRqstDstoy = 0; //设置是否请求销毁。

	p_CnctInfoTmpPt->m_CurCnctSts = CurCnctSts; //设置当前连接状态。
	p_CnctInfoTmpPt->m_RmtTkbkMode = SrvrThrd::TkbkModeNone; //设置远端对讲模式。

	p_CnctInfoTmpPt->m_IsRecvExitPkt = 0; //设置未接收退出包。

	SrvrThrdPt->m_UserCnctInitFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl, p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt ); //调用用户定义的连接添加函数。

	//全部发送对讲索引包。
	{
		SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypTkbkIdx; //设置对讲索引包。
		SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = p_CnctInfoTmpPt->m_Idx; //设置对讲索引。

		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) ) //如果连接信息已初始化，且当前连接状态为已连接。
			{
				if( ( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmp2Pt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmp2Pt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：发送对讲索引包成功。对讲索引：%uzd。总长度：2。" ), p_CnctInfoTmp2Pt->m_Idx, p_CnctInfoTmpPt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
				else
				{
					VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：连接%uzd：发送对讲索引包失败。对讲索引：%uzd。总长度：2。原因：" ), p_CnctInfoTmp2Pt->m_Idx, p_CnctInfoTmpPt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
			}
		}
	}

	//补发之前的对讲索引包。
	{
		SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypTkbkIdx; //设置对讲索引包。

		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) && ( p_CnctInfoTmp2Pt->m_Idx != p_CnctInfoTmpPt->m_Idx ) ) //如果连接信息已初始化，且当前连接状态为已连接，且不是本次连接信息。
			{
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = p_CnctInfoTmp2Pt->m_Idx; //设置对讲索引。
				if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：发送对讲索引包成功。对讲索引：%uzd。总长度：2。" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmp2Pt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
				else
				{
					VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：连接%uzd：发送对讲索引包失败。对讲索引：%uzd。总长度：2。原因：" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmp2Pt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
			}
		}
	}
		
	//补发之前的对讲模式包。
	{
		SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypTkbkMode; //设置对讲模式包。

		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) && ( p_CnctInfoTmp2Pt->m_Idx != p_CnctInfoTmpPt->m_Idx ) ) //如果连接信息已初始化，且当前连接状态为已连接，且不是本次连接信息。
			{
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = p_CnctInfoTmp2Pt->m_Idx; //设置对讲索引。
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] = p_CnctInfoTmp2Pt->m_RmtTkbkMode; //设置对讲模式。
				if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 3, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 3, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：发送对讲模式包成功。对讲索引：%uzd。对讲模式：%z8s。总长度：3。" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmp2Pt->m_Idx, g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ] );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
				else
				{
					VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：连接%uzd：发送对讲模式包失败。对讲索引：%uzd。对讲模式：%z8s。总长度：3。原因：" ), p_CnctInfoTmpPt->m_Idx, p_CnctInfoTmp2Pt->m_Idx, g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ] );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
			}
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		if( p_CnctInfoTmpPt != NULL )
		{
			if( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeNameVstrPt );
			if( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt != NULL ) VstrDstoy( p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			p_CnctInfoTmpPt->m_IsInit = 0; //设置连接信息未初始化。
		}
	}
	return p_CnctInfoTmpPt;
}

//连接信息销毁。
void SrvrThrdCnctInfoDstoy( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	SrvrThrd::CnctInfo * p_CnctInfoTmpPt;
	size_t p_ElmTotal; //存放元素总数。

	if( CnctInfoPt->m_IsInit != 0 )
	{
		//全部发送退出包。
		SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypExit; //设置退出包。
		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) && ( p_CnctInfoTmpPt->m_IsRecvExitPkt == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) ) //如果连接信息已初始化，且未接收退出包，且当前连接状态为已连接。
			{
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = CnctInfoPt->m_Idx; //设置对讲索引。
				if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：发送退出包成功。对讲索引：%uzd。总长度：2。" ), p_CnctInfoTmpPt->m_Idx, CnctInfoPt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
				else
				{
					VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：连接%uzd：发送退出包失败。对讲索引：%uzd。总长度：2。原因：" ), p_CnctInfoTmpPt->m_Idx, CnctInfoPt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				}
			}
		}

		//销毁本端Tcp协议客户端套接字。
		if( CnctInfoPt->m_TcpClntSoktPt != NULL )
		{
			TcpClntDstoy( CnctInfoPt->m_TcpClntSoktPt, UINT16_MAX, NULL );
			CnctInfoPt->m_TcpClntSoktPt = NULL;
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：销毁本端Tcp协议客户端套接字成功。" ), CnctInfoPt->m_Idx );
		}

		//销毁本端高级Udp协议客户端连接。
		if( CnctInfoPt->m_AudpClntCnctIdx != -1 )
		{
			AudpClosCnct( SrvrThrdPt->m_AudpSrvrSoktPt, CnctInfoPt->m_AudpClntCnctIdx, NULL );
			CnctInfoPt->m_AudpClntCnctIdx = -1;
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：销毁本端高级Udp协议客户端连接成功。" ), CnctInfoPt->m_Idx );
		}

		//设置当前连接状态。
		if( CnctInfoPt->m_CurCnctSts <= SrvrThrd::CnctStsWait )
		{
			CnctInfoPt->m_CurCnctSts = SrvrThrd::CnctStsTmot;
			SrvrThrdPt->m_UserCnctStsFuncPt( SrvrThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
		}
		else if( CnctInfoPt->m_CurCnctSts == SrvrThrd::CnctStsCnct )
		{
			CnctInfoPt->m_CurCnctSts = SrvrThrd::CnctStsDsct;
			SrvrThrdPt->m_UserCnctStsFuncPt( SrvrThrdPt, CnctInfoPt, CnctInfoPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
		}
		
		SrvrThrdPt->m_UserCnctDstoyFuncPt( SrvrThrdPt, CnctInfoPt ); //调用用户定义的连接销毁函数。
		VstrDstoy( CnctInfoPt->m_RmtNodeNameVstrPt ); //销毁远端套接字绑定的远端节点名称动态字符串。
		VstrDstoy( CnctInfoPt->m_RmtNodeSrvcVstrPt ); //销毁远端套接字绑定的远端节点服务动态字符串。
		CnctInfoPt->m_IsInit = 0; //设置连接信息未初始化。
		
		//递减后面连接信息的序号。
		for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
		{
			if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) && ( p_CnctInfoTmpPt->m_Num > CnctInfoPt->m_Num ) )
			{
				p_CnctInfoTmpPt->m_Num--; //设置后面连接信息的序号全部递减1。
			}
		}
		SrvrThrdPt->m_CnctInfoCurMaxNum--; //递减连接信息的当前最大序号。
		
		VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：已销毁。" ), CnctInfoPt->m_Idx );
		if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
		SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		
		SrvrThrdIsAutoRqirExit( SrvrThrdPt, NULL ); //判断是否自动请求退出。
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
}

//判断是否自动请求退出。
void SrvrThrdIsAutoRqirExit( SrvrThrd * SrvrThrdPt, Vstr * ErrInfoVstrPt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	size_t p_ElmTotal; //存放元素总数。
	
	//判断各个变量是否正确。
	if( SrvrThrdPt == NULL )
	{
		VstrCpy( ErrInfoVstrPt, Cu8vstr( "服务端线程的指针不正确。" ), , );
		goto Out;
	}

	if( SrvrThrdPt->m_IsAutoRqirExit == 0 )
	{

	}
	else if( SrvrThrdPt->m_IsAutoRqirExit == 1 )
	{
		if( ( SrvrThrdPt->m_CnctInfoCurMaxNum == -1 ) && ( SrvrThrdPt->m_CnctInfoCurMaxNum == -1 ) )
		{
			SrvrThrdSendRqirExitMsg( SrvrThrdPt, 0, NULL );

			VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：所有连接已销毁，自动请求退出。" ), , );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		}
	}
	else if( SrvrThrdPt->m_IsAutoRqirExit == 2 )
	{
		if( ( SrvrThrdPt->m_CnctInfoCurMaxNum == -1 ) && ( SrvrThrdPt->m_CnctInfoCurMaxNum == -1 ) && ( SrvrThrdPt->m_SrvrIsInit == 0 ) )
		{
			SrvrThrdSendRqirExitMsg( SrvrThrdPt, 0, NULL );

			VstrCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：所有连接和服务端已销毁，自动请求退出。" ), , );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
		}
	}
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return;
}

//连接处理，包括接受连接、接收发送数据包、删除连接。
void SrvrThrdCnctPocs( SrvrThrd * SrvrThrdPt )
{
    int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
    SrvrThrd::CnctInfo * p_CnctInfoTmpPt;
	SrvrThrd::CnctInfo * p_CnctInfoTmp2Pt;
	size_t p_PktLenByt;
	uint32_t p_TmpUint32;

	//用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接。
	if( SrvrThrdPt->m_TcpSrvrSoktPt != NULL )
	{
		int p_PocsRslt = -1; //存放本处理段执行结果，为0表示成功，为非0表示失败。
		TcpClntSokt * p_TcpClntSoktTmpPt = NULL;

		while( true )
		{
			if( TcpSrvrAcpt( SrvrThrdPt->m_TcpSrvrSoktPt, &p_TcpClntSoktTmpPt, NULL, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt, ( short )0, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( p_TcpClntSoktTmpPt != NULL ) //如果用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接成功。
				{
					if( TcpClntSetNoDelay( p_TcpClntSoktTmpPt, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。
					{
						VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端Tcp协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
						if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
						SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetSendBufSz( p_TcpClntSoktTmpPt, 1024 * 1024, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端Tcp协议客户端套接字的发送缓冲区大小失败。原因：" ) );
						if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
						SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetRecvBufSz( p_TcpClntSoktTmpPt, 1024 * 1024, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端Tcp协议客户端套接字的接收缓冲区大小失败。原因：" ) );
						if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
						SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( TcpClntSetKeepAlive( p_TcpClntSoktTmpPt, 1, 1, 1, 5, 0, SrvrThrdPt->m_ErrInfoVstrPt ) != 0 )
					{
						VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：设置本端Tcp协议客户端套接字的保活机制失败。原因：" ) );
						if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
						SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
						goto TcpSrvrSoktAcptOut;
					}

					if( ( p_CnctInfoTmpPt = SrvrThrdCnctInfoInit( SrvrThrdPt, 0, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt, p_TcpClntSoktTmpPt, -1, SrvrThrd::CnctStsCnct ) ) == NULL ) goto TcpSrvrSoktAcptOut; //如果连接信息初始化失败。
					SrvrThrdPt->m_UserCnctStsFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
					SrvrThrdPt->m_UserCnctRmtTkbkModeFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtTkbkMode, p_CnctInfoTmpPt->m_RmtTkbkMode ); //调用用户定义的连接远端对讲模式函数。
					
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字[%vs:%vs]的连接成功。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
					break;
				}
				else //如果用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接超时，就跳出接受。
				{
					goto TcpSrvrSoktAcptOut;
				}
			}
			else
			{
				VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：用本端Tcp协议服务端套接字接受远端Tcp协议客户端套接字的连接失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto TcpSrvrSoktAcptOut;
			}
		}

		p_PocsRslt = 0; //设置本处理段执行成功。

		TcpSrvrSoktAcptOut:
		if( p_PocsRslt != 0 ) //如果本处理段执行失败。
		{
			if( p_TcpClntSoktTmpPt != NULL )
			{
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypExit; //设置退出包。
				TcpClntSendApkt( p_TcpClntSoktTmpPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 1, 0, 1, 0, NULL ); //发送退出包。
				TcpClntDstoy( p_TcpClntSoktTmpPt, UINT16_MAX, NULL );
			}
		}
	}

	//用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接。
	if( SrvrThrdPt->m_AudpSrvrSoktPt != NULL )
	{
		int p_PocsRslt = -1; //存放本处理段的执行结果，为0表示成功，为非0表示失败。
		size_t p_AudpCnctIdx = SIZE_MAX; //存放本端高级UDP协议连接索引。

		while( true ) //循环接受远端高级Udp协议套接字的连接。
		{
			if( AudpAcpt( SrvrThrdPt->m_AudpSrvrSoktPt, &p_AudpCnctIdx, NULL, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt, ( short )0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( p_AudpCnctIdx != SIZE_MAX ) //如果用本端高级Udp协议套接字接受远端高级Udp协议套接字的连接成功。
				{
					if( ( p_CnctInfoTmpPt = SrvrThrdCnctInfoInit( SrvrThrdPt, 1, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt, NULL, p_AudpCnctIdx, SrvrThrd::CnctStsCnct ) ) == NULL ) goto AudpSrvrSoktAcptOut; //如果连接信息初始化失败。
					SrvrThrdPt->m_UserCnctStsFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
					SrvrThrdPt->m_UserCnctRmtTkbkModeFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_RmtTkbkMode, p_CnctInfoTmpPt->m_RmtTkbkMode ); //调用用户定义的连接远端对讲模式函数。
					
					VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字[%vs:%vs]的连接成功。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_RmtNodeAddrPt, SrvrThrdPt->m_Thrd.m_RmtNodePortPt );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
					break;
				}
				else //如果用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接超时，就跳出接受。
				{
					goto AudpSrvrSoktAcptOut;
				}
			}
			else
			{
				VstrIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：用本端高级Udp协议服务端套接字接受远端高级Udp协议客户端套接字的连接失败。原因：" ) );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto AudpSrvrSoktAcptOut;
			}
		}

		p_PocsRslt = 0; //设置本处理段执行成功。

		AudpSrvrSoktAcptOut:
		if( p_PocsRslt != 0 ) //如果本处理段执行失败。
		{
			if( p_AudpCnctIdx != SIZE_MAX )
			{
				SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypExit; //设置退出包。
				AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_AudpCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 1, 1, 1, NULL ); //发送退出包。
				AudpClosCnct( SrvrThrdPt->m_AudpSrvrSoktPt, p_AudpCnctIdx, NULL );
			}
		}
	}

    //遍历连接信息容器。
    for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
    {
		if( p_CnctInfoTmpPt->m_IsInit != 0 )
		{
			//用本端套接字接收连接的远端套接字发送的数据包。
			if( ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) ) //如果该连接未请求销毁，且当前连接状态为已连接。
			{
				if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntRecvApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, SrvrThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, 0, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpRecvApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, SrvrThrdPt->m_Thrd.m_TmpByteSz, &p_PktLenByt, NULL, ( short )0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					if( p_PktLenByt != -1 ) //如果用本端套接字接收连接的远端套接字发送的数据包成功。
					{
						if( p_PktLenByt == 0 ) //如果数据包的长度为0。
						{
							if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收数据包。长度为%uzd，表示没有数据，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
							goto RecvPktOut;
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypTkbkMode ) //如果是对讲模式包。
						{
							if( p_PktLenByt < 1 + 1 + 1 ) //如果对讲模式包的长度小于1 + 1 + 1，表示没有对讲索引和对讲模式。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收对讲模式包。长度为%uzd小于1 + 1 + 1，表示没有对讲索引和对讲模式，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收对讲模式包。对讲索引为%uz8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}
							if( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] < SrvrThrd::TkbkModeNone ) || ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] >= SrvrThrd::TkbkModeNoChg ) )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收对讲模式包。对讲模式为%z8d不正确，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] );
								goto RecvPktOut;
							}

							int32_t p_OldRmtTkbkMode = p_CnctInfoTmpPt->m_RmtTkbkMode; //设置旧远端对讲模式。
							p_CnctInfoTmpPt->m_RmtTkbkMode = SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ]; //设置远端对讲模式。
							SrvrThrdPt->m_UserCnctRmtTkbkModeFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_OldRmtTkbkMode, p_CnctInfoTmpPt->m_RmtTkbkMode ); //调用用户定义的连接远端对讲模式函数。
							if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收对讲模式包。对讲索引：%uz8d。对讲模式：%z8s。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ] );

							//全部发送对讲模式包。
							{
								for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
								{
									if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) && ( p_CnctInfoTmp2Pt->m_Idx != p_CnctInfoTmpPt->m_Idx ) ) //如果连接信息已初始化，且当前连接状态为已连接，且不是发送端的连接信息。
									{
										if( ( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmp2Pt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
											( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmp2Pt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
										{
											LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送对讲模式包成功。对讲索引：%uz8d。对讲模式：%z8s。总长度：%uzd。" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ], p_PktLenByt );
										}
										else
										{
											LOGFE( Cu8vstr( "服务端线程：连接%uzd：发送对讲模式包失败。对讲索引：%uz8d。对讲模式：%z8s。总长度：%uzd。原因：%vs" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], g_TkbkModeU8strArrPt[ p_CnctInfoTmpPt->m_RmtTkbkMode ], p_PktLenByt, SrvrThrdPt->m_ErrInfoVstrPt );
										}
									}
								}
							}
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypAdoFrm ) //如果是音频输出帧包。
						{
							if( p_PktLenByt < 1 + 1 + 4 ) //如果音频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收一个音频输出帧包的数据长度为%uzd小于1 + 1 + 4，表示没有对讲索引和音频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收音频输出帧包。对讲索引为%z8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}

							//读取音频输出帧时间戳。
							p_TmpUint32 = ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

							if( p_PktLenByt > 1 + 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收有语音活动的音频输出帧包。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
							else //如果该音频输出帧为无语音活动。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收无语音活动的音频输出帧包。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
							
							//全部发送音频输出帧包。
							if( ( p_CnctInfoTmpPt->m_RmtTkbkMode & SrvrThrd::TkbkModeAdoInpt ) != 0 ) //如果远端对讲模式有音频输入。
							{
								for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
								{
									if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) && ( ( p_CnctInfoTmp2Pt->m_RmtTkbkMode & SrvrThrd::TkbkModeAdoOtpt ) != 0 ) && ( p_CnctInfoTmp2Pt->m_Idx != p_CnctInfoTmpPt->m_Idx ) ) //如果连接信息已初始化，且当前连接状态为已连接，且对讲模式有音频输出，且不是发送端的连接信息。
									{
										if( ( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmp2Pt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
											( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmp2Pt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
										{
											LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送音频输出帧包成功。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
										}
										else
										{
											LOGFE( Cu8vstr( "服务端线程：连接%uzd：发送音频输出帧包失败。对讲索引：%uz8d。音频输出帧时间戳：%uz32d。总长度：%uzd。原因：%vs" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt, SrvrThrdPt->m_ErrInfoVstrPt );
										}
									}
								}
							}
							else //如果远端对讲模式无音频输入。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：远端对讲模式无音频输入，不进行全部发送音频输出帧包。" ), p_CnctInfoTmpPt->m_Idx );
							}
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypVdoFrm ) //如果是视频输出帧包。
						{
							if( p_PktLenByt < 1 + 1 + 4 ) //如果视频输出帧包的长度小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收一个视频输出帧包的数据长度为%uzd小于1 + 1 + 4，表示没有对讲索引和视频输出帧时间戳，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收视频输出帧包。对讲索引为%z8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}

							//读取视频输出帧时间戳。
							p_TmpUint32 = ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 2 ] & 0xFF ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 3 ] & 0xFF ) << 8 ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 4 ] & 0xFF ) << 16 ) + ( ( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 5 ] & 0xFF ) << 24 );

							if( p_PktLenByt > 1 + 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收有图像活动的视频输出帧包。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。类型：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 10 ] & 0xff );
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收无图像活动的视频输出帧包。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
							}
							
							//全部发送视频输出帧包。
							if( ( p_CnctInfoTmpPt->m_RmtTkbkMode & SrvrThrd::TkbkModeVdoInpt ) != 0 ) //如果远端对讲模式有视频输入。
							{
								for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmp2Pt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
								{
									if( ( p_CnctInfoTmp2Pt->m_IsInit != 0 ) && ( p_CnctInfoTmp2Pt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) && ( ( p_CnctInfoTmp2Pt->m_RmtTkbkMode & SrvrThrd::TkbkModeVdoOtpt ) != 0 ) && ( p_CnctInfoTmp2Pt->m_Idx != p_CnctInfoTmpPt->m_Idx ) ) //如果连接信息已初始化，且当前连接状态为已连接，且对讲模式有视频输出，且不是发送端的连接信息。
									{
										if( ( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmp2Pt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
											( ( p_CnctInfoTmp2Pt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmp2Pt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, p_PktLenByt, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
										{
											LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送视频输出帧包成功。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt );
										}
										else
										{
											LOGFE( Cu8vstr( "服务端线程：连接%uzd：发送视频输出帧包失败。对讲索引：%uz8d。视频输出帧时间戳：%uz32d。总长度：%uzd。原因：%vs" ), p_CnctInfoTmp2Pt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_TmpUint32, p_PktLenByt, SrvrThrdPt->m_ErrInfoVstrPt );
										}
									}
								}
							}
							else //如果远端对讲模式无视频输入。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：远端对讲模式无视频输入，不进行全部发送音频输出帧包。" ), p_CnctInfoTmpPt->m_Idx );
							}
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypTstNtwkDly ) //如果是测试网络延迟包。
						{
							if( p_PktLenByt < 1 + 1 ) //如果测试网络延迟包的长度小于1 + 1，表示没有对讲索引。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟包。对讲索引为%z8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}

							if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟包。对讲索引：%z8d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_PktLenByt );

							SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypTstNtwkDlyRply; //设置测试网络延迟应答包。
							if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
								( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
							{
								LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送测试网络延迟应答包成功。对讲索引：%uz8d。总长度：2。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
							}
							else
							{
								LOGFE( Cu8vstr( "服务端线程：连接%uzd：发送测试网络延迟应答包失败。对讲索引：%uz8d。总长度：2。原因：%vs" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], SrvrThrdPt->m_ErrInfoVstrPt );
							}
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypTstNtwkDlyRply ) //如果是测试网络延迟应答包。
						{
							if( p_PktLenByt < 1 + 1 ) //如果测试网络延迟应答包的长度小于1 + 1，表示没有对讲索引。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟应答包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟应答包。对讲索引为%z8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}

							uint64_t p_NtwkDlyMsec = FuncGetTickAsMsec() - p_CnctInfoTmpPt->m_TstNtwkDly.m_LastSendTickMsec; //存放网络延迟，单位为毫秒。

							if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接%uzd：接收测试网络延迟应答包。对讲索引：%z8d。延迟：%uz64d。总长度：%uzd。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_NtwkDlyMsec, p_PktLenByt );

							if( SrvrThrdPt->m_TstNtwkDly.m_IsTstNtwkDly != 0 ) //如果要测试网络延迟。
							{
								p_CnctInfoTmpPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包。
								SrvrThrdPt->m_UserCnctTstNtwkDlyFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_NtwkDlyMsec ); //调用用户定义的连接测试网络延迟函数。
							}
						}
						else if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] == SrvrThrd::PktTypExit ) //如果是退出包。
						{
							if( p_PktLenByt < 1 + 1 ) //如果退出包的长度小于1 + 1，表示没有对讲索引。
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收退出包。长度为%uzd小于1 + 1，表示没有对讲索引，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, p_PktLenByt );
								goto RecvPktOut;
							}
							if( SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] != p_CnctInfoTmpPt->m_Idx )
							{
								if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：连接%uzd：接收退出包。对讲索引为%z8d与发送端的对讲索引%uzd不一致，无法继续接收。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], p_CnctInfoTmpPt->m_Idx );
								goto RecvPktOut;
							}

							p_CnctInfoTmpPt->m_IsRecvExitPkt = 1; //设置已接收退出包。
							p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

							VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：接收退出包。对讲索引：%uz8d。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
							if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
							SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
							if( SrvrThrdPt->m_IsShowToast != 0 ) SrvrThrdPt->m_UserShowToastFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
						}
					} //如果用本端套接字接收一个连接的远端套接字发送的数据包超时，就重新接收。
				}
				else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
				{
					p_CnctInfoTmpPt->m_CurCnctSts = SrvrThrd::CnctStsTmot; //设置当前连接状态为异常断开。
					SrvrThrdPt->m_UserCnctStsFuncPt( SrvrThrdPt, p_CnctInfoTmpPt, p_CnctInfoTmpPt->m_CurCnctSts ); //调用用户定义的连接状态函数。
					p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。

					VstrFmtIns( SrvrThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "服务端线程：连接%uzd：用本端套接字接收连接的远端套接字发送的数据包失败。原因：" ), p_CnctInfoTmpPt->m_Idx );
					if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
					SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
					goto RecvPktOut;
				}
				RecvPktOut:;
			}

			//用本端客户端套接字测试网络延迟。
			if( ( SrvrThrdPt->m_TstNtwkDly.m_IsTstNtwkDly != 0 ) && ( p_CnctInfoTmpPt->m_IsRqstDstoy == 0 ) && ( p_CnctInfoTmpPt->m_CurCnctSts == SrvrThrd::CnctStsCnct ) ) //如果要测试网络延迟，且连接未请求销毁，且当前连接状态为已连接。
			{
				uint64_t p_CurTickMsec = FuncGetTickAsMsec(); //存放当前嘀嗒钟，单位为毫秒。

				if( ( p_CnctInfoTmpPt->m_TstNtwkDly.m_IsRecvRplyPkt != 0 ) && ( p_CurTickMsec - p_CnctInfoTmpPt->m_TstNtwkDly.m_LastSendTickMsec >= SrvrThrdPt->m_TstNtwkDly.m_SendIntvlMsec ) ) //如果已接收测试网络延迟应答包，且最后发送测试网络延迟包已超过间隔时间。
				{
					SrvrThrdPt->m_Thrd.m_TmpBytePt[ 0 ] = SrvrThrd::PktTypTstNtwkDly; //设置数据包类型为测试网络延迟包。
					SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] = p_CnctInfoTmpPt->m_Idx; //设置对讲索引。
					if( ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) && ( TcpClntSendApkt( p_CnctInfoTmpPt->m_TcpClntSoktPt, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 0, 1, 0, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
						( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 1 ) && ( AudpSendApkt( SrvrThrdPt->m_AudpSrvrSoktPt, p_CnctInfoTmpPt->m_AudpClntCnctIdx, SrvrThrdPt->m_Thrd.m_TmpBytePt, 2, 1, 1, SrvrThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
					{
						LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送测试网络延迟包成功。对讲索引：%uz8d。总长度：2。" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ] );
						
						p_CnctInfoTmpPt->m_TstNtwkDly.m_LastSendTickMsec = p_CurTickMsec; //设置测试网络延迟包最后发送的嘀嗒钟。
						p_CnctInfoTmpPt->m_TstNtwkDly.m_IsRecvRplyPkt = 0; //设置未接收测试网络延迟应答包。
					}
					else
					{
						LOGFI( Cu8vstr( "服务端线程：连接%uzd：发送测试网络延迟包失败。对讲索引：%uz8d。总长度：2。原因：%vs" ), p_CnctInfoTmpPt->m_Idx, SrvrThrdPt->m_Thrd.m_TmpBytePt[ 1 ], SrvrThrdPt->m_ErrInfoVstrPt );
					}
				}
			}

			//销毁连接信息。
			if( p_CnctInfoTmpPt->m_IsRqstDstoy == 1 ) //如果该连接已请求销毁。
			{
				SrvrThrdCnctInfoDstoy( SrvrThrdPt, p_CnctInfoTmpPt );
			}
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{

	}
	return;
}

//线程消息处理。
int SrvrThrdThrdMsgPocs( MsgQueue * MsgQueuePt, SrvrThrd * SrvrThrdPt, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为1表示线程消息容器为空，为-1表示失败。
	int32_t p_TmpInt32;
	SrvrThrd::CnctInfo * p_CnctInfoTmpPt = NULL;

	switch( MsgTyp )
	{
		case ThrdMsgTypSetIsUsePrvntSysSleep:
		{
			ThrdMsgSetIsUsePrvntSysSleep * p_ThrdMsgSetIsUsePrvntSysSleepPt = ( ThrdMsgSetIsUsePrvntSysSleep * )MsgPt;

			SrvrThrdPt->m_IsUsePrvntSysSleep = p_ThrdMsgSetIsUsePrvntSysSleepPt->m_IsUsePrvntSysSleep;
			SrvrThrdPrvntSysSleepInitOrDstoy( SrvrThrdPt, SrvrThrdPt->m_IsUsePrvntSysSleep );
			break;
		}
		case ThrdMsgTypSetIsTstNtwkDly:
		{
			ThrdMsgSetIsTstNtwkDly * p_ThrdMsgSetIsTstNtwkDlyPt = ( ThrdMsgSetIsTstNtwkDly * )MsgPt;

			SrvrThrdPt->m_TstNtwkDly.m_IsTstNtwkDly = p_ThrdMsgSetIsTstNtwkDlyPt->m_IsTstNtwkDly; //设置是否测试网络延迟。
			SrvrThrdPt->m_TstNtwkDly.m_SendIntvlMsec = p_ThrdMsgSetIsTstNtwkDlyPt->m_SendIntvlMsec; //设置测试网络延迟包的发送间隔。

			for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
			{
				if( p_CnctInfoTmpPt->m_IsInit != 0 )
				{
					p_CnctInfoTmpPt->m_TstNtwkDly.m_LastSendTickMsec = 0; //设置测试网络延迟包最后发送的嘀嗒钟为0，这样可以立即开始发送。
					p_CnctInfoTmpPt->m_TstNtwkDly.m_IsRecvRplyPkt = 1; //设置已接收测试网络延迟应答包，这样可以立即开始发送。
				}
			}
			break;
		}
		case ThrdMsgTypSrvrInit:
		{
			ThrdMsgSrvrInit * p_ThrdMsgSrvrInitPt = ( ThrdMsgSrvrInit * )MsgPt;
			
			p_Rslt = SrvrThrdSrvrInit( SrvrThrdPt, p_ThrdMsgSrvrInitPt->m_IsTcpOrAudpPrtcl, p_ThrdMsgSrvrInitPt->m_LclNodeNameVstrPt, p_ThrdMsgSrvrInitPt->m_LclNodeSrvcVstrPt, p_ThrdMsgSrvrInitPt->m_MaxCnctNum, p_ThrdMsgSrvrInitPt->m_IsAutoRqirExit );
			VstrDstoy( p_ThrdMsgSrvrInitPt->m_LclNodeNameVstrPt );
			VstrDstoy( p_ThrdMsgSrvrInitPt->m_LclNodeSrvcVstrPt );
			goto Out;
		}
		case ThrdMsgTypSrvrDstoy:
		{
			ThrdMsgSrvrDstoy * p_ThrdMsgSrvrDstoyPt = ( ThrdMsgSrvrDstoy * )MsgPt;
			
			SrvrThrdSrvrDstoy( SrvrThrdPt );
			break;
		}
		case ThrdMsgTypCnctDstoy:
		{
			ThrdMsgCnctDstoy * p_ThrdMsgCnctDstoyPt = ( ThrdMsgCnctDstoy * )MsgPt;

			if( ( p_ThrdMsgCnctDstoyPt->m_CnctNum > SrvrThrdPt->m_CnctInfoCurMaxNum ) || ( p_ThrdMsgCnctDstoyPt->m_CnctNum < 0 ) )
			{
				VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：没有序号为%d的连接，无法删除。" ), p_ThrdMsgCnctDstoyPt->m_CnctNum );
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGE( SrvrThrdPt->m_ErrInfoVstrPt );
				SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );
				goto OutCnctDstoy;
			}

			for( size_t p_CnctInfoLstIdx = 0; CQueueGetByNum( SrvrThrdPt->m_CnctInfoCntnrPt, p_CnctInfoLstIdx, NULL, ( void * * )&p_CnctInfoTmpPt, 0, 0, NULL ) == 0; p_CnctInfoLstIdx++ )
			{
				if( ( p_CnctInfoTmpPt->m_IsInit != 0 ) && ( p_CnctInfoTmpPt->m_Num == p_ThrdMsgCnctDstoyPt->m_CnctNum ) )
				{
					p_CnctInfoTmpPt->m_IsRqstDstoy = 1; //设置已请求销毁。
					break;
				}
			}

			VstrFmtCpy( SrvrThrdPt->m_ErrInfoVstrPt, Cu8vstr( "服务端线程：连接%uzd：请求销毁远端节点%vs[%vs:%vs]的连接。" ), p_CnctInfoTmpPt->m_Idx, ( ( p_CnctInfoTmpPt->m_IsTcpOrAudpPrtcl == 0 ) ? Cu8vstr( "Tcp协议" ) : Cu8vstr( "高级Udp协议" ) ), p_CnctInfoTmpPt->m_RmtNodeNameVstrPt, p_CnctInfoTmpPt->m_RmtNodeSrvcVstrPt );
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( SrvrThrdPt->m_ErrInfoVstrPt );
			SrvrThrdPt->m_UserShowLogFuncPt( SrvrThrdPt, SrvrThrdPt->m_ErrInfoVstrPt );

			p_Rslt = 0; //设置本函数执行成功。

			OutCnctDstoy:
			if( p_Rslt != 0 ) //如果本函数执行失败。
			{

			}
			break;
		}
		case ThrdMsgTypRqirExit:
		{
			ThrdMsgRqirExit * p_ThrdMsgRqirExitPt = ( ThrdMsgRqirExit * )MsgPt;
			
			if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "服务端线程：接收退出请求。" ) );
			SrvrThrdPt->m_IsRqirExit = 1; //设置已请求退出。
			if( SrvrThrdPt->m_SrvrIsInit != 0 ) SrvrThrdSrvrDstoy( SrvrThrdPt ); //服务端销毁。
			break;
		}
		default: //用户消息。
		{
			p_TmpInt32 = SrvrThrdPt->m_UserMsgFuncPt( SrvrThrdPt, MsgTyp - ThrdMsgTypUserMsgMinVal, MsgPt, MsgLenByt );
			if( p_TmpInt32 == 0 )
			{
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：调用用户定义的消息函数成功。返回值：%d。" ), p_TmpInt32 );
			}
			else
			{
				if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "服务端线程：调用用户定义的消息函数失败。返回值：%d。" ), p_TmpInt32 );
				goto Out;
			}
			break;
		}
	}

    p_Rslt = 0; //设置本函数执行成功。
	
	Out:
	if( p_Rslt < 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//服务端线程主函数。
DWORD WINAPI SrvrThrdRun( SrvrThrd * SrvrThrdPt )
{
	uint64_t p_LastTickMsec = 0;

	SrvrThrdPt->m_ThrdMsgQueuePt->m_MsgPocsThrdInfoPt = SrvrThrdPt->m_Thrd.m_ThrdInfoPt; //设置线程消息队列的消息处理线程信息。

	if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：本地代码指令集名称：%s。" ), ( sizeof( size_t ) == 4 ) ? "x86" : "x64" );

	SrvrThrdPt->m_UserSrvrThrdInitFuncPt( SrvrThrdPt ); //调用用户定义的服务端线程初始化函数。

	//媒体处理循环开始。
	while( true )
	{
		while( MsgQueueMsgPocsThrdMsgPocs( SrvrThrdPt->m_ThrdMsgQueuePt, NULL ) == 0 ); //进行线程消息处理，直到线程消息队列为空。
		if( SrvrThrdPt->m_IsRqirExit != 0 ) break; //如果已请求退出，就表示本线程需要退出。

		if( SrvrThrdPt->m_IsPrintLog != 0 ) p_LastTickMsec = FuncGetTickAsMsec();

		SrvrThrdCnctPocs( SrvrThrdPt ); //连接处理。

		//if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "服务端线程：连接处理全部完毕，耗时 %uz64d 毫秒。" ), FuncGetTickAsMsec() - p_LastTickMsec );

		FuncSleep( 1 ); //暂停一下，避免CPU使用率过高。
	} //媒体处理循环结束。
	
	SrvrThrdPt->m_UserSrvrThrdDstoyFuncPt( SrvrThrdPt ); //调用用户定义的服务端线程销毁函数。
	
	SrvrThrdPrvntSysSleepInitOrDstoy( SrvrThrdPt, 0 ); //销毁阻止系统睡眠。

	if( SrvrThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "服务端线程：本线程已退出。" ) );

	return 0;
}

//回调SrvrThrdCls类的用户定义的初始化函数。
extern "C" void __cdecl SrvrThrdClsUserShowLog( SrvrThrd * SrvrThrdPt, Vstr * InfoVstrPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserShowLog( ( InfoVstrPt != NULL ) ? ( VstrCls * )&InfoVstrPt : NULL );
}

//回调SrvrThrdCls类的用户定义的处理函数。
extern "C" void __cdecl SrvrThrdClsUserShowToast( SrvrThrd * SrvrThrdPt, Vstr * InfoVstrPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserShowToast( ( InfoVstrPt != NULL ) ? ( VstrCls * )&InfoVstrPt : NULL );
}

//回调SrvrThrdCls类的用户定义的销毁函数。
extern "C" int __cdecl SrvrThrdClsUserMsg( SrvrThrd * SrvrThrdPt, unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt )
{
	return ( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserMsg( MsgTyp, MsgPt, MsgLenByt );
}

//回调SrvrThrdCls类的用户定义的服务端线程初始化函数。
extern "C" void __cdecl SrvrThrdClsUserSrvrThrdInit( SrvrThrd * SrvrThrdPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserSrvrThrdInit();
}

//回调SrvrThrdCls类的用户定义的服务端线程销毁函数。
extern "C" void __cdecl SrvrThrdClsUserSrvrThrdDstoy( SrvrThrd * SrvrThrdPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserSrvrThrdDstoy();
}

//回调SrvrThrdCls类的用户定义的服务端初始化函数。
extern "C" void __cdecl SrvrThrdClsUserSrvrInit( SrvrThrd * SrvrThrdPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserSrvrInit();
}

//回调SrvrThrdCls类的用户定义的服务端销毁函数。
extern "C" void __cdecl SrvrThrdClsUserSrvrDstoy( SrvrThrd * SrvrThrdPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserSrvrDstoy();
}

//回调SrvrThrdCls类的用户定义的连接初始化函数。
extern "C" void __cdecl SrvrThrdClsUserCnctInit( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserCnctInit( CnctInfoPt, IsTcpOrAudpPrtcl, RmtNodeNameVstrPt, RmtNodeSrvcVstrPt );
}

//回调SrvrThrdCls类的用户定义的连接销毁函数。
extern "C" void __cdecl SrvrThrdClsUserCnctDstoy( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserCnctDstoy( CnctInfoPt );
}

//回调SrvrThrdCls类的用户定义的连接状态函数。
extern "C" void __cdecl SrvrThrdClsUserCnctSts( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt, int32_t CurCnctSts )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserCnctSts( CnctInfoPt, CurCnctSts );
}

//回调SrvrThrdCls类的用户定义的连接远端对讲模式函数。
extern "C" void __cdecl SrvrThrdClsUserCnctRmtTkbkMode( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserCnctRmtTkbkMode( CnctInfoPt, OldRmtTkbkMode, NewRmtTkbkMode );
}

//回调SrvrThrdCls类的用户定义的连接测试网络延迟函数。
extern "C" void __cdecl SrvrThrdClsUserCnctTstNtwkDly( SrvrThrd * SrvrThrdPt, SrvrThrd::CnctInfo * CnctInfoPt, uint64_t NtwkDlyMsec )
{
	( ( SrvrThrdCls * )SrvrThrdPt->m_UserDataPt )->UserCnctTstNtwkDly( CnctInfoPt, NtwkDlyMsec );
}

int SrvrThrdCls::Init( VstrCls * ErrInfoVstrPt )
{
	return SrvrThrdInit( &m_SrvrThrdPt, this,
						 SrvrThrdClsUserShowLog, SrvrThrdClsUserShowToast, SrvrThrdClsUserMsg,
						 SrvrThrdClsUserSrvrThrdInit, SrvrThrdClsUserSrvrThrdDstoy,
						 SrvrThrdClsUserSrvrInit, SrvrThrdClsUserSrvrDstoy,
						 SrvrThrdClsUserCnctInit, SrvrThrdClsUserCnctDstoy, SrvrThrdClsUserCnctSts, SrvrThrdClsUserCnctRmtTkbkMode, SrvrThrdClsUserCnctTstNtwkDly,
						 ( ErrInfoVstrPt != NULL ) ? ErrInfoVstrPt->m_VstrPt : NULL );
}
