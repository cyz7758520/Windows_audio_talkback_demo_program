#include "MySrvrThrd.h"
#include "WndAdoVdoTkbkDemo.h"

//我的服务端线程初始化。
int MySrvrThrdCls::Init( const void * LicnCodePt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	HWND p_SrvrUrlHdl;
	int p_SrvrUrlLenChr;
	Vstr * p_SrvrUrlVstrPt = NULL;
	Vstr * p_PrtclVstrPt = NULL;
	Vstr * p_HostnameVstrPt = NULL;
	Vstr * p_PortVstrPt = NULL;
	int p_CmpRslt;

	if( m_SrvrThrdPt == NULL ) //如果我的服务端线程未启动。
	{
		p_SrvrUrlHdl = GetDlgItem( m_MainDlgWndHdl, SrvrUrlCbBoxId );
		p_SrvrUrlLenChr = GetWindowTextLength( p_SrvrUrlHdl );
		VstrInit( &p_SrvrUrlVstrPt, Utf16, p_SrvrUrlLenChr + 1, );
		GetDlgItemText( m_MainDlgWndHdl, SrvrUrlCbBoxId, ( LPWSTR )p_SrvrUrlVstrPt->m_StrPt, p_SrvrUrlLenChr + 1 );
		p_SrvrUrlVstrPt->m_LenChr = p_SrvrUrlLenChr;

		VstrInit( &p_PrtclVstrPt, Utf16, , );
		VstrInit( &p_HostnameVstrPt, Utf16, , );
		VstrInit( &p_PortVstrPt, Utf16, , );

		if( VstrUrlParse( p_SrvrUrlVstrPt, p_PrtclVstrPt, NULL, NULL, p_HostnameVstrPt, p_PortVstrPt, NULL, NULL, NULL, NULL, g_ErrInfoVstr.m_Pt ) != 0 )
		{
			g_ErrInfoVstr.Ins( 0, Cu8vstr( "解析服务端Url字符串失败。原因：" ) );
			LOGE( g_ErrInfoVstr.m_Pt );
			Toast( m_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_Pt );
			goto Out;
		}
		if( ( VstrCmp( p_PrtclVstrPt, , , Cu8vstr( "Tcp" ), , &p_CmpRslt ), p_CmpRslt != 0 ) && ( VstrCmp( p_PrtclVstrPt, , , Cu8vstr( "Audp" ), , &p_CmpRslt ), p_CmpRslt != 0 ) )
		{
			g_ErrInfoVstr.Cpy( Cu8vstr( "服务端Url字符串的协议不正确。" ) );
			LOGE( g_ErrInfoVstr.m_Pt );
			Toast( m_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_Pt );
			goto Out;
		}
		if( VstrCmp( p_PortVstrPt, , , Cu8vstr( "" ), , &p_CmpRslt ), p_CmpRslt == 0 )
		{
			VstrCpy( p_PortVstrPt, Cu8vstr( "12345" ), , );
		}

		LOGI( Cu8vstr( "我的服务端线程初始化开始。" ) );
		
		//服务端线程初始化。
		if( SrvrThrdCls::Init( LicnCodePt, g_ErrInfoVstr.m_Pt ) != 0 )
		{
			LOGFE( Cu8vstr( "服务端线程初始化失败。原因：%vs" ), g_ErrInfoVstr.m_Pt );
			goto Out;
		}

		//设置所有连接销毁时是否自动请求退出。
		m_SrvrThrdPt->m_IsAutoRqirExit = 2;

		//设置是否打印Log日志、显示Toast。
		SetIsPrintLogShowToast( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
								( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
								NULL,
								g_ErrInfoVstr.m_Pt );

		//设置是否使用阻止系统休眠。
		SendSetIsUsePrvntSysSleepMsg( 0,
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId ) == BST_CHECKED ) ? 1 : 0,
									  g_ErrInfoVstr.m_Pt );

		//设置是否测试网络延迟。
		SendSetIsTstNtwkDlyMsg( 0,
								( IsDlgButtonChecked( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId ) == BST_CHECKED ) ? 1 : 0,
								1000,
								g_ErrInfoVstr.m_Pt );

		//服务端初始化。
		SendSrvrInitMsg( 0,
						 ( VstrCmp( p_PrtclVstrPt, , , Cu8vstr( "Tcp" ), , &p_CmpRslt ), p_CmpRslt == 0 ) ? 0 : 1,
						 p_HostnameVstrPt,
						 p_PortVstrPt,
						 10,
						 2,
						 NULL );

		//我的服务端线程启动。
		if( Start( g_ErrInfoVstr.m_Pt ) != 0 )
		{
			LOGFE( Cu8vstr( "我的服务端线程启动失败。原因：%vs" ), g_ErrInfoVstr.m_Pt );
			goto Out;
		}

		LOGI( Cu8vstr( "我的服务端线程初始化结束。" ) );
	}

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_SrvrUrlVstrPt );
	VstrDstoy( p_PrtclVstrPt );
	VstrDstoy( p_HostnameVstrPt );
	VstrDstoy( p_PortVstrPt );
	if( p_Rslt != 0 )
	{
		Dstoy();
	}
	return p_Rslt;
}

