// WndAdoVdoTkbkDemo.cpp : 定义应用程序的入口点。
//
#if( defined __X86__ )
#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"" )
#elif( defined __X64__ )
#pragma comment( linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"" )
#endif

#include "WndAdoVdoTkbkDemo.h"
#include "WndAdoVdoTkbkStng.h"
#include "MySrvrThrd.h"
#include "MyClntMediaPocsThrd.h"

//全局变量。
HINSTANCE g_IstnsHdl; //存放当前实例的句柄。
VstrCls g_ErrInfoVstr; //存放错误信息动态字符串的指针。
MySrvrThrdCls * g_MySrvrThrdPt = NULL; //存放我的服务端线程的指针。
MyClntMediaPocsThrdCls * g_MyClntMediaPocsThrdPt = NULL; //存放我的客户端媒体处理线程的指针。
HWND g_MainDlgWndHdl = NULL; //存放主对话框窗口的句柄。
long g_MainDlgWndMinHeight = 0; //存放主对话框窗口的最小高度，单位为像素。
long g_MainDlgWndMinWidth = 0; //存放主对话框窗口的最小宽度，单位为像素。
HWND g_SrvrStngDlgWndHdl = NULL; //存放服务端设置对话框窗口的句柄。
HWND g_CnctLstWndHdl = NULL; //存放连接列表窗口的句柄。
HWND g_ClntStngDlgWndHdl = NULL; //存放客户端设置对话框窗口的句柄。
HWND g_ClntLstWndHdl = NULL; //存放客户端列表窗口的句柄。
HWND g_PtbBtnWndHdl = NULL; //存放一键即按即广播按钮窗口的句柄。
HWND g_AdoInptDvcCbBoxWndHdl = NULL; //存放音频输入设备组合框窗口的句柄。
HWND g_AdoOtptDvcCbBoxWndHdl = NULL; //存放音频输出设备组合框窗口的句柄。
HWND g_VdoInptDvcCbBoxWndHdl = NULL; //存放视频输入设备组合框窗口的句柄。
HWND g_LogLstBoxWndHdl = NULL; //存放日志列表框窗口的句柄。
HWND g_VdoInptOtptTxtWndHdl = NULL; //存放视频输入输出文本框窗口的句柄。
typedef struct VdoInptOtptWnd //视频输入输出窗口。
{
	HWND m_TitleTxtWndHdl; //标题文本框窗口句柄。
	HWND m_DspyTxtWndHdl; //显示文本框窗口句柄。
} VdoInptOtptWnd;
CQueueCls g_VdoInptOtptWndCntnr; //存放视频输入输出窗口容器。
long g_VdoTxtWndLeftMargin = 0; //存放视频文本框窗口的左边距，单位为像素。
long g_VdoTxtWndTopMargin = 0; //存放视频文本框窗口的顶边距，单位为像素。
long g_VdoTxtWndRightMargin = 0; //存放视频文本框窗口的右边距，单位为像素。
long g_VdoTxtWndBottomMargin = 0; //存放视频文本框窗口的底边距，单位为像素。
int g_VdoWndShowMode = 0; //存放视频窗口的显示模式，为0表示正常，为1表示垂直最大化排列，为2表示水平最大化排列。
HWND g_PttDlgWndHdl = NULL; //存放一键即按即通对话框窗口的句柄。
HWND g_PttBtnWndHdl = NULL; //存放一键即按即通按钮窗口的句柄。

HWND g_StngDlgWndHdl = NULL; //存放设置对话框窗口的句柄。
HWND g_AjbStngDlgWndHdl = NULL; //存放自适应抖动缓冲器设置对话框窗口的句柄。
HWND g_SaveStsToTxtFileStngDlgWndHdl = NULL; //存放保存状态到Txt文件设置对话框窗口的句柄。
HWND g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl = NULL; //存放保存音视频输入输出到Avi文件设置对话框窗口的句柄。
HWND g_SpeexAecStngDlgWndHdl = NULL; //存放Speex声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecmStngDlgWndHdl = NULL; //存放WebRtc定点版声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecStngDlgWndHdl = NULL; //存放WebRtc浮点版声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAec3StngDlgWndHdl = NULL; //存放WebRtc第三版声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexWebRtcAecStngDlgWndHdl = NULL; //存放SpeexWebRtc三重声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexPrpocsNsStngDlgWndHdl = NULL; //存放Speex预处理器的噪音抑制设置对话框窗口的句柄。
HWND g_WebRtcNsxStngDlgWndHdl = NULL; //存放WebRtc定点版噪音抑制器设置对话框窗口的句柄。
HWND g_WebRtcNsStngDlgWndHdl = NULL; //存放WebRtc浮点版噪音抑制器设置对话框窗口的句柄。
HWND g_SpeexPrpocsStngDlgWndHdl = NULL; //存放Speex预处理器的设置对话框窗口的句柄。
HWND g_SpeexCodecStngDlgWndHdl = NULL; //存放Speex编解码器设置对话框窗口的句柄。
HWND g_SaveAdoInptOtptToWaveFileStngDlgWndHdl = NULL; //存放保存音频输入输出到Wave文件设置对话框窗口的句柄。
HWND g_OpenH264CodecStngDlgWndHdl = NULL; //存放OpenH264编解码器设置对话框窗口的句柄。

IMMNotificationClient * g_NotificationClntPt = NULL; //存放音频输入输出设备改变消息客户端的指针。

extern const uint8_t g_LicnCodePt[]; //存放授权码的指针。

//刷新音视频输入输出设备。
void RefreshAdoVdoInptOtptDvc( int IsRefresAdoInptDvc, int IsRefresAdoOtptDvc, int IsRefresVdoInptDvc )
{
	Vstr * * p_AdoInptDvcNameVstrArrPt = NULL;
	UINT p_AdoInptDvcTotal = 0;
	Vstr * * p_AdoOtptDvcNameVstrArrPt = NULL;
	UINT p_AdoOtptDvcTotal = 0;
	Vstr * * p_VdoInptDvcNameVstrArrPt = NULL;
	UINT p_VdoInptDvcTotal = 0;
	wchar_t * p_CurAdoInptDvcNameU16strPt = NULL;
	wchar_t * p_CurAdoOtptDvcNameU16strPt = NULL;
	wchar_t * p_CurVdoInptDvcNameU16strPt = NULL;
	long p_TmpInt;

	//获取音频输入设备、音频输出设备、视频输入设备的名称。
	if( IsRefresAdoInptDvc != 0 )
	{
		if( MediaPocsThrdGetAdoInptDvcName( &p_AdoInptDvcNameVstrArrPt, &p_AdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
		{
			g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输入设备名称失败。原因：" ) );
			LOGE( g_ErrInfoVstr.m_VstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto RefresAdoVdohDvc;
		}
	}
	if( IsRefresAdoOtptDvc != 0 )
	{
		if( MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameVstrArrPt, &p_AdoOtptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
		{
			g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输出设备名称失败。原因：" ) );
			LOGE( g_ErrInfoVstr.m_VstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto RefresAdoVdohDvc;
		}
	}
	if( IsRefresVdoInptDvc != 0 )
	{
		if( MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameVstrArrPt, &p_VdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
		{
			g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取视频输入设备名称失败。原因：" ) );
			LOGE( g_ErrInfoVstr.m_VstrPt );
			{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
			goto RefresAdoVdohDvc;
		}
	}
	
	//获取当前选择的音频输入设备、音频输出设备、视频输入设备的名称。
	if( IsRefresAdoInptDvc != 0 ) CbBoxGetCurSelTxt( g_AdoInptDvcCbBoxWndHdl, p_CurAdoInptDvcNameU16strPt );
	if( IsRefresAdoOtptDvc != 0 ) CbBoxGetCurSelTxt( g_AdoOtptDvcCbBoxWndHdl, p_CurAdoOtptDvcNameU16strPt );
	if( IsRefresVdoInptDvc != 0 ) CbBoxGetCurSelTxt( g_VdoInptDvcCbBoxWndHdl, p_CurVdoInptDvcNameU16strPt );

	//清空音频输入设备、音频输出设备、视频输入设备的组合框。
	if( IsRefresAdoInptDvc != 0 ) SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );
	if( IsRefresAdoOtptDvc != 0 ) SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );
	if( IsRefresVdoInptDvc != 0 ) SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );

	//添加音频输入设备、音频输出设备、视频输入设备到组合框。
	if( IsRefresAdoInptDvc != 0 )
	{
		if( p_AdoInptDvcTotal > 0 )
		{
			for( p_TmpInt = 0; p_TmpInt < p_AdoInptDvcTotal; p_TmpInt++ )
			{
				SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_AdoInptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
			}
		}
		else
		{
			SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
		}
	}
	if( IsRefresAdoOtptDvc != 0 )
	{
		if( p_AdoOtptDvcTotal > 0 )
		{
			for( p_TmpInt = 0; p_TmpInt < p_AdoOtptDvcTotal; p_TmpInt++ )
			{
				SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_AdoOtptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
			}
		}
		else
		{
			SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
		}
	}
	if( IsRefresVdoInptDvc != 0 )
	{
		if( p_VdoInptDvcTotal > 0 )
		{
			for( p_TmpInt = 0; p_TmpInt < p_VdoInptDvcTotal; p_TmpInt++ )
			{
				SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )p_VdoInptDvcNameVstrArrPt[ p_TmpInt ]->m_Pt );
			}
		}
		else
		{
			SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"无" );
		}
	}

	//默认选择第一个音频输入设备、音频输出设备、视频输入设备。这步必须要执行，因为重新选择之前选择的有可能失败。
	if( IsRefresAdoInptDvc != 0 ) SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL ); 
	if( IsRefresAdoOtptDvc != 0 ) SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
	if( IsRefresVdoInptDvc != 0 ) SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );

	//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
	if( p_CurAdoInptDvcNameU16strPt != NULL ) SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoInptDvcNameU16strPt );
	if( p_CurAdoOtptDvcNameU16strPt != NULL ) SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoOtptDvcNameU16strPt );
	if( p_CurVdoInptDvcNameU16strPt != NULL ) SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVdoInptDvcNameU16strPt );

	RefresAdoVdohDvc:
	if( IsRefresAdoInptDvc != 0 ) MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameVstrArrPt, p_AdoInptDvcTotal, NULL );
	if( IsRefresAdoOtptDvc != 0 ) MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameVstrArrPt, p_AdoOtptDvcTotal, NULL );
	if( IsRefresVdoInptDvc != 0 ) MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameVstrArrPt, p_VdoInptDvcTotal, NULL );
}

