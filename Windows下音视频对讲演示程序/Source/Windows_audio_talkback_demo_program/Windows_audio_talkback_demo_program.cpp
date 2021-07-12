// Windows_audio_talkback_demo_program.cpp : 定义应用程序的入口点。
//

#include "Func.h"
#include "framework.h"
#include "Windows_audio_talkback_demo_program.h"
#include "Queue.h"
#include "LnkLst.h"
#include "Sokt.h"
#include "Ajb.h"
#include "MediaProcThread.h"
#include "shellapi.h"
#include "LibYUV.h"
#include <mmeapi.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <mmddk.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define WM_MYMediaProcThreadSTART  WM_USER + 101  //媒体处理线程启动的消息。
#define WM_MYMediaProcThreadEND    WM_USER + 102  //媒体处理线程退出的消息。
#define WM_SHOWLOG                 WM_USER + 103  //显示日志的消息。
#define WM_REPAINT                 WM_USER + 104  //重绘消息。

//存放媒体信息。
typedef struct MediaInfo
{
	VarStr * m_IPAddrVarStrPt; //存放IP地址动态字符串的内存指针。
    VarStr * m_PortVarStrPt; //存放端口号。
    HWND m_MainWindowHdl; //存放主窗口的句柄。

	int m_UseWhatXfrPrtcl; //存放使用什么传输协议，为0表示TCP协议，为1表示UDP协议。
    int m_IsCreateSrvrOrClnt; //存放创建服务端或者客户端标记，为1表示创建服务端，为0表示创建客户端。
    TcpSrvrSokt * m_TcpSrvrSoktPt; //存放本端TCP协议服务端套接字的内存指针。
    TcpClntSokt * m_TcpClntSoktPt; //存放本端TCP协议客户端套接字的内存指针。
	UdpSokt * m_UdpSoktPt; //存放本端UDP协议套接字的内存指针。
    uint64_t m_LastPktSendTime; //存放最后一个数据包的发送时间，用于判断连接是否中断。
    uint64_t m_LastPktRecvTime; //存放最后一个数据包的接收时间，用于判断连接是否中断。
	#define PKT_TYP_CNCT_HTBT 0x00 //数据包类型：连接请求包或心跳包。
	#define PKT_TYP_AFRAME 0x01 //数据包类型：音频输入输出帧。
	#define PKT_TYP_VFRAME 0x02 //数据包类型：视频输入输出帧。
	#define PKT_TYP_ACK 0x03 //数据包类型：连接应答包或音视频输入输出帧应答包。
	#define PKT_TYP_EXIT 0x04 //数据包类型：退出包。

    int m_LastSendAudioInputFrameIsAct; //存放最后一个发送的音频输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
	int m_LastSendAudioInputFrameIsRecv; //存放最后一个发送的音频输入帧远端是否接收到，为0表示没有收到，为非0表示已经收到。
    uint32_t m_LastSendAudioInputFrameTimeStamp; //存放最后一个发送音频输入帧的时间戳。
    uint32_t m_LastSendVideoInputFrameTimeStamp; //存放最后一个发送视频输入帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOutputFrame; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。
    int m_LastGetAudioOutputFrameIsAct; //存放最后一个取出的音频输出帧是否为有语音活动，为0表示否，为非0表示是。
    uint32_t m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp; //存放最后一个取出的音频输出帧对应视频输出帧的时间戳。

    VarLenLnkLst * m_RecvAudioOutputFrameLnkLstPt; //存放接收音频输出帧链表的内存指针。
	VarLenLnkLst * m_RecvVideoOutputFrameLnkLstPt; //存放接收视频输出帧链表的内存指针。

    AAjb * m_AAjbPt; //存放音频自适应抖动缓冲器类对象的内存指针。
    int m_AAjbMinNeedBufFrameCnt; //存放音频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
    int m_AAjbMaxNeedBufFrameCnt; //存放音频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
    float m_AAjbAdaptSensitivity; //存放音频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。
    VAjb * m_VAjbPt; //存放视频自适应抖动缓冲器类对象的内存指针。
    int m_VAjbMinNeedBufFrameCnt; //存放视频自适应抖动缓冲器的最小需缓冲帧数量，单位个，必须大于0。
    int m_VAjbMaxNeedBufFrameCnt; //存放视频自适应抖动缓冲器的最大需缓冲帧数量，单位个，必须大于最小需缓冲帧的数量。
    float m_VAjbAdaptSensitivity; //存放视频自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0.0,127.0]。

	int8_t * m_TmpBytePt; //存放临时数据的内存指针。
	size_t m_TmpByteSz; //存放临时数据的内存大小。
	int8_t * m_TmpByte2Pt; //存放临时数据的内存指针。
	size_t m_TmpByte2Sz; //存放临时数据的内存大小。
	int8_t * m_TmpByte3Pt; //存放临时数据的内存指针。
	size_t m_TmpByte3Sz; //存放临时数据的内存大小。
}MediaInfo;

//全局变量。
HINSTANCE hInst; //当前实例。
VarStr * g_ErrInfoVarStrPt = NULL; //存放错误信息动态字符串的内存指针。
MediaProcThread * g_MediaProcThreadPt = NULL; //媒体处理线程。
MediaInfo * g_MediaInfoPt = NULL; //媒体信息。
HWND g_MainWndHdl = NULL; //存放主界面窗口的句柄。
HWND g_SettingWndHdl = NULL; //存放设置界面窗口的句柄。
HWND g_AjbSettingWndHdl = NULL; //存放自适应抖动缓冲器设置界面窗口的句柄。
HWND g_SpeexAecSettingWndHdl = NULL; //存放Speex声学回音消除器设置界面窗口的句柄。
HWND g_WebRtcAecmSettingWndHdl = NULL; //存放WebRtc定点版声学回音消除器设置界面窗口的句柄。
HWND g_WebRtcAecSettingWndHdl = NULL; //存放WebRtc浮点版声学回音消除器设置界面窗口的句柄。
HWND g_SpeexWebRtcAecSettingWndHdl = NULL; //存放SpeexWebRtc三重声学回音消除器设置界面窗口的句柄。
HWND g_SpeexPprocNsSettingWndHdl = NULL; //存放Speex预处理器的噪音抑制设置界面窗口的句柄。
HWND g_WebRtcNsxSettingWndHdl = NULL; //存放WebRtc定点版噪音抑制器设置界面窗口的句柄。
HWND g_WebRtcNsSettingWndHdl = NULL; //存放WebRtc浮点版噪音抑制器设置界面窗口的句柄。
HWND g_SpeexPprocOtherSettingWndHdl = NULL; //存放Speex预处理器的其他功能设置界面窗口的句柄。
HWND g_SpeexCodecSettingWndHdl = NULL; //存放Speex编解码器设置界面窗口的句柄。
HWND g_OpenH264CodecSettingWndHdl = NULL; //存放OpenH264编解码器设置界面窗口的句柄。
HWND g_VideoInputPreviewWndHdl = NULL; //存放视频输入预览窗口句柄。
RECT g_VideoInputPreviewWndRect = { 0 }; //存放视频输入预览窗口的位置。
int g_VideoInputPreviewWndIsMax = 0; //存放视频输入预览窗口是否最大化。
HWND g_VideoOutputDisplayWndHdl = NULL; //存放视频输出显示窗口句柄。
RECT g_VideoOutputDisplayWndRect = { 0 }; //存放视频输出显示窗口的位置。
int g_VideoOutputDisplayWndIsMax = 0; //存放视频输出显示窗口是否最大化。

// 此代码模块中包含的函数的前向声明:
INT_PTR CALLBACK DialogProc( HWND, UINT, WPARAM, LPARAM );

