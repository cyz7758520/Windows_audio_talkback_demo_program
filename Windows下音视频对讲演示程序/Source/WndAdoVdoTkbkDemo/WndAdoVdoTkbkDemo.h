#pragma once

#include "resource.h"
#include "Func.h"
#include "NtwkMediaPocsThrd.h"

typedef enum MainDlgWndMsgTyp //主对话框窗口消息。
{
	MainDlgWndMsgTypMyNtwkMediaPocsThrdInit = WM_USER + 101, //我的网络媒体处理线程初始化。
    MainDlgWndMsgTypMyNtwkMediaPocsThrdDstoy, //我的网络媒体处理线程销毁。
    MainDlgWndMsgTypSrvrInit, //服务端初始化。
    MainDlgWndMsgTypSrvrDstoy, //服务端销毁。
    MainDlgWndMsgTypShowLog, //显示日志。
    MainDlgWndMsgTypShowToast, //显示Toast。
    MainDlgWndMsgTypCnctLstAddItem, //连接列表添加项目。
    MainDlgWndMsgTypCnctLstModifyItem, //连接列表修改项目。
    MainDlgWndMsgTypCnctLstDelItem, //连接列表删除项目。
    MainDlgWndMsgTypClntLstAddItem, //客户端列表添加项目。
    MainDlgWndMsgTypClntLstDelItem, //客户端列表删除项目。
} MainDlgWndMsgTyp;
typedef struct MainDlgWndMsgCnctLstAddItem
{
    int m_Num;
    int m_IsSrvrOrClntCnct;
    Vstr * m_RmtNodeNameVstrPt;
    Vstr * m_RmtNodeSrvcVstrPt;
} MainDlgWndMsgCnctLstAddItem;
typedef struct MainDlgWndMsgCnctLstModifyItem
{
    int m_Num;
    Vstr * m_SignVstrPt;
    Vstr * m_LclTkbkModeVstrPt;
    Vstr * m_RmtTkbkModeVstrPt;
} MainDlgWndMsgCnctLstModifyItem;
typedef struct MainDlgWndMsgClntLstAddItem
{
    Vstr * m_PrtclVstrPt;
    Vstr * m_RmtNodeNameVstrPt;
    Vstr * m_RmtNodeSrvcVstrPt;
} MainDlgWndMsgClntLstAddItem;
