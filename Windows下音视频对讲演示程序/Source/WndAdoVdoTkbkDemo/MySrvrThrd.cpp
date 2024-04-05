#include "MySrvrThrd.h"
#include "WndAdoVdoTkbkDemo.h"

//用户定义的显示日志函数。
void MySrvrThrdCls::UserShowLog( VstrCls * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt->m_VstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的显示Toast函数。
void MySrvrThrdCls::UserShowToast( VstrCls * InfoVstrPt )
{
	Vstr * p_InfoVstrPt;

	VstrInit( &p_InfoVstrPt, Utf16, , InfoVstrPt->m_VstrPt );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypShowToast, ( WPARAM )p_InfoVstrPt, 0 );
}

//用户定义的消息函数。
int MySrvrThrdCls::UserMsg( unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt )
{
	return 0;
}

//用户定义的服务端线程初始化函数。
void MySrvrThrdCls::UserSrvrThrdInit()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMySrvrThrdInit, 0, 0 );
}

//用户定义的服务端线程销毁函数。
void MySrvrThrdCls::UserSrvrThrdDstoy()
{
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypMySrvrThrdDstoy, 0, 0 );
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
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}

//用户定义的连接远端对讲模式函数。
void MySrvrThrdCls::UserCnctRmtTkbkMode( SrvrThrd::CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode )
{
	MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )malloc( sizeof( MainDlgWndMsgCnctLstModifyItem ) );

	p_MainDlgWndMsgCnctLstModifyItemPt->m_Num = CnctInfoPt->m_Num;
	p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt = NULL;
	VstrInit( &p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt, Utf16, , Cu8vstr( g_TkbkModeU8strArrPt[ NewRmtTkbkMode ] ) );
	PostMessage( m_MainDlgWndHdl, MainDlgWndMsgTypCnctLstModifyItem, ( WPARAM )p_MainDlgWndMsgCnctLstModifyItemPt, 0 );
}
