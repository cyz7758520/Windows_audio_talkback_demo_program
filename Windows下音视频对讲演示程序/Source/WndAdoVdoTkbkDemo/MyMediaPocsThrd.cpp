#include "MyMediaPocsThrd.h"

//构造函数。
MyMediaPocsThrdCls::MyMediaPocsThrdCls( HWND MainDlgWndHdl )
{
	memset( &m_MainDlgWndHdl, 0, ( char * )&m_TmpByte3Sz + sizeof( m_TmpByte3Sz ) - ( char * )&m_MainDlgWndHdl );
	m_MainDlgWndHdl = MainDlgWndHdl; //设置主对话框窗口的句柄。
	m_IsInterrupt = 0; //设置未中断。

	m_LclTkbkMode = TkbkModeNone;
	m_RmtTkbkMode = TkbkModeNone;
}

//析构函数。
MyMediaPocsThrdCls::~MyMediaPocsThrdCls()
{
	VstrDstoy( m_Ntwk.m_IPAddrVstrPt );
	VstrDstoy( m_Ntwk.m_PortVstrPt );
	if( m_TmpBytePt != NULL ) free( m_TmpBytePt );
	if( m_TmpByte2Pt != NULL ) free( m_TmpByte2Pt );
	if( m_TmpByte3Pt != NULL ) free( m_TmpByte3Pt );
}

