#pragma once

#include "resource.h"
#include "Func.h"
#include "MySrvrThrd.h"
#include "MyClntMediaPocsThrd.h"

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

	MainDlgWndMsgTypAdoInptDvcChg, //音频输入设备改变。
	MainDlgWndMsgTypAdoOtptDvcChg, //音频输出设备改变。
	MainDlgWndMsgTypVdoInptDvcChg, //视频输入设备改变。
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
	Vstr * m_Txt3VstrPt;
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
	Vstr * m_Txt3VstrPt;
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

void RefreshAdoVdoInptOtptDvc( int IsRefresAdoInptDvc, int IsRefresAdoOtptDvc, int IsRefresVdoInptDvc );

HWND SendVdoInptOtptWndInitMsg( Vstr * TitleVstrPt );
void SendVdoInptOtptWndDstoyMsg( HWND DspyTxtWndHdl );
void SendVdoInptOtptWndSetTitleMsg( HWND DspyTxtWndHdl, Vstr * TitleVstrPt );

//全局变量。
extern HINSTANCE g_IstnsHdl; //存放当前实例的句柄。
extern VstrCls g_ErrInfoVstr; //存放错误信息动态字符串的指针。
extern MySrvrThrdCls * g_MySrvrThrdPt; //存放我的服务端线程的指针。
extern MyClntMediaPocsThrdCls * g_MyClntMediaPocsThrdPt; //存放我的客户端媒体处理线程的指针。
extern HWND g_MainDlgWndHdl; //存放主对话框窗口的句柄。
extern long g_MainDlgWndMinHeight; //存放主对话框窗口的最小高度，单位为像素。
extern long g_MainDlgWndMinWidth; //存放主对话框窗口的最小宽度，单位为像素。
extern HWND g_SrvrStngDlgWndHdl; //存放服务端设置对话框窗口的句柄。
extern HWND g_CnctLstWndHdl; //存放连接列表窗口的句柄。
extern HWND g_ClntStngDlgWndHdl; //存放客户端设置对话框窗口的句柄。
extern HWND g_ClntLstWndHdl; //存放客户端列表窗口的句柄。
extern HWND g_PtbBtnWndHdl; //存放一键即按即广播按钮窗口的句柄。
extern HWND g_AdoInptDvcCbBoxWndHdl; //存放音频输入设备组合框窗口的句柄。
extern HWND g_AdoOtptDvcCbBoxWndHdl; //存放音频输出设备组合框窗口的句柄。
extern HWND g_VdoInptDvcCbBoxWndHdl; //存放视频输入设备组合框窗口的句柄。
extern HWND g_LogLstBoxWndHdl; //存放日志列表框窗口的句柄。
extern HWND g_VdoInptOtptTxtWndHdl; //存放视频输入输出文本框窗口的句柄。
extern CQueueCls g_VdoInptOtptWndCntnr; //存放视频输入输出窗口容器。
extern long g_VdoTxtWndLeftMargin; //存放视频文本框窗口的左边距，单位为像素。
extern long g_VdoTxtWndTopMargin; //存放视频文本框窗口的顶边距，单位为像素。
extern long g_VdoTxtWndRightMargin; //存放视频文本框窗口的右边距，单位为像素。
extern long g_VdoTxtWndBottomMargin; //存放视频文本框窗口的底边距，单位为像素。
extern int g_VdoWndShowMode; //存放视频窗口的显示模式，为0表示正常，为1表示垂直最大化排列，为2表示水平最大化排列。
extern HWND g_PttDlgWndHdl; //存放一键即按即通对话框窗口的句柄。
extern HWND g_PttBtnWndHdl; //存放一键即按即通按钮窗口的句柄。

extern HWND g_StngDlgWndHdl; //存放设置对话框窗口的句柄。
extern HWND g_AjbStngDlgWndHdl; //存放自适应抖动缓冲器设置对话框窗口的句柄。
extern HWND g_SaveStsToTxtFileStngDlgWndHdl; //存放保存状态到Txt文件设置对话框窗口的句柄。
extern HWND g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl; //存放保存音视频输入输出到Avi文件设置对话框窗口的句柄。
extern HWND g_SpeexAecStngDlgWndHdl; //存放Speex声学回音消除器设置对话框窗口的句柄。
extern HWND g_WebRtcAecmStngDlgWndHdl; //存放WebRtc定点版声学回音消除器设置对话框窗口的句柄。
extern HWND g_WebRtcAecStngDlgWndHdl; //存放WebRtc浮点版声学回音消除器设置对话框窗口的句柄。
extern HWND g_WebRtcAec3StngDlgWndHdl; //存放WebRtc第三版声学回音消除器设置对话框窗口的句柄。
extern HWND g_SpeexWebRtcAecStngDlgWndHdl; //存放SpeexWebRtc三重声学回音消除器设置对话框窗口的句柄。
extern HWND g_SpeexPrpocsNsStngDlgWndHdl; //存放Speex预处理器的噪音抑制设置对话框窗口的句柄。
extern HWND g_WebRtcNsxStngDlgWndHdl; //存放WebRtc定点版噪音抑制器设置对话框窗口的句柄。
extern HWND g_WebRtcNsStngDlgWndHdl; //存放WebRtc浮点版噪音抑制器设置对话框窗口的句柄。
extern HWND g_SpeexPrpocsStngDlgWndHdl; //存放Speex预处理器的设置对话框窗口的句柄。
extern HWND g_SpeexCodecStngDlgWndHdl; //存放Speex编解码器设置对话框窗口的句柄。
extern HWND g_SaveAdoInptOtptToWaveFileStngDlgWndHdl; //存放保存音频输入输出到Wave文件设置对话框窗口的句柄。
extern HWND g_OpenH264CodecStngDlgWndHdl; //存放OpenH264编解码器设置对话框窗口的句柄。

#define CbBoxGetCurSelTxt( CbBoxWndHdl, TxtU16strPt ) \
{ \
	long p_CurSelIdx; \
	p_CurSelIdx = SendMessage( CbBoxWndHdl, CB_GETCURSEL, 0, 0 ); \
	if( p_CurSelIdx != CB_ERR ) \
	{ \
		TxtU16strPt = ( wchar_t * )alloca( ( SendMessage( CbBoxWndHdl, CB_GETLBTEXTLEN, p_CurSelIdx, NULL ) + 1 ) * 2 ); \
		SendMessage( CbBoxWndHdl, CB_GETLBTEXT, p_CurSelIdx, ( LPARAM )TxtU16strPt ); \
	} \
}