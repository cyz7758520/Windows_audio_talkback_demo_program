// Windows_audio_talkback_demo_program.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Windows_audio_talkback_demo_program.h"
#include "Queue.h"
#include "LnkLst.h"
#include "Sokt.h"
#include "Ajb.h"
#include "AudioProcThread.h"
#include "Log.h"
#include "Func.h"
#include "ThreadLock.h"
#include "shellapi.h"
#include <mmeapi.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <mmddk.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define WM_MYAUDIOPROCTHREADSTART  WM_USER + 101  //音频处理线程启动的消息。
#define WM_MYAUDIOPROCTHREADEND    WM_USER + 102  //音频处理线程退出的消息。
#define WM_SHOWLOG                 WM_USER + 103  //显示日志的消息。

//数据包类型：0x00表示连接请求或心跳，0x01表示输入输出帧，0x02表示连接应答或输入输出帧应答，0x03表示退出。
//存放音频信息。
typedef struct TagAudioInfo
{
	VarStr * m_IPAddrVarStrPt; //存放IP地址动态字符串的内存指针。
    VarStr * m_PortVarStrPt; //存放端口号。
    HWND m_MainWindowHdl; //存放主窗口的句柄。

	int m_UseWhatXfrPrtcl; //存放使用什么传输协议，为0表示TCP协议，为1表示UDP协议。
    int m_IsCreateSrvrOrClnt; //存放创建服务端或者客户端标记，为1表示创建服务端，为0表示创建客户端。
    TcpSrvrSokt * m_TcpSrvrSoktPt; //存放本端TCP协议服务端套接字的内存指针。
    TcpClntSokt * m_TcpClntSoktPt; //存放本端TCP协议客户端套接字的内存指针。
	UdpSokt * m_UdpSoktPt; //存放本端UDP协议套接字的内存指针。
	int m_TcpClntSoktReInitTimes; //存放本端TCP协议客户端套接字类对象重新初始化的次数。
    uint64_t m_LastPktSendTime; //存放最后一个数据包的发送时间，用于判断连接是否中断。
    uint64_t m_LastPktRecvTime; //存放最后一个数据包的接收时间，用于判断连接是否中断。

    int m_LastSendInputFrameIsAct; //存放最后一个发送的输入帧有无语音活动，为1表示有语音活动，为0表示无语音活动。
	int m_LastSendInputFrameIsRecv; //存放最后一个发送的输入帧远端是否接收到，为0表示没有收到，为非0表示已经收到。
    int32_t m_SendInputFrameTimeStamp; //存放发送输入帧的时间戳。
    int32_t m_RecvOutputFrameTimeStamp; //存放接收输出帧的时间戳。
    int8_t m_IsRecvExitPkt; //存放是否接收到退出包，为0表示否，为1表示是。

    int m_UseWhatRecvOutputFrame; //存放使用什么接收输出帧，为0表示链表，为1表示自适应抖动缓冲器。
	MutexLock * m_RecvOutputFrameMutexLockPt; //存放接收输出帧互斥锁的内存指针。

    VarLenLnkLst * m_RecvOutputFrameLnkLstPt; //存放接收输出帧链表的内存指针。

    Ajb * m_AjbPt; //存放自适应抖动缓冲器的内存指针。
    int m_AjbMinNeedBufFrameCnt; //存放自适应抖动缓冲器的最小需缓冲帧数量，单位个。
    int m_AjbMaxNeedBufFrameCnt; //存放自适应抖动缓冲器的最大需缓冲帧数量，单位个。
    int8_t m_AjbAdaptSensitivity; //存放自适应抖动缓冲器的自适应灵敏度，灵敏度越大自适应计算当前需缓冲帧的数量越多，取值区间为[0,127]。

	int8_t * m_TmpBytePt; //存放临时数据的内存指针。
	size_t m_TmpByteSz; //存放临时数据的内存大小。
}AudioInfo;

// 全局变量:
HINSTANCE hInst; //当前实例
VarStr * g_ErrInfoVarStrPt = NULL; //存放错误信息动态字符串的内存指针。
AudioProcThread * g_AudioProcThreadPt = NULL; //音频处理线程
AudioInfo * g_AudioInfoPt = NULL; //音频信息
HWND g_MainWndHdl = NULL; //存放主界面窗口句柄。
HWND g_SettingWndHdl = NULL; //存放设置界面窗口句柄。
HWND g_SpeexAecSettingWndHdl = NULL; //存放Speex声学回音消除器设置界面窗口句柄。
HWND g_WebRtcAecmSettingWndHdl = NULL; //存放WebRtc定点版声学回音消除器设置界面窗口句柄。
HWND g_WebRtcAecSettingWndHdl = NULL; //存放WebRtc浮点版声学回音消除器设置界面窗口句柄。
HWND g_SpeexWebRtcAecSettingWndHdl = NULL; //存放SpeexWebRtc三重声学回音消除器设置界面窗口句柄。
HWND g_SpeexPprocNsSettingWndHdl = NULL; //存放Speex预处理器的噪音抑制设置界面窗口句柄。
HWND g_WebRtcNsxSettingWndHdl = NULL; //存放WebRtc定点版噪音抑制器设置界面窗口句柄。
HWND g_WebRtcNsSettingWndHdl = NULL; //存放WebRtc浮点版噪音抑制器设置界面窗口句柄。
HWND g_SpeexPprocOtherSettingWndHdl = NULL; //存放Speex预处理器的其他功能设置界面窗口句柄。
HWND g_SpeexCodecSettingWndHdl = NULL; //存放Speex编解码器设置界面窗口句柄。
HWND g_AjbSettingWndHdl = NULL; //存放自适应抖动缓冲器设置界面窗口句柄。
HWND g_ReadMeWndHdl = NULL; //存放必读说明界面窗口句柄。

// 此代码模块中包含的函数的前向声明:
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );

