#pragma once

#include "Func.h"
#include "SrvrThrd.h"

class MySrvrThrdCls : public SrvrThrdCls //我的服务端线程线程。
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	
	MySrvrThrdCls( HWND MainDlgWndHdl ) { m_MainDlgWndHdl = MainDlgWndHdl; }
	~MySrvrThrdCls() {}
	
	int Init( const void * LicnCodePt );
	void Dstoy();

	//用户定义的显示日志函数。
	void UserShowLog( VstrCls * InfoVstrPt );
	
	//用户定义的显示Toast函数。
	void UserShowToast( VstrCls * InfoVstrPt );
	
	//用户定义的消息函数。
	int UserMsg( unsigned int MsgTyp, void * MsgPt, size_t MsgLenByt );
	
	//用户定义的服务端线程初始化函数。
	void UserSrvrThrdInit();

	//用户定义的服务端线程销毁函数。
	void UserSrvrThrdDstoy();
	
	//用户定义的服务端初始化函数。
	void UserSrvrInit();

	//用户定义的服务端销毁函数。
	void UserSrvrDstoy();
	
	//用户定义的连接初始化函数。
	void UserCnctInit( SrvrThrd::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的连接销毁函数。
	void UserCnctDstoy( SrvrThrd::CnctInfo * CnctInfoPt );

	//用户定义的连接状态函数。
	void UserCnctSts( SrvrThrd::CnctInfo * CnctInfoPt, int32_t CurCnctSts );
	
	//用户定义的连接远端对讲模式函数。
	void UserCnctRmtTkbkMode( SrvrThrd::CnctInfo * CnctInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode );
	
	//用户定义的连接测试网络延迟函数。
	void UserCnctTstNtwkDly( SrvrThrd::CnctInfo * CnctInfoPt, uint64_t NtwkDlyMsec );
};
