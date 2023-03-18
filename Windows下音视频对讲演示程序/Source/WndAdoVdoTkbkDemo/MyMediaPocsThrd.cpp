#include "MyMediaPocsThrd.h"

//构造函数。
MyMediaPocsThrdCls::MyMediaPocsThrdCls( HWND MainDlgWndHdl )
{
	memset( &m_MainDlgWndHdl, 0, ( char * )&m_TmpByte3Sz + sizeof( m_TmpByte3Sz ) - ( char * )&m_MainDlgWndHdl );
	m_MainDlgWndHdl = MainDlgWndHdl; //设置主对话框窗口的句柄。
	m_IsInterrupt = 0; //设置未中断。

	m_LclTkbkMode = TkbkMode::None;
	m_RmtTkbkMode = TkbkMode::None;
}

//析构函数。
MyMediaPocsThrdCls::~MyMediaPocsThrdCls()
{
	VstrDstoy( m_IPAddrVstrPt );
	VstrDstoy( m_PortVstrPt );
	if( m_TmpBytePt != NULL ) free( m_TmpBytePt );
	if( m_TmpByte2Pt != NULL ) free( m_TmpByte2Pt );
	if( m_TmpByte3Pt != NULL ) free( m_TmpByte3Pt );
}

//用户定义的初始化函数。
int MyMediaPocsThrdCls::UserInit()
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	Vstr * p_LclNodeAddrVstrPt = NULL; //存放本地节点地址。
	Vstr * p_LclNodePortVstrPt = NULL; //存放本地节点端口。
	Vstr * p_RmtNodeAddrVstrPt = NULL; //存放远程节点地址。
	Vstr * p_RmtNodePortVstrPt = NULL; //存放远程节点端口。
	size_t p_TmpSz;

	PostMessage( m_MainDlgWndHdl, WndMsg::MediaPocsThrd_Init, 0, 0 ); //向主对话框发送初始化媒体处理线程的消息。

	m_RqstCnctRslt = 0; //设置请求连接的结果为没有选择。
	m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	if( m_TmpBytePt == NULL )
	{
		m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( m_TmpBytePt == NULL )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
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
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
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
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		m_TmpByte3Sz = 1024 * 1024; //设置临时数据的大小。
	}

	if( VstrInit( &p_LclNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ) );
		LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_LclNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ) );
		LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_RmtNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ) );
		LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( VstrInit( &p_RmtNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ) );
		LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	if( m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( m_TcpSrvrSokt.Init( 4, m_IPAddrVstrPt, m_PortVstrPt, 1, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议服务端套接字成功。
			{
				if( m_TcpSrvrSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), m_IPAddrVstrPt, m_PortVstrPt );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( m_TcpSrvrSokt.Acpt( &m_TcpClntSokt, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( m_TcpClntSokt.m_TcpClntSoktPt != NULL ) //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。

						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break;
					}
					else //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		else if( m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), m_IPAddrVstrPt );
			WinExec( ( char * )m_IPAddrVstrPt->m_StrPt, SW_HIDE );

			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端TCP协议服务端套接字。
			{
				//连接远端
				{
					{
						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}

					if( m_TcpClntSokt.Init( 4, m_IPAddrVstrPt, m_PortVstrPt, NULL, NULL, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字成功。
					{
						if( m_TcpClntSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto Out;
						}
						if( m_TcpClntSokt.GetRmtAddr( NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
							LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto Out;
						}

						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议客户端套接字[%vs:%vs]，并连接远端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break; //跳出重连。
					}
					else
					{
						VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), m_IPAddrVstrPt, m_PortVstrPt );
						LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > m_MaxCnctTimes )
				{
					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}

		if( m_TcpClntSokt.SetNoDelay( 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_TcpClntSokt.SetSendBufSz( 1024 * 1024, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的发送缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的发送缓冲区大小失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_TcpClntSokt.SetRecvBufSz( 1024 * 1024, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的接收缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的接收缓冲区大小失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
		
		if( m_TcpClntSokt.SetKeepAlive( 1, 1, 1, 5, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的保活机制失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的保活机制失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	}
	else //如果使用UDP协议。
	{
		if( m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端高级UDP协议套接字接受远端高级UDP协议套接字的连接。
		{
			if( m_AudpSokt.Init( 4, m_IPAddrVstrPt, m_PortVstrPt, 1, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
			{
				if( m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
				{
					VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else //如果初始化本端高级UDP协议套接字失败。
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]失败。原因：" ), m_IPAddrVstrPt, m_PortVstrPt );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			while( true ) //循环接受远端高级UDP协议套接字的连接。
			{
				if( m_AudpSokt.Acpt( &m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( m_AudpCnctIdx != SIZE_MAX ) //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接成功。
					{
						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						break;
					}
					else //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		else if( m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端高级UDP协议套接字连接远端高级UDP协议套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), m_IPAddrVstrPt );
			WinExec( ( char * )m_IPAddrVstrPt->m_StrPt, SW_HIDE );

			if( m_AudpSokt.Init( 4, NULL, NULL, 0, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
			{
				if( m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
				{
					VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
			else //如果初始化本端高级UDP协议套接字失败。
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			
			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端高级UDP协议服务端套接字。
			{
				//连接远端。
				{
					{
						VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}

					if( m_AudpSokt.Cnct( 4, m_IPAddrVstrPt, m_PortVstrPt, &m_AudpCnctIdx, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果连接远端高级UDP协议套接字成功。
					{
						AudpCnctSts p_AudpCnctSts;

						if( m_AudpSokt.WaitCnct( m_AudpCnctIdx, UINT16_MAX, &p_AudpCnctSts, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端高级UDP协议套接字连接远端是否成功成功。
						{
							if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
							{
								if( m_AudpSokt.GetRmtAddr( m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议客户端套接字连接的远端高级UDP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
									LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
									goto Out;
								}

								VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
								LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								break; //跳出重连。
							}
							else //如果连接失败。
							{
								if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
								{
									VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接超时。" ), m_IPAddrVstrPt, m_PortVstrPt );
									LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								}
								else //如果连接断开。
								{
									VstrFmtCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接断开。" ), m_IPAddrVstrPt, m_PortVstrPt );
									LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
									{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								}
							}
						}

						m_AudpSokt.ClosCnct( m_AudpCnctIdx, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //关闭连接，等待重连。
					}
					else
					{
						VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：" ), m_IPAddrVstrPt, m_PortVstrPt );
						LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > m_MaxCnctTimes )
				{
					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}

				if( m_MediaPocsThrdPt->m_ReadyExitCnt != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		
		if( m_AudpSokt.SetSendBufSz( 1024 * 1024, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}

		if( m_AudpSokt.SetRecvBufSz( 1024 * 1024, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
		{
			VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	} //协议连接结束。

	//等待允许连接。
	if( ( m_IsCreateSrvrOrClnt == 1 ) && ( m_IsAutoAllowCnct != 0 ) ) m_RqstCnctRslt = 1;
	else m_RqstCnctRslt = 0;
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_RmtNodeAddrVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgInit, ( WPARAM )p_ErrInfoVstrPt, 0 ); } //向主窗口发送显示请求连接对话框的消息。
	while( true )
	{
		if( m_IsCreateSrvrOrClnt == 1 ) //如果是服务端。
		{
			if( m_RqstCnctRslt == 1 ) //如果允许连接。
			{
				m_TmpBytePt[0] = PKT_TYP_ALLOW_CNCT; //设置允许连接包。
				if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个允许连接包成功。" ) );
					LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto WaitAllowCnct;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个允许连接包失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
			else if( m_RqstCnctRslt == 2 ) //如果拒绝连接。
			{
				m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
		}
		else //如果是客户端。
		{
			if( m_RqstCnctRslt == 2 ) //如果中断等待。
			{
				m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
			}
		}
		
		//接收一个远端发送的数据包。
		if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 0, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
			( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.RecvApkt( m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
		{
			if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
			{
				if( ( p_TmpSz == 1 ) && ( m_TmpBytePt[0] == PKT_TYP_ALLOW_CNCT ) ) //如果是允许连接包。
				{
					if( m_IsCreateSrvrOrClnt == 0 ) //如果是客户端。
					{
						m_RqstCnctRslt = 1;
						
						PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

						VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个允许连接包。" ) );
						LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto WaitAllowCnct;
					}
					else //如果是服务端。
					{
						//就重新接收。
					}
				}
				else if( ( p_TmpSz == 1 ) && ( m_TmpBytePt[0] == PKT_TYP_REFUSE_CNCT ) ) //如果是拒绝连接包。
				{
					m_RqstCnctRslt = 2;
					
					PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个拒绝连接包。" ) );
					LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
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
			PostMessage( m_MainDlgWndHdl, WndMsg::RqstCnctDlgDstoy, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

			VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
			LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto Out;
		}
	} //等待允许连接结束。
	WaitAllowCnct:;

	m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
	m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
		
	switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果使用链表。
		{
			//初始化接收音频输出帧链表。
			if( m_RecvAdoOtptFrmLnkLst.Init( BufAutoAdjMethFreeNumber, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, 0, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化接收音频输出帧链表成功。" ) );
			}
			else
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收音频输出帧链表失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			//初始化接收视频输出帧链表。
			if( m_RecvVdoOtptFrmLnkLst.Init( BufAutoAdjMethFreeNumber, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, 0, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenByt, SIZE_MAX, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化接收视频输出帧链表成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收视频输出帧链表失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			break;
		}
		case 1: //如果使用自适应抖动缓冲器。
		{
			//初始化音频自适应抖动缓冲器。
			if( AAjbInit( &m_AAjbPt, m_MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, m_MediaPocsThrdPt->m_AdoOtpt.m_FrmLenUnit, 1, 1, 0, m_AAjbMinNeedBufFrmCnt, m_AAjbMaxNeedBufFrmCnt, m_AAjbMaxCntuLostFrmCnt, m_AAjbAdaptSensitivity, ( m_XfrMode == 0 ) ? 0 : 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化音频自适应抖动缓冲器成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化音频自适应抖动缓冲器失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}

			//初始化视频自适应抖动缓冲器。
			if( VAjbInit( &m_VAjbPt, 1, m_VAjbMinNeedBufFrmCnt, m_VAjbMaxNeedBufFrmCnt, m_VAjbAdaptSensitivity, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化视频自适应抖动缓冲器成功。" ) );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化视频自适应抖动缓冲器失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto Out;
			}
			break;
		}
	}

	VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始对讲。" ) );
	LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnInit, 0, 0 ); //向主窗口发送初始化一键即按即通按钮的消息。
	{ UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new UserMsgLclTkbkMode(); p_UserMsgLclTkbkModePt->m_LclTkbkMode = TkbkMode::NoChg; SendUserMsg( p_UserMsgLclTkbkModePt, NULL ); } //发送对讲模式包。

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
	size_t p_TmpSz;
	uint32_t p_TmpUint32;
	size_t p_TmpLnkLstElmTotal;

	//接收远端发送过来的一个数据包。
	if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 0, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
		( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.RecvApkt( m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
	{
		if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
		{
			if( p_TmpSz == 0 ) //如果数据包的长度为0。
			{
				LOGFE( Cu8vstr( "接收到一个数据包的数据长度为%uzd，表示没有数据，无法继续接收。" ), p_TmpSz );
				goto Out;
			}
			else if( m_TmpBytePt[ 0 ] == PKT_TYP_TKBK_MODE ) //如果是对讲模式包。
			{
				if( p_TmpSz < 1 + 1 ) //如果音频输出帧包的数据长度小于1 + 1，表示没有对讲模式。
				{
					LOGFE( Cu8vstr( "接收到一个对讲模式包的数据长度为%uzd小于1 + 1，表示没有对讲模式，无法继续接收。" ), p_TmpSz );
					goto Out;
				}
				if( m_TmpBytePt[ 1 ] >= TkbkMode::NoChg )
				{
					LOGFE( Cu8vstr( "接收到一个对讲模式包的对讲模式为%z8d不正确，无法继续接收。" ), m_TmpBytePt[ 1 ] );
					goto Out;
				}

				m_RmtTkbkMode = ( TkbkMode )m_TmpBytePt[ 1 ]; //设置远端对讲模式。
				LOGFI( Cu8vstr( "接收到一个对讲模式包。对讲模式：%d。" ), m_RmtTkbkMode );
				SetTkbkMode(); //设置对讲模式。
			}
			else if( m_TmpBytePt[0] == PKT_TYP_ADO_FRM ) //如果是音频输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果音频输出帧包的长度小于1 + 4，表示没有音频输出帧时间戳。
				{
					LOGFE( Cu8vstr( "接收到一个音频输出帧包的数据长度为%uzd小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。" ), p_TmpSz );
					goto Out;
				}

				//读取音频输出帧时间戳。
				p_TmpUint32 = ( m_TmpBytePt[1] & 0xFF ) + ( ( m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( m_TmpBytePt[4] & 0xFF ) << 24 );

				if( ( m_MediaPocsThrdPt->m_AdoOtpt.m_IsInitAdoOtpt != 0 ) || //如果已初始化音频输出。
					( ( m_XfrMode == 0 ) && ( ( m_LclTkbkMode == TkbkMode::Ado ) || ( m_LclTkbkMode == TkbkMode::AdoVdo ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为音频或音视频。
				{
					//将输出帧放入链表或自适应抖动缓冲器。
					switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
					{
						case 0: //如果使用链表。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								m_RecvAdoOtptFrmLnkLst.GetTotal( &p_TmpLnkLstElmTotal, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的元素总数。
								if( p_TmpLnkLstElmTotal <= 50 )
								{
									if( m_RecvAdoOtptFrmLnkLst.PutTail( m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
									{
										LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
									}
									else
									{
										LOGFE( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表失败。原因：%vs" ), m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto Out;
									}
								}
								else
								{
									LOGFE( Cu8vstr( "接收到一个有语音活动的音频输出帧包，但接收音频输出帧链表中帧总数为%uzd已经超过上限50，不再放入接收音频输出帧链表。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpLnkLstElmTotal, p_TmpUint32, p_TmpSz );
								}
							}
							else //如果该音频输出帧为无语音活动。
							{
								LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，无需放入接收音频输出帧链表。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}
							break;
						}
						case 1: //如果使用自适应抖动缓冲器。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								AAjbPutFrm( m_AAjbPt, p_TmpUint32, m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
								LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}
							else //如果该音频输出帧为无语音活动。
							{
								AAjbPutFrm( m_AAjbPt, p_TmpUint32, m_TmpBytePt + 1 + 4, 0, 1, NULL );
								LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}

							int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
							int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							AAjbGetBufFrmCnt( m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

							break;
						}
					}
				}
				else //如果未初始化音频输出。
				{
					if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
					{
						LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
					}
					else //如果该音频输出帧为无语音活动。
					{
						LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包成功，但未初始化音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
					}
				}
			}
			else if( m_TmpBytePt[0] == PKT_TYP_VDO_FRM ) //如果是视频输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果视频输出帧包的长度小于1 + 4，表示没有视频输出帧时间戳。
				{
					LOGFE( Cu8vstr( "接收到一个视频输出帧包的长度为%uzd小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。" ), p_TmpSz );
					goto Out;
				}

				//读取视频输出帧时间戳。
				p_TmpUint32 = ( m_TmpBytePt[1] & 0xFF ) + ( ( m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( m_TmpBytePt[4] & 0xFF ) << 24 );

				if( ( m_MediaPocsThrdPt->m_VdoOtpt.m_IsInitVdoOtpt != 0 ) || //如果已初始化视频输出。
					( ( m_XfrMode == 0 ) && ( ( m_LclTkbkMode == TkbkMode::Vdo ) || ( m_LclTkbkMode == TkbkMode::AdoVdo ) ) ) ) //如果传输模式为实时半双工（一键通），且本端对讲模式为视频或音视频。
				{
					//将视频输出帧放入链表或自适应抖动缓冲器。
					switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
					{
						case 0: //如果使用链表。
						{
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								m_RecvVdoOtptFrmLnkLst.GetTotal( &p_TmpLnkLstElmTotal, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的元素总数。
								if( p_TmpLnkLstElmTotal <= 20 )
								{
									if( m_RecvVdoOtptFrmLnkLst.PutTail( m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
									{
										LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
									}
									else
									{
										LOGFE( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表失败。原因：%vs" ), m_MediaPocsThrdPt->m_ErrInfoVstrPt );
										goto Out;
									}
								}
								else
								{
									LOGFE( Cu8vstr( "接收到一个有图像活动的视频输出帧包，但接收视频输出帧链表中帧总数为%uzd已经超过上限20，不再放入接收视频输出帧链表。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpLnkLstElmTotal, p_TmpUint32, p_TmpSz );
								}
							}
							else //如果该视频输出帧为无图像活动。
							{
								LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入接收视频输出帧链表。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}
							break;
						}
						case 1: //如果使用自适应抖动缓冲器。
						{
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								VAjbPutFrm( m_VAjbPt, FuncGetTickAsMsec(), p_TmpUint32, m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
								LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}
							else //如果该视频输出帧为无图像活动。
							{
								LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
							}

							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							VAjbGetBufFrmCnt( m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );
							break;
						}
					}
				}
				else //如果未初始化视频输出。
				{
					if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
					{
						LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
					}
					else //如果该视频输出帧为无图像活动。
					{
						LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包成功，但未初始化视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
					}
				}
			}
			else if( m_TmpBytePt[0] == PKT_TYP_EXIT ) //如果是退出包。
			{
				if( p_TmpSz > 1 ) //如果退出包的长度大于1。
				{
					LOGFE( Cu8vstr( "接收到退出包的长度为%uzd大于1，表示还有其他数据，无法继续接收。" ), p_TmpSz );
					goto Out;
				}

				m_IsRecvExitPkt = 1; //设置已经接收到退出包。
				RqirExit( 1, 0, NULL ); //请求退出。

				VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
				LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
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
		LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	return p_Rslt;
}

//用户定义的销毁函数。
void MyMediaPocsThrdCls::UserDstoy()
{
	size_t p_TmpSz;

	if( ( ( m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( m_AudpSokt.m_AudpSoktPt != NULL ) && ( m_AudpSokt.GetRmtAddr( m_AudpCnctIdx, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
	{
		{
			//发送退出包。
			m_TmpBytePt[0] = PKT_TYP_EXIT; //设置退出包。
			if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, 1, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) ||
				( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, 1, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) )
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个退出包失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto SendExitPkt;
			}

			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个退出包成功。" ) );
			LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			//接收退出包。
			if( m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
			{
				while( true ) //循环接收退出包。
				{
					if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.RecvApkt( m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 5000, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
						( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.RecvApkt( m_AudpCnctIdx, m_TmpBytePt, m_TmpByteSz, &p_TmpSz, 5000, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
					{
						if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
						{
							if( ( p_TmpSz == 1 ) && ( m_TmpBytePt[0] == PKT_TYP_EXIT ) ) //如果是退出包。
							{
								VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
								LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
								{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
								goto SendExitPkt;
							} //如果是其他包，就继续接收。
						}
						else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
						{
							VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
							LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
							{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
							goto SendExitPkt;
						}
					}
					else //用本端套接字接收一个连接的远端套接字发送的数据包失败。
					{
						VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
						LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
						{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
						goto SendExitPkt;
					}
				}
			}
		}
		SendExitPkt:;

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "中断对讲。" ) );
		LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		if( m_MediaPocsThrdPt->m_IsShowToast != 0 ) Toast( NULL, 3000, NULL, m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//销毁本端TCP协议服务端套接字。
	if( m_TcpSrvrSokt.m_TcpSrvrSoktPt != NULL )
	{
		m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议服务端套接字成功。" ) );
		LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁本端TCP协议客户端套接字。
	if( m_TcpClntSokt.m_TcpClntSoktPt != NULL )
	{
		m_TcpClntSokt.Dstoy( ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议客户端套接字成功。" ) );
		LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁本端高级UDP协议套接字。
	if( m_AudpSokt.m_AudpSoktPt != NULL )
	{
		m_AudpSokt.Dstoy( NULL ); //关闭并销毁已创建的本端高级UDP协议套接字。

		VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁本端高级UDP协议套接字成功。" ) );
		LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}

	//销毁接收音频输出帧的链表。
	if( m_RecvAdoOtptFrmLnkLst.m_VarLenLnkLstPt != NULL )
	{
		m_RecvAdoOtptFrmLnkLst.Dstoy( NULL );

		LOGI( Cu8vstr( "销毁接收音频输出帧的链表成功。" ) );
	}

	//销毁接收视频输出帧的链表。
	if( m_RecvVdoOtptFrmLnkLst.m_VarLenLnkLstPt != NULL )
	{
		m_RecvVdoOtptFrmLnkLst.Dstoy( NULL );

		LOGI( Cu8vstr( "销毁接收视频输出帧的链表成功。" ) );
	}

	//销毁音频自适应抖动缓冲器。
	if( m_AAjbPt != NULL )
	{
		AAjbDstoy( m_AAjbPt, NULL );
		m_AAjbPt = NULL;

		LOGI( Cu8vstr( "销毁音频自适应抖动缓冲器成功。" ) );
	}
	
	//销毁视频自适应抖动缓冲器。
	if( m_VAjbPt != NULL )
	{
		VAjbDstoy( m_VAjbPt, NULL );
		m_VAjbPt = NULL;

		LOGI( Cu8vstr( "销毁视频自适应抖动缓冲器成功。" ) );
	}

	if( m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
	{
		if( m_IsRecvExitPkt == 1 )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且接收到了退出包，表示是远端套接字主动退出，本线程重新初始化来继续保持监听。" ) );
			LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
		else if( ( m_IsInterrupt == 0 ) && ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeUserInit ) && ( m_RqstCnctRslt == 2 ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且未中断，且退出码为调用用户定义的初始化函数失败，且请求连接的结果为拒绝，表示是拒绝本次连接，本线程重新初始化来继续保持监听。" ) );
			LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
		else if( ( m_IsInterrupt == 0 ) && ( ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeMediaMsgPocs ) || ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs ) ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且未中断，且退出码为媒体消息处理失败或音视频输入输出处理失败，表示是媒体消息处理失败或连接异常断开，本线程重新初始化来继续保持监听。" ) );
			LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
		else //其他情况，本线程直接退出。
		{
			PostMessage( m_MainDlgWndHdl, WndMsg::MediaPocsThrd_Dstoy, 0, 0 ); //向主对话框发送销毁媒体处理线程的消息。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
	}
	else if( m_IsCreateSrvrOrClnt == 0 ) //如果是创建客户端。
	{
		if( ( m_IsInterrupt == 0 ) && ( ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeMediaMsgPocs ) || ( m_MediaPocsThrdPt->m_ExitCode == MediaPocsThrd::ExitCodeAdoVdoInptOtptPocs ) ) )
		{
			VstrCpy( m_MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建客户端，且未中断，且退出码为媒体消息处理失败或音视频输入输出处理失败，表示是媒体消息处理失败或连接异常断开，本线程重新初始化来重连服务端。" ) );
			LOGI( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }

			RqirExit( 2, 0, NULL ); //请求重启。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
		else //其他情况，本线程直接退出。
		{
			PostMessage( m_MainDlgWndHdl, WndMsg::MediaPocsThrd_Dstoy, 0, 0 ); //向主对话框发送销毁媒体处理线程的消息。
			if( m_XfrMode == 0 ) PostMessage( m_MainDlgWndHdl, WndMsg::PttBtnDstoy, 0, 0 ); //向主界面发送销毁一键即按即通按钮的消息。
		}
	}
}

//设置对讲模式。
void MyMediaPocsThrdCls::SetTkbkMode()
{
	if( m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		if( m_PttBtnIsDown == 0 ) //如果一键即按即通按钮为弹起。
		{
			switch( m_LclTkbkMode )
			{
				case None: //如果本端对讲模式为空。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case Ado: //如果本端对讲模式为音频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case Vdo: //如果本端对讲模式为视频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 1, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case AdoVdo: //如果本端对讲模式为音视频。
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
				case None: //如果本端对讲模式为空。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case Ado: //如果本端对讲模式为音频。
				{
					SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case Vdo: //如果本端对讲模式为视频。
				{
					SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
					break;
				}
				case AdoVdo: //如果本端对讲模式为音视频。
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
			case None: //如果本端对讲模式为空。
			{
				SetIsUseAdoVdoInptOtpt( 0, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
				break;
			}
			case Ado: //如果本端对讲模式为音频。
			{
				switch( m_RmtTkbkMode )
				{
					case None: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Ado: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Vdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case AdoVdo: //如果远端对讲模式为音视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 0, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
				}
				break;
			}
			case Vdo: //如果本端对讲模式为视频。
			{
				switch( m_RmtTkbkMode )
				{
					case None: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Ado: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Vdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case AdoVdo: //如果远端对讲模式为音视频。
					{
						SetIsUseAdoVdoInptOtpt( 0, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
				}
				break;
			}
			case AdoVdo: //如果本端对讲模式为音视频。
			{
				switch( m_RmtTkbkMode )
				{
					case None: //如果远端对讲模式为空。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Ado: //如果远端对讲模式为音频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 1, 1, 0, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case Vdo: //如果远端对讲模式为视频。
					{
						SetIsUseAdoVdoInptOtpt( 1, 0, 1, 1, NULL ); //设置是否使用音视频输入输出。
						break;
					}
					case AdoVdo: //如果远端对讲模式为音视频。
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

	switch( ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_MsgTyp )
	{
		case UserMsg::LclTkbkMode:
		{
			if( ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_LclTkbkMode != TkbkMode::NoChg ) m_LclTkbkMode = ( ( UserMsgLclTkbkMode * )MsgArgPt )->m_LclTkbkMode; //设置本端对讲模式。
			SetTkbkMode(); //设置对讲模式。
			if( ( ( m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( m_AudpSokt.m_AudpSoktPt != NULL ) && ( m_AudpSokt.GetRmtAddr( m_AudpCnctIdx, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
			{
				//发送对讲模式包。
				m_TmpBytePt[0] = PKT_TYP_TKBK_MODE; //设置对讲模式包。
				m_TmpBytePt[1] = m_LclTkbkMode; //设置对讲模式。
				if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, 2, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) ||
					( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, 2, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) )
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个对讲模式包失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
					goto Out;
				}
				else
				{
					LOGFI( Cu8vstr( "发送一个对讲模式包成功。对讲模式：%d。" ), m_LclTkbkMode );
				}
			}
			break;
		}
		case UserMsg::RmtTkbkMode:
		{
			if( ( ( UserMsgRmtTkbkMode * )MsgArgPt )->m_RmtTkbkMode != TkbkMode::NoChg ) m_RmtTkbkMode = ( ( UserMsgRmtTkbkMode * )MsgArgPt )->m_RmtTkbkMode; //设置本端对讲模式。
			SetTkbkMode(); //设置对讲模式。
			break;
		}
		case UserMsg::PttBtnDown:
		{
			m_PttBtnIsDown = 1; //设置一键即按即通按钮为按下。
			SetTkbkMode(); //设置对讲模式。
			break;
		}
		case UserMsg::PttBtnUp:
		{
			m_PttBtnIsDown = 0; //设置一键即按即通按钮为弹起。
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
void MyMediaPocsThrdCls::UserReadAdoVdoInptFrm( int16_t * PcmAdoInptSrcFrmPt, int16_t * PcmAdoInptRsltFrmPt, int32_t PcmAdoInptFrmLenUnit, int32_t PcmAdoInptRsltFrmVoiceActSts,
												uint8_t * EncdAdoInptRsltFrmPt, size_t EncdAdoInptRsltFrmLenByt, int32_t EncdAdoInptRsltFrmIsNeedTrans,
												uint8_t * BgraVdoInptSrcFrmPt, int32_t BgraVdoInptSrcFrmWidth, int32_t BgraVdoInptSrcFrmHeight, size_t BgraVdoInptSrcFrmLenByt,
												uint8_t * YU12VdoInptRsltFrmPt, int32_t YU12VdoInptRsltFrmWidth, int32_t YU12VdoInptRsltFrmHeight, size_t YU12VdoInptRsltFrmLenByt,
												uint8_t * EncdVdoInptRsltFrmPt, size_t EncdVdoInptRsltFrmLenByt )
{
	int p_FrmPktLen = 0; //存放帧数据包的长度，单位为字节。

	//发送音频输入帧。
	if( PcmAdoInptSrcFrmPt != NULL ) //如果有Pcm格式音频输入原始帧。
	{
		if( EncdAdoInptRsltFrmPt == NULL ) //如果没有已编码格式音频输入结果帧。
		{
			if( PcmAdoInptRsltFrmVoiceActSts != 0 ) //如果Pcm格式音频输入结果帧为有语音活动。
			{
				memcpy( m_TmpBytePt + 1 + 4, PcmAdoInptRsltFrmPt, m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt ); //设置音频输入帧。
				p_FrmPktLen = 1 + 4 + m_MediaPocsThrdPt->m_AdoInpt.m_FrmLenByt; //帧数据包长度 = 数据包类型 + 音频输入帧时间戳 + Pcm格式音频输入帧。
			}
			else //如果本次音频输入帧为无语音活动，或不需要传输。
			{
				p_FrmPktLen = 1 + 4; //帧数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}
		else //如果有已编码格式音频输入结果帧。
		{
			if( PcmAdoInptRsltFrmVoiceActSts != 0 && EncdAdoInptRsltFrmIsNeedTrans != 0 ) //如果Pcm格式音频输入结果帧为有语音活动，且已编码格式音频输入结果帧需要传输。
			{
				memcpy( m_TmpBytePt + 1 + 4, EncdAdoInptRsltFrmPt, EncdAdoInptRsltFrmLenByt ); //设置音频输入帧。
				p_FrmPktLen = 1 + 4 + EncdAdoInptRsltFrmLenByt; //帧数据包长度 = 数据包类型 + 音频输入帧时间戳 + 已编码格式音频输入帧。
			}
			else //如果本次音频输入帧为无语音活动，或不需要传输。
			{
				p_FrmPktLen = 1 + 4; //帧数据包长度 = 数据包类型 + 音频输入帧时间戳。
			}
		}

		if( p_FrmPktLen != 1 + 4 ) //如果本次音频输入帧为有语音活动，就发送。
		{
			m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
			m_TmpBytePt[0] = PKT_TYP_ADO_FRM;
			//设置音频输入帧时间戳。
			m_TmpBytePt[1] = ( int8_t ) ( m_LastSendAdoInptFrmTimeStamp & 0xFF );
			m_TmpBytePt[2] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			m_TmpBytePt[3] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			m_TmpBytePt[4] = ( int8_t ) ( ( m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, p_FrmPktLen, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, p_FrmPktLen, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				LOGFI( Cu8vstr( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), m_LastSendAdoInptFrmTimeStamp, p_FrmPktLen );
			}
			else
			{
				VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有语音活动的音频输入帧包失败。原因：" ) );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}

			m_LastSendAdoInptFrmIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
		}
		else //如果本次音频输入帧为无语音活动。
		{
			if( m_LastSendAdoInptFrmIsAct != 0 ) //如果最后一个发送的音频输入帧为有语音活动，就发送。
			{
				m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

				//设置数据包类型为音频输入帧包。
				m_TmpBytePt[ 0 ] = PKT_TYP_ADO_FRM;
				//设置音频输入帧时间戳。
				m_TmpBytePt[ 1 ] = ( int8_t )( m_LastSendAdoInptFrmTimeStamp & 0xFF );
				m_TmpBytePt[ 2 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
				m_TmpBytePt[ 3 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
				m_TmpBytePt[ 4 ] = ( int8_t )( ( m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

				if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, p_FrmPktLen, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, p_FrmPktLen, 10, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					LOGFI( Cu8vstr( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), m_LastSendAdoInptFrmTimeStamp, p_FrmPktLen );
				}
				else
				{
					VstrIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个无语音活动的音频输入帧包失败。原因：" ) );
					LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
					{
						Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 );
					}
				}

				m_LastSendAdoInptFrmIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
			}
			else //如果最后一个发送的音频输入帧为无语音活动，无需发送。
			{
				LOGI( Cu8vstr( "本次音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" ) );
			}
		}
	}

	//发送视频输入帧。
	if( YU12VdoInptRsltFrmPt != NULL ) //如果有YU12格式视频输入结果帧。
	{
		if( EncdVdoInptRsltFrmPt == NULL ) //如果没有已编码格式视频输入结果帧。
		{
			//设置视频输入帧宽度。
			m_TmpBytePt[5] = ( int8_t ) ( YU12VdoInptRsltFrmWidth & 0xFF );
			m_TmpBytePt[6] = ( int8_t ) ( ( YU12VdoInptRsltFrmWidth & 0xFF00 ) >> 8 );
			m_TmpBytePt[7] = ( int8_t ) ( ( YU12VdoInptRsltFrmWidth & 0xFF0000 ) >> 16 );
			m_TmpBytePt[8] = ( int8_t ) ( ( YU12VdoInptRsltFrmWidth & 0xFF000000 ) >> 24 );
			//设置视频输入帧高度。
			m_TmpBytePt[9] = ( int8_t ) ( YU12VdoInptRsltFrmHeight & 0xFF );
			m_TmpBytePt[10] = ( int8_t ) ( ( YU12VdoInptRsltFrmHeight & 0xFF00 ) >> 8 );
			m_TmpBytePt[11] = ( int8_t ) ( ( YU12VdoInptRsltFrmHeight & 0xFF0000 ) >> 16 );
			m_TmpBytePt[12] = ( int8_t ) ( ( YU12VdoInptRsltFrmHeight & 0xFF000000 ) >> 24 );

			memcpy( m_TmpBytePt + 1 + 4 + 4 + 4, YU12VdoInptRsltFrmPt, YU12VdoInptRsltFrmWidth * YU12VdoInptRsltFrmHeight * 3 / 2 ); //设置视频输入帧。
			p_FrmPktLen = 1 + 4 + 4 + 4 + YU12VdoInptRsltFrmWidth * YU12VdoInptRsltFrmHeight * 3 / 2; //帧数据包长度 = 数据包类型 + 视频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + YU12格式视频输入结果帧。
		}
		else //如果有已编码格式视频输入结果帧。
		{
			if( EncdVdoInptRsltFrmLenByt != 0 ) //如果本次已编码格式视频输入结果帧为有图像活动。
			{
				memcpy( m_TmpBytePt + 1 + 4, EncdVdoInptRsltFrmPt, EncdVdoInptRsltFrmLenByt ); //设置视频输入帧。
				p_FrmPktLen = 1 + 4 + EncdVdoInptRsltFrmLenByt; //帧数据包长度 = 数据包类型 + 视频输入帧时间戳 + 已编码格式视频输入结果帧。
			}
			else
			{
				p_FrmPktLen = 1 + 4; //帧数据包长度 = 数据包类型 + 视频输入帧时间戳。
			}
		}

		if( p_FrmPktLen != 1 + 4 ) //如果本次视频输入帧为有图像活动，就发送。
		{
			m_LastSendVdoInptFrmTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

			//设置数据包类型为视频输入帧包。
			m_TmpBytePt[ 0 ] = PKT_TYP_VDO_FRM;
			//设置视频输入帧时间戳。
			m_TmpBytePt[ 1 ] = ( int8_t )( m_LastSendVdoInptFrmTimeStamp & 0xFF );
			m_TmpBytePt[ 2 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
			m_TmpBytePt[ 3 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
			m_TmpBytePt[ 4 ] = ( int8_t )( ( m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

			if( ( ( m_UseWhatXfrPrtcl == 0 ) && ( m_TcpClntSokt.SendApkt( m_TmpBytePt, p_FrmPktLen, 0, 1, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
				( ( m_UseWhatXfrPrtcl == 1 ) && ( m_AudpSokt.SendApkt( m_AudpCnctIdx, m_TmpBytePt, p_FrmPktLen, 1, m_MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
			{
				LOGFI( Cu8vstr( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。" ), m_LastSendVdoInptFrmTimeStamp, p_FrmPktLen, m_TmpBytePt[ 13 ] & 0xff );
			}
			else
			{
				VstrFmtIns( m_MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有图像活动的视频输入帧包失败。视频输入帧时间戳：%uz32d，总长度：%d，类型：%d。原因：" ), m_LastSendVdoInptFrmTimeStamp, p_FrmPktLen, m_TmpBytePt[13] & 0xff );
				LOGE( m_MediaPocsThrdPt->m_ErrInfoVstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , m_MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( m_MainDlgWndHdl, WndMsg::ShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			}
		}
		else //如果本次视频输入帧为无图像活动，无需发送。
		{
			LOGI( Cu8vstr( "本次视频输入帧为无图像活动，无需发送。" ) );
		}
	}
}

//用户定义的写入音频输出帧函数。
void MyMediaPocsThrdCls::UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
											  int16_t * PcmAdoOtptSrcFrmPt, int32_t PcmAdoOtptFrmLenUnit,
											  uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmSzByt, size_t * EncdAdoOtptSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//取出并写入音频输出帧。
	{
		//从链表或音频自适应抖动缓冲器取出一个音频输出帧。
		switch( m_UseWhatRecvOtptFrm ) //使用什么接收音频输出帧。
		{
			case 0: //如果使用链表。
			{
				m_RecvAdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
				if( m_TmpSz != 0 )
				{
					m_RecvAdoOtptFrmLnkLst.Locked( NULL ); //接收音频输出帧链表的互斥锁加锁。
					m_RecvAdoOtptFrmLnkLst.GetHead( NULL, m_TmpByte2Pt, m_TmpByte2Sz, NULL, &m_TmpSz, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的第一个音频输出帧。
					m_RecvAdoOtptFrmLnkLst.DelHead( 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //删除接收音频输出帧链表的第一个音频输出帧。
					m_RecvAdoOtptFrmLnkLst.Unlock( NULL ); //接收音频输出帧链表的互斥锁解锁。
				}

				if( m_TmpSz != 0 ) //如果接收音频输出帧链表的第一个输出帧为有语音活动。
				{
					LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个有语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}
				else //如果接收音频输出帧链表为空，或第一个音频输出帧为无语音活动。
				{
					LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个无语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}

				break;
			}
			case 1: //如果使用自适应抖动缓冲器。
			{
				int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
				int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
				int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
				int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
				int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
				int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
				int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
				AAjbGetBufFrmCnt( m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
				LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_AdoOtptFrmTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出音频输出帧。
				AAjbGetFrm( m_AAjbPt, &p_AdoOtptFrmTimeStamp, m_TmpByte2Pt, m_TmpByte2Sz, &m_TmpSz, 1, NULL );

				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				break;
			}
		}

		//写入音频输出帧。
		if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
		{
			if( PcmAdoOtptSrcFrmPt != NULL ) //如果有Pcm格式音频输出原始帧。
			{
				if( m_TmpSz != PcmAdoOtptFrmLenUnit * sizeof( int16_t ) )
				{
					memset( PcmAdoOtptSrcFrmPt, 0, PcmAdoOtptFrmLenUnit * sizeof( int16_t ) );
					LOGFE( Cu8vstr( "音频输出帧的长度不等于Pcm格式的长度。音频输出帧：%uzd，Pcm格式：%z32d。" ), m_TmpSz, PcmAdoOtptFrmLenUnit * sizeof( int16_t ) );
				}
				else
				{
					//写入Pcm格式音频输出帧。
					memcpy( PcmAdoOtptSrcFrmPt, m_TmpByte2Pt, PcmAdoOtptFrmLenUnit * sizeof( int16_t ) );
				}
			}
			else //如果没有Pcm格式音频输出原始帧。
			{
				if( m_TmpSz > EncdAdoOtptSrcFrmSzByt )
				{
					LOGFE( Cu8vstr( "音频输出帧的长度已超过已编码格式的长度。音频输出帧：%uzd，已编码格式：%uzd。" ), m_TmpSz, EncdAdoOtptSrcFrmSzByt );
					*EncdAdoOtptSrcFrmLenBytPt = 0;
				}
				else
				{
					//写入已编码格式音频输出帧。
					*EncdAdoOtptSrcFrmLenBytPt = m_TmpSz;
					memcpy( EncdAdoOtptSrcFrmPt, m_TmpByte2Pt, m_TmpSz );
				}
			}
		}
		else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
		{
			if( PcmAdoOtptSrcFrmPt != NULL ) //如果要使用Pcm格式音频输出帧。
			{
				memset( PcmAdoOtptSrcFrmPt, 0, PcmAdoOtptFrmLenUnit * sizeof( int16_t ) );
			}
			else //如果要使用已编码格式音频输出帧。
			{
				*EncdAdoOtptSrcFrmLenBytPt = 0;
			}
		}
		else //如果音频输出帧为丢失。
		{
			if( PcmAdoOtptSrcFrmPt != NULL ) //如果要使用Pcm格式音频输出帧。
			{
				memset( PcmAdoOtptSrcFrmPt, 0, PcmAdoOtptFrmLenUnit * sizeof( int16_t ) );
			}
			else //如果要使用已编码格式音频输出帧。
			{
				*EncdAdoOtptSrcFrmLenBytPt = m_TmpSz;
			}
		}
	}
}

//用户定义的获取音频输出帧函数。
void MyMediaPocsThrdCls::UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
											int16_t * PcmAdoOtptSrcFrmPt, int32_t PcmAdoOtptFrmLenUnit,
											uint8_t * EncdAdoOtptSrcFrmPt, size_t EncdAdoOtptSrcFrmLenByt )
{
		
}

//用户定义的写入视频输出帧函数。
void MyMediaPocsThrdCls::UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
											  uint8_t * YU12VdoOtptSrcFrmPt, int32_t * YU12VdoOtptSrcFrmWidthPt, int32_t * YU12VdoOtptSrcFrmHeightPt,
											  uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmSzByt, size_t * EncdVdoOtptSrcFrmLenBytPt )
{
	size_t m_TmpSz = 0;

	//从链表或自适应抖动缓冲器取出一个视频输出帧。
	switch( m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
		case 0: //如果使用链表。
		{
			m_RecvVdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
			if( m_TmpSz != 0 ) //如果接收视频输出帧链表不为空。
			{
				m_RecvVdoOtptFrmLnkLst.Locked( NULL ); //接收视频输出帧链表的互斥锁加锁。
				m_RecvVdoOtptFrmLnkLst.GetHead( NULL, m_TmpByte3Pt, m_TmpByte3Sz, NULL, &m_TmpSz, 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收视频输出帧链表的第一个视频输出帧。
				m_RecvVdoOtptFrmLnkLst.DelHead( 0, m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //删除接收视频输出帧链表的第一个视频输出帧。
				m_RecvVdoOtptFrmLnkLst.Unlock( NULL ); //接收视频输出帧链表的互斥锁解锁。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个有图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个无图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
			}
			break;
		}
		case 1: //如果使用自适应抖动缓冲器。
		{
			int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
			int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
			int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
			int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
			VAjbGetBufFrmCnt( m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );

			if( p_CurHaveBufFrmCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );

				uint32_t p_VdoOtptFrmTimeStamp;

				//从视频自适应抖动缓冲器取出视频输出帧。
				VAjbGetFrm( m_VAjbPt, FuncGetTickAsMsec(), &p_VdoOtptFrmTimeStamp, m_TmpByte3Pt, m_TmpByte3Sz, &m_TmpSz, 1, NULL );

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
			}
			break;
		}
	}

	//写入视频输出帧。
	if( m_TmpSz > 0 ) //如果视频输出帧为有图像活动。
	{
		if( YU12VdoOtptSrcFrmPt != NULL ) //如果要使用YU12格式视频输出帧。
		{
			//读取视频输出帧宽度。
			*YU12VdoOtptSrcFrmWidthPt = ( m_TmpByte3Pt[0] & 0xFF ) + ( ( m_TmpByte3Pt[1] & 0xFF ) << 8 ) + ( ( m_TmpByte3Pt[2] & 0xFF ) << 16 ) + ( ( m_TmpByte3Pt[3] & 0xFF ) << 24 );
			//读取视频输出帧高度。
			*YU12VdoOtptSrcFrmHeightPt = ( m_TmpByte3Pt[4] & 0xFF ) + ( ( m_TmpByte3Pt[5] & 0xFF ) << 8 ) + ( ( m_TmpByte3Pt[6] & 0xFF ) << 16 ) + ( ( m_TmpByte3Pt[7] & 0xFF ) << 24 );

			if( m_TmpSz - 4 - 4 != *YU12VdoOtptSrcFrmWidthPt * *YU12VdoOtptSrcFrmHeightPt * 3 / 2 )
			{
				LOGFE( Cu8vstr( "视频输出帧的长度不等于YU12格式的长度。视频输出帧：%uzd，YU12格式：%z32d。" ), m_TmpSz - 4 - 4, *YU12VdoOtptSrcFrmWidthPt * *YU12VdoOtptSrcFrmHeightPt * 3 / 2 );
				*YU12VdoOtptSrcFrmWidthPt = 0;
				*YU12VdoOtptSrcFrmHeightPt = 0;
				return;
			}

			//写入YU12格式视频输出帧。
			memcpy( YU12VdoOtptSrcFrmPt, m_TmpByte3Pt + 4 + 4, m_TmpSz - 4 - 4 );
		}
		else //如果要使用已编码格式视频输出帧。
		{
			if( m_TmpSz > EncdVdoOtptSrcFrmSzByt )
			{
				*EncdVdoOtptSrcFrmLenBytPt = 0;
				LOGFE( Cu8vstr( "视频输出帧的长度已超过已编码格式的长度。视频输出帧：%uzd，已编码格式：%z32d。" ), m_TmpSz, EncdVdoOtptSrcFrmSzByt );
				return;
			}

			//写入已编码格式视频输出帧。
			memcpy( EncdVdoOtptSrcFrmPt, m_TmpByte3Pt, m_TmpSz );
			*EncdVdoOtptSrcFrmLenBytPt = m_TmpSz;
		}
	}
	else if( m_TmpSz == 0 ) //如果视频输出帧为无图像活动。
	{
		if( YU12VdoOtptSrcFrmPt != NULL ) //如果要使用YU12格式视频输出帧。
		{
			
		}
		else //如果要使用已编码格式视频输出帧。
		{
			
		}
	}
}

//用户定义的获取视频输出帧函数。
void MyMediaPocsThrdCls::UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
											uint8_t * YU12VdoOtptSrcFrmPt, int32_t YU12VdoOtptSrcFrmWidth, int32_t YU12VdoOtptSrcFrmHeight,
											uint8_t * EncdVdoOtptSrcFrmPt, size_t EncdVdoOtptSrcFrmLenByt )
{
	
}