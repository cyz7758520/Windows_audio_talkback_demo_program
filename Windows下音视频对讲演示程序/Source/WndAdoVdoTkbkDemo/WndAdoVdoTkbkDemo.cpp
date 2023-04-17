// WndAdoVdoTkbkDemo.cpp : 定义应用程序的入口点。
//
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "WndAdoVdoTkbkDemo.h"
#include "MyMediaPocsThrd.h"

//全局变量。
HINSTANCE g_IstnsHdl; //存放当前实例的句柄。
VstrCls g_ErrInfoVstr; //存放错误信息动态字符串的指针。
VstrCls g_StngFileFullPathVstr; //存放设置文件的完整路径动态字符串。
VstrCls g_AdoVdoInptOtptAviFileFullPathVstr; //存放音视频输入输出Avi文件的完整路径动态字符串。
VstrCls g_AdoInptSrcWaveFileFullPathVstr; //存放音频输入原始Wave文件的完整路径动态字符串。
VstrCls g_AdoInptRsltWaveFileFullPathVstr; //存放音频输入结果Wave文件的完整路径动态字符串。
VstrCls g_AdoOtptSrcWaveFileFullPathVstr; //存放音频输出原始Wave文件的完整路径动态字符串。
MyMediaPocsThrdCls * g_MyMediaPocsThrdPt = NULL; //存放媒体处理线程的指针。
HWND g_MainDlgWndHdl = NULL; //存放主对话框窗口的句柄。
long g_MainDlgWndMinHeight = 0; //存放主对话框窗口的最小高度，单位为像素。
long g_MainDlgWndMinWidth = 0; //存放主对话框窗口的最小宽度，单位为像素。
HWND g_LogLtBoxWndHdl = NULL; //存放日志列表框窗口的句柄。
HWND g_VdoInptPrvwTxtWndHdl = NULL; //存放视频输入预览文本框窗口的句柄。
HWND g_VdoOtptDspyTxtWndHdl = NULL; //存放视频输出显示文本框窗口句柄。
long g_VdoTxtWndLeftMargin = 0; //存放视频文本框窗口的左边距，单位为像素。
long g_VdoTxtWndTopMargin = 0; //存放视频文本框窗口的顶边距，单位为像素。
long g_VdoTxtWndRightMargin = 0; //存放视频文本框窗口的右边距，单位为像素。
long g_VdoTxtWndBottomMargin = 0; //存放视频文本框窗口的底边距，单位为像素。
int g_VdoWndShowMode = 0; //存放视频窗口的显示模式，为0表示正常，为1表示垂直最大化排列，为2表示水平最大化排列。
HWND g_VdoInptPrvwWndHdl = NULL; //存放视频输入预览窗口的句柄。
HWND g_VdoOtptDspyWndHdl = NULL; //存放视频输出显示窗口句柄。

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
HWND g_SpeexPrpocsStngDlgWndHdl = NULL; //存放Speex预处理器的设置对话框窗口的句柄。
HWND g_SpeexCodecStngDlgWndHdl = NULL; //存放Speex编解码器设置对话框窗口的句柄。
HWND g_OpenH264CodecStngDlgWndHdl = NULL; //存放OpenH264编解码器设置对话框窗口的句柄。

HWND g_TkbkStsWndHdl = NULL; //存放对讲状态窗口的句柄。

//设置视频窗口的显示模式。
void SetVdoWndShowMode( int VdoWndShowMode )
{
	RECT p_MainDlgWndRect;
	GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect );

	switch( VdoWndShowMode )
	{
		default:
		case 0: //正常。
		{
			if( g_VdoWndShowMode != 0 )
			{
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ), SW_SHOW ); //设置TCP协议单选按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseUdpPrtclRdBtnId ), SW_SHOW ); //设置UDP协议单选按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, XfrPrtclStngBtnId ), SW_SHOW ); //设置传输协议设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, IPAddrCbBoxId ), SW_SHOW ); //设置IP地址组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, PortEdTxtId ), SW_SHOW ); //设置端口编辑框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ), SW_SHOW ); //设置音频对讲模式单选按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ), SW_SHOW ); //设置视频对讲模式单选按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ), SW_SHOW ); //设置音视频对讲模式单选按钮为显示。
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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_SHOW ); //设置音频输入原始波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_SHOW ); //设置音频输出原始波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_SHOW ); //设置音频输入结果波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_SHOW ); //设置日志列表框为显示。
				ShowWindow( g_VdoOtptDspyTxtWndHdl, SW_SHOW ); //设置视频输出显示文本框为显示。

				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin, RectWidth( p_MainDlgWndRect ) - g_VdoTxtWndLeftMargin - g_VdoTxtWndRightMargin, ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin + ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, RectWidth( p_MainDlgWndRect ) - g_VdoTxtWndLeftMargin - g_VdoTxtWndRightMargin, ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, 0 ); //设置视频输出显示文本框窗口的位置。

				InvalidateRect( g_MainDlgWndHdl, NULL, TRUE ); //重绘主对话框窗口。
				g_VdoWndShowMode = 0;
			}
			else
			{
				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin, RectWidth( p_MainDlgWndRect ) - g_VdoTxtWndLeftMargin - g_VdoTxtWndRightMargin, ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin + ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, RectWidth( p_MainDlgWndRect ) - g_VdoTxtWndLeftMargin - g_VdoTxtWndRightMargin, ( RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin ) / 2, 0 ); //设置视频输出显示文本框窗口的位置。
				
				InvalidateRect( g_VdoInptPrvwTxtWndHdl, NULL, TRUE ); //重绘视频输入预览文本框窗口。
				InvalidateRect( g_VdoOtptDspyTxtWndHdl, NULL, TRUE ); //重绘视频输出显示文本框窗口。
			}
			break;
		}
		case 1: //垂直最大化排列。
		{
			if( g_VdoWndShowMode != 1 )
			{
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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_HIDE ); //设置音频输入原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_HIDE ); //设置音频输出原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_HIDE ); //设置音频输入结果波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表框为隐藏。

				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, 0, 0, RectWidth( p_MainDlgWndRect ), ( RectHeight( p_MainDlgWndRect ) ) / 2, 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, 0, ( RectHeight( p_MainDlgWndRect ) ) / 2, RectWidth( p_MainDlgWndRect ), ( RectHeight( p_MainDlgWndRect ) ) / 2, 0 ); //设置视频输出显示文本框窗口的位置。

				g_VdoWndShowMode = 1;
			}
			else
			{
				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, 0, 0, RectWidth( p_MainDlgWndRect ), ( RectHeight( p_MainDlgWndRect ) ) / 2, 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, 0, ( RectHeight( p_MainDlgWndRect ) ) / 2, RectWidth( p_MainDlgWndRect ), ( RectHeight( p_MainDlgWndRect ) ) / 2, 0 ); //设置视频输出显示文本框窗口的位置。
			}

			InvalidateRect( g_VdoInptPrvwTxtWndHdl, NULL, TRUE ); //重绘视频输入预览文本框窗口。
			InvalidateRect( g_VdoOtptDspyTxtWndHdl, NULL, TRUE ); //重绘视频输出显示文本框窗口。
			break;
		}
		case 2: //水平最大化排列。
		{
			if( g_VdoWndShowMode != 2 )
			{
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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_HIDE ); //设置音频输入原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_HIDE ); //设置音频输出原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_HIDE ); //设置音频输入结果波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ), SW_HIDE ); //设置日志列表框为隐藏。

				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, 0, 0, ( RectWidth( p_MainDlgWndRect ) ) / 2, RectHeight( p_MainDlgWndRect ), 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, ( RectWidth( p_MainDlgWndRect ) ) / 2, 0, ( RectWidth( p_MainDlgWndRect ) ) / 2, RectHeight( p_MainDlgWndRect ), 0 ); //设置视频输出显示文本框窗口的位置。

				g_VdoWndShowMode = 2;
			}
			else
			{
				SetWindowPos( g_VdoInptPrvwTxtWndHdl, HWND_BOTTOM, 0, 0, ( RectWidth( p_MainDlgWndRect ) ) / 2, RectHeight( p_MainDlgWndRect ), 0 ); //设置视频输入预览文本框窗口的位置。
				SetWindowPos( g_VdoOtptDspyTxtWndHdl, HWND_BOTTOM, ( RectWidth( p_MainDlgWndRect ) ) / 2, 0, ( RectWidth( p_MainDlgWndRect ) ) / 2, RectHeight( p_MainDlgWndRect ), 0 ); //设置视频输出显示文本框窗口的位置。
			}

			InvalidateRect( g_VdoInptPrvwTxtWndHdl, NULL, TRUE ); //重绘视频输入预览文本框窗口。
			InvalidateRect( g_VdoOtptDspyTxtWndHdl, NULL, TRUE ); //重绘视频输出显示文本框窗口。
			break;
		}
	}
}

