#include "MyClntMediaPocsThrd.h"
#include "WndAdoVdoTkbkDemo.h"

//我的客户端媒体处理线程初始化。
int MyClntMediaPocsThrdCls::Init( const void * LicnCodePt )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( m_ClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未启动。
	{
		wchar_t p_TmpU16strPt[ 1024 ];

		LOGI( Cu8vstr( "开始启动我的客户端媒体处理线程。" ) );

		//初始化我的客户端媒体处理线程。
		if( ClntMediaPocsThrdCls::Init( LicnCodePt, &g_ErrInfoVstr ) != 0 )
		{
			LOGFE( Cu8vstr( "初始化我的客户端媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
			goto Out;
		}

		//设置网络。
		{
			//设置传输模式。
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode = ( IsDlgButtonChecked( g_ClntStngDlgWndHdl, UsePttRdBtnId ) == BST_CHECKED ) ? 0 : 1;

			//设置最大连接次数。
			GetWindowText( GetDlgItem( g_ClntStngDlgWndHdl, MaxCnctTimesEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_MaxCnctTimes = _wtoi( p_TmpU16strPt );

			//设置要参考远端对讲模式来设置对讲模式。
			m_ClntMediaPocsThrdPt->m_IsReferRmtTkbkModeSetTkbkMode = ( IsDlgButtonChecked( g_ClntStngDlgWndHdl, IsReferRmtTkbkModeSetTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0;

			//设置在对讲客户端的连接销毁且广播客户端销毁时自动请求退出。
			m_ClntMediaPocsThrdPt->m_IsAutoRqirExit = 1;
		}

		//设置是否使用容器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId ) == BST_CHECKED )
		{
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_UseWhatRecvOtptFrm = 0;
		}

		//设置是否使用自适应抖动缓冲器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseAjbRecvOtptFrmRdBtnId ) == BST_CHECKED )
		{
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_UseWhatRecvOtptFrm = 1;

			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_AAjbParm.m_MinNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_AAjbParm.m_MaxNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_AAjbParm.m_MaxCntuLostFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_AAjbParm.m_AdaptSensitivity = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_VAjbParm.m_MinNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_VAjbParm.m_MaxNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			m_ClntMediaPocsThrdPt->m_TkbkClnt.m_VAjbParm.m_AdaptSensitivity = _wtof( p_TmpU16strPt );
		}

		//设置是否打印Log日志、显示Toast。
		m_MediaPocsThrdClsPt->SetIsPrintLogShowToast(
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			NULL,
			&g_ErrInfoVstr );

		//设置是否使用阻止系统休眠。
		m_MediaPocsThrdClsPt->SetIsUsePrvntSysSleep(
			0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			&g_ErrInfoVstr );

		//启动我的客户端媒体处理线程。
		if( m_MediaPocsThrdClsPt->Start( &g_ErrInfoVstr ) != 0 )
		{
			LOGFE( Cu8vstr( "启动我的客户端媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
			goto Out;
		}

		LOGI( Cu8vstr( "启动我的客户端媒体处理线程完毕。" ) );

		p_Rslt = 0; //设置本函数执行成功。
	}

	Out:
	if( p_Rslt != 0 )
	{
		Dstoy();
	}
	return p_Rslt;
}

//我的客户端媒体处理线程销毁。
void MyClntMediaPocsThrdCls::Dstoy()
{
	LOGI( Cu8vstr( "请求并等待我的客户端媒体处理线程退出开始。" ) );
	m_MediaPocsThrdClsPt->RqirExit( 1, 1, &g_ErrInfoVstr );
	LOGI( Cu8vstr( "请求并等待我的客户端媒体处理线程退出结束。" ) );
}

//对讲初始化。
int MyClntMediaPocsThrdCls::TkbkInit( const void * LicnCodePt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	int p_Num = ListView_GetSelectionMark( g_ClntLstWndHdl );
	int p_IsTcpOrAudpPrtcl;
	Vstr * p_RmtNodeNameVstrPt = NULL;
	Vstr * p_RmtNodeSrvcVstrPt = NULL;
	wchar_t p_TmpU16strPt[ 1024 ];
	
	LOGI( Cu8vstr( "对讲初始化开始。" ) );

	if( p_Num != -1 )
	{
		if( m_ClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未启动。
		{
			if( Init( LicnCodePt ) != 0 ) //如果我的客户端媒体处理线程初始化失败。
			{
				goto Out;
			}
		}
		
		//发送对讲客户端设置是否测试网络延迟消息。
		SendTkbkClntSetIsTstNtwkDlyMsg( 0,
										( IsDlgButtonChecked( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId ) == BST_CHECKED ) ? 1 : 0,
										1000,
										&g_ErrInfoVstr );

		//发送对讲客户端的连接初始化消息。
		m_TkbkClntNum = p_Num;
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 0, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		p_IsTcpOrAudpPrtcl = ( wcscmp( p_TmpU16strPt, L"Tcp" ) == 0 ) ? 0 : 1;
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		VstrInit( &p_RmtNodeNameVstrPt, Utf16, , Cu16vstr( p_TmpU16strPt ) );
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		VstrInit( &p_RmtNodeSrvcVstrPt, Utf16, , Cu16vstr( p_TmpU16strPt ) );
		SendTkbkClntCnctInitMsg( 1, p_IsTcpOrAudpPrtcl, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt, NULL );

		//发送对讲客户端的本端对讲模式消息。
		SendTkbkClntLclTkbkModeMsg( 0,
									( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeAdoInpt : 0 ) +
									( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeAdoOtpt : 0 ) +
									( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeVdoInpt : 0 ) +
									( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeVdoOtpt : 0 ),
									&g_ErrInfoVstr );
	}
	
	LOGI( Cu8vstr( "对讲初始化结束。" ) );
	
	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_RmtNodeNameVstrPt );
	VstrDstoy( p_RmtNodeSrvcVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		TkbkDstoy();
	}
	return ( INT_PTR )TRUE;
}

//对讲销毁。
void MyClntMediaPocsThrdCls::TkbkDstoy()
{
	LOGI( Cu8vstr( "开始请求并等待对讲销毁。" ) );
	SendTkbkClntCnctDstoyMsg( 1, NULL );
	LOGI( Cu8vstr( "结束请求并等待对讲销毁。" ) );
}

//广播初始化。
int MyClntMediaPocsThrdCls::BdctInit( const void * LicnCodePt )
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	int p_IsTcpOrAudpPrtcl;
	Vstr * p_RmtNodeNameVstrPt = NULL;
	Vstr * p_RmtNodeSrvcVstrPt = NULL;
	wchar_t p_TmpU16strPt[ 1024 ];

	LOGI( Cu8vstr( "广播初始化开始。" ) );

	if( m_ClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未启动。
	{
		if( Init( LicnCodePt ) != 0 ) //如果我的客户端媒体处理线程初始化失败。
		{
			goto Out;
		}
	}

	//发送广播客户端初始化消息。
	SendBdctClntInitMsg( 0, 0, NULL );

	//添加客户端列表。
	VstrInit( &p_RmtNodeNameVstrPt, Utf16, ,  );
	VstrInit( &p_RmtNodeSrvcVstrPt, Utf16, ,  );
	for( int p_Num = 0, p_ClntLstTotal = ListView_GetItemCount( g_ClntLstWndHdl ); p_Num < p_ClntLstTotal; p_Num++ )
	{
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 0, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		p_IsTcpOrAudpPrtcl = ( wcscmp( p_TmpU16strPt, L"Tcp" ) == 0 ) ? 0 : 1;
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		VstrCpy( p_RmtNodeNameVstrPt, Cu16vstr( p_TmpU16strPt ), ,  );
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		VstrCpy( p_RmtNodeSrvcVstrPt, Cu16vstr( p_TmpU16strPt ), ,  );
		SendBdctClntCnctInitMsg( 0, p_IsTcpOrAudpPrtcl, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt, NULL );
	}

	LOGI( Cu8vstr( "广播初始化结束。" ) );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_RmtNodeNameVstrPt );
	VstrDstoy( p_RmtNodeSrvcVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		BdctDstoy();
	}
	return p_Rslt;
}

//广播销毁。
void MyClntMediaPocsThrdCls::BdctDstoy()
{
	LOGI( Cu8vstr( "开始请求并等待广播销毁。" ) );
	SendBdctClntDstoyMsg( 1, NULL );
	LOGI( Cu8vstr( "结束请求并等待广播销毁。" ) );
}

//用户定义的显示日志函数。
void MyClntMediaPocsThrdCls::UserShowLog( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的显示Toast函数。
void MyClntMediaPocsThrdCls::UserShowToast( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowToast, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的客户端媒体处理线程初始化函数。
void MyClntMediaPocsThrdCls::UserClntMediaPocsThrdInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMyClntMediaPocsThrdInit, 0, 0 );
}

//用户定义的客户端媒体处理线程销毁函数。
void MyClntMediaPocsThrdCls::UserClntMediaPocsThrdDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMyClntMediaPocsThrdDstoy, 0, 0 );
}

//用户定义的对讲客户端连接初始化函数。
void MyClntMediaPocsThrdCls::UserTkbkClntCnctInit( int32_t IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypTkbkClntCnctInit, 0, 0 );
}

//用户定义的对讲客户端连接销毁函数。
void MyClntMediaPocsThrdCls::UserTkbkClntCnctDstoy()
{
	MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )malloc( sizeof( MainDlgWndMsgClntLstModifyItem ) );

	p_MainDlgWndMsgClntLstModifyItemPt->m_Num = m_TkbkClntNum;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Utf16, ,  );
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt, Utf16, ,  );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypClntLstModifyItem, ( WPARAM )p_MainDlgWndMsgClntLstModifyItemPt, 0 );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypTkbkClntCnctDstoy, 0, 0 );
	SendVdoInptOtptWndDstoyMsg( m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl );
}