//用户定义的初始化函数，在本线程刚启动时调用一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyAudioProcThreadUserInit( AudioProcThread * AudioProcThreadPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	
	PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_MYAUDIOPROCTHREADSTART, 0, 0 ); //向主界面发送音频处理线程启动的消息。

	g_AudioInfoPt->m_IsRecvExitPkt = 0; //设置没有接收到退出包。
	if( g_AudioInfoPt->m_TmpBytePt == NULL )
	{
		g_AudioInfoPt->m_TmpBytePt = ( int8_t * )malloc( 1 + 4 + AudioProcThreadPt->m_FrameLen * 2 ); //创建存放临时数据的内存块。
		if( g_AudioInfoPt->m_TmpBytePt == NULL )
		{
			VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "malloc 临时数据数组的内存块失败。" );
			LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
		g_AudioInfoPt->m_TmpByteSz = 1 + 4 + AudioProcThreadPt->m_FrameLen * 2; //设置临时数据的内存大小。
	}

	if( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) //如果使用TCP协议。
	{
		if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接。
		{
			if( TcpSrvrInit( &g_AudioInfoPt->m_TcpSrvrSoktPt, 4, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt, 1, 1, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端TCP协议服务端套接字成功。
			{
				char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

				if( TcpSrvrGetLclAddr( g_AudioInfoPt->m_TcpSrvrSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。
				{
					VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端TCP协议服务端套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			}
			else
			{
				VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端TCP协议服务端套接字[%s:%s]失败。原因：", g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}

			char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

			while( true ) //循环接受远端TCP协议客户端套接字的连接。
			{
				if( TcpSrvrAccept( g_AudioInfoPt->m_TcpSrvrSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, 1, &g_AudioInfoPt->m_TcpClntSoktPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
				{
					if( g_AudioInfoPt->m_TcpClntSoktPt != NULL ) //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接成功。
					{
						TcpSrvrDestroy( g_AudioInfoPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字，防止还有其他远端TCP协议客户端套接字继续连接。
						g_AudioInfoPt->m_TcpSrvrSoktPt = NULL;

						VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						break;
					}
					else //如果用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接超时。
					{

					}
				}
				else
				{
					VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端TCP协议服务端套接字接受远端TCP协议客户端套接字的连接失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
				}

				if( AudioProcThreadPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
			}
		}
		else if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端TCP协议客户端套接字连接远端TCP协议服务端套接字。
		{
			int p_ReInitTimes = 1;
			while( true ) //循环连接已监听的远端TCP协议服务端套接字。
			{
				if( TcpClntInit( &g_AudioInfoPt->m_TcpClntSoktPt, 4, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt, NULL, NULL, 5000, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字成功。
				{
					char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
					char p_LclNodePortPt[6];
					char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
					char p_RmtNodePortPt[6];

					if( TcpClntGetLclAddr( g_AudioInfoPt->m_TcpClntSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
					{
						VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字绑定的本地节点地址和端口失败。原因：" );
						LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
					}
					if( TcpClntGetRmtAddr( g_AudioInfoPt->m_TcpClntSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
					{
						VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端TCP协议客户端套接字连接的远端TCP协议客户端套接字绑定的远程节点地址和端口失败。原因：" );
						LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
					}

					VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化本端TCP协议客户端套接字[%s:%s]，并连接已监听的远端TCP协议服务端套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt, p_RmtNodeAddrPt, p_RmtNodePortPt );
					LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					break; //跳出重连。
				}
				else
				{
					VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化本端TCP协议客户端套接字，并连接已监听的远端TCP协议服务端套接字[%s:%s]失败。原因：", g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					
                    if( p_ReInitTimes <= 5 ) //如果还需要进行重连。
                    {
						VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "开始第 %d 次重连。", p_ReInitTimes );
						LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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

		if( TcpClntSetNoDelay( g_AudioInfoPt->m_TcpClntSoktPt, 1, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。
		{
			VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "设置已连接的本端TCP协议客户端套接字的Nagle延迟算法状态为禁用失败。原因：" );
			LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
	}
	else //如果使用UDP协议。
    {
        if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建本端UDP协议套接字接受远端UDP协议套接字的连接。
        {
            if( UdpInit( &g_AudioInfoPt->m_UdpSoktPt, 4, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

                if( UdpGetLclAddr( g_AudioInfoPt->m_UdpSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
					VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
				VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字[%s:%s]失败。原因：", g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

            while( true ) //循环接受远端UDP协议套接字的连接。
            {
                if( UdpRecvPkt( g_AudioInfoPt->m_UdpSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 1, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                    {
                        if( ( p_TmpSz == 1 ) && ( g_AudioInfoPt->m_TmpBytePt[0] == 0x00 ) ) //如果是连接请求包。
                        {
                            UdpConnect( g_AudioInfoPt->m_UdpSoktPt, 4, p_RmtNodeAddrPt, p_RmtNodePortPt, NULL ); //用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字，已连接的本端UDP协议套接字只能接收连接的远端UDP协议套接字发送的数据包。

                            int p_ReSendTimes = 1;
                            while( true ) //循环发送连接请求包，并接收连接应答包。
                            {
                                g_AudioInfoPt->m_TmpBytePt[0] = 0x00; //设置连接请求包。
                                if( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                                {
									VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
									LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                                    goto out;
                                }

                                while( true ) //循环接收连接应答包。
                                {
                                    if( UdpRecvPkt( g_AudioInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 1000, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                                    {
                                        if( p_TmpSz != -1 ) //如果用已监听的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                                        {
                                            if( ( p_TmpSz >= 1 ) && ( g_AudioInfoPt->m_TmpBytePt[0] != 0x00 ) ) //如果不是连接请求包。
                                            {
                                                VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字接受远端UDP协议套接字[%s:%s]的连接成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
												LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
												{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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
                                                VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
												LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
												{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
												goto UdpSrvrReSend; //重新接受连接。
                                            }
                                        }
                                    }
                                    else
                                    {
                                        VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
										LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
										{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
										goto UdpSrvrReSend; //重新接受连接。
                                    }
                                }
								UdpSrvrReRecv:;
                            }
							UdpSrvrReSend:
							UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL ); //将已连接的本端UDP协议套接字断开连接的远端UDP协议套接字，已连接的本端UDP协议套接字将变成已监听的本端UDP协议套接字。

							VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "本端UDP协议套接字继续保持监听来接受连接。" );
							LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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
                    VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字接受远端UDP协议套接字的连接失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				if( AudioProcThreadPt->m_ExitFlag != 0 ) //如果本线程接收到退出请求。
				{
					LOGI( "本线程接收到退出请求，开始准备退出。" );
					goto out;
				}
            }
			UdpSrvrReAccept:;
        }
        else if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建本端UDP协议套接字连接远端UDP协议套接字。
        {
            if( UdpInit( &g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果创建并初始化已监听的本端UDP协议套接字成功。
            {
                char p_LclNodeAddrPt[INET6_ADDRSTRLEN];
				char p_LclNodePortPt[6];

                if( UdpGetLclAddr( g_AudioInfoPt->m_UdpSoktPt, NULL, p_LclNodeAddrPt, p_LclNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) //如果获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。
                {
                    VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已监听的本端UDP协议套接字绑定的本地节点地址和端口失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto out;
                }

				VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化已监听的本端UDP协议套接字[%s:%s]成功。", p_LclNodeAddrPt, p_LclNodePortPt );
				LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
            else //如果创建并初始化已监听的本端UDP协议套接字失败。
            {
                VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化已监听的本端UDP协议套接字失败。原因：" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            char p_RmtNodeAddrPt[INET6_ADDRSTRLEN];
			char p_RmtNodePortPt[6];

            if( UdpConnect( g_AudioInfoPt->m_UdpSoktPt, 4, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
            {
				VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrPt );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            if( UdpGetRmtAddr( g_AudioInfoPt->m_UdpSoktPt, NULL, p_RmtNodeAddrPt, p_RmtNodePortPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
            {
                UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL );
                VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "获取已连接的本端UDP协议套接字连接的远端UDP协议套接字绑定的远程节点地址和端口失败。原因：" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                goto out;
            }

            int p_ReSendTimes = 1;
            while( true ) //循环连接已监听的远端UDP协议套接字。
            {
                g_AudioInfoPt->m_TmpBytePt[0] = 0x00; //设置连接请求包。
                if( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                {
                    UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL );
                    VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
                }

                while( true ) //循环接收连接请求包。
                {
                    if( UdpRecvPkt( g_AudioInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 1000, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                    {
                        if( p_TmpSz != -1 ) //如果用已连接的本端UDP协议套接字开始接收远端UDP协议套接字发送的一个数据包成功。
                        {
                            if( ( p_TmpSz == 1 ) && ( g_AudioInfoPt->m_TmpBytePt[0] == 0x00 ) ) //如果是连接请求包。
                            {
                                g_AudioInfoPt->m_TmpBytePt[0] = 0x02; //设置连接应答包。
                                if( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 )
                                {
                                    UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL );
                                    VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]失败。原因：", p_RmtNodeAddrPt, p_RmtNodePortPt );
									LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
									{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
									goto out;
                                }

								VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
								LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
								{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
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
                                UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL );
                                VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
								LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
								{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
								goto out;
                            }
                        }
                    }
                    else
                    {
                        UdpDisconnect( g_AudioInfoPt->m_UdpSoktPt, NULL );
                        VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已监听的本端UDP协议套接字连接已监听的远端UDP协议套接字[%s:%s]成功。", p_RmtNodeAddrPt, p_RmtNodePortPt );
						LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
                    }
                }
				UdpClntReRecv:;
            }
			UdpClntReSend:;
        }
    }

	if( MutexLockInit( &g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) //如果初始化接收输出帧互斥锁失败。
	{
		LOGI( "创建并初始化接收输出帧互斥锁成功。" );
	}
	else
	{
		VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收输出帧互斥锁失败。原因：" );
		LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	    goto out;
	}
	switch( g_AudioInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
	{
	    case 0: //如果使用链表。
	    {
			if( VarLenLnkLstInit( &g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, LNKLST_BUF_AUTO_ADJ_METH_FREENUMBER, AudioProcThreadPt->m_FrameLen * 2, AudioProcThreadPt->m_FrameLen * 2, SIZE_MAX, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
			{
				LOGI( "创建并初始化接收输出帧链表成功。" );
			}
			else
			{
				VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "创建并初始化接收输出帧链表失败。原因：" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
	        break;
	    }
	    case 1: //如果使用自适应抖动缓冲器。
	    {
			if( AjbInit( &g_AudioInfoPt->m_AjbPt, AudioProcThreadPt->m_SamplingRate, AudioProcThreadPt->m_FrameLen, 1, 1, 0, g_AudioInfoPt->m_AjbMinNeedBufFrameCnt, g_AudioInfoPt->m_AjbMaxNeedBufFrameCnt, g_AudioInfoPt->m_AjbAdaptSensitivity ) == 0 )
			{
				LOGI( "创建并初始化自适应抖动缓冲器成功。" );
			}
			else
			{
				VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "创建并初始化自适应抖动缓冲器失败。" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
	            goto out;
	        }
	        break;
	    }
	}

	FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间为当前时间。
	g_AudioInfoPt->m_LastPktRecvTime = g_AudioInfoPt->m_LastPktSendTime; //设置最后一个数据包的接收时间为当前时间。

	g_AudioInfoPt->m_LastSendInputFrameIsAct = 0; //设置最后发送的一个音频帧为无语音活动。
	g_AudioInfoPt->m_LastSendInputFrameIsRecv = 1; //设置最后一个发送的输入帧远端已经接收到。
	g_AudioInfoPt->m_SendInputFrameTimeStamp = 0 - 1; //设置发送输入帧的时间戳为0的前一个，因为第一次发送输入帧时会递增一个步进。
	g_AudioInfoPt->m_RecvOutputFrameTimeStamp = 0; //设置接收输出帧的时间戳为0。
	
	VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "开始进行音频对讲。" );
	LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
	{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

	p_Result = 0; //设置本函数执行成功。

	out:
	if( p_Result != 0 ) //如果本函数执行失败。
	{
		
	}
    return p_Result;
}

//用户定义的处理函数，在本线程运行时每隔1毫秒就调用一次，返回值表示是否成功，为0表示成功，为非0表示失败。
int __cdecl MyAudioProcThreadUserProcess( AudioProcThread * AudioProcThreadPt )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	size_t p_TmpSz;
	uint64_t p_TmpUint64;

    //接收远端发送过来的一个数据包。
	if( ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntRecvPkt( g_AudioInfoPt->m_TcpClntSoktPt, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
        ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpRecvPkt( g_AudioInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
    {
		if( p_TmpSz != -1 ) //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包成功。
		{
			FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

			if( p_TmpSz == 0 ) //如果数据包的数据长度为0。
			{
				LOGFE( "接收到数据包的数据长度为%" PRIuPTR "，表示没有数据，无法继续接收。", p_TmpSz );
                goto out;
			}
			else if( g_AudioInfoPt->m_TmpBytePt[0] == 0x00 ) //如果是心跳包。
            {
                if( p_TmpSz > 1 ) //如果心跳包的数据长度大于1。
                {
                    LOGFE( "接收到心跳包的数据长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                LOGI( "接收到一个心跳包。" );
            }
			else if( g_AudioInfoPt->m_TmpBytePt[0] == 0x01 ) //如果是输出帧包。
			{
				if( p_TmpSz < 1 + 4 ) //如果输出帧包的数据长度小于1 + 4，表示没有时间戳。
                {
                    LOGFE( "接收到输出帧包的数据长度为%" PRIuPTR "小于1 + 4，表示没有时间戳，无法继续接收。", p_TmpSz );
                    goto out;
                }

                if( ( p_TmpSz > 1 + 4 ) && ( AudioProcThreadPt->m_UseWhatCodec == 0 ) && ( p_TmpSz != 1 + 4 + AudioProcThreadPt->m_FrameLen * 2 ) ) //如果该输出帧为有语音活动，且使用了PCM原始数据，但接收到的PCM格式输出帧的数据长度与帧的数据长度不同。
                {
                    LOGE( "接收到的PCM格式输出帧的数据长度与帧的数据长度不同，无法继续接收。" );
                    goto out;
                }
				
				//读取时间戳。
				g_AudioInfoPt->m_RecvOutputFrameTimeStamp = ( g_AudioInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_AudioInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_AudioInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_AudioInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );
				
				//将输出帧放入链表或自适应抖动缓冲器。
				switch( g_AudioInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
				{
					case 0: //如果使用链表。
					{
						if( p_TmpSz > 1 + 4 ) //如果该输出帧为有语音活动。
						{
							MutexLockLock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
							if( VarLenLnkLstPutTail( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, g_AudioInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
							{
								LOGFI( "接收到一个输出帧包，并放入链表成功。时间戳：%d，总长度：%" PRIuPTR "。", g_AudioInfoPt->m_RecvOutputFrameTimeStamp, p_TmpSz );
							}
							else
							{
								LOGFE( "接收一个输出帧并放入链表失败。原因：%s", AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
								MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
								goto out;
							}
							MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
						}
						break;
					}
					case 1: //如果使用自适应抖动缓冲器。
					{
						MutexLockLock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
						AjbPutOneFrame( g_AudioInfoPt->m_AjbPt, g_AudioInfoPt->m_RecvOutputFrameTimeStamp, g_AudioInfoPt->m_TmpBytePt + 1 + 4, p_TmpSz - 1 - 4 );
						
						LOGFI( "接收一个到输出帧包，并放入自适应抖动缓冲器成功。时间戳：%d，总长度：%" PRIuPTR "。", g_AudioInfoPt->m_RecvOutputFrameTimeStamp, p_TmpSz );
						
						int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
						int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
						int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
						int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
						int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
						int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
						AjbGetBufFrameCnt( g_AudioInfoPt->m_AjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
						LOGFI( "自适应抖动缓冲器：有活动帧：%" PRIi32 "，无活动帧：%" PRIi32 "，帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufActFrameCnt, p_CurHaveBufInactFrameCnt, p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );
						
						MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
						break;
					}
				}

                if( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( p_TmpSz == 1 + 4 ) ) //如果是使用UDP协议，且本输出帧为无语音活动。
                {
                    //设置输出帧应答包。
                    g_AudioInfoPt->m_TmpBytePt[0] = 0x02;
                    //设置时间戳。
                    g_AudioInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_AudioInfoPt->m_RecvOutputFrameTimeStamp & 0xFF );
                    g_AudioInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_AudioInfoPt->m_RecvOutputFrameTimeStamp & 0xFF00 ) >> 8 );
                    g_AudioInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_AudioInfoPt->m_RecvOutputFrameTimeStamp & 0xFF0000 ) >> 16 );
                    g_AudioInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_AudioInfoPt->m_RecvOutputFrameTimeStamp & 0xFF000000 ) >> 24 );

                    if( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1 + 4, ( short ) 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                    {
                        FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                        LOGFI( "发送一个输出帧应答包成功。时间戳：%" PRIu32 "，总长度：%d。", g_AudioInfoPt->m_RecvOutputFrameTimeStamp, 1 + 4 );
                    }
                    else
                    {
						VarStrFmtIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个输出帧应答包失败。原因：" );
						LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
						goto out;
                    }
                }
			}
			else if( g_AudioInfoPt->m_TmpBytePt[0] == 0x02 ) //如果是连接应答包或输入输出帧应答包。
            {
                if( p_TmpSz == 1 ) //如果退出包的数据长度等于1，表示是连接应答包，就不管。
                {

                }
                else //如果退出包的数据长度大于1，表示是输入输出帧应答包。
                {
                    if( p_TmpSz != 1 + 4 )
                    {
                        LOGFE( "接收到输入输出帧应答包的数据长度为%" PRIuPTR "不等于1 + 4，表示格式不正确，无法继续接收。", p_TmpSz );
                        goto out;
                    }

                    //读取时间戳。
                    g_AudioInfoPt->m_RecvOutputFrameTimeStamp = ( g_AudioInfoPt->m_TmpBytePt[1] & 0xFF ) + ( ( g_AudioInfoPt->m_TmpBytePt[2] & 0xFF ) << 8 ) + ( ( g_AudioInfoPt->m_TmpBytePt[3] & 0xFF ) << 16 ) + ( ( g_AudioInfoPt->m_TmpBytePt[4] & 0xFF ) << 24 );

                    LOGFI( "接收到一个输入输出帧应答包。时间戳：%" PRIi32 "，总长度：%" PRIuPTR "。", g_AudioInfoPt->m_RecvOutputFrameTimeStamp, p_TmpSz );

                    //设置最后一个发送的输入帧远端是否接收到。
                    if( g_AudioInfoPt->m_SendInputFrameTimeStamp == g_AudioInfoPt->m_RecvOutputFrameTimeStamp ) g_AudioInfoPt->m_LastSendInputFrameIsRecv = 1;
                }
            }
            else if( g_AudioInfoPt->m_TmpBytePt[0] == 0x03 ) //如果是退出包。
            {
                if( p_TmpSz > 1 ) //如果退出包的数据长度大于1。
                {
                    LOGFE( "接收到退出包的数据长度为%" PRIuPTR "大于1，表示还有其他数据，无法继续接收。", p_TmpSz );
                    goto out;
                }

                g_AudioInfoPt->m_IsRecvExitPkt = 1; //设置已经接收到退出包。
                AudioProcThreadRequireExit( AudioProcThreadPt, 1, 0, NULL ); //请求退出。

				VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
				LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            }
		}
		else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包超时。
        {
			
        }
	}
	else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。
	{
		VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
		LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
		goto out;
	}

    //发送心跳包。
	FuncGetTimeAsMsec( &p_TmpUint64 );
    if( p_TmpUint64 - g_AudioInfoPt->m_LastPktSendTime >= 100 ) //如果超过100毫秒没有发送任何数据包，就发送一个心跳包。
    {
		g_AudioInfoPt->m_TmpBytePt[0] = 0x00; //设置心跳包。
        if( ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_AudioInfoPt->m_TcpClntSoktPt, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
            ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
        {
			g_AudioInfoPt->m_LastPktSendTime = p_TmpUint64; //记录最后一个数据包的发送时间。
			LOGI( "发送一个心跳包成功。" );
		}
		else
		{
			VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个心跳包失败。原因：" );
			LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
			goto out;
		}
    }

    //判断套接字连接是否中断。
	FuncGetTimeAsMsec( &p_TmpUint64 );
    if( p_TmpUint64 - g_AudioInfoPt->m_LastPktRecvTime > 2000 ) //如果超过2000毫秒没有接收任何数据包，就判定连接已经断开了。
    {
		VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "超过2000毫秒没有接收任何数据包，判定连接已经断开了。" );
		LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
		goto out;
    }

    p_Result = 0; //设置本函数执行成功。

	out:
	return p_Result;
}

//用户定义的销毁函数，在本线程退出时调用一次。
void __cdecl MyAudioProcThreadUserDestroy( AudioProcThread * AudioProcThreadPt )
{
	size_t p_TmpSz;
	
	if( ( AudioProcThreadPt->m_ExitFlag == 1 ) && ( ( g_AudioInfoPt->m_TcpClntSoktPt != NULL ) || ( ( g_AudioInfoPt->m_UdpSoktPt != NULL ) && ( UdpGetRmtAddr( g_AudioInfoPt->m_UdpSoktPt, NULL, NULL, NULL, NULL ) == 0 ) ) ) ) //如果本线程接收到退出请求，且本端TCP协议客户端套接字类对象不为空或本端UDP协议套接字类对象不为空且已连接远端。
    {
        //循环发送退出包。
        g_AudioInfoPt->m_TmpBytePt[0] = 0x03; //设置退出包。
		for( int p_SendTimes = ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) ? 1 : 5; p_SendTimes > 0; p_SendTimes-- )
        {
            if( ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_AudioInfoPt->m_TcpClntSoktPt, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) ) ||
                ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) != 0 ) ) )
            {
                VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个退出包失败。原因：" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto SendExitPkt;
            }
        }

        FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktSendTime ); //记录最后一个数据包的发送时间。

		VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "发送一个退出包成功。" );
		LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

		//接收退出包。
        if( g_AudioInfoPt->m_IsRecvExitPkt == 0 ) //如果没有接收到退出包。
        {
            while( true ) //循环接收退出包。
            {
                if( ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntRecvPkt( g_AudioInfoPt->m_TcpClntSoktPt, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 5000, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                    ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpRecvPkt( g_AudioInfoPt->m_UdpSoktPt, NULL, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, g_AudioInfoPt->m_TmpByteSz, &p_TmpSz, 5000, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
                {
                    if( p_TmpSz != -1 ) //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包成功。
                    {
						FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktRecvTime ); //记录最后一个数据包的接收时间。

                        if( ( p_TmpSz == 1 ) && ( g_AudioInfoPt->m_TmpBytePt[0] == 0x03 ) ) //如果是退出包。
                        {
                            VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "接收到一个退出包。" );
							LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
							{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                            goto SendExitPkt;
                        }
						else //如果是其他包，继续接收。
                        {

                        }
                    }
                    else //如果用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包超时。
                    {
                        VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
						LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
						{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                        goto SendExitPkt;
                    }
                }
                else //如果用已连接的本端TCP协议客户端套接字开始接收连接的远端TCP协议客户端套接字发送的一个数据包失败。
                {
                    VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "用已连接的本端套接字开始接收连接的远端套接字发送的一个数据包失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
                    goto SendExitPkt;
                }
            }
        }
    }
	SendExitPkt:;

    //销毁本端TCP协议服务端套接字。
    if( g_AudioInfoPt->m_TcpSrvrSoktPt != NULL )
    {
        TcpSrvrDestroy( g_AudioInfoPt->m_TcpSrvrSoktPt, NULL ); //关闭并销毁已创建的本端TCP协议服务端套接字。
        g_AudioInfoPt->m_TcpSrvrSoktPt = NULL;

		VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议服务端套接字成功。" );
		LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁本端TCP协议客户端套接字。
    if( g_AudioInfoPt->m_TcpClntSoktPt != NULL )
    {
        TcpClntDestroy( g_AudioInfoPt->m_TcpClntSoktPt, ( uint16_t ) -1, NULL ); //关闭并销毁已创建的本端TCP协议客户端套接字。
        g_AudioInfoPt->m_TcpClntSoktPt = NULL;

		VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端TCP协议客户端套接字成功。" );
		LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }
	
    //销毁本端UDP协议套接字。
    if( g_AudioInfoPt->m_UdpSoktPt != NULL )
    {
        UdpDestroy( g_AudioInfoPt->m_UdpSoktPt, NULL ); //关闭并销毁已创建的本端UDP协议套接字。
        g_AudioInfoPt->m_TcpClntSoktPt = NULL;

		VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "关闭并销毁已创建的本端UDP协议套接字成功。" );
		LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
		{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
    }

    //销毁接收音频输出帧的链表类对象。
    if( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt != NULL )
    {
		VarLenLnkLstDestroy( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, NULL );
        g_AudioInfoPt->m_RecvOutputFrameLnkLstPt = NULL;

		LOGI( "销毁接收输出帧链表成功。" );
    }

    //销毁自适应抖动缓冲器类对象。
    if( g_AudioInfoPt->m_AjbPt != NULL )
    {
        AjbDestroy( g_AudioInfoPt->m_AjbPt );
        g_AudioInfoPt->m_AjbPt = NULL;

		LOGI( "销毁自适应抖动缓冲器成功。" );
    }

	//销毁接收输出帧互斥锁。
	if( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt != NULL )
	{
		MutexLockDestroy( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, NULL );
		g_AudioInfoPt->m_RecvOutputFrameMutexLockPt = NULL;

		LOGI( "销毁接收输出帧互斥锁成功。" );
	}

    if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
    {
        if( ( AudioProcThreadPt->m_ExitFlag == 1 ) && ( g_AudioInfoPt->m_IsRecvExitPkt == 1 ) ) //如果本线程接收到退出请求，且接收到了退出包。
        {
			VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程接收到退出请求，且接收到了退出包，表示是远端TCP协议客户端套接字主动退出，本线程重新初始化来继续保持监听。" );
			LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			AudioProcThreadRequireExit( AudioProcThreadPt, 2, 0, NULL ); //请求重启。
        }
        else if( ( AudioProcThreadPt->m_ExitFlag == 0 ) && ( AudioProcThreadPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
			VarStrCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "由于是创建服务端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来继续保持监听。" );
			LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
			AudioProcThreadRequireExit( AudioProcThreadPt, 2, 0, NULL ); //请求重启。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_MYAUDIOPROCTHREADEND, 0, 0 ); //向主界面发送音频处理线程退出的消息。
        }
    }
    else if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 0 ) //如果是创建客户端。
    {
        if( ( AudioProcThreadPt->m_ExitFlag == 0 ) && ( AudioProcThreadPt->m_ExitCode == -2 ) ) //如果本线程没收到退出请求，且退出代码为处理失败。
        {
            g_AudioInfoPt->m_TcpClntSoktReInitTimes++; //递增本端TCP协议客户端套接字类对象重新初始化的次数。
			VarStrFmtCpy( AudioProcThreadPt->m_ErrInfoVarStrPt, "由于是创建客户端，且本线程没收到退出请求，且退出码为处理失败，表示是处理失败或连接异常断开，本线程重新初始化来重连服务端。" );
			LOGI( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
			{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
            
            AudioProcThreadRequireExit( AudioProcThreadPt, 2, 0, NULL ); //请求重启。
        }
		else //其他情况，本线程直接退出。
        {
            PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_MYAUDIOPROCTHREADEND, 0, 0 ); //向主界面发送音频处理线程退出的消息。
        }
    }
}

//用户定义的读取输入帧函数，在读取到一个输入帧并处理完后回调一次，为0表示成功，为非0表示失败。
int __cdecl MyAudioProcThreadUserReadInputFrame( AudioProcThread * AudioProcThreadPt, int16_t * PcmInputFramePt, int16_t * PcmResultFramePt, int32_t VoiceActSts, int8_t * SpeexInputFramePt, size_t SpeexInputFrameLen, int32_t SpeexInputFrameIsNeedTrans )
{
    int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	int p_TmpInt = 0;

    //发送输入帧。
    {
        if( VoiceActSts == 1 ) //如果该输入帧为有语音活动。
        {
            switch( AudioProcThreadPt->m_UseWhatCodec ) //使用什么编解码器。
            {
                case 0: //如果使用PCM原始数据。
                {
					memcpy( g_AudioInfoPt->m_TmpBytePt + 1 + 4, PcmResultFramePt, AudioProcThreadPt->m_FrameLen * 2 );
					
                    p_TmpInt = 1 + 4 + AudioProcThreadPt->m_FrameLen * 2; //数据包长度 = 格式标记 + 时间戳长度 + PCM格式输入帧长度。

                    break;
                }
                case 1: //如果使用Speex编解码器。
                {
                    if( SpeexInputFrameIsNeedTrans == 1 ) //如果本Speex格式音频输入数据帧需要传输。
                    {
                        memcpy( g_AudioInfoPt->m_TmpBytePt + 1 + 4, SpeexInputFramePt, SpeexInputFrameLen );

                        p_TmpInt = 1 + 4 + SpeexInputFrameLen; //数据包长度 = 格式标记 + 时间戳长度 + Speex格式输入帧长度。
                    }
                    else //如果本Speex格式音频输入数据帧不需要传输。
                    {
                        p_TmpInt = 1 + 4; //数据包长度 = 格式标记 + 时间戳长度。
                    }

                    break;
                }
            }
        }
        else //如果本音频输入数据帧为无语音活动。
        {
            p_TmpInt = 1 + 4; //数据包长度 = 格式标记 + 时间戳长度。
        }

        if( ( p_TmpInt != 1 + 4 ) || //如果本音频输入数据帧为有语音活动，就发送。
            ( ( p_TmpInt == 1 + 4 ) && ( g_AudioInfoPt->m_LastSendInputFrameIsAct != 0 ) ) ) //如果本音频输入数据帧为无语音活动，但最后一个发送的输入帧为有语音活动，就发送。
        {
			g_AudioInfoPt->m_SendInputFrameTimeStamp += 1; //时间戳递增一个步进。

			//设置输入帧包。
            g_AudioInfoPt->m_TmpBytePt[0] = 0x01;
            //设置时间戳。
            g_AudioInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF );
            g_AudioInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF00 ) >> 8 );
            g_AudioInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF0000 ) >> 16 );
            g_AudioInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF000000 ) >> 24 );

            if( ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 0 ) && ( TcpClntSendPkt( g_AudioInfoPt->m_TcpClntSoktPt, g_AudioInfoPt->m_TmpBytePt, p_TmpInt, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) ||
                ( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, p_TmpInt, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 ) ) )
            {
				FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
				LOGFI( "发送一个输入帧成功。时间戳：%d，总长度：%d。", g_AudioInfoPt->m_SendInputFrameTimeStamp, p_TmpInt );
			}
			else
			{
				VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "发送一个输入帧失败。原因：" );
				LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto out;
			}
			
            if( p_TmpInt != 1 + 4 ) //如果本音频输入数据帧为有语音活动。
            {
                g_AudioInfoPt->m_LastSendInputFrameIsAct = 1; //设置最后一个发送的输入帧有语音活动。
                g_AudioInfoPt->m_LastSendInputFrameIsRecv = 1; //设置最后一个发送的输入帧远端已经接收到。
            }
            else if( ( ( p_TmpInt == 1 + 4 ) && ( g_AudioInfoPt->m_LastSendInputFrameIsAct != 0 ) ) ) //如果本音频输入数据帧为无语音活动，但最后一个发送的输入帧为有语音活动。
            {
                g_AudioInfoPt->m_LastSendInputFrameIsAct = 0; //设置最后一个发送的输入帧无语音活动。
                g_AudioInfoPt->m_LastSendInputFrameIsRecv = 0; //设置最后一个发送的输入帧远端没有接收到。
            }
        }
        else
        {
            LOGI( "本输入帧为无语音活动，且最后发送的一个输入帧为无语音活动，无需发送。" );

			if( ( g_AudioInfoPt->m_UseWhatXfrPrtcl == 1 ) && ( g_AudioInfoPt->m_LastSendInputFrameIsAct == 0 ) && ( g_AudioInfoPt->m_LastSendInputFrameIsRecv == 0 ) ) //如果是使用UDP协议，且最后一个发送的输入帧为无语音活动，且最后一个发送的输入帧远端没有接收到。
            {
                //设置输入帧包。
                g_AudioInfoPt->m_TmpBytePt[0] = 0x01;
                //设置时间戳。
                g_AudioInfoPt->m_TmpBytePt[1] = ( int8_t ) ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF );
                g_AudioInfoPt->m_TmpBytePt[2] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF00 ) >> 8 );
                g_AudioInfoPt->m_TmpBytePt[3] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF0000 ) >> 16 );
                g_AudioInfoPt->m_TmpBytePt[4] = ( int8_t ) ( ( g_AudioInfoPt->m_SendInputFrameTimeStamp & 0xFF000000 ) >> 24 );

                if( UdpSendPkt( g_AudioInfoPt->m_UdpSoktPt, 4, NULL, NULL, g_AudioInfoPt->m_TmpBytePt, 1 + 4, 0, AudioProcThreadPt->m_ErrInfoVarStrPt ) == 0 )
                {
                    FuncGetTimeAsMsec( &g_AudioInfoPt->m_LastPktSendTime ); //设置最后一个数据包的发送时间。
                    LOGFI( "重新发送一个无语音活动输入帧包成功。时间戳：%" PRIi32 "，总长度：%d。", 1 + 4 );
                }
                else
                {
                    VarStrIns( AudioProcThreadPt->m_ErrInfoVarStrPt, 0, "重新发送一个无语音活动输入帧包失败。原因：" );
					LOGE( AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt );
					{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, AudioProcThreadPt->m_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_AudioInfoPt->m_MainWindowHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
					goto out;
                }
            }
        }

        //设置最后一个发送的输入帧有无语音活动。
        if( p_TmpInt != 1 + 4 ) g_AudioInfoPt->m_LastSendInputFrameIsAct = 1;
        else g_AudioInfoPt->m_LastSendInputFrameIsAct = 0;
    }

    p_Result = 0; //设置本函数执行成功。

	out:
    return p_Result;
}

//用户定义的写入输出帧函数，在需要写入一个输出帧时回调一次。注意：本函数不是在音频处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致回音消除失败。
void __cdecl MyAudioProcThreadUserWriteOutputFrame( AudioProcThread * AudioProcThreadPt, int16_t * PcmOutputFramePt, int8_t * SpeexOutputFramePt, size_t * SpeexOutputFrameLenPt )
{
	int32_t m_TmpInt32;
	size_t m_TmpSz;

    //从链表或自适应抖动缓冲器取出一个输出帧。
    switch( g_AudioInfoPt->m_UseWhatRecvOutputFrame ) //使用什么接收输出帧。
    {
        case 0: //如果使用链表。
        {
            switch( AudioProcThreadPt->m_UseWhatCodec ) //使用什么编解码器。
            {
                case 0: //如果使用PCM原始数据。
                {
					//从链表取出一个输出帧。
					MutexLockLock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
					VarLenLnkLstGetTotal( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, &m_TmpSz, AudioProcThreadPt->m_ErrInfoVarStrPt );
					if( m_TmpSz != 0 )
					{
						VarLenLnkLstGetHead( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, NULL, PcmOutputFramePt, AudioProcThreadPt->m_FrameLen * 2, NULL, &m_TmpSz, AudioProcThreadPt->m_ErrInfoVarStrPt ); //获取接收输出帧链表的第一个输出帧。
						VarLenLnkLstDelHead( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, AudioProcThreadPt->m_ErrInfoVarStrPt ); //删除接收输出帧链表的第一个输出帧。
					}
					MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );

                    if( m_TmpSz != 0 ) //如果接收输出帧链表的第一个输出帧为有语音活动。
                    {
                        LOGFI( "从接收输出帧链表取出一个有语音活动的PCM格式输出帧，帧的数据长度：%" PRIuPTR "。", m_TmpSz );
                    }
                    else //如果接收音频输出数据帧的链表为空，或第一个音频输出数据帧为无语音活动。
                    {
						memset( PcmOutputFramePt, 0, AudioProcThreadPt->m_FrameLen * 2 );

                        LOGI( "从接收输出帧链表取出一个无语音活动的PCM格式输出帧。" );
                    }

                    break;
                }
                case 1: //如果使用Speex编解码器。
                {
					//从链表取出一个输出帧。
					MutexLockLock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
					VarLenLnkLstGetTotal( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, &m_TmpSz, AudioProcThreadPt->m_ErrInfoVarStrPt );
					if( m_TmpSz != 0 )
					{
						VarLenLnkLstGetHead( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, NULL, SpeexOutputFramePt, 960, NULL, &m_TmpSz, AudioProcThreadPt->m_ErrInfoVarStrPt );
						VarLenLnkLstDelHead( g_AudioInfoPt->m_RecvOutputFrameLnkLstPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
					}
					MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );

                    if( m_TmpSz != 0 ) //如果接收输出帧链表的第一个输出帧为有语音活动。
                    {
                        *SpeexOutputFrameLenPt = m_TmpSz;

                        LOGFI( "从接收输出帧链表取出一个有语音活动的Speex格式输出帧，帧的数据长度：%" PRIuPTR "。", m_TmpSz );
                    }
                    else //如果接收音频输出数据帧的链表为空，或第一个音频输出数据帧为无语音活动。
                    {
                        *SpeexOutputFrameLenPt = 0;

                        LOGI( "从接收输出帧链表取出一个无语音活动的Speex格式输出帧。" );
                    }

                    break;
                }
            }

            break;
        }
        case 1: //如果使用自适应抖动缓冲器。
        {
			uint32_t p_OutputFrameTimeStamp; //存放输出帧的时间戳。

			MutexLockLock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
            switch( AudioProcThreadPt->m_UseWhatCodec ) //使用什么编解码器。
            {
                case 0: //如果使用PCM原始数据。
                {
					//从自适应抖动缓冲器取出一个输出帧。
					AjbGetOneFrame( g_AudioInfoPt->m_AjbPt, &p_OutputFrameTimeStamp, ( int8_t * )PcmOutputFramePt, AudioProcThreadPt->m_FrameLen * 2, &m_TmpSz );
					
					if( m_TmpSz == 0 ) //如果输出帧为无语音活动。
                    {
                        memset( PcmOutputFramePt, 0, AudioProcThreadPt->m_FrameLen * 2 );

                        LOGFI( "从自适应抖动缓冲器取出一个无语音活动的PCM格式输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }
					else if( m_TmpSz == SIZE_MAX ) //如果输出帧为丢失。
                    {
                        memset( PcmOutputFramePt, 0, AudioProcThreadPt->m_FrameLen * 2 );

                        LOGFI( "从自适应抖动缓冲器取出一个丢失的PCM格式输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }
                    else //如果输出帧为有语音活动。
                    {
                        LOGFI( "从自适应抖动缓冲器取出一个有语音活动的PCM格式输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }

                    break;
                }
                case 1: //如果使用Speex编解码器。
                {
					//从自适应抖动缓冲器取出一个输出帧。
					AjbGetOneFrame( g_AudioInfoPt->m_AjbPt, &p_OutputFrameTimeStamp, ( int8_t * )SpeexOutputFramePt, 640, &m_TmpSz );

                    *SpeexOutputFrameLenPt = m_TmpSz;

					if( m_TmpSz == 0 ) //如果输出帧为无语音活动。
                    {
                        LOGFI( "从自适应抖动缓冲器取出一个无语音活动的Speex格式输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }
					else if( m_TmpSz == SIZE_MAX ) //如果输出帧为丢失。
                    {
                        LOGFI( "从自适应抖动缓冲器取出一个丢失的Speex格式输出帧。时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }
					else //如果输出帧为有语音活动。
                    {
                        LOGFI( "从自适应抖动缓冲器取出一个有语音活动的Speex格式输出帧，时间戳：%" PRIu32 "，数据长度：%" PRIuPTR "。", p_OutputFrameTimeStamp, m_TmpSz );
                    }

                    break;
                }
            }

			int32_t p_CurHaveBufActFrameCnt; //存放当前已缓冲有活动帧的数量。
            int32_t p_CurHaveBufInactFrameCnt; //存放当前已缓冲无活动帧的数量。
            int32_t p_CurHaveBufFrameCnt; //存放当前已缓冲帧的数量。
            int32_t p_MinNeedBufFrameCnt; //存放最小需缓冲帧的数量。
            int32_t p_MaxNeedBufFrameCnt; //存放最大需缓冲帧的数量。
            int32_t p_CurNeedBufFrameCnt; //存放当前需缓冲帧的数量。
			AjbGetBufFrameCnt( g_AudioInfoPt->m_AjbPt, &p_CurHaveBufActFrameCnt, &p_CurHaveBufInactFrameCnt, &p_CurHaveBufFrameCnt, &p_MinNeedBufFrameCnt, &p_MaxNeedBufFrameCnt, &p_CurNeedBufFrameCnt );
			LOGFI( "自适应抖动缓冲器：有活动帧：%" PRIi32 "，无活动帧：%" PRIi32 "，帧：%" PRIi32 "，最小需帧：%" PRIi32 "，最大需帧：%" PRIi32 "，当前需帧：%" PRIi32 "。", p_CurHaveBufActFrameCnt, p_CurHaveBufInactFrameCnt, p_CurHaveBufFrameCnt, p_MinNeedBufFrameCnt, p_MaxNeedBufFrameCnt, p_CurNeedBufFrameCnt );
			
			MutexLockUnlock( g_AudioInfoPt->m_RecvOutputFrameMutexLockPt, AudioProcThreadPt->m_ErrInfoVarStrPt );
            break;
        }
    }
}

//用户定义的获取PCM格式输出帧函数，在解码完一个输出帧时回调一次。注意：本函数不是在音频处理线程中执行的，而是在音频输出线程中执行的，所以本函数应尽量在一瞬间完成执行，否则会导致音频输入输出帧不同步，从而导致回音消除失败。
void __cdecl MyAudioProcThreadUserGetPcmOutputFrame( AudioProcThread * AudioProcThreadPt, int16_t * PcmOutputFramePt )
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
	//_CrtSetBreakAlloc( 82 );

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

	//初始化窗口。
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_MAIN ), NULL, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SETTING ), g_MainWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SPEEXAEC ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_WEBRTCAECM ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_WEBRTCAEC ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SPEEXWEBRTCAEC ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SPEEXPPROCNS ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_WEBRTCNSX ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_WEBRTCNS ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SPEEXPPROCOTHER ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_SPEEXCODEC ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_AJB ), g_SettingWndHdl, About );
	CreateDialog( hInst, MAKEINTRESOURCE( IDD_README ), g_MainWndHdl, About );
	ShowWindow( g_MainWndHdl, SW_SHOW );

    MSG msg;

    // 主消息循环:
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		if( !TranslateAccelerator( msg.hwnd, NULL, &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	LogDestroy(); //销毁日志。
	VarStrDestroy( g_ErrInfoVarStrPt ); //销毁错误信息动态字符串。
	
	_CrtDumpMemoryLeaks();
	return (int) msg.wParam;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int p_Result = -1; //存放本函数执行结果的值，为0表示成功，为非0表示失败。
	
	UNREFERENCED_PARAMETER( lParam );
	switch( message )
	{
		case WM_INITDIALOG: //窗口初始化消息。
		{
			if( g_MainWndHdl == NULL ) //如果是主界面。
			{
				g_MainWndHdl = hDlg; //设置主界面窗口的句柄。

				//设置窗口图标。
				#if( ( defined __X86__ ) )
				SetClassLong( hDlg, GCL_HICONSM, ( LONG )LoadIcon( hInst, MAKEINTRESOURCE( IDI_WINDOWSAUDIOTALKBACKDEMOPROGRAM ) ) );
				#elif( ( defined __X64__ ) )
				SetClassLong( hDlg, GCLP_HICONSM, ( LONG )LoadIcon( hInst, MAKEINTRESOURCE( IDI_WINDOWSAUDIOTALKBACKDEMOPROGRAM ) ) );
				#endif

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

				//设置IP地址控件的内容。
				if( p_LclNodeAddrInfoListPt != NULL )
				{
					for( p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListPt; p_LclNodeAddrInfoListLoopPt != NULL; p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListLoopPt->ai_next )
					{
						VarStrFmtCpy( g_ErrInfoVarStrPt, "%uz8d.%uz8d.%uz8d.%uz8d", ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b1, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b2, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b3, ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr.S_un.S_un_b.s_b4 );
						SendMessage( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), CB_ADDSTRING, 0, ( LPARAM )g_ErrInfoVarStrPt->m_StrPt );
					}
					freeaddrinfo( p_LclNodeAddrInfoListPt ); //销毁本地节点地址信息结构体链表。
				}
				SendMessage( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), CB_ADDSTRING, 0, ( LPARAM )"0.0.0.0" );
				SendMessage( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), CB_ADDSTRING, 0, ( LPARAM )"127.0.0.1" );
				SendMessage( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), CB_SETCURSEL, 0, 0 ); //设置IP地址控件默认选择第一个IP地址。
				
				//设置端口控件的内容。
				SetWindowText( GetDlgItem( hDlg, IDC_PORTEDIT ), "12345" );

				//初始化音频输入设备和音频输出设备的控件。
				PostMessage( hDlg, WM_DEVICECHANGE, 0, 0 );

				if( g_ErrInfoVarStrPt->m_StrSz < MAX_PATH ) VarStrSetSz( g_ErrInfoVarStrPt, MAX_PATH );
				GetCurrentDirectory( g_ErrInfoVarStrPt->m_StrSz, g_ErrInfoVarStrPt->m_StrPt );
				VarStrIns( g_ErrInfoVarStrPt, 0, "可执行文件所在目录完整绝对路径：" );
				LOGI( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}

				p_Result = 0; // //设置本函数执行成功。

				outinit:
				return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
			}
			else if( g_SettingWndHdl == NULL ) //如果是设置界面。
			{
				g_SettingWndHdl = hDlg;

				CheckRadioButton( hDlg, IDC_USETCPPRTCLRADIOBTN, IDC_USEUDPPRTCLRADIOBTN, IDC_USEUDPPRTCLRADIOBTN );
				CheckRadioButton( hDlg, IDC_USESAMPLINGRATE8000RADIOBTN, IDC_USESAMPLINGRATE32000RADIOBTN, IDC_USESAMPLINGRATE16000RADIOBTN );
				CheckRadioButton( hDlg, IDC_USEFRAME10MSLENRADIOBTN, IDC_USEFRAME30MSLENRADIOBTN, IDC_USEFRAME20MSLENRADIOBTN );
				CheckDlgButton( hDlg, IDC_ISSAVESETTINGTOFILECHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_ISPRINTLOGCATCHECKBOX, BST_CHECKED );
				CheckRadioButton( hDlg, IDC_USENOAECRADIOBTN, IDC_USESPEEXWEBRTCAECRADIOBTN, IDC_USESPEEXWEBRTCAECRADIOBTN );
				CheckRadioButton( hDlg, IDC_USENONSRADIOBTN, IDC_USERNNOISERADIOBTN, IDC_USERNNOISERADIOBTN );
				CheckDlgButton( hDlg, IDC_ISUSESPEEXPPROCOTHERCHECKBOX, BST_CHECKED );
				CheckRadioButton( hDlg, IDC_USEPCMRADIOBTN, IDC_USEOPUSCODECRADIOBTN, IDC_USESPEEXCODECRADIOBTN );
				CheckRadioButton( hDlg, IDC_USELNKLSTRADIOBTN, IDC_USEAJBRADIOBTN, IDC_USEAJBRADIOBTN );
				CheckDlgButton( hDlg, IDC_ISSAVEAUDIOTOFILECHECKBOX, BST_CHECKED );
			}
			else if( g_SpeexAecSettingWndHdl == NULL ) //如果是Speex声学回音消除器设置界面。
			{
				g_SpeexAecSettingWndHdl = hDlg;

				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXAECFILTERLENEDIT ), "500" );
				CheckDlgButton( hDlg, IDC_SPEEXAECISUSERECCHECKBOX, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXAECECHOMULTIPLEEDIT ), "3.0" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXAECECHOCONTEDIT ), "0.65" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXAECECHOSUPESEDIT ), "-32768" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXAECECHOSUPESACTEDIT ), "-32768" );
				CheckDlgButton( hDlg, IDC_SPEEXAECISSAVEMEMFILECHECKBOX, BST_CHECKED );
			}
			else if( g_WebRtcAecmSettingWndHdl == NULL ) //如果是WebRtc定点版声学回音消除器设置界面。
			{
				g_WebRtcAecmSettingWndHdl = hDlg;

				CheckDlgButton( hDlg, IDC_WEBRTCAECMISUSECNGMODECHECKBOX, BST_UNCHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCAECMECHOMODEEDIT ), "4" );
				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCAECMDELAYEDIT ), "0" );
			}
			else if( g_WebRtcAecSettingWndHdl == NULL ) //如果是WebRtc浮点版声学回音消除器设置界面。
			{
				g_WebRtcAecSettingWndHdl = hDlg;

				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCAECECHOMODEEDIT ), "2" );
				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCAECDELAYEDIT ), "0" );
				CheckDlgButton( hDlg, IDC_WEBRTCAECISUSEDELAYAGNOSTICMODECHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_WEBRTCAECISUSEEXTDFILTERMODECHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_WEBRTCAECISUSEREFINEDFILTERADAPTAECMODECHECKBOX, BST_UNCHECKED );
				CheckDlgButton( hDlg, IDC_WEBRTCAECISUSEADAPTADJDELAYCHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_WEBRTCAECISSAVEMEMFILECHECKBOX, BST_CHECKED );
			}
			else if( g_SpeexWebRtcAecSettingWndHdl == NULL ) //如果是SpeexWebRtc三重声学回音消除器设置界面。
			{
				g_SpeexWebRtcAecSettingWndHdl = hDlg;

				CheckRadioButton( hDlg, IDC_SPEEXWEBRTCAECWORKMODESPEEXAECWEBRTCAECMRADIOBTN, IDC_SPEEXWEBRTCAECWORKMODESPEEXAECWEBRTCAECMWEBRTCAECRADIOBTN, IDC_SPEEXWEBRTCAECWORKMODESPEEXAECWEBRTCAECMWEBRTCAECRADIOBTN );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECFILTERLENEDIT ), "500" );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECISUSERECCHECKBOX, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECECHOMULTIPLEEDIT ), "3.0" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECECHOCONTEDIT ), "0.65" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECECHOSUPESEDIT ), "-32768" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSPEEXAECECHOSUPESACTEDIT ), "-32768" );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECMISUSECNGMODECHECKBOX, BST_UNCHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECMECHOMODEEDIT ), "4" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECMDELAYEDIT ), "0" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECECHOMODEEDIT ), "2" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECDELAYEDIT ), "0" );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEDELAYAGNOSTICMODECHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEEXTDFILTERMODECHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEREFINEDFILTERADAPTAECMODECHECKBOX, BST_UNCHECKED );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEADAPTADJDELAYCHECKBOX, BST_CHECKED );
				CheckDlgButton( hDlg, IDC_SPEEXWEBRTCAECISUSESAMEROOMAEC, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXWEBRTCAECSAMEROOMECHOMINDELAY ), "420" );
			}
			else if( g_SpeexPprocNsSettingWndHdl == NULL ) //如果是Speex预处理器的噪音抑制设置界面。
			{
				g_SpeexPprocNsSettingWndHdl = hDlg;

				CheckDlgButton( hDlg, IDC_SPEEXPPROCISUSENSCHECKBOX, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCNOISESUPESEDIT ), "-32768" );
				CheckDlgButton( hDlg, IDC_SPEEXPPROCISUSEDEREVERBCHECKBOX, BST_CHECKED );
			}
			else if( g_WebRtcNsxSettingWndHdl == NULL ) //如果是WebRtc定点版噪音抑制器设置界面。
			{
				g_WebRtcNsxSettingWndHdl = hDlg;

				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCNSXPOLICYMODE ), "3" );
			}
			else if( g_WebRtcNsSettingWndHdl == NULL ) //如果是WebRtc浮点版噪音抑制器设置界面。
			{
				g_WebRtcNsSettingWndHdl = hDlg;

				SetWindowText( GetDlgItem( hDlg, IDC_WEBRTCNSPOLICYMODE ), "3" );
			}
			else if( g_SpeexPprocOtherSettingWndHdl == NULL ) //如果是Speex预处理器的其他功能设置界面。
			{
				g_SpeexPprocOtherSettingWndHdl = hDlg;

				CheckDlgButton( hDlg, IDC_SPEEXPPROCISUSEVADCHECKBOX, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCVADPROBSTARTEDIT ), "95" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCVADPROBCONTEDIT ), "95" );
				CheckDlgButton( hDlg, IDC_SPEEXPPROCISUSEAGCCHECKBOX, BST_CHECKED );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCAGCLEVELEDIT ), "30000" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCAGCINCREMENTEDIT ), "10" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCAGCDECREMENTEDIT ), "-30000" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXPPROCAGCMAXGAINEDIT ), "25" );
			}
			else if( g_SpeexCodecSettingWndHdl == NULL ) //如果是Speex编解码器设置界面。
			{
				g_SpeexCodecSettingWndHdl = hDlg;

				CheckRadioButton( hDlg, IDC_SPEEXCODECENCODERUSECBRRADIOBTN, IDC_SPEEXCODECENCODERUSEVBRRADIOBTN, IDC_SPEEXCODECENCODERUSEVBRRADIOBTN );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXCODECENCODERQUALITYEDIT ), "10" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXCODECENCODERCOMPLEXITYEDIT ), "10" );
				SetWindowText( GetDlgItem( hDlg, IDC_SPEEXCODECENCODERPLCEXPECTEDLOSSRATEEDIT ), "100" );
				CheckDlgButton( hDlg, IDC_SPEEXCODECISUSEPERCEPTUALENHANCEMENTCHECKBOX, BST_CHECKED );
			}
			else if( g_AjbSettingWndHdl == NULL ) //如果是自适应抖动缓冲器设置界面。
			{
				g_AjbSettingWndHdl = hDlg;

				SetWindowText( GetDlgItem( hDlg, IDC_AJBMINNEEDBUFFRAMECNT ), "5" );
				SetWindowText( GetDlgItem( hDlg, IDC_AJBMAXNEEDBUFFRAMECNT ), "50" );
				SetWindowText( GetDlgItem( hDlg, IDC_AJBADAPTSENSITIVITY ), "1" );
			}
			else if( g_ReadMeWndHdl == NULL ) //如果是必读说明界面。
			{
				g_ReadMeWndHdl = hDlg; //设置必读说明界面窗口的句柄。

				//设置声学回音消除器对比图。
				{
					SetWindowLong( GetDlgItem( hDlg, IDC_AECCMP ), GWL_STYLE, GetWindowLong( GetDlgItem( hDlg, IDC_AECCMP ), GWL_STYLE ) | SS_REALSIZECONTROL );
					SendMessage( GetDlgItem( hDlg, IDC_AECCMP ), STM_SETIMAGE, IMAGE_BITMAP, ( LPARAM )LoadImage( hInst, MAKEINTRESOURCE( IDB_AECCMP ), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS ) );
				}

				//设置垂直滚动条。
				{
					RECT p_Rect1, p_Rect2;
					SCROLLINFO p_ScrollInfo;
					POINT p_Point;

					//获取确定按钮在窗口客户区的高度。
					GetWindowRect( GetDlgItem( hDlg, IDC_READMEOK ), &p_Rect1 );
					p_Point.x = p_Rect1.right;
					p_Point.y = p_Rect1.bottom;
					ScreenToClient( hDlg, &p_Point );

					//获取窗口客户区的高度。
					GetClientRect( hDlg, &p_Rect2 );

					//设置垂直滚动条的高度为确定按钮在窗口客户区的高度，页面高度为窗口客户区的高度。
					p_ScrollInfo.cbSize = sizeof( SCROLLINFO );
					p_ScrollInfo.fMask = SIF_ALL;
					GetScrollInfo( hDlg, SB_VERT, &p_ScrollInfo );
					p_ScrollInfo.nMax = p_Point.y + 12;
					p_ScrollInfo.nPage = p_Rect2.bottom;
					SetScrollInfo( hDlg, SB_VERT, &p_ScrollInfo, TRUE );
				}
			}
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
					goto outinit;
				}
				return ( INT_PTR )TRUE;
			}
			return ( INT_PTR )FALSE;
		}
		case WM_DEVICECHANGE: //系统硬件设备改变消息。
		{
			HWND p_AudioInputDeviceComboBoxWndHdl = NULL;
			HWND p_AudioOutputDeviceComboBoxWndHdl = NULL;
			WAVEINCAPS * p_InputCapsArrPt = NULL;
			UINT p_InputCapsArrTotal;
			WAVEOUTCAPS * p_OutputCapsArrPt = NULL;
			UINT p_OutputCapsArrTotal;
			char p_CurAudioInputDevice[sizeof( p_InputCapsArrPt->szPname )] = "";
			char p_CurAudioOutputDevice[sizeof( p_OutputCapsArrPt->szPname )] = "";
			int p_TmpInt;

			//获取音频输入和输出设备的功能。
			if( AudioProcThreadGetDeviceCaps( &p_InputCapsArrPt, &p_InputCapsArrTotal, &p_OutputCapsArrPt, &p_OutputCapsArrTotal, g_ErrInfoVarStrPt ) != 0 )
			{
				VarStrIns( g_ErrInfoVarStrPt, 0, "获取音频输入和输出设备的功能失败。原因：" );
				LOGE( g_ErrInfoVarStrPt->m_StrPt );
				{VarStr * p_ErrInfoVarStrPt = NULL; VarStrInit( &p_ErrInfoVarStrPt ); VarStrCpy( p_ErrInfoVarStrPt, g_ErrInfoVarStrPt->m_StrPt ); PostMessage( g_MainWndHdl, WM_SHOWLOG, ( WPARAM )p_ErrInfoVarStrPt, 0 );}
				goto outdevicechange;
			}
			
			//获取音频输入设备和音频输出设备的控件句柄。
			p_AudioInputDeviceComboBoxWndHdl = GetDlgItem( g_MainWndHdl, IDC_AUDIOINPUTDEVICECOMBOBOX );
			p_AudioOutputDeviceComboBoxWndHdl = GetDlgItem( g_MainWndHdl, IDC_AUDIOOUTPUTDEVICECOMBOBOX );

			//获取当前选择的音频输入输出设备。
			p_TmpInt = SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioInputDevice );
			}
			p_TmpInt = SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAudioOutputDevice );
			}

			//清空音频输入设备和音频输出设备的控件。
			SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );
			SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_RESETCONTENT, 0, 0 );

			if( p_InputCapsArrTotal > 0 )
			{
				p_TmpInt = SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );

				for( p_TmpInt = 0; p_TmpInt < p_InputCapsArrTotal; p_TmpInt++ )
				{
					SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_InputCapsArrPt[p_TmpInt].szPname );
				}
			}
			else
			{
				SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"无" );
			}
			
			if( p_OutputCapsArrTotal > 0 )
			{
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )"默认" );

				for( p_TmpInt = 0; p_TmpInt < p_OutputCapsArrTotal; p_TmpInt++ )
				{
					SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, 0, ( LPARAM )p_OutputCapsArrPt[p_TmpInt].szPname );
				}
			}
			else
			{
				SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )"无" );
			}
			
			//重新选择之前选择的音频输入输出设备。
			if( SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioInputDevice ) == CB_ERR )
			{
				if( SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioInputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			if( SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAudioOutputDevice ) == CB_ERR )
			{
				if( SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"默认" ) == CB_ERR )
				{
					SendMessage( p_AudioOutputDeviceComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )"无" );
				}
			}
			
			p_Result = 0; // //设置本函数执行成功。

			outdevicechange:
			if( p_InputCapsArrPt != NULL ) free( p_InputCapsArrPt );
			if( p_OutputCapsArrPt != NULL ) free( p_OutputCapsArrPt );
			return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
		}
		case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
				case IDC_AUDIOINPUTDEVICECOMBOBOX: //音频输入设备组合框。
				case IDC_AUDIOOUTPUTDEVICECOMBOBOX: //音频输出设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //组合框选择的项目改变消息。
						{
							if( g_AudioProcThreadPt != NULL )
							{
								//判断使用的音频输入设备和音频输出设备。
								AudioProcThreadSetUseDevice( g_AudioProcThreadPt,
															 SendMessage( GetDlgItem( g_MainWndHdl, IDC_AUDIOINPUTDEVICECOMBOBOX ), CB_GETCURSEL, 0, 0 ) - 1,
															 SendMessage( GetDlgItem( g_MainWndHdl, IDC_AUDIOOUTPUTDEVICECOMBOBOX ), CB_GETCURSEL, 0, 0 ) - 1,
															 g_ErrInfoVarStrPt );

								if( g_AudioProcThreadPt->m_InputFrameLnkLstPt != NULL ) //如果音频处理线程已经启动并初始化完毕。
								{
									AudioProcThreadRequireExit( g_AudioProcThreadPt, 3, 1, g_ErrInfoVarStrPt ); //请求重启并阻塞等待。
								}
							}
							break;
						}
					}
					break;
				}
				case IDC_AUDIOINPUTDEVICEISMUTECHECKBOX: //音频输入设备静音按钮。
				{
					if( g_AudioProcThreadPt != NULL )
					{
						AudioProcThreadSetAudioInputDeviceMute( g_AudioProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AUDIOINPUTDEVICEISMUTECHECKBOX ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_AUDIOOUTPUTDEVICEISMUTECHECKBOX: //音频输出设备静音按钮。
				{
					if( g_AudioProcThreadPt != NULL )
					{
						AudioProcThreadSetAudioOutputDeviceMute( g_AudioProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AUDIOOUTPUTDEVICEISMUTECHECKBOX ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );
					}
					break;
				}
				case IDC_CREATESRVRBTN: //创建服务器按钮。
				case IDC_CONNECTSRVRBTN: //连接服务器按钮。
				{
					if( g_AudioProcThreadPt == NULL ) //如果音频处理线程还没有启动。
					{
						LOGI( "开始启动音频处理线程。" );

						//创建音频信息的内存块。
						g_AudioInfoPt = ( AudioInfo * )calloc( 1, sizeof( AudioInfo ) );
						if( g_AudioInfoPt == NULL )
						{
							LOGE( "创建音频信息的内存块失败。" );
							goto outcreate;
						}

						if( VarStrInit( &g_AudioInfoPt->m_IPAddrVarStrPt ) != 0 )
						{
							LOGE( "初始化IP地址动态字符串失败。" );
							goto outcreate;
						}
						
						if( VarStrInit( &g_AudioInfoPt->m_PortVarStrPt ) != 0 )
						{
							LOGE( "初始化端口动态字符串失败。" );
							goto outcreate;
						}

						if( LOWORD( wParam ) == IDC_CREATESRVRBTN )
						{
							g_AudioInfoPt->m_IsCreateSrvrOrClnt = 1; //标记创建服务端接受客户端。
						}
						else
						{
							g_AudioInfoPt->m_IsCreateSrvrOrClnt = 0; //标记创建客户端连接服务端。
						}

						g_AudioInfoPt->m_MainWindowHdl = hDlg; //设置主窗口的句柄。

						//设置IP地址字符串、端口。
						HWND p_TmpWndHdl;
						p_TmpWndHdl = GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX );
						VarStrSetSz( g_AudioInfoPt->m_IPAddrVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
						GetDlgItemText( hDlg, IDC_IPADDRCOMBOBOX, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrPt, g_AudioInfoPt->m_IPAddrVarStrPt->m_StrSz );
						p_TmpWndHdl = GetDlgItem( hDlg, IDC_PORTEDIT );
						VarStrSetSz( g_AudioInfoPt->m_PortVarStrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
						GetDlgItemText( hDlg, IDC_PORTEDIT, g_AudioInfoPt->m_PortVarStrPt->m_StrPt, g_AudioInfoPt->m_PortVarStrPt->m_StrSz );
						
						//初始化音频处理线程的内存指针。
						if( AudioProcThreadInit( &g_AudioProcThreadPt, g_AudioInfoPt, MyAudioProcThreadUserInit, MyAudioProcThreadUserProcess, MyAudioProcThreadUserDestroy, MyAudioProcThreadUserReadInputFrame, MyAudioProcThreadUserWriteOutputFrame, MyAudioProcThreadUserGetPcmOutputFrame,
														( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESAMPLINGRATE8000RADIOBTN ) == BST_CHECKED ) ? 8000 :
															( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESAMPLINGRATE16000RADIOBTN ) == BST_CHECKED ) ? 16000 :
																( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESAMPLINGRATE32000RADIOBTN ) == BST_CHECKED ) ? 32000 : 0,
														( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEFRAME10MSLENRADIOBTN ) == BST_CHECKED ) ? 10 :
															( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEFRAME20MSLENRADIOBTN ) == BST_CHECKED ) ? 20 :
																( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEFRAME30MSLENRADIOBTN ) == BST_CHECKED ) ? 30 : 0, g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "初始化音频处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto outcreate;
						}

						//判断是否使用什么传输协议。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USETCPPRTCLRADIOBTN ) == BST_CHECKED )
						{
							g_AudioInfoPt->m_UseWhatXfrPrtcl = 0;
						}
						else
						{
							g_AudioInfoPt->m_UseWhatXfrPrtcl = 1;
						}

						//判断是否保存设置到文件。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_ISSAVESETTINGTOFILECHECKBOX ) == BST_CHECKED )
						{
							AudioProcThreadSetSaveSettingToFile( g_AudioProcThreadPt, 1, ".\\Setting.txt", g_ErrInfoVarStrPt );
						}
						else
						{
							AudioProcThreadSetSaveSettingToFile( g_AudioProcThreadPt, 0, NULL, g_ErrInfoVarStrPt );
						}
						
						//判断是否打印Log日志。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_ISPRINTLOGCATCHECKBOX ) == BST_CHECKED )
						{
							AudioProcThreadSetPrintLog( g_AudioProcThreadPt, 1, g_ErrInfoVarStrPt );
						}
						else
						{
							AudioProcThreadSetPrintLog( g_AudioProcThreadPt, 0, g_ErrInfoVarStrPt );
						}

						//判断使用的音频输入设备和音频输出设备。
						AudioProcThreadSetUseDevice( g_AudioProcThreadPt,
													 SendMessage( GetDlgItem( g_MainWndHdl, IDC_AUDIOINPUTDEVICECOMBOBOX ), CB_GETCURSEL, 0, 0 ) - 1,
													 SendMessage( GetDlgItem( g_MainWndHdl, IDC_AUDIOOUTPUTDEVICECOMBOBOX ), CB_GETCURSEL, 0, 0 ) - 1,
													 g_ErrInfoVarStrPt );

						//判断音频输入设备是否静音。
						AudioProcThreadSetAudioInputDeviceMute( g_AudioProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AUDIOINPUTDEVICEISMUTECHECKBOX ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );

						//判断音频输出设备是否静音。
						AudioProcThreadSetAudioOutputDeviceMute( g_AudioProcThreadPt, ( IsDlgButtonChecked( g_MainWndHdl, IDC_AUDIOOUTPUTDEVICEISMUTECHECKBOX ) == BST_CHECKED ) ? 1 : 0, g_ErrInfoVarStrPt );

						//判断是否不使用声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USENOAECRADIOBTN ) == BST_CHECKED )
						{
							AudioProcThreadSetUseNoAec( g_AudioProcThreadPt, g_ErrInfoVarStrPt );
						}

						//判断是否使用Speex声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESPEEXAECRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_FilterLen;
							int32_t p_IsUseRec;
							float p_EchoMultiple;
							float p_EchoCont;
							int32_t p_EchoSupes;
							int32_t p_EchoSupesAct;
							int p_IsSaveMemFile;
							
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SPEEXAECFILTERLENEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_FilterLen = atoi( p_TmpStrPt );
							p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecSettingWndHdl, IDC_SPEEXAECISUSERECCHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SPEEXAECECHOMULTIPLEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SPEEXAECECHOCONTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SPEEXAECECHOSUPESEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexAecSettingWndHdl, IDC_SPEEXAECECHOSUPESACTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoSupesAct = atoi( p_TmpStrPt );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_SpeexAecSettingWndHdl, IDC_SPEEXAECISSAVEMEMFILECHECKBOX ) == BST_CHECKED );

							AudioProcThreadSetUseSpeexAec( g_AudioProcThreadPt, p_FilterLen, p_IsUseRec, p_EchoMultiple, p_EchoCont, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, ".\\SpeexAecMem", g_ErrInfoVarStrPt );
						}

						//判断是否使用WebRtc定点版声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEWEBRTCAECMRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseCNGMode;
							int32_t p_EchoMode;
							int32_t p_Delay;
							
							p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmSettingWndHdl, IDC_WEBRTCAECMISUSECNGMODECHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WEBRTCAECMECHOMODEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecmSettingWndHdl, IDC_WEBRTCAECMDELAYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							
							AudioProcThreadSetUseWebRtcAecm( g_AudioProcThreadPt, p_IsUseCNGMode, p_EchoMode, p_Delay, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用WebRtc浮点版声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEWEBRTCAECRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EchoMode;
							int32_t p_Delay;
							int32_t p_IsUseDelayAgnosticMode;
							int32_t p_IsUseExtdFilterMode;
							int32_t p_IsUseRefinedFilterAdaptAecMode;
							int32_t p_IsUseAdaptAdjDelay;
							int p_IsSaveMemFile;

							GetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECECHOMODEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECDELAYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_Delay = atoi( p_TmpStrPt );
							p_IsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECISUSEDELAYAGNOSTICMODECHECKBOX ) == BST_CHECKED );
							p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECISUSEEXTDFILTERMODECHECKBOX ) == BST_CHECKED );
							p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECISUSEREFINEDFILTERADAPTAECMODECHECKBOX ) == BST_CHECKED );
							p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECISUSEADAPTADJDELAYCHECKBOX ) == BST_CHECKED );
							p_IsSaveMemFile = ( IsDlgButtonChecked( g_WebRtcAecSettingWndHdl, IDC_WEBRTCAECISSAVEMEMFILECHECKBOX ) == BST_CHECKED );

							AudioProcThreadSetUseWebRtcAec( g_AudioProcThreadPt, p_EchoMode, p_Delay, p_IsUseDelayAgnosticMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, ".\\WebRtcAecMem", g_ErrInfoVarStrPt );
						}

						//判断是否使用SpeexWebRtc三重声学回音消除器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESPEEXWEBRTCAECRADIOBTN ) == BST_CHECKED )
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

							p_WorkMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWORKMODESPEEXAECWEBRTCAECMRADIOBTN ) == BST_CHECKED ) ? 1 :
											( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWORKMODEWEBRTCAECMWEBRTCAECRADIOBTN ) == BST_CHECKED ) ? 2 :
												( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWORKMODESPEEXAECWEBRTCAECMWEBRTCAECRADIOBTN ) == BST_CHECKED ) ? 3 : 0;
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECFILTERLENEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecFilterLen = atoi( p_TmpStrPt );
							p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECISUSERECCHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECECHOMULTIPLEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoMultiple = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECECHOCONTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoCont = atof( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECECHOSUPESEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupes = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSPEEXAECECHOSUPESACTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SpeexAecEchoSupesAct = atoi( p_TmpStrPt );
							p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECMISUSECNGMODECHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECMECHOMODEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECMDELAYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecmDelay = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECECHOMODEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecEchoMode = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECDELAYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_WebRtcAecDelay = atoi( p_TmpStrPt );
							p_WebRtcAecIsUseDelayAgnosticMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEDELAYAGNOSTICMODECHECKBOX ) == BST_CHECKED );
							p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEEXTDFILTERMODECHECKBOX ) == BST_CHECKED );
							p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEREFINEDFILTERADAPTAECMODECHECKBOX ) == BST_CHECKED );
							p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECWEBRTCAECISUSEADAPTADJDELAYCHECKBOX ) == BST_CHECKED );
							p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECISUSESAMEROOMAEC ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexWebRtcAecSettingWndHdl, IDC_SPEEXWEBRTCAECSAMEROOMECHOMINDELAY ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_SameRoomEchoMinDelay = atoi( p_TmpStrPt );

							AudioProcThreadSetUseSpeexWebRtcAec( g_AudioProcThreadPt, p_WorkMode, p_SpeexAecFilterLen, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCont, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgnosticMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, g_ErrInfoVarStrPt );
						}
						
						//判断是否不使用噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USENONSRADIOBTN ) == BST_CHECKED )
						{
							AudioProcThreadSetUseNoNs( g_AudioProcThreadPt, g_ErrInfoVarStrPt );
						}

						//判断是否使用Speex预处理器的噪音抑制。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESPEEXPPROCNSRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_IsUseNs;
							int32_t p_NoiseSupes;
							int32_t p_IsUseDereverb;

							p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPprocNsSettingWndHdl, IDC_SPEEXPPROCISUSENSCHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocNsSettingWndHdl, IDC_SPEEXPPROCNOISESUPESEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_NoiseSupes = atoi( p_TmpStrPt );
							p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPprocNsSettingWndHdl, IDC_SPEEXPPROCISUSEDEREVERBCHECKBOX ) == BST_CHECKED );

							AudioProcThreadSetUseSpeexPprocNs( g_AudioProcThreadPt, p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, g_ErrInfoVarStrPt );
						}

						//判断是否使用WebRtc定点版噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEWEBRTCNSXRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsxSettingWndHdl, IDC_WEBRTCNSXPOLICYMODE ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							AudioProcThreadSetUseWebRtcNsx( g_AudioProcThreadPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用WebRtc浮点版噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEWEBRTCNSRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsSettingWndHdl, IDC_WEBRTCNSPOLICYMODE ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = atoi( p_TmpStrPt );

							AudioProcThreadSetUseWebRtcNs( g_AudioProcThreadPt, p_PolicyMode, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用RNNoise噪音抑制器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USERNNOISERADIOBTN ) == BST_CHECKED )
						{
							AudioProcThreadSetUseRNNoise( g_AudioProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用Speex预处理器的其他功能。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_ISUSESPEEXPPROCOTHERCHECKBOX ) == BST_CHECKED )
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

							p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCISUSEVADCHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCVADPROBSTARTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbStart = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCVADPROBCONTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbCont = atoi( p_TmpStrPt );
							p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCISUSEAGCCHECKBOX ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCAGCLEVELEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcLevel = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCAGCINCREMENTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcIncrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCAGCDECREMENTEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcDecrement = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPprocOtherSettingWndHdl, IDC_SPEEXPPROCAGCMAXGAINEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcMaxGain = atoi( p_TmpStrPt );

							AudioProcThreadSetSpeexPprocOther( g_AudioProcThreadPt, 1, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, g_ErrInfoVarStrPt );
						}
						else
						{
							AudioProcThreadSetSpeexPprocOther( g_AudioProcThreadPt, 0, 0, 0, 0, 0, 0, 0, 0, 0, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEPCMRADIOBTN ) == BST_CHECKED )
						{
							AudioProcThreadSetUsePcm( g_AudioProcThreadPt, g_ErrInfoVarStrPt );
						}
						
						//判断是否使用Speex编解码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USESPEEXCODECRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int32_t p_EncoderUseCbrOrVbr;
							int32_t p_EncoderQuality;
							int32_t p_EncoderComplexity;
							int32_t p_EncoderPlcExpectedLossRate;
							int32_t p_DecoderIsUsePerceptualEnhancement;

							p_EncoderUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecSettingWndHdl, IDC_SPEEXCODECENCODERUSECBRRADIOBTN ) == BST_CHECKED ) ? 0 : 1;
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SPEEXCODECENCODERQUALITYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderQuality = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SPEEXCODECENCODERCOMPLEXITYEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderComplexity = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexCodecSettingWndHdl, IDC_SPEEXCODECENCODERPLCEXPECTEDLOSSRATEEDIT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_EncoderPlcExpectedLossRate = atoi( p_TmpStrPt );
							p_DecoderIsUsePerceptualEnhancement = ( IsDlgButtonChecked( g_SpeexCodecSettingWndHdl, IDC_SPEEXCODECISUSEPERCEPTUALENHANCEMENTCHECKBOX ) == BST_CHECKED );

							AudioProcThreadSetUseSpeexCodec( g_AudioProcThreadPt, p_EncoderUseCbrOrVbr, p_EncoderQuality, p_EncoderComplexity, p_EncoderPlcExpectedLossRate, p_DecoderIsUsePerceptualEnhancement, g_ErrInfoVarStrPt );
						}

						//判断是否使用Opus编解码器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEOPUSCODECRADIOBTN ) == BST_CHECKED )
						{
							AudioProcThreadSetUseOpus( g_AudioProcThreadPt, g_ErrInfoVarStrPt );
						}

						//判断是否使用链表。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USELNKLSTRADIOBTN ) == BST_CHECKED )
						{
							g_AudioInfoPt->m_UseWhatRecvOutputFrame = 0;
						}
						
						//判断是否使用自己设计的自适应抖动缓冲器。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_USEAJBRADIOBTN ) == BST_CHECKED )
						{
							char p_TmpStrPt[100];
							int p_AjbMinNeedBufFrameCnt;
							int p_AjbMaxNeedBufFrameCnt;
							int8_t p_AjbAdaptSensitivity;

							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AJBMINNEEDBUFFRAMECNT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AjbMinNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AJBMAXNEEDBUFFRAMECNT ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AjbMaxNeedBufFrameCnt = atoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_AjbSettingWndHdl, IDC_AJBADAPTSENSITIVITY ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AjbAdaptSensitivity = atoi( p_TmpStrPt );

							g_AudioInfoPt->m_UseWhatRecvOutputFrame = 1;
							g_AudioInfoPt->m_AjbMinNeedBufFrameCnt = p_AjbMinNeedBufFrameCnt;
							g_AudioInfoPt->m_AjbMaxNeedBufFrameCnt = p_AjbMaxNeedBufFrameCnt;
							g_AudioInfoPt->m_AjbAdaptSensitivity = p_AjbAdaptSensitivity;
						}

						//判断是否保存音频到文件。
						if( IsDlgButtonChecked( g_SettingWndHdl, IDC_ISSAVEAUDIOTOFILECHECKBOX ) == BST_CHECKED )
						{
							AudioProcThreadSetSaveAudioToFile( g_AudioProcThreadPt, 1, ".\\AudioInput.wav", ".\\AudioOutput.wav", ".\\AudioResult.wav", g_ErrInfoVarStrPt );
						}
						else
						{
							AudioProcThreadSetSaveAudioToFile( g_AudioProcThreadPt, 0, NULL, NULL, NULL, g_ErrInfoVarStrPt );
						}
						
						//启动音频处理线程。
						if( AudioProcThreadStart( g_AudioProcThreadPt, g_ErrInfoVarStrPt ) != 0 )
						{
							LOGFE( "启动音频处理线程失败。原因：%s", g_ErrInfoVarStrPt->m_StrPt );
							goto outcreate;
						}

						LOGI( "启动音频处理线程完毕。" );

						p_Result = 0; // //设置本函数执行成功。

						outcreate:
						if( p_Result != 0 )
						{
							if( g_AudioProcThreadPt != NULL )
							{
								AudioProcThreadRequireExit( g_AudioProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求音频处理线程退出。
								AudioProcThreadDestroy( g_AudioProcThreadPt, g_ErrInfoVarStrPt ); //销毁音频处理线程。
								g_AudioProcThreadPt = NULL;
							}
							if( g_AudioInfoPt != NULL )
							{
								VarStrDestroy( g_AudioInfoPt->m_IPAddrVarStrPt );
								VarStrDestroy( g_AudioInfoPt->m_PortVarStrPt );
								free( g_AudioInfoPt );
								g_AudioInfoPt = NULL;
							}
						}
					}
					else //如果音频处理线程已经启动。
					{
						AudioProcThreadRequireExit( g_AudioProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求音频处理线程退出。

						p_Result = 0; // //设置本函数执行成功。
					}
					return ( p_Result == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
					break;
				}
				case IDC_SETTINGBTN:
				{
					ShowWindow( g_SettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXAECSETTING:
				{
					ShowWindow( g_SpeexAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXAECDELMEMFILE:
				{
					if( DeleteFile( ".\\SpeexAecMem" ) != 0 )
					{
						MessageBox( hDlg, "删除Speex声学回音消除器的内存块文件 SpeexAecMem 成功。", "Windows下音频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						VarStrFmtCpy( g_ErrInfoVarStrPt, "删除Speex声学回音消除器的内存块文件 SpeexAecMem 失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
						MessageBox( hDlg, g_ErrInfoVarStrPt->m_StrPt, "Windows下音频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case IDC_WEBRTCAECMSETTING:
				{
					ShowWindow( g_WebRtcAecmSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WEBRTCAECSETTING:
				{
					ShowWindow( g_WebRtcAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXWEBRTCAECSETTING:
				{
					ShowWindow( g_SpeexWebRtcAecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示SpeexWebRtc三重声学回音消除器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WEBRTCAECDELMEMFILE:
				{
					if( DeleteFile( ".\\WebRtcAecMem" ) != 0 )
					{
						MessageBox( hDlg, "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 成功。", "Windows下音频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						VarStrFmtCpy( g_ErrInfoVarStrPt, "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 失败。原因：%s", FuncGetErrInfo( GetLastError(), 'G' ) );
						MessageBox( hDlg, g_ErrInfoVarStrPt->m_StrPt, "Windows下音频对讲演示程序", MB_OK | MB_ICONERROR );
					}
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXPPROCNSSETTING:
				{
					ShowWindow( g_SpeexPprocNsSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的噪音抑制设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WEBRTCNSXSETTING:
				{
					ShowWindow( g_WebRtcNsxSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc定点版噪音抑制器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_WEBRTCNSSETTING:
				{
					ShowWindow( g_WebRtcNsSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc浮点版噪音抑制器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXPPROCOTHERSETTING:
				{
					ShowWindow( g_SpeexPprocOtherSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的其他功能设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_SPEEXCODECSETTING:
				{
					ShowWindow( g_SpeexCodecSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex编解码器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_AJBSETTING:
				{
					ShowWindow( g_AjbSettingWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_READMEBTN:
				{
					ShowWindow( g_ReadMeWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示必读说明界面的模态对话框。
					return ( INT_PTR )TRUE;
				}
				case IDC_CLEARLOGBTN:
				{
					SendMessage( GetDlgItem( hDlg, IDC_LOGLIST ), LB_RESETCONTENT, 0, 0 ); //清空日志列表。
					SendDlgItemMessage( hDlg, IDC_LOGLIST, LB_SETHORIZONTALEXTENT, 0, 0 ); //清空滚动条的当前像素大小。
					return ( INT_PTR )TRUE;
				}
				case IDC_BLOGADDRBTN:
				{
					ShellExecute( NULL, "open", "http://www.cnblogs.com/gaoyaguo", NULL, NULL, SW_SHOWNORMAL );
					return ( INT_PTR )TRUE;
				}
				case IDC_WINDOWSSRCADDRBTN:
				{
					ShellExecute( NULL, "open", "https://github.com/cyz7758520/Windows_audio_talkback_demo_program", NULL, NULL, SW_SHOWNORMAL );
					return ( INT_PTR )TRUE;
				}
				case IDC_ANDROIDSRCADDRBTN:
				{
					ShellExecute( NULL, "open", "https://github.com/cyz7758520/Android_audio_talkback_demo_program", NULL, NULL, SW_SHOWNORMAL );
					return ( INT_PTR )TRUE;
				}
				case IDCANCEL:
				case IDC_SETTINGOK:
				case IDC_SPEEXAECSETTINGOK:
				case IDC_WEBRTCAECMSETTINGOK:
				case IDC_WEBRTCAECSETTINGOK:
				case IDC_SPEEXWEBRTCAECSETTINGOK:
				case IDC_SPEEXPPROCNSSETTINGOK:
				case IDC_WEBRTCNSXSETTINGOK:
				case IDC_WEBRTCNSSETTINGOK:
				case IDC_SPEEXPPROCOTHERSETTINGOK:
				case IDC_SPEEXCODECSETTINGOK:
				case IDC_AJBSETTINGOK:
				case IDC_READMEOK:
				{
					if( g_MainWndHdl == hDlg ) //如果是主界面。
					{
						LOGI( "用户在主界面按下关闭按钮，本软件退出。" );

						AudioProcThreadRequireExit( g_AudioProcThreadPt, 1, 1, g_ErrInfoVarStrPt ); //请求音频处理线程退出。

						PostQuitMessage( 0 ); //发送退出消息。
					}
					else //如果是其他界面。
					{
						EnableWindow( GetParent( hDlg ), TRUE ), ShowWindow( hDlg, SW_HIDE ); //隐藏当前模态对话框。
					}
					return ( INT_PTR )TRUE;
				}
			}
			break;
		}
		case WM_VSCROLL: //垂直滚动条滚动消息。
		{
			int p_CurPos = GetScrollPos( hDlg, SB_VERT );
			int p_NewPos;
			SCROLLINFO p_ScrollInfo;

			switch( LOWORD( wParam ) )
			{
				case SB_LINEUP: //向上滚动一行。
				{
					SetScrollPos( hDlg, SB_VERT, p_CurPos - 13, TRUE );
					break;
				}
				case SB_LINEDOWN: //向下滚动一行。
				{
					SetScrollPos( hDlg, SB_VERT, p_CurPos + 13, TRUE );
					break;
				}
				case SB_PAGEUP: //向上滚动一页。
				{
					p_ScrollInfo.fMask = SIF_PAGE;
					GetScrollInfo( hDlg, SB_VERT, &p_ScrollInfo );
					SetScrollPos( hDlg, SB_VERT, p_CurPos - p_ScrollInfo.nPage, TRUE );
					break;
				}
				case SB_PAGEDOWN: //向下滚动一页。
				{
					p_ScrollInfo.fMask = SIF_PAGE;
					GetScrollInfo( hDlg, SB_VERT, &p_ScrollInfo );
					SetScrollPos( hDlg, SB_VERT, p_CurPos + p_ScrollInfo.nPage, TRUE );
					break;
				}
				case SB_THUMBTRACK: //用户正在拖动滚动条。
				case SB_THUMBPOSITION: //用户拖动滚动条完毕并释放鼠标。
				{
					SetScrollPos( hDlg, SB_VERT, HIWORD( wParam ), TRUE );
					break;
				}
			}

			p_NewPos = GetScrollPos( hDlg, SB_VERT ); //获取当前窗口的垂直滚动条的位置。

			if( hDlg == g_ReadMeWndHdl ) //如果是必读说明界面。
			{
				ScrollWindow( hDlg, 0, p_CurPos - p_NewPos, NULL, NULL ); //滚动当前窗口的到指定的位置。
			}
			
			return ( INT_PTR )TRUE;
		}
		case WM_MOUSEWHEEL: //鼠标滚轮消息。
		{
			if( hDlg == g_ReadMeWndHdl ) //如果是必读说明界面。
			{
				UINT p_WheelScrollLines;

				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &p_WheelScrollLines, 0 ); //获取滚轮滚动的行数。

				if( ( int16_t )HIWORD( wParam ) > 0 )
				{
					for( int p_WheelTimes = ( int16_t )HIWORD( wParam ) / WHEEL_DELTA * p_WheelScrollLines; p_WheelTimes > 0; p_WheelTimes-- )
					{
						SendMessage( g_ReadMeWndHdl, WM_VSCROLL, SB_LINEUP, 0 );
					}
				}
				else
				{
					for( int p_WheelTimes = ( int16_t )HIWORD( wParam ) / WHEEL_DELTA * p_WheelScrollLines; p_WheelTimes < 0; p_WheelTimes++ )
					{
						SendMessage( g_ReadMeWndHdl, WM_VSCROLL, SB_LINEDOWN, 0 );
					}
				}
			}
			
			return ( INT_PTR )TRUE;
		}
		case WM_DESTROY: //窗口销毁消息。
		{
			return ( INT_PTR )TRUE;
		}
		case WM_MYAUDIOPROCTHREADSTART: //音频处理线程启动的消息。
		{
			if( g_AudioInfoPt->m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
            {
				EnableWindow( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_PORTEDIT ), FALSE ); //设置端口控件为不可用。
				SetWindowText( GetDlgItem( hDlg, IDC_CREATESRVRBTN ), "中断" ); //设置创建服务端按钮的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, IDC_CONNECTSRVRBTN ), FALSE ); //设置连接服务端按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_SETTINGBTN ), FALSE ); //设置设置按钮为不可用。
            }
            else //如果是创建客户端。
            {
				EnableWindow( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), FALSE ); //设置IP地址控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_PORTEDIT ), FALSE ); //设置端口控件为不可用。
				EnableWindow( GetDlgItem( hDlg, IDC_CREATESRVRBTN ), FALSE ); //设置创建服务端按钮为不可用。
				SetWindowText( GetDlgItem( hDlg, IDC_CONNECTSRVRBTN ), "中断" ); //设置连接服务端按钮的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, IDC_SETTINGBTN ), FALSE ); //设置设置按钮为不可用。
            }
			return ( INT_PTR )TRUE;
		}
		case WM_MYAUDIOPROCTHREADEND: //音频处理线程退出的消息。
		{
			if( g_AudioProcThreadPt != NULL )
			{
				AudioProcThreadDestroy( g_AudioProcThreadPt, g_ErrInfoVarStrPt ); //销毁音频处理线程。
				g_AudioProcThreadPt = NULL;
			}
			if( g_AudioInfoPt != NULL )
			{
				VarStrDestroy( g_AudioInfoPt->m_IPAddrVarStrPt );
				VarStrDestroy( g_AudioInfoPt->m_PortVarStrPt );
				if( g_AudioInfoPt->m_TmpBytePt != NULL ) free( g_AudioInfoPt->m_TmpBytePt );
				free( g_AudioInfoPt );
				g_AudioInfoPt = NULL;
			}

			EnableWindow( GetDlgItem( hDlg, IDC_IPADDRCOMBOBOX ), TRUE ); //设置IP地址控件为不可用。
			EnableWindow( GetDlgItem( hDlg, IDC_PORTEDIT ), TRUE ); //设置端口控件为不可用。
			EnableWindow( GetDlgItem( hDlg, IDC_CREATESRVRBTN ), TRUE ); //设置创建服务端按钮为不可用。
			SetWindowText( GetDlgItem( hDlg, IDC_CREATESRVRBTN ), "创建服务端" ); //设置创建服务端按钮的内容为“中断”。
			EnableWindow( GetDlgItem( hDlg, IDC_CONNECTSRVRBTN ), TRUE ); //设置连接服务端按钮为不可用。
			SetWindowText( GetDlgItem( hDlg, IDC_CONNECTSRVRBTN ), "连接服务端" ); //设置连接服务端按钮的内容为“中断”。
			EnableWindow( GetDlgItem( hDlg, IDC_SETTINGBTN ), TRUE ); //设置设置按钮为不可用。
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
					SendMessage( GetDlgItem( hDlg, IDC_LOGLIST ), LB_ADDSTRING, 0, ( WPARAM )( ( VarStr * )wParam )->m_StrPt );
				}
				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, IDC_LOGLIST, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( ( VarStr * )wParam )->m_StrPt, ( ( VarStr * )wParam )->m_StrLen, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, IDC_LOGLIST, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}
				VarStrDestroy( ( VarStr * )wParam );
			}
			return ( INT_PTR )TRUE;
		}
	}
	return ( INT_PTR )FALSE;
}