//设置视频窗口的显示模式。
void SetVdoWndShowMode( int VdoWndShowMode )
{
	RECT p_MainDlgWndRect;
	int p_VdoInptOtptWndWdh;
	int p_VdoInptOtptWndHgt;
	int p_PerTitleTxtWndHgt = 15;
	int p_PerVdoInptOtptWndWdh;
	int p_PerVdoInptOtptWndHgt;
	size_t p_VdoInptOtptWndTotal;
	int p_XTmp;
	VdoInptOtptWnd * p_VdoInptOtptWndTmpPt;

	GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect );
	switch( VdoWndShowMode )
	{
		default:
		case 0: //正常。
		{
			if( g_VdoWndShowMode != 0 ) //如果当前显示模式不是正常。
			{
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_SHOW ); //设置服务端Url组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_SHOW ); //设置服务端创建或销毁按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_SHOW ); //设置服务端设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_SHOW ); //设置连接销毁按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_SHOW ); //设置连接列表为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_SHOW ); //设置客户端的服务端Url组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_SHOW ); //设置编辑框添加按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_SHOW ); //设置客户端设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), SW_SHOW ); //设置客户端连接或销毁按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_SHOW ); //设置客户端删除按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_SHOW ); //设置客户端列表为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ), SW_SHOW ); //设置音频输入对讲模式复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ), SW_SHOW ); //设置音频输出对讲模式复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ), SW_SHOW ); //设置视频输入对讲模式复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ), SW_SHOW ); //设置视频输出对讲模式复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_SHOW ); //设置音频输入静音复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_SHOW ); //设置音频输出静音复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_SHOW ); //设置视频输入黑屏复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_SHOW ); //设置视频输出黑屏复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_SHOW ); //设置音频输入设备组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_SHOW ); //设置音频输出设备组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_SHOW ); //设置视频输入设备组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, PtbBtnId ), SW_SHOW ); //设置一键即按即广播按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_SHOW ); //设置设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), SW_SHOW ); //设置保存设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), SW_SHOW ); //设置读取设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), SW_SHOW ); //设置删除设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), SW_SHOW ); //设置重置设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_SHOW ); //设置必读说明按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ), SW_SHOW ); //设置是否绘制音频波形到窗口复选框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_SHOW ); //设置音频输入原始波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_SHOW ); //设置音频输出原始波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_SHOW ); //设置音频输入结果波形文本框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_SHOW ); //设置清空日志按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLstBoxId ), SW_SHOW ); //设置日志列表框为显示。

				g_VdoWndShowMode = 0;
			}

			p_VdoInptOtptWndWdh = RectWidth( p_MainDlgWndRect ) - g_VdoTxtWndLeftMargin - g_VdoTxtWndRightMargin;
			p_VdoInptOtptWndHgt = RectHeight( p_MainDlgWndRect ) - g_VdoTxtWndTopMargin - g_VdoTxtWndBottomMargin;
			SetWindowPos( g_VdoInptOtptTxtWndHdl, HWND_BOTTOM, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin, p_VdoInptOtptWndWdh, p_VdoInptOtptWndHgt, 0 ); //设置视频输入输出文本框窗口的位置。
			g_VdoInptOtptWndCntnr.GetTotal( &p_VdoInptOtptWndTotal, 0, NULL );
			if( p_VdoInptOtptWndTotal > 0 ) //如果有视频输入输出窗口。
			{
				p_PerVdoInptOtptWndHgt = p_VdoInptOtptWndHgt / p_VdoInptOtptWndTotal - p_PerTitleTxtWndHgt;
				p_XTmp = 0;
				for( size_t p_VdoInptOtptWndIdx = 0; g_VdoInptOtptWndCntnr.GetByNum( p_VdoInptOtptWndIdx, NULL, ( void * * )&p_VdoInptOtptWndTmpPt, 0, 0, NULL ) == 0; p_VdoInptOtptWndIdx++ )
				{
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_TitleTxtWndHdl, NULL, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin + p_XTmp, p_VdoInptOtptWndWdh, p_PerTitleTxtWndHgt, SWP_NOZORDER );
					p_XTmp += p_PerTitleTxtWndHgt;
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl, NULL, g_VdoTxtWndLeftMargin, g_VdoTxtWndTopMargin + p_XTmp, p_VdoInptOtptWndWdh, p_PerVdoInptOtptWndHgt, SWP_NOZORDER );
					p_XTmp += p_PerVdoInptOtptWndHgt;
				}
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_HIDE );
			}
			else //如果没有视频输入输出窗口。
			{
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_SHOW );
			}
			break;
		}
		case 1: //垂直最大化排列。
		{
			if( g_VdoWndShowMode != 1 ) //如果当前显示模式不是垂直最大化排列。
			{
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_HIDE ); //设置服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_HIDE ); //设置服务端创建或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_HIDE ); //设置服务端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_HIDE ); //设置连接销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_HIDE ); //设置连接列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_HIDE ); //设置客户端的服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_HIDE ); //设置编辑框添加按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_HIDE ); //设置客户端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), SW_HIDE ); //设置客户端连接或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_HIDE ); //设置客户端删除按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_HIDE ); //设置客户端列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ), SW_HIDE ); //设置音频输入对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ), SW_HIDE ); //设置音频输出对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ), SW_HIDE ); //设置视频输入对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ), SW_HIDE ); //设置视频输出对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_HIDE ); //设置音频输入设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_HIDE ); //设置音频输出设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_HIDE ); //设置视频输入设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, PtbBtnId ), SW_HIDE ); //设置一键即按即广播按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), SW_HIDE ); //设置保存设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), SW_HIDE ); //设置读取设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), SW_HIDE ); //设置删除设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), SW_HIDE ); //设置重置设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ), SW_HIDE ); //设置是否绘制音频波形到窗口复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_HIDE ); //设置音频输入原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_HIDE ); //设置音频输出原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_HIDE ); //设置音频输入结果波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLstBoxId ), SW_HIDE ); //设置日志列表框为隐藏。

				g_VdoWndShowMode = 1;
			}

			p_VdoInptOtptWndWdh = RectWidth( p_MainDlgWndRect );
			p_VdoInptOtptWndHgt = RectHeight( p_MainDlgWndRect );
			SetWindowPos( g_VdoInptOtptTxtWndHdl, HWND_BOTTOM, 0, 0, p_VdoInptOtptWndWdh, p_VdoInptOtptWndHgt, 0 ); //设置视频输入输出文本框窗口的位置。
			g_VdoInptOtptWndCntnr.GetTotal( &p_VdoInptOtptWndTotal, 0, NULL );
			if( p_VdoInptOtptWndTotal > 0 ) //如果有视频输入输出窗口。
			{
				p_PerVdoInptOtptWndHgt = p_VdoInptOtptWndHgt / p_VdoInptOtptWndTotal - p_PerTitleTxtWndHgt;
				p_XTmp = 0;
				for( size_t p_VdoInptOtptWndIdx = 0; g_VdoInptOtptWndCntnr.GetByNum( p_VdoInptOtptWndIdx, NULL, ( void * * )&p_VdoInptOtptWndTmpPt, 0, 0, NULL ) == 0; p_VdoInptOtptWndIdx++ )
				{
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_TitleTxtWndHdl, NULL, 0, p_XTmp, p_VdoInptOtptWndWdh, p_PerTitleTxtWndHgt, SWP_NOZORDER );
					p_XTmp += p_PerTitleTxtWndHgt;
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl, NULL, 0, p_XTmp, p_VdoInptOtptWndWdh, p_PerVdoInptOtptWndHgt, SWP_NOZORDER );
					p_XTmp += p_PerVdoInptOtptWndHgt;
				}
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_HIDE );
			}
			else //如果没有视频输入输出窗口。
			{
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_SHOW );
			}
			break;
		}
		case 2: //水平最大化排列。
		{
			if( g_VdoWndShowMode != 2 ) //如果当前显示模式不是水平最大化排列。
			{
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_HIDE ); //设置服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_HIDE ); //设置服务端创建或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_HIDE ); //设置服务端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_HIDE ); //设置连接销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_HIDE ); //设置连接列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_HIDE ); //设置客户端的服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_HIDE ); //设置编辑框添加按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_HIDE ); //设置客户端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), SW_HIDE ); //设置客户端连接或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_HIDE ); //设置客户端删除按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_HIDE ); //设置客户端列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ), SW_HIDE ); //设置音频输入对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ), SW_HIDE ); //设置音频输出对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ), SW_HIDE ); //设置视频输入对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ), SW_HIDE ); //设置视频输出对讲模式复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ), SW_HIDE ); //设置音频输入静音复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ), SW_HIDE ); //设置音频输出静音复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ), SW_HIDE ); //设置视频输入黑屏复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ), SW_HIDE ); //设置视频输出黑屏复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), SW_HIDE ); //设置音频输入设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), SW_HIDE ); //设置音频输出设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), SW_HIDE ); //设置视频输入设备组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, PtbBtnId ), SW_HIDE ); //设置一键即按即广播按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), SW_HIDE ); //设置设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), SW_HIDE ); //设置保存设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), SW_HIDE ); //设置读取设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), SW_HIDE ); //设置删除设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), SW_HIDE ); //设置重置设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ReadMeBtnId ), SW_HIDE ); //设置必读说明按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ), SW_HIDE ); //设置是否绘制音频波形到窗口复选框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ), SW_HIDE ); //设置音频输入原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ), SW_HIDE ); //设置音频输出原始波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ), SW_HIDE ); //设置音频输入结果波形文本框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClearLogBtnId ), SW_HIDE ); //设置清空日志按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, LogLstBoxId ), SW_HIDE ); //设置日志列表框为隐藏。
				
				g_VdoWndShowMode = 2;
			}

			p_VdoInptOtptWndWdh = RectWidth( p_MainDlgWndRect );
			p_VdoInptOtptWndHgt = RectHeight( p_MainDlgWndRect );
			SetWindowPos( g_VdoInptOtptTxtWndHdl, HWND_BOTTOM, 0, 0, p_VdoInptOtptWndWdh, p_VdoInptOtptWndHgt, 0 ); //设置视频输入输出文本框窗口的位置。
			g_VdoInptOtptWndCntnr.GetTotal( &p_VdoInptOtptWndTotal, 0, NULL );
			if( p_VdoInptOtptWndTotal > 0 ) //如果有视频输入输出窗口。
			{
				p_PerVdoInptOtptWndWdh = p_VdoInptOtptWndWdh / p_VdoInptOtptWndTotal;
				p_PerVdoInptOtptWndHgt = p_VdoInptOtptWndHgt - p_PerTitleTxtWndHgt;
				p_XTmp = 0;
				for( size_t p_VdoInptOtptWndIdx = 0; g_VdoInptOtptWndCntnr.GetByNum( p_VdoInptOtptWndIdx, NULL, ( void * * )&p_VdoInptOtptWndTmpPt, 0, 0, NULL ) == 0; p_VdoInptOtptWndIdx++ )
				{
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_TitleTxtWndHdl, NULL, p_XTmp, 0, p_PerVdoInptOtptWndWdh, p_PerTitleTxtWndHgt, SWP_NOZORDER );
					SetWindowPos( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl, NULL, p_XTmp, p_PerTitleTxtWndHgt, p_PerVdoInptOtptWndWdh, p_PerVdoInptOtptWndHgt, SWP_NOZORDER );
					p_XTmp += p_PerVdoInptOtptWndWdh;
				}
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_HIDE );
			}
			else //如果没有视频输入输出窗口。
			{
				ShowWindow( g_VdoInptOtptTxtWndHdl, SW_SHOW );
			}
			break;
		}
	}
	InvalidateRect( g_MainDlgWndHdl, NULL, TRUE ); //重绘主对话框窗口。
}