//用户定义的对讲客户端连接状态函数。
void MyClntMediaPocsThrdCls::UserTkbkClntCnctSts( int32_t CurCnctSts )
{
	MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )malloc( sizeof( MainDlgWndMsgClntLstModifyItem ) );

	p_MainDlgWndMsgClntLstModifyItemPt->m_Num = m_TkbkClntNum;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Utf16, ,  );
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt = NULL;
	if( CurCnctSts == ClntMediaPocsThrd::CnctStsWait )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "等待远端接受连接" ), ,  );
	}
	else if( CurCnctSts < ClntMediaPocsThrd::CnctStsWait )
	{
		VstrFmtCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "第%d次连接" ), -CurCnctSts );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsCnct )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "已连接" ), ,  );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsTmot )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "异常断开" ), ,  );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsDsct )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt, Cu8vstr( "已断开" ), ,  );
	}
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypClntLstModifyItem, ( WPARAM )p_MainDlgWndMsgClntLstModifyItemPt, 0 );
}

//用户定义的对讲客户端我的对讲索引函数。
void MyClntMediaPocsThrdCls::UserTkbkClntMyTkbkIdx( int32_t MyTkbkIdx )
{
	Vstr * p_TitleVstrPt;

	AvstrFmtCpy( p_TitleVstrPt, Utf16, Cu8vstr( "视频输入预览%z32d" ), MyTkbkIdx );
	SendVdoInptOtptWndSetTitleMsg( m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl, p_TitleVstrPt );
}

