// WndAdoVdoTkbkDemo.cpp : 定义应用程序的入口点。
//
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Func.h"
#include "WndAdoVdoTkbkDemo.h"
#include "Queue.h"
#include "LnkLst.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaPocsThrd.h"
#include "LibYUV.h"
#ifdef __DEBUG__
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define WM_INIT_MEDIA_PROC_THREAD    WM_USER + 101  //初始化媒体处理线程的消息。
#define WM_DSTRY_MEDIA_PROC_THREAD   WM_USER + 102  //销毁媒体处理线程的消息。
#define WM_SHOW_RQST_CNCT_DIALOG     WM_USER + 103  //显示请求连接对话框的消息。
#define WM_DSTRY_RQST_CNCT_DIALOG    WM_USER + 104  //销毁请求连接对话框的消息。
#define WM_SHOW_LOG                  WM_USER + 105  //显示日志的消息。
#define WM_REPAINT                   WM_USER + 106  //重绘消息。

//存放媒体信息。
typedef struct MediaInfo
{
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。

	Vstr * m_IPAddrVstrPt; //存放IP地址动态字符串的指针。
    Vstr * m_PortVstrPt; //存放端口动态字符串的指针。
	int m_XfrMode; //存放传输模式，为0表示实时半双工（一键通），为1表示实时全双工。
	int m_PttBtnIsDown; //存放一键即按即通按钮是否按下，为0表示弹起，为1表示按下。
	int m_MaxCnctTimes; //存放最大连接次数，取值区间为[1,2147483647]。
    int m_UseWhatXfrPrtcl; //存放使用什么传输协议，为0表示TCP协议，为1表示UDP协议。
    int m_IsCreateSrvrOrClnt; //存放创建服务端或者客户端标记，为1表示创建服务端，为0表示创建客户端。
    TcpSrvrSoktCls m_TcpSrvrSokt; //存放本端TCP协议服务端套接字。
    TcpClntSoktCls m_TcpClntSokt; //存放本端TCP协议客户端套接字。
	AudpSoktCls m_AudpSokt; //存放本端高级UDP协议套接字。
	size_t m_AudpCnctIdx; //存放本端高级UDP协议连接索引。
	#define PKT_TYP_ALLOW_CNCT  1 //数据包类型：允许连接包。
	#define PKT_TYP_REFUSE_CNCT 2 //数据包类型：拒绝连接包。
	#define PKT_TYP_ADO_FRM     3 //数据包类型：音频输入输出帧。
	#define PKT_TYP_VDO_FRM     4 //数据包类型：视频输入输出帧。
	#define PKT_TYP_EXIT        5 //数据包类型：退出包。
	
	int m_IsAutoAllowCnct; //存放是否自动允许连接，为0表示手动，为1表示自动。
	int m_RqstCnctRslt; //存放请求连接的结果，为0表示没有选择，为1表示允许，为2表示拒绝。

    int m_LastSendAdoInptFrmIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
    uint32_t m_LastSendAdoInptFrmTimeStamp; //存放最后一个发送音频输入帧的时间戳。
    uint32_t m_LastSendVdoInptFrmTimeStamp; //存放最后一个发送视频输入帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOtptFrm; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。
    int m_LastGetAdoOtptFrmIsAct; //存放最后一个取出的音频输出帧是否为有语音活动，为0表示否，为非0表示是。
    uint32_t m_LastGetAdoOtptFrmVdoOtptFrmTimeStamp; //存放最后一个取出的音频输出帧对应视频输出帧的时间戳。

    VarLenLnkLstCls m_RecvAdoOtptFrmLnkLst; //存放接收音频输出帧链表。
	VarLenLnkLstCls m_RecvVdoOtptFrmLnkLst; //存放接收视频输出帧链表。

	AAjb * m_AAjbPt; //存放音频自适应抖动缓冲器的指针。
	int m_AAjbMinNeedBufFrmCnt; //存放音频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
	int m_AAjbMaxNeedBufFrmCnt; //存放音频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
	int m_AAjbMaxCntuLostFrmCnt; //音频自适应抖动缓冲器的最大连续丢失帧的数量，单位为个帧，取值区间为[1,2147483647]，当连续丢失帧的数量超过最大时，认为是对方中途暂停发送。
	float m_AAjbAdaptSensitivity; //存放音频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
	VAjb * m_VAjbPt; //存放视频自适应抖动缓冲器的指针。
	int m_VAjbMinNeedBufFrmCnt; //存放视频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
	int m_VAjbMaxNeedBufFrmCnt; //存放视频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
	float m_VAjbAdaptSensitivity; //存放视频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。

	int8_t * m_TmpBytePt; //存放临时数据的指针。
	size_t m_TmpByteSz; //存放临时数据的大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的指针。
	size_t m_TmpByte2Sz; //存放临时数据的大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的指针。
	size_t m_TmpByte3Sz; //存放临时数据的大小。
}MediaInfo;

//全局变量。
HINSTANCE g_IstnsHdl; //存放当前实例的句柄。
VstrCls g_ErrInfoVstr; //存放错误信息动态字符串的指针。
MediaPocsThrdCls g_MediaPocsThrd; //存放媒体处理线程的指针。
MediaInfo * g_MediaInfoPt = NULL; //存放媒体信息的指针。
HWND g_MainDlgWndHdl = NULL; //存放主对话框窗口的句柄。
HWND g_RqstCnctDlgWndHdl = NULL; //存放请求连接对话框窗口的句柄。
HWND g_PttDlgWndHdl = NULL; //存放一键即按即通对话框窗口的句柄。
HWND g_PttBtnWndHdl = NULL; //存放一键即按即通按钮窗口的句柄。
HWND g_XfrPrtclStngDlgWndHdl = NULL; //存放传输协议设置对话框窗口的句柄。
HWND g_StngDlgWndHdl = NULL; //存放设置对话框窗口的句柄。
HWND g_AjbStngDlgWndHdl = NULL; //存放自适应抖动缓冲器设置对话框窗口的句柄。
HWND g_SpeexAecStngDlgWndHdl = NULL; //存放Speex声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecmStngDlgWndHdl = NULL; //存放WebRtc定点版声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecStngDlgWndHdl = NULL; //存放WebRtc浮点版声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexWebRtcAecStngDlgWndHdl = NULL; //存放SpeexWebRtc三重声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexPrpocsNsStngDlgWndHdl = NULL; //存放Speex预处理器的噪音抑制设置对话框窗口的句柄。
HWND g_WebRtcNsxStngDlgWndHdl = NULL; //存放WebRtc定点版噪音抑制器设置对话框窗口的句柄。
HWND g_WebRtcNsStngDlgWndHdl = NULL; //存放WebRtc浮点版噪音抑制器设置对话框窗口的句柄。
HWND g_SpeexPrpocsOtherStngDlgWndHdl = NULL; //存放Speex预处理器的其他功能设置对话框窗口的句柄。
HWND g_SpeexCodecStngDlgWndHdl = NULL; //存放Speex编解码器设置对话框窗口的句柄。
HWND g_OpenH264CodecStngDlgWndHdl = NULL; //存放OpenH264编解码器设置对话框窗口的句柄。
HWND g_VdoInptPrvwTxtWndHdl = NULL; //存放视频输入预览文本框窗口的句柄。
RECT g_VdoInptPrvwTxtWndRect = { 0 }; //存放视频输入预览文本框窗口的位置。
int g_VdoInptPrvwTxtWndIsMax = 0; //存放视频输入预览文本框窗口是否最大化。
HWND g_VdoOtptDspyTxtWndHdl = NULL; //存放视频输出显示文本框窗口句柄。
RECT g_VdoOtptDspyTxtWndRect = { 0 }; //存放视频输出显示文本框窗口的位置。
int g_VdoOtptDspyTxtWndIsMax = 0; //存放视频输出显示文本框窗口是否最大化。