//发送视频输入输出窗口初始化消息。
HWND SendVdoInptOtptWndInitMsg( Vstr * TitleVstrPt )
{
	MainDlgWndMsgVdoInptOtptWndInit * p_MainDlgWndMsgVdoInptOtptWndInitPt = ( MainDlgWndMsgVdoInptOtptWndInit * )malloc( sizeof( MainDlgWndMsgVdoInptOtptWndInit ) );
	VstrInit( &p_MainDlgWndMsgVdoInptOtptWndInitPt->m_TitleVstrPt, Utf16, , TitleVstrPt );
	SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypVdoInptOtptWndInit, ( WPARAM )p_MainDlgWndMsgVdoInptOtptWndInitPt, 0 );
	HWND p_DspyTxtWndHdl = p_MainDlgWndMsgVdoInptOtptWndInitPt->m_DspyTxtWndHdl;
	VstrDstoy( p_MainDlgWndMsgVdoInptOtptWndInitPt->m_TitleVstrPt );
	free( p_MainDlgWndMsgVdoInptOtptWndInitPt );
	return p_DspyTxtWndHdl;
}

//发送视频输入输出窗口销毁消息。
void SendVdoInptOtptWndDstoyMsg( HWND DspyTxtWndHdl )
{
	MainDlgWndMsgVdoInptOtptWndDstoy * p_MainDlgWndMsgVdoInptOtptWndDstoyPt = ( MainDlgWndMsgVdoInptOtptWndDstoy * )malloc( sizeof( MainDlgWndMsgVdoInptOtptWndDstoy ) );
	p_MainDlgWndMsgVdoInptOtptWndDstoyPt->m_DspyTxtWndHdl = DspyTxtWndHdl;
	PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypVdoInptOtptWndDstoy, ( WPARAM )p_MainDlgWndMsgVdoInptOtptWndDstoyPt, 0 );
}