//用户定义的初始化函数，在本线程刚启动时回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaProcThreadUserInit( MediaProcThread * MediaProcThreadPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	
	PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_MYMediaProcThreadSTART, 0, 0 ); //向主界面发送媒体处理线程启动的消息。

	g_MediaInfoPt->m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	if( g_MediaInfoPt->m_TmpBytePt == NULL )
	{
		g_MediaInfoPt->m_TmpBytePt = ( int8_t * )malloc( 1024 * 1024 * 4 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpBytePt == NULL )
		{
			VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByteSz = 1024 * 1024 * 4; //设置临时数据的内存大小。
	}
	if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte2Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte2Pt == NULL )
		{
			VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByte2Sz = 1024 * 1024; //设置临时数据的内存大小。
	}
	if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
	{
		g_MediaInfoPt->m_TmpByte3Pt = ( int8_t * )malloc( 1024 * 1024 ); //创建临时数据的内存块。
		if( g_MediaInfoPt->m_TmpByte3Pt == NULL )
		{
			VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建临时数据的内存块失败。" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_MediaInfoPt->m_TmpByte3Sz = 1024 * 1024; //设置临时数据的内存大小。
	}

	if( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( TcpSrvrInit( &g_MediaInfoPt->m_TcpSrvrSoktPt, 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, 1, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端TCP协议服务端套接字成功。
			{
				char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

				if( TcpSrvrGetLclAddr( g_MediaInfoPt->m_TcpSrvrSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			}
			else
			{
				VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}

			char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( TcpSrvrAccept( g_MediaInfoPt->m_TcpSrvrSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, 1, &g_MediaInfoPt->m_TcpClntSoktPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( g_MediaInfoPt->m_TcpClntSoktPt != NULL ) //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						TcpSrvrDestroy( g_MediaInfoPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。
						g_MediaInfoPt->m_TcpSrvrSoktPt = NULL;

						VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						break;
					}
					else //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时，就重新接受。
					{

					}
				}
				else
				{
					VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				if( MediaProcThreadPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
			}
		}
		else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "ping -n 1 -w 1 %s", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt );
			WinExec( g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, SW_HIDE );

			int p_ReInitTimes = 1;
			while( true ) //循环连接已监听的远端TCP协议服务端套接字。
			{
				if( TcpClntInit( &g_MediaInfoPt->m_TcpClntSoktPt, 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, NULL, NULL, 5000, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字成功。
				{
					char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
					char p_LclNodePortPt[6];
					char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
					char p_RmtNodePortPt[6];

					if( TcpClntGetLclAddr( g_MediaInfoPt->m_TcpClntSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
					{
						VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" );
						LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
					}
					if( TcpClntGetRmtAddr( g_MediaInfoPt->m_TcpClntSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
					{
						VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" );
						LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
					}

					VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化本端TCP协议客户端套接字[%s:%s]，并连接已监听的远端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt, p_RmtNodeAddrPt, p_RmtNodePortPt );
					LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					break; //跳出重连。
				}
				else
				{
					VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					
                    if( p_ReInitTimes <= 5 ) //如果还需要进行重连。
                    {
						VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "开始第 %d 次重连。", p_ReInitTimes );
						LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						p_ReInitTimes++;
                        Sleep( 500 ); //暂停一下，避免CPU使用率过高。
                    }
                    else //如果不需要重连了。
                    {
                        goto out;
                    }
				}
			}
		}

		if( TcpClntSetNoDelay( g_MediaInfoPt->m_TcpClntSoktPt, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
	}
	else //如果使用UDP协议。
    {
        if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端UDP协议套接字接受远端UDP协议套接字的连接。
        {
            if( UdpInit( &g_MediaInfoPt->m_UdpSoktPt, 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

                if( UdpGetLclAddr( g_MediaInfoPt->m_UdpSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
					VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
				VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

            while( true ) //循环接受远端UDP协议套接字的连接。
            {
                if( UdpRecvPkt( g_MediaInfoPt->m_UdpSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                    {
                        if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_CNCT_HTBT ) ) //如果是连接请求包。
                        {
                            UdpConnect( g_MediaInfoPt->m_UdpSoktPt, 4, p_RmtNodeAddrPt, p_RmtNodePortPt, NULL ); //用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字，已连接的本端UDP协议套接字只能接收连接的远端UDP协议套接字发送的数据包。

                            int p_ReSendTimes = 1;
                            while( true ) //循环发送连接请求包，并接收连接应答包。
                            {
                                g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_CNCT_HTBT; //设置连接请求包。
                                if( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                                {
									VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
									LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                                    goto out;
                                }

                                while( true ) //循环接收连接应答包。
                                {
                                    if( UdpRecvPkt( g_MediaInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1000, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                                    {
                                        if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                                        {
                                            if( ( p_TmpSz >= 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] != PKT_TYP_CNCT_HTBT ) ) //如果不是连接请求包。
                                            {
                                                VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
												LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
												{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
												goto UdpSrvrReAccept; //跳出连接循环。
                                            }
                                            else //如果是连接请求包，就不管，重新接收连接应答包。
                                            {
												
                                            }
                                        }
                                        else //如果用已监听的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包超时。
                                        {
                                            if( p_ReSendTimes <= 5 ) //如果还需要进行重发。
                                            {
                                                p_ReSendTimes++;
												goto UdpSrvrReRecv; //重发连接请求包。
                                            }
                                            else //如果不需要重连了。
                                            {
                                                VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
												LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
												{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
												goto UdpSrvrReSend; //重新接受连接。
                                            }
                                        }
                                    }
                                    else
                                    {
                                        VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
										LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
										{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
										goto UdpSrvrReSend; //重新接受连接。
                                    }
                                }
								UdpSrvrReRecv:;
                            }
							UdpSrvrReSend:
							UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

							VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "本端UDP协议套接字继续保持监听来接受连接。" );
							LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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
                    VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				if( MediaProcThreadPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
            }
			UdpSrvrReAccept:;
        }
        else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端UDP协议套接字连接远端UDP协议套接字。
        {
			//Ping一下远程节点地址，这样可以快速获取ARP条目。
			VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "ping -n 1 -w 1 %s", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt );
			WinExec( g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, SW_HIDE );

            if( UdpInit( &g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

                if( UdpGetLclAddr( g_MediaInfoPt->m_UdpSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
                    VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
                VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

            if( UdpConnect( g_MediaInfoPt->m_UdpSoktPt, 4, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
            {
				VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            if( UdpGetRmtAddr( g_MediaInfoPt->m_UdpSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
            {
                UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL );
                VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端UDP协议套接字连接的远端UDP协议套接字绑定的远程节点地址和端口失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            int p_ReSendTimes = 1;
            while( true ) //循环连接已监听的远端UDP协议套接字。
            {
                g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_CNCT_HTBT; //设置连接请求包。
                if( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                {
                    UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL );
                    VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
                }

                while( true ) //循环接收连接请求包。
                {
                    if( UdpRecvPkt( g_MediaInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 1000, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                    {
                        if( p_TmpSz != -1 ) //如果用已连接的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                        {
                            if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_CNCT_HTBT ) ) //如果是连接请求包。
                            {
                                g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ACK; //设置连接应答包。
                                if( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                                {
                                    UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL );
                                    VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
									LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
									goto out;
                                }

								VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
								LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
								{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
								goto UdpClntReSend; //跳出连接循环。
                            }
                            else //如果不是连接请求包，就不管，重新接收连接请求包。
                            {

                            }
                        }
                        else //如果用已连接的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包超时。
                        {
                            if( p_ReSendTimes <= 5 ) //如果还需要进行重发。
                            {
                                p_ReSendTimes++;
                                goto UdpClntReRecv; //重发连接请求包。
                            }
                            else //如果不需要重连了。
                            {
                                UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL );
                                VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
								LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
								{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
								goto out;
                            }
                        }
                    }
                    else
                    {
                        UdpDisconnect( g_MediaInfoPt->m_UdpSoktPt, NULL );
                        VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
                    }
                }
				UdpClntReRecv:;
            }
			UdpClntReSend:;
        }
    } //协议连接结束。

	switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
	{
	    case 0: //如果使用链表。
	    {
			//创建并初始化接收音频输出帧链表。
			if( VarLenLnkLstInit( &g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaProcThreadPt->m_AudioOutput.m_FrameLen * 2, MediaProcThreadPt->m_AudioOutput.m_FrameLen * 2, SIZE_MAX, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				LOGI( "创建并初始化接收音频输出帧链表成功。" );
			}
			else
			{
				VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收音频输出帧链表失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}

			//创建并初始化接收视频输出帧链表。
			if( VarLenLnkLstInit( &g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, MediaProcThreadPt->m_AudioOutput.m_FrameLen * 2, MediaProcThreadPt->m_AudioOutput.m_FrameLen * 2, SIZE_MAX, 1, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				LOGI( "创建并初始化接收视频输出帧链表成功。" );
			}
			else
			{
				VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收视频输出帧链表失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
	        break;
	    }
	    case 1: //如果使用自适应抖动缓冲器。
	    {
			//初始化音频自适应抖动缓冲器。
            if( AAjbInit( &g_MediaInfoPt->m_AAjbPt, MediaProcThreadPt->m_AudioOutput.m_SamplingRate, MediaProcThreadPt->m_AudioOutput.m_FrameLen, 1, 1, 0, g_MediaInfoPt->m_AAjbMinNeedBufFrameCnt, g_MediaInfoPt->m_AAjbMaxNeedBufFrameCnt, g_MediaInfoPt->m_AAjbAdaptSensitivity, 1 ) == 0 )
            {
                LOGI( "创建并初始化音频自适应抖动缓冲器成功。" );
            }
            else
            {
                VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化音频自适应抖动缓冲器失败。" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	            goto out;
            }

            //初始化视频自适应抖动缓冲器。
            if( VAjbInit( &g_MediaInfoPt->m_VAjbPt, 1, g_MediaInfoPt->m_VAjbMinNeedBufFrameCnt, g_MediaInfoPt->m_VAjbMaxNeedBufFrameCnt, g_MediaInfoPt->m_VAjbAdaptSensitivity, 1 ) == 0 )
            {
                LOGI( "创建并初始化视频自适应抖动缓冲器成功。" );
            }
            else
            {
                VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化视频自适应抖动缓冲器失败。" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	            goto out;
            }
	        break;
	    }
	}

	FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间为当前时间。
	g_MediaInfoPt->m_LastPktRecvTime = g_MediaInfoPt->m_LastPktSendTime; //设置最后一个数据包的接收时间为当前时间。

	g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 0; //设置最后发送的一个音频输入帧为无语音活动。
	g_MediaInfoPt->m_LastSendAudioInputFrameIsRecv = 1; //设置最后一个发送的音频输入帧远端已经接收到。
	g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp = 0 - 1; //设置最后一个发送音频输入帧的时间戳为0的前一个，因为第一次发送音频输入帧时会递增一个步进。
    g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp = 0 - 1; //设置最后一个发送视频输入帧的时间戳为0的前一个，因为第一次发送视频输入帧时会递增一个步进。
	
	VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "开始进行对讲。" );
	LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		
	}
    return p_Result;
}

//用户定义的处理函数，在本线程运行时每隔1毫秒就回调一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyMediaProcThreadUserProcess( MediaProcThread * MediaProcThreadPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	uint32_t p_TmpUint32;
	uint64_t p_TmpUint64;

    //接收远端发送过来的一个数据包。
	if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntRecvPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
        ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpRecvPkt( g_MediaInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
    {
		if( p_TmpSz != -1 ) //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包成功。
		{
			FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

			if( p_TmpSz == 0 ) //如果数据包的数据长度为0。
			{
				LOGFE( "接收到数据包的数据长度为%" PRIuPTR "，表示没有数据，无法继续接收。", p_TmpSz );
                goto out;
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_CNCT_HTBT ) //如果是心跳包。
            {
                if( p_TmpSz > 1 ) //如果心跳包的数据长度大于1。
                {
                    LOGFE( "接收到心跳包的数据长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                LOGI( "接收到一个心跳包。" );
            }
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_AFRAME ) //如果是音频输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果音频输出帧包的数据长度小于1 + 4，表示没有音频输出帧时间戳。
                {
                    LOGFE( "接收到音频输出帧包的数据长度为%" PRIuPTR "小于1 + 4，表示没有音频输出帧时间戳，无法继续接收。", p_TmpSz );
                    goto out;
                }

				//读取音频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
				if( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 ) //如果要使用音频输出。
				{
					//将输出帧放入链表或自适应抖动缓冲器。
					switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
					{
						case 0: //如果使用链表。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
							{
								if( VarLenLnkLstPutTail( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
								{
									LOGFI( "接收一个有语音活动的音频输出帧包，并放入接收音频输出帧链表成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( "接收一个有语音活动的音频输出帧包，并放入接收音频输出帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									goto out;
								}
							}
							else //如果该音频输出帧为无语音活动。
                            {
                                LOGFI( "接收一个无语音活动的音频输出帧包，无需放入接收音频输出帧链表。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
							break;
						}
						case 1: //如果使用自适应抖动缓冲器。
						{
							if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                            {
                                AAjbPutOneFrame( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4 );
                                LOGFI( "接收一个有语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            else //如果该音频输出帧为无语音活动。
                            {
								AAjbPutOneFrame( g_MediaInfoPt->m_AAjbPt, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, 0 );
                                LOGFI( "接收一个无语音活动的音频输出帧包，并放入音频自适应抖动缓冲器成功。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
							int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
							int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
							AAjbGetBufFrameCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
							LOGFI( "音频自适应抖动缓冲器：有活动帧：%" PRIi32 "，无活动帧：%" PRIi32 "，帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufActFrameCnt, p_CurHaveBufInactFrameCnt, p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );

							break;
						}
					}
				}
				else //如果不使用音频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该音频输出帧为有语音活动。
                    {
                        LOGFI( "接收一个有语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                    else //如果该音频输出帧为无语音活动。
                    {
                        LOGFI( "接收一个无语音活动的音频输出帧包成功，但不使用音频输出。音频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                }

                if( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( p_TmpSz == 1 + 4 ) ) //如果是使用UDP协议，且本音频输出帧为无语音活动。
                {
                    //设置音频输出帧应答包。
                    g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_ACK;
                    //设置音频输出帧时间戳。
                    g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( p_TmpUint32 & 0xFF );
                    g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( p_TmpUint32 & 0xFF00 ) >> 8 );
                    g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( p_TmpUint32 & 0xFF0000 ) >> 16 );
                    g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( p_TmpUint32 & 0xFF000000 ) >> 24 );

                    if( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1 + 4, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                    {
                        FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                        LOGFI( "发送一个音频输出帧应答包成功。音频输出帧时间戳：%" PRIu32 "，总长度：%d。", p_TmpUint32, 1 + 4 );
                    }
                    else
                    {
						VarStrFmtIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个音频输出帧应答包失败。原因：" );
						LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
                    }
                }
			}
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_VFRAME ) //如果是视频输出帧包。
            {
                if( p_TmpSz < 1 + 4 ) //如果视频输出帧包的数据长度小于1 + 4，表示没有视频输出帧时间戳。
                {
					LOGFE( "接收一个视频输出帧包的数据长度为%" PRIuPTR "小于1 + 4，表示没有视频输出帧时间戳，无法继续接收。", p_TmpSz );
                    goto out;
                }

				//读取视频输出帧时间戳。
				p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
                if( MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput ) //如果要使用视频输出。
                {
                    //将视频输出帧放入链表或自适应抖动缓冲器。
                    switch( g_MediaInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
                    {
                        case 0: //如果使用链表。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
							{
								if( VarLenLnkLstPutTail( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
								{
									LOGFI( "接收一个有图像活动的视频输出帧包，并放入接收视频输出帧链表成功。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
								}
								else
								{
									LOGFE( "接收一个有图像活动的视频输出帧包，并放入接收视频输出帧链表失败。原因：%s", MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									goto out;
								}
							}
							else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( "接收一个无图像活动的视频输出帧包，无需放入接收视频输出帧链表。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            break;
                        }
                        case 1: //如果使用自适应抖动缓冲器。
                        {
							if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                            {
                                VAjbPutOneFrame( g_MediaInfoPt->m_VAjbPt, g_MediaInfoPt->m_LastPktRecvTime, p_TmpUint32, g_MediaInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4 );
                                LOGFI( "接收一个有图像活动的视频输出帧包，并放入视频自适应抖动缓冲器成功。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }
                            else //如果该视频输出帧为无图像活动。
                            {
                                LOGFI( "接收一个无图像活动的视频输出帧包，无需放入视频自适应抖动缓冲器。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                            }

							int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
							int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
							int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
							int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
							VAjbGetBufFrameCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
							LOGFI( "视频自适应抖动缓冲器：帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );
                            break;
                        }
                    }
                }
                else //如果不使用视频输出。
                {
                    if( p_TmpSz > 1 + 4 ) //如果该视频输出帧为有图像活动。
                    {
                        LOGFI( "接收一个有图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                    else //如果该视频输出帧为无图像活动。
                    {
                        LOGFI( "接收一个无图像活动的视频输出帧包成功，但不使用视频输出。视频输出帧时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );
                    }
                }
            }
			else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_ACK ) //如果是连接应答包或音视频输入输出帧应答包。
            {
                if( p_TmpSz == 1 ) //如果数据包的数据长度等于1，表示是连接应答包，就不管。
                {

                }
                else //如果数据包的数据长度大于1，表示是音视频输入输出帧应答包。
                {
                    if( p_TmpSz != 1 + 4 )
                    {
                        LOGFE( "接收一个音视频输入输出帧应答包的数据长度为%" PRIuPTR "不等于1 + 4，表示格式不正确，无法继续接收。", p_TmpSz );
                        goto out;
                    }

                    //读取时间戳。
                    p_TmpUint32 = ( g_MediaInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );

                    LOGFI( "接收一个音视频输入输出帧应答包。时间戳：%" PRIu32 "，总长度：%" PRIuPTR "。", p_TmpUint32, p_TmpSz );

                    //设置最后一个发送的音频输入帧远端是否接收到。
                    if( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp == p_TmpUint32 ) g_MediaInfoPt->m_LastSendAudioInputFrameIsRecv = 1;
                }
            }
            else if( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) //如果是退出包。
            {
                if( p_TmpSz > 1 ) //如果退出包的数据长度大于1。
                {
                    LOGFE( "接收到退出包的数据长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                g_MediaInfoPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
                MediaProcThreadRequireExit( MediaProcThreadPt, 1, 0, NULL ); //请求退出。

				VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
				LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
		}
		else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包超时。
        {
			
        }
	}
	else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。
	{
		VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
		LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
		goto out;
	}

    //发送心跳包。
	FuncGetTimeAsMsec( &p_TmpUint64 );
    if( p_TmpUint64 - g_MediaInfoPt->m_LastPktSendTime >= 100 ) //如果超过100毫秒没有发送任何数据包，就发送一个心跳包。
    {
		g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_CNCT_HTBT; //设置心跳包。
        if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
            ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
        {
			g_MediaInfoPt->m_LastPktSendTime = p_TmpUint64; //记录最后一个数据包的发送时间。
			LOGI( "发送一个心跳包成功。" );
		}
		else
		{
			VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个心跳包失败。原因：" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
    }

    //判断套接字连接是否中断。
	FuncGetTimeAsMsec( &p_TmpUint64 );
	if( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 2000 ) //如果超过2000毫秒没有接收任何数据包，就判定连接已经断开了。
		{
			VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "超过2000毫秒没有接收任何数据包，判定连接已经断开了。" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
	}
	else if( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) //如果使用UDP协议。
	{
		if( p_TmpUint64 - g_MediaInfoPt->m_LastPktRecvTime > 5000 ) //如果超过5000毫秒没有接收任何数据包，就判定连接已经断开了。
		{
			VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "超过5000毫秒没有接收任何数据包，判定连接已经断开了。" );
			LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
	}

    p_Result = 0; //设置本函数执行成功。

	out:
	return p_Result;
}

//用户定义的销毁函数，在本线程退出时回调一次。
void __cdecl MyMediaProcThreadUserDestroy( MediaProcThread * MediaProcThreadPt )
{
	size_t p_TmpSz;
	
	if( ( MediaProcThreadPt->m_ExitFlag == 1 ) && ( ( g_MediaInfoPt->m_TcpClntSoktPt != NULL ) || ( ( g_MediaInfoPt->m_UdpSoktPt != NULL ) && ( UdpGetRmtAddr( g_MediaInfoPt->m_UdpSoktPt, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本线程接收到退出请求，且本端TCP协议客户端套接字类对象不为空或本端UDP协议套接字类对象不为空且已连接远端。
    {
        //循环发送退出包。
        g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_EXIT; //设置退出包。
		for( int p_SendTimes = ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) ? 1 : 5; p_SendTimes > 0; p_SendTimes-- )
        {
            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) ) )
            {
                VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个退出包失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto SendExitPkt;
            }
        }

        FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //记录最后一个数据包的发送时间。

		VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "发送一个退出包成功。" );
		LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

		//接收退出包。
        if( g_MediaInfoPt->m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
        {
            while( true ) //循环接收退出包。
            {
                if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntRecvPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                    ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpRecvPkt( g_MediaInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, g_MediaInfoPt->m_TmpByteSz, &p_TmpSz, 5000, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
                {
                    if( p_TmpSz != -1 ) //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包成功。
                    {
						FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

                        if( ( p_TmpSz == 1 ) && ( g_MediaInfoPt->m_TmpBytePt[0] == PKT_TYP_EXIT ) ) //如果是退出包。
                        {
                            VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
							LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                            goto SendExitPkt;
                        }
						else //如果是其他包，继续接收。
                        {

                        }
                    }
                    else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包超时。
                    {
                        VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
						LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                        goto SendExitPkt;
                    }
                }
                else //用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。
                {
                    VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto SendExitPkt;
                }
            }
        }
    }
	SendExitPkt:;

    //销毁本端TCP协议服务端套接字。
    if( g_MediaInfoPt->m_TcpSrvrSoktPt != NULL )
    {
        TcpSrvrDestroy( g_MediaInfoPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。
        g_MediaInfoPt->m_TcpSrvrSoktPt = NULL;

		VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议服务端套接字成功。" );
		LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁本端TCP协议客户端套接字。
    if( g_MediaInfoPt->m_TcpClntSoktPt != NULL )
    {
        TcpClntDestroy( g_MediaInfoPt->m_TcpClntSoktPt, ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。
        g_MediaInfoPt->m_TcpClntSoktPt = NULL;

		VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议客户端套接字成功。" );
		LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }
	
    //销毁本端UDP协议套接字。
    if( g_MediaInfoPt->m_UdpSoktPt != NULL )
    {
        UdpDestroy( g_MediaInfoPt->m_UdpSoktPt, NULL ); //关闭并销毁已创建的本端UDP协议套接字。
        g_MediaInfoPt->m_TcpClntSoktPt = NULL;

		VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端UDP协议套接字成功。" );
		LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁接收音频输出帧的链表。
    if( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt != NULL )
    {
		VarLenLnkLstDestroy( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, NULL );
        g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt = NULL;

		LOGI( "销毁接收音频输出帧的链表成功。" );
    }
	
    //销毁接收视频输出帧的链表。
    if( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt != NULL )
    {
		VarLenLnkLstDestroy( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, NULL );
        g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt = NULL;

		LOGI( "销毁接收视频输出帧的链表成功。" );
    }

    //销毁音频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_AAjbPt != NULL )
    {
        AAjbDestroy( g_MediaInfoPt->m_AAjbPt );
        g_MediaInfoPt->m_AAjbPt = NULL;

		LOGI( "销毁音频自适应抖动缓冲器成功。" );
    }
	
    //销毁视频自适应抖动缓冲器。
    if( g_MediaInfoPt->m_VAjbPt != NULL )
    {
        VAjbDestroy( g_MediaInfoPt->m_VAjbPt );
        g_MediaInfoPt->m_VAjbPt = NULL;

		LOGI( "销毁视频自适应抖动缓冲器成功。" );
    }

    if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
    {
        if( ( MediaProcThreadPt->m_ExitFlag == 1 ) && ( g_MediaInfoPt->m_IsRecvExitPkt == 1 ) ) //如果本线程接收到退出请求，且接收到了退出包。
        {
			VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程接收到退出请求，且接收到了退出包，表示是远端TCP协议客户端套接字主动退出，本线程重新初始化来继续保持监听。" );
			LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			MediaProcThreadRequireExit( MediaProcThreadPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_REPAINT, 0, 0 ); //向主界面发送重绘消息。
        }
        else if( ( MediaProcThreadPt->m_ExitFlag == 0 ) && ( MediaProcThreadPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
			VarStrCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来继续保持监听。" );
			LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			MediaProcThreadRequireExit( MediaProcThreadPt, 2, 0, NULL ); //请求重启。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_MYMediaProcThreadEND, 0, 0 ); //向主界面发送媒体处理线程退出的消息。
			PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_REPAINT, 0, 0 ); //向主界面发送重绘消息。
        }
    }
    else if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建客户端。
    {
        if( ( MediaProcThreadPt->m_ExitFlag == 0 ) && ( MediaProcThreadPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
            VarStrFmtCpy( MediaProcThreadPt->m_ErrInfoVarStrPt, "由于是创建客户端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来重连服务端。" );
			LOGI( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
            MediaProcThreadRequireExit( MediaProcThreadPt, 2, 0, NULL ); //请求重启。
			PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_REPAINT, 0, 0 ); //向主界面发送重绘消息。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_MYMediaProcThreadEND, 0, 0 ); //向主界面发送媒体处理线程退出的消息。
			PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_REPAINT, 0, 0 ); //向主界面发送重绘消息。
        }
    }
}

//用户定义的读取音视频输入帧函数，在读取到一个音频输入帧或视频输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
void __cdecl MyMediaProcThreadUserReadAudioVideoInputFrame( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioInputFramePt, int16_t * PcmAudioResultFramePt, int32_t VoiceActSts, uint8_t * EncoderAudioInputFramePt, size_t EncoderAudioInputFrameLen, int32_t EncoderAudioInputFrameIsNeedTrans, uint8_t * YU12VideoInputFramePt, int32_t YU12VideoInputFrameWidth, int32_t YU12VideoInputFrameHeigth, uint8_t * EncoderVideoInputFramePt, size_t EncoderVideoInputFrameLen )
{
    int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	int p_FramePktLen = 0; //存放输入输出帧数据包的数据长度，单位字节。

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
				memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, PcmAudioResultFramePt, MediaProcThreadPt->m_AudioInput.m_FrameLen * 2 ); //设置音频输入输出帧。
                p_FramePktLen = 1 + 4 + 4 + MediaProcThreadPt->m_AudioInput.m_FrameLen * 2; //数据包长度 = 数据包类型 + 音频输入帧时间戳 + 视频输入帧时间戳 + PCM格式音频输入帧。
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

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
				FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
				LOGFI( "发送一个有语音活动的音频输入帧包成功。音频输入帧时间戳：%" PRIu32 "，视频输入帧时间戳：%" PRIu32 "，总长度：%d。", g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, p_FramePktLen );
			}
			else
			{
				VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个有语音活动的音频输入帧包失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
			
			g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 1; //设置最后一个发送的音频输入帧有语音活动。
            g_MediaInfoPt->m_LastSendAudioInputFrameIsRecv = 1; //设置最后一个发送的音频输入帧远端已经接收到。
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

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
                FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                LOGFI( "发送一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%" PRIu32 "，总长度：%d。", g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen );
            }
            else
            {
                VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个无语音活动的音频输入帧包失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
            }

            g_MediaInfoPt->m_LastSendAudioInputFrameIsAct = 0; //设置最后一个发送的音频输入帧无语音活动。
            g_MediaInfoPt->m_LastSendAudioInputFrameIsRecv = 0; //设置最后一个发送的音频输入帧远端没有接收到。
        }
		else //如果本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。
		{
            LOGI( "本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，无需发送。" );

			if( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_MediaInfoPt->m_LastSendAudioInputFrameIsAct == 0 ) && ( g_MediaInfoPt->m_LastSendAudioInputFrameIsRecv == 0 ) ) //如果是使用UDP协议，且本音频输入帧为无语音活动，且最后一个发送的音频输入帧为无语音活动，且最后一个发送的音频输入帧远端没有接收到。
            {
                //设置音频输入帧包。
                g_MediaInfoPt->m_TmpBytePt[0] = PKT_TYP_AFRAME;
                //设置音频输入帧时间戳。
                g_MediaInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF );
				g_MediaInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF00 ) >> 8 );
				g_MediaInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF0000 ) >> 16 );
				g_MediaInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp & 0xFF000000 ) >> 24 );

                if( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, 1 + 4, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                    LOGFI( "重新发送最后一个无语音活动的音频输入帧包成功。音频输入帧时间戳：%" PRIu32 "，总长度：%d。", g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen );
                }
                else
                {
                    VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "重新发送最后一个无语音活动的音频输入帧包失败。原因：" );
					LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
                }
            }
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
			memcpy( g_MediaInfoPt->m_TmpBytePt + 1 + 4 + 4, YU12VideoInputFramePt, YU12VideoInputFrameWidth * YU12VideoInputFrameHeigth * 3 / 2 ); //设置视频输入输出帧。
            p_FramePktLen = 1 + 4 + 4 + YU12VideoInputFrameWidth * YU12VideoInputFrameHeigth * 3 / 2; //数据包长度 = 数据包类型 + 视频输入帧时间戳 + 音频输入帧时间戳 + YU12格式视频输入帧。
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

            if( ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_MediaInfoPt->m_TcpClntSoktPt, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_MediaInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_MediaInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_MediaInfoPt->m_TmpBytePt, p_FramePktLen, 0, MediaProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
                FuncGetTimeAsMsec( &g_MediaInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
				LOGFI( "发送一个有图像活动的视频输入帧包成功。视频输入帧时间戳：%" PRIu32 "，音频输入帧时间戳：%" PRIu32 "，总长度：%d，类型：%d。", g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
            }
            else
            {
				LOGFE( "视频输入帧时间戳：%" PRIu32 "，音频输入帧时间戳：%" PRIu32 "，总长度：%d，类型：%d。", g_MediaInfoPt->m_LastSendVideoInputFrameTimeStamp, g_MediaInfoPt->m_LastSendAudioInputFrameTimeStamp, p_FramePktLen, g_MediaInfoPt->m_TmpBytePt[13] & 0xff );
				VarStrIns( MediaProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个有图像活动的视频输入帧包失败。原因：" );
				LOGE( MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, MediaProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MediaInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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
void __cdecl MyMediaProcThreadUserWriteAudioOutputFrame( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioOutputFramePt, uint8_t * EncoderAudioOutputFramePt, size_t * AudioOutputFrameLenPt  )
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
				VarLenLnkLstGetTotal( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, &m_TmpSz, NULL );
				if( m_TmpSz != 0 )
				{
					VarLenLnkLstGetHead( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, NULL, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, NULL, &m_TmpSz, MediaProcThreadPt->m_ErrInfoVarStrPt ); //获取接收音频输出帧链表的第一个音频输出帧。
					VarLenLnkLstDelHead( g_MediaInfoPt->m_RecvAudioOutputFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ); //删除接收音频输出帧链表的第一个音频输出帧。
				}

				if( m_TmpSz != 0 ) //如果接收音频输出帧链表的第一个输出帧为有语音活动。
				{
					LOGFI( "从接收音频输出帧链表取出一个有语音活动的音频输出帧，帧的数据长度：%" PRIuPTR "。", m_TmpSz );
				}
				else //如果接收音频输出帧链表为空，或第一个音频输出帧为无语音活动。
				{
					LOGFI( "从接收音频输出帧链表取出一个无语音活动的音频输出帧，帧的数据长度：%" PRIuPTR "。", m_TmpSz );
				}

                break;
			}
			case 1: //如果使用自适应抖动缓冲器。
			{
				uint32_t p_AudioOutputFrameTimeStamp; //存放音频输出帧的时间戳。

				//从音频自适应抖动缓冲器取出一个音频输出帧。
				AAjbGetOneFrame( g_MediaInfoPt->m_AAjbPt, &p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte2Pt, g_MediaInfoPt->m_TmpByte2Sz, &m_TmpSz );
				
				if( ( m_TmpSz > 0 ) && ( m_TmpSz != SIZE_MAX ) ) //如果音频输出帧为有语音活动。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp = ( g_MediaInfoPt->m_TmpByte2Pt[0] & 0xFF ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[1] & 0xFF ) << 8 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[2] & 0xFF ) << 16 ) + ( ( g_MediaInfoPt->m_TmpByte2Pt[3] & 0xFF ) << 24 ); //设置最后一个取出的音频输出帧对应视频输出帧的时间戳。
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个有语音活动的音频输出帧。音频输出帧时间戳：%" PRIu32 "，视频输出帧时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, m_TmpSz );
				}
				else if( m_TmpSz == 0 ) //如果音频输出帧为无语音活动。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 0; //设置最后一个取出的音频输出帧为无语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个无语音活动的音频输出帧。音频输出帧时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, m_TmpSz );
				}
				else //如果音频输出帧为丢失。
				{
					g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct = 1; //设置最后一个取出的音频输出帧为有语音活动。
					LOGFI( "从音频自适应抖动缓冲器取出一个丢失的音频输出帧。音频输出帧时间戳：%" PRIu32 "，视频输出帧时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_AudioOutputFrameTimeStamp, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, m_TmpSz );
				}

				int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
				int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
				int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
				int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
				int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
				int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
				AAjbGetBufFrameCnt( g_MediaInfoPt->m_AAjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
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
                    LOGFE( "音频输出帧的数据长度不等于PCM格式的数据长度。音频输出帧：%" PRIuPTR "，PCM格式：%" PRIi32 "。", m_TmpSz - 4, *AudioOutputFrameLenPt );
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
                    LOGFE( "音频输出帧的数据长度已超过已编码格式的数据长度。音频输出帧：%" PRIuPTR "，已编码格式：%" PRIuPTR "。", m_TmpSz - 4, *AudioOutputFrameLenPt );
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
void __cdecl MyMediaProcThreadUserGetPcmAudioOutputFrame( MediaProcThread * MediaProcThreadPt, int16_t * PcmAudioOutputFramePt, size_t PcmAudioOutputFrameLen )
{
	
}

//用户定义的写入视频输出帧函数，在可以显示一个视频输出帧时回调一次。注意：本函数不是在媒体处理线程中执行的，而是在视频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音视频输出帧不同步。
void __cdecl MyMediaProcThreadUserWriteVideoOutputFrame( MediaProcThread * MediaProcThreadPt, uint8_t * YU12VideoOutputFramePt, uint8_t * EncoderVideoOutputFramePt, size_t * EncoderVideoOutputFrameLenPt )
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
			VarLenLnkLstGetTotal( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, &m_TmpSz, NULL );
            if( m_TmpSz != 0 ) //如果接收视频输出帧链表不为空。
            {
				VarLenLnkLstGetHead( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, NULL, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, NULL, &m_TmpSz, MediaProcThreadPt->m_ErrInfoVarStrPt ); //获取接收视频输出帧链表的第一个视频输出帧。
				VarLenLnkLstDelHead( g_MediaInfoPt->m_RecvVideoOutputFrameLnkLstPt, MediaProcThreadPt->m_ErrInfoVarStrPt ); //删除接收视频输出帧链表的第一个视频输出帧。
				
				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( "从接收视频输出帧链表取出一个有图像活动的视频输出帧。数据长度：%" PRIuPTR "。", m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( "从接收视频输出帧链表取出一个无图像活动的视频输出帧。数据长度：%" PRIuPTR "。", m_TmpSz );
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
			VAjbGetBufFrameCnt( g_MediaInfoPt->m_VAjbPt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
			
			if( p_CurHaveBufFrameCnt != 0 ) //如果视频自适应抖动缓冲器不为空。
			{
				LOGFI( "视频自适应抖动缓冲器：帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );

				//从视频自适应抖动缓冲器取出一个视频输出帧。
				FuncGetTimeAsMsec( &p_NowTimeMesc );
				if( MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaInfoPt->m_LastGetAudioOutputFrameIsAct != 0 ) //如果要使用音频输出，且最后一个取出的音频输出帧为有语音活动，就根据最后一个取出的音频输出帧对应视频输出帧的时间戳来取出。
				{
					VAjbGetOneFrameWantTimeStamp( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, g_MediaInfoPt->m_LastGetAudioOutputFrameVideoOutputFrameTimeStamp, &p_VideoOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz );
				}
				else //如果最后一个取出的音频输出帧为无语音活动，就根据直接取出。
				{
					VAjbGetOneFrame( g_MediaInfoPt->m_VAjbPt, p_NowTimeMesc, &p_VideoOutputFrameTimeStamp, g_MediaInfoPt->m_TmpByte3Pt, g_MediaInfoPt->m_TmpByte3Sz, &m_TmpSz );
				}

				if( m_TmpSz != 0 ) //如果视频输出帧为有图像活动。
				{
					LOGFI( "从视频自适应抖动缓冲器取出一个有图像活动的视频输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_VideoOutputFrameTimeStamp, m_TmpSz );
				}
				else //如果视频输出帧为无图像活动。
				{
					LOGFI( "从视频自适应抖动缓冲器取出一个无图像活动的视频输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_VideoOutputFrameTimeStamp, m_TmpSz );
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
            if( m_TmpSz - 4 != *EncoderVideoOutputFrameLenPt )
            {
                memset( YU12VideoOutputFramePt, 0, *EncoderVideoOutputFrameLenPt );
                LOGFE( "视频输出帧的数据长度不等于YU12格式的数据长度。视频输出帧：%" PRIuPTR "，YU12格式：%" PRIi32"。", m_TmpSz - 4, *EncoderVideoOutputFrameLenPt );
                return;
            }

            //写入YU12格式视频输出帧。
			memcpy( YU12VideoOutputFramePt, g_MediaInfoPt->m_TmpByte3Pt + 4, m_TmpSz - 4 );
        }
        else //如果要使用已编码格式视频输出帧。
        {
            if( m_TmpSz - 4 > *EncoderVideoOutputFrameLenPt )
            {
                *EncoderVideoOutputFrameLenPt = 0;
				LOGFE( "视频输出帧的数据长度已超过已编码格式的数据长度。视频输出帧：%" PRIuPTR "，已编码格式：%" PRIi32"。", m_TmpSz - 4, *EncoderVideoOutputFrameLenPt );
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
void __cdecl MyMediaProcThreadUserGetYU12VideoOutputFrame( MediaProcThread * MediaProcThreadPt, uint8_t * YU12VideoOutputFramePt, int32_t YU12VideoOutputFrameWidth, int32_t YU12VideoOutputFrameHeigth )
{
	
}

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR    lpCmdLine,
					   _In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	
	// TODO: 在此处放置代码。
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 748 );

	size_t LogNum;

	hInst = hInstance; //设置当前实例的句柄。
	
	FuncSetCurActDir( NULL, NULL ); //设置当前进程的当前活动目录的路径。

	VarStrInit( &g_ErrInfoVarStrPt );

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
			( memcmp( p_AppFileNamePt, "Windows_audio_talkback_demo_program", strlen( "Windows_audio_talkback_demo_program" ) ) != 0 ) )
		{
			if( MessageBox( NULL, "应用程序可执行文件的名称不为“Windows下音视频对讲演示程序.exe”，运行会报错，是否继续？", "警告", MB_YESNO | MB_ICONWARNING ) == IDNO )
			{
				return 0;
			}
		}
	}

	//创建窗口。
	g_MainWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_Main ), NULL, DialogProc );
	g_SettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_Setting ), g_MainWndHdl, DialogProc );
	g_AjbSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_AjbSetting ), g_SettingWndHdl, DialogProc );
	g_SpeexAecSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_SpeexAecSetting ), g_SettingWndHdl, DialogProc );
	g_WebRtcAecmSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_WebRtcAecmSetting ), g_SettingWndHdl, DialogProc );
	g_WebRtcAecSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_WebRtcAecSetting ), g_SettingWndHdl, DialogProc );
	g_SpeexWebRtcAecSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_SpeexWebRtcAecSetting ), g_SettingWndHdl, DialogProc );
	g_SpeexPprocNsSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_SpeexPprocNsSetting ), g_SettingWndHdl, DialogProc );
	g_WebRtcNsxSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_WebRtcNsxSetting ), g_SettingWndHdl, DialogProc );
	g_WebRtcNsSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_WebRtcNsSetting ), g_SettingWndHdl, DialogProc );
	g_SpeexPprocOtherSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_SpeexPprocOtherSetting ), g_SettingWndHdl, DialogProc );
	g_SpeexCodecSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_SpeexCodecSetting ), g_SettingWndHdl, DialogProc );
	g_OpenH264CodecSettingWndHdl = CreateDialog( hInst, MAKEINTRESOURCE( IDD_OpenH264Setting ), g_SettingWndHdl, DialogProc );

	//设置窗口图标。
	#if( ( defined __X86__ ) )
	SetClassLong( g_MainWndHdl, GCL_HICONSM, ( LONG )LoadIcon( hInst, MAKEINTRESOURCE( IDI_WINDOWSAUDIOTALKBACKDEMOPROGRAM ) ) );
	#elif( ( defined __X64__ ) )
	SetClassLong( g_MainWndHdl, GCLP_HICONSM, ( LONG )LoadIcon( hInst, MAKEINTRESOURCE( IDI_WINDOWSAUDIOTALKBACKDEMOPROGRAM ) ) );
	#endif

	//默认使用UDP协议。
	CheckRadioButton( g_MainWndHdl, IDC_UseTcpPrtcl, IDC_UseUdpPrtcl, IDC_UseUdpPrtcl );

	//设置IP地址控件的内容。
	{
		struct addrinfo p_LclNodeHintsAddrInfo; //存放本地节点条件地址信息结构体。
		struct addrinfo * p_LclNodeAddrInfoListPt = NULL; //存放本地节点地址信息结构体链表的内存指针。
		struct addrinfo * p_LclNodeAddrInfoListLoopPt = NULL; //存放本地节点地址信息结构体链表的内存指针。

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
				SendMessage( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), CB_ADDSTRING, 0, ( LPARAM )g_ErrInfoVarStrPt->m_StrPt );
			}
			freeaddrinfo( p_LclNodeAddrInfoListPt ); //销毁本地节点地址信息结构体链表。
		}
		SendMessage( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), CB_ADDSTRING, 0, ( LPARAM )"0.0.0.0" );
		SendMessage( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), CB_ADDSTRING, 0, ( LPARAM )"127.0.0.1" );
		SendMessage( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), CB_SETCURSEL, 0, 0 ); //设置IP地址控件默认选择第一个IP地址。
	}

	//设置端口控件的内容。
	SetWindowText( GetDlgItem( g_MainWndHdl, IDC_Port ), "12345" );

	//默认使用音频对讲。
	CheckRadioButton( g_MainWndHdl, IDC_UseAudioTalkback, IDC_UseAudioVideoTalkback, IDC_UseAudioTalkback );

	//初始化音频输入设备和音频输出设备的控件。
	PostMessage( g_MainWndHdl, WM_DEVICECHANGE, 0, 0 );

	//获取视频输入预览窗口和视频输出显示窗口的句柄和位置。
	g_VideoInputPreviewWndHdl = GetDlgItem( g_MainWndHdl, IDC_VideoInputPreview );
	GetWindowRect( g_VideoInputPreviewWndHdl, &g_VideoInputPreviewWndRect );
	ScreenToClient( g_MainWndHdl, ( LPPOINT )&g_VideoInputPreviewWndRect );
	ScreenToClient( g_MainWndHdl, ( LPPOINT )&g_VideoInputPreviewWndRect.right );
	g_VideoOutputDisplayWndHdl = GetDlgItem( g_MainWndHdl, IDC_VideoOutputDisplay );
	GetWindowRect( g_VideoOutputDisplayWndHdl, &g_VideoOutputDisplayWndRect );
	ScreenToClient( g_MainWndHdl, ( LPPOINT )&g_VideoOutputDisplayWndRect );
	ScreenToClient( g_MainWndHdl, ( LPPOINT )&g_VideoOutputDisplayWndRect.right );
	
	//打印可执行文件所在目录完整绝对路径到日志。
	if( g_ErrInfoVarStrPt->m_StrSz < MAX_PATH ) VarStrSetSz( g_ErrInfoVarStrPt, MAX_PATH );
	GetCurrentDirectory( g_ErrInfoVarStrPt->m_StrSz, g_ErrInfoVarStrPt->m_StrPt );
	VarStrIns( g_ErrInfoVarStrPt, 0, "可执行文件所在目录完整绝对路径：" );
	LOGI( g_ErrInfoVarStrPt->m_StrPt );
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

	//显示主界面窗口。
	ShowWindow( g_MainWndHdl, SW_SHOW );

	//默认设置。
	{
		CheckRadioButton( g_SettingWndHdl, IDC_UseLnkLstRecvOutputFrame, IDC_UseAjbRecvOutputFrame, IDC_UseAjbRecvOutputFrame );
		CheckDlgButton( g_SettingWndHdl, IDC_IsSaveSettingToFile, BST_CHECKED );
		CheckDlgButton( g_SettingWndHdl, IDC_IsPrintLogShowToast, BST_CHECKED );
		CheckDlgButton( g_SettingWndHdl, IDC_IsSaveAudioToFile, BST_CHECKED );
		SendMessage( g_MainWndHdl, WM_COMMAND, IDC_UseEffectSuper, 0 );
		SendMessage( g_MainWndHdl, WM_COMMAND, IDC_UseBitrateSuper, 0 );
		
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbMinNeedBufFrameCnt ), "5" );
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbMaxNeedBufFrameCnt ), "50" );
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbAdaptSensitivity ), "1.0" );
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbMinNeedBufFrameCnt ), "1" );
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbMaxNeedBufFrameCnt ), "24" );
		SetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbAdaptSensitivity ), "1.0" );
	}

    MSG p_Msg;

    // 主消息循环:
	while( GetMessage( &p_Msg, NULL, 0, 0 ) )
	{
		//if( IsDialogMessage( p_Msg.hwnd, &p_Msg ) == 0 )
		if( TranslateAccelerator( p_Msg.hwnd, NULL, &p_Msg ) == 0 )
		{
			TranslateMessage( &p_Msg );
			DispatchMessage( &p_Msg );
		}
	}

	LogDestroy(); //销毁日志。
	VarStrDestroy( g_ErrInfoVarStrPt ); //销毁错误信息动态字符串。
	
	_CrtDumpMemoryLeaks();
	return (int) p_Msg.wParam;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK DialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
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
			HWND p_AudioInputDeviceComboBoxWndHdl = NULL;
			HWND p_AudioOutputDeviceComboBoxWndHdl = NULL;
			HWND p_VideoInputDeviceComboBoxWndHdl = NULL;
			VarStr * * p_AudioInputDeviceNameArrPt = NULL;
			UINT p_AudioInputDeviceTotal = 0;
			VarStr * * p_AudioOutputDeviceNameArrPt = NULL;
			UINT p_AudioOutputDeviceTotal = 0;
			VarStr * * p_VideoInputDeviceNameArrPt = NULL;
			UINT p_VideoInputDeviceTotal = 0;
			VarStr * p_CurAudioInputDeviceName = NULL;
			VarStr * p_CurAudioOutputDeviceName = NULL;
			VarStr * p_CurVideoInputDeviceName = NULL;
			int p_TmpInt;

			//获取音频输入设备、音频输出设备、视频输入设备的名称。
			if( MediaProcThreadGetAudioInputDeviceName( &p_AudioInputDeviceNameArrPt, &p_AudioInputDeviceTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取音频输入设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto outdevicechange;
			}
			if( MediaProcThreadGetAudioOutputDeviceName( &p_AudioOutputDeviceNameArrPt, &p_AudioOutputDeviceTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取音频输出设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto outdevicechange;
			}
			if( MediaProcThreadGetVideoInputDeviceName( &p_VideoInputDeviceNameArrPt, &p_VideoInputDeviceTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取视频输入设备名称失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInitByStr( &p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto outdevicechange;
			}
			
			//获取音频输入设备、音频输出设备、视频输入设备的控件句柄。
			p_AudioInputDeviceComboBoxWndHdl = GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice );
			p_AudioOutputDeviceComboBoxWndHdl = GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice );
			p_VideoInputDeviceComboBoxWndHdl = GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice );

			//获取当前选择的音频输入设备、音频输出设备、视频输入设备。
			p_TmpInt = SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurAudioInputDeviceName, SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioInputDeviceName->m_StrPt );
			}
			p_TmpInt = SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurAudioOutputDeviceName, SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioOutputDeviceName->m_StrPt );
			}
			p_TmpInt = SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				VarStrInit( &p_CurVideoInputDeviceName, SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 );
				SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurVideoInputDeviceName->m_StrPt );
			}

			//清空音频输入设备、音频输出设备、视频输入设备的控件。
			SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );

			//添加音频输入设备、音频输出设备、视频输入设备到控件。
			if( p_AudioInputDeviceTotal > 0 )
			{
				p_TmpInt = SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AudioInputDeviceTotal; p_TmpInt++ )
					SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AudioInputDeviceNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"无" );
			}
			if( p_AudioOutputDeviceTotal > 0 )
			{
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_AudioOutputDeviceTotal; p_TmpInt++ )
					SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_AudioOutputDeviceNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )"无" );
			}
			if( p_VideoInputDeviceTotal > 0 )
			{
				SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );
				for( p_TmpInt = 0; p_TmpInt < p_VideoInputDeviceTotal; p_TmpInt++ )
					SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_VideoInputDeviceNameArrPt[p_TmpInt]->m_StrPt );
			}
			else
			{
				SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )"无" );
			}
			
			//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
			if( ( p_CurAudioInputDeviceName == NULL ) || ( SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioInputDeviceName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			if( ( p_CurAudioOutputDeviceName == NULL ) || ( SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioOutputDeviceName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			if( ( p_CurVideoInputDeviceName == NULL ) || ( SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVideoInputDeviceName->m_StrPt ) == CB_ERR ) )
			{
				if( SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_VideoInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			
			p_Result = 0; // //设置本函数执行成功。

			outdevicechange:
			MediaProcThreadDestroyDeviceName( p_AudioInputDeviceNameArrPt, p_AudioInputDeviceTotal, NULL );
			MediaProcThreadDestroyDeviceName( p_AudioOutputDeviceNameArrPt, p_AudioOutputDeviceTotal, NULL );
			MediaProcThreadDestroyDeviceName( p_VideoInputDeviceNameArrPt, p_VideoInputDeviceTotal, NULL );
			VarStrDestroy( p_CurAudioInputDeviceName );
			VarStrDestroy( p_CurAudioOutputDeviceName );
			VarStrDestroy( p_CurVideoInputDeviceName );
			return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
		}
		case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
				case IDC_UseAudioTalkback: //音频对讲控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						g_MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput = 1;
						g_MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput = 1;
						g_MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput = 0;
						g_MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput = 0;

						if( g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为音频对讲，请求重启媒体处理线程。" );
							MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case IDC_UseVideoTalkback: //视频对讲控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						g_MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput = 0;
						g_MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput = 0;
						g_MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput = 1;
						g_MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput = 1;

						if( g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为视频对讲，请求重启媒体处理线程。" );
							MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case IDC_UseAudioVideoTalkback: //音视频对讲控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						g_MediaProcThreadPt->m_AudioInput.m_IsUseAudioInput = 1;
						g_MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput = 1;
						g_MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput = 1;
						g_MediaProcThreadPt->m_VideoOutput.m_IsUseVideoOutput = 1;

						if( g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果媒体处理线程已经初始化完毕。
						{
							LOGI( "对讲模式已改变为音视频对讲，请求重启媒体处理线程。" );
							MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
						}
					}
					break;
				}
				case IDC_UseAudioInputDevice: //音频输入设备控件。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaProcThreadPt != NULL )
							{
								//设置音频输入使用的设备。
								MediaProcThreadSetAudioInputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( "音频输入使用的设备已改变，请求重启媒体处理线程。" );
									MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case IDC_UseAudioOutputDevice: //音频输出设备控件。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaProcThreadPt != NULL )
							{
								//设置音频输出使用的设备。
								MediaProcThreadSetAudioOutputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaProcThreadPt->m_AudioOutput.m_IsUseAudioOutput != 0 && g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用音频输出，且媒体处理线程已经初始化完毕。
								{
									LOGI( "音频输出使用的设备已改变，请求重启媒体处理线程。" );
									MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case IDC_UseVideoInputDevice: //视频输入设备控件。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //控件选择的项目改变消息。
						{
							if( g_MediaProcThreadPt != NULL )
							{
								//设置视频输入使用的设备。
								MediaProcThreadSetVideoInputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

								if( g_MediaProcThreadPt->m_VideoInput.m_IsUseVideoInput != 0 && g_MediaProcThreadPt->m_RunFlag > RUN_FLAG_INIT ) //如果要使用视频输入，且媒体处理线程已经初始化完毕。
								{
									LOGI( "视频输入使用的设备已改变，请求重启媒体处理线程。" );
									MediaProcThreadRequireExit( g_MediaProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case IDC_AudioInputIsMute: //音频输入静音控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						MediaProcThreadSetAudioInputIsMute( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AudioInputIsMute ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_AudioOutputIsMute: //音频输出静音控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						MediaProcThreadSetAudioOutputIsMute( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AudioOutputIsMute ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_VideoInputIsBlack: //视频输入黑屏控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						MediaProcThreadSetVideoInputIsBlack( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_VideoInputIsBlack ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_VideoOutputIsBlack: //视频输出黑屏控件。
				{
					if( g_MediaProcThreadPt != NULL )
					{
						MediaProcThreadSetVideoOutputIsBlack( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_VideoOutputIsBlack ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_CreateSrvr: //创建服务器控件。
				case IDC_CnctSrvr: //连接服务器控件。
				{
					if( g_MediaProcThreadPt == NULL ) //如果媒体处理线程还没有启动。
					{
						LOGI( "开始启动媒体处理线程。" );

						//创建媒体信息的内存块。
						g_MediaInfoPt = ( MediaInfo * )calloc( 1, sizeof( MediaInfo ) );
						if( g_MediaInfoPt == NULL )
						{
							LOGE( "创建媒体信息的内存块失败。" );
							goto outcreate;
						}

						if( VarStrInit( &g_MediaInfoPt->m_IPAddrVarStrPt ) != 0 )
						{
							LOGE( "初始化IP地址动态字符串失败。" );
							goto outcreate;
						}
						
						if( VarStrInit( &g_MediaInfoPt->m_PortVarStrPt ) != 0 )
						{
							LOGE( "初始化端口动态字符串失败。" );
							goto outcreate;
						}

						if( LOWORD( wParam ) == IDC_CreateSrvr )
						{
							g_MediaInfoPt->m_IsCreateSrvrOrClnt = 1; //标记创建服务端接受客户端。
						}
						else
						{
							g_MediaInfoPt->m_IsCreateSrvrOrClnt = 0; //标记创建客户端连接服务端。
						}

						g_MediaInfoPt->m_MainWindowHdl = hDlg; //设置主窗口的句柄。

						//设置IP地址字符串、端口。
						HWND p_TmpWndHdl;
						p_TmpWndHdl = GetDlgItem( hDlg, IDC_IPAddr );
						VarStrSetSz( g_MediaInfoPt->m_IPAddrVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
						GetDlgItemText( hDlg, IDC_IPAddr, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrPt, g_MediaInfoPt->m_IPAddrVarStrPt->m_StrSz );
						p_TmpWndHdl = GetDlgItem( hDlg, IDC_Port );
						VarStrSetSz( g_MediaInfoPt->m_PortVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
						GetDlgItemText( hDlg, IDC_Port, g_MediaInfoPt->m_PortVarStrPt->m_StrPt, g_MediaInfoPt->m_PortVarStrPt->m_StrSz );
						
						//初始化媒体处理线程的内存指针。
						if( MediaProcThreadInit( &g_MediaProcThreadPt, g_MediaInfoPt,
												 MyMediaProcThreadUserInit, MyMediaProcThreadUserProcess, MyMediaProcThreadUserDestroy,
												 MyMediaProcThreadUserReadAudioVideoInputFrame,
												 MyMediaProcThreadUserWriteAudioOutputFrame, MyMediaProcThreadUserGetPcmAudioOutputFrame,
												 MyMediaProcThreadUserWriteVideoOutputFrame, MyMediaProcThreadUserGetYU12VideoOutputFrame,
												 g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "初始化媒体处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto outcreate;
						}

						//判断是否使用什么传输协议。
						if( IsDlgButtonChecked( g_MainWndHdl, IDC_UseTcpPrtcl ) == BST_CHECKED )
						{
							g_MediaInfoPt->m_UseWhatXfrPrtcl = 0;
						}
						else
						{
							g_MediaInfoPt->m_UseWhatXfrPrtcl = 1;
						}

						//判断是否保存设置到文件。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_IsSaveSettingToFile ) == BST_CHECKED )
						{
							MediaProcThreadSetIsSaveSettingToFile( g_MediaProcThreadPt, 1, ".\\Setting.txt", g_ErrInfoVarStrPt );
						}
						else
						{
							MediaProcThreadSetIsSaveSettingToFile( g_MediaProcThreadPt, 0, NULL, g_ErrInfoVarStrPt );
						}
						
						//判断是否打印Log日志，并显示Toast。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_IsPrintLogShowToast ) == BST_CHECKED )
						{
							MediaProcThreadSetIsPrintLogShowToast( g_MediaProcThreadPt, 1, 1, g_ErrInfoVarStrPt );
						}
						else
						{
							MediaProcThreadSetIsPrintLogShowToast( g_MediaProcThreadPt, 0, 0, g_ErrInfoVarStrPt );
						}

						//判断是否使用音频输入。
						MediaProcThreadSetIsUseAudioInput( g_MediaProcThreadPt,
														   ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioTalkback ) == BST_CHECKED ) ? 1 :
														     ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioVideoTalkback ) == BST_CHECKED ) ? 1 : 0,
														   ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate8000 ) == BST_CHECKED ) ? 8000 :
														     ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate16000 ) == BST_CHECKED ) ? 16000 :
														       ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate32000 ) == BST_CHECKED ) ? 32000 : 0,
														   ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen10ms ) == BST_CHECKED ) ? 10 :
														     ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen20ms ) == BST_CHECKED ) ? 20 :
														       ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen30ms ) == BST_CHECKED ) ? 30 : 0,
														   g_ErrInfoVarStrPt );

						//判断是否不使用声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseNoAec ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUseNoAec( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}

						//判断是否使用Speex声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseSpeexAec ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_FilterLen;
							int32_t p_IsUseRec;
							float p_EchoMultiple;
							float p_EchoCont;
							int32_t p_EchoSupes;
							int32_t p_EchoSupesAct;
							int p_IsSaveMemFile;
							
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_FilterLen = atoi( p_TmpStrPt );
							p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupesAct = atoi( p_TmpStrPt );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile ) == BST_CHECKED );

							MediaProcThreadSetAudioInputUseSpeexAec( g_MediaProcThreadPt, p_FilterLen, p_IsUseRec, p_EchoMultiple, p_EchoCont, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, ".\\SpeexAecMem", g_ErrInfoVarStrPt );
						}

						//判断是否使用WebRtc定点版声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseWebRtcAecm ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseCNGMode;
							int32_t p_EchoMode;
							int32_t p_Delay;
							
							p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							
							MediaProcThreadSetAudioInputUseWebRtcAecm( g_MediaProcThreadPt, p_IsUseCNGMode, p_EchoMode, p_Delay, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用WebRtc浮点版声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseWebRtcAec ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EchoMode;
							int32_t p_Delay;
							int32_t p_IsUseDelayAgnosticMode;
							int32_t p_IsUseExtdFilterMode;
							int32_t p_IsUseRefinedFilterAdaptAecMode;
							int32_t p_IsUseAdaptAdjDelay;
							int p_IsSaveMemFile;

							GetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							p_IsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode ) == BST_CHECKED );
							p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode ) == BST_CHECKED );
							p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode ) == BST_CHECKED );
							p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay ) == BST_CHECKED );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile ) == BST_CHECKED );

							MediaProcThreadSetAudioInputUseWebRtcAec( g_MediaProcThreadPt, p_EchoMode, p_Delay, p_IsUseDelayAgnosticMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, ".\\WebRtcAecMem", g_ErrInfoVarStrPt );
						}

						//判断是否使用SpeexWebRtc三重声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseSpeexWebRtcAec ) == BST_CHECKED )
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

							p_WorkMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm ) == BST_CHECKED ) ? 1 :
											( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec ) == BST_CHECKED ) ? 2 :
												( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec ) == BST_CHECKED ) ? 3 : 0;
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecFilterLen = atoi( p_TmpStrPt );
							p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupesAct = atoi( p_TmpStrPt );
							p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmDelay = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecDelay = atoi( p_TmpStrPt );
							p_WebRtcAecIsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode ) == BST_CHECKED );
							p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode ) == BST_CHECKED );
							p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode ) == BST_CHECKED );
							p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay ) == BST_CHECKED );
							p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SameRoomEchoMinDelay = atoi( p_TmpStrPt );

							MediaProcThreadSetAudioInputUseSpeexWebRtcAec( g_MediaProcThreadPt, p_WorkMode, p_SpeexAecFilterLen, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCont, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgnosticMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, g_ErrInfoVarStrPt );
						}
						
						//判断是否不使用噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseNoNs ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUseNoNs( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}

						//判断是否使用Speex预处理器的噪音抑制。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseSpeexPprocNs ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseNs;
							int32_t p_NoiseSupes;
							int32_t p_IsUseDereverb;

							p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_NoiseSupes = atoi( p_TmpStrPt );
							p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb ) == BST_CHECKED );

							MediaProcThreadSetAudioInputUseSpeexPprocNs( g_MediaProcThreadPt, p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, g_ErrInfoVarStrPt );
						}

						//判断是否使用WebRtc定点版噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseWebRtcNsx ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							MediaProcThreadSetAudioInputUseWebRtcNsx( g_MediaProcThreadPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用WebRtc浮点版噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseWebRtcNs ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							MediaProcThreadSetAudioInputUseWebRtcNs( g_MediaProcThreadPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用RNNoise噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseRNNoise ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUseRNNoise( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用Speex预处理器的其他功能。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_IsUseSpeexPprocOther ) == BST_CHECKED )
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

							p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbStart = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbCont = atoi( p_TmpStrPt );
							p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcLevel = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcIncrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcDecrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcMaxGain = atoi( p_TmpStrPt );

							MediaProcThreadSetAudioInputIsUseSpeexPprocOther( g_MediaProcThreadPt, 1, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, g_ErrInfoVarStrPt );
						}
						else
						{
							MediaProcThreadSetAudioInputIsUseSpeexPprocOther( g_MediaProcThreadPt, 0, 0, 0, 0, 0, 0, 0, 0, 0, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UsePcm ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUsePcm( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用Speex编码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseSpeexCodec ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EncoderUseCbrOrVbr;
							int32_t p_EncoderQuality;
							int32_t p_EncoderComplexity;
							int32_t p_EncoderPlcExpectedLossRate;

							p_EncoderUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr ) == BST_CHECKED ) ? 0 : 1;
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderQuality = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderComplexity = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderPlcExpectedLossRate = atoi( p_TmpStrPt );
							
							MediaProcThreadSetAudioInputUseSpeexEncoder( g_MediaProcThreadPt, p_EncoderUseCbrOrVbr, p_EncoderQuality, p_EncoderComplexity, p_EncoderPlcExpectedLossRate, g_ErrInfoVarStrPt );
						}

						//判断是否使用Opus编码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseOpusCodec ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUseOpusEncoder( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断音频输入是否保存音频到文件。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_IsSaveAudioToFile ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputIsSaveAudioToFile( g_MediaProcThreadPt, 1, ".\\AudioInput.wav", ".\\AudioResult.wav", g_ErrInfoVarStrPt );
						}
						else
						{
							MediaProcThreadSetAudioInputIsSaveAudioToFile( g_MediaProcThreadPt, 0, NULL, NULL, g_ErrInfoVarStrPt );
						}
						
						//判断使用的音频输入设备。
						MediaProcThreadSetAudioInputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );

						//判断音频输入是否静音。
						MediaProcThreadSetAudioInputIsMute( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AudioInputIsMute ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );

						//判断是否使用音频输出。
						MediaProcThreadSetIsUseAudioOutput( g_MediaProcThreadPt,
															( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioTalkback ) == BST_CHECKED ) ? 1 :
														     ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioVideoTalkback ) == BST_CHECKED ) ? 1 : 0,
														    ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate8000 ) == BST_CHECKED ) ? 8000 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate16000 ) == BST_CHECKED ) ? 16000 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioSamplingRate32000 ) == BST_CHECKED ) ? 32000 : 0,
														    ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen10ms ) == BST_CHECKED ) ? 10 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen20ms ) == BST_CHECKED ) ? 20 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAudioFrameLen30ms ) == BST_CHECKED ) ? 30 : 0,
														    g_ErrInfoVarStrPt );
						
						//判断是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UsePcm ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioInputUsePcm( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用Speex解码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseSpeexCodec ) == BST_CHECKED )
						{
							int32_t p_DecoderIsUsePerceptualEnhancement;

							p_DecoderIsUsePerceptualEnhancement = ( IsDlgButtonChecked( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement ) == BST_CHECKED );

							MediaProcThreadSetAudioOutputUseSpeexDecoder( g_MediaProcThreadPt, p_DecoderIsUsePerceptualEnhancement, g_ErrInfoVarStrPt );
						}

						//判断是否使用Opus解码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseOpusCodec ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioOutputUseOpusDecoder( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断音频输出是否保存音频到文件。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_IsSaveAudioToFile ) == BST_CHECKED )
						{
							MediaProcThreadSetAudioOutputIsSaveAudioToFile( g_MediaProcThreadPt, 1, ".\\AudioOutput.wav", g_ErrInfoVarStrPt );
						}
						else
						{
							MediaProcThreadSetAudioOutputIsSaveAudioToFile( g_MediaProcThreadPt, 0, NULL, g_ErrInfoVarStrPt );
						}
						
						//判断使用的音频输出设备。
						MediaProcThreadSetAudioOutputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );
						
						//判断音频输出是否静音。
						MediaProcThreadSetAudioOutputIsMute( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AudioOutputIsMute ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
						
						//判断是否使用视频输入。
						MediaProcThreadSetIsUseVideoInput( g_MediaProcThreadPt,
														   ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseVideoTalkback ) == BST_CHECKED ) ? 1 :
														     ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioVideoTalkback ) == BST_CHECKED ) ? 1 : 0,
														   ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoSamplingRate12 ) == BST_CHECKED ) ? 12 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoSamplingRate15 ) == BST_CHECKED ) ? 15 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoSamplingRate24 ) == BST_CHECKED ) ? 24 :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoSamplingRate30 ) == BST_CHECKED ) ? 30 : 0,
														   ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize120_160 ) == BST_CHECKED ) ? 120 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize240_320 ) == BST_CHECKED ) ? 240 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize480_640 ) == BST_CHECKED ) ? 480 :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize960_1280 ) == BST_CHECKED ) ? 960 : 0,
														   ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize120_160 ) == BST_CHECKED ) ? 160 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize240_320 ) == BST_CHECKED ) ? 320 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize480_640 ) == BST_CHECKED ) ? 640 :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize960_1280 ) == BST_CHECKED ) ? 1280 : 0,
														   GetDlgItem( g_MainWndHdl, IDC_VideoInputPreview ), g_ErrInfoVarStrPt );
						
						//判断视频输入是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseYU12 ) == BST_CHECKED )
						{
							MediaProcThreadSetVideoInputUseYU12( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断视频输入是否使用OpenH264编码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseOpenH264 ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_OpenH264EncoderVideoType;
							int32_t p_OpenH264EncoderEncodedBitrate;
							int32_t p_OpenH264EncoderBitrateControlMode;
							int32_t p_OpenH264EncoderIDRFrameIntvl;
							int32_t p_OpenH264EncoderComplexity;

							GetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderVideoType = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderEncodedBitrate = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderBitrateControlMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderIDRFrameIntvl = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_OpenH264EncoderComplexity = atoi( p_TmpStrPt );

							MediaProcThreadSetVideoInputUseOpenH264Encoder( g_MediaProcThreadPt,
																			p_OpenH264EncoderVideoType,
																			p_OpenH264EncoderEncodedBitrate * 1024 * 8,
																			p_OpenH264EncoderBitrateControlMode,
																			p_OpenH264EncoderIDRFrameIntvl,
																			p_OpenH264EncoderComplexity,
																			g_ErrInfoVarStrPt );
						}
						
						//判断使用的视频输入设备。
						MediaProcThreadSetVideoInputUseDevice( g_MediaProcThreadPt, SendMessage( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), CB_GETCURSEL, 0, 0 ) - 1, g_ErrInfoVarStrPt );
						
						//判断视频输入是否黑屏。
						MediaProcThreadSetVideoInputIsBlack( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_VideoInputIsBlack ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );

						//判断是否使用视频输出。
						MediaProcThreadSetIsUseVideoOutput( g_MediaProcThreadPt,
														    ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseVideoTalkback ) == BST_CHECKED ) ? 1 :
														      ( IsDlgButtonChecked( g_MainWndHdl, IDC_UseAudioVideoTalkback ) == BST_CHECKED ) ? 1 : 0,
															( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize120_160 ) == BST_CHECKED ) ? 120 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize240_320 ) == BST_CHECKED ) ? 240 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize480_640 ) == BST_CHECKED ) ? 480 :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize960_1280 ) == BST_CHECKED ) ? 960 : 0,
														    ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize120_160 ) == BST_CHECKED ) ? 160 :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize240_320 ) == BST_CHECKED ) ? 320 :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize480_640 ) == BST_CHECKED ) ? 640 :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoFrameSize960_1280 ) == BST_CHECKED ) ? 1280 : 0,
															GetDlgItem( g_MainWndHdl, IDC_VideoOutputDisplay ),
															( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0 ) == BST_CHECKED ) ? 1.0f :
														      ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoDisplayScale1_5 ) == BST_CHECKED ) ? 1.5f :
														        ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoDisplayScale2_0 ) == BST_CHECKED ) ? 2.0f :
																  ( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseVideoDisplayScale3_0 ) == BST_CHECKED ) ? 3.0f : 0,
															g_ErrInfoVarStrPt );
						
						//判断视频输出是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseYU12 ) == BST_CHECKED )
						{
							MediaProcThreadSetVideoOutputUseYU12( g_MediaProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断视频输出是否使用OpenH264解码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseOpenH264 ) == BST_CHECKED )
						{
							MediaProcThreadSetVideoOutputUseOpenH264Decoder( g_MediaProcThreadPt, 0, g_ErrInfoVarStrPt );
						}
						
						//判断视频输出是否黑屏。
						MediaProcThreadSetVideoOutputIsBlack( g_MediaProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_VideoOutputIsBlack ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );

						//判断是否使用链表。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseLnkLstRecvOutputFrame ) == BST_CHECKED )
						{
							g_MediaInfoPt->m_UseWhatRecvOutputFrame = 0;
						}
						
						//判断是否使用自己设计的自适应抖动缓冲器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_UseAjbRecvOutputFrame ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int p_AAjbMinNeedBufFrameCnt;
							int p_AAjbMaxNeedBufFrameCnt;
							float p_AAjbAdaptSensitivity;
							int p_VAjbMinNeedBufFrameCnt;
							int p_VAjbMaxNeedBufFrameCnt;
							float p_VAjbAdaptSensitivity;

							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbMinNeedBufFrameCnt ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMinNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbMaxNeedBufFrameCnt ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbMaxNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AAjbAdaptSensitivity ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AAjbAdaptSensitivity = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbMinNeedBufFrameCnt ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMinNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbMaxNeedBufFrameCnt ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbMaxNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_VAjbAdaptSensitivity ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VAjbAdaptSensitivity = atof( p_TmpStrPt );

							g_MediaInfoPt->m_UseWhatRecvOutputFrame = 1;
							g_MediaInfoPt->m_AAjbMinNeedBufFrameCnt = p_AAjbMinNeedBufFrameCnt;
							g_MediaInfoPt->m_AAjbMaxNeedBufFrameCnt = p_AAjbMaxNeedBufFrameCnt;
							g_MediaInfoPt->m_AAjbAdaptSensitivity = p_AAjbAdaptSensitivity;
							g_MediaInfoPt->m_VAjbMinNeedBufFrameCnt = p_VAjbMinNeedBufFrameCnt;
							g_MediaInfoPt->m_VAjbMaxNeedBufFrameCnt = p_VAjbMaxNeedBufFrameCnt;
							g_MediaInfoPt->m_VAjbAdaptSensitivity = p_VAjbAdaptSensitivity;
						}

						//启动媒体处理线程。
						if( MediaProcThreadStart( g_MediaProcThreadPt, g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "启动媒体处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto outcreate;
						}

						LOGI( "启动媒体处理线程完毕。" );

						p_Result = 0; // //设置本函数执行成功。

						outcreate:
						if( p_Result != 0 )
						{
							if( g_MediaProcThreadPt != NULL )
							{
								MediaProcThreadRequireExit( g_MediaProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。
								MediaProcThreadDestroy( g_MediaProcThreadPt, g_ErrInfoVarStrPt ); //销毁媒体处理线程。
								g_MediaProcThreadPt = NULL;
							}
							if( g_MediaInfoPt != NULL )
							{
								VarStrDestroy( g_MediaInfoPt->m_IPAddrVarStrPt );
								VarStrDestroy( g_MediaInfoPt->m_PortVarStrPt );
								free( g_MediaInfoPt );
								g_MediaInfoPt = NULL;
							}
						}
					}
					else //如果媒体处理线程已经启动。
					{
						MediaProcThreadRequireExit( g_MediaProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。

						p_Result = 0; // //设置本函数执行成功。
					}
					return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
				}
				case IDC_Setting: //设置控件。
				{
					ShowWindow( g_SettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_UseEffectLow: //效果等级：低。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseEffectLow, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioSamplingRate8000, IDC_UseAudioSamplingRate32000, IDC_UseAudioSamplingRate8000 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioFrameLen10ms, IDC_UseAudioFrameLen30ms, IDC_UseAudioFrameLen20ms );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoAec, IDC_UseSpeexWebRtcAec, IDC_UseWebRtcAecm );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoNs, IDC_UseRNNoise, IDC_UseSpeexPprocNs );
					CheckDlgButton( g_SettingWndHdl, IDC_IsUseSpeexPprocOther, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UsePcm, IDC_UseOpusCodec, IDC_UseSpeexCodec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoSamplingRate12, IDC_UseVideoSamplingRate30, IDC_UseVideoSamplingRate12 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoFrameSize120_160, IDC_UseVideoFrameSize960_1280, IDC_UseVideoFrameSize120_160 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0, IDC_UseVideoDisplayScale3_0, IDC_UseVideoDisplayScale1_0 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseYU12, IDC_UseOpenH264, IDC_UseOpenH264 );

					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), "0" );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), "420" );

					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), "-32768" );
					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), "3" );

					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), "95" );
					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), "25" );

					CheckRadioButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr, IDC_SpeexCodecEncoderUseVbr, IDC_SpeexCodecEncoderUseCbr );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), "1" );
					CheckDlgButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), "12" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), "0" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseEffectMid: //效果等级：中。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseEffectMid, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioSamplingRate8000, IDC_UseAudioSamplingRate32000, IDC_UseAudioSamplingRate16000 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioFrameLen10ms, IDC_UseAudioFrameLen30ms, IDC_UseAudioFrameLen20ms );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoAec, IDC_UseSpeexWebRtcAec, IDC_UseWebRtcAec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoNs, IDC_UseRNNoise, IDC_UseWebRtcNsx );
					CheckDlgButton( g_SettingWndHdl, IDC_IsUseSpeexPprocOther, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UsePcm, IDC_UseOpusCodec, IDC_UseSpeexCodec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoSamplingRate12, IDC_UseVideoSamplingRate30, IDC_UseVideoSamplingRate15 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoFrameSize120_160, IDC_UseVideoFrameSize960_1280, IDC_UseVideoFrameSize240_320 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0, IDC_UseVideoDisplayScale3_0, IDC_UseVideoDisplayScale1_0 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseYU12, IDC_UseOpenH264, IDC_UseOpenH264 );

					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), "0" );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec, IDC_SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAec );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), "420" );

					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), "-32768" );
					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), "3" );

					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), "95" );
					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), "25" );

					CheckRadioButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr, IDC_SpeexCodecEncoderUseVbr, IDC_SpeexCodecEncoderUseCbr );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), "4" );
					CheckDlgButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), "15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), "0" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseEffectHigh: //效果等级：高。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseEffectHigh, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioSamplingRate8000, IDC_UseAudioSamplingRate32000, IDC_UseAudioSamplingRate16000 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioFrameLen10ms, IDC_UseAudioFrameLen30ms, IDC_UseAudioFrameLen20ms );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoAec, IDC_UseSpeexWebRtcAec, IDC_UseSpeexWebRtcAec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoNs, IDC_UseRNNoise, IDC_UseWebRtcNs );
					CheckDlgButton( g_SettingWndHdl, IDC_IsUseSpeexPprocOther, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UsePcm, IDC_UseOpusCodec, IDC_UseSpeexCodec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoSamplingRate12, IDC_UseVideoSamplingRate30, IDC_UseVideoSamplingRate15 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoFrameSize120_160, IDC_UseVideoFrameSize960_1280, IDC_UseVideoFrameSize480_640 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0, IDC_UseVideoDisplayScale3_0, IDC_UseVideoDisplayScale1_0 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseYU12, IDC_UseOpenH264, IDC_UseOpenH264 );

					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), "0" );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), "420" );

					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), "-32768" );
					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), "3" );

					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), "95" );
					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), "25" );

					CheckRadioButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr, IDC_SpeexCodecEncoderUseVbr, IDC_SpeexCodecEncoderUseVbr );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), "8" );
					CheckDlgButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), "15" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), "1" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseEffectSuper: //效果等级：超。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseEffectSuper, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioSamplingRate8000, IDC_UseAudioSamplingRate32000, IDC_UseAudioSamplingRate16000 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioFrameLen10ms, IDC_UseAudioFrameLen30ms, IDC_UseAudioFrameLen20ms );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoAec, IDC_UseSpeexWebRtcAec, IDC_UseSpeexWebRtcAec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoNs, IDC_UseRNNoise, IDC_UseRNNoise );
					CheckDlgButton( g_SettingWndHdl, IDC_IsUseSpeexPprocOther, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UsePcm, IDC_UseOpusCodec, IDC_UseSpeexCodec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoSamplingRate12, IDC_UseVideoSamplingRate30, IDC_UseVideoSamplingRate24 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoFrameSize120_160, IDC_UseVideoFrameSize960_1280, IDC_UseVideoFrameSize480_640 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0, IDC_UseVideoDisplayScale3_0, IDC_UseVideoDisplayScale1_0 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseYU12, IDC_UseOpenH264, IDC_UseOpenH264 );

					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), "0" );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), "420" );

					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), "-32768" );
					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), "3" );

					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), "95" );
					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), "25" );

					CheckRadioButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr, IDC_SpeexCodecEncoderUseVbr, IDC_SpeexCodecEncoderUseVbr );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), "10" );
					CheckDlgButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), "24" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), "1" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseEffectPremium: //效果等级：特。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseEffectPremium, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioSamplingRate8000, IDC_UseAudioSamplingRate32000, IDC_UseAudioSamplingRate32000 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseAudioFrameLen10ms, IDC_UseAudioFrameLen30ms, IDC_UseAudioFrameLen20ms );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoAec, IDC_UseSpeexWebRtcAec, IDC_UseSpeexWebRtcAec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseNoNs, IDC_UseRNNoise, IDC_UseRNNoise );
					CheckDlgButton( g_SettingWndHdl, IDC_IsUseSpeexPprocOther, BST_CHECKED );
					CheckRadioButton( g_SettingWndHdl, IDC_UsePcm, IDC_UseOpusCodec, IDC_UseSpeexCodec );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoSamplingRate12, IDC_UseVideoSamplingRate30, IDC_UseVideoSamplingRate30 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoFrameSize120_160, IDC_UseVideoFrameSize960_1280, IDC_UseVideoFrameSize960_1280 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseVideoDisplayScale1_0, IDC_UseVideoDisplayScale3_0, IDC_UseVideoDisplayScale1_0 );
					CheckRadioButton( g_SettingWndHdl, IDC_UseYU12, IDC_UseOpenH264, IDC_UseOpenH264 );

					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexAecSettingWndHdl, IDC_SpeexAecIsSaveMemFile, BST_UNCHECKED );

					CheckDlgButton( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WebRtcAecmDelay ), "0" );

					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecDelay ), "0" );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_WebRtcAecSettingWndHdl, IDC_WebRtcAecIsSaveMemFile, BST_UNCHECKED );

					CheckRadioButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecm, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec, IDC_SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAec );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecFilterLen ), "500" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecIsUseRec, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoMutp ), "3.0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoCntu ), "0.65" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupes ), "-32768" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSpeexAecEchoSupesAct ), "-32768" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmIsUseCNGMode, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmEchoMode ), "4" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecmDelay ), "0" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecEchoMode ), "2" );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecDelay ), "0" );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseDelayAgnosticMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseExtdFilterMode, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecMode, BST_UNCHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelay, BST_CHECKED );
					CheckDlgButton( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecIsUseSameRoomAec, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SpeexWebRtcAecSameRoomEchoMinDelay ), "420" );

					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseNs, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocNoiseSupes ), "-32768" );
					CheckDlgButton( g_SpeexPprocNsSettingWndHdl, IDC_SpeexPprocIsUseDereverb, BST_CHECKED );

					SetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WebRtcNsxPolicyMode ), "3" );

					SetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WebRtcNsPolicyMode ), "3" );

					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseVad, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbStart ), "95" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocVadProbCntu ), "95" );
					CheckDlgButton( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocIsUseAgc, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcLevel ), "30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcIncrement ), "10" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcDecrement ), "-30000" );
					SetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SpeexPprocAgcMaxGain ), "25" );

					CheckRadioButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderUseCbr, IDC_SpeexCodecEncoderUseVbr, IDC_SpeexCodecEncoderUseVbr );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderComplexity ), "10" );
					CheckDlgButton( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecIsUsePerceptualEnhancement, BST_CHECKED );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderVideoType ), "0" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderBitrateControlMode ), "3" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderIDRFrameIntvl ), "30" );
					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderComplexity ), "2" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseBitrateLow: //比特率等级：低。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseBitrateLow, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), "1" );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), "1" );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), "1" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseBitrateMid: //比特率等级：中。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseBitrateMid, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), "4" );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), "40" );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), "20" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseBitrateHigh: //比特率等级：高。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseBitrateHigh, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), "8" );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), "80" );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), "40" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseBitrateSuper: //比特率等级：超。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseBitrateSuper, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), "10" );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), "100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), "60" );

					return ( INT_PTR )TRUE;
				}
				case IDC_UseBitratePremium: //比特率等级：特。
				{
					CheckDlgButton( g_SettingWndHdl, IDC_UseBitratePremium, BST_CHECKED );

					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderQuality ), "10" );
					SetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SpeexCodecEncoderPlcExpectedLossRate ), "100" );

					SetWindowText( GetDlgItem( g_OpenH264CodecSettingWndHdl, IDC_OpenH264EncoderEncodedBitrate ), "80" );

					return ( INT_PTR )TRUE;
				}
				case IDC_AjbSetting:
				{
					ShowWindow( g_AjbSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SpeexAecSetting: //Speex声学回音消除设置控件。
				{
					ShowWindow( g_SpeexAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SpeexAecDelMemFile: //Speex声学回音消除器的删除内存块文件控件。
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
				case IDC_WebRtcAecmSetting:
				{
					ShowWindow( g_WebRtcAecmSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WebRtcAecSetting:
				{
					ShowWindow( g_WebRtcAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SpeexWebRtcAecSetting:
				{
					ShowWindow( g_SpeexWebRtcAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示SpeexWebRtc三重声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WebRtcAecDelMemFile: //WebRtc浮点版声学回音消除器的删除内存块文件控件。
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
				case IDC_SpeexPprocNsSetting:
				{
					ShowWindow( g_SpeexPprocNsSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的噪音抑制设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WebRtcNsxSetting:
				{
					ShowWindow( g_WebRtcNsxSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版噪音抑制器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WebRtcNsSetting:
				{
					ShowWindow( g_WebRtcNsSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版噪音抑制器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SpeexPprocOtherSetting:
				{
					ShowWindow( g_SpeexPprocOtherSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的其他功能设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SpeexCodecSetting:
				{
					ShowWindow( g_SpeexCodecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex编解码器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_OpenH264Setting:
				{
					ShowWindow( g_OpenH264CodecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex编解码器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_ReadMe:
				{
					ShellExecute( NULL, "open", "https://github.com/cyz7758520/Windows_audio_talkback_demo_program", NULL, NULL, SW_SHOWNORMAL ); //打开必读说明网页。
					return ( INT_PTR )TRUE;
				}
				case IDC_ClearLog:
				{
					SendMessage( GetDlgItem( hDlg, IDC_LogList ), LB_RESETCONTENT, 0, 0 ); //清空日志列表。
					SendDlgItemMessage( hDlg, IDC_LogList, LB_SETHORIZONTALEXTENT, 0, 0 ); //清空滚动条的当前像素大小。
					return ( INT_PTR )TRUE;
				}
				case IDCANCEL:
				case IDC_SettingOk:
				case IDC_AjbSettingOk:
				case IDC_SpeexAecSettingOk:
				case IDC_WebRtcAecmSettingOk:
				case IDC_WebRtcAecSettingOk:
				case IDC_SpeexWebRtcAecSettingOk:
				case IDC_SpeexPprocNsSettingOk:
				case IDC_WebRtcNsxSettingOk:
				case IDC_WebRtcNsSettingOk:
				case IDC_SpeexPprocOtherSettingOk:
				case IDC_SpeexCodecSettingOk:
				case IDC_OpenH264SettingOk:
				{
					if( g_MainWndHdl == hDlg ) //如果是主界面。
					{
						LOGI( "用户在主界面按下关闭控件，本软件退出。" );

						MediaProcThreadRequireExit( g_MediaProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求媒体处理线程退出。

						PostQuitMessage( 0 ); //发送退出消息。
					}
					else //如果是其他界面。
					{
						EnableWindow( GetParent( hDlg ), TRUE ), ShowWindow( hDlg, SW_HIDE ); //隐藏当前模态对话框。
					}
					return ( INT_PTR )TRUE;
				}
				case IDC_VideoInputPreview: //视频输入预览控件。
				{
					if( g_VideoInputPreviewWndIsMax == 0 ) //如果视频输入预览窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainWndHdl, &p_Rect );
						SetWindowPos( g_VideoInputPreviewWndHdl, g_VideoOutputDisplayWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseTcpPrtcl ), SW_HIDE ); //设置TCP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseUdpPrtcl ), SW_HIDE ); //设置UDP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), SW_HIDE ); //设置IP地址控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Port ), SW_HIDE ); //设置端口控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioTalkback ), SW_HIDE ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoTalkback ), SW_HIDE ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioVideoTalkback ), SW_HIDE ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), SW_HIDE ); //设置音频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), SW_HIDE ); //设置音频输出设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), SW_HIDE ); //设置视频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioInputIsMute ), SW_HIDE ); //设置音频输入静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioOutputIsMute ), SW_HIDE ); //设置音频输出静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoInputIsBlack ), SW_HIDE ); //设置视频输入黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoOutputIsBlack ), SW_HIDE ); //设置视频输出黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CreateSrvr ), SW_HIDE ); //设置创建服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CnctSrvr ), SW_HIDE ); //设置连接服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Setting ), SW_HIDE ); //设置设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ReadMe ), SW_HIDE ); //设置必读说明控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ClearLog ), SW_HIDE ); //设置清空日志控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_LogList ), SW_HIDE ); //设置日志列表控件为隐藏。
						ShowWindow( g_VideoOutputDisplayWndHdl, SW_HIDE ); //设置视频输出显示控件为隐藏。
						InvalidateRect( g_MainWndHdl, NULL, TRUE );
						g_VideoInputPreviewWndIsMax = 1;
					}
					else //如果视频输入预览窗口已经最大化。
					{
						SetWindowPos( g_VideoInputPreviewWndHdl, g_VideoOutputDisplayWndHdl, g_VideoInputPreviewWndRect.left, g_VideoInputPreviewWndRect.top, g_VideoInputPreviewWndRect.right - g_VideoInputPreviewWndRect.left, g_VideoInputPreviewWndRect.bottom - g_VideoInputPreviewWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseTcpPrtcl ), SW_SHOW ); //设置TCP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseUdpPrtcl ), SW_SHOW ); //设置UDP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), SW_SHOW ); //设置IP地址控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Port ), SW_SHOW ); //设置端口控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioTalkback ), SW_SHOW ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoTalkback ), SW_SHOW ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioVideoTalkback ), SW_SHOW ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), SW_SHOW ); //设置音频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), SW_SHOW ); //设置音频输出设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), SW_SHOW ); //设置视频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioInputIsMute ), SW_SHOW ); //设置音频输入静音控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioOutputIsMute ), SW_SHOW ); //设置音频输出静音控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoInputIsBlack ), SW_SHOW ); //设置视频输入黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoOutputIsBlack ), SW_SHOW ); //设置视频输出黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CreateSrvr ), SW_SHOW ); //设置创建服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CnctSrvr ), SW_SHOW ); //设置连接服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Setting ), SW_SHOW ); //设置设置控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ReadMe ), SW_SHOW ); //设置必读说明控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ClearLog ), SW_SHOW ); //设置清空日志控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_LogList ), SW_SHOW ); //设置日志列表控件为显示。
						ShowWindow( g_VideoOutputDisplayWndHdl, SW_SHOW ); //设置视频输出显示控件为显示。
						InvalidateRect( g_MainWndHdl, NULL, TRUE );
						g_VideoInputPreviewWndIsMax = 0;
					}
					return ( INT_PTR )TRUE;
				}
				case IDC_VideoOutputDisplay: //视频输出显示控件。
				{
					if( g_VideoOutputDisplayWndIsMax == 0 ) //如果视频输出显示窗口没有最大化。
					{
						RECT p_Rect;
						GetClientRect( g_MainWndHdl, &p_Rect );
						SetWindowPos( g_VideoOutputDisplayWndHdl, g_VideoInputPreviewWndHdl, p_Rect.left, p_Rect.top, p_Rect.right, p_Rect.bottom, 0 );

						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseTcpPrtcl ), SW_HIDE ); //设置TCP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseUdpPrtcl ), SW_HIDE ); //设置UDP协议控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), SW_HIDE ); //设置IP地址控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Port ), SW_HIDE ); //设置端口控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioTalkback ), SW_HIDE ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoTalkback ), SW_HIDE ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioVideoTalkback ), SW_HIDE ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), SW_HIDE ); //设置音频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), SW_HIDE ); //设置音频输出设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), SW_HIDE ); //设置视频输入设备控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioInputIsMute ), SW_HIDE ); //设置音频输入静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioOutputIsMute ), SW_HIDE ); //设置音频输出静音控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoInputIsBlack ), SW_HIDE ); //设置视频输入黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoOutputIsBlack ), SW_HIDE ); //设置视频输出黑屏控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CreateSrvr ), SW_HIDE ); //设置创建服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CnctSrvr ), SW_HIDE ); //设置连接服务端控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Setting ), SW_HIDE ); //设置设置控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ReadMe ), SW_HIDE ); //设置必读说明控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ClearLog ), SW_HIDE ); //设置清空日志控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_LogList ), SW_HIDE ); //设置日志列表控件为隐藏。
						ShowWindow( g_VideoInputPreviewWndHdl, SW_HIDE ); //设置视频输入预览控件为隐藏。
						InvalidateRect( g_MainWndHdl, NULL, TRUE );
						g_VideoOutputDisplayWndIsMax = 1;
					}
					else //如果视频输出显示窗口已经最大化。
					{
						SetWindowPos( g_VideoOutputDisplayWndHdl, g_VideoInputPreviewWndHdl, g_VideoOutputDisplayWndRect.left, g_VideoOutputDisplayWndRect.top, g_VideoOutputDisplayWndRect.right - g_VideoOutputDisplayWndRect.left, g_VideoOutputDisplayWndRect.bottom - g_VideoOutputDisplayWndRect.top, 0 );
						
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseTcpPrtcl ), SW_SHOW ); //设置TCP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseUdpPrtcl ), SW_SHOW ); //设置UDP协议控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_IPAddr ), SW_SHOW ); //设置IP地址控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Port ), SW_SHOW ); //设置端口控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioTalkback ), SW_SHOW ); //设置音频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoTalkback ), SW_SHOW ); //设置视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioVideoTalkback ), SW_SHOW ); //设置音视频对讲控件为隐藏。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioInputDevice ), SW_SHOW ); //设置音频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseAudioOutputDevice ), SW_SHOW ); //设置音频输出设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_UseVideoInputDevice ), SW_SHOW ); //设置视频输入设备控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioInputIsMute ), SW_SHOW ); //设置音频输入静音控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_AudioOutputIsMute ), SW_SHOW ); //设置音频输出静音控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoInputIsBlack ), SW_SHOW ); //设置视频输入黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_VideoOutputIsBlack ), SW_SHOW ); //设置视频输出黑屏控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CreateSrvr ), SW_SHOW ); //设置创建服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_CnctSrvr ), SW_SHOW ); //设置连接服务端控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_Setting ), SW_SHOW ); //设置设置控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ReadMe ), SW_SHOW ); //设置必读说明控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_ClearLog ), SW_SHOW ); //设置清空日志控件为显示。
						ShowWindow( GetDlgItem( g_MainWndHdl, IDC_LogList ), SW_SHOW ); //设置日志列表控件为显示。
						ShowWindow( g_VideoInputPreviewWndHdl, SW_SHOW ); //设置视频输入预览控件为显示。
						InvalidateRect( g_MainWndHdl, NULL, TRUE );
						g_VideoOutputDisplayWndIsMax = 0;
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
		case WM_MYMediaProcThreadSTART: //媒体处理线程启动的消息。
		{
			if( g_MediaInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
            {
				EnableWindow( GetDlgItem( hDlg, IDC_UseTcpPrtcl ), FALSE ); //设置TCP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_UseUdpPrtcl ), FALSE ); //设置UDP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_IPAddr ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_Port ), FALSE ); //设置端口控件为不可用。
				SetWindowText( GetDlgItem( hDlg, IDC_CreateSrvr ), "中断" ); //设置创建服务端控件的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, IDC_CnctSrvr ), FALSE ); //设置连接服务端控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_Setting ), FALSE ); //设置设置控件为不可用。
            }
            else //如果是创建客户端。
            {
				EnableWindow( GetDlgItem( hDlg, IDC_UseTcpPrtcl ), FALSE ); //设置TCP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_UseUdpPrtcl ), FALSE ); //设置UDP协议控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_IPAddr ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_Port ), FALSE ); //设置端口控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_CreateSrvr ), FALSE ); //设置创建服务端控件为不可用。
				SetWindowText( GetDlgItem( hDlg, IDC_CnctSrvr ), "中断" ); //设置连接服务端控件的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, IDC_Setting ), FALSE ); //设置设置控件为不可用。
            }
			return ( INT_PTR )TRUE;
		}
		case WM_MYMediaProcThreadEND: //媒体处理线程退出的消息。
		{
			if( g_MediaProcThreadPt != NULL )
			{
				MediaProcThreadDestroy( g_MediaProcThreadPt, g_ErrInfoVarStrPt ); //销毁媒体处理线程。
				g_MediaProcThreadPt = NULL;
			}
			if( g_MediaInfoPt != NULL )
			{
				VarStrDestroy( g_MediaInfoPt->m_IPAddrVarStrPt );
				VarStrDestroy( g_MediaInfoPt->m_PortVarStrPt );
				if( g_MediaInfoPt->m_TmpBytePt != NULL ) free( g_MediaInfoPt->m_TmpBytePt );
				if( g_MediaInfoPt->m_TmpByte2Pt != NULL ) free( g_MediaInfoPt->m_TmpByte2Pt );
				if( g_MediaInfoPt->m_TmpByte3Pt != NULL ) free( g_MediaInfoPt->m_TmpByte3Pt );
				free( g_MediaInfoPt );
				g_MediaInfoPt = NULL;
			}

			EnableWindow( GetDlgItem( hDlg, IDC_UseTcpPrtcl ), TRUE ); //设置TCP协议控件为可用。
			EnableWindow( GetDlgItem( hDlg, IDC_UseUdpPrtcl ), TRUE ); //设置UDP协议控件为可用。
			EnableWindow( GetDlgItem( hDlg, IDC_IPAddr ), TRUE ); //设置IP地址控件为可用。
			EnableWindow( GetDlgItem( hDlg, IDC_Port ), TRUE ); //设置端口控件为可用。
			EnableWindow( GetDlgItem( hDlg, IDC_CreateSrvr ), TRUE ); //设置创建服务端控件为可用。
			SetWindowText( GetDlgItem( hDlg, IDC_CreateSrvr ), "创建服务端" ); //设置创建服务端控件的内容为“创建服务端”。
			EnableWindow( GetDlgItem( hDlg, IDC_CnctSrvr ), TRUE ); //设置连接服务端控件为可用。
			SetWindowText( GetDlgItem( hDlg, IDC_CnctSrvr ), "连接服务端" ); //设置连接服务端控件的内容为“连接服务端”。
			EnableWindow( GetDlgItem( hDlg, IDC_Setting ), TRUE ); //设置设置控件为可用。
			return ( INT_PTR )TRUE;
		}
		case WM_SHOWLOG: //显示日志的消息。
		{
			if( ( VarStr * )wParam != NULL )
			{
				//显示日志。
				{
					SYSTEMTIME p_SystemTime;

					GetLocalTime( &p_SystemTime );
					VarStrFmtIns( ( VarStr * )wParam, 0, "%02.2d:%02.2d:%02.2d %03.3d：", p_SystemTime.wHour, p_SystemTime.wMinute, p_SystemTime.wSecond, p_SystemTime.wMilliseconds );
					SendMessage( GetDlgItem( hDlg, IDC_LogList ), LB_ADDSTRING, 0, ( WPARAM )( ( VarStr * )wParam )->m_StrPt );
				}
				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, IDC_LogList, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( ( VarStr * )wParam )->m_StrPt, ( ( VarStr * )wParam )->m_StrLen, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, IDC_LogList, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}
				VarStrDestroy( ( VarStr * )wParam );
			}
			return ( INT_PTR )TRUE;
		}
		case WM_REPAINT: //重绘消息。
		{
			InvalidateRect( g_VideoInputPreviewWndHdl, NULL, TRUE ); //重绘视频输入预览窗口。
			InvalidateRect( g_VideoOutputDisplayWndHdl, NULL, TRUE ); //重绘视频输出显示窗口。
			return ( INT_PTR )TRUE;
		}
	}
	return ( INT_PTR )FALSE;
}