//用户定义的对讲客户端本端对讲模式函数。
void MyClntMediaPocsThrdCls::UserTkbkClntLclTkbkMode( int32_t OldLclTkbkMode, int32_t NewLclTkbkMode )
{
	if( ( NewLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) //如果新对讲模式有音频输入。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) == 0 ) //如果旧对讲模式无音频输入。
		{
			if( m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_IsInit == 0 ) //如果音频输入未初始化。
			{
				SetToUseAdoInpt(); //设置要使用音频输入。
			}
		}
	}
	else //如果新对讲模式无音频输入。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) //如果旧对讲模式有音频输入。
		{
			SetNotUseAdoInpt(); //设置不使用音频输入。
		}
	}

	if( ( NewLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) //如果新对讲模式有音频输出。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) == 0 ) //如果旧对讲模式无音频输出。
		{
			SetToUseAdoOtpt(); //设置要使用音频输出。
		}
	}
	else //如果新对讲模式无音频输出。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) //如果旧对讲模式有音频输出。
		{
			SetNotUseAdoOtpt(); //设置不使用音频输入。
		}
	}

	if( ( NewLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) //如果新对讲模式有视频输入。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) == 0 ) //如果旧对讲模式无视频输入。
		{
			SetToUseVdoInpt(); //设置要使用视频输入。
		}
	}
	else //如果新对讲模式无视频输入。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) //如果旧对讲模式有视频输入。
		{
			SetNotUseVdoInpt(); //设置不使用视频输入。
		}
	}

	if( ( NewLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) //如果新对讲模式有视频输出。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) == 0 ) //如果旧对讲模式无视频输出。
		{
			SetToUseVdoOtpt(); //设置要使用视频输出。
		}
	}
	else //如果新对讲模式无视频输出。
	{
		if( ( OldLclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) //如果旧对讲模式有视频输出。
		{
			SetNotUseVdoOtpt(); //设置不使用视频输出。
		}
	}

	//设置是否保存音视频输入输出到Avi文件。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED )
	{
		wchar_t p_FullPathU16strPt[ 1024 ];
		wchar_t p_TmpU16strPt[ 1024 ];
		size_t p_WrBufSzByt;

		GetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId ), p_FullPathU16strPt, SzOfArr( p_FullPathU16strPt ) );
		GetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		p_WrBufSzByt = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->SetIsSaveAdoVdoInptOtptToAviFile( 0,
																Cu16vstr( p_FullPathU16strPt ),
																p_WrBufSzByt,
																( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置是否保存状态到Txt文件。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId ) == BST_CHECKED )
	{
		wchar_t p_FullPathU16strPt[ 1024 ];

		GetWindowText( GetDlgItem( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId ), p_FullPathU16strPt, SzOfArr( p_FullPathU16strPt ) );

		m_MediaPocsThrdClsPt->SaveStsToTxtFile( 0,
												Cu16vstr( p_FullPathU16strPt ),
												( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}
}

//用户定义的对讲客户端对讲信息初始化函数。
void MyClntMediaPocsThrdCls::UserTkbkClntTkbkInfoInit( TkbkClnt::TkbkInfo * TkbkInfoPt )
{

}

//用户定义的对讲客户端对讲信息销毁函数。
void MyClntMediaPocsThrdCls::UserTkbkClntTkbkInfoDstoy( TkbkClnt::TkbkInfo * TkbkInfoPt )
{
	size_t p_StrmNum = 0;
	VdoOtpt::Strm * p_StrmPt; //存放流的指针。

	m_MediaPocsThrdClsPt->AdoOtptDelStrm( 1, 0, TkbkInfoPt->m_TkbkIdx, NULL ); //删除流操作需要立即执行，因为要防止中途出现其他消息导致重复删除流。

	for( ; m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, &p_StrmPt, NULL, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == TkbkInfoPt->m_TkbkIdx )
		{
			SendVdoInptOtptWndDstoyMsg( p_StrmPt->m_Dvc.m_DspyWndHdl );
			break;
		}
	}
	m_MediaPocsThrdClsPt->VdoOtptDelStrm( 1, 0, TkbkInfoPt->m_TkbkIdx, NULL ); //删除流操作需要立即执行，因为要防止中途出现其他消息导致重复删除流。
}

