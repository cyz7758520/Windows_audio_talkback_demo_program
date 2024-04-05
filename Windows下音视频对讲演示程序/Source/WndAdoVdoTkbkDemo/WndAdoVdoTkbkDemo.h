#pragma once

#include "resource.h"
#include "Func.h"

typedef enum MainDlgWndMsgTyp //主对话框窗口消息。
{
	MainDlgWndMsgTypShowLog = WM_USER + 101, //显示日志。
	MainDlgWndMsgTypShowToast, //显示Toast。

	MainDlgWndMsgTypMySrvrThrdInit, //我的服务端线程初始化。
	MainDlgWndMsgTypMySrvrThrdDstoy, //我的服务端线程销毁。
	MainDlgWndMsgTypSrvrInit, //服务端初始化。
	MainDlgWndMsgTypSrvrDstoy, //服务端销毁。

	MainDlgWndMsgTypMyClntMediaPocsThrdInit, //我的客户端媒体处理线程初始化。
	MainDlgWndMsgTypMyClntMediaPocsThrdDstoy, //我的客户端媒体处理线程销毁。
	MainDlgWndMsgTypTkbkClntCnctInit, //对讲客户端连接初始化。
	MainDlgWndMsgTypTkbkClntCnctDstoy, //对讲客户端连接销毁。

	MainDlgWndMsgTypCnctLstAddItem, //连接列表添加项目。
	MainDlgWndMsgTypCnctLstDelItem, //连接列表删除项目。
	MainDlgWndMsgTypCnctLstModifyItem, //连接列表修改项目。

	MainDlgWndMsgTypClntLstAddItem, //客户端列表添加项目。
	MainDlgWndMsgTypClntLstDelItem, //客户端列表删除项目。
	MainDlgWndMsgTypClntLstModifyItem, //客户端列表修改项目。

	MainDlgWndMsgTypVdoInptOtptWndInit, //视频输入输出窗口初始化。
	MainDlgWndMsgTypVdoInptOtptWndDstoy, //视频输入输出窗口销毁。
	MainDlgWndMsgTypVdoInptOtptWndSetTitle, //视频输入输出窗口设置标题。
} MainDlgWndMsgTyp;
typedef struct MainDlgWndMsgCnctLstAddItem
{
	int m_IsTcpOrAudpPrtcl;
	Vstr * m_RmtNodeNameVstrPt;
	Vstr * m_RmtNodeSrvcVstrPt;
} MainDlgWndMsgCnctLstAddItem;
typedef struct MainDlgWndMsgCnctLstModifyItem
{
	int m_Num;
	Vstr * m_Txt1VstrPt;
	Vstr * m_Txt2VstrPt;
} MainDlgWndMsgCnctLstModifyItem;
typedef struct MainDlgWndMsgClntLstAddItem
{
	Vstr * m_PrtclVstrPt;
	Vstr * m_RmtNodeNameVstrPt;
	Vstr * m_RmtNodeSrvcVstrPt;
} MainDlgWndMsgClntLstAddItem;
typedef struct MainDlgWndMsgClntLstModifyItem
{
	int m_Num;
	Vstr * m_Txt1VstrPt;
	Vstr * m_Txt2VstrPt;
} MainDlgWndMsgClntLstModifyItem;
typedef struct MainDlgWndMsgVdoInptOtptWndInit
{
	Vstr * m_TitleVstrPt;
	HWND m_DspyTxtWndHdl;
} MainDlgWndMsgVdoInptOtptWndInit;
typedef struct MainDlgWndMsgVdoInptOtptWndDstoy
{
	HWND m_DspyTxtWndHdl;
} MainDlgWndMsgVdoInptOtptWndDstoy;
typedef struct MainDlgWndMsgVdoInptOtptWndSetTitle
{
	HWND m_DspyTxtWndHdl;
	Vstr * m_TitleVstrPt;
} MainDlgWndMsgVdoInptOtptWndSetTitle;

HWND SendVdoInptOtptWndInitMsg( Vstr * TitleStrPt );
void SendVdoInptOtptWndDstoyMsg( HWND DspyTxtWndHdl );
void SendVdoInptOtptWndSetTitleMsg( HWND DspyTxtWndHdl, Vstr * TitleStrPt );