//用户定义的初始化函数。
int MyMediaPocsThrdCls::UserInit()
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	int p_RmtNodeAddrFamly; //存放远端节点的地址族，为4表示IPv4，为6表示IPv6，为0表示自动选择。
	Vstr * p_LclNodeAddrVstrPt = NULL; //存放本地节点地址。
	Vstr * p_LclNodePortVstrPt = NULL; //存放本地节点端口。
	Vstr * p_RmtNodeAddrVstrPt = NULL; //存放远程节点地址。
	Vstr * p_RmtNodePortVstrPt = NULL; //存放远程节点端口。
	size_t p_PktLenByt;

	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgMediaPocsThrdInit, 0, 0 ); //向主对话框发送初始化媒体处理线程消息。

	m_Ntwk.m_RqstCnctRslt = 0; //设置请求连接的结果为没有选择。
	m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	{ //设置远端节点的地址族。
		ADDRINFOW * p_AddrInfoListPt;
		if( GetAddrInfo( ( PCWSTR )m_Ntwk.m_IPAddrVstrPt->m_Pt, NULL, NULL, &p_AddrInfoListPt ) == 0 )
		{
			p_RmtNodeAddrFamly = ( p_AddrInfoListPt->ai_family == AF_INET ) ? 4 : ( p_AddrInfoListPt->ai_family == AF_INET6 ) ? 6 : 0;
			FreeAddrInfo( p_AddrInfoListPt );
		}
		else p_RmtNodeAddrFamly = 0;
	}
	if( m_TmpBytePt == NULL )
	{
		m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( m_TmpBytePt == NULL )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据的大小。
	}
	if( m_TmpByte2Pt == NULL )
	{
		m_TmpByte2Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( m_TmpByte2Pt == NULL )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		m_TmpByte2Sz = 1024 * 1024; //设置临时数据的大小。
	}
	if( m_TmpByte3Pt == NULL )
	{
		m_TmpByte3Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( m_TmpByte3Pt == NULL )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		m_TmpByte3Sz = 1024 * 1024; //设置临时数据的大小。
	}

	if( VstrInit( &p_LclNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_LclNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_RmtNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_RmtNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) //如果要使用TCP协议。
	{
		if( m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrStart, 0, 0 ); //向对讲状态窗口发送开始创建服务器消息。

			if( m_Ntwk.m_TcpSrvrSokt.Init( p_RmtNodeAddrFamly, m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt, 1, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议服务端套接字成功。
			{
				if( m_Ntwk.m_TcpSrvrSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrFail, 0, 0 ); //向对讲状态窗口发送创建服务器失败消息。

					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrSucs, 0, 0 ); //向对讲状态窗口发送创建服务器成功消息。

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else
			{
				if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrFail, 0, 0 ); //向对讲状态窗口发送创建服务器失败消息。

				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( m_Ntwk.m_TcpSrvrSokt.Acpt( &m_Ntwk.m_TcpClntSokt, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( m_Ntwk.m_TcpClntSokt.m_TcpClntSoktPt != NULL ) //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						m_Ntwk.m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。
						
						if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSucs, 0, 0 ); //向对讲状态窗口发送连接服务器或接受连接成功消息。

						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break;
					}
					else //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接服务器或接受连接失败消息。

					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		else if( m_Ntwk.m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), m_Ntwk.m_IPAddrVstrPt );
			WinExec( ( char * )m_Ntwk.m_IPAddrVstrPt->m_Pt, SW_HIDE );
			
			if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSrvrStart, 0, 0 ); //向对讲状态窗口发送开始连接服务器消息。

			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端TCP协议服务端套接字。
			{
				//连接远端
				{
					{
						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}

					if( m_Ntwk.m_TcpClntSokt.Init( p_RmtNodeAddrFamly, m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt, NULL, NULL, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字成功。
					{
						if( m_Ntwk.m_TcpClntSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

							VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto Out;
						}
						if( m_Ntwk.m_TcpClntSokt.GetRmtAddr( NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

							VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
							if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto Out;
						}
						
						if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSucs, 0, 0 ); //向对讲状态窗口发送连接成功消息。

						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议客户端套接字[%vs:%vs]，并连接远端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break; //跳出重连。
					}
					else
					{
						VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > m_Ntwk.m_MaxCnctTimes )
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}

		if( m_Ntwk.m_TcpClntSokt.SetNoDelay( 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_Ntwk.m_TcpClntSokt.SetSendBufSz( 1024 * 1024, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的发送缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的发送缓冲区大小失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_Ntwk.m_TcpClntSokt.SetRecvBufSz( 1024 * 1024, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的接收缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的接收缓冲区大小失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		
		if( m_Ntwk.m_TcpClntSokt.SetKeepAlive( 1, 1, 1, 5, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的保活机制失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的保活机制失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	}
	else //如果要使用UDP协议。
	{
		if( m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端高级UDP协议套接字接受远端高级UDP协议套接字的连接。
		{
			if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrStart, 0, 0 ); //向对讲状态窗口发送开始创建服务器消息。

			if( m_Ntwk.m_AudpSokt.Init( p_RmtNodeAddrFamly, m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt, 1, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
			{
				if( m_Ntwk.m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrFail, 0, 0 ); //向对讲状态窗口发送创建服务器失败消息。

					VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
				
				if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrSucs, 0, 0 ); //向对讲状态窗口发送创建服务器失败消息。

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else //如果初始化本端高级UDP协议套接字失败。
			{
				if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCreateSrvrFail, 0, 0 ); //向对讲状态窗口发送创建服务器失败消息。

				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]失败。原因：" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			while( true ) //循环接受远端高级UDP协议套接字的连接。
			{
				if( m_Ntwk.m_AudpSokt.Acpt( &m_Ntwk.m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( m_Ntwk.m_AudpCnctIdx != SIZE_MAX ) //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接成功。
					{
						if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSucs, 0, 0 ); //向对讲状态窗口发送连接服务器或接受连接成功消息。

						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break;
					}
					else //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接服务器或接受连接失败消息。

					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		else if( m_Ntwk.m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端高级UDP协议套接字连接远端高级UDP协议套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), m_Ntwk.m_IPAddrVstrPt );
			WinExec( ( char * )m_Ntwk.m_IPAddrVstrPt->m_Pt, SW_HIDE );
			
			if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSrvrStart, 0, 0 ); //向对讲状态窗口发送开始连接服务器消息。

			if( m_Ntwk.m_AudpSokt.Init( p_RmtNodeAddrFamly, NULL, NULL, 0, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
			{
				if( m_Ntwk.m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

					VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else //如果初始化本端高级UDP协议套接字失败。
			{
				if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			
			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端高级UDP协议服务端套接字。
			{
				//连接远端。
				{
					{
						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}

					if( m_Ntwk.m_AudpSokt.Cnct( p_RmtNodeAddrFamly, m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt, &m_Ntwk.m_AudpCnctIdx, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果连接远端高级UDP协议套接字成功。
					{
						AudpCnctSts p_AudpCnctSts;

						if( m_Ntwk.m_AudpSokt.WaitCnct( m_Ntwk.m_AudpCnctIdx, UINT16_MAX, &p_AudpCnctSts, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端高级UDP协议套接字连接远端是否成功成功。
						{
							if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
							{
								if( m_Ntwk.m_AudpSokt.GetRmtAddr( m_Ntwk.m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

									VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议客户端套接字连接的远端高级UDP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
									if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
									goto Out;
								}

								if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctSucs, 0, 0 ); //向对讲状态窗口发送连接成功消息。

								VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								break; //跳出重连。
							}
							else //如果连接失败。
							{
								if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
								{
									VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接超时。" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
									if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								}
								else //如果连接断开。
								{
									VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接断开。" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
									if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								}
							}
						}

						m_Ntwk.m_AudpSokt.ClosCnct( m_Ntwk.m_AudpCnctIdx, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //关闭连接，等待重连。
					}
					else
					{
						VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：" ), m_Ntwk.m_IPAddrVstrPt, m_Ntwk.m_PortVstrPt );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > m_Ntwk.m_MaxCnctTimes )
				{
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgCnctFail, 0, 0 ); //向对讲状态窗口发送连接失败消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		
		if( m_Ntwk.m_AudpSokt.SetSendBufSz( 1024 * 1024, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_Ntwk.m_AudpSokt.SetRecvBufSz( 1024 * 1024, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	} //协议连接结束。

	//等待允许连接。
	if( ( m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) && ( m_Ntwk.m_IsAutoAllowCnct != 0 ) ) m_Ntwk.m_RqstCnctRslt = 1;
	else m_Ntwk.m_RqstCnctRslt = 0;
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_RmtNodeAddrVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgInit, ( WPARAM )p_ErrInfoVstrPt, 0 ); } //向主窗口发送初始化请求连接对话框消息。
	while( true )
	{
		if( m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) //如果是服务端。
		{
			if( m_Ntwk.m_RqstCnctRslt == 1 ) //如果允许连接。
			{
				m_TmpBytePt[0] = PktTypAllowCnct; //设置允许连接包。
				if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgAllowCnct, 0, 0 ); //向对讲状态窗口发送允许连接消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个允许连接包成功。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto WaitAllowCnct;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个允许连接包失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
			else if( m_Ntwk.m_RqstCnctRslt == 2 ) //如果拒绝连接。
			{
				m_TmpBytePt[0] = PktTypRefuseCnct; //设置拒绝连接包。
				if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
		}
		else //如果是客户端。
		{
			if( m_Ntwk.m_RqstCnctRslt == 2 ) //如果中断等待。
			{
				m_TmpBytePt[0] = PktTypRefuseCnct; //设置拒绝连接包。
				if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
		}
		
		//接收一个远端发送的数据包。
		if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 0, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
			( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.RecvApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
		{
			if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
			{
				if( ( p_PktLenByt == 1 ) && ( m_TmpBytePt[0] == PktTypAllowCnct ) ) //如果是允许连接包。
				{
					if( m_Ntwk.m_IsCreateSrvrOrClnt == 0 ) //如果是客户端。
					{
						m_Ntwk.m_RqstCnctRslt = 1;
						
						PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
						if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgAllowCnct, 0, 0 ); //向对讲状态窗口发送允许连接消息。

						VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个允许连接包。" ) );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto WaitAllowCnct;
					}
					else //如果是服务端。
					{
						//就重新接收。
					}
				}
				else if( ( p_PktLenByt == 1 ) && ( m_TmpBytePt[0] == PktTypRefuseCnct ) ) //如果是拒绝连接包。
				{
					m_Ntwk.m_RqstCnctRslt = 2;
					
					PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
					if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个拒绝连接包。" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else //如果是其他包。
				{
					//就重新接收。
				}
			}
			else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
			{
				//就重新接收。
			}
		}
		else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
		{
			PostMessage( m_MainDlgWndHdl, MainDlgWndMsgRqstCnctDlgDstoy, 0, 0 ); //向主对话框发送销毁请求连接对话框消息。
			if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgRefuseCnct, 0, 0 ); //向对讲状态窗口发送拒绝连接消息。

			VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	} //等待允许连接结束。
	WaitAllowCnct:;

	m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
	m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。

	switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果要使用链表。
		{
			//初始化接收音频输出帧链表。
			if( m_RecvAdoOtptFrmLnkLst.Init( m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收音频输出帧链表成功。" ) );
			}
			else
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收音频输出帧链表失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			//初始化接收视频输出帧链表。
			if( m_RecvVdoOtptFrmLnkLst.Init( m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, BufAutoAdjMethFreeNumber, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化接收视频输出帧链表成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收视频输出帧链表失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			break;
		}
		case 1: //如果要使用自适应抖动缓冲器。
		{
			//初始化音频自适应抖动缓冲器。
			if( AAjbInit( &m_AAjb.m_Pt, m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, 1, 1, 0, m_AAjb.m_MinNeedBufFrmCnt, m_AAjb.m_MaxNeedBufFrmCnt, m_AAjb.m_MaxCntuLostFrmCnt, m_AAjb.m_AdaptSensitivity, ( m_Ntwk.m_XfrMode == 0 ) ? 0 : 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化音频自适应抖动缓冲器成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化音频自适应抖动缓冲器失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			//初始化视频自适应抖动缓冲器。
			if( VAjbInit( &m_VAjb.m_Pt, 1, m_VAjb.m_MinNeedBufFrmCnt, m_VAjb.m_MaxNeedBufFrmCnt, m_VAjb.m_AdaptSensitivity, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "初始化视频自适应抖动缓冲器成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化视频自适应抖动缓冲器失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			break;
		}
	}

	VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始对讲。" ) );
	if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnInit, 0, 0 ); //向主窗口发送初始化一键即按即通按钮消息。
	{ UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new UserMsgLclTkbkMode(); p_UserMsgLclTkbkModePt->m_LclTkbkMode = TkbkModeNoChg; SendUserMsg( p_UserMsgLclTkbkModePt, NULL ); } //发送对讲模式包。

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_LclNodeAddrVstrPt );
	VstrDstoy( p_LclNodePortVstrPt );
	VstrDstoy( p_RmtNodeAddrVstrPt );
	VstrDstoy( p_RmtNodePortVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
	return p_Rslt;
}

//用户定义的处理函数。
int MyMediaPocsThrdCls::UserPocs()
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	size_t p_PktLenByt;
	uint32_t p_TmpUint32;
	size_t p_TmpLnkLstElmTotal;

	//接收远端发送过来的一个数据包。
	if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 0, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
		( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.RecvApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
	{
		if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
		{
			if( p_PktLenByt == 0 ) //如果数据包的长度为0。
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个数据包的数据长度为%uzd，表示没有数据，无法继续接收。" ), p_PktLenByt );
				goto Out;
			}
			else if( m_TmpBytePt[ 0 ] == PktTypTkbkMode ) //如果是对讲模式包。
			{
				if( p_PktLenByt < 1 + 1 ) //如果音频输出帧包的数据长度小于1 + 1，表示没有对讲模式。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个对讲模式包的数据长度为%uzd小于1 + 1，表示没有对讲模式，无法继续接收。" ), p_PktLenByt );
					goto Out;
				}
				if( m_TmpBytePt[ 1 ] >= TkbkModeNoChg )
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个对讲模式包的对讲模式为%z8d不正确，无法继续接收。" ), m_TmpBytePt[ 1 ] );
					goto Out;
				}

				m_RmtTkbkMode = ( TkbkMode )m_TmpBytePt[ 1 ]; //设置远端对讲模式。
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个对讲模式包。对讲模式：%d。" ), m_RmtTkbkMode );
				SetTkbkMode(); //设置对讲模式。
			}
			else if( m_TmpBytePt[0] == PktTypAdoFrm ) //如果是音频输出帧包。
			{
				if( p_PktLenByt < 1 + 4 ) //如果音频输出帧包的长度小于1 + 4，表示没有音频输出帧时间戳。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个音频输出帧包的数据长度为%uzd小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。" ), p_PktLenByt );
					goto Out;
				}

				//读取音频输出帧时间戳。
				p_TmpUint32 = ( m_TmpBytePt[1] & 0xFF ) + ( ( m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( m_TmpBytePt[4] & 0xFF ) << 24 );

				if( ( m_MediaPocsThrdPt->m_AdoOtpt.m_IsInit != 0 ) || //如果已初始化音频输出。
					( ( m_Ntwk.m_XfrMode == 0 ) && ( ( m_LclTkbkMode == TkbkModeAdo ) || ( m_LclTkbkMode == TkbkModeAdoVdo ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为音频或音视频。
				{
					//将输出帧放入链表或自适应抖动缓冲器。
					switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
					{
						case 0: //如果要使用链表。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								m_RecvAdoOtptFrmLnkLst.GetTotal( &p_TmpLnkLstElmTotal, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的元素总数。
								if( p_TmpLnkLstElmTotal <= 50 )
								{
									if( m_RecvAdoOtptFrmLnkLst.PutTail( m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
									{
										if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
									}
									else
									{
										if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表失败。原因：%vs" ), m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto Out;
									}
								}
								else
								{
									if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个有语音活动的音频输出帧包，但接收音频输出帧链表中帧总数为%uzd已经超过上限50，不再放入接收音频输出帧链表。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpLnkLstElmTotal, p_TmpUint32, p_PktLenByt );
								}
							}
							else //如果该音频输出帧为无语音活动。
							{
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，无需放入接收音频输出帧链表。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}
							break;
						}
						case 1: //如果要使用自适应抖动缓冲器。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								AAjbPutFrm( m_AAjb.m_Pt, p_TmpUint32, m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, NULL );
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}
							else //如果该音频输出帧为无语音活动。
							{
								AAjbPutFrm( m_AAjb.m_Pt, p_TmpUint32, m_TmpBytePt + 1 + 4, 0, 1, NULL );
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}

							int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
							int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							AAjbGetBufFrmCnt( m_AAjb.m_Pt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

							break;
						}
					}
				}
				else //如果未初始化音频输出。
				{
					if( p_PktLenByt > 1 + 4 ) //如果该音频输出帧为有语音活动。
					{
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
					}
					else //如果该音频输出帧为无语音活动。
					{
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
					}
				}
			}
			else if( m_TmpBytePt[0] == PktTypVdoFrm ) //如果是视频输出帧包。
			{
				if( p_PktLenByt < 1 + 4 ) //如果视频输出帧包的长度小于1 + 4，表示没有视频输出帧时间戳。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个视频输出帧包的长度为%uzd小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。" ), p_PktLenByt );
					goto Out;
				}

				//读取视频输出帧时间戳。
				p_TmpUint32 = ( m_TmpBytePt[1] & 0xFF ) + ( ( m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( m_TmpBytePt[4] & 0xFF ) << 24 );

				if( ( m_MediaPocsThrdPt->m_VdoOtpt.m_IsInit != 0 ) || //如果已初始化视频输出。
					( ( m_Ntwk.m_XfrMode == 0 ) && ( ( m_LclTkbkMode == TkbkModeVdo ) || ( m_LclTkbkMode == TkbkModeAdoVdo ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为视频或音视频。
				{
					//将视频输出帧放入链表或自适应抖动缓冲器。
					switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
					{
						case 0: //如果要使用链表。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								m_RecvVdoOtptFrmLnkLst.GetTotal( &p_TmpLnkLstElmTotal, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的元素总数。
								if( p_TmpLnkLstElmTotal <= 20 )
								{
									if( m_RecvVdoOtptFrmLnkLst.PutTail( m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
									{
										if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
									}
									else
									{
										if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表失败。原因：%vs" ), m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto Out;
									}
								}
								else
								{
									if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到一个有图像活动的视频输出帧包，但接收视频输出帧链表中帧总数为%uzd已经超过上限20，不再放入接收视频输出帧链表。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpLnkLstElmTotal, p_TmpUint32, p_PktLenByt );
								}
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入接收视频输出帧链表。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}
							break;
						}
						case 1: //如果要使用自适应抖动缓冲器。
						{
							if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								VAjbPutFrm( m_VAjb.m_Pt, FuncGetTickAsMsec(), p_TmpUint32, m_TmpBytePt + 1 + 4, p_PktLenByt - 1 - 4, 1, NULL );
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}
							else //如果该视频输出帧为无图像活动。
							{
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
							}

							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							VAjbGetBufFrmCnt( m_VAjb.m_Pt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );
							break;
						}
					}
				}
				else //如果未初始化视频输出。
				{
					if( p_PktLenByt > 1 + 4 ) //如果该视频输出帧为有图像活动。
					{
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
					}
					else //如果该视频输出帧为无图像活动。
					{
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_PktLenByt );
					}
				}
			}
			else if( m_TmpBytePt[0] == PktTypExit ) //如果是退出包。
			{
				if( p_PktLenByt > 1 ) //如果退出包的长度大于1。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "接收到退出包的长度为%uzd大于1，表示还有其他数据，无法继续接收。" ), p_PktLenByt );
					goto Out;
				}

				m_IsRecvExitPkt = 1; //设置已经接收到退出包。
				RqirExit( 1, 0, NULL ); //请求退出。

				VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			}
		}
		else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
		{

		}
	}
	else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
	{
		VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	return p_Rslt;
}

//用户定义的销毁函数。
void MyMediaPocsThrdCls::UserDstoy()
{
	size_t p_PktLenByt;

	if( ( ( m_Ntwk.m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( m_Ntwk.m_AudpSokt.m_AudpSoktPt != NULL ) && ( m_Ntwk.m_AudpSokt.GetRmtAddr( m_Ntwk.m_AudpCnctIdx, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
	{
		{
			//发送退出包。
			m_TmpBytePt[0] = PktTypExit; //设置退出包。
			if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) ||
				( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) )
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个退出包失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto SendExitPkt;
			}

			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个退出包成功。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			//接收退出包。
			if( m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
			{
				while( true ) //循环接收退出包。
				{
					if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 5000, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
						( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.RecvApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_PktLenByt, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
					{
						if( p_PktLenByt != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
						{
							if( ( p_PktLenByt == 1 ) && ( m_TmpBytePt[0] == PktTypExit ) ) //如果是退出包。
							{
								VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
								if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								goto SendExitPkt;
							} //如果是其他包，就继续接收。
						}
						else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
						{
							VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
							if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto SendExitPkt;
						}
					}
					else //用本端套接字接收一个连接的远端套接字发送的数据包失败。
					{
						VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
						if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						goto SendExitPkt;
					}
				}
			}
		}
		SendExitPkt:;
		
		if( m_TkbkStsWndHdl != NULL ) PostMessage( m_TkbkStsWndHdl, TkbkStsMsgInterruptTkbk, 0, 0 ); //向对讲状态窗口发送中断对讲消息。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "中断对讲。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//销毁本端TCP协议服务端套接字。
	if( m_Ntwk.m_TcpSrvrSokt.m_TcpSrvrSoktPt != NULL )
	{
		m_Ntwk.m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议服务端套接字成功。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁本端TCP协议客户端套接字。
	if( m_Ntwk.m_TcpClntSokt.m_TcpClntSoktPt != NULL )
	{
		m_Ntwk.m_TcpClntSokt.Dstoy( ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议客户端套接字成功。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁本端高级UDP协议套接字。
	if( m_Ntwk.m_AudpSokt.m_AudpSoktPt != NULL )
	{
		m_Ntwk.m_AudpSokt.Dstoy( NULL ); //关闭并销毁已创建的本端高级UDP协议套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁本端高级UDP协议套接字成功。" ) );
		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁接收音频输出帧的链表。
	if( m_RecvAdoOtptFrmLnkLst.m_VLnkLstPt != NULL )
	{
		m_RecvAdoOtptFrmLnkLst.Dstoy( NULL );

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "销毁接收音频输出帧的链表成功。" ) );
	}

	//销毁接收视频输出帧的链表。
	if( m_RecvVdoOtptFrmLnkLst.m_VLnkLstPt != NULL )
	{
		m_RecvVdoOtptFrmLnkLst.Dstoy( NULL );

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "销毁接收视频输出帧的链表成功。" ) );
	}

	//销毁音频自适应抖动缓冲器。
	if( m_AAjb.m_Pt != NULL )
	{
		AAjbDstoy( m_AAjb.m_Pt, NULL );
		m_AAjb.m_Pt = NULL;

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "销毁音频自适应抖动缓冲器成功。" ) );
	}
	
	//销毁视频自适应抖动缓冲器。
	if( m_VAjb.m_Pt != NULL )
	{
		VAjbDstoy( m_VAjb.m_Pt, NULL );
		m_VAjb.m_Pt = NULL;

		if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "销毁视频自适应抖动缓冲器成功。" ) );
	}

	if( m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
	{
		if( m_IsRecvExitPkt == 1 )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且接收到了退出包，表示是远端套接字主动退出，本线程重新初始化来继续保持监听。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
		else if( ( m_IsInterrupt == 0 ) && ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeUserInit ) && ( m_Ntwk.m_RqstCnctRslt == 2 ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且未中断，且退出码为调用用户定义的初始化函数失败，且请求连接的结果为拒绝，表示是拒绝本次连接，本线程重新初始化来继续保持监听。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
		else if( ( m_IsInterrupt == 0 ) && ( ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeMediaMsgPocs ) || ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs ) ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且未中断，且退出码为媒体消息处理失败或音视频输入输出处理失败，表示是媒体消息处理失败或连接异常断开，本线程重新初始化来继续保持监听。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
		else //其他情况，本线程直接退出。
		{
			PostMessage( m_MainDlgWndHdl, MainDlgWndMsgMediaPocsThrdDstoy, 0, 0 ); //向主对话框发送销毁媒体处理线程消息。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
	}
	else if( m_Ntwk.m_IsCreateSrvrOrClnt == 0 ) //如果是创建客户端。
	{
		if( ( m_IsInterrupt == 0 ) && ( ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeMediaMsgPocs ) || ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs ) ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建客户端，且未中断，且退出码为媒体消息处理失败或音视频输入输出处理失败，表示是媒体消息处理失败或连接异常断开，本线程重新初始化来重连服务端。" ) );
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
		else //其他情况，本线程直接退出。
		{
			PostMessage( m_MainDlgWndHdl, MainDlgWndMsgMediaPocsThrdDstoy, 0, 0 ); //向主对话框发送销毁媒体处理线程消息。
			if( m_Ntwk.m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, MainDlgWndMsgPttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮消息。
		}
	}
}

//设置对讲模式。
void MyMediaPocsThrdCls::SetTkbkMode()
{
	if( m_Ntwk.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		if( m_Ntwk.m_PttBtnIsDown == 0 ) //如果一键即按即通按钮为弹起。
		{
			switch( m_LclTkbkMode )
			{
				case TkbkModeNone: //如果本端对讲模式为空。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeAdo: //如果本端对讲模式为音频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeVdo: //如果本端对讲模式为视频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 1, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeAdoVdo: //如果本端对讲模式为音视频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 1, 0, 1, NULL ); //设置是否使用音视频输入输出。
					break;
				}
			}
		}
		else //如果一键即按即通按钮为按下。
		{
			switch( m_LclTkbkMode )
			{
				case TkbkModeNone: //如果本端对讲模式为空。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeAdo: //如果本端对讲模式为音频。
				{
					SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeVdo: //如果本端对讲模式为视频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case TkbkModeAdoVdo: //如果本端对讲模式为音视频。
				{
					SetIsUseAdoVdoInptOtpt( 1, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
			}
		}
	}
	else //如果传输模式为实时全双工。
	{
		switch( m_LclTkbkMode )
		{
			case TkbkModeNone: //如果本端对讲模式为空。
			{
				SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
				break;
			}
			case TkbkModeAdo: //如果本端对讲模式为音频。
			{
				switch( m_RmtTkbkMode )
				{
					case TkbkModeNone: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdo: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeVdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdoVdo: //如果远端对讲模式为音视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
				}
				break;
			}
			case TkbkModeVdo: //如果本端对讲模式为视频。
			{
				switch( m_RmtTkbkMode )
				{
					case TkbkModeNone: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdo: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeVdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdoVdo: //如果远端对讲模式为音视频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
				}
				break;
			}
			case TkbkModeAdoVdo: //如果本端对讲模式为音视频。
			{
				switch( m_RmtTkbkMode )
				{
					case TkbkModeNone: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdo: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeVdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case TkbkModeAdoVdo: //如果远端对讲模式为音视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
				}
				break;
			}
		}
	}
}

//用户定义的消息函数。
int MyMediaPocsThrdCls::UserMsg( void * MsgArgPt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	switch( ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_UserMsgTyp )
	{
		case UserMsgTypLclTkbkMode:
		{
			if( ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_LclTkbkMode != TkbkModeNoChg ) m_LclTkbkMode = ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_LclTkbkMode; //设置本端对讲模式。
			SetTkbkMode(); //设置对讲模式。
			if( ( ( m_Ntwk.m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( m_Ntwk.m_AudpSokt.m_AudpSoktPt != NULL ) && ( m_Ntwk.m_AudpSokt.GetRmtAddr( m_Ntwk.m_AudpCnctIdx, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
			{
				//发送对讲模式包。
				m_TmpBytePt[0] = PktTypTkbkMode; //设置对讲模式包。
				m_TmpBytePt[1] = m_LclTkbkMode; //设置对讲模式。
				if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, 2, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) ||
					( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, 2, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) )
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个对讲模式包失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
				else
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个对讲模式包成功。对讲模式：%d。" ), m_LclTkbkMode );
				}
			}
			break;
		}
		case UserMsgTypRmtTkbkMode:
		{
			if( ( ( UserMsgRmtTkbkMode * )MsgArgPt )->m_RmtTkbkMode != TkbkModeNoChg ) m_RmtTkbkMode = ( ( UserMsgRmtTkbkMode * )MsgArgPt )->m_RmtTkbkMode; //设置本端对讲模式。
			SetTkbkMode(); //设置对讲模式。
			break;
		}
		case UserMsgTypPttBtnDown:
		{
			m_Ntwk.m_PttBtnIsDown = 1; //设置一键即按即通按钮为按下。
			SetTkbkMode(); //设置对讲模式。
			break;
		}
		case UserMsgTypPttBtnUp:
		{
			m_Ntwk.m_PttBtnIsDown = 0; //设置一键即按即通按钮为弹起。
			SetTkbkMode(); //设置对讲模式。
			break;
		}
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	delete MsgArgPt;
	return p_Rslt;
}

//用户定义的读取音视频输入帧函数。
void MyMediaPocsThrdCls::UserReadAdoVdoInptFrm( int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
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
				memcpy( m_TmpBytePt + 1 + 4, AdoInptPcmRsltFrmPt, m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入Pcm格式帧。
			}
			else //如果本次音频输入帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}
		else //如果有音频输入已编码格式结果帧。
		{
			if( AdoInptPcmRsltFrmVoiceActSts != 0 && AdoInptEncdRsltFrmIsNeedTrans != 0 ) //如果音频输入Pcm格式结果帧为有语音活动，且音频输入已编码格式结果帧需要传输。
			{
				memcpy( m_TmpBytePt + 1 + 4, AdoInptEncdRsltFrmPt, AdoInptEncdRsltFrmLenByt ); //设置音频输入帧。
				p_PktLenByt = 1 + 4 + AdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 音频输入已编码格式帧。
			}
			else //如果本次音频输入帧为无语音活动，或不需要传输。
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			m_TmpBytePt[0] = PktTypAdoFrm;
			//设置音频输入帧时间戳。
			m_TmpBytePt[1] = ( int8_t ) ( m_LastSendAdoInptFrmTimeStamp & 0xFF );
			m_TmpBytePt[2] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			m_TmpBytePt[3] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			m_TmpBytePt[4] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, p_PktLenByt, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, p_PktLenByt, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有语音活动的音频输入帧包失败。原因：" ) );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}

			m_LastSendAdoInptFrmIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( m_LastSendAdoInptFrmIsAct != 0 ) //如果最后一个发送的音频输入帧为有语音活动，就发送。
			{
				m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				m_TmpBytePt[ 0 ] = PktTypAdoFrm;
				//设置音频输入帧时间戳。
				m_TmpBytePt[ 1 ] = ( int8_t )( m_LastSendAdoInptFrmTimeStamp & 0xFF );
				m_TmpBytePt[ 2 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				m_TmpBytePt[ 3 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				m_TmpBytePt[ 4 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, p_PktLenByt, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, p_PktLenByt, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), m_LastSendAdoInptFrmTimeStamp, p_PktLenByt );
				}
				else
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个无语音活动的音频输入帧包失败。原因：" ) );
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{
						Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 );
					}
				}

				m_LastSendAdoInptFrmIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
			}
			else //如果最后一个发送的音频输入帧为无语音活动，无需发送。
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本次音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}

	//发送视频输入帧。
	if( VdoInptYu12RsltFrmPt != NULL ) //如果有视频输入Yu12格式结果帧。
	{
		if( VdoInptEncdRsltFrmPt == NULL ) //如果没有视频输入已编码格式结果帧。
		{
			//设置视频输入帧宽度。
			m_TmpBytePt[5] = ( int8_t ) ( VdoInptYu12RsltFrmWidth & 0xFF );
			m_TmpBytePt[6] = ( int8_t ) ( ( VdoInptYu12RsltFrmWidth & 0xFF00 ) >> 8 );
			m_TmpBytePt[7] = ( int8_t ) ( ( VdoInptYu12RsltFrmWidth & 0xFF0000 ) >> 16 );
			m_TmpBytePt[8] = ( int8_t ) ( ( VdoInptYu12RsltFrmWidth & 0xFF000000 ) >> 24 );
			//设置视频输入帧高度。
			m_TmpBytePt[9] = ( int8_t ) ( VdoInptYu12RsltFrmHeight & 0xFF );
			m_TmpBytePt[10] = ( int8_t ) ( ( VdoInptYu12RsltFrmHeight & 0xFF00 ) >> 8 );
			m_TmpBytePt[11] = ( int8_t ) ( ( VdoInptYu12RsltFrmHeight & 0xFF0000 ) >> 16 );
			m_TmpBytePt[12] = ( int8_t ) ( ( VdoInptYu12RsltFrmHeight & 0xFF000000 ) >> 24 );

			memcpy( m_TmpBytePt + 1 + 4 + 4 + 4, VdoInptYu12RsltFrmPt, VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2 ); //设置视频输入帧。
			p_PktLenByt = 1 + 4 + 4 + 4 + VdoInptYu12RsltFrmWidth * VdoInptYu12RsltFrmHeight * 3 / 2; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + 视频输入Yu12格式结果帧。
		}
		else //如果有视频输入已编码格式结果帧。
		{
			if( VdoInptEncdRsltFrmLenByt != 0 ) //如果本次视频输入已编码格式结果帧为有图像活动。
			{
				memcpy( m_TmpBytePt + 1 + 4, VdoInptEncdRsltFrmPt, VdoInptEncdRsltFrmLenByt ); //设置视频输入帧。
				p_PktLenByt = 1 + 4 + VdoInptEncdRsltFrmLenByt; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 视频输入已编码格式结果帧。
			}
			else
			{
				p_PktLenByt = 1 + 4; //数据包长度 = 数据包类型 + 视频输入帧时间戳。
			}
		}

		if( p_PktLenByt != 1 + 4 ) //如果本次视频输入帧为有图像活动，就发送。
		{
			m_LastSendVdoInptFrmTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

			//设置数据包类型为视频输入帧包。
			m_TmpBytePt[ 0 ] = PktTypVdoFrm;
			//设置视频输入帧时间戳。
			m_TmpBytePt[ 1 ] = ( int8_t )( m_LastSendVdoInptFrmTimeStamp & 0xFF );
			m_TmpBytePt[ 2 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			m_TmpBytePt[ 3 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			m_TmpBytePt[ 4 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( ( ( m_Ntwk.m_UseWhatXfrPrtcl == 0 ) && ( m_Ntwk.m_TcpClntSokt.SendApkt( m_TmpBytePt, p_PktLenByt, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( m_Ntwk.m_UseWhatXfrPrtcl == 1 ) && ( m_Ntwk.m_AudpSokt.SendApkt( m_Ntwk.m_AudpCnctIdx, m_TmpBytePt, p_PktLenByt, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。" ), m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, m_TmpBytePt[ 13 ] & 0xff );
			}
			else
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有图像活动的视频输入帧包失败。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。原因：" ), m_LastSendVdoInptFrmTimeStamp, p_PktLenByt, m_TmpBytePt[13] & 0xff );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
		}
		else //如果本次视频输入帧为无图像活动，无需发送。
		{
			if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGI( Cu8vstr( "本次视频输入帧为无图像活动，无需发送。" ) );
		}
	}
}

//用户定义的写入音频输出帧函数。
void MyMediaPocsThrdCls::UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
											  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
											  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//取出并写入音频输出帧。
	{
		//从链表或音频自适应抖动缓冲器取出一个音频输出帧。
		switch( m_UseWhatRecvOtptFrm ) //使用什么接收音频输出帧。
		{
			case 0: //如果要使用链表。
			{
				m_RecvAdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
				if( m_TmpSz != 0 )
				{
					m_RecvAdoOtptFrmLnkLst.GetHead( NULL, m_TmpByte2Pt, m_TmpByte2Sz, NULL, &m_TmpSz, 1, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取并删除接收音频输出帧链表的第一个音频输出帧。
				}

				if( m_TmpSz != 0 ) //如果接收音频输出帧链表的第一个输出帧为有语音活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个有语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}
				else //如果接收音频输出帧链表为空，或第一个音频输出帧为无语音活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个无语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
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
				AAjbGetBufFrmCnt( m_AAjb.m_Pt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_AdoOtptFrmTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出音频输出帧。
				AAjbGetFrm( m_AAjb.m_Pt, &p_AdoOtptFrmTimeStamp, m_TmpByte2Pt, m_TmpByte2Sz, &m_TmpSz, 1, NULL );

				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
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
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出帧的长度不等于Pcm格式的长度。音频输出帧：%uzd，Pcm格式：%z32d。" ), m_TmpSz, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
				else
				{
					//写入音频输出Pcm格式原始帧。
					memcpy( AdoOtptPcmSrcFrmPt, m_TmpByte2Pt, AdoOtptPcmFrmLenUnit * sizeof( int16_t ) );
				}
			}
			else //如果要使用音频输出已编码格式原始帧。
			{
				if( m_TmpSz > AdoOtptEncdSrcFrmSzByt )
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "音频输出帧的长度已超过已编码格式的长度。音频输出帧：%uzd，已编码格式：%uzd。" ), m_TmpSz, AdoOtptEncdSrcFrmSzByt );
					*AdoOtptEncdSrcFrmLenBytPt = 0;
				}
				else
				{
					//写入音频输出已编码格式原始帧。
					*AdoOtptEncdSrcFrmLenBytPt = m_TmpSz;
					memcpy( AdoOtptEncdSrcFrmPt, m_TmpByte2Pt, m_TmpSz );
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
void MyMediaPocsThrdCls::UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
											int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
											uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt )
{
		
}

//用户定义的写入视频输出帧函数。
void MyMediaPocsThrdCls::UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
											  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
											  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//从链表或自适应抖动缓冲器取出一个视频输出帧。
	switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果要使用链表。
		{
			m_RecvVdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
			if( m_TmpSz != 0 ) //如果接收视频输出帧链表不为空。
			{
				m_RecvVdoOtptFrmLnkLst.GetHead( NULL, m_TmpByte3Pt, m_TmpByte3Sz, NULL, &m_TmpSz, 1, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取并删除接收视频输出帧链表的第一个视频输出帧。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个有图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个无图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
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
			VAjbGetBufFrmCnt( m_VAjb.m_Pt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );

			if( p_CurHaveBufFrmCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_VdoOtptFrmTimeStamp;

				//从视频自适应抖动缓冲器取出视频输出帧。
				VAjbGetFrm( m_VAjb.m_Pt, FuncGetTickAsMsec(), &p_VdoOtptFrmTimeStamp, m_TmpByte3Pt, m_TmpByte3Sz, &m_TmpSz, 1, NULL );

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
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
			*VdoOtptYu12SrcFrmWidthPt = ( m_TmpByte3Pt[0] & 0xFF ) + ( ( m_TmpByte3Pt[1] & 0xFF ) << 8 ) + ( ( m_TmpByte3Pt[2] & 0xFF ) << 16 ) + ( ( m_TmpByte3Pt[3] & 0xFF ) << 24 );
			//读取视频输出帧高度。
			*VdoOtptYu12SrcFrmHeightPt = ( m_TmpByte3Pt[4] & 0xFF ) + ( ( m_TmpByte3Pt[5] & 0xFF ) << 8 ) + ( ( m_TmpByte3Pt[6] & 0xFF ) << 16 ) + ( ( m_TmpByte3Pt[7] & 0xFF ) << 24 );

			if( m_TmpSz - 4 - 4 != *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 )
			{
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出帧的长度不等于Yu12格式的长度。视频输出帧：%uzd，Yu12格式：%z32d。" ), m_TmpSz - 4 - 4, *VdoOtptYu12SrcFrmWidthPt * *VdoOtptYu12SrcFrmHeightPt * 3 / 2 );
				*VdoOtptYu12SrcFrmWidthPt = 0;
				*VdoOtptYu12SrcFrmHeightPt = 0;
				return;
			}

			//写入视频输出Yu12格式原始帧。
			memcpy( VdoOtptYu12SrcFrmPt, m_TmpByte3Pt + 4 + 4, m_TmpSz - 4 - 4 );
		}
		else //如果要使用视频输出已编码格式帧。
		{
			if( m_TmpSz > VdoOtptEncdSrcFrmSzByt )
			{
				*VdoOtptEncdSrcFrmLenBytPt = 0;
				if( m_MediaPocsThrdPt->m_IsPrintLog != 0 ) LOGFE( Cu8vstr( "视频输出帧的长度已超过已编码格式的长度。视频输出帧：%uzd，已编码格式：%z32d。" ), m_TmpSz, VdoOtptEncdSrcFrmSzByt );
				return;
			}

			//写入视频输出已编码格式帧。
			memcpy( VdoOtptEncdSrcFrmPt, m_TmpByte3Pt, m_TmpSz );
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
void MyMediaPocsThrdCls::UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
											uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
											uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt )
{
	
}