//用户定义的初始化函数，在本线程刚启动时回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	Vstr * p_LclNodeAddrVstrPt = NULL; //存放本地节点地址。
	Vstr * p_LclNodePortVstrPt = NULL; //存放本地节点端口。
	Vstr * p_RmtNodeAddrVstrPt = NULL; //存放远程节点地址。
	Vstr * p_RmtNodePortVstrPt = NULL; //存放远程节点端口。
	uint64_t p_TmpUint64;
	size_t p_TmpSz;
	
	PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_INIT_MEDIA_PROC_THREAD, 0, 0 ); //向主对话框发送初始化媒体处理线程的消息。

	g_MediaInfoPt->m_RqstCnctRslt = 0; //设置请求连接的结果为没有选择。
	g_MediaInfoPt->m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	if( g_MediaInfoPt->m_TmpBytePt == NULL )
	{
		g_MediaInfoPt->m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpBytePt == NULL )
		{
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			goto Out;
		}
		g_MediaInfoPt->m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据的大小。
	}
	if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte2Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
		{
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			goto Out;
		}
		g_MediaInfoPt->m_TmpByte2Sz = 1024 * 1024; //设置临时数据的大小。
	}
	if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte3Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
		{
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建临时数据的内存块失败。" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			goto Out;
		}
		g_MediaInfoPt->m_TmpByte3Sz = 1024 * 1024; //设置临时数据的大小。
	}
	
	if( VstrInit( &p_LclNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点地址动态字符串失败。原因：内存不足。" ) );
		LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		goto Out;
	}
	
	if( VstrInit( &p_LclNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建本端节点端口动态字符串失败。原因：内存不足。" ) );
		LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		goto Out;
	}
	
	if( VstrInit( &p_RmtNodeAddrVstrPt, , INET6_ADDRSTRLEN,  ) != 0 )
	{
		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点地址动态字符串失败。原因：内存不足。" ) );
		LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		goto Out;
	}
	
	if( VstrInit( &p_RmtNodePortVstrPt, , 6,  ) != 0 )
	{
		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "创建远端节点端口动态字符串失败。原因：内存不足。" ) );
		LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		goto Out;
	}

	if( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( g_MediaInfoPt->m_TcpSrvrSokt.Init( 4, g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt, 1, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议服务端套接字成功。
			{
				if( g_MediaInfoPt->m_TcpSrvrSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}

				VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			}
			else
			{
				VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
			}

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( g_MediaInfoPt->m_TcpSrvrSokt.Acpt( &g_MediaInfoPt->m_TcpClntSokt, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL ) //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						g_MediaInfoPt->m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。

						VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
						break;
					}
					else //如果用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}
		else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), g_MediaInfoPt->m_IPAddrVstrPt );
			WinExec( ( char * )g_MediaInfoPt->m_IPAddrVstrPt->m_StrPt, SW_HIDE );

			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端TCP协议服务端套接字。
			{
				//连接远端
				{
					{
						VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					}

					if( g_MediaInfoPt->m_TcpClntSokt.Init( 4, g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt, NULL, NULL, 5000, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字成功。
					{
						if( g_MediaInfoPt->m_TcpClntSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" ) );
							LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
							{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
							goto Out;
						}
						if( g_MediaInfoPt->m_TcpClntSokt.GetRmtAddr( NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
						{
							VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
							LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
							{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
							goto Out;
						}

						VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端TCP协议客户端套接字[%vs:%vs]，并连接远端TCP协议服务端套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
						break; //跳出重连。
					}
					else
					{
						VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端TCP协议客户端套接字，并连接远端TCP协议服务端套接字[%vs:%vs]失败。原因：" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
						LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > g_MediaInfoPt->m_MaxCnctTimes )
				{
					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
		}

		if( g_MediaInfoPt->m_TcpClntSokt.SetNoDelay( 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			goto Out;
		}

		if( g_MediaInfoPt->m_TcpClntSokt.SetSendBufSz( 1024 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的发送缓冲区大小失败。
        {
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的发送缓冲区大小失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            goto Out;
        }

		if( g_MediaInfoPt->m_TcpClntSokt.SetRecvBufSz( 1024 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的接收缓冲区大小失败。
        {
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的接收缓冲区大小失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            goto Out;
        }
		
		if( g_MediaInfoPt->m_TcpClntSokt.SetKeepAlive( 1, 1, 1, 5, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端TCP协议客户端套接字的保活机制失败。
        {
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端TCP协议客户端套接字的保活机制失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            goto Out;
        }
	}
	else //如果使用UDP协议。
    {
        if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端高级UDP协议套接字接受远端高级UDP协议套接字的连接。
        {
            if( g_MediaInfoPt->m_AudpSokt.Init( 4, g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt, 1, 5000, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
            {
                if( g_MediaInfoPt->m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
                {
					VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
                    goto Out;
                }

				VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            }
            else //如果初始化本端高级UDP协议套接字失败。
            {
				VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字[%s:%s]失败。原因：" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
                goto Out;
            }

            while( true ) //循环接受远端高级UDP协议套接字的连接。
            {
				if( g_MediaInfoPt->m_AudpSokt.Acpt( &g_MediaInfoPt->m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
				{
					if( g_MediaInfoPt->m_AudpCnctIdx != SIZE_MAX ) //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接成功。
					{
						VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字[%vs:%vs]的连接成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
						break;
					}
					else //如果用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字接受远端高级UDP协议套接字的连接失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
            }
        }
        else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端高级UDP协议套接字连接远端高级UDP协议套接字。
        {
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "ping -n 1 -w 1 %vs" ), g_MediaInfoPt->m_IPAddrVstrPt );
			WinExec( ( char * )g_MediaInfoPt->m_IPAddrVstrPt->m_StrPt, SW_HIDE );

            if( g_MediaInfoPt->m_AudpSokt.Init( 4, NULL, NULL, 0, 5000, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果初始化本端高级UDP协议套接字成功。
            {
                if( g_MediaInfoPt->m_AudpSokt.GetLclAddr( NULL, p_LclNodeAddrVstrPt, p_LclNodePortVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。
                {
					VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议套接字绑定的本地节点地址和端口失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
                    goto Out;
                }

				VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "初始化本端高级UDP协议套接字[%vs:%vs]成功。" ), p_LclNodeAddrVstrPt, p_LclNodePortVstrPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            }
            else //如果初始化本端高级UDP协议套接字失败。
            {
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化本端高级UDP协议套接字失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
                goto Out;
            }
			
			int p_CurCnctTimes = 1;
			while( true ) //循环连接远端高级UDP协议服务端套接字。
			{
				//连接远端。
				{
					{
						VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始第 %d 次连接。" ), p_CurCnctTimes );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					}

					if( g_MediaInfoPt->m_AudpSokt.Cnct( 4, g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt, &g_MediaInfoPt->m_AudpCnctIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果连接远端高级UDP协议套接字成功。
					{
						AudpCnctSts p_AudpCnctSts;

						if( g_MediaInfoPt->m_AudpSokt.WaitCnct( g_MediaInfoPt->m_AudpCnctIdx, UINT16_MAX, &p_AudpCnctSts, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) //如果等待本端高级UDP协议套接字连接远端是否成功成功。
						{
							if( p_AudpCnctSts == AudpCnctStsCnct ) //如果连接成功。
							{
								if( g_MediaInfoPt->m_AudpSokt.GetRmtAddr( g_MediaInfoPt->m_AudpCnctIdx, NULL, p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 )
								{
									VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "获取本端高级UDP协议客户端套接字连接的远端高级UDP协议客户端套接字绑定的远程节点地址和端口失败。原因：" ) );
									LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
									{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
									goto Out;
								}

								VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]成功。" ), p_RmtNodeAddrVstrPt, p_RmtNodePortVstrPt );
								LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
								{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
								break; //跳出重连。
							}
							else //如果连接失败。
							{
								if( p_AudpCnctSts == AudpCnctStsTmot ) //如果连接超时。
								{
									VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接超时。" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
									LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
									{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
								}
								else //如果连接断开。
								{
									VstrFmtCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：连接断开。" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
									LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
									{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
								}
							}
						}

						g_MediaInfoPt->m_AudpSokt.ClosCnct( g_MediaInfoPt->m_AudpCnctIdx, MediaPocsThrdPt->m_ErrInfoVstrPt ); //关闭连接，等待重连。
					}
					else
					{
						VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端高级UDP协议套接字连接远端高级UDP协议套接字[%vs:%vs]失败。原因：" ), g_MediaInfoPt->m_IPAddrVstrPt, g_MediaInfoPt->m_PortVstrPt );
						LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					}
				}
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > g_MediaInfoPt->m_MaxCnctTimes )
				{
					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "达到最大连接次数，中断连接。" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( Cu8vstr( "本线程接收到退出请求，开始准备退出。" ) );
					goto Out;
				}
			}
        }
		
		if( g_MediaInfoPt->m_AudpSokt.SetSendBufSz( 1024 * 1024, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的发送缓冲区大小失败。
        {
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的发送缓冲区大小失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            goto Out;
        }

		if( g_MediaInfoPt->m_AudpSokt.SetRecvBufSz( 1024 * 1024, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) //如果设置本端高级UDP协议套接字的接收缓冲区大小失败。
        {
			VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "设置本端高级UDP协议套接字的接收缓冲区大小失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            goto Out;
        }
    } //协议连接结束。

	//等待允许连接。
	if( ( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) && ( g_MediaInfoPt->m_IsAutoAllowCnct != 0 ) ) g_MediaInfoPt->m_RqstCnctRslt = 1;
	else g_MediaInfoPt->m_RqstCnctRslt = 0;
	{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_RmtNodeAddrVstrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_RQST_CNCT_DIALOG, ( WPARAM )p_ErrInfoVstrPt, 0 );} //向主界面发送显示请求连接对话框的消息。
	while( true )
	{
		if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是服务端。
		{
			if( g_MediaInfoPt->m_RqstCnctRslt == 1 ) //如果允许连接。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ALLOW_CNCT; //设置允许连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, 1, 10, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个允许连接包成功。" ) );
					LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto WaitAllowCnct;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个允许连接包失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}
			}
			else if( g_MediaInfoPt->m_RqstCnctRslt == 2 ) //如果拒绝连接。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, 1, 10, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}
			}
		}
		else //如果是客户端。
		{
			if( g_MediaInfoPt->m_RqstCnctRslt == 2 ) //如果中断等待。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, 1, 10, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个拒绝连接包成功。" ) );
					LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个拒绝连接包失败。原因：" ) );
					LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					goto Out;
				}
			}
		}
		
		//接收一个远端发送的数据包。
		if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
			( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.RecvPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
		{
			if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
			{
				if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_ALLOW_CNCT ) ) //如果是允许连接包。
				{
					if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是客户端。
					{
						g_MediaInfoPt->m_RqstCnctRslt = 1;
						
						PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

						VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个允许连接包。" ) );
						LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
						goto WaitAllowCnct;
					}
					else //如果是服务端。
					{
						//就重新接收。
					}
				}
				else if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_REFUSE_CNCT ) ) //如果是拒绝连接包。
				{
					g_MediaInfoPt->m_RqstCnctRslt = 2;
					
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个拒绝连接包。" ) );
					LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
					{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
					goto Out;
				}
				else //如果是其他包。
				{
					//就重新接收。
				}
			}
			else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
			{
				//就重新接收。
			}
		}
		else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
		{
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

			VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
			LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
			goto Out;
		}
	}
	WaitAllowCnct:;

	switch( g_MediaInfoPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
	{
	    case 0: //如果使用链表。
	    {
			//初始化接收音频输出帧链表。
			if( g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.Init( LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * 2, 0, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * 2, SIZE_MAX, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化接收音频输出帧链表成功。" ) );
			}
			else
			{
				VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收音频输出帧链表失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
			}

			//初始化接收视频输出帧链表。
			if( g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.Init( LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * 2, 0, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen * 2, SIZE_MAX, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
			{
				LOGI( Cu8vstr( "初始化接收视频输出帧链表成功。" ) );
			}
			else
			{
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化接收视频输出帧链表失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
			}
	        break;
	    }
	    case 1: //如果使用自适应抖动缓冲器。
	    {
			//初始化音频自适应抖动缓冲器。
            if( AAjbInit( &g_MediaInfoPt->m_AAjbPt, MediaPocsThrdPt->m_AdoOtpt.m_SmplRate, MediaPocsThrdPt->m_AdoOtpt.m_FrmLen, 1, 1, 0, g_MediaInfoPt->m_AAjbMinNeedBufFrmCnt, g_MediaInfoPt->m_AAjbMaxNeedBufFrmCnt, g_MediaInfoPt->m_AAjbMaxCntuLostFrmCnt, g_MediaInfoPt->m_AAjbAdaptSensitivity, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
            {
                LOGI( Cu8vstr( "初始化音频自适应抖动缓冲器成功。" ) );
            }
            else
            {
                VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化音频自适应抖动缓冲器失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
	            goto Out;
            }

            //初始化视频自适应抖动缓冲器。
            if( VAjbInit( &g_MediaInfoPt->m_VAjbPt, 1, g_MediaInfoPt->m_VAjbMinNeedBufFrmCnt, g_MediaInfoPt->m_VAjbMaxNeedBufFrmCnt, g_MediaInfoPt->m_VAjbAdaptSensitivity, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
            {
                LOGI( Cu8vstr( "初始化视频自适应抖动缓冲器成功。" ) );
            }
            else
            {
                VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "初始化视频自适应抖动缓冲器失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
	            goto Out;
            }
	        break;
	    }
	}

	g_MediaInfoPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
    g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
	
	VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "开始对讲。" ) );
	LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
	{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
	if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_LclNodeAddrVstrPt );
	VstrDstoy( p_LclNodePortVstrPt );
	VstrDstoy( p_RmtNodeAddrVstrPt );
	VstrDstoy( p_RmtNodePortVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		
	}
    return p_Rslt;
}

//用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaPocsThrdUserPocs( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	uint32_t p_TmpUint32;
	uint64_t p_TmpUint64;

    //接收远端发送过来的一个数据包。
	if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
        ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.RecvPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
    {
		if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
		{
			if( p_TmpSz == 0 ) //如果数据包的长度为0。
			{
				LOGFE( Cu8vstr( "接收到一个数据包的数据长度为%uzd，表示没有数据，无法继续接收。" ), p_TmpSz );
                goto Out;
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_ADO_FRM ) //如果是音频输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果音频输出帧包的长度小于1 + 4，表示没有音频输出帧时间戳。
                {
                    LOGFE( Cu8vstr( "接收到一个音频输出帧包的数据长度为%uzd小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。" ), p_TmpSz );
                    goto Out;
                }

				//读取音频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) //如果要使用音频输出。
				{
					//将输出帧放入链表或自适应抖动缓冲器。
					switch( g_MediaInfoPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
					{
						case 0: //如果使用链表。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.PutTail( g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
								{
									LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto Out;
								}
							}
							else //如果该音频输出帧为无语音活动。
                            {
                                LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，无需放入接收音频输出帧链表。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }
							break;
						}
						case 1: //如果使用自适应抖动缓冲器。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                            {
                                AAjbPutFrm( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
                                LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }
                            else //如果该音频输出帧为无语音活动。
                            {
								AAjbPutFrm( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, 0, 1, NULL );
                                LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
							int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							AAjbGetBufFrmCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

							break;
						}
					}
				}
				else //如果不使用音频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                    {
                        LOGFI( Cu8vstr( "接收到一个有语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                    }
                    else //如果该音频输出帧为无语音活动。
                    {
                        LOGFI( Cu8vstr( "接收到一个无语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                    }
                }
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_VDO_FRM ) //如果是视频输出帧包。
            {
                if( p_TmpSz < 1 + 4 ) //如果视频输出帧包的长度小于1 + 4，表示没有视频输出帧时间戳。
                {
					LOGFE( Cu8vstr( "接收到一个视频输出帧包的长度为%uzd小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。" ), p_TmpSz );
                    goto Out;
                }

				//读取视频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
                if( MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt ) //如果要使用视频输出。
                {
                    //将视频输出帧放入链表或自适应抖动缓冲器。
                    switch( g_MediaInfoPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
                    {
                        case 0: //如果使用链表。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.PutTail( g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 )
								{
									LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表失败。原因：%vs" ), MediaPocsThrdPt->m_ErrInfoVstrPt );
									goto Out;
								}
							}
							else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入接收视频输出帧链表。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }
                            break;
                        }
                        case 1: //如果使用自适应抖动缓冲器。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                            {
								uint64_t p_CurTime;
								FuncGetTimeAsMsec( &p_CurTime );
                                VAjbPutFrm( g_MediaInfoPt->m_VAjbPt, p_CurTime, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
                                LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }
                            else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
							VAjbGetBufFrmCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
							LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );
                            break;
                        }
                    }
                }
                else //如果不使用视频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                    {
                        LOGFI( Cu8vstr( "接收到一个有图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                    }
                    else //如果该视频输出帧为无图像活动。
                    {
                        LOGFI( Cu8vstr( "接收到一个无图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%uz32d，总长度：%uzd。" ), p_TmpUint32, p_TmpSz );
                    }
                }
            }
            else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) //如果是退出包。
            {
                if( p_TmpSz > 1 ) //如果退出包的长度大于1。
                {
                    LOGFE( Cu8vstr( "接收到退出包的长度为%uzd大于1，表示还有其他数据，无法继续接收。" ), p_TmpSz );
                    goto Out;
                }

                g_MediaInfoPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
                MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 0, NULL ); //请求退出。

				VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
				LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
            }
		}
		else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
        {
			
        }
	}
	else //如果用本端套接字接收一个连接的远端套接字发送的数据包失败。
	{
		VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
		LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		goto Out;
	}

	if( g_MediaInfoPt->m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
	{
		if( g_MediaInfoPt->m_PttBtnIsDown == 0 ) //如果一键即按即通按钮为弹起。
		{
			if( ( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt != 0 ) && ( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt == 0 ) ) //如果要使用音频输入，且不使用音频输出。
			{
				MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = 0;
				MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = 1;
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL ); //请求重启。
			}

			if( ( MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt != 0 ) && ( MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt == 0 ) ) //如果要使用视频输入，且不使用视频输出。
			{
				MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = 0;
				MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = 1;
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL ); //请求重启。
			}
		}
		else //如果一键即按即通按钮为按下。
		{
			if( ( MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt == 0 ) && ( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 ) ) //如果不使用音频输入，且要使用音频输出。
			{
				MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = 1;
				MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = 0;
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL ); //请求重启。
			}

			if( ( MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt == 0 ) && ( MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt != 0 ) ) //如果不使用视频输入，且要使用视频输出。
			{
				MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = 1;
				MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = 0;
				MediaPocsThrdRqirExit( MediaPocsThrdPt, 3, 0, NULL ); //请求重启。
			}
		}
	}

    p_Rslt = 0; //设置本函数执行成功。

	Out:
	return p_Rslt;
}

//用户定义的销毁函数，在本线程退出时回调一次。
void __cdecl MyMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	size_t p_TmpSz;
	
	if( ( MediaPocsThrdPt->m_ExitFlag == 1 ) && ( ( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( g_MediaInfoPt->m_AudpSokt.m_AudpSoktPt != NULL ) && ( g_MediaInfoPt->m_AudpSokt.GetRmtAddr( g_MediaInfoPt->m_AudpCnctIdx, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本线程接收到退出请求，且本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
    {
		{
			//发送退出包。
			g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_EXIT; //设置退出包。
			if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) ||
				( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, 1, 10, MediaPocsThrdPt->m_ErrInfoVstrPt ) != 0 ) ) )
			{
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个退出包失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto SendExitPkt;
			}
			
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "发送一个退出包成功。" ) );
			LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}

			//接收退出包。
			if( g_MediaInfoPt->m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
			{
				while( true ) //循环接收退出包。
				{
					if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
						( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.RecvPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
					{
						if( p_TmpSz != -1 ) //如果用本端套接字接收一个连接的远端套接字发送的数据包成功。
						{
							if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) ) //如果是退出包。
							{
								VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "接收到一个退出包。" ) );
								LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
								{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
								goto SendExitPkt;
							}
							else //如果是其他包，继续接收。
							{

							}
						}
						else //如果用本端套接字接收一个连接的远端套接字发送的数据包超时。
						{
							VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
							LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
							{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
							goto SendExitPkt;
						}
					}
					else //用本端套接字接收一个连接的远端套接字发送的数据包失败。
					{
						VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "用本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" ) );
						LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
						{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
						goto SendExitPkt;
					}
				}
			}
		}
		SendExitPkt:;

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "中断对讲。" ) );
		LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
		if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVstrPt );
    }

    //销毁本端TCP协议服务端套接字。
    if( g_MediaInfoPt->m_TcpSrvrSokt.m_TcpSrvrSoktPt != NULL )
    {
        g_MediaInfoPt->m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议服务端套接字成功。" ) );
		LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
    }

    //销毁本端TCP协议客户端套接字。
    if( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL )
    {
        g_MediaInfoPt->m_TcpClntSokt.Dstoy( ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁已创建的本端TCP协议客户端套接字成功。" ) );
		LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
    }
	
    //销毁本端高级UDP协议套接字。
    if( g_MediaInfoPt->m_AudpSokt.m_AudpSoktPt != NULL )
    {
        g_MediaInfoPt->m_AudpSokt.Dstoy( NULL ); //关闭并销毁已创建的本端高级UDP协议套接字。

		VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "关闭并销毁本端高级UDP协议套接字成功。" ) );
		LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
		{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
    }

    //销毁接收音频输出帧的链表。
    if( g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.m_VarLenLnkLstPt != NULL )
    {
		g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.Dstoy( NULL );

		LOGI( Cu8vstr( "销毁接收音频输出帧的链表成功。" ) );
    }
	
    //销毁接收视频输出帧的链表。
    if( g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.m_VarLenLnkLstPt != NULL )
    {
		g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.Dstoy( NULL );

		LOGI( Cu8vstr( "销毁接收视频输出帧的链表成功。" ) );
    }

    //销毁音频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_AAjbPt != NULL )
    {
        AAjbDstoy( g_MediaInfoPt->m_AAjbPt, NULL );
        g_MediaInfoPt->m_AAjbPt = NULL;

		LOGI( Cu8vstr( "销毁音频自适应抖动缓冲器成功。" ) );
    }
	
    //销毁视频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_VAjbPt != NULL )
    {
        VAjbDstoy( g_MediaInfoPt->m_VAjbPt, NULL );
        g_MediaInfoPt->m_VAjbPt = NULL;

		LOGI( Cu8vstr( "销毁视频自适应抖动缓冲器成功。" ) );
    }
	
    if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
    {
        if( ( MediaPocsThrdPt->m_ExitFlag == 1 ) && ( g_MediaInfoPt->m_IsRecvExitPkt == 1 ) ) //如果本线程接收到退出请求，且接收到了退出包。
        {
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且本线程接收到退出请求，且接收到了退出包，表示是远端TCP协议客户端套接字主动退出，本线程重新初始化来继续保持监听。" ) );
			LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            
			MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
        }
		else if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) && ( MediaPocsThrdPt->m_ExitCode == -1 ) && ( g_MediaInfoPt->m_RqstCnctRslt == 2 ) ) //如果本线程没收到退出请求，且退出代码为初始化失败，且请求连接的结果为拒绝。
		{
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且本线程没收到退出请求，且初始化失败，且请求连接的结果为拒绝，表示是拒绝本次连接，本线程重新初始化来继续保持监听。" ) );
			LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            
			MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
		}
        else if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) && ( MediaPocsThrdPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
			VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建服务端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来继续保持监听。" ) );
			LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            
			MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_MEDIA_PROC_THREAD, 0, 0 ); //向主对话框发送销毁媒体处理线程的消息。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
        }
    }
    else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建客户端。
    {
        if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) && ( MediaPocsThrdPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
            VstrCpy( MediaPocsThrdPt->m_ErrInfoVstrPt, Cu8vstr( "由于是创建客户端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来重连服务端。" ) );
			LOGI( MediaPocsThrdPt->m_ErrInfoVstrPt );
			{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
            
            MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_MEDIA_PROC_THREAD, 0, 0 ); //向主对话框发送销毁媒体处理线程的消息。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
        }
    }
}

//用户定义的读取音视频输入帧函数，在读取到一个音频输入帧或视频输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
void __cdecl MyMediaPocsThrdUserReadAdoVdoInptFrm( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAdoInptFrmPt, int16_t * PcmAdoRsltFrmPt, int32_t VoiceActSts, uint8_t * EncdAdoInptFrmPt, size_t EncdAdoInptFrmLen, int32_t EncdAdoInptFrmIsNeedTrans, uint8_t * YU12VdoInptFrmPt, int32_t YU12VdoInptFrmWidth, int32_t YU12VdoInptFrmHeight, uint8_t * EncdVdoInptFrmPt, size_t EncdVdoInptFrmLen )
{
    int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	int p_FrmPktLen = 0; //存放输入输出帧数据包的长度，单位为字节。

    //发送音频输入帧。
	if( PcmAdoInptFrmPt != NULL ) //如果要使用音频输入。
    {
		if( EncdAdoInptFrmPt != NULL ) //如果要使用已编码格式音频输入帧。
        {
            if( VoiceActSts != 0 && EncdAdoInptFrmIsNeedTrans != 0 ) //如果本次音频输入帧为有语音活动，且需要传输。
            {
                memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, EncdAdoInptFrmPt, EncdAdoInptFrmLen ); //设置音频输入输出帧。
                p_FrmPktLen = 1 + 4 + 4 + EncdAdoInptFrmLen; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 视频输入帧时间戳 + 已编码格式音频输入帧。
            }
            else //如果本次音频输入帧为无语音活动，或不需要传输。
            {
                p_FrmPktLen = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
            }
        }
        else //如果要使用PCM格式音频输入帧。
        {
            if( VoiceActSts != 0 ) //如果本次音频输入帧为有语音活动。
            {
				memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, PcmAdoRsltFrmPt, MediaPocsThrdPt->m_AdoInpt.m_FrmLen * 2 ); //设置音频输入输出帧。
                p_FrmPktLen = 1 + 4 + 4 + MediaPocsThrdPt->m_AdoInpt.m_FrmLen * 2; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 视频输入帧时间戳 + PCM格式音频输入帧。
            }
            else //如果本次音频输入帧为无语音活动，或不需要传输。
            {
                p_FrmPktLen = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
            }
        }

        if( p_FrmPktLen != 1 + 4 ) //如果本音频输入帧为有语音活动，就发送。
        {
			g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ADO_FRM;
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );
			//设置视频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[5] = ( int8_t ) ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[6] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[7] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[8] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
            {
				LOGFI( Cu8vstr( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，视频输入帧时间戳：%uz32d，总长度：%d。" ), g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp, g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp, p_FrmPktLen );
			}
			else
			{
				VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有语音活动的音频输入帧包失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
			}
			
			g_MediaInfoPt->m_LastSendAdoInptFrmIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
        }
        else if( ( p_FrmPktLen == 1 + 4 ) && ( g_MediaInfoPt->m_LastSendAdoInptFrmIsAct != 0 ) ) //如果本音频输入帧为无语音活动，但最后一个发送的音频输入帧为有语音活动，就发送。
        {
            g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

            //设置数据包类型为音频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ADO_FRM;
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 10, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
            {
                LOGFI( Cu8vstr( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%uz32d，总长度：%d。" ), g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp, p_FrmPktLen );
            }
            else
            {
                VstrIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个无语音活动的音频输入帧包失败。原因：" ) );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
            }

            g_MediaInfoPt->m_LastSendAdoInptFrmIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
        }
		else //如果本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。
		{
            LOGI( Cu8vstr( "本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" ) );
        }
    }

    //发送视频输入帧。
    if( YU12VdoInptFrmPt != NULL ) //如果要使用视频输入。
    {
        if( EncdVdoInptFrmPt != NULL ) //如果要使用已编码格式视频输入帧。
        {
            if( EncdVdoInptFrmLen != 0 ) //如果本次视频输入帧为有图像活动。
            {
				memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, EncdVdoInptFrmPt, EncdVdoInptFrmLen ); //设置视频输入输出帧。
                p_FrmPktLen = 1 + 4 + 4 + EncdVdoInptFrmLen; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 音频输入帧时间戳 + 已编码格式视频输入帧。
            }
            else
            {
                p_FrmPktLen = 1 + 4; //数据包长度 = 数据包类型 + 视频输入帧时间戳。
            }
        }
        else //如果要使用YU12格式视频输入帧。
        {
			//设置视频输入帧宽度。
            g_MediaInfoPt->m_TmpBytePt[9] = ( int8_t ) ( YU12VdoInptFrmWidth & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[10] = ( int8_t ) ( ( YU12VdoInptFrmWidth & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[11] = ( int8_t ) ( ( YU12VdoInptFrmWidth & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[12] = ( int8_t ) ( ( YU12VdoInptFrmWidth & 0xFF000000 ) >> 24 );
            //设置视频输入帧高度。
            g_MediaInfoPt->m_TmpBytePt[13] = ( int8_t ) ( YU12VdoInptFrmHeight & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[14] = ( int8_t ) ( ( YU12VdoInptFrmHeight & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[15] = ( int8_t ) ( ( YU12VdoInptFrmHeight & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[16] = ( int8_t ) ( ( YU12VdoInptFrmHeight & 0xFF000000 ) >> 24 );

			memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4 + 4 + 4, YU12VdoInptFrmPt, YU12VdoInptFrmWidth * YU12VdoInptFrmHeight * 3 / 2 ); //设置视频输入输出帧。
            p_FrmPktLen = 1 + 4 + 4 + 4 + 4 + YU12VdoInptFrmWidth * YU12VdoInptFrmHeight * 3 / 2; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 音频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + YU12格式视频输入帧。
        }

        //发送视频输入帧数据包。
        if( p_FrmPktLen != 1 + 4 ) //如果本视频输入帧为有图像活动，就发送。
        {
            g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

            //设置数据包类型为视频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_VDO_FRM;
            //设置视频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[5] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[6] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[7] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[8] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_AudpSokt.SendPkt( g_MediaInfoPt->m_AudpCnctIdx, g_MediaInfoPt->m_TmpBytePt, p_FrmPktLen, 1, MediaPocsThrdPt->m_ErrInfoVstrPt ) == 0 ) ) )
            {
				LOGFI( Cu8vstr( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%uz32d，音频输入帧时间戳：%uz32d，总长度：%d，类型：%d。" ), g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp, g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp, p_FrmPktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
            }
            else
            {
				VstrFmtIns( MediaPocsThrdPt->m_ErrInfoVstrPt, 0, Cu8vstr( "发送一个有图像活动的视频输入帧包失败。视频输入帧时间戳：%uz32d，音频输入帧时间戳：%uz32d，总长度：%d，类型：%d。原因：" ), g_MediaInfoPt->m_LastSendVdoInptFrmTimeStamp, g_MediaInfoPt->m_LastSendAdoInptFrmTimeStamp, p_FrmPktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
				LOGE( MediaPocsThrdPt->m_ErrInfoVstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , MediaPocsThrdPt->m_ErrInfoVstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto Out;
            }
        }
        else //如果本视频输入帧为无图像活动，无需发送。
        {
            LOGI( Cu8vstr( "本视频输入帧为无图像活动，无需发送。" ) );
        }
    }

    p_Rslt = 0; //设置本函数执行成功。

	Out:
    return;
}

//用户定义的写入音频输出帧函数，在需要写入一个音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
void __cdecl MyMediaPocsThrdUserWriteAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, uint8_t * EncdAdoOtptFrmPt, size_t * AdoOtptFrmLenPt )
{
	size_t m_TmpSz = 0;

	//取出并写入音频输出帧。
	{
		//从链表或音频自适应抖动缓冲器取出一个音频输出帧。
		switch( g_MediaInfoPt->m_UseWhatRecvOtptFrm ) //使用什么接收音频输出帧。
		{
			case 0: //如果使用链表。
			{
				g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
				if( m_TmpSz != 0 )
				{
					g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.Lock( NULL ); //接收音频输出帧链表的互斥锁加锁。
					g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.GetHead( NULL, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, NULL, &m_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收音频输出帧链表的第一个音频输出帧。
					g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.DelHead( 0, MediaPocsThrdPt->m_ErrInfoVstrPt ); //删除接收音频输出帧链表的第一个音频输出帧。
					g_MediaInfoPt->m_RecvAdoOtptFrmLnkLst.Unlock( NULL ); //接收音频输出帧链表的互斥锁解锁。
				}
				
				if( m_TmpSz != 0 ) //如果接收音频输出帧链表的第一个输出帧为有语音活动。
				{
					LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个有语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}
				else //如果接收音频输出帧链表为空，或第一个音频输出帧为无语音活动。
				{
					LOGFI( Cu8vstr( "从接收音频输出帧链表取出一个无语音活动的音频输出帧，帧的长度：%uzd。" ), m_TmpSz );
				}

                break;
			}
			case 1: //如果使用自适应抖动缓冲器。
			{
				uint32_t p_AdoOtptFrmTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出音频输出帧。
				AAjbGetFrm( g_MediaInfoPt->m_AAjbPt, &p_AdoOtptFrmTimeStamp, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, &m_TmpSz, 1, NULL );
				
				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					g_MediaInfoPt->m_LastGetAdoOtptFrmVdoOtptFrmTimeStamp = ( g_MediaInfoPt->m_TmpByte2Pt[0] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[1] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[2] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[3] & 0xFF ) << 24 ); //设置最后一个取出的音频输出帧对应视频输出帧的时间戳。
					g_MediaInfoPt->m_LastGetAdoOtptFrmIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%uz32d，视频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, g_MediaInfoPt->m_LastGetAdoOtptFrmVdoOtptFrmTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					g_MediaInfoPt->m_LastGetAdoOtptFrmIsAct = 0; //设置最后一个取出的音频输出帧为无语音活动。
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					g_MediaInfoPt->m_LastGetAdoOtptFrmIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( Cu8vstr( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%uz32d，视频输出帧时间戳：%uz32d，长度：%uzd。" ), p_AdoOtptFrmTimeStamp, g_MediaInfoPt->m_LastGetAdoOtptFrmVdoOtptFrmTimeStamp, m_TmpSz );
				}
				
				int32_t p_CurHaveBufActFrmCnt; //存放当前已缓冲有活动帧的数量。
				int32_t p_CurHaveBufInactFrmCnt; //存放当前已缓冲无活动帧的数量。
				int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
				int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
				int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
				int32_t p_MaxCntuLostFrmCnt; //存放最大连续丢失帧的数量。
				int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
				AAjbGetBufFrmCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrmCnt, &p_CurHaveBufInactFrmCnt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_MaxCntuLostFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
				LOGFI( Cu8vstr( "音频自适应抖动缓冲器：有活动帧：%z32d，无活动帧：%z32d，帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，最大丢帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufActFrmCnt, p_CurHaveBufInactFrmCnt, p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_MaxCntuLostFrmCnt, p_CurNeedBufFrmCnt );

				break;
			}
		}

		//写入音频输出帧。
		if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
		{
			if( PcmAdoOtptFrmPt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                if( m_TmpSz - 4 != *AdoOtptFrmLenPt )
                {
					memset( PcmAdoOtptFrmPt, 0, *AdoOtptFrmLenPt );
                    LOGFE( Cu8vstr( "音频输出帧的长度不等于PCM格式的长度。音频输出帧：%uzd，PCM格式：%z32d。" ), m_TmpSz - 4, *AdoOtptFrmLenPt );
                }
				else
				{
					//写入PCM格式音频输出帧。
					memcpy( PcmAdoOtptFrmPt, g_MediaInfoPt->m_TmpByte2Pt + 4, *AdoOtptFrmLenPt );
				}
            }
            else //如果要使用已编码格式音频输出帧。
            {
                if( m_TmpSz - 4 > *AdoOtptFrmLenPt )
                {
                    LOGFE( Cu8vstr( "音频输出帧的长度已超过已编码格式的长度。音频输出帧：%uzd，已编码格式：%uzd。" ), m_TmpSz - 4, *AdoOtptFrmLenPt );
                    *AdoOtptFrmLenPt = 0;
                }
				else
				{
					//写入已编码格式音频输出帧。
					*AdoOtptFrmLenPt = m_TmpSz - 4;
					memcpy( EncdAdoOtptFrmPt, g_MediaInfoPt->m_TmpByte2Pt + 4, *AdoOtptFrmLenPt );
				}
            }
		}
		else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
		{
			if( PcmAdoOtptFrmPt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                memset( PcmAdoOtptFrmPt, 0, *AdoOtptFrmLenPt );
            }
            else //如果要使用已编码格式音频输出帧。
            {
                *AdoOtptFrmLenPt = 0;
            }
		}
		else //如果音频输出帧为丢失。
		{
			if( PcmAdoOtptFrmPt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                memset( PcmAdoOtptFrmPt, 0, *AdoOtptFrmLenPt );
            }
            else //如果要使用已编码格式音频输出帧。
            {
                *AdoOtptFrmLenPt = m_TmpSz;
            }
		}
	}

	return;
}

//用户定义的获取PCM格式音频输出帧函数，在解码完一个已编码音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
void __cdecl MyMediaPocsThrdUserGetPcmAdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, int32_t AdoOtptStrmIdx, int16_t * PcmAdoOtptFrmPt, size_t PcmAdoOtptFrmLen )
{
	
}

//用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
void __cdecl MyMediaPocsThrdUserWriteVdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VdoOtptFrmPt, int32_t * YU12VdoOtptFrmWidthPt, int32_t * YU12VdoOtptFrmHeightPt, uint8_t * EncdVdoOtptFrmPt, size_t * EncdVdoOtptFrmLenPt )
{
	uint32_t p_VdoOtptFrmTimeStamp;
    uint32_t p_VdoOtptFrmAdoOtptFrmTimeStamp;
	uint64_t p_NowTimeMesc;
	size_t m_TmpSz = 0;

    //从链表或自适应抖动缓冲器取出一个视频输出帧。
    switch( g_MediaInfoPt->m_UseWhatRecvOtptFrm ) //使用什么接收输出帧。
    {
        case 0: //如果使用链表。
        {
			g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.GetTotal( &m_TmpSz, 1, NULL );
            if( m_TmpSz != 0 ) //如果接收视频输出帧链表不为空。
            {
				g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.Lock( NULL ); //接收视频输出帧链表的互斥锁加锁。
				g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.GetHead( NULL, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, NULL, &m_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVstrPt ); //获取接收视频输出帧链表的第一个视频输出帧。
				g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.DelHead( 0, MediaPocsThrdPt->m_ErrInfoVstrPt ); //删除接收视频输出帧链表的第一个视频输出帧。
				g_MediaInfoPt->m_RecvVdoOtptFrmLnkLst.Unlock( NULL ); //接收视频输出帧链表的互斥锁解锁。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个有图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( Cu8vstr( "从接收视频输出帧链表取出一个无图像活动的视频输出帧。长度：%uzd。" ), m_TmpSz );
				}
            }
			
            break;
        }
        case 1: //如果使用自适应抖动缓冲器。
        {
            int32_t p_CurHaveBufFrmCnt; //存放当前已缓冲帧的数量。
			int32_t p_MinNeedBufFrmCnt; //存放最小需缓冲帧的数量。
			int32_t p_MaxNeedBufFrmCnt; //存放最大需缓冲帧的数量。
			int32_t p_CurNeedBufFrmCnt; //存放当前需缓冲帧的数量。
			VAjbGetBufFrmCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrmCnt, &p_MinNeedBufFrmCnt, &p_MaxNeedBufFrmCnt, &p_CurNeedBufFrmCnt, 1, NULL );
			
			if( p_CurHaveBufFrmCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				LOGFI( Cu8vstr( "视频自适应抖动缓冲器：帧：%z32d，最小需帧：%z32d，最大需帧：%z32d，当前需帧：%z32d。" ), p_CurHaveBufFrmCnt, p_MinNeedBufFrmCnt, p_MaxNeedBufFrmCnt, p_CurNeedBufFrmCnt );

				//从视频自适应抖动缓冲器取出视频输出帧。
				FuncGetTimeAsMsec( &p_NowTimeMesc );
				if( MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 && g_MediaInfoPt->m_LastGetAdoOtptFrmIsAct != 0 ) //如果要使用音频输出，且最后一个取出的音频输出帧为有语音活动，就根据最后一个取出的音频输出帧对应视频输出帧的时间戳来取出。
				{
					VAjbGetFrmWantTimeStamp( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, g_MediaInfoPt->m_LastGetAdoOtptFrmVdoOtptFrmTimeStamp, &p_VdoOtptFrmTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz, 1, NULL );
				}
				else //如果最后一个取出的音频输出帧为无语音活动，就根据直接取出。
				{
					VAjbGetFrm( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, &p_VdoOtptFrmTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz, 1, NULL );
				}

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( Cu8vstr( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%uz32d，长度：%uzd。" ), p_VdoOtptFrmTimeStamp, m_TmpSz );
				}
			}
            break;
        }
    }

    //写入视频输出帧。
    if( m_TmpSz > 0 ) //如果视频输出帧为有图像活动。
    {
        //读取视频输出帧对应音频输出帧的时间戳。
        p_VdoOtptFrmAdoOtptFrmTimeStamp = ( g_MediaInfoPt->m_TmpByte3Pt[0] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[1] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[2] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[3] & 0xFF ) << 24 );

        if( YU12VdoOtptFrmPt != NULL ) //如果要使用YU12格式视频输出帧。
        {
			*YU12VdoOtptFrmWidthPt = ( g_MediaInfoPt->m_TmpByte3Pt[4] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[5] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[6] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[7] & 0xFF ) << 24 );
			*YU12VdoOtptFrmHeightPt = ( g_MediaInfoPt->m_TmpByte3Pt[8] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[9] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[10] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[11] & 0xFF ) << 24 );

            if( m_TmpSz - 4 - 4 - 4 != *YU12VdoOtptFrmWidthPt * *YU12VdoOtptFrmHeightPt * 3 / 2 )
            {
                LOGFE( Cu8vstr( "视频输出帧的长度不等于YU12格式的长度。视频输出帧：%uzd，YU12格式：%z32d。" ), m_TmpSz - 4 - 4 - 4, *EncdVdoOtptFrmLenPt );
				*YU12VdoOtptFrmWidthPt = 0;
				*YU12VdoOtptFrmHeightPt = 0;
				return;
            }

            //写入YU12格式视频输出帧。
			memcpy( YU12VdoOtptFrmPt, g_MediaInfoPt->m_TmpByte3Pt + 4 + 4 + 4, m_TmpSz - 4 - 4 - 4 );
        }
        else //如果要使用已编码格式视频输出帧。
        {
            if( m_TmpSz - 4 > *EncdVdoOtptFrmLenPt )
            {
                *EncdVdoOtptFrmLenPt = 0;
				LOGFE( Cu8vstr( "视频输出帧的长度已超过已编码格式的长度。视频输出帧：%uzd，已编码格式：%z32d。" ), m_TmpSz - 4, *EncdVdoOtptFrmLenPt );
                return;
            }

            //写入已编码格式视频输出帧。
            memcpy( EncdVdoOtptFrmPt, g_MediaInfoPt->m_TmpByte3Pt + 4, m_TmpSz - 4 );
            *EncdVdoOtptFrmLenPt = m_TmpSz - 4;
        }
    }
    else if( m_TmpSz == 0 ) //如果视频输出帧为无图像活动。
    {
		if( YU12VdoOtptFrmPt != NULL ) //如果要使用YU12格式视频输出帧。
        {
            *EncdVdoOtptFrmLenPt = 0;
        }
        else //如果要使用已编码格式视频输出帧。
        {
            *EncdVdoOtptFrmLenPt = 0;
        }
    }
}

//用户定义的获取YU12格式视频输出帧函数，在解码完一个已编码视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
void __cdecl MyMediaPocsThrdUserGetYU12VdoOtptFrm( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VdoOtptFrmPt, int32_t YU12VdoOtptFrmWidth, int32_t YU12VdoOtptFrmHeight )
{
	
}

//窗口消息处理过程函数。
INT_PTR CALLBACK WndMsgPocsPocdr( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int p_Rslt = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	
	UNREFERENCED_PARAMETER( lParam );
	switch( message )
	{
		case WM_INITDIALOG: //窗口初始化消息。
		{
			return ( INT_PTR )FALSE;
		}
		case WM_SHOWWINDOW: //窗口显示消息。
		{
			if( ( wParam == TRUE ) && ( lParam == 0 ) )
			{
				//设置当前窗口的位置相对于父窗口居中。
				if( FuncSetWndCenter( hDlg, GetParent( hDlg ), g_ErrInfoVstr.m_VstrPt ) != 0 )
				{
					LOGFE( Cu8vstr( "设置窗口居中失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
				}
				return ( INT_PTR )TRUE;
			}
			return ( INT_PTR )FALSE;
		}
		case WM_NCHITTEST: //让用户可以在窗口客户区移动窗口。
		{
			LRESULT lTemp = DefWindowProc( hDlg, message, wParam, lParam );
			if( lTemp == HTCLIENT )
			{
				SetWindowLong( hDlg, 0, HTCAPTION );
				return ( INT_PTR )TRUE;
			}
			return ( INT_PTR )FALSE;
		}
		case WM_DEVICECHANGE: //系统硬件设备改变消息。
		{
			HWND p_AdoInptDvcComboBoxWndHdl = NULL;
			HWND p_AdoOtptDvcComboBoxWndHdl = NULL;
			HWND p_VdoInptDvcComboBoxWndHdl = NULL;
			Vstr * * p_AdoInptDvcNameVstrArrPt = NULL;
			UINT p_AdoInptDvcTotal = 0;
			Vstr * * p_AdoOtptDvcNameVstrArrPt = NULL;
			UINT p_AdoOtptDvcTotal = 0;
			Vstr * * p_VdoInptDvcNameVstrArrPt = NULL;
			UINT p_VdoInptDvcTotal = 0;
			wchar_t * p_CurAdoInptDvcNamePt = NULL;
			wchar_t * p_CurAdoOtptDvcNamePt = NULL;
			wchar_t * p_CurVdoInptDvcNamePt = NULL;
			long p_TmpInt;

			//获取音频输入设备、音频输出设备、视频输入设备的名称。
			if( MediaPocsThrdGetAdoInptDvcName( &p_AdoInptDvcNameVstrArrPt, &p_AdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
			{
				g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输入设备名称失败。原因：" ) );
				LOGE( g_ErrInfoVstr.m_VstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameVstrArrPt, &p_AdoOtptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
			{
				g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输出设备名称失败。原因：" ) );
				LOGE( g_ErrInfoVstr.m_VstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameVstrArrPt, &p_VdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
			{
				g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取视频输入设备名称失败。原因：" ) );
				LOGE( g_ErrInfoVstr.m_VstrPt );
				{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}
				goto OutDvcChange;
			}
			
			//获取音频输入设备、音频输出设备、视频输入设备的组合框窗口句柄。
			p_AdoInptDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId );
			p_AdoOtptDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId );
			p_VdoInptDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId );

			//获取当前选择的音频输入设备、音频输出设备、视频输入设备。
			p_TmpInt = SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurAdoInptDvcNamePt = ( wchar_t * )alloca( ( SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAdoInptDvcNamePt );
			}
			p_TmpInt = SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurAdoOtptDvcNamePt = ( wchar_t * )alloca( ( SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAdoOtptDvcNamePt );
			}
			p_TmpInt = SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurVdoInptDvcNamePt = ( wchar_t * )alloca( ( SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurVdoInptDvcNamePt );
			}

			//清空音频输入设备、音频输出设备、视频输入设备的组合框。
			SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );

			//添加音频输入设备、音频输出设备、视频输入设备到组合框。
			if( p_AdoInptDvcTotal > 0 )
			{
				p_TmpInt = SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )L"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AdoInptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AdoInptDvcNameVstrArrPt[p_TmpInt]->m_StrPt );
				}
			}
			else
			{
				SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )L"无" );
			}
			if( p_AdoOtptDvcTotal > 0 )
			{
				SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )L"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AdoOtptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AdoOtptDvcNameVstrArrPt[p_TmpInt]->m_StrPt );
				}
			}
			else
			{
				SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
			}
			if( p_VdoInptDvcTotal > 0 )
			{
				SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )L"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_VdoInptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_VdoInptDvcNameVstrArrPt[p_TmpInt]->m_StrPt );
				}
			}
			else
			{
				SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
			}
			
			//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
			if( ( p_CurAdoInptDvcNamePt == NULL ) || ( SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoInptDvcNamePt ) == CB_ERR ) )
			{
				if( SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"默认" ) == CB_ERR )
				{
					SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"无" );
				}
			}
			if( ( p_CurAdoOtptDvcNamePt == NULL ) || ( SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoOtptDvcNamePt ) == CB_ERR ) )
			{
				if( SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"默认" ) == CB_ERR )
				{
					SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"无" );
				}
			}
			if( ( p_CurVdoInptDvcNamePt == NULL ) || ( SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVdoInptDvcNamePt ) == CB_ERR ) )
			{
				if( SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"默认" ) == CB_ERR )
				{
					SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"无" );
				}
			}
			
			p_Rslt = 0; // //设置本函数执行成功。

			OutDvcChange:
			MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameVstrArrPt, p_AdoInptDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameVstrArrPt, p_AdoOtptDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameVstrArrPt, p_VdoInptDvcTotal, NULL );
			return ( p_Rslt == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
		}
		case WM_COMMAND: //单击消息。
		{
			switch( LOWORD( wParam ) )
			{
				case UseAdoTkbkModeRdBtnId: //使用音频对讲模式单选按钮。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = 0;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = 0;

						if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( Cu8vstr( "对讲模式已改变为音频对讲，请求重启媒体处理线程。" ) );
							g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseVdoTkbkModeRdBtnId: //使用视频对讲模式单选按钮。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = 0;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = 0;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = 1;

						if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( Cu8vstr( "对讲模式已改变为视频对讲，请求重启媒体处理线程。" ) );
							g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseAdoVdoTkbkModeRdBtnId: //使用音视频对讲模式单选按钮。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoInpt.m_IsUseAdoInpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt = 1;
						g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoOtpt.m_IsUseVdoOtpt = 1;

						if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( Cu8vstr( "对讲模式已改变为音视频对讲，请求重启媒体处理线程。" ) );
							g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseAdoInptDvcCbBoxId: //使用音频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								g_MediaPocsThrd.SetAdoInptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, &g_ErrInfoVstr );

								if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 && g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( Cu8vstr( "音频输入使用的设备已改变，请求重启媒体处理线程。" ) );
									g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case UseAdoOtptDvcCbBoxId: //使用音频输出设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
							{
								//设置音频输出使用的设备。
								g_MediaPocsThrd.SetAdoOtptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, &g_ErrInfoVstr );

								if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_AdoOtpt.m_IsUseAdoOtpt != 0 && g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( Cu8vstr( "音频输出使用的设备已改变，请求重启媒体处理线程。" ) );
									g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case UseVdoInptDvcCbBoxId: //使用视频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
							{
								//设置视频输入使用的设备。
								g_MediaPocsThrd.SetVdoInptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, &g_ErrInfoVstr );

								if( g_MediaPocsThrd.m_MediaPocsThrdPt->m_VdoInpt.m_IsUseVdoInpt != 0 && g_MediaPocsThrd.m_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用视频输入，且媒体处理线程已经初始化完毕。
								{
									LOGI( Cu8vstr( "视频输入使用的设备已改变，请求重启媒体处理线程。" ) );
									g_MediaPocsThrd.RqirExit( 3, 1, &g_ErrInfoVstr ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case AdoInptIsMuteCkBoxId: //音频输入静音复选框。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.SetAdoInptIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case AdoOtptIsMuteCkBoxId: //音频输出静音复选框。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.SetAdoOtptIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoInptIsBlackCkBoxId: //视频输入黑屏复选框。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.SetVdoInptIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoOtptIsBlackCkBoxId: //视频输出黑屏复选框。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrd.SetVdoOtptIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case CreateSrvrBtnId: //创建服务器按钮。
				case CnctSrvrBtnId: //连接服务器按钮。
				{
					if( g_MediaPocsThrd.m_MediaPocsThrdPt == NULL ) //如果媒体处理线程还没有启动。
					{
						LOGI( Cu8vstr( "开始启动媒体处理线程。" ) );

						//创建媒体信息的内存块。
						g_MediaInfoPt = ( MediaInfo * )calloc( 1, sizeof( MediaInfo ) );
						if( g_MediaInfoPt == NULL )
						{
							LOGE( Cu8vstr( "创建媒体信息的内存块失败。" ) );
							goto OutCreateSrvrOrClnt;
						}
						
						//初始化媒体处理线程的指针。
						if( g_MediaPocsThrd.Init( g_MediaInfoPt,
												  MyMediaPocsThrdUserInit, MyMediaPocsThrdUserPocs, MyMediaPocsThrdUserDstoy,
												  MyMediaPocsThrdUserReadAdoVdoInptFrm,
												  MyMediaPocsThrdUserWriteAdoOtptFrm, MyMediaPocsThrdUserGetPcmAdoOtptFrm,
												  MyMediaPocsThrdUserWriteVdoOtptFrm, MyMediaPocsThrdUserGetYU12VdoOtptFrm,
												  &g_ErrInfoVstr ) != 0 )
						{
							LOGFE( Cu8vstr( "初始化媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
							goto OutCreateSrvrOrClnt;
						}

						g_MediaInfoPt->m_MainDlgWndHdl = g_MainDlgWndHdl; //设置主对话框窗口的句柄。

						//设置网络。
						{
							HWND p_TmpWndHdl;
							
							//设置IP地址动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, IPAddrCbBoxId );
							if( VstrInit( &g_MediaInfoPt->m_IPAddrVstrPt, Utf16, , ) != 0 )
							{
								LOGE( Cu8vstr( "初始化IP地址动态字符串失败。" ) );
								goto OutCreateSrvrOrClnt;
							}
							VstrSetSz( g_MediaInfoPt->m_IPAddrVstrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, IPAddrCbBoxId, ( wchar_t * )g_MediaInfoPt->m_IPAddrVstrPt->m_StrPt, g_MediaInfoPt->m_IPAddrVstrPt->m_StrSz );

							//设置端口动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, PortEdTxtId );
							if( VstrInit( &g_MediaInfoPt->m_PortVstrPt, Utf16, , ) != 0 )
							{
								LOGE( Cu8vstr( "初始化端口动态字符串失败。" ) );
								goto OutCreateSrvrOrClnt;
							}
							VstrSetSz( g_MediaInfoPt->m_PortVstrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, PortEdTxtId, ( wchar_t * )g_MediaInfoPt->m_PortVstrPt->m_StrPt, g_MediaInfoPt->m_PortVstrPt->m_StrSz );
							
							//设置使用什么传输协议。
							g_MediaInfoPt->m_UseWhatXfrPrtcl = ( IsDlgButtonChecked( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ) == BST_CHECKED ) ? 0 : 1;
							
							//设置传输模式。
							if( IsDlgButtonChecked( g_XfrPrtclStngDlgWndHdl, UsePttRdBtnId ) == BST_CHECKED )
							{
								g_MediaInfoPt->m_XfrMode = 0;
							}
							else
							{
								g_MediaInfoPt->m_XfrMode = 1;
							}

							//设置最大连接次数。
							{
								wchar_t p_TmpStrPt[100];

								GetWindowText( GetDlgItem( g_XfrPrtclStngDlgWndHdl, MaxCnctTimesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								g_MediaInfoPt->m_MaxCnctTimes = _wtoi( p_TmpStrPt );
							}
							
							//设置创建服务端或者客户端标记。
							g_MediaInfoPt->m_IsCreateSrvrOrClnt = ( LOWORD( wParam ) == CreateSrvrBtnId ) ? 1 : 0;

							//设置是否自动允许连接。
							g_MediaInfoPt->m_IsAutoAllowCnct = ( IsDlgButtonChecked( g_XfrPrtclStngDlgWndHdl, IsAutoAllowCnctCkBoxId ) == BST_CHECKED ) ? 1 : 0;
						}
						
						//设置是否使用链表。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseLnkLstRecvOtptFrmRdBtnId ) == BST_CHECKED )
						{
							g_MediaInfoPt->m_UseWhatRecvOtptFrm = 0;
						}
						
						//设置是否使用自己设计的自适应抖动缓冲器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseAjbRecvOtptFrmRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int p_AAjbMinNeedBufFrmCnt;
							int p_AAjbMaxNeedBufFrmCnt;
							int p_AAjbMaxCntuLostFrmCnt;
							float p_AAjbAdaptSensitivity;
							int p_VAjbMinNeedBufFrmCnt;
							int p_VAjbMaxNeedBufFrmCnt;
							float p_VAjbAdaptSensitivity;

							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMinNeedBufFrmCnt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMaxNeedBufFrmCnt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMaxCntuLostFrmCnt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbAdaptSensitivity = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMinNeedBufFrmCnt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMaxNeedBufFrmCnt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbAdaptSensitivity = _wtof( p_TmpStrPt );

							g_MediaInfoPt->m_UseWhatRecvOtptFrm = 1;
							g_MediaInfoPt->m_AAjbMinNeedBufFrmCnt = p_AAjbMinNeedBufFrmCnt;
							g_MediaInfoPt->m_AAjbMaxNeedBufFrmCnt = p_AAjbMaxNeedBufFrmCnt;
							g_MediaInfoPt->m_AAjbMaxCntuLostFrmCnt = p_AAjbMaxCntuLostFrmCnt;
							g_MediaInfoPt->m_AAjbAdaptSensitivity = p_AAjbAdaptSensitivity;
							g_MediaInfoPt->m_VAjbMinNeedBufFrmCnt = p_VAjbMinNeedBufFrmCnt;
							g_MediaInfoPt->m_VAjbMaxNeedBufFrmCnt = p_VAjbMaxNeedBufFrmCnt;
							g_MediaInfoPt->m_VAjbAdaptSensitivity = p_VAjbAdaptSensitivity;
						}

						//设置是否保存设置到文件。
						g_MediaPocsThrd.SetIsSaveStngToFile( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															 ".\\Setting.txt",
															 &g_ErrInfoVstr );
						
						//设置是否打印Log日志、显示Toast。
						g_MediaPocsThrd.SetIsPrintLogShowToast( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																&g_ErrInfoVstr );

						//设置是否使用音频输入。
						g_MediaPocsThrd.SetIsUseAdoInpt( ( g_MediaInfoPt->m_XfrMode == 0 ) ? 0 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 :
														     ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
														 &g_ErrInfoVstr );

						if( g_MediaInfoPt->m_XfrMode == 0 ) //如果传输模式为实时半双工。
						{
							g_MediaPocsThrd.SetAdoInptUseNoAec( &g_ErrInfoVstr );
						}
						else //如果传输模式为实时全双工。
						{
							//设置音频输入是否不使用声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED )
							{
								g_MediaPocsThrd.SetAdoInptUseNoAec( &g_ErrInfoVstr );
							}

							//设置音频输入是否使用Speex声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_FilterLen;
								int32_t p_IsUseRec;
								float p_EchoMultiple;
								float p_EchoCont;
								int32_t p_EchoSupes;
								int32_t p_EchoSupesAct;
								int p_IsSaveMemFile;

								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_FilterLen = _wtoi( p_TmpStrPt );
								p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoMultiple = _wtof( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoCont = _wtof( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoSupes = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoSupesAct = _wtoi( p_TmpStrPt );
								p_IsSaveMemFile = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

								g_MediaPocsThrd.SetAdoInptUseSpeexAec( p_FilterLen, p_IsUseRec, p_EchoMultiple, p_EchoCont, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, Cu8vstr( ".\\SpeexAecMem" ), &g_ErrInfoVstr );
							}

							//设置音频输入是否使用WebRtc定点版声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecmRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_IsUseCNGMode;
								int32_t p_EchoMode;
								int32_t p_Delay;

								p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
								GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoMode = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_Delay = _wtoi( p_TmpStrPt );

								g_MediaPocsThrd.SetAdoInptUseWebRtcAecm( p_IsUseCNGMode, p_EchoMode, p_Delay, &g_ErrInfoVstr );
							}

							//设置音频输入是否使用WebRtc浮点版声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_EchoMode;
								int32_t p_Delay;
								int32_t p_IsUseDelayAgstcMode;
								int32_t p_IsUseExtdFilterMode;
								int32_t p_IsUseRefinedFilterAdaptAecMode;
								int32_t p_IsUseAdaptAdjDelay;
								int p_IsSaveMemFile;

								GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_EchoMode = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_Delay = _wtoi( p_TmpStrPt );
								p_IsUseDelayAgstcMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
								p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
								p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
								p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
								p_IsSaveMemFile = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

								g_MediaPocsThrd.SetAdoInptUseWebRtcAec( p_EchoMode, p_Delay, p_IsUseDelayAgstcMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, Cu8vstr( ".\\WebRtcAecMem" ), &g_ErrInfoVstr );
							}

							//设置音频输入是否使用SpeexWebRtc三重声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_WorkMode;
								int32_t p_SpeexAecFilterLen;
								int32_t p_SpeexAecIsUseRec;
								float p_SpeexAecEchoMultiple;
								float p_SpeexAecEchoCont;
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
								int32_t p_IsUseSameRoomAec;
								int32_t p_SameRoomEchoMinDelay;

								p_WorkMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId ) == BST_CHECKED ) ? 1 :
											   ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? 2 :
												 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? 3 : 0;
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecFilterLen = _wtoi( p_TmpStrPt );
								p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecEchoMultiple = _wtof( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecEchoCont = _wtof( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecEchoSupes = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecEchoSupesAct = _wtoi( p_TmpStrPt );
								p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_WebRtcAecmEchoMode = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_WebRtcAecmDelay = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_WebRtcAecEchoMode = _wtoi( p_TmpStrPt );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_WebRtcAecDelay = _wtoi( p_TmpStrPt );
								p_WebRtcAecIsUseDelayAgstcMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
								p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
								p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
								p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
								p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId ) == BST_CHECKED );
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SameRoomEchoMinDelay = _wtoi( p_TmpStrPt );

								g_MediaPocsThrd.SetAdoInptUseSpeexWebRtcAec( p_WorkMode, p_SpeexAecFilterLen, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCont, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgstcMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, &g_ErrInfoVstr );
							}
						}

						//设置音频输入是否不使用噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoInptUseNoNs( &g_ErrInfoVstr );
						}

						//设置音频输入是否使用Speex预处理器的噪音抑制。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexPrpocsNsRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_IsUseNs;
							int32_t p_NoiseSupes;
							int32_t p_IsUseDereverb;

							p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_NoiseSupes = _wtoi( p_TmpStrPt );
							p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId ) == BST_CHECKED );

							g_MediaPocsThrd.SetAdoInptUseSpeexPrpocsNs( p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, &g_ErrInfoVstr );
						}

						//设置音频输入是否使用WebRtc定点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = _wtoi( p_TmpStrPt );

							g_MediaPocsThrd.SetAdoInptUseWebRtcNsx( p_PolicyMode, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用WebRtc浮点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = _wtoi( p_TmpStrPt );

							g_MediaPocsThrd.SetAdoInptUseWebRtcNs( p_PolicyMode, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用RNNoise噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoInptUseRNNoise( &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用Speex预处理器的其他功能。
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_IsUseVad;
							int32_t p_VadProbStart;
							int32_t p_VadProbCont;
							int32_t p_IsUseAgc;
							int32_t p_AgcLevel;
							int32_t p_AgcIncrement;
							int32_t p_AgcDecrement;
							int32_t p_AgcMaxGain;

							p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbStart = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbCont = _wtoi( p_TmpStrPt );
							p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcLevel = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcIncrement = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcDecrement = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcMaxGain = _wtoi( p_TmpStrPt );

							g_MediaPocsThrd.SetAdoInptIsUseSpeexPrpocsOther( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId ) == BST_CHECKED ) ? 1 : 0, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoInptUsePcm( &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用Speex编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_EncdUseCbrOrVbr;
							int32_t p_EncdQualt;
							int32_t p_EncdCmplxt;
							int32_t p_EncdPlcExptLossRate;

							p_EncdUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId ) == BST_CHECKED ) ? 0 : 1;
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncdQualt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncdCmplxt = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncdPlcExptLossRate = _wtoi( p_TmpStrPt );
							
							g_MediaPocsThrd.SetAdoInptUseSpeexEncd( p_EncdUseCbrOrVbr, p_EncdQualt, p_EncdCmplxt, p_EncdPlcExptLossRate, &g_ErrInfoVstr );
						}

						//设置音频输入是否使用Opus编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoInptUseOpusEncd( &g_ErrInfoVstr );
						}
						
						//设置音频输入是否保存音频到文件。
						g_MediaPocsThrd.SetAdoInptIsSaveAdoToFile( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																   Cu8vstr( ".\\AdoInpt.wav" ),
																   Cu8vstr( ".\\AdoRslt.wav" ),
																   &g_ErrInfoVstr );
						
						//设置音频输入是否绘制音频波形到窗口。
						g_MediaPocsThrd.SetAdoInptIsDrawAdoWavfmToWnd( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																	   GetDlgItem( g_MainDlgWndHdl, AdoInptWavfmTxtId ),
																	   GetDlgItem( g_MainDlgWndHdl, AdoRsltWavfmTxtId ),
																	   &g_ErrInfoVstr );
						
						//设置音频输入使用的设备。
						g_MediaPocsThrd.SetAdoInptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														  &g_ErrInfoVstr );

						//设置音频输入是否静音。
						g_MediaPocsThrd.SetAdoInptIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														  & g_ErrInfoVstr );

						//设置是否使用音频输出。
						g_MediaPocsThrd.SetIsUseAdoOtpt( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
														 &g_ErrInfoVstr );
						g_MediaPocsThrd.AddAdoOtptStrm( 0, &g_ErrInfoVstr );
						
						//设置音频输出是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoOtptStrmUsePcm( 0, &g_ErrInfoVstr );
						}
						
						//设置音频输出是否使用Speex解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
						{
							int32_t p_DecdIsUsePrcplEnhsmt;

							p_DecdIsUsePrcplEnhsmt = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId ) == BST_CHECKED );

							g_MediaPocsThrd.SetAdoOtptStrmUseSpeexDecd( 0, p_DecdIsUsePrcplEnhsmt, &g_ErrInfoVstr );
						}

						//设置音频输出是否使用Opus解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetAdoOtptStrmUseOpusDecd( 0, &g_ErrInfoVstr );
						}
						
						//设置音频输出是否保存音频到文件。
						g_MediaPocsThrd.SetAdoOtptIsSaveAdoToFile( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																   Cu8vstr( ".\\AdoOtpt.wav" ),
																   & g_ErrInfoVstr );
						
						//设置音频输出是否绘制音频波形到窗口。
						g_MediaPocsThrd.SetAdoOtptIsDrawAdoWavfmToWnd( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																	   GetDlgItem( g_MainDlgWndHdl, AdoOtptWavfmTxtId ),
																	   & g_ErrInfoVstr );
						
						//设置音频输出使用的设备。
						g_MediaPocsThrd.SetAdoOtptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														  &g_ErrInfoVstr );
						
						//设置音频输出是否静音。
						g_MediaPocsThrd.SetAdoOtptIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														  &g_ErrInfoVstr );
						
						//设置是否使用视频输入。
						g_MediaPocsThrd.SetIsUseVdoInpt( ( g_MediaInfoPt->m_XfrMode == 0 ) ? 0 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 :
														     ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId ) == BST_CHECKED ) ? 120 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize240_320RdBtnId ) == BST_CHECKED ) ? 240 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize480_640RdBtnId ) == BST_CHECKED ) ? 480 :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize960_1280RdBtnId ) == BST_CHECKED ) ? 960 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId ) == BST_CHECKED ) ? 160 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize240_320RdBtnId ) == BST_CHECKED ) ? 320 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize480_640RdBtnId ) == BST_CHECKED ) ? 640 :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSize960_1280RdBtnId ) == BST_CHECKED ) ? 1280 : 0,
														 GetDlgItem( g_MainDlgWndHdl, VdoInptPrvwTxtId ),
														 &g_ErrInfoVstr );
						
						//设置视频输入是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetVdoInptUseYU12( &g_ErrInfoVstr );
						}
						
						//设置视频输入是否使用OpenH264编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_OpenH264EncdVdoType;
							int32_t p_OpenH264EncdEncdBitrate;
							int32_t p_OpenH264EncdBitrateCtrlMode;
							int32_t p_OpenH264EncdIDRFrmIntvl;
							int32_t p_OpenH264EncdCmplxt;

							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncdVdoType = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncdEncdBitrate = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncdBitrateCtrlMode = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncdIDRFrmIntvl = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncdCmplxt = _wtoi( p_TmpStrPt );

							g_MediaPocsThrd.SetVdoInptUseOpenH264Encd( p_OpenH264EncdVdoType,
																	   p_OpenH264EncdEncdBitrate * 1024 * 8,
																	   p_OpenH264EncdBitrateCtrlMode,
																	   p_OpenH264EncdIDRFrmIntvl,
																	   p_OpenH264EncdCmplxt,
																	   &g_ErrInfoVstr );
						}
						
						//设置视频输入使用的设备。
						g_MediaPocsThrd.SetVdoInptUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														  &g_ErrInfoVstr );
						
						//设置视频输入是否黑屏。
						g_MediaPocsThrd.SetVdoInptIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														   &g_ErrInfoVstr );

						//设置是否使用视频输出。
						g_MediaPocsThrd.SetIsUseVdoOtpt( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 GetDlgItem( g_MainDlgWndHdl, VdoOtptDspyTxtId ),
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId ) == BST_CHECKED ) ? 1.0f :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoDspyScale1_5RdBtnId ) == BST_CHECKED ) ? 1.5f :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoDspyScale2_0RdBtnId ) == BST_CHECKED ) ? 2.0f :
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoDspyScale3_0RdBtnId ) == BST_CHECKED ) ? 3.0f : 0,
														 &g_ErrInfoVstr );
						
						//设置视频输出是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetVdoOtptUseYU12( &g_ErrInfoVstr );
						}
						
						//设置视频输出是否使用OpenH264解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
						{
							g_MediaPocsThrd.SetVdoOtptUseOpenH264Decd( 0, &g_ErrInfoVstr );
						}
						
						//设置视频输出是否黑屏。
						g_MediaPocsThrd.SetVdoOtptIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														   &g_ErrInfoVstr );

						//启动媒体处理线程。
						if( g_MediaPocsThrd.Start( &g_ErrInfoVstr ) != 0 )
						{
							LOGFE( Cu8vstr( "启动媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
							goto OutCreateSrvrOrClnt;
						}

						LOGI( Cu8vstr( "启动媒体处理线程完毕。" ) );

						p_Rslt = 0; // //设置本函数执行成功。

						OutCreateSrvrOrClnt:
						if( p_Rslt != 0 )
						{
							if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
							{
								g_MediaPocsThrd.RqirExit( 1, 1, &g_ErrInfoVstr ); //请求媒体处理线程退出。
								g_MediaPocsThrd.Dstoy( &g_ErrInfoVstr ); //销毁媒体处理线程。
							}
							if( g_MediaInfoPt != NULL )
							{
								VstrDstoy( g_MediaInfoPt->m_IPAddrVstrPt );
								VstrDstoy( g_MediaInfoPt->m_PortVstrPt );
								free( g_MediaInfoPt );
								g_MediaInfoPt = NULL;
							}
						}
					}
					else //如果媒体处理线程已经启动。
					{
						g_MediaPocsThrd.RqirExit( 1, 1, &g_ErrInfoVstr ); //请求媒体处理线程退出。

						p_Rslt = 0; // //设置本函数执行成功。
					}
					return ( p_Rslt == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
				}
				case RqstCnctAllowBtnId: //请求连接对话框的允许按钮。
				{
					g_MediaInfoPt->m_RqstCnctRslt = 1;
					EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框。
					return ( INT_PTR )TRUE;
				}
				case RqstCnctRefuseBtnId: //请求连接对话框的拒绝按钮。
				{
					g_MediaInfoPt->m_RqstCnctRslt = 2;
					EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框。
					return ( INT_PTR )TRUE;
				}
				case XfrPrtclStngBtnId: //传输协议设置按钮。
				{
					ShowWindow( g_XfrPrtclStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示传输协议设置对话框。
					return ( INT_PTR )TRUE;
				}
				case StngBtnId: //设置按钮。
				{
					ShowWindow( g_StngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示设置对话框。
					return ( INT_PTR )TRUE;
				}
				case AjbStngBtnId: //自适应抖动缓冲器设置按钮。
				{
					ShowWindow( g_AjbStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case UseEffectLowRdBtnId: //效果等级：低。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate8000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecmRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseSpeexPrpocsNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate12RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId, UseVdoFrmSize960_1280RdBtnId, UseVdoFrmSize120_160RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId, UseVdoDspyScale3_0RdBtnId, UseVdoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), L"3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), L"0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), L"0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), L"1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), L"0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), L"380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), L"3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), L"3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseCbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"1" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"12" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectMidRdBtnId: //效果等级：中。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectMidRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsxRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId, UseVdoFrmSize960_1280RdBtnId, UseVdoFrmSize240_320RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId, UseVdoDspyScale3_0RdBtnId, UseVdoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), L"3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), L"0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), L"0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), L"1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), L"0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), L"380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), L"3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), L"3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseCbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"4" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectHighRdBtnId: //效果等级：高。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectHighRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId, UseVdoFrmSize960_1280RdBtnId, UseVdoFrmSize480_640RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId, UseVdoDspyScale3_0RdBtnId, UseVdoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), L"3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), L"0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), L"0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), L"1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), L"0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), L"380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), L"3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), L"3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"8" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectSuperRdBtnId: //效果等级：超。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectSuperRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate24RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId, UseVdoFrmSize960_1280RdBtnId, UseVdoFrmSize480_640RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId, UseVdoDspyScale3_0RdBtnId, UseVdoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), L"3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), L"0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), L"0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), L"1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), L"0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), L"380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), L"3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), L"3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"10" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"24" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"1" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectPremiumRdBtnId: //效果等级：特。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectPremiumRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate32000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate30RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSize120_160RdBtnId, UseVdoFrmSize960_1280RdBtnId, UseVdoFrmSize960_1280RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoDspyScale1_0RdBtnId, UseVdoDspyScale3_0RdBtnId, UseVdoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), L"3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), L"0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), L"0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), L"500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), L"1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), L"0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), L"-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), L"2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), L"0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), L"380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), L"-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), L"3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), L"3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"10" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"30" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"2" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateLowRdBtnId: //比特率等级：低。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateLowRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"1" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"1" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"10" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateMidRdBtnId: //比特率等级：中。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateMidRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"4" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"40" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"20" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateHighRdBtnId: //比特率等级：高。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateHighRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"8" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"80" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"40" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateSuperRdBtnId: //比特率等级：超。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateSuperRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"60" );

					return ( INT_PTR )TRUE;
				}
				case UseBitratePremiumRdBtnId: //比特率等级：特。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitratePremiumRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"80" );

					return ( INT_PTR )TRUE;
				}
				case SpeexAecStngBtnId: //Speex声学回音消除器设置按钮。
				{
					ShowWindow( g_SpeexAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecmStngBtnId: //WebRtc定点版声学回音消除器设置按钮。
				{
					ShowWindow( g_WebRtcAecmStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecStngBtnId: //WebRtc浮点版声学回音消除器设置按钮。
				{
					ShowWindow( g_WebRtcAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexWebRtcAecStngBtnId: //SpeexWebRtc三重声学回音消除器设置按钮。
				{
					ShowWindow( g_SpeexWebRtcAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示SpeexWebRtc三重声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsNsStngBtnId: //Speex预处理器的噪音抑制设置按钮。
				{
					ShowWindow( g_SpeexPrpocsNsStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的噪音抑制设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsxStngBtnId: //WebRtc定点版噪音抑制器设置按钮。
				{
					ShowWindow( g_WebRtcNsxStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版噪音抑制器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsStngBtnId: //WebRtc浮点版噪音抑制器设置按钮。
				{
					ShowWindow( g_WebRtcNsStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版噪音抑制器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsOtherStngBtnId: //Speex预处理器的其他功能设置按钮。
				{
					ShowWindow( g_SpeexPrpocsOtherStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的其他功能设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexCodecStngBtnId: //Speex编解码器设置按钮。
				{
					ShowWindow( g_SpeexCodecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex编解码器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case OpusCodecStngBtnId: //Opus编解码器设置按钮。
				{
					return ( INT_PTR )TRUE;
				}
				case OpenH264CodecStngBtnId: //OpenH264编解码器设置按钮。
				{
					ShowWindow( g_OpenH264CodecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示OpenH264编解码器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case ReadMeBtnId: //必读说明按钮。
				{
					ShellExecute( NULL, L"open", L"https://github.com/cyz7758520/Windows_audio_talkback_demo_program", NULL, NULL, SW_SHOWNORMAL ); //打开必读说明网页。
					return ( INT_PTR )TRUE;
				}
				case ClearLogBtnId: //清空日志按钮。
				{
					SendMessage( GetDlgItem( hDlg, LogLtBoxId ), LB_RESETCONTENT, 0, 0 ); //清空日志列表。
					SendDlgItemMessage( hDlg, LogLtBoxId, LB_SETHORIZONTALEXTENT, 0, 0 ); //清空滚动条的当前像素大小。
					return ( INT_PTR )TRUE;
				}
				case VdoInptPrvwTxtId: //视频输入预览文本框。
				{
					if( g_VdoInptPrvwTxtWndIsMax == 0 ) //如果视频输入预览文本框窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainDlgWndHdl, &p_Rect );
						SetWindowPos( g_VdoInptPrvwTxtWndHdl, g_VdoOtptDspyTxtWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_HIDE ); //设置TCP协议单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_HIDE ); //设置UDP协议单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_HIDE ); //设置传输协议设置按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_HIDE ); //设置IP地址组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_HIDE ); //设置端口编辑框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ), SW_HIDE ); //设置音频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ), SW_HIDE ); //设置视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ), SW_HIDE ); //设置音视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_HIDE ); //设置音频输入设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_HIDE ); //设置音频输出设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_HIDE ); //设置视频输入设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_HIDE ); //设置创建服务端按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_HIDE ); //设置连接服务端按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptWavfmTxtId ), SW_HIDE ); //设置音频输入波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptWavfmTxtId ), SW_HIDE ); //设置音频输出波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoRsltWavfmTxtId ), SW_HIDE ); //设置音频结果波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表框为隐藏。
						ShowWindow( g_VdoOtptDspyTxtWndHdl, SW_HIDE ); //设置视频输出显示文本框为隐藏。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VdoInptPrvwTxtWndIsMax = 1;
					}
					else //如果视频输入预览文本框窗口已经最大化。
					{
						SetWindowPos( g_VdoInptPrvwTxtWndHdl, g_VdoOtptDspyTxtWndHdl, g_VdoInptPrvwTxtWndRect.left, g_VdoInptPrvwTxtWndRect.top, g_VdoInptPrvwTxtWndRect.right - g_VdoInptPrvwTxtWndRect.left, g_VdoInptPrvwTxtWndRect.bottom - g_VdoInptPrvwTxtWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_SHOW ); //设置TCP协议单选按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_SHOW ); //设置UDP协议单选按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_SHOW ); //设置传输协议设置按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_SHOW ); //设置IP地址组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_SHOW ); //设置端口编辑框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ), SW_SHOW ); //设置音频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ), SW_SHOW ); //设置视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ), SW_SHOW ); //设置音视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_SHOW ); //设置音频输入设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_SHOW ); //设置音频输出设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_SHOW ); //设置视频输入设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_SHOW ); //设置音频输入静音复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_SHOW ); //设置音频输出静音复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_SHOW ); //设置视频输入黑屏复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_SHOW ); //设置视频输出黑屏复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_SHOW ); //设置创建服务端按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_SHOW ); //设置连接服务端按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_SHOW ); //设置设置按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_SHOW ); //设置必读说明按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptWavfmTxtId ), SW_SHOW ); //设置音频输入波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptWavfmTxtId ), SW_SHOW ); //设置音频输出波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoRsltWavfmTxtId ), SW_SHOW ); //设置音频结果波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_SHOW ); //设置日志列表框为显示。
						ShowWindow( g_VdoOtptDspyTxtWndHdl, SW_SHOW ); //设置视频输出显示文本框为显示。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VdoInptPrvwTxtWndIsMax = 0;
					}
					return ( INT_PTR )TRUE;
				}
				case VdoOtptDspyTxtId: //视频输出显示文本框。
				{
					if( g_VdoOtptDspyTxtWndIsMax == 0 ) //如果视频输出显示文本框窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainDlgWndHdl, &p_Rect );
						SetWindowPos( g_VdoOtptDspyTxtWndHdl, g_VdoInptPrvwTxtWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_HIDE ); //设置TCP协议单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_HIDE ); //设置UDP协议单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_HIDE ); //设置传输协议设置按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_HIDE ); //设置IP地址组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_HIDE ); //设置端口编辑框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ), SW_HIDE ); //设置音频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ), SW_HIDE ); //设置视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ), SW_HIDE ); //设置音视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_HIDE ); //设置音频输入设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_HIDE ); //设置音频输出设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_HIDE ); //设置视频输入设备组合框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏复选框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_HIDE ); //设置创建服务端按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_HIDE ); //设置连接服务端按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptWavfmTxtId ), SW_HIDE ); //设置音频输入波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptWavfmTxtId ), SW_HIDE ); //设置音频输出波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoRsltWavfmTxtId ), SW_HIDE ); //设置音频结果波形文本框为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表框为隐藏。
						ShowWindow( g_VdoInptPrvwTxtWndHdl, SW_HIDE ); //设置视频输入预览文本框为隐藏。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VdoOtptDspyTxtWndIsMax = 1;
					}
					else //如果视频输出显示文本框窗口已经最大化。
					{
						SetWindowPos( g_VdoOtptDspyTxtWndHdl, g_VdoInptPrvwTxtWndHdl, g_VdoOtptDspyTxtWndRect.left, g_VdoOtptDspyTxtWndRect.top, g_VdoOtptDspyTxtWndRect.right - g_VdoOtptDspyTxtWndRect.left, g_VdoOtptDspyTxtWndRect.bottom - g_VdoOtptDspyTxtWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_SHOW ); //设置TCP协议单选按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_SHOW ); //设置UDP协议单选按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_SHOW ); //设置传输协议设置按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_SHOW ); //设置IP地址组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_SHOW ); //设置端口编辑框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ), SW_SHOW ); //设置音频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ), SW_SHOW ); //设置视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ), SW_SHOW ); //设置音视频对讲模式单选按钮为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_SHOW ); //设置音频输入设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_SHOW ); //设置音频输出设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_SHOW ); //设置视频输入设备组合框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_SHOW ); //设置音频输入静音复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_SHOW ); //设置音频输出静音复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_SHOW ); //设置视频输入黑屏复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_SHOW ); //设置视频输出黑屏复选框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_SHOW ); //设置创建服务端按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_SHOW ); //设置连接服务端按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_SHOW ); //设置设置按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_SHOW ); //设置必读说明按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptWavfmTxtId ), SW_SHOW ); //设置音频输入波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptWavfmTxtId ), SW_SHOW ); //设置音频输出波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoRsltWavfmTxtId ), SW_SHOW ); //设置音频结果波形文本框为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志按钮为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_SHOW ); //设置日志列表框为显示。
						ShowWindow( g_VdoInptPrvwTxtWndHdl, SW_SHOW ); //设置视频输入预览文本框为显示。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VdoOtptDspyTxtWndIsMax = 0;
					}
					return ( INT_PTR )TRUE;
				}
				case XfrPrtclStngOkBtnId: //传输协议设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_XfrPrtclStngDlgWndHdl ), TRUE ), ShowWindow( g_XfrPrtclStngDlgWndHdl, SW_HIDE ); //隐藏传输协议设置对话框。
					return ( INT_PTR )TRUE;
				}
				case StngOkBtnId: //设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_StngDlgWndHdl ), TRUE ), ShowWindow( g_StngDlgWndHdl, SW_HIDE ); //隐藏设置对话框。
					return ( INT_PTR )TRUE;
				}
				case AjbStngOkBtnId: //自适应抖动缓冲器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_AjbStngDlgWndHdl ), TRUE ), ShowWindow( g_AjbStngDlgWndHdl, SW_HIDE ); //隐藏自适应抖动缓冲器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexAecDelMemFileBtnId: //Speex声学回音消除器的删除内存块文件按钮。
				{
					if( DeleteFile( L".\\SpeexAecMem" ) != 0 )
					{
						MessageBox( hDlg, L"删除Speex声学回音消除器的内存块文件 SpeexAecMem 成功。", L"Windows下音视频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						uint16_t * p_TmpU16strPt;
						GetLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除Speex声学回音消除器的内存块文件 SpeexAecMem 失败。原因：" ) );
						StrU16TmpCpy( p_TmpU16strPt, g_ErrInfoVstr.m_VstrPt->m_StrPt, g_ErrInfoVstr.m_VstrPt->m_StrChrSet );
						MessageBox( hDlg, ( wchar_t * )p_TmpU16strPt, L"Windows下音视频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case SpeexAecStngOkBtnId: //Speex声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexAecStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexAecStngDlgWndHdl, SW_HIDE ); //隐藏Speex声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecmStngOkBtnId: //WebRtc定点版声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcAecmStngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcAecmStngDlgWndHdl, SW_HIDE ); //隐藏WebRtc定点版声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecDelMemFileBtnId: //WebRtc浮点版声学回音消除器的删除内存块文件按钮。
				{
					if( DeleteFile( L".\\WebRtcAecMem" ) != 0 )
					{
						MessageBox( hDlg, L"删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 成功。", L"Windows下音视频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						uint16_t * p_TmpU16strPt;
						GetLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 失败。原因：" ) );
						StrU16TmpCpy( p_TmpU16strPt, g_ErrInfoVstr.m_VstrPt->m_StrPt, g_ErrInfoVstr.m_VstrPt->m_StrChrSet );
						MessageBox( hDlg, ( wchar_t * )p_TmpU16strPt, L"Windows下音视频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecStngOkBtnId: //WebRtc浮点版声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcAecStngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcAecStngDlgWndHdl, SW_HIDE ); //隐藏WebRtc浮点版声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexWebRtcAecStngOkBtnId: //SpeexWebRtc三重声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexWebRtcAecStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexWebRtcAecStngDlgWndHdl, SW_HIDE ); //隐藏SpeexWebRtc三重声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsNsStngOkBtnId: //Speex预处理器的噪音抑制设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexPrpocsNsStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexPrpocsNsStngDlgWndHdl, SW_HIDE ); //隐藏Speex预处理器的噪音抑制设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsxStngOkBtnId: //WebRtc定点版噪音抑制器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcNsxStngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcNsxStngDlgWndHdl, SW_HIDE ); //隐藏WebRtc定点版噪音抑制器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsStngOkBtnId: //WebRtc浮点版噪音抑制器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcNsStngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcNsStngDlgWndHdl, SW_HIDE ); //隐藏WebRtc浮点版噪音抑制器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsOtherStngOkBtnId: //Speex预处理器的其他功能设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexPrpocsOtherStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexPrpocsOtherStngDlgWndHdl, SW_HIDE ); //隐藏Speex预处理器的其他功能设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexCodecStngOkBtnId: //Speex编解码器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexCodecStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexCodecStngDlgWndHdl, SW_HIDE ); //隐藏Speex编解码器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case OpenH264CodecStngOkBtnId: //OpenH264编解码器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_OpenH264CodecStngDlgWndHdl ), TRUE ), ShowWindow( g_OpenH264CodecStngDlgWndHdl, SW_HIDE ); //隐藏OpenH264编解码器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case IDCANCEL: //窗口右上角关闭按钮。
				{
					if( g_MainDlgWndHdl == hDlg ) //如果是主对话框。
					{
						LOGI( Cu8vstr( "用户在主对话框按下关闭按钮，本软件退出。" ) );

						g_MediaPocsThrd.RqirExit( 1, 1, &g_ErrInfoVstr ); //请求媒体处理线程退出。

						PostQuitMessage( 0 ); //发送退出消息。
					}
					else //如果是其他对话框。
					{
						EnableWindow( GetParent( hDlg ), TRUE ), ShowWindow( hDlg, SW_HIDE ); //隐藏当前对话框。
					}
					return ( INT_PTR )TRUE;
				}
			}
			break;
		}
		case WM_DESTROY: //窗口销毁消息。
		{
			return ( INT_PTR )TRUE;
		}
		case WM_INIT_MEDIA_PROC_THREAD: //初始化媒体处理线程的消息。
		{
			if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
            {
				EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), FALSE ); //设置TCP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), FALSE ); //设置UDP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), FALSE ); //设置传输协议设置按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), FALSE ); //设置IP地址组合框为不可用。
				EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), FALSE ); //设置端口编辑框为不可用。
				SetWindowText( GetDlgItem( hDlg, CreateSrvrBtnId ), L"中断" ); //设置创建服务端按钮的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, CnctSrvrBtnId ), FALSE ); //设置连接服务端按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, StngBtnId ), FALSE ); //设置设置按钮为不可用。
				if( ( g_MediaInfoPt != NULL ) && ( g_MediaInfoPt->m_XfrMode == 0 ) )
				{
					ShowWindow( g_PttDlgWndHdl, SW_SHOW ); //设置一键即按即通对话框为显示。
				}
            }
            else //如果是创建客户端。
            {
				EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), FALSE ); //设置TCP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), FALSE ); //设置UDP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), FALSE ); //设置传输协议设置按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), FALSE ); //设置IP地址组合框为不可用。
				EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), FALSE ); //设置端口编辑框为不可用。
				EnableWindow( GetDlgItem( hDlg, CreateSrvrBtnId ), FALSE ); //设置创建服务端按钮为不可用。
				SetWindowText( GetDlgItem( hDlg, CnctSrvrBtnId ), L"中断" ); //设置连接服务端按钮的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, StngBtnId ), FALSE ); //设置设置按钮为不可用。
				if( ( g_MediaInfoPt != NULL ) && ( g_MediaInfoPt->m_XfrMode == 0 ) )
				{
					ShowWindow( g_PttDlgWndHdl, SW_SHOW ); //设置一键即按即通对话框为显示。
				}
            }
			return ( INT_PTR )TRUE;
		}
		case WM_DSTRY_MEDIA_PROC_THREAD: //销毁媒体处理线程的消息。
		{
			if( g_MediaPocsThrd.m_MediaPocsThrdPt != NULL )
			{
				g_MediaPocsThrd.Dstoy( &g_ErrInfoVstr ); //销毁媒体处理线程。
			}
			if( g_MediaInfoPt != NULL )
			{
				VstrDstoy( g_MediaInfoPt->m_IPAddrVstrPt );
				VstrDstoy( g_MediaInfoPt->m_PortVstrPt );
				if( g_MediaInfoPt->m_TmpBytePt != NULL ) free( g_MediaInfoPt->m_TmpBytePt );
				if( g_MediaInfoPt->m_TmpByte2Pt != NULL ) free( g_MediaInfoPt->m_TmpByte2Pt );
				if( g_MediaInfoPt->m_TmpByte3Pt != NULL ) free( g_MediaInfoPt->m_TmpByte3Pt );
				free( g_MediaInfoPt );
				g_MediaInfoPt = NULL;
			}

			EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), TRUE ); //设置TCP协议单选按钮为可用。
			EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), TRUE ); //设置UDP协议单选按钮为可用。
			EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), TRUE ); //设置传输协议设置按钮为可用。
			EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), TRUE ); //设置IP地址组合框为可用。
			EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), TRUE ); //设置端口编辑框为可用。
			EnableWindow( GetDlgItem( hDlg, CreateSrvrBtnId ), TRUE ); //设置创建服务端按钮为可用。
			SetWindowText( GetDlgItem( hDlg, CreateSrvrBtnId ), L"创建服务端" ); //设置创建服务端按钮的内容为“创建服务端”。
			EnableWindow( GetDlgItem( hDlg, CnctSrvrBtnId ), TRUE ); //设置连接服务端按钮为可用。
			SetWindowText( GetDlgItem( hDlg, CnctSrvrBtnId ), L"连接服务端" ); //设置连接服务端按钮的内容为“连接服务端”。
			EnableWindow( GetDlgItem( hDlg, StngBtnId ), TRUE ); //设置设置按钮为可用。
			ShowWindow( g_PttDlgWndHdl, SW_HIDE ); //设置一键即按即通对话框为隐藏。
			return ( INT_PTR )TRUE;
		}
		case WM_SHOW_RQST_CNCT_DIALOG: //显示请求连接对话框的消息。
		{
			if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 )
			{
				VstrIns( ( Vstr * )wParam, 0, Cu8vstr( "您是否允许远端[" ) );
				VstrCat( ( Vstr * )wParam, Cu8vstr( "]的连接？" ) );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( wchar_t * )( ( Vstr * )wParam )->m_StrPt ); //设置请求连接对话框文本框的内容。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), L"允许" ); //设置请求连接对话框允许按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_SHOW ); //设置请求连接对话框允许按钮为显示。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), L"拒绝" ); //设置请求连接对话框拒绝按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝按钮为显示。
			}
			else
			{
				VstrIns( ( Vstr * )wParam, 0, Cu8vstr( "等待远端[" ) );
				VstrCat( ( Vstr * )wParam, Cu8vstr( "]允许您的连接..." ) );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( wchar_t * )( ( Vstr * )wParam )->m_StrPt ); //设置请求连接对话框的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_HIDE ); //设置请求连接对话框允许按钮为隐藏。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), L"中断" ); //设置请求连接对话框拒绝按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝按钮为显示。
			}
			VstrDstoy( ( Vstr * )wParam );
			ShowWindow( g_RqstCnctDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示请求连接对话框。
			return ( INT_PTR )TRUE;
		}
		case WM_DSTRY_RQST_CNCT_DIALOG: //销毁请求连接对话框的消息。
		{
			EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框。
			return ( INT_PTR )TRUE;
		}
		case WM_SHOW_LOG: //显示日志的消息。
		{
			if( ( VstrCls * )wParam != NULL )
			{
				Vstr * p_LogVstrPt = ( Vstr * )wParam;
				uint16_t * p_LogU16strTmpPt;

				//显示日志。
				{
					SYSTEMTIME p_SystemTime;

					GetLocalTime( &p_SystemTime );
					VstrFmtIns( p_LogVstrPt, 0, Cu8vstr( "%02.2d:%02.2d:%02.2d %03.3d：" ), p_SystemTime.wHour, p_SystemTime.wMinute, p_SystemTime.wSecond, p_SystemTime.wMilliseconds );
					p_LogU16strTmpPt = ( uint16_t * )alloca( ( p_LogVstrPt->m_StrLen + 1 ) * 2 );
					StrU16TmpCpy( p_LogU16strTmpPt, p_LogVstrPt->m_StrPt, p_LogVstrPt->m_StrChrSet );
					SendMessage( GetDlgItem( hDlg, LogLtBoxId ), LB_ADDSTRING, 0, ( WPARAM )p_LogU16strTmpPt );
				}

				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, LogLtBoxId, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( wchar_t *)p_LogU16strTmpPt, p_LogVstrPt->m_StrLen, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, LogLtBoxId, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}

				VstrDstoy( p_LogVstrPt );
			}
			return ( INT_PTR )TRUE;
		}
		case WM_REPAINT: //重绘消息。
		{
			InvalidateRect( g_VdoInptPrvwTxtWndHdl, NULL, TRUE ); //重绘视频输入预览文本框窗口。
			InvalidateRect( g_VdoOtptDspyTxtWndHdl, NULL, TRUE ); //重绘视频输出显示文本框窗口。
			return ( INT_PTR )TRUE;
		}
	}
	return ( INT_PTR )FALSE;
}

//主函数。
int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR    lpCmdLine,
					   _In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	
	// TODO: 在此处放置代码。
	#ifdef __DEBUG__
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 514 );
	#endif

	g_IstnsHdl = hInstance; //设置当前实例的句柄。
	
	FuncSetCurActPath( NULL, NULL ); //设置当前进程活动目录的路径为为当前进程可执行文件的上级路径。
	
	//初始化错误信息动态字符串。
	g_ErrInfoVstr.Init();

	//初始化日志。
	{
		size_t p_LogNum;
		DeleteFile( L".\\log.txt" );
		LogOpenFile( Cu8vstr( ".\\log.txt" ), &p_LogNum, g_ErrInfoVstr.m_VstrPt );
		LogSetDfltFile( p_LogNum, g_ErrInfoVstr.m_VstrPt );
	}
	
	//初始化套接字库。
	{
		WSADATA p_WsaData;
		WSAStartup( MAKEWORD( 2, 2 ), &p_WsaData );
	}

	//判断应用程序可执行文件的名称是否为“Windows下音视频对讲演示程序.exe”。
	{
		HANDLE p_CurProcHdl;
		wchar_t p_PocsImageFileName[ MAX_PATH ];
		const wchar_t * p_AppFileNamePt;

		//获取Windows应用程序的进程名字符串。
		p_CurProcHdl = GetCurrentProcess();
		GetProcessImageFileName( p_CurProcHdl, p_PocsImageFileName, sizeof( p_PocsImageFileName ) );
		FuncPathFindFileName( Cu16vstr( p_PocsImageFileName ), ( const void * * )&p_AppFileNamePt );
		CloseHandle( p_CurProcHdl );
		if( ( memcmp( p_AppFileNamePt, L"Windows下音视频对讲演示程序.exe", wcslen( L"Windows下音视频对讲演示程序.exe" ) + 1 ) != 0 ) &&
			( memcmp( p_AppFileNamePt, L"WndAdoVdoTkbkDemo", wcslen( L"WndAdoVdoTkbkDemo" ) ) != 0 ) )
		{
			if( MessageBox( NULL, L"当前应用程序可执行文件的名称不为“Windows下音视频对讲演示程序.exe”，运行会报错，是否继续？", L"警告", MB_YESNO | MB_ICONWARNING ) == IDNO )
			{
				return 0;
			}
		}
	}

	//创建窗口。
	{
		g_MainDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( MainDlgId ), NULL, WndMsgPocsPocdr );

		g_RqstCnctDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( RqstCnctDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_PttDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( PttDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_PttBtnWndHdl = GetDlgItem( g_PttDlgWndHdl, PttBtnId );
		g_XfrPrtclStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( XfrPrtclStngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_StngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( StngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );

		g_AjbStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( AjbStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecmStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecmStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexWebRtcAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexWebRtcAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexPrpocsNsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexPrpocsNsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcNsxStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcNsxStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcNsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcNsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexPrpocsOtherStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexPrpocsOtherStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexCodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexCodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_OpenH264CodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( OpenH264CodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
	}
	
	//显示主对话框窗口。
	ShowWindow( g_MainDlgWndHdl, SW_SHOW );

	//设置窗口图标。
	#if( ( defined __X86__ ) )
	SetClassLong( g_MainDlgWndHdl, GCL_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WndAdoVdoTkbkDemoIconId ) ) );
	#elif( ( defined __X64__ ) )
	SetClassLong( g_MainDlgWndHdl, GCLP_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WndAdoVdoTkbkDemoIconId ) ) );
	#endif

	//设置使用UDP协议。
	CheckRadioButton( g_MainDlgWndHdl, UseTcpPrtclRdBtnId, UseUdpPrtclRdBtnId, UseUdpPrtclRdBtnId );

	//设置传输协议。
	CheckRadioButton( g_XfrPrtclStngDlgWndHdl, UsePttRdBtnId, UseRtFdRdBtnId, UseRtFdRdBtnId );
	SetWindowText( GetDlgItem( g_XfrPrtclStngDlgWndHdl, MaxCnctTimesEdTxtId ), L"5" );
	CheckDlgButton( g_XfrPrtclStngDlgWndHdl, IsAutoAllowCnctCkBoxId, BST_CHECKED );

	//设置IP地址组合框的内容。
	{
		struct addrinfo p_LclNodeHintsAddrInfo; //存放本地节点条件地址信息结构体。
		struct addrinfo * p_LclNodeAddrInfoListPt = NULL; //存放本地节点地址信息结构体链表的指针。
		struct addrinfo * p_LclNodeAddrInfoListLoopPt = NULL; //存放本地节点地址信息结构体链表的指针。

		//初始化本地节点条件地址信息结构体。
		p_LclNodeHintsAddrInfo.ai_canonname = NULL;
		p_LclNodeHintsAddrInfo.ai_next = NULL;
		p_LclNodeHintsAddrInfo.ai_addr = NULL;
		p_LclNodeHintsAddrInfo.ai_addrlen = 0;
		p_LclNodeHintsAddrInfo.ai_flags = AI_PASSIVE; //获取用于调用bind()函数的地址信息。
		p_LclNodeHintsAddrInfo.ai_family = AF_INET; //要获取IPv4地址簇的地址信息。
		p_LclNodeHintsAddrInfo.ai_protocol = IPPROTO_UDP; //获取UDP协议的地址信息。
		p_LclNodeHintsAddrInfo.ai_socktype = SOCK_DGRAM; //获取无序、不可靠、双向的非连接数据报类型的地址信息。

		//根据指定的本地节点名称、本地节点服务、本地节点条件地址信息，获取本地节点符合条件的地址信息。
		getaddrinfo( "", "", &p_LclNodeHintsAddrInfo, &p_LclNodeAddrInfoListPt );

		//设置IP地址组合框的内容为本地节点的IP地址。
		if( p_LclNodeAddrInfoListPt != NULL )
		{
			uint16_t p_LclNodeAddrStrPt[ INET6_ADDRSTRLEN ];
			for( p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListPt; p_LclNodeAddrInfoListLoopPt != NULL; p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListLoopPt->ai_next )
			{
				InetNtop( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_family, &( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr ), ( wchar_t * )p_LclNodeAddrStrPt, SzOfArr( p_LclNodeAddrStrPt ) );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeAddrStrPt );
			}
			freeaddrinfo( p_LclNodeAddrInfoListPt ); //销毁本地节点地址信息结构体链表。
		}
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )L"0.0.0.0" );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )L"127.0.0.1" );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_SETCURSEL, 0, 0 ); //设置IP地址组合框默认选择第一个IP地址。
	}

	//设置端口编辑框的内容。
	SetWindowText( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), L"12345" );

	//设置使用音频对讲模式。
	CheckRadioButton( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseAdoTkbkModeRdBtnId );

	//设置音频输入设备组合框、音频输出设备组合框。
	PostMessage( g_MainDlgWndHdl, WM_DEVICECHANGE, 0, 0 );

	//设置默认设置。
	{
		CheckRadioButton( g_StngDlgWndHdl, UseLnkLstRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );

		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseEffectSuperRdBtnId, 0 ); //默认效果等级：超。
		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseBitrateSuperRdBtnId, 0 ); //默认比特率等级：超。
		
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), L"5" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), L"1.0" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), L"1" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), L"24" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), L"1.0" );
	}

	//获取视频输入预览文本框窗口和视频输出显示文本框窗口的句柄和位置。
	g_VdoInptPrvwTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoInptPrvwTxtId );
	GetWindowRect( g_VdoInptPrvwTxtWndHdl, &g_VdoInptPrvwTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VdoInptPrvwTxtWndRect.left );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VdoInptPrvwTxtWndRect.right );
	g_VdoOtptDspyTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoOtptDspyTxtId );
	GetWindowRect( g_VdoOtptDspyTxtWndHdl, &g_VdoOtptDspyTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VdoOtptDspyTxtWndRect.left );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VdoOtptDspyTxtWndRect.right );
	
	//打印当前进程活动目录的完整绝对路径到日志。
	FuncGetCurActPath( g_ErrInfoVstr.m_VstrPt, g_ErrInfoVstr.m_VstrPt );
	g_ErrInfoVstr.Ins( 0, Cu8vstr( "当前进程活动目录的完整绝对路径：" ) );
	LOGI( g_ErrInfoVstr.m_VstrPt );
	{Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVstrPt, 0 );}

    //主消息循环。
	MSG p_Msg;
	while( GetMessage( &p_Msg, NULL, 0, 0 ) )
	{
		//判断一键即按即通按钮是否按下。
		if( g_PttBtnWndHdl == p_Msg.hwnd )
		{
			if( p_Msg.message == WM_LBUTTONDOWN )
			{
				if( g_MediaInfoPt != NULL ) g_MediaInfoPt->m_PttBtnIsDown = 1;
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				if( g_MediaInfoPt != NULL ) g_MediaInfoPt->m_PttBtnIsDown = 0;
			}
		}
		
		//if( IsDialogMessage( p_Msg.hwnd, &p_Msg ) == 0 )
		if( TranslateAccelerator( p_Msg.hwnd, NULL, &p_Msg ) == 0 )
		{
			TranslateMessage( &p_Msg );
			DispatchMessage( &p_Msg );
		}
	}

	LogDstoy(); //销毁日志。
	g_ErrInfoVstr.Dstoy(); //销毁错误信息动态字符串。
	
	#ifdef __DEBUG__
	_CrtDumpMemoryLeaks();
	#endif
	return (int) p_Msg.wParam;
}
