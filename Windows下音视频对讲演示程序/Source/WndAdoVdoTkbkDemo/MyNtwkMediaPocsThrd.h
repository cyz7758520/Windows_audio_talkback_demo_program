#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "NtwkMediaPocsThrd.h"

class MyNtwkMediaPocsThrdCls : public NtwkMediaPocsThrdCls //我的网络媒体处理线程。
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	
	MyNtwkMediaPocsThrdCls( HWND MainDlgWndHdl ) { m_MainDlgWndHdl = MainDlgWndHdl; }
	~MyNtwkMediaPocsThrdCls() {}
	
	//用户定义的网络媒体处理线程初始化函数。
	void UserNtwkMediaPocsThrdInit();

	//用户定义的网络媒体处理线程销毁函数。
	void UserNtwkMediaPocsThrdDstoy();
	
	//用户定义的服务端初始化函数。
	void UserSrvrInit();

	//用户定义的服务端销毁函数。
	void UserSrvrDstoy();
	
	//用户定义的显示日志函数。
	void UserShowLog( Vstr * InfoVstrPt );

	//用户定义的显示Toast函数。
	void UserShowToast( Vstr * InfoVstrPt );
	
	//用户定义的连接初始化函数。
	void UserCnctInit( TkbkNtwk::CnctInfo * CnctInfoPt, int IsSrvrOrClntCnct, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的连接状态函数。
	void UserCnctSts( TkbkNtwk::CnctInfo * CnctInfoPt, int CurCnctSts );
	
	//用户定义的连接激活函数。
	void UserCnctAct( TkbkNtwk::CnctInfo * CnctInfoPt, int IsAct );

	//用户定义的连接本端对讲模式函数。
	void UserCnctLclTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int LclTkbkMode );
	
	//用户定义的连接远端对讲模式函数。
	void UserCnctRmtTkbkMode( TkbkNtwk::CnctInfo * CnctInfoPt, int IsRqirAct, int RmtTkbkMode );
	
	//用户定义的连接销毁函数。
	void UserCnctDstoy( TkbkNtwk::CnctInfo * CnctInfoPt );
};