//我的服务端线程销毁。
void MySrvrThrdCls::Dstoy()
{
	LOGI( Cu8vstr( "请求并等待我的服务端线程退出开始。" ) );
	SendRqirExitMsg( 1, g_ErrInfoVstr.m_Pt );
	LOGI( Cu8vstr( "请求并等待我的服务端线程退出结束。" ) );
}

//用户定义的初始化函数。
void MySrvrThrdCls::UserInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMySrvrThrdInit, 0, 0 );
}

//用户定义的销毁函数。
void MySrvrThrdCls::UserDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMySrvrThrdDstoy, 0, 0 );
}

//用户定义的处理函数。
void MySrvrThrdCls::UserPocs()
{
	
}

//用户定义的消息函数。
int MySrvrThrdCls::UserMsg( unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt )
{
	return 0;
}

//用户定义的显示日志函数。
void MySrvrThrdCls::UserShowLog( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的显示Toast函数。
void MySrvrThrdCls::UserShowToast( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowToast, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的服务端初始化函数。
void MySrvrThrdCls::UserSrvrInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypSrvrInit, 0, 0 );
}

//用户定义的服务端销毁函数。
void MySrvrThrdCls::UserSrvrDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypSrvrDstoy, 0, 0 );
}

//用户定义的连接初始化函数。
void MySrvrThrdCls::UserCnctInit( SrvrThrd::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	MainDlgWndMsgCnctLstAddItem * p_MainDlgWndMsgCnctLstAddItemPt = ( MainDlgWndMsgCnctLstAddItem * )malloc( sizeof( MainDlgWndMsgCnctLstAddItem ) );

	p_MainDlgWndMsgCnctLstAddItemPt->m_IsTcpOrAudpPrtcl = IsTcpOrAudpPrtcl;
	VstrInit( &p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt );
	VstrInit( &p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstAddItem, ( WPARAM )p_MainDlgWndMsgCnctLstAddItemPt, 0 );
}

//用户定义的连接销毁函数。
void MySrvrThrdCls::UserCnctDstoy( SrvrThrd::CnctInfo * CnctInfoPt )
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstDelItem, ( WPARAM )CnctInfoPt->m_Num, 0 );
}

//用户定义的连接状态函数。
void MySrvrThrdCls::UserCnctSts( SrvrThrd::CnctInfo * CnctInfoPt, int32_t CurCnctSts )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Utf16, ,  );
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt, Utf16, ,  );
	if( CurCnctSts == SrvrThrd::CnctStsWait )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "等待远端接受连接" ), ,  );
	}
	else if( CurCnctSts < SrvrThrd::CnctStsWait )
	{
		VstrFmtCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "第%d次连接" ), -CurCnctSts );
	}
	else if( CurCnctSts == SrvrThrd::CnctStsCnct )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "已连接" ), ,  );
	}
	else if( CurCnctSts == SrvrThrd::CnctStsTmot )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "异常断开" ), ,  );
	}
	else if( CurCnctSts == SrvrThrd::CnctStsDsct )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "已断开" ), ,  );
	}
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接远端对讲模式函数。
void MySrvrThrdCls::UserCnctRmtTkbkMode( SrvrThrd::CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt, Utf16, , Cu8vstr( g_TkbkModeU8strArrPt[ NewRmtTkbkMode ] ) );
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接测试网络延迟函数。
void MySrvrThrdCls::UserCnctTstNtwkDly( SrvrThrd::CnctInfo * CnctInfoPt, uint64_t NtwkDlyMsec )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );
	
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt = NULL;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt, Utf16, ,  );
	VstrFmtCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt, Cu8vstr( "%uz64dms" ), NtwkDlyMsec );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}
