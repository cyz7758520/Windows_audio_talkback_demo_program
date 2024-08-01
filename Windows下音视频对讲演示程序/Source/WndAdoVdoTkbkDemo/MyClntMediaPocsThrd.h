#pragma once

#include "Func.h"
#include "DataStruct.h"
#include "Sokt.h"
#include "Ajb.h"
#include "ClntMediaPocsThrd.h"

class MyClntMediaPocsThrdCls : public ClntMediaPocsThrdCls //我的客户端媒体处理线程。
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	int32_t m_TkbkClntNum; //存放对讲客户端的序号。
	
	MyClntMediaPocsThrdCls( HWND MainDlgWndHdl ) { m_MainDlgWndHdl = MainDlgWndHdl; }
	~MyClntMediaPocsThrdCls() {}
	
	int Init();
	void Dstoy();
	int TkbkInit();
	void TkbkDstoy();
	int BdctInit();
	void BdctDstoy();

	//用户定义的显示日志函数。
	void UserShowLog( Vstr * InfoVstrPt );

	//用户定义的显示Toast函数。
	void UserShowToast( Vstr * InfoVstrPt );
	
	//用户定义的客户端媒体处理线程初始化函数。
	void UserClntMediaPocsThrdInit();

	//用户定义的客户端媒体处理线程销毁函数。
	void UserClntMediaPocsThrdDstoy();
	
	//用户定义的对讲客户端连接初始化函数。
	void UserTkbkClntCnctInit( int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的对讲客户端连接销毁函数。
	void UserTkbkClntCnctDstoy();
	
	//用户定义的对讲客户端连接状态函数。
	void UserTkbkClntCnctSts( int32_t CurCnctSts );
	
	//用户定义的对讲客户端我的对讲索引函数。
	void UserTkbkClntMyTkbkIdx( int32_t MyTkbkIdx );
	
	//用户定义的对讲客户端本端对讲模式函数。
	void UserTkbkClntLclTkbkMode( int32_t OldLclTkbkMode, int32_t NewLclTkbkMode );
	
	//用户定义的对讲客户端对讲信息初始化函数。
	void UserTkbkClntTkbkInfoInit( TkbkClnt::TkbkInfo * TkbkInfoPt );
	
	//用户定义的对讲客户端对讲信息销毁函数。
	void UserTkbkClntTkbkInfoDstoy( TkbkClnt::TkbkInfo * TkbkInfoPt );
	
	//用户定义的对讲客户端对讲信息远端对讲模式函数。
	void UserTkbkClntTkbkInfoRmtTkbkMode( TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode );
	
	//用户定义的对讲客户端测试网络延迟函数。
	void UserTkbkClntTstNtwkDly( uint64_t NtwkDlyMsec );

	//用户定义的广播客户端初始化函数。
	void UserBdctClntInit();

	//用户定义的广播客户端销毁函数。
	void UserBdctClntDstoy();
	
	//用户定义的广播客户端连接初始化函数。
	void UserBdctClntCnctInit( BdctClnt::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的广播客户端连接销毁函数。
	void UserBdctClntCnctDstoy( BdctClnt::CnctInfo * CnctInfoPt );
	
	//用户定义的广播客户端连接状态函数。
	void UserBdctClntCnctSts( BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts );

	void SetToUseAdoInpt();
	void SetNotUseAdoInpt();
	void SetToUseAdoOtpt();
	void SetNotUseAdoOtpt();
	void SetToUseAdoOtptStrm( int TkbkIdx );
	void SetNotUseAdoOtptStrm( int TkbkIdx );
	void SetToUseVdoInpt();
	void SetNotUseVdoInpt();
	void SetToUseVdoOtpt();
	void SetNotUseVdoOtpt();
	void SetToUseVdoOtptStrm( int TkbkIdx );
	void SetNotUseVdoOtptStrm( int TkbkIdx );
};