//发送视频输入输出窗口设置标题消息。
void SendVdoInptOtptWndSetTitleMsg( HWND DspyTxtWndHdl, Vstr * TitleVstrPt )
{
	MainDlgWndMsgVdoInptOtptWndSetTitle * p_MainDlgWndMsgVdoInptOtptWndSetTitlePt = ( MainDlgWndMsgVdoInptOtptWndSetTitle * )malloc( sizeof( MainDlgWndMsgVdoInptOtptWndSetTitle ) );
	p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_DspyTxtWndHdl = DspyTxtWndHdl;
	VstrInit( &p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_TitleVstrPt, Utf16, , TitleVstrPt );
	PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypVdoInptOtptWndSetTitle, ( WPARAM )p_MainDlgWndMsgVdoInptOtptWndSetTitlePt, 0 );
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
			RECT p_LogLstBoxWndRect;

			GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect ); //获取主对话框窗口的位置。

			//移动日志列表框窗口。
			GetWindowRect( g_LogLstBoxWndHdl, &p_LogLstBoxWndRect );
			RectScrnToClnt( g_MainDlgWndHdl, p_LogLstBoxWndRect );
			p_LogLstBoxWndRect.bottom = p_MainDlgWndRect.bottom - g_VdoTxtWndBottomMargin;
			SetWindowPos( g_LogLstBoxWndHdl, NULL, p_LogLstBoxWndRect.left, p_LogLstBoxWndRect.top, RectWidth( p_LogLstBoxWndRect ), RectHeight( p_LogLstBoxWndRect ), SWP_NOMOVE | SWP_NOZORDER );
			//InvalidateRect( g_LogLstBoxWndHdl, NULL, TRUE ); //重绘日志列表框窗口。
			
			SetVdoWndShowMode( g_VdoWndShowMode ); //移动视频输入输出窗口。

			return ( INT_PTR )TRUE;
		}
		case WM_DEVICECHANGE: //系统设备改变消息。
		{
			if( wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE ) //如果是有设备添加或删除。
			{
				PDEV_BROADCAST_HDR pHdr = ( PDEV_BROADCAST_HDR )lParam;
				if( ( pHdr != NULL ) && ( pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE ) )
				{
					PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = ( PDEV_BROADCAST_DEVICEINTERFACE )pHdr;

					if( IsEqualGUID( pDevInterface->dbcc_classguid, KSCATEGORY_CAPTURE ) ) //如果是视频输入设备。
					{
						LOGI( Cu8vstr( "接收到视频输入设备改变消息。" ) );
						PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypVdoInptDvcChg, 0, 0 );
					}
				}
			}
			return ( INT_PTR )TRUE;
		}
		case WM_COMMAND: //命令消息。
		{
			switch( LOWORD( wParam ) )
			{
				case SrvrCreateOrDstoyBtnId: //服务端初始化或销毁按钮。
				{
					if( g_MySrvrThrdPt == NULL ) //如果我的服务端线程未初始化。
					{
						g_MySrvrThrdPt = new MySrvrThrdCls( g_MainDlgWndHdl );
						if( g_MySrvrThrdPt->Init( g_LicnCodePt ) != 0 )
						{
							delete g_MySrvrThrdPt;
							g_MySrvrThrdPt = NULL;
						}
					}
					else //如果对讲客户端端连接已初始化。
					{
						g_MySrvrThrdPt->Dstoy();
					}
					return ( INT_PTR )TRUE;
				}
				case SrvrStngBtnId: //服务端设置按钮。
				{
					ShowWindow( g_SrvrStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示服务端设置对话框。
					return ( INT_PTR )TRUE;
				}
				case CnctDstoyBtnId: //连接销毁按钮。
				{
					int p_Num;
					
					if( g_MySrvrThrdPt != NULL )
					{
						p_Num = ListView_GetSelectionMark( g_CnctLstWndHdl );
						if( p_Num != -1 )
						{
							g_MySrvrThrdPt->SendCnctDstoyMsg( 1, p_Num, NULL );
						}
					}
					return ( INT_PTR )TRUE;
				}
				case ClntStngBtnId: //客户端设置按钮。
				{
					ShowWindow( g_ClntStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示客户端设置对话框。
					return ( INT_PTR )TRUE;
				}
				case ClntAddBtnId: //客户端添加按钮。
				{
					HWND p_ClntSrvrUrlHdl;
					int p_ClntSrvrUrlLenChr;
					Vstr * p_ClntSrvrUrlVstrPt = NULL;
					Vstr * p_PrtclVstrPt = NULL;
					Vstr * p_HostnameVstrPt = NULL;
					Vstr * p_PortVstrPt = NULL;
					int p_CmpRslt;

					p_ClntSrvrUrlHdl = GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId );
					p_ClntSrvrUrlLenChr = GetWindowTextLength( p_ClntSrvrUrlHdl );
					VstrInit( &p_ClntSrvrUrlVstrPt, Utf16, p_ClntSrvrUrlLenChr + 1, );
					GetDlgItemText( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId, ( LPWSTR )p_ClntSrvrUrlVstrPt->m_Pt, p_ClntSrvrUrlLenChr + 1 );
					p_ClntSrvrUrlVstrPt->m_LenChr = p_ClntSrvrUrlLenChr;

					VstrInit( &p_PrtclVstrPt, Utf16, ,  );
					VstrInit( &p_HostnameVstrPt, Utf16, , );
					VstrInit( &p_PortVstrPt, Utf16, , );

					if( VstrUrlParse( p_ClntSrvrUrlVstrPt, p_PrtclVstrPt, NULL, NULL, p_HostnameVstrPt, p_PortVstrPt, NULL, NULL, NULL, NULL, g_ErrInfoVstr.m_VstrPt ) != 0 )
					{
						g_ErrInfoVstr.Ins( 0, Cu8vstr( "解析客户端的服务端Url字符串失败。原因：" ) );
						LOGE( g_ErrInfoVstr.m_VstrPt );
						Toast( g_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_VstrPt );
						goto ClntAddOut;
					}
					if( ( VstrCmp( p_PrtclVstrPt, , , Cu8vstr( "Tcp" ), , &p_CmpRslt ),p_CmpRslt != 0 ) && ( VstrCmp( p_PrtclVstrPt, , , Cu8vstr( "Audp" ), , &p_CmpRslt ),p_CmpRslt != 0 ) )
					{
						g_ErrInfoVstr.Cpy( Cu8vstr( "客户端的服务端Url字符串的协议不正确。" ) );
						LOGE( g_ErrInfoVstr.m_VstrPt );
						Toast( g_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_VstrPt );
						goto ClntAddOut;
					}
					if( VstrCmp( p_PortVstrPt, , , Cu8vstr( "" ), , &p_CmpRslt ), p_CmpRslt == 0 )
					{
						VstrCpy( p_PortVstrPt, Cu8vstr( "12345" ), ,  );
					}

					MainDlgWndMsgClntLstAddItem * p_MainDlgWndMsgClntLstAddItemPt = ( MainDlgWndMsgClntLstAddItem * )malloc( sizeof( MainDlgWndMsgClntLstAddItem ) );
					p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt = p_PrtclVstrPt;
					p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt = p_HostnameVstrPt;
					p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt = p_PortVstrPt;
					SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypClntLstAddItem, ( WPARAM )p_MainDlgWndMsgClntLstAddItemPt, 0 );

					ClntAddOut:
					VstrDstoy( p_ClntSrvrUrlVstrPt );
					return ( INT_PTR )TRUE;
				}
				case ClntCnctOrDstoyBtnId: //客户端连接或销毁按钮。
				{
					if( g_MyClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未初始化。
					{
						g_MyClntMediaPocsThrdPt = new MyClntMediaPocsThrdCls( g_MainDlgWndHdl );
						if( g_MyClntMediaPocsThrdPt->Init( g_LicnCodePt ) != 0 )
						{
							delete g_MyClntMediaPocsThrdPt;
							g_MyClntMediaPocsThrdPt = NULL;
						}
					}

					if( g_MyClntMediaPocsThrdPt != NULL ) //如果我的客户端媒体处理线程已初始化。
					{
						if( g_MyClntMediaPocsThrdPt->m_ClntMediaPocsThrdPt->m_TkbkClnt.m_CnctIsInit == 0 ) //如果对讲客户端连接未初始化。
						{
							g_MyClntMediaPocsThrdPt->TkbkInit( g_LicnCodePt );
						}
						else //如果对讲客户端端连接已初始化。
						{
							g_MyClntMediaPocsThrdPt->TkbkDstoy();
						}
					}
					return ( INT_PTR )TRUE;
				}
				case ClntDelBtnId: //客户端删除按钮。
				{
					SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypClntLstDelItem, ListView_GetSelectionMark( g_ClntLstWndHdl ), 0 );
					return ( INT_PTR )TRUE;
				}
				case UseAdoInptTkbkModeCkBoxId: //使用音频输入对讲模式复选框。
				case UseAdoOtptTkbkModeCkBoxId: //使用音频输出对讲模式复选框。
				case UseVdoInptTkbkModeCkBoxId: //使用视频输入对讲模式复选框。
				case UseVdoOtptTkbkModeCkBoxId: //使用视频输出对讲模式复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						g_MyClntMediaPocsThrdPt->SendTkbkClntLclTkbkModeMsg( 0,
																			 ( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeAdoInpt : 0 ) +
																			 ( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeAdoOtpt : 0 ) +
																			 ( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeVdoInpt : 0 ) +
																			 ( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? ClntMediaPocsThrd::TkbkModeVdoOtpt : 0 ),
																			 NULL );
					}
					return ( INT_PTR )TRUE;
				}
				case UseAdoInptDvcCbBoxId: //使用音频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MyClntMediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								wchar_t * p_CurAdoInptDvcNameU16strPt = NULL;
								CbBoxGetCurSelTxt( g_AdoInptDvcCbBoxWndHdl, p_CurAdoInptDvcNameU16strPt ); //获取当前选择的音频输入设备的名称。
								g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseDvc( 0,
																								 Cu16vstr( p_CurAdoInptDvcNameU16strPt ),
																								 &g_ErrInfoVstr );
							}
							break;
						}
					}
					return ( INT_PTR )TRUE;
				}
				case UseAdoOtptDvcCbBoxId: //使用音频输出设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MyClntMediaPocsThrdPt != NULL )
							{
								//设置音频输出使用的设备。
								wchar_t * p_CurAdoOtptDvcNameU16strPt = NULL;
								CbBoxGetCurSelTxt( g_AdoOtptDvcCbBoxWndHdl, p_CurAdoOtptDvcNameU16strPt ); //获取当前选择的音频输出设备的名称。
								g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetUseDvc( 0,
																								 Cu16vstr( p_CurAdoOtptDvcNameU16strPt ),
																								 &g_ErrInfoVstr );
							}
							break;
						}
					}
					return ( INT_PTR )TRUE;
				}
				case UseVdoInptDvcCbBoxId: //使用视频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MyClntMediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								wchar_t * p_CurVdoInptDvcNameU16strPt = NULL;
								CbBoxGetCurSelTxt( g_VdoInptDvcCbBoxWndHdl, p_CurVdoInptDvcNameU16strPt ); //获取当前选择的音频输入设备的名称。
								g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetUseDvc( 0,
																								 Cu16vstr( p_CurVdoInptDvcNameU16strPt ),
																								 &g_ErrInfoVstr );
							}
							break;
						}
					}
					return ( INT_PTR )TRUE;
				}
				case AdoInptIsMuteCkBoxId: //音频输入静音复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsMute( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					return ( INT_PTR )TRUE;
				}
				case AdoOtptIsMuteCkBoxId: //音频输出静音复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsMute( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					return ( INT_PTR )TRUE;
				}
				case VdoInptIsBlackCkBoxId: //视频输入黑屏复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetIsBlack( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					return ( INT_PTR )TRUE;
				}
				case VdoOtptIsBlackCkBoxId: //视频输出黑屏复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmIsBlack( 1, 0, ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					return ( INT_PTR )TRUE;
				}
				case IsDrawAdoWavfmToWndCkBoxId: //绘制音频波形到窗口复选框。
				{
					if( g_MyClntMediaPocsThrdPt != NULL )
					{
						//设置音频输入是否绘制音频波形到窗口。
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsDrawAdoWavfmToWnd( 1,
																									  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																									  GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ),
																									  GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ),
																									  &g_ErrInfoVstr );

						//设置音频输出是否绘制音频波形到窗口。
						g_MyClntMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsDrawAdoWavfmToWnd( 1,
																									  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																									  GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ),
																									  &g_ErrInfoVstr );
					}
					return ( INT_PTR )TRUE;
				}
				case StngBtnId: //设置按钮。
				{
					ShowWindow( g_StngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SaveStngBtnId: //保存设置按钮。
				{
					SaveStngToXmlFile();
					return ( INT_PTR )TRUE;
				}
				case ReadStngBtnId: //读取设置按钮。
				{
					ReadStngFromXmlFile();
					return ( INT_PTR )TRUE;
				}
				case DelStngBtnId: //删除设置按钮。
				{
					DelStngXmlFile();
					return ( INT_PTR )TRUE;
				}
				case ResetStngBtnId: //重置设置按钮。
				{
					ResetStng();
					return ( INT_PTR )TRUE;
				}
				case UseEffectLowRdBtnId: //效果等级：低。
				case UseEffectMidRdBtnId: //效果等级：中。
				case UseEffectHighRdBtnId: //效果等级：高。
				case UseEffectSuperRdBtnId: //效果等级：超。
				case UseEffectPremiumRdBtnId: //效果等级：特。
				case UseAecTendEchoCnclRdBtnId: //声学回音消除倾向：消除彻底，双讲吃音。
				case UseAecTendDblTalkRdBtnId: //声学回音消除倾向：双讲流畅，残余回音。
				{
					SetEffectAecTendStng();
					return ( INT_PTR )TRUE;
				}
				case IsUseDebugInfoCkBoxId: //使用调试信息复选框。
				{
					SetDebugInfoStng();
					return ( INT_PTR )TRUE;
				}
				case AjbStngBtnId: //自适应抖动缓冲器设置按钮。
				{
					ShowWindow( g_AjbStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SaveStsToTxtFileStngBtnId: //保存状态到Txt文件设置按钮。
				{
					ShowWindow( g_SaveStsToTxtFileStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示保存状态到Txt文件设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SaveAdoVdoInptOtptToAviFileStngBtnId: //保存音视频输入输出到Avi文件设置按钮。
				{
					ShowWindow( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示自适应抖动缓冲器设置对话框。
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
				case WebRtcAec3StngBtnId: //WebRtc第三版声学回音消除器设置按钮。
				{
					ShowWindow( g_WebRtcAec3StngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示WebRtc第三版声学回音消除器设置对话框。
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
				case SaveAdoInptOtptToWaveFileStngBtnId: //保存音频输入输出到Wave文件设置按钮。
				{
					ShowWindow( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示保存音频输入输出到Wave文件设置对话框。
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
					SendMessage( GetDlgItem( hDlg, LogLstBoxId ), LB_RESETCONTENT, 0, 0 ); //清空日志列表。
					SendDlgItemMessage( hDlg, LogLstBoxId, LB_SETHORIZONTALEXTENT, 0, 0 ); //清空滚动条的当前像素大小。
					
					return ( INT_PTR )TRUE;
				}
				case VdoInptOtptTxtId: //视频输入输出文本框。
				{
					SetVdoWndShowMode( g_VdoWndShowMode + 1 ); //设置视频窗口的显示模式。
					return ( INT_PTR )TRUE;
				}
				case SrvrStngOkBtnId: //服务端设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SrvrStngDlgWndHdl ), TRUE ), ShowWindow( g_SrvrStngDlgWndHdl, SW_HIDE ); //隐藏服务端设置对话框。
					return ( INT_PTR )TRUE;
				}
				case ClntStngOkBtnId: //客户端设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_ClntStngDlgWndHdl ), TRUE ), ShowWindow( g_ClntStngDlgWndHdl, SW_HIDE ); //隐藏客户端设置对话框。
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
				case SaveStsToTxtFileStngOkBtnId: //保存状态到Txt文件设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SaveStsToTxtFileStngDlgWndHdl ), TRUE ), ShowWindow( g_SaveStsToTxtFileStngDlgWndHdl, SW_HIDE ); //隐藏保存状态到Txt文件设置对话框。
					return ( INT_PTR )TRUE;
				}
				case SaveAdoVdoInptOtptToAviFileStngOkBtnId: //保存音视频输入输出到Avi文件设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl ), TRUE ), ShowWindow( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SW_HIDE ); //隐藏保存音视频输入输出到Avi文件设置对话框。
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
				case WebRtcAecStngOkBtnId: //WebRtc浮点版声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcAecStngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcAecStngDlgWndHdl, SW_HIDE ); //隐藏WebRtc浮点版声学回音消除器设置对话框。
					return ( INT_PTR )TRUE;
				}
				case WebRtcAec3StngOkBtnId: //WebRtc第三版声学回音消除器设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_WebRtcAec3StngDlgWndHdl ), TRUE ), ShowWindow( g_WebRtcAec3StngDlgWndHdl, SW_HIDE ); //隐藏WebRtc第三版声学回音消除器设置对话框。
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
				case SaveAdoInptOtptToWaveFileStngOkBtnId: //保存音频输入输出到Wave文件设置对话框的确定按钮。
				{
					EnableWindow( GetParent( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl ), TRUE ), ShowWindow( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SW_HIDE ); //隐藏保存音频输入输出到Wave文件设置对话框。
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
						if( g_MySrvrThrdPt != NULL ) g_MySrvrThrdPt->Dstoy();
						if( g_MyClntMediaPocsThrdPt != NULL ) g_MyClntMediaPocsThrdPt->Dstoy();
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
			break;
		}
		case WM_DESTROY: //窗口销毁消息。
		{
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypShowLog: //显示日志消息。
		{
			if( ( VstrCls * )wParam != NULL )
			{
				Vstr * InfoVstrPt = ( Vstr * )wParam;

				//显示日志。
				{
					int p_Hour, p_Min, p_Sec, p_Msec;
					FuncGetTimeAsLclDateTime( NULL, NULL, NULL, &p_Hour, &p_Min, &p_Sec, &p_Msec );
					VstrFmtIns( InfoVstrPt, 0, Cu8vstr( "%02.2d:%02.2d:%02.2d %03.3d：" ), p_Hour, p_Min, p_Sec, p_Msec );
					SendMessage( g_LogLstBoxWndHdl, LB_ADDSTRING, 0, ( WPARAM )InfoVstrPt->m_Pt );
					SendMessage( g_LogLstBoxWndHdl, WM_VSCROLL, SB_LINEDOWN, NULL ); //日志列表框窗口的垂直滚动条下移一行。
				}

				//设置水平滚动条的当前像素大小。
				{
					HDC p_DcHdl;
					SIZE p_NewSz;
					long p_CurSz;

					p_CurSz = ( long )SendDlgItemMessage( hDlg, LogLstBoxId, LB_GETHORIZONTALEXTENT, 0, 0 ); //获取滚动条的当前像素大小。

					p_DcHdl = GetDC( hDlg ); //获取DC句柄。
					GetTextExtentPoint( p_DcHdl, ( wchar_t *)InfoVstrPt->m_Pt, InfoVstrPt->m_LenChr, &p_NewSz ); //获取字符串的像素大小。
					if( p_NewSz.cx > p_CurSz ) SendDlgItemMessage( hDlg, LogLstBoxId, LB_SETHORIZONTALEXTENT, ( WPARAM )p_NewSz.cx, 0 ); //设置滚动条的当前像素大小。
					ReleaseDC( hDlg, p_DcHdl ); //释放DC句柄。
				}

				VstrDstoy( InfoVstrPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypShowToast: //显示Toast消息。
		{
			if( ( VstrCls * )wParam != NULL )
			{
				Vstr * InfoVstrPt = ( Vstr * )wParam;

				Toast( g_MainDlgWndHdl, 3000, NULL, InfoVstrPt ); //显示Toast。

				VstrDstoy( InfoVstrPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypMySrvrThrdInit: //我的服务端线程初始化消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), FALSE ); //设置服务端设置按钮为不可用。
			
			if( g_MyClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未初始化。
			{
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), FALSE ); //设置设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), FALSE ); //设置保存设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), FALSE ); //设置读取设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), FALSE ); //设置删除设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), FALSE ); //设置重置设置按钮为不可用。
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypMySrvrThrdDstoy: //我的服务端线程销毁消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), TRUE ); //设置服务端设置按钮为可用。
			
			if( g_MyClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未初始化。
			{
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), TRUE ); //设置设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), TRUE ); //设置保存设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), TRUE ); //设置读取设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), TRUE ); //设置删除设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), TRUE ); //设置重置设置按钮为可用。
			}

			if( g_MySrvrThrdPt != NULL )
			{
				g_MySrvrThrdPt->Dstoy(); //销毁媒体处理线程。
				delete g_MySrvrThrdPt;
				g_MySrvrThrdPt = NULL;
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypSrvrInit: //服务端初始化消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), FALSE ); //设置服务端Url组合框为不可用。
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), L"销毁" ); //设置服务端创建或销毁按钮的内容为“销毁”。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), TRUE ); //设置服务端连接销毁按钮的内容为可用。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypSrvrDstoy: //服务端销毁消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), TRUE ); //设置服务端Url组合框为可用。
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), L"创建" ); //设置服务端创建或销毁按钮的内容为“创建”。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), FALSE ); //设置服务端连接销毁按钮的内容为不可用。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypMyClntMediaPocsThrdInit: //我的客户端媒体处理线程初始化消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), FALSE ); //设置客户端添加按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), FALSE ); //设置客户端设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), FALSE ); //设置客户端删除按钮为不可用。
            if( g_MyClntMediaPocsThrdPt->m_ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode == 0 ) ShowWindow( g_PttDlgWndHdl, SW_SHOW ); //设置一键即按即通对话框为显示。

			if( g_MySrvrThrdPt == NULL ) //如果我的服务端线程未初始化。
			{
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), FALSE ); //设置设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), FALSE ); //设置保存设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), FALSE ); //设置读取设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), FALSE ); //设置删除设置按钮为不可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), FALSE ); //设置重置设置按钮为不可用。
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypMyClntMediaPocsThrdDstoy: //我的客户端媒体处理线程销毁消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), TRUE ); //设置客户端添加按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), TRUE ); //设置客户端设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), TRUE ); //设置客户端删除按钮为可用。
			if( g_MyClntMediaPocsThrdPt->m_ClntMediaPocsThrdPt->m_TkbkClnt.m_XfrMode == 0 ) ShowWindow( g_PttDlgWndHdl, SW_HIDE ); //设置一键即按即通对话框为隐藏。

			if( g_MySrvrThrdPt == NULL ) //如果我的服务端线程未初始化。
			{
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), TRUE ); //设置设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), TRUE ); //设置保存设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), TRUE ); //设置读取设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), TRUE ); //设置删除设置按钮为可用。
				EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), TRUE ); //设置重置设置按钮为可用。
			}

			if( g_MyClntMediaPocsThrdPt != NULL )
			{
				g_MyClntMediaPocsThrdPt->Dstoy(); //销毁媒体处理线程。
				delete g_MyClntMediaPocsThrdPt;
				g_MyClntMediaPocsThrdPt = NULL;
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypTkbkClntCnctInit: //对讲客户端连接初始化消息。
		{
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), L"销毁" ); //设置客户端连接或销毁按钮的内容为“销毁”。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypTkbkClntCnctDstoy: //对讲客户端连接销毁消息。
		{
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), L"连接" ); //设置客户端连接或销毁按钮的内容为“连接”。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypCnctLstAddItem: //连接列表添加项目消息。
		{
			MainDlgWndMsgCnctLstAddItem * p_MainDlgWndMsgCnctLstAddItemPt = ( MainDlgWndMsgCnctLstAddItem * )wParam;
			LVITEM p_LvItem;

			if( p_MainDlgWndMsgCnctLstAddItemPt != NULL )
			{
				p_LvItem.mask = 0;
				p_LvItem.iItem = ListView_GetItemCount( g_CnctLstWndHdl );
				p_LvItem.iSubItem = 0;
				ListView_InsertItem( g_CnctLstWndHdl, &p_LvItem ); // 如果成功插入，返回项目句柄

				ListView_SetItemText( g_CnctLstWndHdl, p_LvItem.iItem, 0, ( p_MainDlgWndMsgCnctLstAddItemPt->m_IsTcpOrAudpPrtcl == 0 ) ? L"Tcp" : L"Audp" );
				ListView_SetItemText( g_CnctLstWndHdl, p_LvItem.iItem, 1, ( LPWSTR )p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt );
				ListView_SetItemText( g_CnctLstWndHdl, p_LvItem.iItem, 2, ( LPWSTR )p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt );
				free( p_MainDlgWndMsgCnctLstAddItemPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypCnctLstDelItem: //连接列表删除项目消息。
		{
			ListView_DeleteItem( g_CnctLstWndHdl, ( int )wParam );
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypCnctLstModifyItem: //连接列表修改项目消息。
		{
			MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )wParam;

			if( p_MainDlgWndMsgCnctLstModifyItemPt != NULL )
			{
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 3, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt->m_Pt );
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 4, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt->m_Pt );
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 5, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt1VstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt2VstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_Txt3VstrPt );
				free( p_MainDlgWndMsgCnctLstModifyItemPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypClntLstAddItem: //客户端列表添加项目消息。
		{
			MainDlgWndMsgClntLstAddItem * p_MainDlgWndMsgClntLstAddItemPt = ( MainDlgWndMsgClntLstAddItem * )wParam;
			int p_ClntLstTotal = ListView_GetItemCount( g_ClntLstWndHdl );
			wchar_t p_TmpU16strPt[ 1024 ];
			LVITEM p_LvItem;

			if( p_MainDlgWndMsgClntLstAddItemPt != NULL )
			{
				for( int p_Num = 0; p_Num < p_ClntLstTotal; p_Num++ )
				{
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 0, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
					if( wcscmp( p_TmpU16strPt, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt->m_Pt ) != 0 ) continue;
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
					if( wcscmp( p_TmpU16strPt, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt ) != 0 ) continue;
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
					if( wcscmp( p_TmpU16strPt, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt ) != 0 ) continue;
					Toast( g_MainDlgWndHdl, 3000, NULL, Cu8vstr( "已存在相同的客户端的服务端，无需重复添加。" ) );
					goto MainDlgWndMsgTypClntLstAddItemOut;
				}

				p_LvItem.mask = 0;
				p_LvItem.iItem = p_ClntLstTotal;
				p_LvItem.iSubItem = 0;
				ListView_InsertItem( g_ClntLstWndHdl, &p_LvItem ); // 如果成功插入，返回项目句柄

				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 0, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt->m_Pt );
				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 1, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt );
				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 2, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt );
				VstrDstoy( p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt );
				VstrDstoy( p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt );
				free( p_MainDlgWndMsgClntLstAddItemPt );
			}

			MainDlgWndMsgTypClntLstAddItemOut:
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypClntLstDelItem: //客户端列表删除项目消息。
		{
			ListView_DeleteItem( g_ClntLstWndHdl, ( int )wParam );
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypClntLstModifyItem: //客户端列表修改项目消息。
		{
			MainDlgWndMsgClntLstModifyItem * p_MainDlgWndMsgClntLstModifyItemPt = ( MainDlgWndMsgClntLstModifyItem * )wParam;

			if( p_MainDlgWndMsgClntLstModifyItemPt != NULL )
			{
				if( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt != NULL ) ListView_SetItemText( g_ClntLstWndHdl, p_MainDlgWndMsgClntLstModifyItemPt->m_Num, 3, ( LPWSTR )p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt->m_Pt );
				if( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt != NULL ) ListView_SetItemText( g_ClntLstWndHdl, p_MainDlgWndMsgClntLstModifyItemPt->m_Num, 4, ( LPWSTR )p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt->m_Pt );
				if( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt != NULL ) ListView_SetItemText( g_ClntLstWndHdl, p_MainDlgWndMsgClntLstModifyItemPt->m_Num, 5, ( LPWSTR )p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt1VstrPt );
				VstrDstoy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt2VstrPt );
				VstrDstoy( p_MainDlgWndMsgClntLstModifyItemPt->m_Txt3VstrPt );
				free( p_MainDlgWndMsgClntLstModifyItemPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypVdoInptOtptWndInit: //视频输入输出窗口初始化。
		{
			MainDlgWndMsgVdoInptOtptWndInit * p_MainDlgWndMsgVdoInptOtptWndInitPt = ( MainDlgWndMsgVdoInptOtptWndInit * )wParam;
			VdoInptOtptWnd p_VdoInptOtptWnd = { 0 };
			
			if( p_MainDlgWndMsgVdoInptOtptWndInitPt != NULL )
			{
				//标题文本框窗口初始化。
				p_VdoInptOtptWnd.m_TitleTxtWndHdl = CreateWindowEx( GetWindowExStyle( g_VdoInptOtptTxtWndHdl ),
																	L"STATIC",
																	( wchar_t * )p_MainDlgWndMsgVdoInptOtptWndInitPt->m_TitleVstrPt->m_Pt,
																	GetWindowStyle( g_VdoInptOtptTxtWndHdl ) | WS_VISIBLE,
																	0, 0,
																	0, 0,
																	g_MainDlgWndHdl,
																	( HMENU )VdoInptOtptTxtId,
																	g_IstnsHdl,
																	NULL );
				SendMessage( p_VdoInptOtptWnd.m_TitleTxtWndHdl, WM_SETFONT, ( WPARAM )SendMessage( g_VdoInptOtptTxtWndHdl, WM_GETFONT, 0, 0 ), MAKELPARAM( TRUE, 0 ) );

				//显示文本框窗口初始化。
				p_VdoInptOtptWnd.m_DspyTxtWndHdl = CreateWindowEx( GetWindowExStyle( g_VdoInptOtptTxtWndHdl ),
																   L"STATIC",
																   ( wchar_t * )p_MainDlgWndMsgVdoInptOtptWndInitPt->m_TitleVstrPt->m_Pt,
																   GetWindowStyle( g_VdoInptOtptTxtWndHdl ) | WS_VISIBLE,
																   0, 0,
																   0, 0,
																   g_MainDlgWndHdl,
																   ( HMENU )VdoInptOtptTxtId,
																   g_IstnsHdl,
																   NULL );
				SendMessage( p_VdoInptOtptWnd.m_DspyTxtWndHdl, WM_SETFONT, ( WPARAM )SendMessage( g_VdoInptOtptTxtWndHdl, WM_GETFONT, 0, 0 ), MAKELPARAM( TRUE, 0 ) );

				g_VdoInptOtptWndCntnr.PutTail( &p_VdoInptOtptWnd, NULL, 0, NULL ); //放入视频输入输出窗口容器。
				SetVdoWndShowMode( g_VdoWndShowMode ); //移动视频输入输出窗口。

				p_MainDlgWndMsgVdoInptOtptWndInitPt->m_DspyTxtWndHdl = p_VdoInptOtptWnd.m_DspyTxtWndHdl; //设置显示文本框窗口的句柄。
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypVdoInptOtptWndDstoy: //视频输入输出窗口销毁。
		{
			MainDlgWndMsgVdoInptOtptWndDstoy * p_MainDlgWndMsgVdoInptOtptWndDstoyPt = ( MainDlgWndMsgVdoInptOtptWndDstoy * )wParam;
			VdoInptOtptWnd p_VdoInptOtptWnd = { 0 };
			VdoInptOtptWnd * p_VdoInptOtptWndTmpPt;
			
			if( p_MainDlgWndMsgVdoInptOtptWndDstoyPt != NULL )
			{
				for( size_t p_VdoInptOtptWndIdx = 0; g_VdoInptOtptWndCntnr.GetByNum( p_VdoInptOtptWndIdx, NULL, ( void * * )&p_VdoInptOtptWndTmpPt, 0, 0, NULL ) == 0; p_VdoInptOtptWndIdx++ )
				{
					if( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl == p_MainDlgWndMsgVdoInptOtptWndDstoyPt->m_DspyTxtWndHdl )
					{
						DestroyWindow( p_VdoInptOtptWndTmpPt->m_TitleTxtWndHdl );
						DestroyWindow( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl );
						g_VdoInptOtptWndCntnr.DelByNum( p_VdoInptOtptWndIdx, 0, NULL );
						break;
					}
				}

				SetVdoWndShowMode( g_VdoWndShowMode ); //移动视频输入输出窗口。
				
				free( p_MainDlgWndMsgVdoInptOtptWndDstoyPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypVdoInptOtptWndSetTitle: //视频输入输出窗口设置标题。
		{
			MainDlgWndMsgVdoInptOtptWndSetTitle * p_MainDlgWndMsgVdoInptOtptWndSetTitlePt = ( MainDlgWndMsgVdoInptOtptWndSetTitle * )wParam;
			VdoInptOtptWnd p_VdoInptOtptWnd = { 0 };
			VdoInptOtptWnd * p_VdoInptOtptWndTmpPt;
			
			if( p_MainDlgWndMsgVdoInptOtptWndSetTitlePt != NULL )
			{
				for( size_t p_VdoInptOtptWndIdx = 0; g_VdoInptOtptWndCntnr.GetByNum( p_VdoInptOtptWndIdx, NULL, ( void * * )&p_VdoInptOtptWndTmpPt, 0, 0, NULL ) == 0; p_VdoInptOtptWndIdx++ )
				{
					if( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl == p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_DspyTxtWndHdl )
					{
						SetWindowText( p_VdoInptOtptWndTmpPt->m_TitleTxtWndHdl, ( wchar_t * )p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_TitleVstrPt->m_Pt );
						SetWindowText( p_VdoInptOtptWndTmpPt->m_DspyTxtWndHdl, ( wchar_t * )p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_TitleVstrPt->m_Pt );
						break;
					}
				}
				
				VstrDstoy( p_MainDlgWndMsgVdoInptOtptWndSetTitlePt->m_TitleVstrPt );
				free( p_MainDlgWndMsgVdoInptOtptWndSetTitlePt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypAdoInptDvcChg:
		{
			Vstr * p_AdoInptDvcNameVstrPt = ( Vstr * )wParam;

			RefreshAdoVdoInptOtptDvc( 1, 0, 0 ); //刷新音频输入设备。

			if( p_AdoInptDvcNameVstrPt != NULL )
			{
				SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_AdoInptDvcNameVstrPt->m_Pt ); //选择当前使用的音频输入设备。

				VstrDstoy( p_AdoInptDvcNameVstrPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypAdoOtptDvcChg:
		{
			Vstr * p_AdoOtptDvcNameVstrPt = ( Vstr * )wParam;

			RefreshAdoVdoInptOtptDvc( 0, 1, 0 ); //刷新音频输出设备。

			if( p_AdoOtptDvcNameVstrPt != NULL )
			{
				SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_AdoOtptDvcNameVstrPt->m_Pt ); //选择当前使用的音频输出设备。

				VstrDstoy( p_AdoOtptDvcNameVstrPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypVdoInptDvcChg:
		{
			Vstr * p_VdoInptDvcNameVstrPt = ( Vstr * )wParam;

			RefreshAdoVdoInptOtptDvc( 0, 0, 1 ); //刷新视频输入设备。

			if( p_VdoInptDvcNameVstrPt != NULL )
			{
				SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_VdoInptDvcNameVstrPt->m_Pt ); //选择当前使用的视频输入设备。

				VstrDstoy( p_VdoInptDvcNameVstrPt );
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
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );
	//_CrtSetBreakAlloc( 2829 );
	#endif

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY ); //初始化COM库。

	g_IstnsHdl = hInstance; //设置当前实例的句柄。
	
	FuncSetCurActPath( NULL, NULL ); //设置当前进程活动目录为当前进程可执行文件上级路径。
	
	//初始化错误信息动态字符串。
	g_ErrInfoVstr.Init();
	
	//初始化日志。
	{
		DeleteFile( L".\\log.txt" );
		LogOpenFile( Cu8vstr( ".\\log.txt" ), NULL, 1, g_ErrInfoVstr.m_VstrPt );
	}

	//初始化套接字库。
	{
		WSADATA p_WsaData;
		WSAStartup( MAKEWORD( 2, 2 ), &p_WsaData );
	}

	{
		Vstr * p_LmtAppNameVstrPt = NULL;
		Vstr * p_CurAppNameVstrPt = NULL;
		uint64_t p_LmtTimeSec;
		uint64_t p_RmnTimeSec;

		VstrInit( &p_LmtAppNameVstrPt, , , );
		VstrInit( &p_CurAppNameVstrPt, , , );

		if( AudpGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "本端高级UDP协议套接字限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "本端高级UDP协议套接字剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取本端高级UDP协议套接字的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( SpeexAecGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "Speex声学回音消除器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "Speex声学回音消除器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取Speex声学回音消除器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( WebRtcAecGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "WebRtc浮点版声学回音消除器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "WebRtc浮点版声学回音消除器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取WebRtc浮点版声学回音消除器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( WebRtcAec3GetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "WebRtc第三版声学回音消除器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "WebRtc第三版声学回音消除器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取WebRtc第三版声学回音消除器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( SpeexWebRtcAecGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "SpeexWebRtc三重声学回音消除器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "SpeexWebRtc三重声学回音消除器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取SpeexWebRtc三重声学回音消除器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( RNNoiseGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "RNNoise噪音抑制器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "RNNoise噪音抑制器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取RNNoise噪音抑制器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( OpenH264EncdGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "OpenH264编码器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "OpenH264编码器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取OpenH264编码器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( OpenH264DecdGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "OpenH264解码器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "OpenH264解码器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取OpenH264解码器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( AAjbGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "音频自适应抖动缓冲器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "音频自适应抖动缓冲器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取音频自适应抖动缓冲器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( VAjbGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, g_ErrInfoVstr.m_VstrPt ) == 0 )
		{
			LOGFD( Cu8vstr( "视频自适应抖动缓冲器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "视频自适应抖动缓冲器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取视频自适应抖动缓冲器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}
		
		if( AviFileWriterGetAppLmtInfo( g_LicnCodePt, &p_LmtTimeSec, &p_RmnTimeSec, NULL ) == 0 )
		{
			LOGFD( Cu8vstr( "Avi文件写入器限制时间：%uz64d。" ), p_LmtTimeSec );
			LOGFD( Cu8vstr( "Avi文件写入器剩余时间：%uz64d，约%uz64d天。" ), p_RmnTimeSec, p_RmnTimeSec / 24 / 60 / 60 );
		}
		else
		{
			LOGFD( Cu8vstr( "获取Avi文件写入器的应用程序限制信息失败。原因：%vs" ), g_ErrInfoVstr );
		}

		VstrDstoy( p_LmtAppNameVstrPt );
		VstrDstoy( p_CurAppNameVstrPt );
	}

	//判断应用程序可执行文件的名称是否为“Windows下音视频对讲演示程序.exe”或“WndAdoVdoTkbkDemo*”。
	{
		HANDLE p_CurProcHdl;
		wchar_t p_PocsImageFileName[ MAX_PATH ];
		const wchar_t * p_AppFileNameStrPt;

		//获取Windows应用程序的进程名字符串。
		p_CurProcHdl = GetCurrentProcess();
		GetProcessImageFileName( p_CurProcHdl, p_PocsImageFileName, sizeof( p_PocsImageFileName ) );
		FuncPathFindFileName( Cu16vstr( p_PocsImageFileName ), ( const void * * )&p_AppFileNameStrPt );
		CloseHandle( p_CurProcHdl );
		if( ( memcmp( p_AppFileNameStrPt, L"Windows下音视频对讲演示程序.exe", ( wcslen( L"Windows下音视频对讲演示程序.exe" ) + 1 ) * sizeof( wchar_t ) ) != 0 ) &&
			( memcmp( p_AppFileNameStrPt, L"WndAdoVdoTkbkDemo", wcslen( L"WndAdoVdoTkbkDemo" ) * sizeof( wchar_t ) ) != 0 ) &&
			( memcmp( p_AppFileNameStrPt, L"Test_", wcslen( L"Test_" ) * sizeof( wchar_t ) ) != 0 ) )
		{
			if( MessageBox( NULL, L"当前应用程序可执行文件的名称不为“Windows下音视频对讲演示程序.exe”，运行会报错，是否继续？", L"警告", MB_YESNO | MB_ICONWARNING ) == IDNO )
			{
				return 0;
			}
		}
	}
	
	//创建窗口。
	{
		g_MainDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( MainDlgId ), NULL, WndMsgPocsPocdr ); //设置主对话框窗口的句柄。
		g_SrvrStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SrvrStngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr ); //设置服务端设置对话框窗口的句柄。
		g_CnctLstWndHdl = GetDlgItem( g_MainDlgWndHdl, CnctLstId ); //设置连接列表窗口的句柄。
		g_ClntStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( ClntStngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr ); //设置客户端设置对话框窗口的句柄。
		g_ClntLstWndHdl = GetDlgItem( g_MainDlgWndHdl, ClntLstId ); //设置客户端列表窗口的句柄。
		{
			//连接列表和客户端列表添加字段。
			LVCOLUMN p_LvCol;

			p_LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 38;
			p_LvCol.pszText = L"协议";
			ListView_InsertColumn( g_CnctLstWndHdl, 0, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 0, &p_LvCol );

			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 146;
			p_LvCol.pszText = L"名称";
			ListView_InsertColumn( g_CnctLstWndHdl, 1, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 1, &p_LvCol );

			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 42;
			p_LvCol.pszText = L"服务";
			ListView_InsertColumn( g_CnctLstWndHdl, 2, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 2, &p_LvCol );

			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 99;
			p_LvCol.pszText = L"连接状态";
			ListView_InsertColumn( g_CnctLstWndHdl, 3, &p_LvCol );
			p_LvCol.pszText = L"对讲连接状态";
			ListView_InsertColumn( g_ClntLstWndHdl, 3, &p_LvCol );

			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 99;
			p_LvCol.pszText = L"远端对讲模式";
			ListView_InsertColumn( g_CnctLstWndHdl, 4, &p_LvCol );
			p_LvCol.pszText = L"广播连接状态";
			ListView_InsertColumn( g_ClntLstWndHdl, 4, &p_LvCol );
			
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.cx = 43;
			p_LvCol.pszText = L"延迟";
			ListView_InsertColumn( g_CnctLstWndHdl, 5, &p_LvCol );
			p_LvCol.pszText = L"延迟";
			ListView_InsertColumn( g_ClntLstWndHdl, 5, &p_LvCol );
			
			//连接列表设置样式。
			ListView_SetExtendedListViewStyleEx( g_CnctLstWndHdl, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
			ListView_SetExtendedListViewStyleEx( g_ClntLstWndHdl, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
		}
		g_PtbBtnWndHdl = GetDlgItem( g_MainDlgWndHdl, PtbBtnId ); //设置一键即按即广播按钮窗口的句柄。
		g_AdoInptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ); //设置音频输入设备组合框窗口的句柄。
		g_AdoOtptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ); //设置音频输出设备组合框窗口的句柄。
		g_VdoInptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ); //设置视频输入设备组合框窗口的句柄。
		g_LogLstBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, LogLstBoxId ); //设置日志列表框窗口的句柄。
		g_VdoInptOtptTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoInptOtptTxtId ); //设置视频输入输出文本框窗口的句柄。
		g_VdoInptOtptWndCntnr.Init( sizeof( VdoInptOtptWnd ), 1, BufAutoAdjMethFreeNumber, 1, SIZE_MAX, NULL ); //视频输入输出窗口容器初始化。
		g_PttDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( PttDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_PttBtnWndHdl = GetDlgItem( g_PttDlgWndHdl, PttBtnId );

		g_StngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( StngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_AjbStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( AjbStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SaveStsToTxtFileStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SaveStsToTxtFileStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SaveAdoVdoInptOtptToAviFileStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecmStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecmStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAec3StngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAec3StngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexWebRtcAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexWebRtcAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexPrpocsNsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexPrpocsNsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcNsxStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcNsxStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcNsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcNsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexPrpocsStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexPrpocsStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexCodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexCodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SaveAdoInptOtptToWaveFileStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SaveAdoInptOtptToWaveFileStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_OpenH264CodecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( OpenH264CodecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
	}
	
	//设置窗口图标。
	#if( ( defined __X86__ ) )
	SetClassLong( g_MainDlgWndHdl, GCL_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WndAdoVdoTkbkDemoIconId ) ) );
	#elif( ( defined __X64__ ) )
	SetClassLong( g_MainDlgWndHdl, GCLP_HICONSM, ( LONG )LoadIcon( g_IstnsHdl, MAKEINTRESOURCE( WndAdoVdoTkbkDemoIconId ) ) );
	#endif
	
	//设置句柄、高度、宽度、边距。
	{
		RECT p_MainDlgWndRect;
		RECT p_VdoInptPrvwTxtWndRect;
		
		//GetWindowRect( g_MainDlgWndHdl, &p_MainDlgWndRect );
		//g_MainDlgWndMinWidth = RectWidth( p_MainDlgWndRect ); //设置主对话框窗口的最小宽度，单位为像素。
		//g_MainDlgWndMinHeight = RectHeight( p_MainDlgWndRect ); //设置主对话框窗口的最小高度，单位为像素。
		g_MainDlgWndMinWidth = 700; //设置主对话框窗口的最小宽度，单位为像素。
		g_MainDlgWndMinHeight = 700; //设置主对话框窗口的最小高度，单位为像素。
		
		GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect );
		GetWindowRect( g_VdoInptOtptTxtWndHdl, &p_VdoInptPrvwTxtWndRect );
		RectScrnToClnt( g_MainDlgWndHdl, p_VdoInptPrvwTxtWndRect );
		g_VdoTxtWndLeftMargin = p_VdoInptPrvwTxtWndRect.left; //设置视频文本框窗口的左边距，单位为像素。
		g_VdoTxtWndTopMargin = p_VdoInptPrvwTxtWndRect.top; //设置视频文本框窗口的顶边距，单位为像素。
		g_VdoTxtWndRightMargin = p_MainDlgWndRect.right - p_VdoInptPrvwTxtWndRect.right; //设置视频文本框窗口的右边距，单位为像素。
		g_VdoTxtWndBottomMargin = p_MainDlgWndRect.bottom - p_VdoInptPrvwTxtWndRect.bottom; //设置视频文本框窗口的底边距，单位为像素。
	}
	
	//注册音频输入输出设备改变消息。
	{
		class MyMMNotificationClient : public IMMNotificationClient
		{
		public:
			IMMDeviceEnumerator * m_AdoDvcEnumPt; //存放音频设备枚举器的指针。
			ULONG m_RefCnt;

			MyMMNotificationClient( IMMDeviceEnumerator * AdoDvcEnumPt )
			{
				m_AdoDvcEnumPt = AdoDvcEnumPt;
				m_RefCnt = 0;
				AddRef();
			}

			~MyMMNotificationClient()
			{

			}
			
			HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR * ppvObject )
			{
				if( riid == __uuidof( IUnknown ) || riid == __uuidof( IMMNotificationClient ) )
				{
					*ppvObject = ( IMMNotificationClient * )this;
					AddRef();
					return S_OK;
				}
				else
				{
					*ppvObject = NULL;
					return E_NOINTERFACE;
				}
			}

			ULONG STDMETHODCALLTYPE AddRef()
			{
				return InterlockedIncrement( &m_RefCnt );
			}

			ULONG STDMETHODCALLTYPE Release()
			{
				ULONG p_RefCnt = InterlockedDecrement( &m_RefCnt );
				if( 0 == p_RefCnt )
				{
					m_AdoDvcEnumPt->UnregisterEndpointNotificationCallback( this );
					m_AdoDvcEnumPt->Release();
					delete this;
				}
				return p_RefCnt;
			}

			HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId ) //默认的音频输入输出设备改变回调。
			{
				if( ( flow == eCapture ) && ( role == eConsole ) )
				{
					LOGI( Cu8vstr( "接收到默认的音频输入设备改变消息。" ) );
					PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypAdoInptDvcChg, 0, 0 );
				}
				else if( ( flow == eRender ) && ( role == eConsole ) )
				{
					LOGI( Cu8vstr( "接收到默认的音频输出设备改变消息。" ) );
					PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypAdoOtptDvcChg, 0, 0 );
				}
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE OnDeviceAdded( LPCWSTR pwstrDeviceId ) //音频输入输出设备增加回调。
			{
				IMMDevice * p_DvcPt = NULL;
				IMMEndpoint * p_EndpointPt = NULL;
				EDataFlow p_DataFlow;
				HRESULT p_HRslt;

				p_HRslt = m_AdoDvcEnumPt->GetDevice( pwstrDeviceId, &p_DvcPt );
				if( p_HRslt != S_OK )
				{
					goto Out;
				}

				p_HRslt = p_DvcPt->QueryInterface( __uuidof( IMMEndpoint ), ( void * * )&p_EndpointPt );
				if( p_HRslt != S_OK )
				{
					goto Out;
				}

				p_HRslt = p_EndpointPt->GetDataFlow( &p_DataFlow );
				if( p_HRslt != S_OK )
				{
					goto Out;
				}

				if( p_DataFlow == eCapture )
				{
					LOGI( Cu8vstr( "接收到音频输入设备改变消息。" ) );
					PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypAdoInptDvcChg, 0, 0 );
				}
				else if( p_DataFlow == eRender )
				{
					LOGI( Cu8vstr( "接收到音频输出设备改变消息。" ) );
					PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypAdoOtptDvcChg, 0, 0 );
				}

				Out:
				if( p_EndpointPt != NULL ) p_EndpointPt->Release();
				if( p_DvcPt != NULL ) p_DvcPt->Release();
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE OnDeviceRemoved( LPCWSTR pwstrDeviceId ) //音频输入输出设备删除回调。
			{
				return OnDeviceAdded( pwstrDeviceId );
			}

			HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( LPCWSTR pwstrDeviceId, DWORD dwNewState ) //音频输入输出设备状态改变回调。
			{
				return OnDeviceAdded( pwstrDeviceId );
			}

			HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( LPCWSTR pwstrDeviceId, const PROPERTYKEY key ) //音频输入输出设备属性改变回调。
			{
				return S_OK;
			}
		};

		IMMDeviceEnumerator * p_AdoDvcEnumPt = NULL;
		HRESULT p_HRslt = CoCreateInstance( __uuidof( MMDeviceEnumerator ), NULL, CLSCTX_ALL, __uuidof( IMMDeviceEnumerator ), ( void * * )&p_AdoDvcEnumPt );
		if( p_HRslt == S_OK )
		{
			g_NotificationClntPt = new MyMMNotificationClient( p_AdoDvcEnumPt );
			p_AdoDvcEnumPt->RegisterEndpointNotificationCallback( g_NotificationClntPt );
		}
	}
	
	//注册视频输入设备改变消息。
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof( NotificationFilter ) );
		NotificationFilter.dbcc_size = sizeof( DEV_BROADCAST_DEVICEINTERFACE );
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = KSCATEGORY_CAPTURE;
		RegisterDeviceNotification( g_MainDlgWndHdl, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE );
	}

	//打印当前进程活动目录的完整绝对路径到日志。
	FuncGetCurActPath( g_ErrInfoVstr.m_VstrPt, g_ErrInfoVstr.m_VstrPt );
	g_ErrInfoVstr.Ins( 0, Cu8vstr( "当前进程活动目录的完整绝对路径：" ) );
	LOGI( g_ErrInfoVstr.m_VstrPt );
	{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	
	//重置设置。
	ResetStng();
	
	//读取设置。
	if( _waccess( L"Stng.xml", 0 ) == 0 ) ReadStngFromXmlFile();

	//显示主对话框窗口。放在初始化最后才显示，这样用户看不见初始化的过程。
	ShowWindow( g_MainDlgWndHdl, SW_SHOW );
	SetVdoWndShowMode( 0 );

	//处理命令行。
	{
		int p_Argc;
		wchar_t * * p_Argv = CommandLineToArgvW( lpCmdLine, &p_Argc ); //获取命令行参数指针数组。

		for( int p_Argn = 0; p_Argn < p_Argc; p_Argn++ )
		{
			if( wcscmp( p_Argv[ p_Argn ], L"-Title" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetWindowText( g_MainDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SrvrStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_ClntStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_PttDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_StngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_AjbStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SaveStsToTxtFileStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SpeexAecStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_WebRtcAecmStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_WebRtcAecStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_WebRtcAec3StngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SpeexWebRtcAecStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SpeexPrpocsNsStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_WebRtcNsxStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_WebRtcNsStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SpeexPrpocsStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SpeexCodecStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, p_Argv[ p_Argn ] );
					SetWindowText( g_OpenH264CodecStngDlgWndHdl, p_Argv[ p_Argn ] );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-SrvrUrl" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_MainDlgWndHdl, SrvrUrlCbBoxId, p_Argv[ p_Argn ] );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-AddClntSrvrUrl" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId, p_Argv[ p_Argn ] );
					SendMessage( g_MainDlgWndHdl, WM_COMMAND, ClntAddBtnId, 0 );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-AdoInpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoAdoInpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-AdoOtpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoAdoOtpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoInpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoVdoInpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoOtpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoVdoOtpt" ) == 0 )
			{
				CheckDlgButton( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-SaveStsToTxtFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId, BST_CHECKED );
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId, p_Argv[ p_Argn ] );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoSaveStsToTxtFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-PrintLogShowToast" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoPrintLogShowToast" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-SaveAdoVdoInptOtptToAviFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_CHECKED );
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId, p_Argv[ p_Argn ] );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoSaveAdoVdoInptOtptToAviFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-UseSystemAecNsAgc" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoUseSystemAecNsAgc" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-SaveAdoInptOtptToWaveFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
				if( p_Argn + 3 < p_Argc )
				{
					p_Argn++;
					SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId, p_Argv[ p_Argn ] );
					p_Argn++;
					SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId, p_Argv[ p_Argn ] );
					p_Argn++;
					SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId, p_Argv[ p_Argn ] );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-NoSaveAdoInptOtptToWaveFile" ) == 0 )
			{
				CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_UNCHECKED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoWndShowMode" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					int p_VdoWndShowMode;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &p_VdoWndShowMode, sizeof( p_VdoWndShowMode ) );
					SetVdoWndShowMode( p_VdoWndShowMode );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoFrmSz" ) == 0 )
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
			else if( wcscmp( p_Argv[ p_Argn ], L"-WndSz" ) == 0 )
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
			else if( wcscmp( p_Argv[ p_Argn ], L"-ShowWnd" ) == 0 )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOW );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-HideWnd" ) == 0 )
			{
				ShowWindow( g_MainDlgWndHdl, SW_HIDE );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-MinWnd" ) == 0 )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOWMINIMIZED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-MaxWnd" ) == 0 )
			{
				ShowWindow( g_MainDlgWndHdl, SW_SHOWMAXIMIZED );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-SrvrCreate" ) == 0 )
			{
				SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), BM_CLICK, 0, 0 );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-ClntCnct" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					ListView_SetSelectionMark( g_ClntLstWndHdl, _wtoi( p_Argv[ p_Argn ] ) );
					SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntCnctOrDstoyBtnId ), BM_CLICK, 0, 0 );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-BdctInit" ) == 0 )
			{
				if( g_MyClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未初始化。
				{
					g_MyClntMediaPocsThrdPt = new MyClntMediaPocsThrdCls( g_MainDlgWndHdl );
					if( g_MyClntMediaPocsThrdPt->Init( g_LicnCodePt ) != 0 )
					{
						delete g_MyClntMediaPocsThrdPt;
						g_MyClntMediaPocsThrdPt = NULL;
					}
				}

				if( g_MyClntMediaPocsThrdPt != NULL ) //如果我的客户端媒体处理线程已初始化。
				{
					g_MyClntMediaPocsThrdPt->BdctInit( g_LicnCodePt );
				}
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
				g_MyClntMediaPocsThrdPt->SendTkbkClntPttBtnDownMsg( 1, NULL );
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				g_MyClntMediaPocsThrdPt->SendTkbkClntPttBtnUpMsg( 1, NULL );
			}
		}
		else if( p_Msg.hwnd == g_PtbBtnWndHdl )
		{
			if( p_Msg.message == WM_LBUTTONDOWN )
			{
				if( g_MyClntMediaPocsThrdPt == NULL ) //如果我的客户端媒体处理线程未初始化。
				{
					g_MyClntMediaPocsThrdPt = new MyClntMediaPocsThrdCls( g_MainDlgWndHdl );
					if( g_MyClntMediaPocsThrdPt->Init( g_LicnCodePt ) != 0 )
					{
						delete g_MyClntMediaPocsThrdPt;
						g_MyClntMediaPocsThrdPt = NULL;
					}
				}

				if( g_MyClntMediaPocsThrdPt != NULL ) //如果我的客户端媒体处理线程已初始化。
				{
					g_MyClntMediaPocsThrdPt->BdctInit( g_LicnCodePt );
				}
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				if( g_MyClntMediaPocsThrdPt != NULL ) //如果我的客户端媒体处理线程已初始化。
				{
					g_MyClntMediaPocsThrdPt->BdctDstoy();
				}
			}
		}
		
		//if( IsDialogMessage( p_Msg.hwnd, &p_Msg ) == 0 )
		if( TranslateAccelerator( p_Msg.hwnd, NULL, &p_Msg ) == 0 )
		{
			TranslateMessage( &p_Msg );
			DispatchMessage( &p_Msg );
		}
	}

	if( g_NotificationClntPt != NULL ) g_NotificationClntPt->Release(); //销毁音频输入输出设备改变消息客户端。
	g_VdoInptOtptWndCntnr.Dstoy( NULL ); //视频输入输出窗口容器销毁。
	LogDstoy(); //销毁日志。
	g_ErrInfoVstr.Dstoy(); //销毁错误信息动态字符串。
	CoUninitialize(); //销毁COM库。

	#ifdef __DEBUG__
	_ASSERTE( _CrtCheckMemory() ); //检查堆和栈是否被破坏。
	//_CrtDumpMemoryLeaks(); //打印内存泄漏信息到输出窗口。可以不用手动调用，退出时会再自动调用一次。
	#endif
	return (int) p_Msg.wParam;
}