//用户定义的对讲客户端对讲信息远端对讲模式函数。
void MyClntMediaPocsThrdCls::UserTkbkClntTkbkInfoRmtTkbkMode( TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode )
{
	//设置音频输出流。
	if( ( NewRmtTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) //如果新对讲模式有音频输入。
	{
		if( ( OldRmtTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) == 0 ) //如果旧对讲模式无音频输入。
		{
			if( ( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) //如果本端对讲模式有音频输出。
			{
				SetToUseAdoOtptStrm( TkbkInfoPt->m_TkbkIdx ); //设置要使用音频输出流。
			}
		}
	}
	else //如果新对讲模式无音频输出。
	{
		if( ( OldRmtTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) //如果旧对讲模式有音频输入。
		{
			if( ( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeAdoOtpt ) != 0 ) //如果本端对讲模式有音频输出。
			{
				SetNotUseAdoOtptStrm( TkbkInfoPt->m_TkbkIdx ); //设置不使用音频输出流。
			}
		}
	}

	//设置视频输出流。
	if( ( NewRmtTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) //如果新对讲模式有视频输入。
	{
		if( ( OldRmtTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) == 0 ) //如果旧对讲模式无视频输入。
		{
			if( ( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) //如果本端对讲模式有视频输出。
			{
				SetToUseVdoOtptStrm( TkbkInfoPt->m_TkbkIdx ); //设置要使用视频输出流。
			}
		}
	}
	else //如果新对讲模式无视频输入。
	{
		if( ( OldRmtTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) //如果旧对讲模式有视频输入。
		{
			if( ( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_LclTkbkMode & ClntMediaPocsThrd::TkbkModeVdoOtpt ) != 0 ) //如果本端对讲模式有视频输出。
			{
				SetNotUseVdoOtptStrm( TkbkInfoPt->m_TkbkIdx ); //设置不使用视频输出流。
			}
		}
	}
}

//用户定义的对讲客户端测试网络延迟函数。
void MyClntMediaPocsThrdCls::UserTkbkClntTstNtwkDly( uint64_t NtwkDlyMsec )
{
	MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )malloc( sizeof( MainDlgWndMsgClntLstModifyItem ) );
	
	p_MainDlgWndMsgClntLstModifyItemPt->m_Num = m_TkbkClntNum;
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt = NULL;
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt, Utf16, ,  );
	VstrFmtCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt, Cu8vstr( "%uz64dms" ), NtwkDlyMsec );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypClntLstModifyItem, ( WPARAM )p_MainDlgWndMsgClntLstModifyItemPt, 0 );
}

//用户定义的广播客户端初始化函数。
void MyClntMediaPocsThrdCls::UserBdctClntInit()
{
    if( m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_AdoInpt.m_IsInit == 0 ) //如果音频输入未初始化。
	{
		SetToUseAdoInpt(); //设置要使用音频输入。
	}
}

//用户定义的广播客户端销毁函数。
void MyClntMediaPocsThrdCls::UserBdctClntDstoy()
{
	
}

//用户定义的广播客户端连接初始化函数。
void MyClntMediaPocsThrdCls::UserBdctClntCnctInit( BdctClnt::CnctInfo * CnctInfoPt, int32_t IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	
}

//用户定义的广播客户端连接销毁函数。
void MyClntMediaPocsThrdCls::UserBdctClntCnctDstoy( BdctClnt::CnctInfo * CnctInfoPt )
{
	MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )malloc( sizeof( MainDlgWndMsgClntLstModifyItem ) );
	
	p_MainDlgWndMsgClntLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Utf16, ,  );
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypClntLstModifyItem, ( WPARAM )p_MainDlgWndMsgClntLstModifyItemPt, 0 );
}

//用户定义的连接状态函数。
void MyClntMediaPocsThrdCls::UserBdctClntCnctSts( BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts )
{
	MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )malloc( sizeof( MainDlgWndMsgClntLstModifyItem ) );
	
	p_MainDlgWndMsgClntLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Utf16, ,  );
	if( CurCnctSts == ClntMediaPocsThrd::CnctStsWait )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Cu8vstr( "等待远端接受连接" ), ,  );
	}
	else if( CurCnctSts < ClntMediaPocsThrd::CnctStsWait )
	{
		VstrFmtCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Cu8vstr( "第%d次连接" ), -CurCnctSts );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsCnct )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Cu8vstr( "已连接" ), ,  );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsTmot )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Cu8vstr( "异常断开" ), ,  );
	}
	else if( CurCnctSts == ClntMediaPocsThrd::CnctStsDsct )
	{
		VstrCpy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt, Cu8vstr( "已断开" ), ,  );
	}
	p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypClntLstModifyItem, ( WPARAM )p_MainDlgWndMsgClntLstModifyItemPt, 0 );
}

