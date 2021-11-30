// WindowsAVTalkbackDemo.cpp : 定义应用程序的入口点。
//
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Func.h"
#include "WindowsAVTalkbackDemo.h"
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

#define WM_INIT_MEDIA_PROC_THREAD       WM_USER + 101  //初始化媒体处理线程的消息。
#define WM_DSTRY_MEDIA_PROC_THREAD      WM_USER + 102  //销毁媒体处理线程的消息。
#define WM_SHOW_RQST_CNCT_DIALOG     WM_USER + 103  //显示请求连接对话框的消息。
#define WM_DSTRY_RQST_CNCT_DIALOG    WM_USER + 104  //销毁请求连接对话框的消息。
#define WM_SHOW_LOG                     WM_USER + 105  //显示日志的消息。
#define WM_REPAINT                      WM_USER + 106  //重绘消息。

//存放媒体信息。
typedef struct MediaInfo
{
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。

	VarStr * m_IPAddrVarStrPt; //存放IP地址动态字符串的指针。
    VarStr * m_PortVarStrPt; //存放端口动态字符串的指针。
	int m_UseWhatXfrPrtcl; //存放使用什么传输协议，为0表示TCP协议，为1表示UDP协议。
    int m_MaxCnctTimes; //存放最大连接次数，取值区间为[1,2147483647]。
    int m_IsCreateSrvrOrClnt; //存放创建服务端或者客户端标记，为1表示创建服务端，为0表示创建客户端。
    TcpSrvrSoktCls m_TcpSrvrSokt; //存放本端TCP协议服务端套接字。
    TcpClntSoktCls m_TcpClntSokt; //存放本端TCP协议客户端套接字。
	UdpSoktCls m_UdpSokt; //存放本端UDP协议套接字。
    uint64_t m_LastPktSendTime; //存放最后一个数据包的发送时间，用于判断连接是否中断。
    uint64_t m_LastPktRecvTime; //存放最后一个数据包的接收时间，用于判断连接是否中断。
	#define PKT_TYP_RQST_CNCT   1 //数据包类型：请求连接包。
	#define PKT_TYP_CNCT_ACK    2 //数据包类型：连接应答包。
	#define PKT_TYP_ALLOW_CNCT  3 //数据包类型：允许连接包。
	#define PKT_TYP_REFUSE_CNCT 4 //数据包类型：拒绝连接包。
	#define PKT_TYP_AFRAME      5 //数据包类型：音频输入输出帧。
	#define PKT_TYP_VFRAME      6 //数据包类型：视频输入输出帧。
	#define PKT_TYP_HTBT        7 //数据包类型：心跳包。
	#define PKT_TYP_EXIT        8 //数据包类型：退出包。
	
	int m_IsAutoAllowCnct; //存放是否自动允许连接，为0表示手动，为1表示自动。
	int m_RqstCnctResult; //存放请求连接的结果，为0表示没有选择，为1表示允许，为2表示拒绝。

    int m_LastSendAudioInputFrameIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
    uint32_t m_LastSendAudioInputFrameTimeStamp; //存放最后一个发送音频输入帧的时间戳。
    uint32_t m_LastSendVideoInputFrameTimeStamp; //存放最后一个发送视频输入帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOutputFrame; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。
    int m_LastGetAudioOutputFrameIsAct; //存放最后一个取出的音频输出帧是否为有语音活动，为0表示否，为非0表示是。
    uint32_t m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp; //存放最后一个取出的音频输出帧对应视频输出帧的时间戳。

    VarLenLnkLstCls m_RecvAudioOutputFrameLnkLst; //存放接收音频输出帧链表。
	VarLenLnkLstCls m_RecvVideoOutputFrameLnkLst; //存放接收视频输出帧链表。

    AAjb * m_AAjbPt; //存放音频自适应抖动缓冲器的指针。
    int m_AAjbMinNeedBufFrameCnt; //存放音频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
    int m_AAjbMaxNeedBufFrameCnt; //存放音频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
    float m_AAjbAdaptSensitivity; //存放音频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
    VAjb * m_VAjbPt; //存放视频自适应抖动缓冲器的指针。
    int m_VAjbMinNeedBufFrameCnt; //存放视频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
    int m_VAjbMaxNeedBufFrameCnt; //存放视频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
    float m_VAjbAdaptSensitivity; //存放视频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。

	int8_t * m_TmpBytePt; //存放临时数据的指针。
	size_t m_TmpByteSz; //存放临时数据的内存大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的指针。
	size_t m_TmpByte2Sz; //存放临时数据的内存大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的指针。
	size_t m_TmpByte3Sz; //存放临时数据的内存大小。
}MediaInfo;

//全局变量。
HINSTANCE g_IstnsHdl; //当前实例的句柄。
VarStr * g_ErrInfoVarStrPt = NULL; //存放错误信息动态字符串的指针。
MediaPocsThrd * g_MediaPocsThrdPt = NULL; //媒体处理线程的指针。
MediaInfo * g_MediaInfoPt = NULL; //媒体信息的指针。
HWND g_MainDlgWndHdl = NULL; //存放主对话框窗口的句柄。
HWND g_RqstCnctDlgWndHdl = NULL; //存放请求连接对话框窗口的句柄。
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
HWND g_VideoInputPrvwTxtWndHdl = NULL; //存放视频输入预览文本框窗口的句柄。
RECT g_VideoInputPrvwTxtWndRect = { 0 }; //存放视频输入预览文本框窗口的位置。
int g_VideoInputPrvwTxtWndIsMax = 0; //存放视频输入预览文本框窗口是否最大化。
HWND g_VideoOutputDspyTxtWndHdl = NULL; //存放视频输出显示文本框窗口句柄。
RECT g_VideoOutputDspyTxtWndRect = { 0 }; //存放视频输出显示文本框窗口的位置。
int g_VideoOutputDspyTxtWndIsMax = 0; //存放视频输出显示文本框窗口是否最大化。