//窗口消息处理过程函数。
INT_PTR CALLBACK WndMsgPocsPocdr( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。
	
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
		case WM_SIZING:
		{
			RECT p_CurRect;
			RECT * p_AfterRectPt = ( RECT * )lParam;
			
			if( hDlg == g_MainDlgWndHdl ) //如果是主对话框窗口。
			{
				GetWindowRect( g_MainDlgWndHdl, &p_CurRect ); //获取主对话框窗口当前的位置。

				switch( wParam )
				{
					case WMSZ_BOTTOM: //底边被拖动。
					{
						if( p_AfterRectPt->bottom - p_AfterRectPt->top < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->bottom = p_CurRect.top + g_MainDlgWndMinHeight;
						}
						break;
					}
					case WMSZ_BOTTOMLEFT: //底边和左边被拖动，也就是左下角被拖动。
					{
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->left = p_CurRect.right - g_MainDlgWndMinWidth;
						}
						if( RectHeight( *p_AfterRectPt ) < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->bottom = p_CurRect.top + g_MainDlgWndMinHeight;
						}
						break;
					}
					case WMSZ_BOTTOMRIGHT: //底边和右边被拖动，也就是右下角被拖动。
					{
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->right = p_CurRect.left + g_MainDlgWndMinWidth;
						}
						if( RectHeight( *p_AfterRectPt ) < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->bottom = p_CurRect.top + g_MainDlgWndMinHeight;
						}
						break;
					}
					case WMSZ_LEFT: //左边被拖动。
					{
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->left = p_CurRect.right - g_MainDlgWndMinWidth;
						}
						break;
					}
					case WMSZ_RIGHT: //右边被拖动。
					{
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->right = p_CurRect.left + g_MainDlgWndMinWidth;
						}
						break;
					}
					case WMSZ_TOP: //顶边被拖动。
					{
						if( RectHeight( *p_AfterRectPt ) < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->top = p_CurRect.bottom - g_MainDlgWndMinHeight;
						}
						break;
					}
					case WMSZ_TOPLEFT: //顶边和左边被拖动，也就是左上角被拖动。
					{
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->left = p_CurRect.right - g_MainDlgWndMinWidth;
						}
						if( RectHeight( *p_AfterRectPt ) < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->top = p_CurRect.bottom - g_MainDlgWndMinHeight;
						}
						break;
					}
					case WMSZ_TOPRIGHT: //顶边和右边被拖动，也就是右上角被拖动。
					{
						if( RectHeight( *p_AfterRectPt ) < g_MainDlgWndMinHeight ) //如果窗口的高度小于最小高度。
						{
							p_AfterRectPt->top = p_CurRect.bottom - g_MainDlgWndMinHeight;
						}
						if( RectWidth( *p_AfterRectPt ) < g_MainDlgWndMinWidth ) //如果窗口的高度小于最小宽度。
						{
							p_AfterRectPt->right = p_CurRect.left + g_MainDlgWndMinWidth;
						}
						break;
					}
				}
			}
			return ( INT_PTR )TRUE;
		}
		case WM_SIZE:
		{
			RECT p_MainDlgWndRect;
			RECT p_LogLtBoxWndRect;
			RECT p_VdoInptPrvwTxtWndRect;
			RECT p_VdoOtptDspyTxtWndHdlRect;

			GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect ); //获取主对话框窗口的位置。

			//移动日志列表框窗口。
			GetWindowRect( g_LogLtBoxWndHdl, &p_LogLtBoxWndRect );
			RectScrnToClnt( g_MainDlgWndHdl, p_LogLtBoxWndRect );
			p_LogLtBoxWndRect.bottom = p_MainDlgWndRect.bottom - g_VdoTxtWndBottomMargin;
			SetWindowPos( g_LogLtBoxWndHdl, NULL, p_LogLtBoxWndRect.left, p_LogLtBoxWndRect.top, RectWidth( p_LogLtBoxWndRect ), RectHeight( p_LogLtBoxWndRect ), SWP_NOMOVE | SWP_NOZORDER );
			//InvalidateRect( g_LogLtBoxWndHdl, NULL, TRUE ); //重绘日志列表框窗口。
			
			//移动视频输入预览文本框窗口和视频输出显示文本框窗口。
			SetVdoWndShowMode( g_VdoWndShowMode );

			return ( INT_PTR )TRUE;
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
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameVstrArrPt, &p_AdoOtptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
			{
				g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输出设备名称失败。原因：" ) );
				LOGE( g_ErrInfoVstr.m_VstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
				goto OutDvcChange;
			}
			if( MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameVstrArrPt, &p_VdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
			{
				g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取视频输入设备名称失败。原因：" ) );
				LOGE( g_ErrInfoVstr.m_VstrPt );
				{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
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
				for( p_TmpInt = 0; p_TmpInt < p_AdoInptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_AdoInptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
				}
			}
			else
			{
				SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
			}
			if( p_AdoOtptDvcTotal > 0 )
			{
				for( p_TmpInt = 0; p_TmpInt < p_AdoOtptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_AdoOtptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
				}
			}
			else
			{
				SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
			}
			if( p_VdoInptDvcTotal > 0 )
			{
				for( p_TmpInt = 0; p_TmpInt < p_VdoInptDvcTotal; p_TmpInt++ )
				{
					SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_VdoInptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
				}
			}
			else
			{
				SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
			}
			
			//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
			
			if( ( p_CurAdoInptDvcNamePt == NULL ) || ( SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoInptDvcNamePt ) == CB_ERR ) )
			{
				SendMessage( p_AdoInptDvcComboBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
			}
			if( ( p_CurAdoOtptDvcNamePt == NULL ) || ( SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoOtptDvcNamePt ) == CB_ERR ) )
			{
				SendMessage( p_AdoOtptDvcComboBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
			}
			if( ( p_CurVdoInptDvcNamePt == NULL ) || ( SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVdoInptDvcNamePt ) == CB_ERR ) )
			{
				SendMessage( p_VdoInptDvcComboBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
			}
			
			p_Rslt = 0; // //设置本函数执行成功。

			OutDvcChange:
			MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameVstrArrPt, p_AdoInptDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameVstrArrPt, p_AdoOtptDvcTotal, NULL );
			MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameVstrArrPt, p_VdoInptDvcTotal, NULL );
			return ( p_Rslt == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
		}
		case WM_COMMAND: //命令消息。
		{
			switch( LOWORD( wParam ) )
			{
				case UseAdoTkbkModeRdBtnId: //使用音频对讲模式单选按钮。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						MyMediaPocsThrdCls::UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new MyMediaPocsThrdCls::UserMsgLclTkbkMode;
						p_UserMsgLclTkbkModePt->m_LclTkbkMode = MyMediaPocsThrdCls::TkbkModeAdo;
						g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgLclTkbkModePt, NULL );
					}
					break;
				}
				case UseVdoTkbkModeRdBtnId: //使用视频对讲模式单选按钮。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						MyMediaPocsThrdCls::UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new MyMediaPocsThrdCls::UserMsgLclTkbkMode;
						p_UserMsgLclTkbkModePt->m_LclTkbkMode = MyMediaPocsThrdCls::TkbkModeVdo;
						g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgLclTkbkModePt, NULL );
					}
					break;
				}
				case UseAdoVdoTkbkModeRdBtnId: //使用音视频对讲模式单选按钮。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						MyMediaPocsThrdCls::UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new MyMediaPocsThrdCls::UserMsgLclTkbkMode;
						p_UserMsgLclTkbkModePt->m_LclTkbkMode = MyMediaPocsThrdCls::TkbkModeAdoVdo;
						g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgLclTkbkModePt, NULL );
					}
					break;
				}
				case UseAdoInptDvcCbBoxId: //使用音频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MyMediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								g_MyMediaPocsThrdPt->AdoInptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
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
							if( g_MyMediaPocsThrdPt != NULL )
							{
								//设置音频输出使用的设备。
								g_MyMediaPocsThrdPt->AdoOtptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
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
							if( g_MyMediaPocsThrdPt != NULL )
							{
								//设置视频输入使用的设备。
								g_MyMediaPocsThrdPt->VdoInptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
							}
							break;
						}
					}
					break;
				}
				case AdoInptIsMuteCkBoxId: //音频输入静音复选框。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						g_MyMediaPocsThrdPt->AdoInptSetIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case AdoOtptIsMuteCkBoxId: //音频输出静音复选框。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						g_MyMediaPocsThrdPt->AdoOtptSetIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoInptIsBlackCkBoxId: //视频输入黑屏复选框。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						g_MyMediaPocsThrdPt->VdoInptSetIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoOtptIsBlackCkBoxId: //视频输出黑屏复选框。
				{
					if( g_MyMediaPocsThrdPt != NULL )
					{
						g_MyMediaPocsThrdPt->VdoOtptSetStrmIsBlack( 0, ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case CreateSrvrBtnId: //创建服务器按钮。
				case CnctSrvrBtnId: //连接服务器按钮。
				{
					if( g_MyMediaPocsThrdPt == NULL ) //如果媒体处理线程还没有启动。
					{
						LOGI( Cu8vstr( "开始启动媒体处理线程。" ) );

						//初始化媒体处理线程的指针。
						g_MyMediaPocsThrdPt = new MyMediaPocsThrdCls( g_MainDlgWndHdl );
						if( g_MyMediaPocsThrdPt->Init( &g_ErrInfoVstr ) != 0 )
						{
							LOGFE( Cu8vstr( "初始化媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
							goto OutCreateSrvrOrClnt;
						}
						g_MyMediaPocsThrdPt->m_TkbkStsWndHdl = g_TkbkStsWndHdl;

						//设置网络。
						{
							HWND p_TmpWndHdl;
							
							//设置IP地址动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, IPAddrCbBoxId );
							if( VstrInit( &g_MyMediaPocsThrdPt->m_Ntwk.m_IPAddrVstrPt, Utf16, , ) != 0 )
							{
								LOGE( Cu8vstr( "初始化IP地址动态字符串失败。" ) );
								goto OutCreateSrvrOrClnt;
							}
							VstrSetSz( g_MyMediaPocsThrdPt->m_Ntwk.m_IPAddrVstrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, IPAddrCbBoxId, ( wchar_t * )g_MyMediaPocsThrdPt->m_Ntwk.m_IPAddrVstrPt->m_Pt, g_MyMediaPocsThrdPt->m_Ntwk.m_IPAddrVstrPt->m_SzChr );

							//设置端口动态字符串。
							p_TmpWndHdl = GetDlgItem( hDlg, PortEdTxtId );
							if( VstrInit( &g_MyMediaPocsThrdPt->m_Ntwk.m_PortVstrPt, Utf16, , ) != 0 )
							{
								LOGE( Cu8vstr( "初始化端口动态字符串失败。" ) );
								goto OutCreateSrvrOrClnt;
							}
							VstrSetSz( g_MyMediaPocsThrdPt->m_Ntwk.m_PortVstrPt, GetWindowTextLength( p_TmpWndHdl ) + 1 );
							GetDlgItemText( hDlg, PortEdTxtId, ( wchar_t * )g_MyMediaPocsThrdPt->m_Ntwk.m_PortVstrPt->m_Pt, g_MyMediaPocsThrdPt->m_Ntwk.m_PortVstrPt->m_SzChr );
							
							//设置使用什么传输协议。
							g_MyMediaPocsThrdPt->m_Ntwk.m_UseWhatXfrPrtcl = ( IsDlgButtonChecked( g_MainDlgWndHdl, UseTcpPrtclRdBtnId ) == BST_CHECKED ) ? 0 : 1;
							
							//设置传输模式。
							if( IsDlgButtonChecked( g_XfrPrtclStngDlgWndHdl, UsePttRdBtnId ) == BST_CHECKED )
							{
								g_MyMediaPocsThrdPt->m_Ntwk.m_XfrMode = 0;
							}
							else
							{
								g_MyMediaPocsThrdPt->m_Ntwk.m_XfrMode = 1;
							}

							//设置最大连接次数。
							{
								wchar_t p_TmpStrPt[100];

								GetWindowText( GetDlgItem( g_XfrPrtclStngDlgWndHdl, MaxCnctTimesEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								g_MyMediaPocsThrdPt->m_Ntwk.m_MaxCnctTimes = _wtoi( p_TmpStrPt );
							}
							
							//设置创建服务端或者客户端标记。
							g_MyMediaPocsThrdPt->m_Ntwk.m_IsCreateSrvrOrClnt = ( LOWORD( wParam ) == CreateSrvrBtnId ) ? 1 : 0;

							//设置是否自动允许连接。
							g_MyMediaPocsThrdPt->m_Ntwk.m_IsAutoAllowCnct = ( IsDlgButtonChecked( g_XfrPrtclStngDlgWndHdl, IsAutoAllowCnctCkBoxId ) == BST_CHECKED ) ? 1 : 0;
						}
						
						//设置是否使用链表。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseLnkLstRecvOtptFrmRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->m_UseWhatRecvOtptFrm = 0;
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

							g_MyMediaPocsThrdPt->m_UseWhatRecvOtptFrm = 1;
							g_MyMediaPocsThrdPt->m_AAjb.m_MinNeedBufFrmCnt = p_AAjbMinNeedBufFrmCnt;
							g_MyMediaPocsThrdPt->m_AAjb.m_MaxNeedBufFrmCnt = p_AAjbMaxNeedBufFrmCnt;
							g_MyMediaPocsThrdPt->m_AAjb.m_MaxCntuLostFrmCnt = p_AAjbMaxCntuLostFrmCnt;
							g_MyMediaPocsThrdPt->m_AAjb.m_AdaptSensitivity = p_AAjbAdaptSensitivity;
							g_MyMediaPocsThrdPt->m_VAjb.m_MinNeedBufFrmCnt = p_VAjbMinNeedBufFrmCnt;
							g_MyMediaPocsThrdPt->m_VAjb.m_MaxNeedBufFrmCnt = p_VAjbMaxNeedBufFrmCnt;
							g_MyMediaPocsThrdPt->m_VAjb.m_AdaptSensitivity = p_VAjbAdaptSensitivity;
						}

						//设置是否打印Log日志、显示Toast。
						g_MyMediaPocsThrdPt->SetIsPrintLogShowToast(
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							NULL,
							&g_ErrInfoVstr );

						//设置是否使用阻止系统休眠。
						g_MyMediaPocsThrdPt->SetIsUsePrvntSysSleep(
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							&g_ErrInfoVstr );

						//设置是否保存音视频输入输出到文件。
						g_MyMediaPocsThrdPt->SetIsSaveAdoVdoInptOtptToAviFile(
							g_AdoVdoInptOtptAviFileFullPathVstr.m_VstrPt,
							8 * 1024,
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
							&g_ErrInfoVstr );

						//设置音频输入。
						g_MyMediaPocsThrdPt->SetAdoInpt(
							( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
							  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
								  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
							  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
							&g_ErrInfoVstr );

						//设置音频输入是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
						g_MyMediaPocsThrdPt->AdoInptSetIsUseSystemAecNsAgc( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );

						if( g_MyMediaPocsThrdPt->m_Ntwk.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
						{
							g_MyMediaPocsThrdPt->AdoInptSetUseNoAec( &g_ErrInfoVstr );
						}
						else //如果传输模式为实时全双工。
						{
							//设置音频输入是否不使用声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED )
							{
								g_MyMediaPocsThrdPt->AdoInptSetUseNoAec( &g_ErrInfoVstr );
							}

							//设置音频输入是否使用Speex声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_FilterLenMsec;
								int32_t p_IsUseRec;
								float p_EchoMultiple;
								float p_EchoCont;
								int32_t p_EchoSupes;
								int32_t p_EchoSupesAct;
								int p_IsSaveMemFile;

								GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_FilterLenMsec = _wtoi( p_TmpStrPt );
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

								g_MyMediaPocsThrdPt->AdoInptSetUseSpeexAec( p_FilterLenMsec, p_IsUseRec, p_EchoMultiple, p_EchoCont, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, Cu8vstr( ".\\SpeexAecMem" ), &g_ErrInfoVstr );
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

								g_MyMediaPocsThrdPt->AdoInptSetUseWebRtcAecm( p_IsUseCNGMode, p_EchoMode, p_Delay, &g_ErrInfoVstr );
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

								g_MyMediaPocsThrdPt->AdoInptSetUseWebRtcAec( p_EchoMode, p_Delay, p_IsUseDelayAgstcMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, Cu8vstr( ".\\WebRtcAecMem" ), &g_ErrInfoVstr );
							}

							//设置音频输入是否使用SpeexWebRtc三重声学回音消除器。
							if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED )
							{
								wchar_t p_TmpStrPt[100];
								int32_t p_WorkMode;
								int32_t p_SpeexAecFilterLenMsec;
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
								GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
								p_SpeexAecFilterLenMsec = _wtoi( p_TmpStrPt );
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

								g_MyMediaPocsThrdPt->AdoInptSetUseSpeexWebRtcAec( p_WorkMode, p_SpeexAecFilterLenMsec, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCont, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgstcMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, &g_ErrInfoVstr );
							}
						}

						//设置音频输入是否不使用噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoInptSetUseNoNs( &g_ErrInfoVstr );
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

							g_MyMediaPocsThrdPt->AdoInptSetUseSpeexPrpocsNs( p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, &g_ErrInfoVstr );
						}

						//设置音频输入是否使用WebRtc定点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = _wtoi( p_TmpStrPt );

							g_MyMediaPocsThrdPt->AdoInptSetUseWebRtcNsx( p_PolicyMode, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用WebRtc浮点版噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED )
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_PolicyMode;

							GetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_PolicyMode = _wtoi( p_TmpStrPt );

							g_MyMediaPocsThrdPt->AdoInptSetUseWebRtcNs( p_PolicyMode, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用RNNoise噪音抑制器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoInptSetUseRNNoise( &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用Speex预处理器。
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

							p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbStart = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VadProbCont = _wtoi( p_TmpStrPt );
							p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcLevel = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcIncrement = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcDecrement = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_AgcMaxGain = _wtoi( p_TmpStrPt );

							g_MyMediaPocsThrdPt->AdoInptSetIsUseSpeexPrpocs( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId ) == BST_CHECKED ) ? 1 : 0, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, &g_ErrInfoVstr );
						}
						
						//设置音频输入是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoInptSetUsePcm( &g_ErrInfoVstr );
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
							
							g_MyMediaPocsThrdPt->AdoInptSetUseSpeexEncd( p_EncdUseCbrOrVbr, p_EncdQualt, p_EncdCmplxt, p_EncdPlcExptLossRate, &g_ErrInfoVstr );
						}

						//设置音频输入是否使用Opus编码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoInptSetUseOpusEncd( &g_ErrInfoVstr );
						}
						
						//设置音频输入是否绘制音频波形到窗口。
						g_MyMediaPocsThrdPt->AdoInptSetIsDrawAdoWavfmToWnd( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ),
																			GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ),
																			&g_ErrInfoVstr );

						//设置音频输入是否保存音频到Wave文件。
						g_MyMediaPocsThrdPt->AdoInptSetIsSaveAdoToWaveFile( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			g_AdoInptSrcWaveFileFullPathVstr.m_VstrPt,
																			g_AdoInptRsltWaveFileFullPathVstr.m_VstrPt,
																			8 * 1024,
																			&g_ErrInfoVstr );

						//设置音频输入使用的设备。
						g_MyMediaPocsThrdPt->AdoInptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
															   &g_ErrInfoVstr );

						//设置音频输入是否静音。
						g_MyMediaPocsThrdPt->AdoInptSetIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															   &g_ErrInfoVstr );

						//设置音频输出。
						g_MyMediaPocsThrdPt->SetAdoOtpt(
							( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
							  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
								  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
							( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
							  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
							&g_ErrInfoVstr );
						g_MyMediaPocsThrdPt->AdoOtptAddStrm( 0, &g_ErrInfoVstr );
						
						//设置音频输出是否使用PCM原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoOtptSetStrmUsePcm( 0, &g_ErrInfoVstr );
						}
						
						//设置音频输出是否使用Speex解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
						{
							int32_t p_DecdIsUsePrcplEnhsmt;

							p_DecdIsUsePrcplEnhsmt = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId ) == BST_CHECKED );

							g_MyMediaPocsThrdPt->AdoOtptSetStrmUseSpeexDecd( 0, p_DecdIsUsePrcplEnhsmt, &g_ErrInfoVstr );
						}

						//设置音频输出是否使用Opus解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->AdoOtptSetStrmUseOpusDecd( 0, &g_ErrInfoVstr );
						}
						
						//设置音频输出流是否使用。
						g_MyMediaPocsThrdPt->AdoOtptSetStrmIsUse( 0, 1, &g_ErrInfoVstr );
						
						//设置音频输出是否绘制音频波形到窗口。
						g_MyMediaPocsThrdPt->AdoOtptSetIsDrawAdoWavfmToWnd( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ),
																			&g_ErrInfoVstr );
						
						//设置音频输出是否保存音频到Wave文件。
						g_MyMediaPocsThrdPt->AdoOtptSetIsSaveAdoToWaveFile( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			g_AdoOtptSrcWaveFileFullPathVstr.m_VstrPt,
																			8 * 1024,
																			& g_ErrInfoVstr );

						//设置音频输出使用的设备。
						g_MyMediaPocsThrdPt->AdoOtptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
															   &g_ErrInfoVstr );
						
						//设置音频输出是否静音。
						g_MyMediaPocsThrdPt->AdoOtptSetIsMute( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
															   &g_ErrInfoVstr );
						
						//设置视频输入。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId ) == BST_CHECKED ) //如果要使用预设的帧大小。
						{
							g_MyMediaPocsThrdPt->SetVdoInpt(
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
								  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
									( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
								( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 0 ) ? 120 :
								  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 1 ) ? 240 :
									( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 2 ) ? 480 :
									  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 3 ) ? 960 : 0,
								( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 0 ) ? 160 :
								  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 1 ) ? 320 :
									( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 2 ) ? 640 :
									  ( SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_GETCURSEL, 0, 0 ) == 3 ) ? 1280 : 0,
								g_VdoInptPrvwWndHdl,
								&g_ErrInfoVstr );
						}
						else //如果要使用其他的帧大小。
						{
							wchar_t p_TmpStrPt[100];
							int32_t p_VdoFrmSzOtherWidth;
							int32_t p_VdoFrmSzOtherHeight;
							
							GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VdoFrmSzOtherWidth = _wtoi( p_TmpStrPt );
							GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), p_TmpStrPt, sizeof( p_TmpStrPt ) );
							p_VdoFrmSzOtherHeight = _wtoi( p_TmpStrPt );

							g_MyMediaPocsThrdPt->SetVdoInpt(
								( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
								  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
									( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
									  ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
								p_VdoFrmSzOtherWidth,
								p_VdoFrmSzOtherHeight,
								g_VdoInptPrvwWndHdl,
								&g_ErrInfoVstr );
						}
						
						//设置视频输入是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->VdoInptSetUseYU12( &g_ErrInfoVstr );
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

							g_MyMediaPocsThrdPt->VdoInptSetUseOpenH264Encd( p_OpenH264EncdVdoType,
																			p_OpenH264EncdEncdBitrate * 1024 * 8,
																			p_OpenH264EncdBitrateCtrlMode,
																			p_OpenH264EncdIDRFrmIntvl,
																			p_OpenH264EncdCmplxt,
																			&g_ErrInfoVstr );
						}
						
						//设置视频输入使用的设备。
						g_MyMediaPocsThrdPt->VdoInptSetUseDvc( SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
															   &g_ErrInfoVstr );
						
						//设置视频输入是否黑屏。
						g_MyMediaPocsThrdPt->VdoInptSetIsBlack( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																& g_ErrInfoVstr );

						//设置视频输出。
						g_MyMediaPocsThrdPt->VdoOtptAddStrm( 0, NULL );
						g_MyMediaPocsThrdPt->VdoOtptSetStrm(
							0,
							g_VdoOtptDspyWndHdl,
							&g_ErrInfoVstr );
						
						//设置视频输出是否使用YU12原始数据。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYU12RdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->VdoOtptSetStrmUseYU12( 0, &g_ErrInfoVstr );
						}
						
						//设置视频输出是否使用OpenH264解码器。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
						{
							g_MyMediaPocsThrdPt->VdoOtptSetStrmUseOpenH264Decd( 0, 0, &g_ErrInfoVstr );
						}
						
						//设置视频输出是否黑屏。
						g_MyMediaPocsThrdPt->VdoOtptSetStrmIsBlack( 0,
																	( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																	&g_ErrInfoVstr );
						
						//设置视频输出流是否使用。
						g_MyMediaPocsThrdPt->VdoOtptSetStrmIsUse( 0, 1, &g_ErrInfoVstr );

						//设置本端对讲模式。
						MyMediaPocsThrdCls::UserMsgLclTkbkMode * p_UserMsgLclTkbkModePt = new MyMediaPocsThrdCls::UserMsgLclTkbkMode;
						p_UserMsgLclTkbkModePt->m_LclTkbkMode =
							( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ) == BST_CHECKED ) ? MyMediaPocsThrdCls::TkbkModeAdo :
								( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? MyMediaPocsThrdCls::TkbkModeVdo :
									( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? MyMediaPocsThrdCls::TkbkModeAdoVdo : MyMediaPocsThrdCls::TkbkModeNoChg;
						g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgLclTkbkModePt, NULL );

						//设置是否保存设置到文件。
						if( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId ) == BST_CHECKED )
							g_MyMediaPocsThrdPt->SaveStngToFile( g_StngFileFullPathVstr.m_VstrPt,
																 &g_ErrInfoVstr );
						
						//启动媒体处理线程。
						if( g_MyMediaPocsThrdPt->Start( &g_ErrInfoVstr ) != 0 )
						{
							LOGFE( Cu8vstr( "启动媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
							goto OutCreateSrvrOrClnt;
						}

						LOGI( Cu8vstr( "启动媒体处理线程完毕。" ) );

						p_Rslt = 0; // //设置本函数执行成功。

						OutCreateSrvrOrClnt:
						if( p_Rslt != 0 )
						{
							if( g_MyMediaPocsThrdPt != NULL )
							{
								g_MyMediaPocsThrdPt->Dstoy( &g_ErrInfoVstr ); //销毁媒体处理线程。
								delete g_MyMediaPocsThrdPt;
								g_MyMediaPocsThrdPt = NULL;
							}
						}
					}
					else //如果媒体处理线程已经启动。
					{
						g_MyMediaPocsThrdPt->RqirExit( 1, 1, &g_ErrInfoVstr ); //请求媒体处理线程退出。

						p_Rslt = 0; // //设置本函数执行成功。
					}
					return ( p_Rslt == 0 ) ? ( INT_PTR )TRUE : ( INT_PTR )FALSE;
				}
				case RqstCnctAllowBtnId: //请求连接对话框的允许按钮。
				{
					g_MyMediaPocsThrdPt->m_Ntwk.m_RqstCnctRslt = 1;
					EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框。
					return ( INT_PTR )TRUE;
				}
				case RqstCnctRefuseBtnId: //请求连接对话框的拒绝按钮。
				{
					g_MyMediaPocsThrdPt->m_Ntwk.m_RqstCnctRslt = 2;
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
					CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecmRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseSpeexPrpocsNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate12RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
					SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"120×160" );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), L"500" );
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
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), L"500" );
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

					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_UNCHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

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
					CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsxRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
					SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"240×320" );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), L"500" );
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
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), L"500" );
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

					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

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
					CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
					SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"480×640" );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), L"500" );
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
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), L"500" );
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

					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

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
					CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate24RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
					SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"480×640" );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), L"500" );
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
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), L"500" );
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

					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

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
					CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
					CheckDlgButton( g_StngDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate30RdBtnId );
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
					SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"960×1280" );
					CheckRadioButton( g_StngDlgWndHdl, UseYU12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

					SetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), L"500" );
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
					SetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), L"500" );
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

					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), L"95" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), L"95" );
					CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), L"20000" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), L"10" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), L"-200" );
					SetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), L"20" );

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
				case SpeexPrpocsStngBtnId: //Speex预处理器的设置按钮。
				{
					ShowWindow( g_SpeexPrpocsStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示Speex预处理器的设置对话框。
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
				case VdoOtptDspyTxtId: //视频输出显示文本框。
				{
					SetVdoWndShowMode( g_VdoWndShowMode + 1 ); //设置视频窗口的显示模式。
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
						GetWinLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除Speex声学回音消除器的内存块文件 SpeexAecMem 失败。原因：" ) );
						StrU16TmpCpy( p_TmpU16strPt, g_ErrInfoVstr.m_VstrPt->m_Pt, g_ErrInfoVstr.m_VstrPt->m_ChrSet );
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
						GetWinLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 失败。原因：" ) );
						StrU16TmpCpy( p_TmpU16strPt, g_ErrInfoVstr.m_VstrPt->m_Pt, g_ErrInfoVstr.m_VstrPt->m_ChrSet );
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
				case SpeexPrpocsStngOkBtnId: //Speex预处理器的设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SpeexPrpocsStngDlgWndHdl ), TRUE ), ShowWindow( g_SpeexPrpocsStngDlgWndHdl, SW_HIDE ); //隐藏Speex预处理器的设置对话框。
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

						if( g_MyMediaPocsThrdPt != NULL ) g_MyMediaPocsThrdPt->RqirExit( 1, 1, &g_ErrInfoVstr ); //请求媒体处理线程退出。

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
		case WM_SYSCOMMAND: //系统命令消息。
		{
			/*switch( wParam )
			{
				case 
			}*/
			return ( INT_PTR )FALSE;
		}
		case WM_DESTROY: //窗口销毁消息。
		{
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgMediaPocsThrdInit: //初始化媒体处理线程消息。
		{
			if( g_MyMediaPocsThrdPt->m_Ntwk.m_IsCreateSrvrOrClnt == 1 ) //如果是创建服务端。
            {
				EnableWindow( GetDlgItem( hDlg, UseTcpPrtclRdBtnId ), FALSE ); //设置TCP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, UseUdpPrtclRdBtnId ), FALSE ); //设置UDP协议单选按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, XfrPrtclStngBtnId ), FALSE ); //设置传输协议设置按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, IPAddrCbBoxId ), FALSE ); //设置IP地址组合框为不可用。
				EnableWindow( GetDlgItem( hDlg, PortEdTxtId ), FALSE ); //设置端口编辑框为不可用。
				SetWindowText( GetDlgItem( hDlg, CreateSrvrBtnId ), L"中断" ); //设置创建服务端按钮的内容为“中断”。
				EnableWindow( GetDlgItem( hDlg, CnctSrvrBtnId ), FALSE ); //设置连接服务端按钮为不可用。
				EnableWindow( GetDlgItem( hDlg, StngBtnId ), FALSE ); //设置设置按钮为不可用。
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
            }
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgMediaPocsThrdDstoy: //销毁媒体处理线程消息。
		{
			if( g_MyMediaPocsThrdPt != NULL )
			{
				g_MyMediaPocsThrdPt->Dstoy( &g_ErrInfoVstr ); //销毁媒体处理线程。
				delete g_MyMediaPocsThrdPt;
				g_MyMediaPocsThrdPt = NULL;
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
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgRqstCnctDlgInit: //显示请求连接对话框消息。
		{
			if( g_MyMediaPocsThrdPt->m_Ntwk.m_IsCreateSrvrOrClnt == 1 )
			{
				VstrIns( ( Vstr * )wParam, 0, Cu8vstr( "您是否允许远端[" ) );
				VstrCat( ( Vstr * )wParam, Cu8vstr( "]的连接？" ) );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( wchar_t * )( ( Vstr * )wParam )->m_Pt ); //设置请求连接对话框文本框的内容。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), L"允许" ); //设置请求连接对话框允许按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_SHOW ); //设置请求连接对话框允许按钮为显示。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), L"拒绝" ); //设置请求连接对话框拒绝按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝按钮为显示。
			}
			else
			{
				VstrIns( ( Vstr * )wParam, 0, Cu8vstr( "等待远端[" ) );
				VstrCat( ( Vstr * )wParam, Cu8vstr( "]允许您的连接..." ) );
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctTipTxtId ), ( wchar_t * )( ( Vstr * )wParam )->m_Pt ); //设置请求连接对话框的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctAllowBtnId ), SW_HIDE ); //设置请求连接对话框允许按钮为隐藏。
				SetWindowText( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), L"中断" ); //设置请求连接对话框拒绝按钮的内容。
				ShowWindow( GetDlgItem( g_RqstCnctDlgWndHdl, RqstCnctRefuseBtnId ), SW_SHOW ); //设置请求连接对话框拒绝按钮为显示。
			}
			VstrDstoy( ( Vstr * )wParam );
			ShowWindow( g_RqstCnctDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示请求连接对话框。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgRqstCnctDlgDstoy: //销毁请求连接对话框消息。
		{
			EnableWindow( g_MainDlgWndHdl, TRUE ), ShowWindow( g_RqstCnctDlgWndHdl, SW_HIDE ); //隐藏请求连接对话框。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgPttBtnInit: //初始化一键即按即通按钮消息。
		{
			ShowWindow( g_PttDlgWndHdl, SW_SHOW ); //设置一键即按即通对话框为显示。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgPttBtnDstoy: //销毁一键即按即通按钮消息。
		{
			ShowWindow( g_PttDlgWndHdl, SW_HIDE ); //设置一键即按即通对话框为隐藏。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgShowLog: //显示日志消息。
		{
			if( ( VstrCls * )wParam != NULL )
			{
				Vstr * p_LogVstrPt = ( Vstr * )wParam;
				uint16_t * p_LogU16strTmpPt;

				//显示日志。
				{
					int p_Hour, p_Min, p_Sec, p_Msec;
					FuncGetTimeAsLclDateTime( NULL, NULL, NULL, &p_Hour, &p_Min, &p_Sec, &p_Msec );
					VstrFmtIns( p_LogVstrPt, 0, Cu8vstr( "%02.2d:%02.2d:%02.2d %03.3d：" ), p_Hour, p_Min, p_Sec, p_Msec );
					p_LogU16strTmpPt = ( uint16_t * )alloca( ( p_LogVstrPt->m_LenChr + 1 ) * 2 );
					StrU16TmpCpy( p_LogU16strTmpPt, p_LogVstrPt->m_Pt, p_LogVstrPt->m_ChrSet );
					SendMessage( GetDlgItem( hDlg, LogLtBoxId ), LB_ADDSTRING, 0, ( WPARAM )p_LogU16strTmpPt );
				}

				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, LogLtBoxId, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( wchar_t *)p_LogU16strTmpPt, p_LogVstrPt->m_LenChr, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, LogLtBoxId, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}

				VstrDstoy( p_LogVstrPt );
			}
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
	//_CrtSetBreakAlloc( 711 );
	#endif

	g_IstnsHdl = hInstance; //设置当前实例的句柄。
	
	FuncSetCurActPath( NULL, NULL ); //设置当前进程活动目录的路径为为当前进程可执行文件的上级路径。
	
	//初始化错误信息动态字符串。
	g_ErrInfoVstr.Init();
	g_StngFileFullPathVstr.Init( Utf8, 4, Cu8vstr( "Setting.txt" ) );
	g_AdoVdoInptOtptAviFileFullPathVstr.Init( Utf8, 4, Cu8vstr( "AdoVdoInptOtpt.avi" ) );
	g_AdoInptSrcWaveFileFullPathVstr.Init( Utf8, 4, Cu8vstr( "AdoInptSrc.wav" ) );
	g_AdoInptRsltWaveFileFullPathVstr.Init( Utf8, 4, Cu8vstr( "AdoInptRslt.wav" ) );
	g_AdoOtptSrcWaveFileFullPathVstr.Init( Utf8, 4, Cu8vstr( "AdoOtptSrc.wav" ) );
	
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
	
	//判断应用程序可执行文件的名称是否为“Windows下音视频对讲演示程序.exe”或“WndAdoVdoTkbkDemo*”。
	{
		HANDLE p_CurProcHdl;
		wchar_t p_PocsImageFileName[ MAX_PATH ];
		const wchar_t * p_AppFileNamePt;

		//获取Windows应用程序的进程名字符串。
		p_CurProcHdl = GetCurrentProcess();
		GetProcessImageFileName( p_CurProcHdl, p_PocsImageFileName, sizeof( p_PocsImageFileName ) );
		FuncPathFindFileName( Cu16vstr( p_PocsImageFileName ), ( const void * * )&p_AppFileNamePt );
		CloseHandle( p_CurProcHdl );
		if( ( memcmp( p_AppFileNamePt, L"Windows下音视频对讲演示程序.exe", ( wcslen( L"Windows下音视频对讲演示程序.exe" ) + 1 ) * sizeof( wchar_t ) ) != 0 ) &&
			( memcmp( p_AppFileNamePt, L"WndAdoVdoTkbkDemo", wcslen( L"WndAdoVdoTkbkDemo" ) * sizeof( wchar_t ) ) != 0 ) )
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
		g_SpeexPrpocsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexPrpocsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexCodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexCodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_OpenH264CodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( OpenH264CodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
	}
	
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
	SendMessage( g_MainDlgWndHdl, WM_DEVICECHANGE, 0, 0 );

	//设置默认设置。
	{
		CheckRadioButton( g_StngDlgWndHdl, UseLnkLstRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_CHECKED );

		SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), L"640" );
		SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), L"480" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"120×160" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"240×320" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"480×640" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"960×1280" );

		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseEffectSuperRdBtnId, 0 ); //默认效果等级：超。
		SendMessage( g_MainDlgWndHdl, WM_COMMAND, UseBitrateSuperRdBtnId, 0 ); //默认比特率等级：超。
		
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), L"5" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), L"1.0" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), L"3" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), L"24" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), L"1.0" );
	}

	//设置句柄和高度和宽度和边距。
	{
		g_LogLtBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, LogLtBoxId ); //设置日志列表框窗口的句柄。
		g_VdoInptPrvwTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoInptPrvwTxtId ); //设置视频输入预览文本框窗口的句柄。
		g_VdoOtptDspyTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoOtptDspyTxtId ); //设置视频输出显示文本框窗口的句柄。
		g_VdoInptPrvwWndHdl = g_VdoInptPrvwTxtWndHdl;
		g_VdoOtptDspyWndHdl = g_VdoOtptDspyTxtWndHdl;

		RECT p_MainDlgWndRect;
		RECT p_VdoInptPrvwTxtWndRect;
		RECT p_VdoOtptDspyTxtWndRect;
		
		//GetWindowRect( g_MainDlgWndHdl, &p_MainDlgWndRect );
		//g_MainDlgWndMinWidth = RectWidth( p_MainDlgWndRect ); //设置主对话框窗口的最小宽度，单位为像素。
		//g_MainDlgWndMinHeight = RectHeight( p_MainDlgWndRect ); //设置主对话框窗口的最小高度，单位为像素。
		g_MainDlgWndMinWidth = 650; //设置主对话框窗口的最小宽度，单位为像素。
		g_MainDlgWndMinHeight = 550; //设置主对话框窗口的最小高度，单位为像素。
		
		//GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect );
		//GetWindowRect( g_VdoInptPrvwTxtWndHdl, &p_VdoInptPrvwTxtWndRect );
		//RectScrnToClnt( g_MainDlgWndHdl, p_VdoInptPrvwTxtWndRect );
		//GetWindowRect( g_VdoOtptDspyTxtWndHdl, &p_VdoOtptDspyTxtWndRect );
		//RectScrnToClnt( g_MainDlgWndHdl, p_VdoOtptDspyTxtWndRect );
		//g_VdoTxtWndLeftMargin = p_VdoInptPrvwTxtWndRect.left - p_MainDlgWndRect.left; //设置视频文本框窗口的左边距，单位为像素。
		//g_VdoTxtWndTopMargin = p_VdoInptPrvwTxtWndRect.top - p_MainDlgWndRect.top; //设置视频文本框窗口的顶边距，单位为像素。
		//g_VdoTxtWndRightMargin = p_MainDlgWndRect.right - p_VdoOtptDspyTxtWndRect.right; //设置视频文本框窗口的右边距，单位为像素。
		//g_VdoTxtWndBottomMargin = p_MainDlgWndRect.bottom - p_VdoOtptDspyTxtWndRect.bottom; //设置视频文本框窗口的底边距，单位为像素。
		g_VdoTxtWndLeftMargin = 455; //设置视频文本框窗口的左边距，单位为像素。
		g_VdoTxtWndTopMargin = 11; //设置视频文本框窗口的顶边距，单位为像素。
		g_VdoTxtWndRightMargin = 10; //设置视频文本框窗口的右边距，单位为像素。
		g_VdoTxtWndBottomMargin = 12; //设置视频文本框窗口的底边距，单位为像素。
	}
	
	//打印当前进程活动目录的完整绝对路径到日志。
	FuncGetCurActPath( g_ErrInfoVstr.m_VstrPt, g_ErrInfoVstr.m_VstrPt );
	g_ErrInfoVstr.Ins( 0, Cu8vstr( "当前进程活动目录的完整绝对路径：" ) );
	LOGI( g_ErrInfoVstr.m_VstrPt );
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	
	//显示主对话框窗口。放在初始化最后才显示，这样用户看不见初始化的过程。
	ShowWindow( g_MainDlgWndHdl, SW_SHOW );
	
	//处理命令行。
	{
		int p_Argc;
		wchar_t * * p_Argv = CommandLineToArgvW( lpCmdLine, &p_Argc ); //获取命令行参数的指针数组。

		for( int p_Argn = 0; p_Argn < p_Argc; p_Argn++ )
		{
			if( ( wcscmp( p_Argv[ p_Argn ], L"-tcp" ) == 0 ) ||
				( wcscmp( p_Argv[ p_Argn ], L"-Tcp" ) == 0 ) ||
				( wcscmp( p_Argv[ p_Argn ], L"-TCP" ) == 0 ) )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseTcpPrtclRdBtnId, UseUdpPrtclRdBtnId, UseTcpPrtclRdBtnId );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-udp" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-Udp" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-UDP" ) == 0 ) )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseTcpPrtclRdBtnId, UseUdpPrtclRdBtnId, UseUdpPrtclRdBtnId );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-ip" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-Ip" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-IP" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_MainDlgWndHdl, IPAddrCbBoxId, p_Argv[ p_Argn ] );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-port" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-Port" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-PORT" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_MainDlgWndHdl, PortEdTxtId, p_Argv[ p_Argn ] );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-ado" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-Ado" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-ADO" ) == 0 ) )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseAdoTkbkModeRdBtnId );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-vdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-Vdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VDO" ) == 0 ) )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseVdoTkbkModeRdBtnId );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-adovdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-AdoVdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-ADOVDO" ) == 0 ) )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-savestng" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SaveStng" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SAVESTNG" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					g_StngFileFullPathVstr.Cpy( Cu16vstr( p_Argv[ p_Argn ] ) );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId, BST_CHECKED );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-nosavestng" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NoSaveStng" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NOSAVESTNG" ) == 0 ) )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveStngToFileCkBoxId, BST_UNCHECKED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-printlogshowtoast" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-PrintLogShowToast" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-PRINTLOGSHOWTOAST" ) == 0 ) )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-noprintlogshowtoast" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NoPrintLogShowToast" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NOPRINTLOGSHOWTOAST" ) == 0 ) )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_UNCHECKED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-saveadovdoinptotpt" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SaveAdoVdoInptOtpt" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SAVEADOVDOINPTOTPT" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					g_AdoVdoInptOtptAviFileFullPathVstr.Cpy( Cu16vstr( p_Argv[ p_Argn ] ) );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_CHECKED );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-nosaveadovdoinptotpt" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NoSaveAdoVdoInptOtpt" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NOSAVEADOVDOINPTOTPT" ) == 0 ) )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_UNCHECKED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-saveado" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SaveAdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SAVEADO" ) == 0 ) )
			{
				if( p_Argn + 3 < p_Argc )
				{
					p_Argn++;
					g_AdoInptSrcWaveFileFullPathVstr.Cpy( Cu16vstr( p_Argv[ p_Argn ] ) );
					p_Argn++;
					g_AdoInptRsltWaveFileFullPathVstr.Cpy( Cu16vstr( p_Argv[ p_Argn ] ) );
					p_Argn++;
					g_AdoOtptSrcWaveFileFullPathVstr.Cpy( Cu16vstr( p_Argv[ p_Argn ] ) );
					CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_CHECKED );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-nosaveado" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NoSaveAdo" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-NOSAVEADO" ) == 0 ) )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoToWaveFileCkBoxId, BST_UNCHECKED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-vdoinptprvwwndhdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VdoInptPrvwWndHdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VDOINPTPRVWWNDHDL" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &g_VdoInptPrvwWndHdl, sizeof( g_VdoInptPrvwWndHdl ) );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-vdootptdspywndhdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VdoOtptDspyWndHdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VDOOTPTDSPYWNDHDL" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &g_VdoOtptDspyWndHdl, sizeof( g_VdoOtptDspyWndHdl ) );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-vdowndshowmode" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VdoWndShowMode" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VDOWNDSHOWMODE" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					int p_VdoWndShowMode;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &p_VdoWndShowMode, sizeof( p_VdoWndShowMode ) );
					SetVdoWndShowMode( p_VdoWndShowMode );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-vdofrmsz" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VdoFrmSz" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-VDOFRMSZ" ) == 0 ) )
			{
				if( p_Argn + 2 < p_Argc )
				{
					CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzOtherRdBtnId );
					p_Argn++;
					SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), p_Argv[ p_Argn ] );
					p_Argn++;
					SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), p_Argv[ p_Argn ] );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-wndsz" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-WndSz" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-WNDSZ" ) == 0 ) )
			{
				if( p_Argn + 2 < p_Argc )
				{
					int p_MainDlgWndWdith = 800;
					int p_MainDlgWndHeight = 400;

					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &p_MainDlgWndWdith, sizeof( p_MainDlgWndWdith ) );
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &p_MainDlgWndHeight, sizeof( p_MainDlgWndHeight ) );
					
					SetWindowPos( g_MainDlgWndHdl, NULL, 0, 0, p_MainDlgWndWdith, p_MainDlgWndHeight, SWP_NOZORDER | SWP_NOMOVE ); //设置主对话框窗口的位置。
					FuncSetWndCenter( g_MainDlgWndHdl, NULL, NULL );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-showwnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-ShowWnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-SHOWWND" ) == 0 ) )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOW );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-hidewnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-HideWnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-HIDEWND" ) == 0 ) )
			{
				ShowWindow( g_MainDlgWndHdl, SW_HIDE );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-minwnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-MinWnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-MINWND" ) == 0 ) )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOWMINIMIZED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-maxwnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-MaxWnd" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-MAXWND" ) == 0 ) )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOWMAXIMIZED );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-tkbkstswndhdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-TkbkStsWndHdl" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-TKBKSTSWNDHDL" ) == 0 ) )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &g_TkbkStsWndHdl, sizeof( g_TkbkStsWndHdl ) );
				}
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-createsrvr" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-CreateSrvr" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-CREATESRVR" ) == 0 ) )
			{
				SendMessage( g_MainDlgWndHdl, WM_COMMAND, CreateSrvrBtnId, 0 );
			}
			else if( ( wcscmp( p_Argv[ p_Argn ], L"-cnctsrvr" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-CnctSrvr" ) == 0 ) ||
					 ( wcscmp( p_Argv[ p_Argn ], L"-CNCTSRVR" ) == 0 ) )
			{
				SendMessage( g_MainDlgWndHdl, WM_COMMAND, CnctSrvrBtnId, 0 );
			}
		}

		LocalFree( p_Argv ); //销毁命令行参数的指针数组。
	}

    //主消息循环。
	MSG p_Msg;
	while( GetMessage( &p_Msg, NULL, 0, 0 ) )
	{
		//判断一键即按即通按钮是否按下。
		if( p_Msg.hwnd == g_PttBtnWndHdl )
		{
			if( p_Msg.message == WM_LBUTTONDOWN )
			{
				MyMediaPocsThrdCls::UserMsgPttBtnDown * p_UserMsgPttBtnDownPt = new MyMediaPocsThrdCls::UserMsgPttBtnDown;
				g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgPttBtnDownPt, NULL );
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				MyMediaPocsThrdCls::UserMsgPttBtnUp * p_UserMsgPttBtnUpPt = new MyMediaPocsThrdCls::UserMsgPttBtnUp;
				g_MyMediaPocsThrdPt->SendUserMsg( p_UserMsgPttBtnUpPt, NULL );
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
	_ASSERTE( _CrtCheckMemory() ); //检查堆和栈是否被破坏。
	#endif
	return (int) p_Msg.wParam;
}