//设置要使用音频输入。
void MyClntMediaPocsThrdCls::SetToUseAdoInpt()
{
	wchar_t p_TmpU16strPt[ 1024 ];

	//设置音频输入。
	m_MediaPocsThrdClsPt->SetAdoInpt( 0,
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
									  ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输入是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
	m_MediaPocsThrdClsPt->AdoInptSetIsUseSystemAecNsAgc( 0, ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId ) == BST_CHECKED ) ? 1 : 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	if( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		m_MediaPocsThrdClsPt->AdoInptSetUseNoAec( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}
	else //如果传输模式为实时全双工。
	{
		//设置音频输入是否不使用声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED )
		{
			m_MediaPocsThrdClsPt->AdoInptSetUseNoAec( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//设置音频输入是否使用Speex声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_FilterLenMsec;
			int32_t p_IsUseRec;
			float p_EchoMultiple;
			float p_EchoCntu;
			int32_t p_EchoSupes;
			int32_t p_EchoSupesAct;

			GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_FilterLenMsec = _wtoi( p_TmpU16strPt );
			p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoMultiple = _wtof( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoCntu = _wtof( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoSupes = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoSupesAct = _wtoi( p_TmpU16strPt );

			m_MediaPocsThrdClsPt->AdoInptSetUseSpeexAec( 0, p_FilterLenMsec, p_IsUseRec, p_EchoMultiple, p_EchoCntu, p_EchoSupes, p_EchoSupesAct, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//设置音频输入是否使用WebRtc定点版声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecmRdBtnId ) == BST_CHECKED )
		{
			int32_t p_IsUseCNGMode;
			int32_t p_EchoMode;
			int32_t p_Delay;

			p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoMode = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_Delay = _wtoi( p_TmpU16strPt );

			m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcAecm( 0, p_IsUseCNGMode, p_EchoMode, p_Delay, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//设置音频输入是否使用WebRtc浮点版声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_EchoMode;
			int32_t p_Delay;
			int32_t p_IsUseDelayAgstcMode;
			int32_t p_IsUseExtdFilterMode;
			int32_t p_IsUseRefinedFilterAdaptAecMode;
			int32_t p_IsUseAdaptAdjDelay;

			GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EchoMode = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_Delay = _wtoi( p_TmpU16strPt );
			p_IsUseDelayAgstcMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
			p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
			p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
			p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );

			m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcAec( 0, p_EchoMode, p_Delay, p_IsUseDelayAgstcMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
		
		//设置音频输入是否使用WebRtc第三版声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAec3RdBtnId ) == BST_CHECKED )
		{
			int32_t p_Delay;

			GetWindowText( GetDlgItem( g_WebRtcAec3StngDlgWndHdl, WebRtcAec3DelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_Delay = _wtoi( p_TmpU16strPt );

			m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcAec3( 0, p_Delay, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}

		//设置音频输入是否使用SpeexWebRtc三重声学回音消除器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_WorkMode;
			int32_t p_SpeexAecFilterLenMsec;
			int32_t p_SpeexAecIsUseRec;
			float p_SpeexAecEchoMultiple;
			float p_SpeexAecEchoCntu;
			int32_t p_SpeexAecEchoSupes;
			int32_t p_SpeexAecEchoSupesAct;
			int32_t p_WebRtcAecmIsUseCNGMode;
			int32_t p_WebRtcAecmEchoMode;
			int32_t p_WebRtcAecmDelay;
			int32_t p_WebRtcAecEchoMode;
			int32_t p_WebRtcAecDelay;
			int32_t p_WebRtcAecIsUseDelayAgstcMode;
			int32_t p_WebRtcAecIsUseExtdFilterMode;
			int32_t p_WebRtcAecIsUseRefinedFilterAdaptAecMode;
			int32_t p_WebRtcAecIsUseAdaptAdjDelay;
			int32_t p_WebRtcAec3Delay;
			int32_t p_IsUseSameRoomAec;
			int32_t p_SameRoomEchoMinDelay;

			p_WorkMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId ) == BST_CHECKED ) ? SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm :
						 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec :
						 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec :
						 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec3RdBtnId ) == BST_CHECKED ) ? SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec3 :
						 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec3RdBtnId ) == BST_CHECKED ) ? SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec3 : 0;
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SpeexAecFilterLenMsec = _wtoi( p_TmpU16strPt );
			p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SpeexAecEchoMultiple = _wtof( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SpeexAecEchoCntu = _wtof( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SpeexAecEchoSupes = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SpeexAecEchoSupesAct = _wtoi( p_TmpU16strPt );
			p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_WebRtcAecmEchoMode = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_WebRtcAecmDelay = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_WebRtcAecEchoMode = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_WebRtcAecDelay = _wtoi( p_TmpU16strPt );
			p_WebRtcAecIsUseDelayAgstcMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
			p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
			p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
			p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAec3DelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_WebRtcAec3Delay = _wtoi( p_TmpU16strPt );
			p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_SameRoomEchoMinDelay = _wtoi( p_TmpU16strPt );

			m_MediaPocsThrdClsPt->AdoInptSetUseSpeexWebRtcAec( 0, p_WorkMode, p_SpeexAecFilterLenMsec, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCntu, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgstcMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_WebRtcAec3Delay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
		}
	}

	//设置音频输入是否不使用噪音抑制器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoInptSetUseNoNs( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用Speex预处理器的噪音抑制。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexPrpocsNsRdBtnId ) == BST_CHECKED )
	{
		int32_t p_IsUseNs;
		int32_t p_NoiseSupes;
		int32_t p_IsUseDereverb;

		p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId ) == BST_CHECKED );
		GetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_NoiseSupes = _wtoi( p_TmpU16strPt );
		p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId ) == BST_CHECKED );

		m_MediaPocsThrdClsPt->AdoInptSetUseSpeexPrpocsNs( 0, p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用WebRtc定点版噪音抑制器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED )
	{
		int32_t p_PolicyMode;

		GetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_PolicyMode = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcNsx( 0, p_PolicyMode, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用WebRtc浮点版噪音抑制器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED )
	{
		int32_t p_PolicyMode;

		GetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_PolicyMode = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcNs( 0, p_PolicyMode, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用RNNoise噪音抑制器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoInptSetUseRNNoise( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用Speex预处理器。
	{
		int32_t p_IsUseVad;
		int32_t p_VadProbStart;
		int32_t p_VadProbCont;
		int32_t p_IsUseAgc;
		int32_t p_AgcLevel;
		int32_t p_AgcIncrement;
		int32_t p_AgcDecrement;
		int32_t p_AgcMaxGain;

		p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId ) == BST_CHECKED );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_VadProbStart = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_VadProbCont = _wtoi( p_TmpU16strPt );
		p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_AgcLevel = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_AgcIncrement = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_AgcDecrement = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_AgcMaxGain = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoInptSetIsUseSpeexPrpocs( 0, ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId ) == BST_CHECKED ) ? 1 : 0, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用Pcm原始数据。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoInptSetUsePcm( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用Speex编码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
	{
		int32_t p_EncdUseCbrOrVbr;
		int32_t p_EncdQualt;
		int32_t p_EncdCmplxt;
		int32_t p_EncdPlcExptLossRate;

		p_EncdUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId ) == BST_CHECKED ) ? 0 : 1;
		GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_EncdQualt = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_EncdCmplxt = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_EncdPlcExptLossRate = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoInptSetUseSpeexEncd( 0, p_EncdUseCbrOrVbr, p_EncdQualt, p_EncdCmplxt, p_EncdPlcExptLossRate, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否使用Opus编码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoInptSetUseOpusEncd( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输入是否绘制音频波形到窗口。
	m_MediaPocsThrdClsPt->AdoInptSetIsDrawAdoWavfmToWnd( 0,
														 ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														 GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ),
														 GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ),
														 ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输入是否保存音频到Wave文件。
	if( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) &&
		( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) )
	{
		wchar_t p_AdoInptSrcFullPathU16strPt[ 1024 ];
		wchar_t p_AdoInptRsltFullPathU16strPt[ 1024 ];
		size_t p_WrBufSzByt;

		GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId ), p_AdoInptSrcFullPathU16strPt, SzOfArr( p_AdoInptSrcFullPathU16strPt ) );
		GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId ), p_AdoInptRsltFullPathU16strPt, SzOfArr( p_AdoInptRsltFullPathU16strPt ) );
		GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		p_WrBufSzByt = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoInptSetIsSaveAdoToWaveFile( 0,
															 ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															 Cu16vstr( p_AdoInptSrcFullPathU16strPt ),
															 Cu16vstr( p_AdoInptRsltFullPathU16strPt ),
															 p_WrBufSzByt,
															 & g_ErrInfoVstr );
	}

	//设置音频输入使用的设备。
	m_MediaPocsThrdClsPt->AdoInptSetUseDvc( 0,
											SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
											( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输入是否静音。
	m_MediaPocsThrdClsPt->AdoInptSetIsMute( 0,
											( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
											& g_ErrInfoVstr );
}

//设置不使用音频输入。
void MyClntMediaPocsThrdCls::SetNotUseAdoInpt()
{

}

//设置要使用音频输出。
void MyClntMediaPocsThrdCls::SetToUseAdoOtpt()
{
	wchar_t p_TmpU16strPt[ 1024 ];
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	//设置音频输出。
	m_MediaPocsThrdClsPt->SetAdoOtpt( 0,
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
									  ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输出是否保存音频到Wave文件。
	if( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) &&
		( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) )
	{
		wchar_t p_AdoOtptSrcFullPathU16strPt[ 1024 ];
		size_t p_WrBufSzByt;

		GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId ), p_AdoOtptSrcFullPathU16strPt, SzOfArr( p_AdoOtptSrcFullPathU16strPt ) );
		GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
		p_WrBufSzByt = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->AdoOtptSetIsSaveAdoToWaveFile( 0,
															 ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															 Cu16vstr( p_AdoOtptSrcFullPathU16strPt ),
															 p_WrBufSzByt,
															 ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输出是否绘制音频波形到窗口。
	m_MediaPocsThrdClsPt->AdoOtptSetIsDrawAdoWavfmToWnd( 0,
														 ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														 GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ),
														 ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输出使用的设备。
	m_MediaPocsThrdClsPt->AdoOtptSetUseDvc( 0,
											SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
											( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输出是否静音。
	m_MediaPocsThrdClsPt->AdoOtptSetIsMute( 0,
											( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
											( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	
	for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
	{
		if( ( p_TkbkInfoTmpPt->m_IsInit != 0 ) &&
			( ( p_TkbkInfoTmpPt->m_RmtTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) )
		{
			SetToUseAdoOtptStrm( p_TkbkInfoTmpPt->m_TkbkIdx );
		}
	}
}

//设置不使用音频输出。
void MyClntMediaPocsThrdCls::SetNotUseAdoOtpt()
{
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
	{
		if( ( p_TkbkInfoTmpPt->m_IsInit != 0 ) &&
			( ( p_TkbkInfoTmpPt->m_RmtTkbkMode & ClntMediaPocsThrd::TkbkModeAdoInpt ) != 0 ) )
		{
			SetNotUseAdoOtptStrm( p_TkbkInfoTmpPt->m_TkbkIdx );
		}
	}
}

//设置要使用音频输出流。
void MyClntMediaPocsThrdCls::SetToUseAdoOtptStrm( int TkbkIdx )
{
	//音频输出添加流。
    m_MediaPocsThrdClsPt->AdoOtptAddStrm( 0, TkbkIdx, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置音频输出是否使用Pcm原始数据。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoOtptSetStrmUsePcm( 0, TkbkIdx, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输出是否使用Speex解码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
	{
		int32_t p_DecdIsUsePrcplEnhsmt;

		p_DecdIsUsePrcplEnhsmt = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId ) == BST_CHECKED );

		m_MediaPocsThrdClsPt->AdoOtptSetStrmUseSpeexDecd( 0, TkbkIdx, p_DecdIsUsePrcplEnhsmt, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输出是否使用Opus解码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->AdoOtptSetStrmUseOpusDecd( 0, TkbkIdx, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置音频输出流是否使用。
	m_MediaPocsThrdClsPt->AdoOtptSetStrmIsUse( 0, TkbkIdx, 1, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
}

//设置不使用音频输出流。
void MyClntMediaPocsThrdCls::SetNotUseAdoOtptStrm( int TkbkIdx )
{
    m_MediaPocsThrdClsPt->AdoOtptDelStrm( 1, 0, TkbkIdx, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt ); //删除流操作需要立即执行，因为要防止中途出现其他消息导致重复删除流。
}

//设置要使用视频输入。
void MyClntMediaPocsThrdCls::SetToUseVdoInpt()
{
	wchar_t p_TmpU16strPt[ 1024 ];

	//设置视频输入。
	Vstr * p_TitleVstrPt;
	AvstrFmtCpy( p_TitleVstrPt, Utf16, Cu8vstr( "视频输入预览%z32d" ), m_ClntMediaPocsThrdPt->m_TkbkClnt.m_MyTkbkIdx );
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId ) == BST_CHECKED ) //如果要使用预设的帧的大小。
	{
		m_MediaPocsThrdClsPt->SetVdoInpt( 0,
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 0 ) ? 120 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 1 ) ? 240 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 2 ) ? 480 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 3 ) ? 960 : 0,
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 0 ) ? 160 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 1 ) ? 320 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 2 ) ? 640 :
										  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 3 ) ? 1280 : 0,
										  0, 0, 0,
										  SendVdoInptOtptWndInitMsg( p_TitleVstrPt ),
										  ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}
	else //如果要使用其他的帧的大小。
	{
		int32_t p_VdoFrmSzOtherWidth;
		int32_t p_VdoFrmSzOtherHeight;

		GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_VdoFrmSzOtherWidth = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_VdoFrmSzOtherHeight = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->SetVdoInpt( 0,
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
										  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
										  p_VdoFrmSzOtherWidth,
										  p_VdoFrmSzOtherHeight,
										  0, 0, 0,
										  SendVdoInptOtptWndInitMsg( p_TitleVstrPt ),
										  ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置视频输入是否使用Yu12原始数据。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYu12RdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->VdoInptSetUseYu12( 0, ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置视频输入是否使用OpenH264编码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
	{
		int32_t p_OpenH264EncdVdoType;
		int32_t p_OpenH264EncdEncdBitrate;
		int32_t p_OpenH264EncdBitrateCtrlMode;
		int32_t p_OpenH264EncdIDRFrmIntvl;
		int32_t p_OpenH264EncdCmplxt;

		GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_OpenH264EncdVdoType = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_OpenH264EncdEncdBitrate = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_OpenH264EncdBitrateCtrlMode = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_OpenH264EncdIDRFrmIntvl = _wtoi( p_TmpU16strPt );
		GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
		p_OpenH264EncdCmplxt = _wtoi( p_TmpU16strPt );

		m_MediaPocsThrdClsPt->VdoInptSetUseOpenH264Encd( 0,
														 p_OpenH264EncdVdoType,
														 p_OpenH264EncdEncdBitrate * 1024 * 8,
														 p_OpenH264EncdBitrateCtrlMode,
														 p_OpenH264EncdIDRFrmIntvl,
														 p_OpenH264EncdCmplxt,
														 ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
	}

	//设置视频输入使用的设备。
	m_MediaPocsThrdClsPt->VdoInptSetUseDvc( 0,
											SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
											( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );

	//设置视频输入是否黑屏。
	m_MediaPocsThrdClsPt->VdoInptSetIsBlack( 0,
											 ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
											 ( VstrCls * )&m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_ErrInfoVstrPt );
}

//设置不使用视频输入。
void MyClntMediaPocsThrdCls::SetNotUseVdoInpt()
{
	SendVdoInptOtptWndDstoyMsg( m_MediaPocsThrdClsPt->m_MediaPocsThrdPt->m_VdoInpt.m_Dvc.m_PrvwWndHdl );
}

//设置要使用视频输出。
void MyClntMediaPocsThrdCls::SetToUseVdoOtpt()
{
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
	{
		if( ( p_TkbkInfoTmpPt->m_IsInit != 0 ) &&
			( ( p_TkbkInfoTmpPt->m_RmtTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) )
		{
			SetToUseVdoOtptStrm( p_TkbkInfoTmpPt->m_TkbkIdx );
		}
	}
}

//设置不使用视频输出。
void MyClntMediaPocsThrdCls::SetNotUseVdoOtpt()
{
	TkbkClnt::TkbkInfo * p_TkbkInfoTmpPt;

	for( size_t p_TkbkInfoIdx = 0; CQueueGetByNum( m_ClntMediaPocsThrdPt->m_TkbkClnt.m_TkbkInfoCntnrPt, p_TkbkInfoIdx, NULL, ( void * * )&p_TkbkInfoTmpPt, 0, 0, NULL ) == 0; p_TkbkInfoIdx++ )
	{
		if( ( p_TkbkInfoTmpPt->m_IsInit != 0 ) &&
			( ( p_TkbkInfoTmpPt->m_RmtTkbkMode & ClntMediaPocsThrd::TkbkModeVdoInpt ) != 0 ) )
		{
			SetNotUseVdoOtptStrm( p_TkbkInfoTmpPt->m_TkbkIdx );
		}
	}
}

//设置要使用视频输出流。
void MyClntMediaPocsThrdCls::SetToUseVdoOtptStrm( int TkbkIdx )
{
	Vstr * p_TitleVstrPt;
	AvstrFmtCpy( p_TitleVstrPt, Utf16, Cu8vstr( "视频输出显示%z32d" ), TkbkIdx );

	//设置视频输出。
	m_MediaPocsThrdClsPt->VdoOtptAddStrm( 0, TkbkIdx, NULL );
	m_MediaPocsThrdClsPt->VdoOtptSetStrm( 0,
										  TkbkIdx,
										  SendVdoInptOtptWndInitMsg( p_TitleVstrPt ),
										  &g_ErrInfoVstr );

	//设置视频输出是否使用Yu12原始数据。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYu12RdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->VdoOtptSetStrmUseYu12( 0, TkbkIdx, &g_ErrInfoVstr );
	}

	//设置视频输出是否使用OpenH264解码器。
	if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
	{
		m_MediaPocsThrdClsPt->VdoOtptSetStrmUseOpenH264Decd( 0, TkbkIdx, 0, &g_ErrInfoVstr );
	}

	//设置视频输出是否黑屏。
	m_MediaPocsThrdClsPt->VdoOtptSetStrmIsBlack( 0,
												 TkbkIdx,
												 ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
												 &g_ErrInfoVstr );

	//设置视频输出流是否使用。
	m_MediaPocsThrdClsPt->VdoOtptSetStrmIsUse( 0, TkbkIdx, 1, &g_ErrInfoVstr );
}

//设置不使用视频输出流。
void MyClntMediaPocsThrdCls::SetNotUseVdoOtptStrm( int TkbkIdx )
{
	VdoOtpt::Strm * p_StrmPt;
	
	for( size_t p_StrmNum = 0; m_ClntMediaPocsThrdPt->m_MediaPocsThrdPt->m_VdoOtpt.m_StrmCntnr.GetByNum( p_StrmNum, &p_StrmPt, NULL, 0, 0, NULL ) == 0; p_StrmNum++ )
	{
		if( p_StrmPt->m_Idx == TkbkIdx )
		{
			SendVdoInptOtptWndDstoyMsg( p_StrmPt->m_Dvc.m_DspyWndHdl );
            break;
		}
	}
	m_MediaPocsThrdClsPt->VdoOtptDelStrm( 1, 0, TkbkIdx, NULL ); //删除流操作需要立即执行，因为要防止中途出现其他消息导致重复删除流。
}