//用户定义的初始化函数，在本线程刚启动时回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaPocsThrdUserInit( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	char p_LclNodeAddrPt[INET6_ADDRSTRLEN]; //存放本地节点地址。
	char p_LclNodePortPt[6]; //存放本地节点端口。
	char p_RmtNodeAddrPt[INET6_ADDRSTRLEN]; //存放远程节点地址。
	char p_RmtNodePortPt[6]; //存放远程节点端口。
	uint64_t p_TmpUint64;
	size_t p_TmpSz;
	
	PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_INIT_MEDIA_PROC_THREAD, 0, 0 ); //向主对话框发送初始化媒体处理线程的消息。

	g_MediaInfoPt->m_RqstCnctResult = 0; //设置请求连接的结果为没有选择。
	g_MediaInfoPt->m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	if( g_MediaInfoPt->m_TmpBytePt == NULL )
	{
		g_MediaInfoPt->m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpBytePt == NULL )
		{
			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据的内存大小。
	}
	if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte2Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
		{
			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByte2Sz = 1024 * 1024; //设置临时数据的内存大小。
	}
	if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte3Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
		{
			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByte3Sz = 1024 * 1024; //设置临时数据的内存大小。
	}

	if( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( g_MediaInfoPt->m_TcpSrvrSokt.Init( 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, 1, 1, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端TCP协议服务端套接字成功。
			{
				if( g_MediaInfoPt->m_TcpSrvrSokt.GetLclAddr( NULL, p_LclNodeAddrPt, p_LclNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			}
			else
			{
				VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( g_MediaInfoPt->m_TcpSrvrSokt.Accept( NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, 1, &g_MediaInfoPt->m_TcpClntSokt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL ) //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						g_MediaInfoPt->m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。

						VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						break;
					}
					else //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
			}
		}
		else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "ping -n 1 -w 1 %s", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt );
			WinExec( g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, SW_HIDE );

			int p_CurCnctTimes = 1;
			while( true ) //循环连接已监听的远端TCP协议服务端套接字。
			{
				//连接远端
				{
					{
						VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "开始第 %d 次连接。", p_CurCnctTimes );
						LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					}

					if( g_MediaInfoPt->m_TcpClntSokt.Init( 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, NULL, NULL, 5000, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字成功。
					{
						if( g_MediaInfoPt->m_TcpClntSokt.GetLclAddr( NULL, p_LclNodeAddrPt, p_LclNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
						{
							VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" );
							LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
							goto out;
						}
						if( g_MediaInfoPt->m_TcpClntSokt.GetRmtAddr( NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
						{
							VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" );
							LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
							goto out;
						}

						VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建并初始化本端TCP协议客户端套接字[%s:%s]，并连接已监听的远端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt, p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						break; //跳出重连。
					}
					else
					{
						VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
						LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					}
				}

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}

				p_CurCnctTimes++;
				if( p_CurCnctTimes > g_MediaInfoPt->m_MaxCnctTimes )
				{
					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "达到最大连接次数，中断连接。" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}
			}
		}

		if( g_MediaInfoPt->m_TcpClntSokt.SetNoDelay( 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}

		if( g_MediaInfoPt->m_TcpClntSokt.SetSendBufSz( 128 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已连接的本端TCP协议客户端套接字的发送缓冲区内存大小失败。
        {
			VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "设置已连接的本端TCP协议客户端套接字的发送缓冲区内存大小失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            goto out;
        }

		if( g_MediaInfoPt->m_TcpClntSokt.SetRecvBufSz( 128 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已连接的本端TCP协议客户端套接字的接收缓冲区内存大小失败。
        {
			VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "设置已连接的本端TCP协议客户端套接字的接收缓冲区内存大小失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            goto out;
        }
	}
	else //如果使用UDP协议。
    {
        if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端UDP协议套接字接受远端UDP协议套接字的连接。
        {
            if( g_MediaInfoPt->m_UdpSokt.Init( 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                if( g_MediaInfoPt->m_UdpSokt.GetLclAddr( NULL, p_LclNodeAddrPt, p_LclNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
					VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
				VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            while( true ) //循环接受远端UDP协议套接字的连接。
            {
                if( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字接收一个远端UDP协议套接字发送的数据包成功。
                    {
                        if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_RQST_CNCT ) ) //如果是请求连接包。
                        {
                            g_MediaInfoPt->m_UdpSokt.Connect( 4, p_RmtNodeAddrPt, p_RmtNodePortPt, 0, NULL ); //用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字，已连接的本端UDP协议套接字只能接收连接的远端UDP协议套接字发送的数据包。

							//连接远端。
                            {
                                g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_RQST_CNCT; //设置请求连接包。
                                if( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
                                {
									VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
									LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
									{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                                    goto UdpSrvrCnctRmt;
                                }

								//接收连接应答包。
								FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime );
                                while( true )
                                {
                                    if( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
                                    {
                                        if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字接收一个远端UDP协议套接字发送的数据包成功。
                                        {
                                            if( ( p_TmpSz >= 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] != PKT_TYP_RQST_CNCT ) ) //如果不是请求连接包。
                                            {
												//就表示连接已经成功建立。
                                                goto UdpSrvrReAccept; //跳出连接循环。
                                            }
                                            else //如果是请求连接包。
                                            {
												//就重新接收连接应答包。
                                            }
                                        }
                                        else //如果用已监听的本端UDP协议套接字接收一个远端UDP协议套接字发送的数据包超时。
                                        {
                                            //就重新接收连接应答包。
                                        }
                                    }
                                    else
                                    {
										g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

                                        VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
										LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
										{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
										goto UdpSrvrCnctRmt;
									}

									FuncGetTimeAsMsec( &p_TmpUint64 );
									if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 5000 )
									{
										VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接收已监听的远端UDP协议套接字[%s:%s]发送的请求连接包失败。原因：接收超时。", p_RmtNodeAddrPt, p_RmtNodePortPt );
										LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
										{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
										goto UdpSrvrCnctRmt;
									}

									if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
									{
										g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

										LOGI( "本线程接收到退出请求，开始准备退出。" );
										goto out;
									}
								}
                            }
							UdpSrvrCnctRmt:
							g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

							VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "本端UDP协议套接字继续保持监听来接受连接。" );
							LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                        }
                        else //如果是其他包，就不管。
                        {

                        }
                    }
                    else //如果用已监听的本端UDP协议套接字接受到远端UDP协议套接字的连接请求超时。
                    {

                    }
                }
                else
                {
                    VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
            }
			UdpSrvrReAccept:

			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
			LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
        }
        else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端UDP协议套接字连接远端UDP协议套接字。
        {
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "ping -n 1 -w 1 %s", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt );
			WinExec( g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, SW_HIDE );

            if( g_MediaInfoPt->m_UdpSokt.Init( 4, NULL, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                if( g_MediaInfoPt->m_UdpSokt.GetLclAddr( NULL, p_LclNodeAddrPt, p_LclNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
                    VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
                VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            if( g_MediaInfoPt->m_UdpSokt.Connect( 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
            {
				VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            if( g_MediaInfoPt->m_UdpSokt.GetRmtAddr( NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
            {
                g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL );
                VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端UDP协议套接字连接的远端UDP协议套接字绑定的远程节点地址和端口失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            int p_CurCnctTimes = 1;
            while( true ) //循环连接已监听的远端UDP协议套接字。
            {
				//连接远端。
				{
					{
						VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "开始第 %d 次连接。", p_CurCnctTimes );
						LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					}

					//发送请求连接包。
					g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_RQST_CNCT; //设置请求连接包。
					if( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
					{
						VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto UdpClntCnctRmt;
					}

					//接收请求连接包。
					FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime );
					while( true )
					{
						if( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
						{
							if( p_TmpSz != -1 ) //如果用已连接的本端UDP协议套接字接收一个远端UDP协议套接字发送的数据包成功。
							{
								if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_RQST_CNCT ) ) //如果是请求连接包。
								{
									//发送连接应答包。
									g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_CNCT_ACK; //设置连接应答包。
									if( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 )
									{
										VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
										LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
										{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
										goto UdpClntCnctRmt;
									}
									break;
								}
								else //如果不是请求连接包。
								{
									//就重新接收请求连接包。
								}
							}
							else //如果用已连接的本端UDP协议套接字接收一个远端UDP协议套接字发送的数据包超时。
							{
								//就重新接收请求连接包。
							}
						}
						else
						{
							g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。
							
							VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
							LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
							goto out;
						}
						
						FuncGetTimeAsMsec( &p_TmpUint64 );
						if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 5000 )
						{
							VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接收已监听的远端UDP协议套接字[%s:%s]发送的请求连接包失败。原因：接收超时。", p_RmtNodeAddrPt, p_RmtNodePortPt );
							LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
							goto UdpClntCnctRmt;
						}

						if( MediaPocsThrdPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
						{
							g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

							LOGI( "本线程接收到退出请求，开始准备退出。" );
							goto out;
						}
					}

					VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
					LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto UdpClntLoopCnct;
				}
				UdpClntCnctRmt:;
				
				p_CurCnctTimes++;
				if( p_CurCnctTimes > g_MediaInfoPt->m_MaxCnctTimes )
				{
					g_MediaInfoPt->m_UdpSokt.Disconnect( 0, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "达到最大连接次数，中断连接。" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}
            }
			UdpClntLoopCnct:;
        }
		
		if( g_MediaInfoPt->m_UdpSokt.SetSendBufSz( 128 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已监听的本端UDP协议套接字的发送缓冲区内存大小失败。
        {
			VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "设置已监听的本端UDP协议套接字的发送缓冲区内存大小失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            goto out;
        }

		if( g_MediaInfoPt->m_UdpSokt.SetRecvBufSz( 128 * 1024, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已监听的本端UDP协议套接字的接收缓冲区内存大小失败。
        {
			VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "设置已监听的本端UDP协议套接字的接收缓冲区内存大小失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            goto out;
        }
    } //协议连接结束。

	//等待允许连接。
	FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); g_MediaInfoPt->m_LastPktSendTime -= 100;
	FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime );
	if( ( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) && ( g_MediaInfoPt->m_IsAutoAllowCnct != 0 ) ) g_MediaInfoPt->m_RqstCnctResult = 1;
	else g_MediaInfoPt->m_RqstCnctResult = 0;
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, p_RmtNodeAddrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_RQST_CNCT_DIALOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	while( true )
	{
		if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是服务端。
		{
			if( g_MediaInfoPt->m_RqstCnctResult == 1 ) //如果允许连接。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ALLOW_CNCT; //设置允许连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "发送一个允许连接包成功。" );
					LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto WaitAllowCnct;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个允许连接包失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}
			}
			else if( g_MediaInfoPt->m_RqstCnctResult == 2 ) //如果拒绝连接。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "发送一个拒绝连接包成功。" );
					LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto out;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个拒绝连接包失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}
			}
		}
		else //如果是客户端。
		{
			if( g_MediaInfoPt->m_RqstCnctResult == 2 ) //如果中断等待。
			{
				g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_REFUSE_CNCT; //设置拒绝连接包。
				if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
					( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "发送一个拒绝连接包成功。" );
					LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto out;
				}
				else
				{
					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。
					
					VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个拒绝连接包失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}
			}
		}
		
		//发送心跳包。
		FuncGetTimeAsMsec( &p_TmpUint64 );
		if( p_TmpUint64 - g_MediaInfoPt->m_LastPktSendTime >= 100 )
		{
			g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_HTBT; //设置心跳包。
			if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
				( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
			{
				g_MediaInfoPt->m_LastPktSendTime = p_TmpUint64; //记录最后一个数据包的发送时间。
				LOGI( "发送一个心跳包成功。" );
			}
			else
			{
				VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个心跳包失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
		}

		//接收一个远端发送的数据包。
		if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
			( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
		{
			if( p_TmpSz != -1 ) //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包成功。
			{
				FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

				if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_HTBT ) ) //如果是心跳包。
				{
					LOGI( "接收到一个心跳包。" );
				}
				else if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_ALLOW_CNCT ) ) //如果是允许连接包。
				{
					if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是客户端。
					{
						g_MediaInfoPt->m_RqstCnctResult = 1;

						PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

						VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "接收到一个允许连接包。" );
						LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						goto WaitAllowCnct;
					}
					else //如果是服务端。
					{
						//就重新接收。
					}
				}
				else if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_REFUSE_CNCT ) ) //如果是拒绝连接包。
				{
					g_MediaInfoPt->m_RqstCnctResult = 2;

					PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

					VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "接收到一个拒绝连接包。" );
					LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					goto out;
				}
				else //如果是其他包。
				{
					//就重新接收。
				}
			}
			else //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包超时。
			{
				//就重新接收。
			}
		}
		else //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。
		{
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

			VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}

		//判断套接字连接是否中断。
		FuncGetTimeAsMsec( &p_TmpUint64 );
		if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 5000 ) //如果超过5000毫秒没有接收任何数据包，就判定连接已经断开了。
		{
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_DSTRY_RQST_CNCT_DIALOG, 0, 0 ); //向主对话框发送毁请求连接对话框的消息。

			VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "5000毫秒没有接收任何数据包，判定套接字连接已经断开了。" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
	}
	WaitAllowCnct:;

	switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
	{
	    case 0: //如果使用链表。
	    {
			//创建并初始化接收音频输出帧链表。
			if( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.Init( LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * 2, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * 2, SIZE_MAX, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				LOGI( "创建并初始化接收音频输出帧链表成功。" );
			}
			else
			{
				VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收音频输出帧链表失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}

			//创建并初始化接收视频输出帧链表。
			if( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.Init( LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * 2, MediaPocsThrdPt->m_AudioOutput.m_FrameLen * 2, SIZE_MAX, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
			{
				LOGI( "创建并初始化接收视频输出帧链表成功。" );
			}
			else
			{
				VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收视频输出帧链表失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
	        break;
	    }
	    case 1: //如果使用自适应抖动缓冲器。
	    {
			//初始化音频自适应抖动缓冲器。
            if( AAjbInit( &g_MediaInfoPt->m_AAjbPt, MediaPocsThrdPt->m_AudioOutput.m_SamplingRate, MediaPocsThrdPt->m_AudioOutput.m_FrameLen, 1, 1, 0, g_MediaInfoPt->m_AAjbMinNeedBufFrameCnt, g_MediaInfoPt->m_AAjbMaxNeedBufFrameCnt, g_MediaInfoPt->m_AAjbAdaptSensitivity, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
            {
                LOGI( "创建并初始化音频自适应抖动缓冲器成功。" );
            }
            else
            {
                VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化音频自适应抖动缓冲器失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	            goto out;
            }

            //初始化视频自适应抖动缓冲器。
            if( VAjbInit( &g_MediaInfoPt->m_VAjbPt, 1, g_MediaInfoPt->m_VAjbMinNeedBufFrameCnt, g_MediaInfoPt->m_VAjbMaxNeedBufFrameCnt, g_MediaInfoPt->m_VAjbAdaptSensitivity, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
            {
                LOGI( "创建并初始化视频自适应抖动缓冲器成功。" );
            }
            else
            {
                VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "创建并初始化视频自适应抖动缓冲器失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	            goto out;
            }
	        break;
	    }
	}

	FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间为当前时间。
	g_MediaInfoPt->m_LastPktRecvTime = g_MediaInfoPt->m_LastPktSendTime; //设置最后一个数据包的接收时间为当前时间。

	g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
    g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
	
	VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "开始进行对讲。" );
	LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		
	}
    return p_Result;
}

//用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaPocsThrdUserProcess( MediaPocsThrd * MediaPocsThrdPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	uint32_t p_TmpUint32;
	uint64_t p_TmpUint64;

    //接收远端发送过来的一个数据包。
	if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
        ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
    {
		if( p_TmpSz != -1 ) //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包成功。
		{
			FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

			if( p_TmpSz == 0 ) //如果数据包的长度为0。
			{
				LOGFE( "接收到数据包的长度为%" PRIuPTR "，表示没有数据，无法继续接收。", p_TmpSz );
                goto out;
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_HTBT ) //如果是心跳包。
            {
                if( p_TmpSz > 1 ) //如果心跳包的长度大于1。
                {
                    LOGFE( "接收到心跳包的长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                LOGI( "接收到一个心跳包。" );
            }
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_AFRAME ) //如果是音频输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果音频输出帧包的长度小于1 + 4，表示没有音频输出帧时间戳。
                {
                    LOGFE( "接收到音频输出帧包的长度为%" PRIuPTR "小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。", p_TmpSz );
                    goto out;
                }

				//读取音频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
				if( MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) //如果要使用音频输出。
				{
					//将输出帧放入链表或自适应抖动缓冲器。
					switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
					{
						case 0: //如果使用链表。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.PutTail( g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
								{
									LOGFI( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( "接收到一个有语音活动的音频输出帧包，并放入接收音频输出帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
									goto out;
								}
							}
							else //如果该音频输出帧为无语音活动。
                            {
                                LOGFI( "接收到一个无语音活动的音频输出帧包，无需放入接收音频输出帧链表。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
							break;
						}
						case 1: //如果使用自适应抖动缓冲器。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                            {
                                AAjbPutOneFrame( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
                                LOGFI( "接收到一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            else //如果该音频输出帧为无语音活动。
                            {
								AAjbPutOneFrame( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, 0, 1, NULL );
                                LOGFI( "接收到一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
							int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
							int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
							AAjbGetBufFrameCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt, 1, NULL );
							LOGFI( "音频自适应抖动缓冲器：有活动帧：%" PRIi32 "，无活动帧：%" PRIi32 "，帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufActFrameCnt, p_CurHaveBufInactFrameCnt, p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );

							break;
						}
					}
				}
				else //如果不使用音频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                    {
                        LOGFI( "接收到一个有语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                    else //如果该音频输出帧为无语音活动。
                    {
                        LOGFI( "接收到一个无语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                }
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_VFRAME ) //如果是视频输出帧包。
            {
                if( p_TmpSz < 1 + 4 ) //如果视频输出帧包的长度小于1 + 4，表示没有视频输出帧时间戳。
                {
					LOGFE( "接收到一个视频输出帧包的长度为%" PRIuPTR "小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。", p_TmpSz );
                    goto out;
                }

				//读取视频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
                if( MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput ) //如果要使用视频输出。
                {
                    //将视频输出帧放入链表或自适应抖动缓冲器。
                    switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
                    {
                        case 0: //如果使用链表。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.PutTail( g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 )
								{
									LOGFI( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表成功。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( "接收到一个有图像活动的视频输出帧包，并放入接收视频输出帧链表失败。原因：%s", MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
									goto out;
								}
							}
							else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( "接收到一个无图像活动的视频输出帧包，无需放入接收视频输出帧链表。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            break;
                        }
                        case 1: //如果使用自适应抖动缓冲器。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                            {
                                VAjbPutOneFrame( g_MediaInfoPt->m_VAjbPt, g_MediaInfoPt->m_LastPktRecvTime, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, 1, NULL );
                                LOGFI( "接收到一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( "接收到一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
							VAjbGetBufFrameCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt, 1, NULL );
							LOGFI( "视频自适应抖动缓冲器：帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );
                            break;
                        }
                    }
                }
                else //如果不使用视频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                    {
                        LOGFI( "接收到一个有图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                    else //如果该视频输出帧为无图像活动。
                    {
                        LOGFI( "接收到一个无图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                }
            }
            else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) //如果是退出包。
            {
                if( p_TmpSz > 1 ) //如果退出包的长度大于1。
                {
                    LOGFE( "接收到退出包的长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                g_MediaInfoPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
                MediaPocsThrdRqirExit( MediaPocsThrdPt, 1, 0, NULL ); //请求退出。

				VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
				LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				if( MediaPocsThrdPt->m_IsShowToast != 0 ) FuncToast( NULL, 3000, NULL, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
            }
		}
		else //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包超时。
        {
			
        }
	}
	else //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。
	{
		VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" );
		LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
		goto out;
	}

    //发送心跳包。
	FuncGetTimeAsMsec( &p_TmpUint64 );
    if( p_TmpUint64 - g_MediaInfoPt->m_LastPktSendTime >= 100 ) //如果超过100毫秒没有发送任何数据包，就发送一个心跳包。
    {
		g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_HTBT; //设置心跳包。
        if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
            ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
        {
			g_MediaInfoPt->m_LastPktSendTime = p_TmpUint64; //记录最后一个数据包的发送时间。
			LOGI( "发送一个心跳包成功。" );
		}
		else
		{
			VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个心跳包失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
    }

    //判断套接字连接是否中断。
	FuncGetTimeAsMsec( &p_TmpUint64 );
	if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 5000 ) //如果超过5000毫秒没有接收任何数据包，就判定连接已经断开了。
	{
		VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "超过5000毫秒没有接收任何数据包，判定连接已经断开了。" );
		LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
		goto out;
	}

    p_Result = 0; //设置本函数执行成功。

	out:
	return p_Result;
}

//用户定义的销毁函数，在本线程退出时回调一次。
void __cdecl MyMediaPocsThrdUserDstoy( MediaPocsThrd * MediaPocsThrdPt )
{
	size_t p_TmpSz;
	
	if( ( MediaPocsThrdPt->m_ExitFlag == 1 ) && ( ( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL ) || ( ( g_MediaInfoPt->m_UdpSokt.m_UdpSoktPt != NULL ) && ( g_MediaInfoPt->m_UdpSokt.GetRmtAddr( NULL, NULL, NULL, 0, NULL ) == 0 ) ) ) ) //如果本线程接收到退出请求，且本端TCP协议客户端套接字不为空或本端UDP协议套接字不为空且已连接远端。
    {
        //发送退出包。
        g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_EXIT; //设置退出包。
        if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, 1, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) ) ||
            ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) != 0 ) ) )
        {
            VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个退出包失败。原因：" );
			LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto SendExitPkt;
        }

        FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //记录最后一个数据包的发送时间。

		VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "发送一个退出包成功。" );
		LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

		//接收退出包。
        if( g_MediaInfoPt->m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
        {
            while( true ) //循环接收退出包。
            {
                if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.RecvPkt( g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                    ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.RecvPkt( NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
                {
                    if( p_TmpSz != -1 ) //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包成功。
                    {
						FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

                        if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) ) //如果是退出包。
                        {
                            VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
							LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                            goto SendExitPkt;
                        }
						else //如果是其他包，继续接收。
                        {

                        }
                    }
                    else //如果用已连接的本端套接字接收一个连接的远端套接字发送的数据包超时。
                    {
                        VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" );
						LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                        goto SendExitPkt;
                    }
                }
                else //用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。
                {
                    VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字接收一个连接的远端套接字发送的数据包失败。原因：" );
					LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto SendExitPkt;
                }
            }
        }
    }
	SendExitPkt:;

    //销毁本端TCP协议服务端套接字。
    if( g_MediaInfoPt->m_TcpSrvrSokt.m_TcpSrvrSoktPt != NULL )
    {
        g_MediaInfoPt->m_TcpSrvrSokt.Dstoy( NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。

		VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议服务端套接字成功。" );
		LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁本端TCP协议客户端套接字。
    if( g_MediaInfoPt->m_TcpClntSokt.m_TcpClntSoktPt != NULL )
    {
        g_MediaInfoPt->m_TcpClntSokt.Dstoy( ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。

		VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议客户端套接字成功。" );
		LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }
	
    //销毁本端UDP协议套接字。
    if( g_MediaInfoPt->m_UdpSokt.m_UdpSoktPt != NULL )
    {
        g_MediaInfoPt->m_UdpSokt.Dstoy( NULL ); //关闭并销毁已创建的本端UDP协议套接字。

		VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端UDP协议套接字成功。" );
		LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁接收音频输出帧的链表。
    if( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.m_VarLenLnkLstPt != NULL )
    {
		g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.Dstoy( NULL );

		LOGI( "销毁接收音频输出帧的链表成功。" );
    }
	
    //销毁接收视频输出帧的链表。
    if( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.m_VarLenLnkLstPt != NULL )
    {
		g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.Dstoy( NULL );

		LOGI( "销毁接收视频输出帧的链表成功。" );
    }

    //销毁音频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_AAjbPt != NULL )
    {
        AAjbDstoy( g_MediaInfoPt->m_AAjbPt, NULL );
        g_MediaInfoPt->m_AAjbPt = NULL;

		LOGI( "销毁音频自适应抖动缓冲器成功。" );
    }
	
    //销毁视频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_VAjbPt != NULL )
    {
        VAjbDstoy( g_MediaInfoPt->m_VAjbPt, NULL );
        g_MediaInfoPt->m_VAjbPt = NULL;

		LOGI( "销毁视频自适应抖动缓冲器成功。" );
    }

    if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
    {
        if( ( MediaPocsThrdPt->m_ExitFlag == 1 ) && ( g_MediaInfoPt->m_IsRecvExitPkt == 1 ) ) //如果本线程接收到退出请求，且接收到了退出包。
        {
			VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程接收到退出请求，且接收到了退出包，表示是远端TCP协议客户端套接字主动退出，本线程重新初始化来继续保持监听。" );
			LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
        }
		else if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) && ( MediaPocsThrdPt->m_ExitCode == -1 ) && ( g_MediaInfoPt->m_RqstCnctResult == 2 ) ) //如果本线程没收到退出请求，且退出代码为初始化失败，且请求连接的结果为拒绝。
		{
			VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程没收到退出请求，且初始化失败，且请求连接的结果为拒绝，表示是拒绝本次连接，本线程重新初始化来继续保持监听。" );
			LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			MediaPocsThrdRqirExit( MediaPocsThrdPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_REPAINT, 0, 0 ); //向主对话框发送重绘消息。
		}
        else if( ( MediaPocsThrdPt->m_ExitFlag == 0 ) && ( MediaPocsThrdPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
			VarStrCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来继续保持监听。" );
			LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
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
            VarStrFmtCpy( MediaPocsThrdPt->m_ErrInfoVarStrPt, "由于是创建客户端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来重连服务端。" );
			LOGI( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
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
void __cdecl MyMediaPocsThrdUserReadAudioVideoInputFrame( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAudioInputFramePt, int16_t * PcmAudioResultFramePt, int32_t VoiceActSts, uint8_t * EncoderAudioInputFramePt, size_t EncoderAudioInputFrameLen, int32_t EncoderAudioInputFrameIsNeedTrans, uint8_t * YU12VideoInputFramePt, int32_t YU12VideoInputFrameWidth, int32_t YU12VideoInputFrameHeight, uint8_t * EncoderVideoInputFramePt, size_t EncoderVideoInputFrameLen )
{
    int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	int p_FramePktLen = 0; //存放输入输出帧数据包的长度，单位字节。

    //发送音频输入帧。
	if( PcmAudioInputFramePt != NULL ) //如果要使用音频输入。
    {
		if( EncoderAudioInputFramePt != NULL ) //如果要使用已编码格式音频输入帧。
        {
            if( VoiceActSts != 0 && EncoderAudioInputFrameIsNeedTrans != 0 ) //如果本次音频输入帧为有语音活动，且需要传输。
            {
                memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, EncoderAudioInputFramePt, EncoderAudioInputFrameLen ); //设置音频输入输出帧。
                p_FramePktLen = 1 + 4 + 4 + EncoderAudioInputFrameLen; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 视频输入帧时间戳 + 已编码格式音频输入帧。
            }
            else //如果本次音频输入帧为无语音活动，或不需要传输。
            {
                p_FramePktLen = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
            }
        }
        else //如果要使用PCM格式音频输入帧。
        {
            if( VoiceActSts != 0 ) //如果本次音频输入帧为有语音活动。
            {
				memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, PcmAudioResultFramePt, MediaPocsThrdPt->m_AudioInput.m_FrameLen * 2 ); //设置音频输入输出帧。
                p_FramePktLen = 1 + 4 + 4 + MediaPocsThrdPt->m_AudioInput.m_FrameLen * 2; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 视频输入帧时间戳 + PCM格式音频输入帧。
            }
            else //如果本次音频输入帧为无语音活动，或不需要传输。
            {
                p_FramePktLen = 1 + 4; //数据包长度 = 数据包类型 + 音频输入帧时间戳。
            }
        }

        if( p_FramePktLen != 1 + 4 ) //如果本音频输入帧为有语音活动，就发送。
        {
			g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

			//设置数据包类型为音频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_AFRAME;
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF000000 ) >> 24 );
			//设置视频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[5] = ( int8_t ) ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[6] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[7] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[8] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
				FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
				LOGFI( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%" PRIu32 "，视频输入帧时间戳：%" PRIu32 "，总长度：%d。", g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, p_FramePktLen );
			}
			else
			{
				VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个有语音活动的音频输入帧包失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
			
			g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
        }
        else if( ( p_FramePktLen == 1 + 4 ) && ( g_MediaInfoPt->m_LastSendAudioInputFrameIsAct != 0 ) ) //如果本音频输入帧为无语音活动，但最后一个发送的音频输入帧为有语音活动，就发送。
        {
            g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp += 1; //音频输入帧的时间戳递增一个步进。

            //设置数据包类型为音频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_AFRAME;
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 5, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
                FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                LOGFI( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%" PRIu32 "，总长度：%d。", g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen );
            }
            else
            {
                VarStrIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个无语音活动的音频输入帧包失败。原因：" );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
            }

            g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
        }
		else //如果本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。
		{
            LOGI( "本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" );
        }
    }

    //发送视频输入帧。
    if( YU12VideoInputFramePt != NULL ) //如果要使用视频输入。
    {
        if( EncoderVideoInputFramePt != NULL ) //如果要使用已编码格式视频输入帧。
        {
            if( EncoderVideoInputFrameLen != 0 ) //如果本次视频输入帧为有图像活动。
            {
				memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, EncoderVideoInputFramePt, EncoderVideoInputFrameLen ); //设置视频输入输出帧。
                p_FramePktLen = 1 + 4 + 4 + EncoderVideoInputFrameLen; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 音频输入帧时间戳 + 已编码格式视频输入帧。
            }
            else
            {
                p_FramePktLen = 1 + 4; //数据包长度 = 数据包类型 + 视频输入帧时间戳。
            }
        }
        else //如果要使用YU12格式视频输入帧。
        {
			//设置视频输入帧宽度。
            g_MediaInfoPt->m_TmpBytePt[9] = ( int8_t ) ( YU12VideoInputFrameWidth & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[10] = ( int8_t ) ( ( YU12VideoInputFrameWidth & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[11] = ( int8_t ) ( ( YU12VideoInputFrameWidth & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[12] = ( int8_t ) ( ( YU12VideoInputFrameWidth & 0xFF000000 ) >> 24 );
            //设置视频输入帧高度。
            g_MediaInfoPt->m_TmpBytePt[13] = ( int8_t ) ( YU12VideoInputFrameHeight & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[14] = ( int8_t ) ( ( YU12VideoInputFrameHeight & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[15] = ( int8_t ) ( ( YU12VideoInputFrameHeight & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[16] = ( int8_t ) ( ( YU12VideoInputFrameHeight & 0xFF000000 ) >> 24 );

			memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4 + 4 + 4, YU12VideoInputFramePt, YU12VideoInputFrameWidth * YU12VideoInputFrameHeight * 3 / 2 ); //设置视频输入输出帧。
            p_FramePktLen = 1 + 4 + 4 + 4 + 4 + YU12VideoInputFrameWidth * YU12VideoInputFrameHeight * 3 / 2; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 音频输入帧时间戳 + 视频输入帧宽度 + 视频输入帧高度 + YU12格式视频输入帧。
        }

        //发送视频输入帧数据包。
        if( p_FramePktLen != 1 + 4 ) //如果本视频输入帧为有图像活动，就发送。
        {
            g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp += 1; //视频输入帧的时间戳递增一个步进。

            //设置数据包类型为视频输入帧包。
            g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_VFRAME;
            //设置视频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp & 0xFF000000 ) >> 24 );
            //设置音频输入帧时间戳。
            g_MediaInfoPt->m_TmpBytePt[5] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF );
            g_MediaInfoPt->m_TmpBytePt[6] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_MediaInfoPt->m_TmpBytePt[7] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_MediaInfoPt->m_TmpBytePt[8] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( g_MediaInfoPt->m_TcpClntSokt.SendPkt( g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_UdpSokt.SendPkt( 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, 1, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
                FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
				LOGFI( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%" PRIu32 "，音频输入帧时间戳：%" PRIu32 "，总长度：%d，类型：%d。", g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
            }
            else
            {
				VarStrFmtIns( MediaPocsThrdPt->m_ErrInfoVarStrPt, 0, "发送一个有图像活动的视频输入帧包失败。视频输入帧时间戳：%" PRIu32 "，音频输入帧时间戳：%" PRIu32 "，总长度：%d，类型：%d。原因：", g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
				LOGE( MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaPocsThrdPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
            }
        }
        else //如果本视频输入帧为无图像活动，无需发送。
        {
            LOGI( "本视频输入帧为无图像活动，无需发送。" );
        }
    }

    p_Result = 0; //设置本函数执行成功。

	out:
    return;
}

//用户定义的写入音频输出帧函数，在需要写入一个音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
void __cdecl MyMediaPocsThrdUserWriteAudioOutputFrame( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAudioOutputFramePt, uint8_t * EncoderAudioOutputFramePt, size_t * AudioOutputFrameLenPt  )
{
	int32_t m_TmpInt32;
	size_t m_TmpSz = 0;

	//取出并写入音频输出帧。
	{
		//从链表或音频自适应抖动缓冲器取出一个音频输出帧。
		switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收音频输出帧。
		{
			case 0: //如果使用链表。
			{
				g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.GetTotal( &m_TmpSz, 1, NULL );
				if( m_TmpSz != 0 )
				{
					g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.Lock( NULL ); //接收音频输出帧链表的互斥锁加锁。
					g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.GetHead( NULL, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, NULL, &m_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ); //获取接收音频输出帧链表的第一个音频输出帧。
					g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.DelHead( 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ); //删除接收音频输出帧链表的第一个音频输出帧。
					g_MediaInfoPt->m_RecvAudioOutputFrameLnkLst.Unlock( NULL ); //接收音频输出帧链表的互斥锁解锁。
				}
				
				if( m_TmpSz != 0 ) //如果接收音频输出帧链表的第一个输出帧为有语音活动。
				{
					LOGFI( "从接收音频输出帧链表取出一个有语音活动的音频输出帧，帧的长度：%" PRIuPTR "。", m_TmpSz );
				}
				else //如果接收音频输出帧链表为空，或第一个音频输出帧为无语音活动。
				{
					LOGFI( "从接收音频输出帧链表取出一个无语音活动的音频输出帧，帧的长度：%" PRIuPTR "。", m_TmpSz );
				}

                break;
			}
			case 1: //如果使用自适应抖动缓冲器。
			{
				uint32_t p_AudioOutputFrameTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出一个音频输出帧。
				AAjbGetOneFrame( g_MediaInfoPt->m_AAjbPt, &p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, &m_TmpSz, 1, NULL );
				
				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp = ( g_MediaInfoPt->m_TmpByte2Pt[0] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[1] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[2] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[3] & 0xFF ) << 24 ); //设置最后一个取出的音频输出帧对应视频输出帧的时间戳。
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%" PRIu32 "，视频输出帧时间戳：%" PRIu32 "，长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 0; //设置最后一个取出的音频输出帧为无语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%" PRIu32 "，长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%" PRIu32 "，视频输出帧时间戳：%" PRIu32 "，长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, m_TmpSz );
				}

				int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
				int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
				int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
				int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
				int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
				int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
				AAjbGetBufFrameCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt, 1, NULL );
				LOGFI( "自适应抖动缓冲器：有活动帧：%" PRIi32 "，无活动帧：%" PRIi32 "，帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufActFrameCnt, p_CurHaveBufInactFrameCnt, p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );

				break;
			}
		}

		//写入音频输出帧。
		if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
		{
			if( PcmAudioOutputFramePt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                if( m_TmpSz - 4 != *AudioOutputFrameLenPt )
                {
					memset( PcmAudioOutputFramePt, 0, *AudioOutputFrameLenPt );
                    LOGFE( "音频输出帧的长度不等于PCM格式的长度。音频输出帧：%" PRIuPTR "，PCM格式：%" PRIi32 "。", m_TmpSz - 4, *AudioOutputFrameLenPt );
                }
				else
				{
					//写入PCM格式音频输出帧。
					memcpy( PcmAudioOutputFramePt, g_MediaInfoPt->m_TmpByte2Pt, *AudioOutputFrameLenPt );
				}
            }
            else //如果要使用已编码格式音频输出帧。
            {
                if( m_TmpSz - 4 > *AudioOutputFrameLenPt )
                {
                    LOGFE( "音频输出帧的长度已超过已编码格式的长度。音频输出帧：%" PRIuPTR "，已编码格式：%" PRIuPTR "。", m_TmpSz - 4, *AudioOutputFrameLenPt );
                    *AudioOutputFrameLenPt = 0;
                }
				else
				{
					//写入已编码格式音频输出帧。
					*AudioOutputFrameLenPt = m_TmpSz - 4;
					memcpy( EncoderAudioOutputFramePt, g_MediaInfoPt->m_TmpByte2Pt + 4, *AudioOutputFrameLenPt );
				}
            }
		}
		else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
		{
			if( PcmAudioOutputFramePt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                memset( PcmAudioOutputFramePt, 0, *AudioOutputFrameLenPt );
            }
            else //如果要使用已编码格式音频输出帧。
            {
                *AudioOutputFrameLenPt = 0;
            }
		}
		else //如果音频输出帧为丢失。
		{
			if( PcmAudioOutputFramePt != NULL ) //如果要使用PCM格式音频输出帧。
            {
                memset( PcmAudioOutputFramePt, 0, *AudioOutputFrameLenPt );
            }
            else //如果要使用已编码格式音频输出帧。
            {
                *AudioOutputFrameLenPt = m_TmpSz;
            }
		}
	}

	out:
	return;
}

//用户定义的获取PCM格式音频输出帧函数，在解码完一个已编码音频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致声学回音消除失败。
void __cdecl MyMediaPocsThrdUserGetPcmAudioOutputFrame( MediaPocsThrd * MediaPocsThrdPt, int16_t * PcmAudioOutputFramePt, size_t PcmAudioOutputFrameLen )
{
	
}

//用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
void __cdecl MyMediaPocsThrdUserWriteVideoOutputFrame( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VideoOutputFramePt, int32_t * YU12VideoOutputFrameWidthPt, int32_t * YU12VideoOutputFrameHeightPt, uint8_t * EncoderVideoOutputFramePt, size_t * EncoderVideoOutputFrameLenPt )
{
	uint32_t p_VideoOutputFrameTimeStamp;
    uint32_t p_VideoOutputFrameAudioOutputFrameTimeStamp;
	uint64_t p_NowTimeMesc;
	size_t m_TmpSz = 0;

    //从链表或自适应抖动缓冲器取出一个视频输出帧。
    switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
    {
        case 0: //如果使用链表。
        {
			g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.GetTotal( &m_TmpSz, 1, NULL );
            if( m_TmpSz != 0 ) //如果接收视频输出帧链表不为空。
            {
				g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.Lock( NULL ); //接收视频输出帧链表的互斥锁加锁。
				g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.GetHead( NULL, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, NULL, &m_TmpSz, 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ); //获取接收视频输出帧链表的第一个视频输出帧。
				g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.DelHead( 0, MediaPocsThrdPt->m_ErrInfoVarStrPt ); //删除接收视频输出帧链表的第一个视频输出帧。
				g_MediaInfoPt->m_RecvVideoOutputFrameLnkLst.Unlock( NULL ); //接收视频输出帧链表的互斥锁解锁。

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( "从接收视频输出帧链表取出一个有图像活动的视频输出帧。长度：%" PRIuPTR "。", m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( "从接收视频输出帧链表取出一个无图像活动的视频输出帧。长度：%" PRIuPTR "。", m_TmpSz );
				}
            }
			
            break;
        }
        case 1: //如果使用自适应抖动缓冲器。
        {
            int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
			int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
			int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
			int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
			VAjbGetBufFrameCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt, 1, NULL );
			
			if( p_CurHaveBufFrameCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				LOGFI( "视频自适应抖动缓冲器：帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );

				//从视频自适应抖动缓冲器取出一个视频输出帧。
				FuncGetTimeAsMsec( &p_NowTimeMesc );
				if( MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct != 0 ) //如果要使用音频输出，且最后一个取出的音频输出帧为有语音活动，就根据最后一个取出的音频输出帧对应视频输出帧的时间戳来取出。
				{
					VAjbGetOneFrameWantTimeStamp( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, &p_VideoOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz, 1, NULL );
				}
				else //如果最后一个取出的音频输出帧为无语音活动，就根据直接取出。
				{
					VAjbGetOneFrame( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, &p_VideoOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz, 1, NULL );
				}

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%" PRIu32 "，长度：%" PRIuPTR "。", p_VideoOutputFrameTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%" PRIu32 "，长度：%" PRIuPTR "。", p_VideoOutputFrameTimeStamp, m_TmpSz );
				}
			}
            break;
        }
    }

    //写入视频输出帧。
    if( m_TmpSz > 0 ) //如果视频输出帧为有图像活动。
    {
        //读取视频输出帧对应音频输出帧的时间戳。
        p_VideoOutputFrameAudioOutputFrameTimeStamp = ( g_MediaInfoPt->m_TmpByte3Pt[0] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[1] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[2] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[3] & 0xFF ) << 24 );

        if( YU12VideoOutputFramePt != NULL ) //如果要使用YU12格式视频输出帧。
        {
			*YU12VideoOutputFrameWidthPt = ( g_MediaInfoPt->m_TmpByte3Pt[4] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[5] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[6] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[7] & 0xFF ) << 24 );
			*YU12VideoOutputFrameHeightPt = ( g_MediaInfoPt->m_TmpByte3Pt[8] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[9] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[10] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte3Pt[11] & 0xFF ) << 24 );

            if( m_TmpSz - 4 - 4 - 4 != *YU12VideoOutputFrameWidthPt * *YU12VideoOutputFrameHeightPt * 3 / 2 )
            {
                LOGFE( "视频输出帧的长度不等于YU12格式的长度。视频输出帧：%" PRIuPTR "，YU12格式：%" PRIi32"。", m_TmpSz - 4 - 4 - 4, *EncoderVideoOutputFrameLenPt );
				*YU12VideoOutputFrameWidthPt = 0;
				*YU12VideoOutputFrameHeightPt = 0;
				return;
            }

            //写入YU12格式视频输出帧。
			memcpy( YU12VideoOutputFramePt, g_MediaInfoPt->m_TmpByte3Pt + 4 + 4 + 4, m_TmpSz - 4 - 4 - 4 );
        }
        else //如果要使用已编码格式视频输出帧。
        {
            if( m_TmpSz - 4 > *EncoderVideoOutputFrameLenPt )
            {
                *EncoderVideoOutputFrameLenPt = 0;
				LOGFE( "视频输出帧的长度已超过已编码格式的长度。视频输出帧：%" PRIuPTR "，已编码格式：%" PRIi32"。", m_TmpSz - 4, *EncoderVideoOutputFrameLenPt );
                return;
            }

            //写入已编码格式视频输出帧。
            memcpy( EncoderVideoOutputFramePt, g_MediaInfoPt->m_TmpByte3Pt + 4, m_TmpSz - 4 );
            *EncoderVideoOutputFrameLenPt = m_TmpSz - 4;
        }
    }
    else if( m_TmpSz == 0 ) //如果视频输出帧为无图像活动。
    {
		if( YU12VideoOutputFramePt != NULL ) //如果要使用YU12格式视频输出帧。
        {
            *EncoderVideoOutputFrameLenPt = 0;
        }
        else //如果要使用已编码格式视频输出帧。
        {
            *EncoderVideoOutputFrameLenPt = 0;
        }
    }
}

//用户定义的获取YU12格式视频输出帧函数，在解码完一个已编码视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
void __cdecl MyMediaPocsThrdUserGetYU12VideoOutputFrame( MediaPocsThrd * MediaPocsThrdPt, uint8_t * YU12VideoOutputFramePt, int32_t YU12VideoOutputFrameWidth, int32_t YU12VideoOutputFrameHeight )
{
	
}

//窗口消息处理过程函数。
INT_PTR CALLBACK WndMsgPocsPocdr( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	
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
				if( FuncSetWndCenter( hDlg, GetParent( hDlg ), g_ErrInfoVarStrPt ) != 0 )
				{
					LOGFE( "设置窗口居中失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
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
			HWND p_AudioInputDvcComboBoxWndHdl = NULL;
			HWND p_AudioOutputDvcComboBoxWndHdl = NULL;
			HWND p_VideoInputDvcComboBoxWndHdl = NULL;
			VarStr * * p_AudioInputDvcNameArrPt = NULL;
			UINT p_AudioInputDvcTotal = 0;
			VarStr * * p_AudioOutputDvcNameArrPt = NULL;
			UINT p_AudioOutputDvcTotal = 0;
			VarStr * * p_VideoInputDvcNameArrPt = NULL;
			UINT p_VideoInputDvcTotal = 0;
			VarStr * p_CurAudioInputDvcName = NULL;
			VarStr * p_CurAudioOutputDvcName = NULL;
			VarStr * p_CurVideoInputDvcName = NULL;
			int p_TmpInt;

			//获取音频输入设备、音频输出设备、视频输入设备的名称。
			if( MediaPocsThrdGetAudioInputDvcName( &p_AudioInputDvcNameArrPt, &p_AudioInputDvcTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取音频输入设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetAudioOutputDvcName( &p_AudioOutputDvcNameArrPt, &p_AudioOutputDvcTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取音频输出设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetVideoInputDvcName( &p_VideoInputDvcNameArrPt, &p_VideoInputDvcTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取视频输入设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto OutDvcChange;
			}
			
			//获取音频输入设备、音频输出设备、视频输入设备的控件句柄。
			p_AudioInputDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId );
			p_AudioOutputDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId );
			p_VideoInputDvcComboBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId );

			//获取当前选择的音频输入设备、音频输出设备、视频输入设备。
			p_TmpInt = SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurAudioInputDvcName, SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioInputDvcName->m_StrPt );
			}
			p_TmpInt = SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurAudioOutputDvcName, SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioOutputDvcName->m_StrPt );
			}
			p_TmpInt = SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurVideoInputDvcName, SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurVideoInputDvcName->m_StrPt );
			}

			//清空音频输入设备、音频输出设备、视频输入设备的控件。
			SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );

			//添加音频输入设备、音频输出设备、视频输入设备到控件。
			if( p_AudioInputDvcTotal > 0 )
			{
				p_TmpInt = SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AudioInputDvcTotal; p_TmpInt++ )
					SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AudioInputDvcNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"无" );
			}
			if( p_AudioOutputDvcTotal > 0 )
			{
				SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AudioOutputDvcTotal; p_TmpInt++ )
					SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AudioOutputDvcNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )"无" );
			}
			if( p_VideoInputDvcTotal > 0 )
			{
				SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_VideoInputDvcTotal; p_TmpInt++ )
					SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_VideoInputDvcNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )"无" );
			}
			
			//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
			if( ( p_CurAudioInputDvcName == NULL ) || ( SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioInputDvcName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			if( ( p_CurAudioOutputDvcName == NULL ) || ( SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioOutputDvcName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioOutputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			if( ( p_CurVideoInputDvcName == NULL ) || ( SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVideoInputDvcName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_VideoInputDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			
			p_Result = 0; // //设置本函数执行成功。

			OutDvcChange:
			MediaPocsThrdDstoyDvcName( p_AudioInputDvcNameArrPt, p_AudioInputDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_AudioOutputDvcNameArrPt, p_AudioOutputDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_VideoInputDvcNameArrPt, p_VideoInputDvcTotal, NULL );
			VarStrDstoy( p_CurAudioInputDvcName );
			VarStrDstoy( p_CurAudioOutputDvcName );
			VarStrDstoy( p_CurVideoInputDvcName );
			return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
		}
		case WM_COMMAND: //单击消息。
		{
			switch( LOWORD( wParam ) )
			{
				case UseAudioTalkbackRdBtnId: //音频对讲模式单选按钮。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput = 1;
						g_MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput = 1;
						g_MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput = 0;
						g_MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput = 0;

						if( g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为音频对讲，请求重启媒体处理线程。" );
							MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseVideoTalkbackRdBtnId: //视频对讲模式单选按钮。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput = 0;
						g_MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput = 0;
						g_MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput = 1;
						g_MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput = 1;

						if( g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为视频对讲，请求重启媒体处理线程。" );
							MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseAudioVideoTalkbackRdBtnId: //音视频对讲模式单选按钮。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						g_MediaPocsThrdPt->m_AudioInput.m_IsUseAudioInput = 1;
						g_MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput = 1;
						g_MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput = 1;
						g_MediaPocsThrdPt->m_VideoOutput.m_IsUseVideoOutput = 1;

						if( g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为音视频对讲，请求重启媒体处理线程。" );
							MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case UseAudioInputDvcCbBoxId: //音频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								MediaPocsThrdSetAudioInputUseDvc( g_MediaPocsThrdPt, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( "音频输入使用的设备已改变，请求重启媒体处理线程。" );
									MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case UseAudioOutputDvcCbBoxId: //音频输出设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaPocsThrdPt != NULL )
							{
								//设置音频输出使用的设备。
								MediaPocsThrdSetAudioOutputUseDvc( g_MediaPocsThrdPt, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaPocsThrdPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( "音频输出使用的设备已改变，请求重启媒体处理线程。" );
									MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case UseVideoInputDvcCbBoxId: //视频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaPocsThrdPt != NULL )
							{
								//设置视频输入使用的设备。
								MediaPocsThrdSetVideoInputUseDvc( g_MediaPocsThrdPt, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaPocsThrdPt->m_VideoInput.m_IsUseVideoInput != 0 && g_MediaPocsThrdPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用视频输入，且媒体处理线程已经初始化完毕。
								{
									LOGI( "视频输入使用的设备已改变，请求重启媒体处理线程。" );
									MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case AudioInputIsMuteCkBoxId: //音频输入静音复选框。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						MediaPocsThrdSetAudioInputIsMute( g_MediaPocsThrdPt, ( IsDlgButtonChecked( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case AudioOutputIsMuteCkBoxId: //音频输出静音复选框。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						MediaPocsThrdSetAudioOutputIsMute( g_MediaPocsThrdPt, ( IsDlgButtonChecked( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case VideoInputIsBlackCkBoxId: //视频输入黑屏复选框。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						MediaPocsThrdSetVideoInputIsBlack( g_MediaPocsThrdPt, ( IsDlgButtonChecked( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case VideoOutputIsBlackCkBoxId: //视频输出黑屏复选框。
				{
					if( g_MediaPocsThrdPt != NULL )
					{
						MediaPocsThrdSetVideoOutputIsBlack( g_MediaPocsThrdPt, ( IsDlgButtonChecked( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case CreateSrvrBtnId: //创建服务器按钮。
				case CnctSrvrBtnId: //连接服务器按钮。
				{
					if( g_MediaPocsThrdPt == NULL ) //如果媒体处理线程还没有启动。
					{
						LOGI( "开始启动媒体处理线程。" );

						//创建媒体信息的内存块。
						g_MediaInfoPt = ( MediaInfo * )calloc( 1, sizeof( MediaInfo ) );
						if( g_MediaInfoPt == NULL )
						{
							LOGE( "创建媒体信息的内存块失败。" );
							goto OutCreateSrvrOrClnt;
						}
						
						//初始化媒体处理线程的指针。
						if( MediaPocsThrdInit( &g_MediaPocsThrdPt, g_MediaInfoPt,
											   MyMediaPocsThrdUserInit, MyMediaPocsThrdUserProcess, MyMediaPocsThrdUserDstoy,
											   MyMediaPocsThrdUserReadAudioVideoInputFrame,
											   MyMediaPocsThrdUserWriteAudioOutputFrame, MyMediaPocsThrdUserGetPcmAudioOutputFrame,
											   MyMediaPocsThrdUserWriteVideoOutputFrame, MyMediaPocsThrdUserGetYU12VideoOutputFrame,
											   g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "初始化媒体处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto OutCreateSrvrOrClnt;
						}

						g_MediaInfoPt->m_MainDlgWndHdl = g_MainDlgWndHdl; //设置主对话框窗口的句柄。

						//设置网络。
						{
							HWND p_TmpWndHdl;
							
							//设置IP地址动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, IPAddrCbBoxId );
							if( VarStrInit( &g_MediaInfoPt->m_IPAddrVarStrPt ) != 0 )
							{
								LOGE( "初始化IP地址动态字符串失败。" );
								goto OutCreateSrvrOrClnt;
							}
							VarStrSetSz( g_MediaInfoPt->m_IPAddrVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, IPAddrCbBoxId, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrSz );

							//设置端口动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, PortEdTxtId );
							if( VarStrInit( &g_MediaInfoPt->m_PortVarStrPt ) != 0 )
							{
								LOGE( "初始化端口动态字符串失败。" );
								goto OutCreateSrvrOrClnt;
							}
							VarStrSetSz( g_MediaInfoPt->m_PortVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, PortEdTxtId, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrSz );
							
							//设置使用什么传输协议。
							g_MediaInfoPt->m_UseWhatXfrPrtcl = ( IsDlgButtonChecked( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ) == BST_CHECKED ) ? 0 : 1;

							//设置最大连接次数。
							{
								char p_TmpStrPt[100];
							
								GetWindowText( GetDlgItem( g_XfrPrtclStngDlgWndHdl, MaxCnctTimesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								g_MediaInfoPt->m_MaxCnctTimes = atoi( p_TmpStrPt );
							}
							
							//设置创建服务端或者客户端标记。
							g_MediaInfoPt->m_IsCreateSrvrOrClnt = ( LOWORD( wParam ) == CreateSrvrBtnId ) ? 1 : 0;

							//设置是否自动允许连接。
							g_MediaInfoPt->m_IsAutoAllowCnct = ( IsDlgButtonChecked( g_XfrPrtclStngDlgWndHdl, IsAutoAllowCnctCkBoxId ) == BST_CHECKED ) ? 1 : 0;
						}
						
						//设置是否使用链表。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseLnkLstRecvOutputFrameRdBtnId ) == BST_CHECKED )
						{
							g_MediaInfoPt->m_UseWhatRecvOutputFrame = 0;
						}
						
						//设置是否使用自己设计的自适应抖动缓冲器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseAjbRecvOutputFrameRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int p_AAjbMinNeedBufFrameCnt;
							int p_AAjbMaxNeedBufFrameCnt;
							float p_AAjbAdaptSensitivity;
							int p_VAjbMinNeedBufFrameCnt;
							int p_VAjbMaxNeedBufFrameCnt;
							float p_VAjbAdaptSensitivity;

							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrameCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMinNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrameCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMaxNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbAdaptSensitivity = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrameCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMinNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrameCntEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMaxNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbAdaptSensitivity = atof( p_TmpStrPt );

							g_MediaInfoPt->m_UseWhatRecvOutputFrame = 1;
							g_MediaInfoPt->m_AAjbMinNeedBufFrameCnt = p_AAjbMinNeedBufFrameCnt;
							g_MediaInfoPt->m_AAjbMaxNeedBufFrameCnt = p_AAjbMaxNeedBufFrameCnt;
							g_MediaInfoPt->m_AAjbAdaptSensitivity = p_AAjbAdaptSensitivity;
							g_MediaInfoPt->m_VAjbMinNeedBufFrameCnt = p_VAjbMinNeedBufFrameCnt;
							g_MediaInfoPt->m_VAjbMaxNeedBufFrameCnt = p_VAjbMaxNeedBufFrameCnt;
							g_MediaInfoPt->m_VAjbAdaptSensitivity = p_VAjbAdaptSensitivity;
						}

						//设置是否保存设置到文件。
						MediaPocsThrdSetIsSaveStngToFile( g_MediaPocsThrdPt,
														  ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														  ".\\Setting.txt",
														  g_ErrInfoVarStrPt );
						
						//设置是否打印Log日志、显示Toast。
						MediaPocsThrdSetIsPrintLogShowToast( g_MediaPocsThrdPt,
															 ( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															 ( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															 g_ErrInfoVarStrPt );

						//设置是否使用音频输入。
						MediaPocsThrdSetIsUseAudioInput( g_MediaPocsThrdPt,
														 ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ) == BST_CHECKED ) ? 1 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate32000RdBtnId ) == BST_CHECKED ) ? 32000 : 
														       ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
														 g_ErrInfoVarStrPt );

						//设置是否不使用声学回音消除器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUseNoAec( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}

						//设置是否使用Speex声学回音消除器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_FilterLen;
							int32_t p_IsUseRec;
							float p_EchoMultiple;
							float p_EchoCont;
							int32_t p_EchoSupes;
							int32_t p_EchoSupesAct;
							int p_IsSaveMemFile;
							
							GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_FilterLen = atoi( p_TmpStrPt );
							p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupesAct = atoi( p_TmpStrPt );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

							MediaPocsThrdSetAudioInputUseSpeexAec( g_MediaPocsThrdPt, p_FilterLen, p_IsUseRec, p_EchoMultiple, p_EchoCont, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, ".\\SpeexAecMem", g_ErrInfoVarStrPt );
						}

						//设置是否使用WebRtc定点版声学回音消除器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecmRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseCNGMode;
							int32_t p_EchoMode;
							int32_t p_Delay;
							
							p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							
							MediaPocsThrdSetAudioInputUseWebRtcAecm( g_MediaPocsThrdPt, p_IsUseCNGMode, p_EchoMode, p_Delay, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用WebRtc浮点版声学回音消除器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EchoMode;
							int32_t p_Delay;
							int32_t p_IsUseDelayAgnosticMode;
							int32_t p_IsUseExtdFilterMode;
							int32_t p_IsUseRefinedFilterAdaptAecMode;
							int32_t p_IsUseAdaptAdjDelay;
							int p_IsSaveMemFile;

							GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							p_IsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId ) == BST_CHECKED );
							p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
							p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
							p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

							MediaPocsThrdSetAudioInputUseWebRtcAec( g_MediaPocsThrdPt, p_EchoMode, p_Delay, p_IsUseDelayAgnosticMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, ".\\WebRtcAecMem", g_ErrInfoVarStrPt );
						}

						//设置是否使用SpeexWebRtc三重声学回音消除器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
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
							int32_t p_WebRtcAecIsUseDelayAgnosticMode;
							int32_t p_WebRtcAecIsUseExtdFilterMode;
							int32_t p_WebRtcAecIsUseRefinedFilterAdaptAecMode;
							int32_t p_WebRtcAecIsUseAdaptAdjDelay;
							int32_t p_IsUseSameRoomAec;
							int32_t p_SameRoomEchoMinDelay;

							p_WorkMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId ) == BST_CHECKED ) ? 1 :
										   ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? 2 :
											 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? 3 : 0;
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecFilterLen = atoi( p_TmpStrPt );
							p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupesAct = atoi( p_TmpStrPt );
							p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmDelay = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecDelay = atoi( p_TmpStrPt );
							p_WebRtcAecIsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId ) == BST_CHECKED );
							p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
							p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
							p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
							p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SameRoomEchoMinDelay = atoi( p_TmpStrPt );

							MediaPocsThrdSetAudioInputUseSpeexWebRtcAec( g_MediaPocsThrdPt, p_WorkMode, p_SpeexAecFilterLen, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCont, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgnosticMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, g_ErrInfoVarStrPt );
						}
						
						//设置是否不使用噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUseNoNs( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}

						//设置是否使用Speex预处理器的噪音抑制。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexPrpocsNsRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseNs;
							int32_t p_NoiseSupes;
							int32_t p_IsUseDereverb;

							p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_NoiseSupes = atoi( p_TmpStrPt );
							p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId ) == BST_CHECKED );

							MediaPocsThrdSetAudioInputUseSpeexPrpocsNs( g_MediaPocsThrdPt, p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, g_ErrInfoVarStrPt );
						}

						//设置是否使用WebRtc定点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							MediaPocsThrdSetAudioInputUseWebRtcNsx( g_MediaPocsThrdPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用WebRtc浮点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							MediaPocsThrdSetAudioInputUseWebRtcNs( g_MediaPocsThrdPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用RNNoise噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUseRNNoise( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用Speex预处理器的其他功能。
						{
							char p_TmpStrPt[100];
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
							p_VadProbStart = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbCont = atoi( p_TmpStrPt );
							p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcLevel = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcIncrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcDecrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcMaxGain = atoi( p_TmpStrPt );

							MediaPocsThrdSetAudioInputIsUseSpeexPrpocsOther( g_MediaPocsThrdPt, ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId ) == BST_CHECKED ) ? 1 : 0, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUsePcm( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用Speex编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EncoderUseCbrOrVbr;
							int32_t p_EncoderQualt;
							int32_t p_EncoderCmplxt;
							int32_t p_EncoderPlcExpectedLossRate;

							p_EncoderUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId ) == BST_CHECKED ) ? 0 : 1;
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderQualt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderCmplxt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderPlcExpectedLossRate = atoi( p_TmpStrPt );
							
							MediaPocsThrdSetAudioInputUseSpeexEncoder( g_MediaPocsThrdPt, p_EncoderUseCbrOrVbr, p_EncoderQualt, p_EncoderCmplxt, p_EncoderPlcExpectedLossRate, g_ErrInfoVarStrPt );
						}

						//设置是否使用Opus编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUseOpusEncoder( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置音频输入是否保存音频到文件。
						MediaPocsThrdSetAudioInputIsSaveAudioToFile( g_MediaPocsThrdPt,
																	 ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAudioToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																	 ".\\AudioInput.wav",
																	 ".\\AudioResult.wav",
																	 g_ErrInfoVarStrPt );
						
						//设置音频输入是否绘制音频波形到窗口。
						MediaPocsThrdSetAudioInputIsDrawAudioOscilloToWnd( g_MediaPocsThrdPt,
																		   ( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAudioOscilloToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																		   GetDlgItem( g_MainDlgWndHdl, AudioInputOscilloBtnId ),
																		   GetDlgItem( g_MainDlgWndHdl, AudioResultOscilloBtnId ),
																		   g_ErrInfoVarStrPt );
						
						//设置使用的音频输入设备。
						MediaPocsThrdSetAudioInputUseDvc( g_MediaPocsThrdPt,
														  SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														  g_ErrInfoVarStrPt );

						//设置音频输入是否静音。
						MediaPocsThrdSetAudioInputIsMute( g_MediaPocsThrdPt,
														  ( IsDlgButtonChecked( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														  g_ErrInfoVarStrPt );

						//设置是否使用音频输出。
						MediaPocsThrdSetIsUseAudioOutput( g_MediaPocsThrdPt,
														  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ) == BST_CHECKED ) ? 1 :
														    ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
														    ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
														      ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
														        ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioSamplingRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
														  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
														    ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
														      ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAudioFrameLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
														  g_ErrInfoVarStrPt );
						
						//设置是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioInputUsePcm( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置是否使用Speex解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
						{
							int32_t p_DecoderIsUsePerceptualEnhancement;

							p_DecoderIsUsePerceptualEnhancement = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId ) == BST_CHECKED );

							MediaPocsThrdSetAudioOutputUseSpeexDecoder( g_MediaPocsThrdPt, p_DecoderIsUsePerceptualEnhancement, g_ErrInfoVarStrPt );
						}

						//设置是否使用Opus解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetAudioOutputUseOpusDecoder( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置音频输出是否保存音频到文件。
						MediaPocsThrdSetAudioOutputIsSaveAudioToFile( g_MediaPocsThrdPt,
																	  ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAudioToFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																	  ".\\AudioOutput.wav",
																	  g_ErrInfoVarStrPt );
						
						//设置音频输出是否绘制音频波形到窗口。
						MediaPocsThrdSetAudioOutputIsDrawAudioOscilloToWnd( g_MediaPocsThrdPt,
																			( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAudioOscilloToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			GetDlgItem( g_MainDlgWndHdl, AudioOutputOscilloBtnId ),
																			g_ErrInfoVarStrPt );
						
						//设置使用的音频输出设备。
						MediaPocsThrdSetAudioOutputUseDvc( g_MediaPocsThrdPt,
														   SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														   g_ErrInfoVarStrPt );
						
						//设置音频输出是否静音。
						MediaPocsThrdSetAudioOutputIsMute( g_MediaPocsThrdPt,
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														   g_ErrInfoVarStrPt );
						
						//设置是否使用视频输入。
						MediaPocsThrdSetIsUseVideoInput( g_MediaPocsThrdPt,
														 ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 :
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId ) == BST_CHECKED ) ? 12 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoSamplingRate15RdBtnId ) == BST_CHECKED ) ? 15 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoSamplingRate24RdBtnId ) == BST_CHECKED ) ? 24 :
															   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoSamplingRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId ) == BST_CHECKED ) ? 120 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize240_320RdBtnId ) == BST_CHECKED ) ? 240 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize480_640RdBtnId ) == BST_CHECKED ) ? 480 :
															   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize960_1280RdBtnId ) == BST_CHECKED ) ? 960 : 0,
														 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId ) == BST_CHECKED ) ? 160 :
														   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize240_320RdBtnId ) == BST_CHECKED ) ? 320 :
														     ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize480_640RdBtnId ) == BST_CHECKED ) ? 640 :
															   ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoFrameSize960_1280RdBtnId ) == BST_CHECKED ) ? 1280 : 0,
														 GetDlgItem( g_MainDlgWndHdl, VideoInputPrvwTxtId ),
														 g_ErrInfoVarStrPt );
						
						//设置视频输入是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetVideoInputUseYU12( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置视频输入是否使用OpenH264编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264RdBtnId ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_OpenH264EncoderVideoType;
							int32_t p_OpenH264EncoderEncodedBitrate;
							int32_t p_OpenH264EncoderBitrateControlMode;
							int32_t p_OpenH264EncoderIDRFrameIntvl;
							int32_t p_OpenH264EncoderCmplxt;

							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderVideoType = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderEncodedBitrate = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderBitrateControlMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderIDRFrameIntvl = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderCmplxt = atoi( p_TmpStrPt );

							MediaPocsThrdSetVideoInputUseOpenH264Encoder( g_MediaPocsThrdPt,
																		  p_OpenH264EncoderVideoType,
																		  p_OpenH264EncoderEncodedBitrate * 1024 * 8,
																		  p_OpenH264EncoderBitrateControlMode,
																		  p_OpenH264EncoderIDRFrameIntvl,
																		  p_OpenH264EncoderCmplxt,
																		  g_ErrInfoVarStrPt );
						}
						
						//设置使用的视频输入设备。
						MediaPocsThrdSetVideoInputUseDvc( g_MediaPocsThrdPt,
														  SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), CB_GETCURSEL, 0, 0 ) - 1,
														  g_ErrInfoVarStrPt );
						
						//设置视频输入是否黑屏。
						MediaPocsThrdSetVideoInputIsBlack( g_MediaPocsThrdPt,
														   ( IsDlgButtonChecked( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
														   g_ErrInfoVarStrPt );

						//设置是否使用视频输出。
						MediaPocsThrdSetIsUseVideoOutput( g_MediaPocsThrdPt,
														  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 :
														    ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ) == BST_CHECKED ) ? 1 : 0,
														  GetDlgItem( g_MainDlgWndHdl, VideoOutputDspyTxtId ),
														  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId ) == BST_CHECKED ) ? 1.0f :
														    ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoDspyScale1_5RdBtnId ) == BST_CHECKED ) ? 1.5f :
														      ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoDspyScale2_0RdBtnId ) == BST_CHECKED ) ? 2.0f :
															    ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVideoDspyScale3_0RdBtnId ) == BST_CHECKED ) ? 3.0f : 0,
														  g_ErrInfoVarStrPt );
						
						//设置视频输出是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetVideoOutputUseYU12( g_MediaPocsThrdPt, g_ErrInfoVarStrPt );
						}
						
						//设置视频输出是否使用OpenH264解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264RdBtnId ) == BST_CHECKED )
						{
							MediaPocsThrdSetVideoOutputUseOpenH264Decoder( g_MediaPocsThrdPt, 0, g_ErrInfoVarStrPt );
						}
						
						//设置视频输出是否黑屏。
						MediaPocsThrdSetVideoOutputIsBlack( g_MediaPocsThrdPt,
															( IsDlgButtonChecked( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															g_ErrInfoVarStrPt );

						//启动媒体处理线程。
						if( MediaPocsThrdStart( g_MediaPocsThrdPt, g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "启动媒体处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto OutCreateSrvrOrClnt;
						}

						LOGI( "启动媒体处理线程完毕。" );

						p_Result = 0; // //设置本函数执行成功。

						OutCreateSrvrOrClnt:
						if( p_Result != 0 )
						{
							if( g_MediaPocsThrdPt != NULL )
							{
								MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。
								MediaPocsThrdDstoy( g_MediaPocsThrdPt, g_ErrInfoVarStrPt ); //销毁媒体处理线程。
								g_MediaPocsThrdPt = NULL;
							}
							if( g_MediaInfoPt != NULL )
							{
								VarStrDstoy( g_MediaInfoPt->m_IPAddrVarStrPt );
								VarStrDstoy( g_MediaInfoPt->m_PortVarStrPt );
								free( g_MediaInfoPt );
								g_MediaInfoPt = NULL;
							}
						}
					}
					else //如果媒体处理线程已经启动。
					{
						MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。

						p_Result = 0; // //设置本函数执行成功。
					}
					return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
				}
				case RqstCnctAllowBtnId: //请求连接对话框的允许按钮。
				{
					g_MediaInfoPt->m_RqstCnctResult = 1;
					EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case RqstCnctRefuseBtnId: //请求连接对话框的拒绝按钮。
				{
					g_MediaInfoPt->m_RqstCnctResult = 2;
					EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case XfrPrtclStngBtnId: //传输协议设置按钮。
				{
					ShowWindow( g_XfrPrtclStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示传输协议设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case StngBtnId: //设置按钮。
				{
					ShowWindow( g_StngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case UseEffectLowRdBtnId: //效果等级：低。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId, UseAudioSamplingRate48000RdBtnId, UseAudioSamplingRate8000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId, UseAudioFrameLen30msRdBtnId, UseAudioFrameLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecmRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseSpeexPrpocsNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId, UseVideoSamplingRate30RdBtnId, UseVideoSamplingRate12RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId, UseVideoFrameSize960_1280RdBtnId, UseVideoFrameSize120_160RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId, UseVideoDspyScale3_0RdBtnId, UseVideoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264RdBtnId, UseOpenH264RdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), "1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), "0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), "380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), "3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), "95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), "25" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId, SpeexCodecEncoderUseCbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), "1" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), "12" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), "0" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectMidRdBtnId: //效果等级：中。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectMidRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId, UseAudioSamplingRate48000RdBtnId, UseAudioSamplingRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId, UseAudioFrameLen30msRdBtnId, UseAudioFrameLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsxRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId, UseVideoSamplingRate30RdBtnId, UseVideoSamplingRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId, UseVideoFrameSize960_1280RdBtnId, UseVideoFrameSize240_320RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId, UseVideoDspyScale3_0RdBtnId, UseVideoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264RdBtnId, UseOpenH264RdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), "1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), "0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), "380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), "3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), "95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), "25" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId, SpeexCodecEncoderUseCbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), "4" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), "15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), "0" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectHighRdBtnId: //效果等级：高。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectHighRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId, UseAudioSamplingRate48000RdBtnId, UseAudioSamplingRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId, UseAudioFrameLen30msRdBtnId, UseAudioFrameLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId, UseVideoSamplingRate30RdBtnId, UseVideoSamplingRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId, UseVideoFrameSize960_1280RdBtnId, UseVideoFrameSize480_640RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId, UseVideoDspyScale3_0RdBtnId, UseVideoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264RdBtnId, UseOpenH264RdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), "1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), "0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), "380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), "3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), "95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), "25" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), "8" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), "15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), "1" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectSuperRdBtnId: //效果等级：超。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectSuperRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId, UseAudioSamplingRate48000RdBtnId, UseAudioSamplingRate16000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId, UseAudioFrameLen30msRdBtnId, UseAudioFrameLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId, UseVideoSamplingRate30RdBtnId, UseVideoSamplingRate24RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId, UseVideoFrameSize960_1280RdBtnId, UseVideoFrameSize480_640RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId, UseVideoDspyScale3_0RdBtnId, UseVideoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264RdBtnId, UseOpenH264RdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), "1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), "0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), "380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), "3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), "95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), "25" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), "10" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), "24" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), "1" );

					return ( INT_PTR )TRUE;
				}
				case UseEffectPremiumRdBtnId: //效果等级：特。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseEffectPremiumRdBtnId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioSamplingRate8000RdBtnId, UseAudioSamplingRate48000RdBtnId, UseAudioSamplingRate32000RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseAudioFrameLen10msRdBtnId, UseAudioFrameLen30msRdBtnId, UseAudioFrameLen20msRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsOtherCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoSamplingRate12RdBtnId, UseVideoSamplingRate30RdBtnId, UseVideoSamplingRate30RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoFrameSize120_160RdBtnId, UseVideoFrameSize960_1280RdBtnId, UseVideoFrameSize960_1280RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVideoDspyScale1_0RdBtnId, UseVideoDspyScale3_0RdBtnId, UseVideoDspyScale1_0RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264RdBtnId, UseOpenH264RdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenEdTxtId ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), "1.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), "0.6" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), "380" );

					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), "-32768" );
					CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), "3" );

					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), "95" );
					CheckDlgButton( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsOtherStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), "25" );

					CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderUseCbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId, SpeexCodecEncoderUseVbrRdBtnId );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderCmplxtEdTxtId ), "10" );
					CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexCodecIsUsePerceptualEnhancementCkBoxId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderVideoTypeEdTxtId ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderBitrateControlModeEdTxtId ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderIDRFrameIntvlEdTxtId ), "30" );
					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderCmplxtEdTxtId ), "2" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateLowRdBtnId: //比特率等级：低。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateLowRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), "1" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), "1" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), "10" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateMidRdBtnId: //比特率等级：中。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateMidRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), "4" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), "40" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), "20" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateHighRdBtnId: //比特率等级：高。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateHighRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), "8" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), "80" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), "40" );

					return ( INT_PTR )TRUE;
				}
				case UseBitrateSuperRdBtnId: //比特率等级：超。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitrateSuperRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), "100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), "60" );

					return ( INT_PTR )TRUE;
				}
				case UseBitratePremiumRdBtnId: //比特率等级：特。
				{
					CheckDlgButton( g_StngDlgWndHdl, UseBitratePremiumRdBtnId, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderQualtEdTxtId ), "10" );
					SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexCodecEncoderPlcExpectedLossRateEdTxtId ), "100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncoderEncodedBitrateEdTxtId ), "80" );

					return ( INT_PTR )TRUE;
				}
				case AjbStngBtnId: //自适应抖动缓冲器设置按钮。
				{
					ShowWindow( g_AjbStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexAecStngBtnId: //Speex声学回音消除设置按钮。
				{
					ShowWindow( g_SpeexAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex声学回音消除器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexAecDelMemFileBtnId: //Speex声学回音消除器的删除内存块文件按钮。
				{
					if( DeleteFile( ".\\SpeexAecMem" ) != 0 )
					{
						MessageBox( hDlg, "删除Speex声学回音消除器的内存块文件 SpeexAecMem 成功。", "Windows下音视频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						VarStrFmtCpy( g_ErrInfoVarStrPt, "删除Speex声学回音消除器的内存块文件 SpeexAecMem 失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
						MessageBox( hDlg, g_ErrInfoVarStrPt->m_StrPt, "Windows下音视频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecmStngBtnId: //WebRtc定点版声学回音消除器设置按钮。
				{
					ShowWindow( g_WebRtcAecmStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版声学回音消除器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecStngBtnId: //WebRtc浮点版声学回音消除器设置按钮。
				{
					ShowWindow( g_WebRtcAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版声学回音消除器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAecDelMemFileBtnId: //WebRtc浮点版声学回音消除器的删除内存块文件控件。
				{
					if( DeleteFile( ".\\WebRtcAecMem" ) != 0 )
					{
						MessageBox( hDlg, "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 成功。", "Windows下音视频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						VarStrFmtCpy( g_ErrInfoVarStrPt, "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
						MessageBox( hDlg, g_ErrInfoVarStrPt->m_StrPt, "Windows下音视频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case SpeexWebRtcAecStngBtnId: //SpeexWebRtc三重声学回音消除器设置按钮。
				{
					ShowWindow( g_SpeexWebRtcAecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示SpeexWebRtc三重声学回音消除器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsNsStngBtnId: //Speex预处理器的噪音抑制设置按钮。
				{
					ShowWindow( g_SpeexPrpocsNsStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的噪音抑制设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsxStngBtnId: //WebRtc定点版噪音抑制器设置按钮。
				{
					ShowWindow( g_WebRtcNsxStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版噪音抑制器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcNsStngBtnId: //WebRtc浮点版噪音抑制器设置按钮。
				{
					ShowWindow( g_WebRtcNsStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版噪音抑制器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexPrpocsOtherStngBtnId: //Speex预处理器的其他功能设置按钮。
				{
					ShowWindow( g_SpeexPrpocsOtherStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的其他功能设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case SpeexCodecStngBtnId: //Speex编解码器设置按钮。
				{
					ShowWindow( g_SpeexCodecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex编解码器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case OpenH264StngBtnId: //OpenH264编解码器设置按钮。
				{
					ShowWindow( g_OpenH264CodecStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示OpenH264编解码器设置对话框的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case ReadMeBtnId: //必读说明按钮。
				{
					ShellExecute( NULL, "open", "https://github.com/cyz7758520/WindowsAVTalkbackDemo", NULL, NULL, SW_SHOWNORMAL ); //打开必读说明网页。
					return ( INT_PTR )TRUE;
				}
				case ClearLogBtnId: //清空日志按钮。
				{
					SendMessage( GetDlgItem( hDlg, LogLtBoxId ), LB_RESETCONTENT, 0, 0 ); //清空日志列表。
					SendDlgItemMessage( hDlg, LogLtBoxId, LB_SETHORIZONTALEXTENT, 0, 0 ); //清空滚动条的当前像素大小。
					return ( INT_PTR )TRUE;
				}
				case IDCANCEL: //窗口右上角关闭按钮。
				case XfrPrtclStngOkBtnId:
				case StngOkBtnId:
				case AjbStngOkBtnId:
				case SpeexAecStngOkBtnId:
				case WebRtcAecmStngOkBtnId:
				case WebRtcAecStngOkBtnId:
				case SpeexWebRtcAecStngOkBtnId:
				case SpeexPrpocsNsStngOkBtnId:
				case WebRtcNsxStngOkBtnId:
				case WebRtcNsStngOkBtnId:
				case SpeexPrpocsOtherStngOkBtnId:
				case SpeexCodecStngOkBtnId:
				case OpenH264StngOkBtnId:
				{
					if( g_MainDlgWndHdl == hDlg ) //如果是主对话框。
					{
						LOGI( "用户在主对话框按下关闭控件，本软件退出。" );

						MediaPocsThrdRqirExit( g_MediaPocsThrdPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。

						PostQuitMessage( 0 ); //发送退出消息。
					}
					else //如果是其他对话框。
					{
						EnableWindow( GetParent( hDlg ), TRUE ), ShowWindow( hDlg, SW_HIDE ); //隐藏当前模态对话框。
					}
					return ( INT_PTR )TRUE;
				}
				case VideoInputPrvwTxtId: //视频输入预览文本框。
				{
					if( g_VideoInputPrvwTxtWndIsMax == 0 ) //如果视频输入预览文本框窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainDlgWndHdl, &p_Rect );
						SetWindowPos( g_VideoInputPrvwTxtWndHdl, g_VideoOutputDspyTxtWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_HIDE ); //设置TCP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_HIDE ); //设置UDP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_HIDE ); //设置传输协议设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_HIDE ); //设置IP地址控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_HIDE ); //设置端口控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ), SW_HIDE ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ), SW_HIDE ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ), SW_HIDE ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), SW_HIDE ); //设置音频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), SW_HIDE ); //设置音频输出设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), SW_HIDE ); //设置视频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_HIDE ); //设置创建服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_HIDE ); //设置连接服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputOscilloBtnId ), SW_HIDE ); //设置音频输入波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputOscilloBtnId ), SW_HIDE ); //设置音频输出波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioResultOscilloBtnId ), SW_HIDE ); //设置音频结果波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表控件为隐藏。
						ShowWindow( g_VideoOutputDspyTxtWndHdl, SW_HIDE ); //设置视频输出显示控件为隐藏。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VideoInputPrvwTxtWndIsMax = 1;
					}
					else //如果视频输入预览文本框窗口已经最大化。
					{
						SetWindowPos( g_VideoInputPrvwTxtWndHdl, g_VideoOutputDspyTxtWndHdl, g_VideoInputPrvwTxtWndRect.left, g_VideoInputPrvwTxtWndRect.top, g_VideoInputPrvwTxtWndRect.right - g_VideoInputPrvwTxtWndRect.left, g_VideoInputPrvwTxtWndRect.bottom - g_VideoInputPrvwTxtWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_SHOW ); //设置TCP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_SHOW ); //设置UDP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_SHOW ); //设置传输协议设置控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_SHOW ); //设置IP地址控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_SHOW ); //设置端口控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ), SW_SHOW ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ), SW_SHOW ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ), SW_SHOW ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), SW_SHOW ); //设置音频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), SW_SHOW ); //设置音频输出设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), SW_SHOW ); //设置视频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ), SW_SHOW ); //设置音频输入静音控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ), SW_SHOW ); //设置音频输出静音控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ), SW_SHOW ); //设置视频输入黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ), SW_SHOW ); //设置视频输出黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_SHOW ); //设置创建服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_SHOW ); //设置连接服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_SHOW ); //设置设置控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_SHOW ); //设置必读说明控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputOscilloBtnId ), SW_SHOW ); //设置音频输入波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputOscilloBtnId ), SW_SHOW ); //设置音频输出波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioResultOscilloBtnId ), SW_SHOW ); //设置音频结果波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_SHOW ); //设置日志列表控件为显示。
						ShowWindow( g_VideoOutputDspyTxtWndHdl, SW_SHOW ); //设置视频输出显示控件为显示。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VideoInputPrvwTxtWndIsMax = 0;
					}
					return ( INT_PTR )TRUE;
				}
				case VideoOutputDspyTxtId: //视频输出显示文本框。
				{
					if( g_VideoOutputDspyTxtWndIsMax == 0 ) //如果视频输出显示文本框窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainDlgWndHdl, &p_Rect );
						SetWindowPos( g_VideoOutputDspyTxtWndHdl, g_VideoInputPrvwTxtWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_HIDE ); //设置TCP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_HIDE ); //设置UDP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_HIDE ); //设置传输协议设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_HIDE ); //设置IP地址控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_HIDE ); //设置端口控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ), SW_HIDE ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ), SW_HIDE ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ), SW_HIDE ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), SW_HIDE ); //设置音频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), SW_HIDE ); //设置音频输出设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), SW_HIDE ); //设置视频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_HIDE ); //设置创建服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_HIDE ); //设置连接服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputOscilloBtnId ), SW_HIDE ); //设置音频输入波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputOscilloBtnId ), SW_HIDE ); //设置音频输出波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioResultOscilloBtnId ), SW_HIDE ); //设置音频结果波形控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表控件为隐藏。
						ShowWindow( g_VideoInputPrvwTxtWndHdl, SW_HIDE ); //设置视频输入预览控件为隐藏。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VideoOutputDspyTxtWndIsMax = 1;
					}
					else //如果视频输出显示文本框窗口已经最大化。
					{
						SetWindowPos( g_VideoOutputDspyTxtWndHdl, g_VideoInputPrvwTxtWndHdl, g_VideoOutputDspyTxtWndRect.left, g_VideoOutputDspyTxtWndRect.top, g_VideoOutputDspyTxtWndRect.right - g_VideoOutputDspyTxtWndRect.left, g_VideoOutputDspyTxtWndRect.bottom - g_VideoOutputDspyTxtWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_SHOW ); //设置TCP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_SHOW ); //设置UDP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_SHOW ); //设置传输协议设置控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_SHOW ); //设置IP地址控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_SHOW ); //设置端口控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId ), SW_SHOW ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoTalkbackRdBtnId ), SW_SHOW ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioVideoTalkbackRdBtnId ), SW_SHOW ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioInputDvcCbBoxId ), SW_SHOW ); //设置音频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAudioOutputDvcCbBoxId ), SW_SHOW ); //设置音频输出设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVideoInputDvcCbBoxId ), SW_SHOW ); //设置视频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputIsMuteCkBoxId ), SW_SHOW ); //设置音频输入静音控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputIsMuteCkBoxId ), SW_SHOW ); //设置音频输出静音控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoInputIsBlackCkBoxId ), SW_SHOW ); //设置视频输入黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, VideoOutputIsBlackCkBoxId ), SW_SHOW ); //设置视频输出黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CreateSrvrBtnId ), SW_SHOW ); //设置创建服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctSrvrBtnId ), SW_SHOW ); //设置连接服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_SHOW ); //设置设置控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_SHOW ); //设置必读说明控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioInputOscilloBtnId ), SW_SHOW ); //设置音频输入波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioOutputOscilloBtnId ), SW_SHOW ); //设置音频输出波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, AudioResultOscilloBtnId ), SW_SHOW ); //设置音频结果波形控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志控件为显示。
						ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_SHOW ); //设置日志列表控件为显示。
						ShowWindow( g_VideoInputPrvwTxtWndHdl, SW_SHOW ); //设置视频输入预览控件为显示。
						InvalidateRect( g_MainDlgWndHdl, NULL, TRUE );
						g_VideoOutputDspyTxtWndIsMax = 0;
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
				EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), FALSE ); //设置TCP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), FALSE ); //设置UDP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), FALSE ); //设置传输协议设置控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), FALSE ); //设置端口控件为不可用。
				SetWindowText( GetDlgItem( hDlg, CreateSrvrBtnId ), "中断" ); //设置创建服务端控件的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, CnctSrvrBtnId ), FALSE ); //设置连接服务端控件为不可用。
				EnableWindow( GetDlgItem( hDlg, StngBtnId ), FALSE ); //设置设置控件为不可用。
            }
            else //如果是创建客户端。
            {
				EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), FALSE ); //设置TCP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), FALSE ); //设置UDP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), FALSE ); //设置传输协议设置控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), FALSE ); //设置端口控件为不可用。
				EnableWindow( GetDlgItem( hDlg, CreateSrvrBtnId ), FALSE ); //设置创建服务端控件为不可用。
				SetWindowText( GetDlgItem( hDlg, CnctSrvrBtnId ), "中断" ); //设置连接服务端控件的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, StngBtnId ), FALSE ); //设置设置控件为不可用。
            }
			return ( INT_PTR )TRUE;
		}
		case WM_DSTRY_MEDIA_PROC_THREAD: //销毁媒体处理线程的消息。
		{
			if( g_MediaPocsThrdPt != NULL )
			{
				MediaPocsThrdDstoy( g_MediaPocsThrdPt, g_ErrInfoVarStrPt ); //销毁媒体处理线程。
				g_MediaPocsThrdPt = NULL;
			}
			if( g_MediaInfoPt != NULL )
			{
				VarStrDstoy( g_MediaInfoPt->m_IPAddrVarStrPt );
				VarStrDstoy( g_MediaInfoPt->m_PortVarStrPt );
				if( g_MediaInfoPt->m_TmpBytePt != NULL ) free( g_MediaInfoPt->m_TmpBytePt );
				if( g_MediaInfoPt->m_TmpByte2Pt != NULL ) free( g_MediaInfoPt->m_TmpByte2Pt );
				if( g_MediaInfoPt->m_TmpByte3Pt != NULL ) free( g_MediaInfoPt->m_TmpByte3Pt );
				free( g_MediaInfoPt );
				g_MediaInfoPt = NULL;
			}

			EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), TRUE ); //设置TCP协议控件为可用。
			EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), TRUE ); //设置UDP协议控件为可用。
			EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), TRUE ); //设置传输协议设置控件为可用。
			EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), TRUE ); //设置IP地址控件为可用。
			EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), TRUE ); //设置端口控件为可用。
			EnableWindow( GetDlgItem( hDlg, CreateSrvrBtnId ), TRUE ); //设置创建服务端控件为可用。
			SetWindowText( GetDlgItem( hDlg, CreateSrvrBtnId ), "创建服务端" ); //设置创建服务端控件的内容为“创建服务端”。
			EnableWindow( GetDlgItem( hDlg, CnctSrvrBtnId ), TRUE ); //设置连接服务端控件为可用。
			SetWindowText( GetDlgItem( hDlg, CnctSrvrBtnId ), "连接服务端" ); //设置连接服务端控件的内容为“连接服务端”。
			EnableWindow( GetDlgItem( hDlg, StngBtnId ), TRUE ); //设置设置控件为可用。
			return ( INT_PTR )TRUE;
		}
		case WM_SHOW_RQST_CNCT_DIALOG: //显示请求连接对话框的消息。
		{
			if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 )
			{
				VarStrIns( ( VarStr * )wParam, 0, "您是否允许远端[" );
				VarStrCat( ( VarStr * )wParam, "]的连接？" );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( ( VarStr * )wParam )->m_StrPt ); //设置请求连接对话框文本控件的内容。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), "允许" ); //设置请求连接对话框允许控件的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_SHOW ); //设置请求连接对话框允许控件为显示。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), "拒绝" ); //设置请求连接对话框拒绝控件的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝控件为显示。
			}
			else
			{
				VarStrIns( ( VarStr * )wParam, 0, "等待远端[" );
				VarStrCat( ( VarStr * )wParam, "]允许您的连接..." );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( ( VarStr * )wParam )->m_StrPt ); //设置请求连接对话框的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_HIDE ); //设置请求连接对话框允许控件为隐藏。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), "中断" ); //设置请求连接对话框拒绝控件的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝控件为显示。
			}
			VarStrDstoy( ( VarStr * )wParam );
			ShowWindow( g_RqstCnctDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示请求连接对话框的模态对话框。
			return ( INT_PTR )TRUE;
		}
		case WM_DSTRY_RQST_CNCT_DIALOG: //销毁请求连接对话框的消息。
		{
			EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框的模态对话框。
			return ( INT_PTR )TRUE;
		}
		case WM_SHOW_LOG: //显示日志的消息。
		{
			if( ( VarStr * )wParam != NULL )
			{
				//显示日志。
				{
					SYSTEMTIME p_SystemTime;

					GetLocalTime( &p_SystemTime );
					VarStrFmtIns( ( VarStr * )wParam, 0, "%02.2d:%02.2d:%02.2d %03.3d：", p_SystemTime.wHour, p_SystemTime.wMinute, p_SystemTime.wSecond, p_SystemTime.wMilliseconds );
					SendMessage( GetDlgItem( hDlg, LogLtBoxId ), LB_ADDSTRING, 0, ( WPARAM )( ( VarStr * )wParam )->m_StrPt );
				}
				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, LogLtBoxId, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( ( VarStr * )wParam )->m_StrPt, ( ( VarStr * )wParam )->m_StrLen, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, LogLtBoxId, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}
				VarStrDstoy( ( VarStr * )wParam );
			}
			return ( INT_PTR )TRUE;
		}
		case WM_REPAINT: //重绘消息。
		{
			InvalidateRect( g_VideoInputPrvwTxtWndHdl, NULL, TRUE ); //重绘视频输入预览文本框窗口。
			InvalidateRect( g_VideoOutputDspyTxtWndHdl, NULL, TRUE ); //重绘视频输出显示文本框窗口。
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
	//_CrtSetBreakAlloc( 322 );
	#endif

	size_t LogNum;

	g_IstnsHdl = hInstance; //设置当前实例的句柄。
	
	FuncSetCurActPath( NULL, NULL ); //设置当前进程活动目录的路径为为当前进程可执行文件的上级路径。

	VarStrInit( &g_ErrInfoVarStrPt ); //创建并初始化错误信息动态字符串。

	//初始化日志。
	DeleteFile( ".\\log.txt" );
	LogOpenFile( ".\\log.txt", &LogNum, g_ErrInfoVarStrPt );
	LogSetDfltFile( LogNum, g_ErrInfoVarStrPt );
	
	//初始化套接字库。
	WORD p_VersionRequested = MAKEWORD( 2, 2 );
	WSADATA p_WsaData;
	WSAStartup( p_VersionRequested, &p_WsaData );

	//判断应用程序可执行文件的名称是否为“Windows下音视频对讲演示程序.exe”。
	{
		HANDLE p_CurProcHdl;
		char p_ProcessImageFileName[260];
		const char * p_AppFileNamePt;

		//获取Windows应用程序的进程名字符串。
		p_CurProcHdl = GetCurrentProcess();
		GetProcessImageFileName( p_CurProcHdl, p_ProcessImageFileName, sizeof( p_ProcessImageFileName ) );
		FuncPathFindFileName( p_ProcessImageFileName, &p_AppFileNamePt );
		CloseHandle( p_CurProcHdl );
		if( ( memcmp( p_AppFileNamePt, "Windows下音视频对讲演示程序.exe", strlen( "Windows下音视频对讲演示程序.exe" ) + 1 ) != 0 ) &&
			( memcmp( p_AppFileNamePt, "WindowsAVTalkbackDemo", strlen( "WindowsAVTalkbackDemo" ) ) != 0 ) )
		{
			if( MessageBox( NULL, "应用程序可执行文件的名称不为“Windows下音视频对讲演示程序.exe”，运行会报错，是否继续？", "警告", MB_YESNO | MB_ICONWARNING ) == IDNO )
			{
				return 0;
			}
		}
	}

	//创建窗口。
	g_MainDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( MainDlgId ), NULL, WndMsgPocsPocdr );
	g_RqstCnctDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( RqstCnctDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
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
	g_OpenH264CodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( OpenH264StngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );

	//设置窗口图标。
	#if( ( defined __X86__ ) )
	SetClassLong( g_MainDlgWndHdl, GCL_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WindowAVTalkbackDemoIconId ) ) );
	#elif( ( defined __X64__ ) )
	SetClassLong( g_MainDlgWndHdl, GCLP_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WindowAVTalkbackDemoIconId ) ) );
	#endif

	//设置使用UDP协议。
	CheckRadioButton( g_MainDlgWndHdl, UseTcpPrtclRdBtnId, UseUdpPrtclRdBtnId, UseUdpPrtclRdBtnId );

	//设置传输协议。
	SetWindowText( GetDlgItem( g_XfrPrtclStngDlgWndHdl, MaxCnctTimesEdTxtId ), "5" );
	CheckDlgButton( g_XfrPrtclStngDlgWndHdl, IsAutoAllowCnctCkBoxId, BST_CHECKED );

	//设置IP地址控件的内容。
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

		//设置IP地址控件的内容为本地节点的IP地址。
		if( p_LclNodeAddrInfoListPt != NULL )
		{
			for( p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListPt; p_LclNodeAddrInfoListLoopPt != NULL; p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListLoopPt->ai_next )
			{
				VarStrFmtCpy( g_ErrInfoVarStrPt, "%uz8d.%uz8d.%uz8d.%uz8d", ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b1, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b2, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b3, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b4 );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )g_ErrInfoVarStrPt->m_StrPt );
			}
			freeaddrinfo( p_LclNodeAddrInfoListPt ); //销毁本地节点地址信息结构体链表。
		}
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )"0.0.0.0" );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )"127.0.0.1" );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), CB_SETCURSEL, 0, 0 ); //设置IP地址控件默认选择第一个IP地址。
	}

	//设置端口控件的内容。
	SetWindowText( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), "12345" );

	//设置使用音频对讲模式。
	CheckRadioButton( g_MainDlgWndHdl, UseAudioTalkbackRdBtnId, UseAudioVideoTalkbackRdBtnId, UseAudioTalkbackRdBtnId );

	//初始化音频输入设备和音频输出设备的控件。
	PostMessage( g_MainDlgWndHdl, WM_DEVICECHANGE, 0, 0 );

	//默认设置。
	{
		CheckRadioButton( g_StngDlgWndHdl, UseLnkLstRecvOutputFrameRdBtnId, UseAjbRecvOutputFrameRdBtnId, UseAjbRecvOutputFrameRdBtnId );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveAudioToFileCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsDrawAudioOscilloToWndCkBoxId, BST_CHECKED );

		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseEffectSuperRdBtnId, 0 ); //默认效果等级：超。
		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseBitrateSuperRdBtnId, 0 ); //默认比特率等级：超。
		
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrameCntEdTxtId ), "5" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrameCntEdTxtId ), "50" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), "1.0" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrameCntEdTxtId ), "1" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrameCntEdTxtId ), "24" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), "1.0" );
	}

	//获取视频输入预览文本框窗口和视频输出显示文本框窗口的句柄和位置。
	g_VideoInputPrvwTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VideoInputPrvwTxtId );
	GetWindowRect( g_VideoInputPrvwTxtWndHdl, &g_VideoInputPrvwTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VideoInputPrvwTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VideoInputPrvwTxtWndRect.right );
	g_VideoOutputDspyTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VideoOutputDspyTxtId );
	GetWindowRect( g_VideoOutputDspyTxtWndHdl, &g_VideoOutputDspyTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VideoOutputDspyTxtWndRect );
	ScreenToClient( g_MainDlgWndHdl, ( LPPOINT )&g_VideoOutputDspyTxtWndRect.right );
	
	//打印当前进程活动目录的完整绝对路径到日志。
	if( g_ErrInfoVarStrPt->m_StrSz < MAX_PATH ) VarStrSetSz( g_ErrInfoVarStrPt, MAX_PATH );
	FuncGetCurActPath( g_ErrInfoVarStrPt->m_StrPt, g_ErrInfoVarStrPt->m_StrSz, &g_ErrInfoVarStrPt->m_StrLen, g_ErrInfoVarStrPt );
	VarStrIns( g_ErrInfoVarStrPt, 0, "当前进程活动目录的完整绝对路径：" );
	LOGI( g_ErrInfoVarStrPt->m_StrPt );
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainDlgWndHdl, WM_SHOW_LOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

	//显示主对话框窗口。
	ShowWindow( g_MainDlgWndHdl, SW_SHOW );

    //主消息循环。
	MSG p_Msg;
	while( GetMessage( &p_Msg, NULL, 0, 0 ) )
	{
		//if( IsDialogMessage( p_Msg.hwnd, &p_Msg ) == 0 )
		if( TranslateAccelerator( p_Msg.hwnd, NULL, &p_Msg ) == 0 )
		{
			TranslateMessage( &p_Msg );
			DispatchMessage( &p_Msg );
		}
	}

	LogDstoy(); //销毁日志。
	VarStrDstoy( g_ErrInfoVarStrPt ); //销毁错误信息动态字符串。
	
	#ifdef __DEBUG__
	_CrtDumpMemoryLeaks();
	#endif
	return (int) p_Msg.wParam;
}
