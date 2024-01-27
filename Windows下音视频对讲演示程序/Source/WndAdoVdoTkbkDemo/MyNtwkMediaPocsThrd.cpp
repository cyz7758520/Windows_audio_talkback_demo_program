#include "MyNtwkMediaPocsThrd.h"
#include "WndAdoVdoTkbkDemo.h"

//用户定义的网络媒体处理线程初始化函数。
void MyNtwkMediaPocsThrdCls::UserNtwkMediaPocsThrdInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMyNtwkMediaPocsThrdInit, 0, 0 );
}

//用户定义的网络媒体处理线程销毁函数。
void MyNtwkMediaPocsThrdCls::UserNtwkMediaPocsThrdDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMyNtwkMediaPocsThrdDstoy, 0, 0 );
}

//用户定义的服务端初始化函数。
void MyNtwkMediaPocsThrdCls::UserSrvrInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypSrvrInit, 0, 0 );
}

//用户定义的服务端销毁函数。
void MyNtwkMediaPocsThrdCls::UserSrvrDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypSrvrDstoy, 0, 0 );
}

//用户定义的显示日志函数。
void MyNtwkMediaPocsThrdCls::UserShowLog( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的显示Toast函数。
void MyNtwkMediaPocsThrdCls::UserShowToast( Vstr * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowToast, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的连接初始化函数。
void MyNtwkMediaPocsThrdCls::UserCnctInit( TkbkNtwk::CnctInfo * CnctInfoPt, int IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt )
{
	MainDlgWndMsgCnctLstAddItem * p_MainDlgWndMsgCnctLstAddItemPt = ( MainDlgWndMsgCnctLstAddItem * )malloc( sizeof( MainDlgWndMsgCnctLstAddItem ) );

	p_MainDlgWndMsgCnctLstAddItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstAddItemPt->m_IsSrvrOrClntCnct = IsSrvrOrClntCnct;
	VstrInit( &p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt, Utf16, , RmtNodeNameVstrPt );
	VstrInit( &p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt, Utf16, , RmtNodeSrvcVstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstAddItem, ( WPARAM )p_MainDlgWndMsgCnctLstAddItemPt, 0 );
}

//用户定义的连接状态函数。
void MyNtwkMediaPocsThrdCls::UserCnctSts( TkbkNtwk::CnctInfo * CnctInfoPt, int CurCnctSts )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Utf16, ,  );
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt, Utf16, ,  );
	if( CurCnctSts == NtwkMediaPocsThrd::CnctStsWait )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "等待远端接受连接" ), ,  );
	}
	else if( CurCnctSts < NtwkMediaPocsThrd::CnctStsWait )
	{
		VstrFmtCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "第%d次连接" ), -CurCnctSts );
	}
	else if( CurCnctSts == NtwkMediaPocsThrd::CnctStsCnct )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "已连接" ), ,  );
	}
	else if( CurCnctSts == NtwkMediaPocsThrd::CnctStsTmot )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "异常断开" ), ,  );
	}
	else if( CurCnctSts == NtwkMediaPocsThrd::CnctStsDsct )
	{
		VstrCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "已断开" ), ,  );
	}
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接激活函数。
void MyNtwkMediaPocsThrdCls::UserCnctAct( TkbkNtwk::CnctInfo * CnctInfoPt, int IsAct )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt, Utf16, , ( IsAct != 0 ) ? Cu8vstr( "⇶" ) : Cu8vstr( "" ) );
	p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt = NULL;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接本端对讲模式函数。
void MyNtwkMediaPocsThrdCls::UserCnctLclTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int LclTkbkMode )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Utf16, , );
	VstrFmtCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt, Cu8vstr( "%z8s%z8s" ), ( IsRqirAct != 0  ) ? "请求激活 " : "", g_TkbkModeU8strArrPt[ LclTkbkMode ] );
	p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt = NULL;
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接远端对讲模式函数。
void MyNtwkMediaPocsThrdCls::UserCnctRmtTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int RmtTkbkMode )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt = NULL;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt, Utf16, , );
	VstrFmtCpy( p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt, Cu8vstr( "%z8s%z8s" ), ( IsRqirAct != 0  ) ? "请求激活 " : "", g_TkbkModeU8strArrPt[ RmtTkbkMode ] );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接销毁函数。
void MyNtwkMediaPocsThrdCls::UserCnctDstoy( TkbkNtwk::CnctInfo * CnctInfoPt )
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstDelItem, ( WPARAM )CnctInfoPt->m_Num, 0 );
}
