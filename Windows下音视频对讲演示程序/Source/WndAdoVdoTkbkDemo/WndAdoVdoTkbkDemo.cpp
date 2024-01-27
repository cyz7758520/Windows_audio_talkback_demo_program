// WndAdoVdoTkbkDemo.cpp : 定义应用程序的入口点。
//
#if( defined __X86__ )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif( defined __X64__ )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "WndAdoVdoTkbkDemo.h"
#include "WndAdoVdoTkbkStng.h"
#include "MyNtwkMediaPocsThrd.h"

//全局变量。
HINSTANCE g_IstnsHdl; //存放当前实例的句柄。
VstrCls g_ErrInfoVstr; //存放错误信息动态字符串的指针。
MyNtwkMediaPocsThrdCls * g_MyNtwkMediaPocsThrdPt = NULL; //存放我的网络媒体处理线程的指针。
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
HWND g_VdoInptPrvwTxtWndHdl = NULL; //存放视频输入预览文本框窗口的句柄。
HWND g_VdoOtptDspyTxtWndHdl = NULL; //存放视频输出显示文本框窗口的句柄。
long g_VdoTxtWndLeftMargin = 0; //存放视频文本框窗口的左边距，单位为像素。
long g_VdoTxtWndTopMargin = 0; //存放视频文本框窗口的顶边距，单位为像素。
long g_VdoTxtWndRightMargin = 0; //存放视频文本框窗口的右边距，单位为像素。
long g_VdoTxtWndBottomMargin = 0; //存放视频文本框窗口的底边距，单位为像素。
int g_VdoWndShowMode = 0; //存放视频窗口的显示模式，为0表示正常，为1表示垂直最大化排列，为2表示水平最大化排列。
HWND g_VdoInptPrvwWndHdl = NULL; //存放视频输入预览窗口的句柄。
HWND g_VdoOtptDspyWndHdl = NULL; //存放视频输出显示窗口的句柄。
HWND g_PttDlgWndHdl = NULL; //存放一键即按即通对话框窗口的句柄。
HWND g_PttBtnWndHdl = NULL; //存放一键即按即通按钮窗口的句柄。

HWND g_StngDlgWndHdl = NULL; //存放设置对话框窗口的句柄。
HWND g_AjbStngDlgWndHdl = NULL; //存放自适应抖动缓冲器设置对话框窗口的句柄。
HWND g_SaveStsToTxtFileStngDlgWndHdl = NULL; //存放保存状态到Txt文件设置对话框窗口的句柄。
HWND g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl = NULL; //存放保存音视频输入输出到Avi文件设置对话框窗口的句柄。
HWND g_SpeexAecStngDlgWndHdl = NULL; //存放Speex声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecmStngDlgWndHdl = NULL; //存放WebRtc定点版声学回音消除器设置对话框窗口的句柄。
HWND g_WebRtcAecStngDlgWndHdl = NULL; //存放WebRtc浮点版声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexWebRtcAecStngDlgWndHdl = NULL; //存放SpeexWebRtc三重声学回音消除器设置对话框窗口的句柄。
HWND g_SpeexPrpocsNsStngDlgWndHdl = NULL; //存放Speex预处理器的噪音抑制设置对话框窗口的句柄。
HWND g_WebRtcNsxStngDlgWndHdl = NULL; //存放WebRtc定点版噪音抑制器设置对话框窗口的句柄。
HWND g_WebRtcNsStngDlgWndHdl = NULL; //存放WebRtc浮点版噪音抑制器设置对话框窗口的句柄。
HWND g_SpeexPrpocsStngDlgWndHdl = NULL; //存放Speex预处理器的设置对话框窗口的句柄。
HWND g_SpeexCodecStngDlgWndHdl = NULL; //存放Speex编解码器设置对话框窗口的句柄。
HWND g_SaveAdoInptOtptToWaveFileStngDlgWndHdl = NULL; //存放保存音频输入输出到Wave文件设置对话框窗口的句柄。
HWND g_OpenH264CodecStngDlgWndHdl = NULL; //存放OpenH264编解码器设置对话框窗口的句柄。

void MyNtwkMediaPocsThrdDstoy();
void BdctDstoy();

 //我的网络媒体处理线程初始化。
int MyNtwkMediaPocsThrdInit()
{
	int p_Rslt = -1; //存放本函数的执行结果，为0表示成功，为非0表示失败。

	if( g_MyNtwkMediaPocsThrdPt == NULL ) //如果媒体处理线程还没有启动。
	{
		wchar_t p_TmpU16strPt[ 1024 ];

		LOGI( Cu8vstr( "开始启动媒体处理线程。" ) );

		//初始化媒体处理线程的指针。
		g_MyNtwkMediaPocsThrdPt = new MyNtwkMediaPocsThrdCls( g_MainDlgWndHdl );
		if( g_MyNtwkMediaPocsThrdPt->Init( &g_ErrInfoVstr ) != 0 )
		{
			LOGFE( Cu8vstr( "初始化媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
			goto Out;
		}

		//设置网络。
		{
			//设置传输模式。
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode = ( IsDlgButtonChecked( g_ClntStngDlgWndHdl, UsePttRdBtnId ) == BST_CHECKED ) ? 0 : 1;

			//设置最大连接次数。
			GetWindowText( GetDlgItem( g_ClntStngDlgWndHdl, MaxCnctTimesEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_MaxCnctTimes = _wtoi( p_TmpU16strPt );

			//设置是否自动激活连接。
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_IsAutoActCnct = ( IsDlgButtonChecked( g_SrvrStngDlgWndHdl, IsAutoActCnctCkBoxId ) == BST_CHECKED ) ? 1 : 0;

			//设置最大连接数。
			GetWindowText( GetDlgItem( g_SrvrStngDlgWndHdl, MaxCnctNumEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_MaxCnctNum = _wtoi( p_TmpU16strPt );

			//设置所有连接销毁时是否自动请求退出。
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_IsAutoRqirExit = 2;
		}

		//设置是否使用容器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_UseWhatRecvOtptFrm = 0;
		}

		//设置是否使用自适应抖动缓冲器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseAjbRecvOtptFrmRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_UseWhatRecvOtptFrm = 1;

			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_AAjb.m_MinNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_AAjb.m_MaxNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_AAjb.m_MaxCntuLostFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_AAjb.m_AdaptSensitivity = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_VAjb.m_MinNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_VAjb.m_MaxNeedBufFrmCnt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_VAjb.m_AdaptSensitivity = _wtof( p_TmpU16strPt );
		}

		//设置是否打印Log日志、显示Toast。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetIsPrintLogShowToast(
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			NULL,
			&g_ErrInfoVstr );

		//设置是否使用阻止系统休眠。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetIsUsePrvntSysSleep(
			0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId ) == BST_CHECKED ) ? 1 : 0,
			&g_ErrInfoVstr );

		//设置是否保存音视频输入输出到Avi文件。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED )
		{
			wchar_t p_FullPathU16strPt[ 1024 ];
			size_t p_WrBufSzByt;

			GetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId ), p_FullPathU16strPt, SzOfArr( p_FullPathU16strPt ) );
			GetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			p_WrBufSzByt = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetIsSaveAdoVdoInptOtptToAviFile(
				0,
				Cu16vstr( p_FullPathU16strPt ),
				p_WrBufSzByt,
				( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
				( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
				( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
				( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0,
				&g_ErrInfoVstr );
		}

		//设置音频输入。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetAdoInpt(
			0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
			&g_ErrInfoVstr );

		//设置音频输入是否使用系统自带的声学回音消除器、噪音抑制器和自动增益控制器。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsUseSystemAecNsAgc( 0, ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );

		if( g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode == 0 ) //如果传输模式为实时半双工（一键通）。
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseNoAec( 0, &g_ErrInfoVstr );
		}
		else //如果传输模式为实时全双工。
		{
			//设置音频输入是否不使用声学回音消除器。
			if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED )
			{
				g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseNoAec( 0, &g_ErrInfoVstr );
			}

			//设置音频输入是否使用Speex声学回音消除器。
			if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED )
			{
				int32_t p_FilterLenMsec;
				int32_t p_IsUseRec;
				float p_EchoMultiple;
				float p_EchoCntu;
				int32_t p_EchoSupes;
				int32_t p_EchoSupesAct;
				int p_IsSaveMemFile;

				GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_FilterLenMsec = _wtoi( p_TmpU16strPt );
				p_IsUseRec = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
				GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoMultiple = _wtof( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoCntu = _wtof( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoSupes = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoSupesAct = _wtoi( p_TmpU16strPt );
				p_IsSaveMemFile = ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

				g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseSpeexAec( 0, p_FilterLenMsec, p_IsUseRec, p_EchoMultiple, p_EchoCntu, p_EchoSupes, p_EchoSupesAct, p_IsSaveMemFile, Cu8vstr( ".\\SpeexAecMem" ), &g_ErrInfoVstr );
			}

			//设置音频输入是否使用WebRtc定点版声学回音消除器。
			if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecmRdBtnId ) == BST_CHECKED )
			{
				int32_t p_IsUseCNGMode;
				int32_t p_EchoMode;
				int32_t p_Delay;

				p_IsUseCNGMode = ( IsDlgButtonChecked( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
				GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoMode = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_Delay = _wtoi( p_TmpU16strPt );

				g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcAecm( 0, p_IsUseCNGMode, p_EchoMode, p_Delay, &g_ErrInfoVstr );
			}

			//设置音频输入是否使用WebRtc浮点版声学回音消除器。
			if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecRdBtnId ) == BST_CHECKED )
			{
				int32_t p_EchoMode;
				int32_t p_Delay;
				int32_t p_IsUseDelayAgstcMode;
				int32_t p_IsUseExtdFilterMode;
				int32_t p_IsUseRefinedFilterAdaptAecMode;
				int32_t p_IsUseAdaptAdjDelay;
				int p_IsSaveMemFile;

				GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_EchoMode = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_Delay = _wtoi( p_TmpU16strPt );
				p_IsUseDelayAgstcMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
				p_IsUseExtdFilterMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
				p_IsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
				p_IsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
				p_IsSaveMemFile = ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId ) == BST_CHECKED );

				g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcAec( 0, p_EchoMode, p_Delay, p_IsUseDelayAgstcMode, p_IsUseExtdFilterMode, p_IsUseRefinedFilterAdaptAecMode, p_IsUseAdaptAdjDelay, p_IsSaveMemFile, Cu8vstr( ".\\WebRtcAecMem" ), &g_ErrInfoVstr );
			}

			//设置音频输入是否使用SpeexWebRtc三重声学回音消除器。
			if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED )
			{
				int32_t p_WorkMode;
				int32_t p_SpeexAecFilterLenMsec;
				int32_t p_SpeexAecIsUseRec;
				float p_SpeexAecEchoMultiple;
				float p_SpeexAecEchoCntu;
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
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SpeexAecFilterLenMsec = _wtoi( p_TmpU16strPt );
				p_SpeexAecIsUseRec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId ) == BST_CHECKED );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SpeexAecEchoMultiple = _wtof( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SpeexAecEchoCntu = _wtof( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SpeexAecEchoSupes = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SpeexAecEchoSupesAct = _wtoi( p_TmpU16strPt );
				p_WebRtcAecmIsUseCNGMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_WebRtcAecmEchoMode = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_WebRtcAecmDelay = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_WebRtcAecEchoMode = _wtoi( p_TmpU16strPt );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_WebRtcAecDelay = _wtoi( p_TmpU16strPt );
				p_WebRtcAecIsUseDelayAgstcMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED );
				p_WebRtcAecIsUseExtdFilterMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED );
				p_WebRtcAecIsUseRefinedFilterAdaptAecMode = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED );
				p_WebRtcAecIsUseAdaptAdjDelay = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED );
				p_IsUseSameRoomAec = ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId ) == BST_CHECKED );
				GetWindowText( GetDlgItem( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
				p_SameRoomEchoMinDelay = _wtoi( p_TmpU16strPt );

				g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseSpeexWebRtcAec( 0, p_WorkMode, p_SpeexAecFilterLenMsec, p_SpeexAecIsUseRec, p_SpeexAecEchoMultiple, p_SpeexAecEchoCntu, p_SpeexAecEchoSupes, p_SpeexAecEchoSupesAct, p_WebRtcAecmIsUseCNGMode, p_WebRtcAecmEchoMode, p_WebRtcAecmDelay, p_WebRtcAecEchoMode, p_WebRtcAecDelay, p_WebRtcAecIsUseDelayAgstcMode, p_WebRtcAecIsUseExtdFilterMode, p_WebRtcAecIsUseRefinedFilterAdaptAecMode, p_WebRtcAecIsUseAdaptAdjDelay, p_IsUseSameRoomAec, p_SameRoomEchoMinDelay, &g_ErrInfoVstr );
			}
		}

		//设置音频输入是否不使用噪音抑制器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseNoNs( 0, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用Speex预处理器的噪音抑制。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexPrpocsNsRdBtnId ) == BST_CHECKED )
		{
			int32_t p_IsUseNs;
			int32_t p_NoiseSupes;
			int32_t p_IsUseDereverb;

			p_IsUseNs = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_NoiseSupes = _wtoi( p_TmpU16strPt );
			p_IsUseDereverb = ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId ) == BST_CHECKED );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseSpeexPrpocsNs( 0, p_IsUseNs, p_NoiseSupes, p_IsUseDereverb, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用WebRtc定点版噪音抑制器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED )
		{
			int32_t p_PolicyMode;

			GetWindowText( GetDlgItem( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_PolicyMode = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcNsx( 0, p_PolicyMode, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用WebRtc浮点版噪音抑制器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED )
		{
			int32_t p_PolicyMode;

			GetWindowText( GetDlgItem( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_PolicyMode = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseWebRtcNs( 0, p_PolicyMode, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用RNNoise噪音抑制器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseRNNoise( 0, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用Speex预处理器。
		{
			int32_t p_IsUseVad;
			int32_t p_VadProbStart;
			int32_t p_VadProbCont;
			int32_t p_IsUseAgc;
			int32_t p_AgcLevel;
			int32_t p_AgcIncrement;
			int32_t p_AgcDecrement;
			int32_t p_AgcMaxGain;

			p_IsUseVad = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_VadProbStart = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_VadProbCont = _wtoi( p_TmpU16strPt );
			p_IsUseAgc = ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_AgcLevel = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_AgcIncrement = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_AgcDecrement = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_AgcMaxGain = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsUseSpeexPrpocs( 0, ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId ) == BST_CHECKED ) ? 1 : 0, p_IsUseVad, p_VadProbStart, p_VadProbCont, p_IsUseAgc, p_AgcLevel, p_AgcIncrement, p_AgcDecrement, p_AgcMaxGain, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用PCM原始数据。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUsePcm( 0, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用Speex编码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_EncdUseCbrOrVbr;
			int32_t p_EncdQualt;
			int32_t p_EncdCmplxt;
			int32_t p_EncdPlcExptLossRate;

			p_EncdUseCbrOrVbr = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId ) == BST_CHECKED ) ? 0 : 1;
			GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EncdQualt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EncdCmplxt = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_EncdPlcExptLossRate = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseSpeexEncd( 0, p_EncdUseCbrOrVbr, p_EncdQualt, p_EncdCmplxt, p_EncdPlcExptLossRate, &g_ErrInfoVstr );
		}

		//设置音频输入是否使用Opus编码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseOpusEncd( 0, &g_ErrInfoVstr );
		}

		//设置音频输入是否绘制音频波形到窗口。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsDrawAdoWavfmToWnd( 0,
																					  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																					  GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ),
																					  GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ),
																					  &g_ErrInfoVstr );

		//设置音频输入是否保存音频到Wave文件。
		if( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) &&
			( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) )
		{
			wchar_t p_AdoInptSrcFullPathU16strPt[ 1024 ];
			wchar_t p_AdoInptRsltFullPathU16strPt[ 1024 ];
			size_t p_WrBufSzByt;

			GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId ), p_AdoInptSrcFullPathU16strPt, SzOfArr( p_AdoInptSrcFullPathU16strPt ) );
			GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId ), p_AdoInptRsltFullPathU16strPt, SzOfArr( p_AdoInptRsltFullPathU16strPt ) );
			GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			p_WrBufSzByt = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsSaveAdoToWaveFile( 0,
																						  ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																						  Cu16vstr( p_AdoInptSrcFullPathU16strPt ),
																						  Cu16vstr( p_AdoInptRsltFullPathU16strPt ),
																						  p_WrBufSzByt,
																						  &g_ErrInfoVstr );
		}

		//设置音频输入使用的设备。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseDvc( 0,
																		 SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
																		 &g_ErrInfoVstr );

		//设置音频输入是否静音。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsMute( 0,
																		 ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																		 &g_ErrInfoVstr );

		//设置音频输出。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetAdoOtpt(
			0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 0,
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
			( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 0,
			&g_ErrInfoVstr );
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptAddStrm( 0, 0, &g_ErrInfoVstr );

		//设置音频输出是否使用PCM原始数据。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetStrmUsePcm( 0, 0, &g_ErrInfoVstr );
		}

		//设置音频输出是否使用Speex解码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_DecdIsUsePrcplEnhsmt;

			p_DecdIsUsePrcplEnhsmt = ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId ) == BST_CHECKED );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetStrmUseSpeexDecd( 0, 0, p_DecdIsUsePrcplEnhsmt, &g_ErrInfoVstr );
		}

		//设置音频输出是否使用Opus解码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetStrmUseOpusDecd( 0, 0, &g_ErrInfoVstr );
		}

		//设置音频输出流是否使用。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetStrmIsUse( 0, 0, 1, &g_ErrInfoVstr );

		//设置音频输出是否绘制音频波形到窗口。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsDrawAdoWavfmToWnd( 0,
																					  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																					  GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ),
																					  &g_ErrInfoVstr );

		//设置音频输出是否保存音频到Wave文件。
		if( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) &&
			( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) )
		{
			wchar_t p_AdoOtptSrcFullPathU16strPt[ 1024 ];
			size_t p_WrBufSzByt;

			GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId ), p_AdoOtptSrcFullPathU16strPt, SzOfArr( p_AdoOtptSrcFullPathU16strPt ) );
			GetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId ), p_TmpU16strPt, SzOfArr( p_TmpU16strPt ) );
			p_WrBufSzByt = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsSaveAdoToWaveFile( 0,
																						  ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																						  Cu16vstr( p_AdoOtptSrcFullPathU16strPt ),
																						  p_WrBufSzByt,
																						  &g_ErrInfoVstr );
		}

		//设置音频输出使用的设备。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetUseDvc( 0,
																		 SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
																		 &g_ErrInfoVstr );

		//设置音频输出是否静音。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsMute( 0,
																		 ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																		 &g_ErrInfoVstr );

		//设置视频输入。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId ) == BST_CHECKED ) //如果要使用预设的帧的大小。
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetVdoInpt(
				0,
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
		else //如果要使用其他的帧的大小。
		{
			int32_t p_VdoFrmSzOtherWidth;
			int32_t p_VdoFrmSzOtherHeight;

			GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_VdoFrmSzOtherWidth = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_VdoFrmSzOtherHeight = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SetVdoInpt(
				0,
				( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
				( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
				( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
				( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 0,
				p_VdoFrmSzOtherWidth,
				p_VdoFrmSzOtherHeight,
				g_VdoInptPrvwWndHdl,
				&g_ErrInfoVstr );
		}

		//设置视频输入是否使用Yu12原始数据。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYu12RdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetUseYu12( 0, &g_ErrInfoVstr );
		}

		//设置视频输入是否使用OpenH264编码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
		{
			int32_t p_OpenH264EncdVdoType;
			int32_t p_OpenH264EncdEncdBitrate;
			int32_t p_OpenH264EncdBitrateCtrlMode;
			int32_t p_OpenH264EncdIDRFrmIntvl;
			int32_t p_OpenH264EncdCmplxt;

			GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_OpenH264EncdVdoType = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_OpenH264EncdEncdBitrate = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_OpenH264EncdBitrateCtrlMode = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_OpenH264EncdIDRFrmIntvl = _wtoi( p_TmpU16strPt );
			GetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), p_TmpU16strPt, sizeof( p_TmpU16strPt ) );
			p_OpenH264EncdCmplxt = _wtoi( p_TmpU16strPt );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetUseOpenH264Encd( 0,
																					  p_OpenH264EncdVdoType,
																					  p_OpenH264EncdEncdBitrate * 1024 * 8,
																					  p_OpenH264EncdBitrateCtrlMode,
																					  p_OpenH264EncdIDRFrmIntvl,
																					  p_OpenH264EncdCmplxt,
																					  &g_ErrInfoVstr );
		}

		//设置视频输入使用的设备。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetUseDvc( 0,
																		 SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ),
																		 &g_ErrInfoVstr );

								  //设置视频输入是否黑屏。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetIsBlack( 0,
																		  ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																		  &g_ErrInfoVstr );

								  //设置视频输出。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptAddStrm( 0, 0, NULL );
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrm( 0,
																	   0,
																	   g_VdoOtptDspyWndHdl,
																	   &g_ErrInfoVstr );

		//设置视频输出是否使用Yu12原始数据。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseYu12RdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmUseYu12( 0, 0, &g_ErrInfoVstr );
		}

		//设置视频输出是否使用OpenH264解码器。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED )
		{
			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmUseOpenH264Decd( 0, 0, 0, &g_ErrInfoVstr );
		}

		//设置视频输出是否黑屏。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmIsBlack( 0,
																			  0,
																			  ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																			  &g_ErrInfoVstr );

		//设置视频输出流是否使用。
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmIsUse( 0, 0, 1, &g_ErrInfoVstr );

		//设置本端对讲模式。
		g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 0,
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeNone :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeAdo :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeVdo :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeAdoVdo :
											  NtwkMediaPocsThrd::TkbkModeNoChg, &g_ErrInfoVstr );

		//设置是否保存状态到Txt文件。
		if( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId ) == BST_CHECKED )
		{
			wchar_t p_FullPathU16strPt[ 1024 ];

			GetWindowText( GetDlgItem( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId ), p_FullPathU16strPt, SzOfArr( p_FullPathU16strPt ) );

			g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->SaveStsToTxtFile( 0,
																			 Cu16vstr( p_FullPathU16strPt ),
																			 &g_ErrInfoVstr );
		}

		//设置本端对讲模式。
		g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 0,
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeNone :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeAdo :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeVdo :
											  ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoVdoTkbkModeRdBtnId ) == BST_CHECKED ) ? NtwkMediaPocsThrd::TkbkModeAdoVdo :
											  NtwkMediaPocsThrd::TkbkModeNoChg, & g_ErrInfoVstr );

		//启动媒体处理线程。
		if( g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->Start( &g_ErrInfoVstr ) != 0 )
		{
			LOGFE( Cu8vstr( "启动媒体处理线程失败。原因：%vs" ), g_ErrInfoVstr.m_VstrPt );
			goto Out;
		}

		LOGI( Cu8vstr( "启动媒体处理线程完毕。" ) );

		p_Rslt = 0; // //设置本函数执行成功。
	}

	Out:
	if( p_Rslt != 0 )
	{
		if( g_MyNtwkMediaPocsThrdPt != NULL )
		{
			MyNtwkMediaPocsThrdDstoy();
			delete g_MyNtwkMediaPocsThrdPt;
			g_MyNtwkMediaPocsThrdPt = NULL;
		}
	}
	return p_Rslt;
}

//我的网络媒体处理线程销毁。
void MyNtwkMediaPocsThrdDstoy()
{
	if( g_MyNtwkMediaPocsThrdPt != NULL )
	{
		LOGI( Cu8vstr( "请求并等待我的网络媒体处理线程退出开始。" ) );
		g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->RqirExit( 1, 1, &g_ErrInfoVstr );
		LOGI( Cu8vstr( "请求并等待我的网络媒体处理线程退出结束。" ) );
	}
}

//广播初始化。
int BdctInit()
{
	int p_Rslt = -1; //存放本函数执行结果，为0表示成功，为非0表示失败。
	int p_IsSrvrOrClntCnct;
	Vstr * p_RmtNodeNameVstrPt = NULL;
	Vstr * p_RmtNodeSrvcVstrPt = NULL;
	wchar_t p_TextTmpArr[ 1024 ];

	LOGI( Cu8vstr( "广播初始化开始。" ) );

	if( g_MyNtwkMediaPocsThrdPt == NULL ) //如果我的网络媒体处理线程还没有启动。
	{
		if( MyNtwkMediaPocsThrdInit() != 0 ) //如果我的网络媒体处理线程初始化失败。
		{
			goto Out;
		}
	}

	//添加客户端列表。
	VstrInit( &p_RmtNodeNameVstrPt, Utf16, ,  );
	VstrInit( &p_RmtNodeSrvcVstrPt, Utf16, ,  );
	for( int p_Num = 0, p_ClntLstTotal = ListView_GetItemCount( g_ClntLstWndHdl ); p_Num < p_ClntLstTotal; p_Num++ )
	{
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
		p_IsSrvrOrClntCnct = ( wcscmp( p_TextTmpArr, L"Tcp" ) == 0 ) ? 0 : 1;
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
		VstrCpy( p_RmtNodeNameVstrPt, Cu16vstr( p_TextTmpArr ), ,  );
		ListView_GetItemText( g_ClntLstWndHdl, p_Num, 3, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
		VstrCpy( p_RmtNodeSrvcVstrPt, Cu16vstr( p_TextTmpArr ), ,  );
		g_MyNtwkMediaPocsThrdPt->BdctCnctInit( 1, p_IsSrvrOrClntCnct, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt, NULL );
	}

	//设置本端对讲模式。
	g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 1, NtwkMediaPocsThrd::TkbkModeNoChg, NULL );

	LOGI( Cu8vstr( "广播初始化结束。" ) );

	p_Rslt = 0; //设置本函数执行成功。

	Out:
	VstrDstoy( p_RmtNodeNameVstrPt );
	VstrDstoy( p_RmtNodeSrvcVstrPt );
	if( p_Rslt != 0 ) //如果本函数执行失败。
	{
		BdctDstoy();
	}
	return p_Rslt;
}

//广播销毁。
void BdctDstoy()
{
	if( g_MyNtwkMediaPocsThrdPt != NULL )
	{
		LOGI( Cu8vstr( "开始请求并等待广播销毁。" ) );
		g_MyNtwkMediaPocsThrdPt->BdctCnctAllDstoy( 1, NULL );
		LOGI( Cu8vstr( "结束请求并等待广播销毁。" ) );
	}
}

//刷新音视频设备。
void RefresAdoVdohDvc()
{
	Vstr * * p_AdoInptDvcNameVstrArrPt = NULL;
	UINT p_AdoInptDvcTotal = 0;
	Vstr * * p_AdoOtptDvcNameVstrArrPt = NULL;
	UINT p_AdoOtptDvcTotal = 0;
	Vstr * * p_VdoInptDvcNameVstrArrPt = NULL;
	UINT p_VdoInptDvcTotal = 0;
	long p_TmpInt;

	//获取音频输入设备、音频输出设备、视频输入设备的名称。
	if( MediaPocsThrdGetAdoInptDvcName( &p_AdoInptDvcNameVstrArrPt, &p_AdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
	{
		g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输入设备名称失败。原因：" ) );
		LOGE( g_ErrInfoVstr.m_VstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto RefresAdoVdohDvc;
	}
	if( MediaPocsThrdGetAdoOtptDvcName( &p_AdoOtptDvcNameVstrArrPt, &p_AdoOtptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
	{
		g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取音频输出设备名称失败。原因：" ) );
		LOGE( g_ErrInfoVstr.m_VstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto RefresAdoVdohDvc;
	}
	if( MediaPocsThrdGetVdoInptDvcName( &p_VdoInptDvcNameVstrArrPt, &p_VdoInptDvcTotal, g_ErrInfoVstr.m_VstrPt ) != 0 )
	{
		g_ErrInfoVstr.Ins( 0, Cu8vstr( "获取视频输入设备名称失败。原因：" ) );
		LOGE( g_ErrInfoVstr.m_VstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto RefresAdoVdohDvc;
	}

	//清空音频输入设备、音频输出设备、视频输入设备的组合框。
	SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );
	SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );
	SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_RESETCONTENT, 0, 0 );

	//添加音频输入设备、音频输出设备、视频输入设备到组合框。
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

	//默认选择第一个设备。
	SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
	SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );
	SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SETCURSEL, ( WPARAM )0, ( LPARAM )NULL );

	RefresAdoVdohDvc:
	MediaPocsThrdDstoyDvcName( p_AdoInptDvcNameVstrArrPt, p_AdoInptDvcTotal, NULL );
	MediaPocsThrdDstoyDvcName( p_AdoOtptDvcNameVstrArrPt, p_AdoOtptDvcTotal, NULL );
	MediaPocsThrdDstoyDvcName( p_VdoInptDvcNameVstrArrPt, p_VdoInptDvcTotal, NULL );
}

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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_SHOW ); //设置服务端Url组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_SHOW ); //设置服务端创建或销毁按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_SHOW ); //设置服务端设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctActBtnId ), SW_SHOW ); //设置连接激活按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_SHOW ); //设置连接销毁按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_SHOW ); //设置连接列表为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_SHOW ); //设置客户端的服务端Url组合框为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_SHOW ); //设置编辑框添加按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_SHOW ); //设置客户端设置按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctBtnId ), SW_SHOW ); //设置客户端连接按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_SHOW ); //设置客户端删除按钮为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_SHOW ); //设置客户端列表为显示。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId ), SW_SHOW ); //设置挂起对讲模式单选按钮为显示。
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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_HIDE ); //设置服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_HIDE ); //设置服务端创建或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_HIDE ); //设置服务端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctActBtnId ), SW_HIDE ); //设置连接激活按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_HIDE ); //设置连接销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_HIDE ); //设置连接列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_HIDE ); //设置客户端的服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_HIDE ); //设置编辑框添加按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_HIDE ); //设置客户端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctBtnId ), SW_HIDE ); //设置客户端连接按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_HIDE ); //设置客户端删除按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_HIDE ); //设置客户端列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId ), SW_HIDE ); //设置挂起对讲模式单选按钮为隐藏。
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
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), SW_HIDE ); //设置服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), SW_HIDE ); //设置服务端创建或销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), SW_HIDE ); //设置服务端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctActBtnId ), SW_HIDE ); //设置连接激活按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctDstoyBtnId ), SW_HIDE ); //设置连接销毁按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, CnctLstId ), SW_HIDE ); //设置连接列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), SW_HIDE ); //设置客户端的服务端Url组合框为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntAddBtnId ), SW_HIDE ); //设置编辑框添加按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), SW_HIDE ); //设置客户端设置按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntCnctBtnId ), SW_HIDE ); //设置客户端连接按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntDelBtnId ), SW_HIDE ); //设置客户端删除按钮为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, ClntLstId ), SW_HIDE ); //设置客户端列表为隐藏。
				ShowWindow( GetDlgItem( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId ), SW_HIDE ); //设置挂起对讲模式单选按钮为隐藏。
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
			RECT p_LogLstBoxWndRect;
			RECT p_VdoInptPrvwTxtWndRect;
			RECT p_VdoOtptDspyTxtWndHdlRect;

			GetClientRect( g_MainDlgWndHdl, &p_MainDlgWndRect ); //获取主对话框窗口的位置。

			//移动日志列表框窗口。
			GetWindowRect( g_LogLstBoxWndHdl, &p_LogLstBoxWndRect );
			RectScrnToClnt( g_MainDlgWndHdl, p_LogLstBoxWndRect );
			p_LogLstBoxWndRect.bottom = p_MainDlgWndRect.bottom - g_VdoTxtWndBottomMargin;
			SetWindowPos( g_LogLstBoxWndHdl, NULL, p_LogLstBoxWndRect.left, p_LogLstBoxWndRect.top, RectWidth( p_LogLstBoxWndRect ), RectHeight( p_LogLstBoxWndRect ), SWP_NOMOVE | SWP_NOZORDER );
			//InvalidateRect( g_LogLstBoxWndHdl, NULL, TRUE ); //重绘日志列表框窗口。
			
			//移动视频输入预览文本框窗口和视频输出显示文本框窗口。
			SetVdoWndShowMode( g_VdoWndShowMode );

			return ( INT_PTR )TRUE;
		}
		case WM_DEVICECHANGE: //系统硬件设备改变消息。
		{
			wchar_t * p_CurAdoInptDvcNameStrPt = NULL;
			wchar_t * p_CurAdoOtptDvcNameStrPt = NULL;
			wchar_t * p_CurVdoInptDvcNameStrPt = NULL;
			long p_TmpInt;

			//获取当前选择的音频输入设备、音频输出设备、视频输入设备。
			p_TmpInt = SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurAdoInptDvcNameStrPt = ( wchar_t * )alloca( ( SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAdoInptDvcNameStrPt );
			}
			p_TmpInt = SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurAdoOtptDvcNameStrPt = ( wchar_t * )alloca( ( SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurAdoOtptDvcNameStrPt );
			}
			p_TmpInt = SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				p_CurVdoInptDvcNameStrPt = ( wchar_t * )alloca( ( SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_GETLBTEXTLEN, p_TmpInt, NULL ) + 1 ) * 2 );
				SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_CurVdoInptDvcNameStrPt );
			}

			//刷新音视频设备。
			RefresAdoVdohDvc();

			//重新选择之前选择的音频输入设备、音频输出设备、视频输入设备。
			if( p_CurAdoInptDvcNameStrPt != NULL ) SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoInptDvcNameStrPt );
			if( p_CurAdoOtptDvcNameStrPt != NULL ) SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurAdoOtptDvcNameStrPt );
			if( p_CurVdoInptDvcNameStrPt != NULL ) SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_CurVdoInptDvcNameStrPt );
			
			return ( INT_PTR )TRUE;
		}
		case WM_COMMAND: //命令消息。
		{
			switch( LOWORD( wParam ) )
			{
				case SrvrCreateOrDstoyBtnId: //服务器创建或销毁按钮。
				{
					if( g_MyNtwkMediaPocsThrdPt == NULL ) //如果我的网络媒体处理线程还没有初始化。
					{
						if( MyNtwkMediaPocsThrdInit() != 0 ) //如果我的网络媒体处理线程初始化失败。
						{
							break;
						}
					}

					if( g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_SrvrIsInit == 0 ) //如果服务端未初始化。
					{
						HWND p_SrvrUrlHdl;
						int p_SrvrUrlLenChr;
						VstrCls p_VstrTmp;

						p_SrvrUrlHdl = GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId );
						p_SrvrUrlLenChr = GetWindowTextLength( p_SrvrUrlHdl );
						p_VstrTmp.Init( Utf16, p_SrvrUrlLenChr + 1 );
						GetDlgItemText( g_MainDlgWndHdl, SrvrUrlCbBoxId, ( LPWSTR )p_VstrTmp.m_VstrPt->m_Pt, p_SrvrUrlLenChr + 1 );
						p_VstrTmp.m_VstrPt->m_LenChr = p_SrvrUrlLenChr;
						g_MyNtwkMediaPocsThrdPt->SrvrInit( 1, &p_VstrTmp, NULL );
					}
					else
					{
						g_MyNtwkMediaPocsThrdPt->SrvrDstoy( 1, NULL );
					}
					break;
				}
				case SrvrStngBtnId: //服务端设置按钮。
				{
					ShowWindow( g_SrvrStngDlgWndHdl, SW_SHOW ), EnableWindow( hDlg, FALSE ); //显示服务端设置对话框。
					return ( INT_PTR )TRUE;
				}
				case CnctActBtnId: //连接激活按钮。
				{
					int p_Num;
					
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						p_Num = ListView_GetSelectionMark( g_CnctLstWndHdl );
						if( p_Num != -1 )
						{
							g_MyNtwkMediaPocsThrdPt->CnctAct( 1, p_Num, NULL );
						}
					}
					return ( INT_PTR )TRUE;
				}
				case CnctDstoyBtnId: //连接销毁按钮。
				{
					int p_Num;
					
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						p_Num = ListView_GetSelectionMark( g_CnctLstWndHdl );
						if( p_Num != -1 )
						{
							g_MyNtwkMediaPocsThrdPt->CnctDstoy( 1, p_Num, NULL );
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
					Vstr * p_VstrTmpPt = NULL;
					Vstr * p_PrtclVstrPt = NULL;
					Vstr * p_HostnameVstrPt = NULL;
					Vstr * p_PortVstrPt = NULL;
					int p_CmpRslt;

					p_ClntSrvrUrlHdl = GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId );
					p_ClntSrvrUrlLenChr = GetWindowTextLength( p_ClntSrvrUrlHdl );
					VstrInit( &p_VstrTmpPt, Utf16, p_ClntSrvrUrlLenChr + 1, );
					GetDlgItemText( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId, ( LPWSTR )p_VstrTmpPt->m_Pt, p_ClntSrvrUrlLenChr + 1 );
					p_VstrTmpPt->m_LenChr = p_ClntSrvrUrlLenChr;

					VstrInit( &p_PrtclVstrPt, Utf16, ,  );
					VstrInit( &p_HostnameVstrPt, Utf16, , );
					VstrInit( &p_PortVstrPt, Utf16, , );

					if( VstrUrlParse( p_VstrTmpPt, p_PrtclVstrPt, NULL, NULL, p_HostnameVstrPt, p_PortVstrPt, NULL, NULL, NULL, NULL, g_ErrInfoVstr.m_VstrPt ) != 0 )
					{
						g_ErrInfoVstr.Ins( 0, Cu8vstr( "解析客户端的服务端Url字符串失败。原因：" ) );
						LOGE( g_ErrInfoVstr.m_VstrPt );
						Toast( g_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_VstrPt );
						goto ClntAddOut;
					}
					if( ( VstrCmp( p_PrtclVstrPt, Cu8vstr( "Tcp" ), &p_CmpRslt ),p_CmpRslt != 0 ) && ( VstrCmp( p_PrtclVstrPt, Cu8vstr( "Audp" ), &p_CmpRslt ),p_CmpRslt != 0 ) )
					{
						g_ErrInfoVstr.Cpy( Cu8vstr( "客户端的服务端Url字符串的协议不正确。" ) );
						LOGE( g_ErrInfoVstr.m_VstrPt );
						Toast( g_MainDlgWndHdl, 3000, NULL, g_ErrInfoVstr.m_VstrPt );
						goto ClntAddOut;
					}
					if( VstrCmp( p_PortVstrPt, Cu8vstr( "" ), &p_CmpRslt ), p_CmpRslt == 0 )
					{
						VstrCpy( p_PortVstrPt, Cu8vstr( "12345" ), ,  );
					}

					MainDlgWndMsgClntLstAddItem * p_MainDlgWndMsgClntLstAddItemPt = ( MainDlgWndMsgClntLstAddItem * )malloc( sizeof( MainDlgWndMsgClntLstAddItem ) );
					p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt = p_PrtclVstrPt;
					p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt = p_HostnameVstrPt;
					p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt = p_PortVstrPt;
					SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypClntLstAddItem, ( WPARAM )p_MainDlgWndMsgClntLstAddItemPt, 0 );

					ClntAddOut:
					VstrDstoy( p_VstrTmpPt );
					return ( INT_PTR )TRUE;
				}
				case ClntCnctBtnId: //客户端连接按钮。
				{
					int p_Num = ListView_GetSelectionMark( g_ClntLstWndHdl );
					int p_IsSrvrOrClntCnct;
					Vstr * p_RmtNodeNameVstrPt = NULL;
					Vstr * p_RmtNodeSrvcVstrPt = NULL;
					wchar_t p_TextTmpArr[ 1024 ];

					if( g_MyNtwkMediaPocsThrdPt == NULL ) //如果我的网络媒体处理线程还没有初始化。
					{
						if( MyNtwkMediaPocsThrdInit() != 0 ) //如果我的网络媒体处理线程初始化失败。
						{
							break;
						}
					}

					if( p_Num != -1 )
					{
						ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
						p_IsSrvrOrClntCnct = ( wcscmp( p_TextTmpArr, L"Tcp" ) == 0 ) ? 0 : 1;
						ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
						VstrInit( &p_RmtNodeNameVstrPt, Utf16, , Cu16vstr( p_TextTmpArr ) );
						ListView_GetItemText( g_ClntLstWndHdl, p_Num, 3, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
						VstrInit( &p_RmtNodeSrvcVstrPt, Utf16, , Cu16vstr( p_TextTmpArr ) );
						g_MyNtwkMediaPocsThrdPt->CnctInit( 1, p_IsSrvrOrClntCnct, p_RmtNodeNameVstrPt, p_RmtNodeSrvcVstrPt, NULL );
					}

					ClntCnctBtnOut:
					VstrDstoy( p_RmtNodeNameVstrPt );
					VstrDstoy( p_RmtNodeSrvcVstrPt );
					return ( INT_PTR )TRUE;
				}
				case ClntDelBtnId: //客户端删除按钮。
				{
					SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypClntLstDelItem, ListView_GetSelectionMark( g_ClntLstWndHdl ), 0 );
					return ( INT_PTR )TRUE;
				}
				case UseNoneTkbkModeRdBtnId: //使用挂起对讲模式单选按钮。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 1, NtwkMediaPocsThrd::TkbkModeNone, NULL );
					}
					break;
				}
				case UseAdoTkbkModeRdBtnId: //使用音频对讲模式单选按钮。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 1, NtwkMediaPocsThrd::TkbkModeAdo, NULL );
					}
					break;
				}
				case UseVdoTkbkModeRdBtnId: //使用视频对讲模式单选按钮。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 1, NtwkMediaPocsThrd::TkbkModeVdo, NULL );
					}
					break;
				}
				case UseAdoVdoTkbkModeRdBtnId: //使用音视频对讲模式单选按钮。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->LclTkbkMode( 1, NtwkMediaPocsThrd::TkbkModeAdoVdo, NULL );
					}
					break;
				}
				case UseAdoInptDvcCbBoxId: //使用音频输入设备组合框。
				{
					switch( HIWORD( wParam ) )
					{
						case CBN_SELCHANGE: //选择项目改变消息。
						{
							if( g_MyNtwkMediaPocsThrdPt != NULL )
							{
								//设置音频输入使用的设备。
								g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetUseDvc( 1, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
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
							if( g_MyNtwkMediaPocsThrdPt != NULL )
							{
								//设置音频输出使用的设备。
								g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetUseDvc( 1, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
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
							if( g_MyNtwkMediaPocsThrdPt != NULL )
							{
								//设置视频输入使用的设备。
								g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetUseDvc( 1, SendMessage( GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ), CB_GETCURSEL, 0, 0 ), &g_ErrInfoVstr );
							}
							break;
						}
					}
					break;
				}
				case AdoInptIsMuteCkBoxId: //音频输入静音复选框。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsMute( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case AdoOtptIsMuteCkBoxId: //音频输出静音复选框。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsMute( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoInptIsBlackCkBoxId: //视频输入黑屏复选框。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoInptSetIsBlack( 1, ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case VdoOtptIsBlackCkBoxId: //视频输出黑屏复选框。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->VdoOtptSetStrmIsBlack( 1, 0, ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0, &g_ErrInfoVstr );
					}
					break;
				}
				case IsDrawAdoWavfmToWndCkBoxId: //绘制音频波形到窗口复选框。
				{
					if( g_MyNtwkMediaPocsThrdPt != NULL )
					{
						//设置音频输入是否绘制音频波形到窗口。
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoInptSetIsDrawAdoWavfmToWnd( 1,
																									  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																									  GetDlgItem( g_MainDlgWndHdl, AdoInptSrcWavfmTxtId ),
																									  GetDlgItem( g_MainDlgWndHdl, AdoInptRsltWavfmTxtId ),
																									  &g_ErrInfoVstr );

						//设置音频输出是否绘制音频波形到窗口。
						g_MyNtwkMediaPocsThrdPt->m_MediaPocsThrdClsPt->AdoOtptSetIsDrawAdoWavfmToWnd( 1,
																									  ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0,
																									  GetDlgItem( g_MainDlgWndHdl, AdoOtptSrcWavfmTxtId ),
																									  &g_ErrInfoVstr );
					}
					break;
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
				case UseEffectLowRdBtnId: //效果等级：低。
				{
					EffectLow();
					return ( INT_PTR )TRUE;
				}
				case UseEffectMidRdBtnId: //效果等级：中。
				{
					EffectMid();
					return ( INT_PTR )TRUE;
				}
				case UseEffectHighRdBtnId: //效果等级：高。
				{
					EffectHigh();
					return ( INT_PTR )TRUE;
				}
				case UseEffectSuperRdBtnId: //效果等级：超。
				{
					EffectSuper();
					return ( INT_PTR )TRUE;
				}
				case UseEffectPremiumRdBtnId: //效果等级：特。
				{
					EffectPremium();
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
				case VdoInptPrvwTxtId: //视频输入预览文本框。
				case VdoOtptDspyTxtId: //视频输出显示文本框。
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
				case SpeexAecDelMemFileBtnId: //Speex声学回音消除器的删除内存块文件按钮。
				{
					if( DeleteFile( L".\\SpeexAecMem" ) != 0 )
					{
						MessageBox( hDlg, L"删除Speex声学回音消除器的内存块文件 SpeexAecMem 成功。", L"Windows下音视频对讲演示程序", MB_OK | MB_ICONINFORMATION );
					}
					else
					{
						uint16_t * p_TmpU16strPt;
						size_t p_TmpU16strLenChr;
						GetWinLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除Speex声学回音消除器的内存块文件 SpeexAecMem 失败。原因：" ) );
						AstrCpy( uint16_t *, p_TmpU16strPt, Utf16, 1, &p_TmpU16strLenChr, g_ErrInfoVstr.m_VstrPt->m_Pt, g_ErrInfoVstr.m_VstrPt->m_ChrSet, , );
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
						size_t p_TmpU16strLenChr;
						GetWinLastErrInfo( , g_ErrInfoVstr.m_VstrPt ), g_ErrInfoVstr.FmtIns( 0, Cu8vstr( "删除WebRtc浮点版声学回音消除器的内存块文件 WebRtcAecMem 失败。原因：" ) );
						AstrCpy( uint16_t *, p_TmpU16strPt, Utf16, 1, &p_TmpU16strLenChr, g_ErrInfoVstr.m_VstrPt->m_Pt, g_ErrInfoVstr.m_VstrPt->m_ChrSet, , );
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
						MyNtwkMediaPocsThrdDstoy();
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
		case MainDlgWndMsgTypMyNtwkMediaPocsThrdInit: //我的网络媒体处理线程初始化消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), FALSE ); //设置服务端设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), FALSE ); //设置客户端设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), FALSE ); //设置设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), FALSE ); //设置保存设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), FALSE ); //设置读取设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), FALSE ); //设置删除设置按钮为不可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), FALSE ); //设置重置设置按钮为不可用。
            if( g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode == 0 ) ShowWindow( g_PttDlgWndHdl, SW_SHOW ); //设置一键即按即通对话框为显示。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypMyNtwkMediaPocsThrdDstoy: //我的网络媒体处理线程销毁消息。
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrStngBtnId ), TRUE ); //设置服务端设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ClntStngBtnId ), TRUE ); //设置客户端设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, StngBtnId ), TRUE ); //设置设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SaveStngBtnId ), TRUE ); //设置保存设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ReadStngBtnId ), TRUE ); //设置读取设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, DelStngBtnId ), TRUE ); //设置删除设置按钮为可用。
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, ResetStngBtnId ), TRUE ); //设置重置设置按钮为可用。
			if( g_MyNtwkMediaPocsThrdPt->m_NtwkMediaPocsThrdPt->m_TkbkNtwk.m_XfrMode == 0 ) ShowWindow( g_PttDlgWndHdl, SW_HIDE ); //设置一键即按即通对话框为隐藏。

			if( g_MyNtwkMediaPocsThrdPt != NULL )
			{
				g_MyNtwkMediaPocsThrdPt->Dstoy( &g_ErrInfoVstr ); //销毁媒体处理线程。
				delete g_MyNtwkMediaPocsThrdPt;
				g_MyNtwkMediaPocsThrdPt = NULL;
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypSrvrInit:
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), FALSE ); //设置服务端Url组合框为不可用。
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), L"销毁" ); //设置服务端创建或销毁按钮的内容为“销毁”。
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypSrvrDstoy:
		{
			EnableWindow( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), TRUE ); //设置服务端Url组合框为可用。
			SetWindowText( GetDlgItem( g_MainDlgWndHdl, SrvrCreateOrDstoyBtnId ), L"创建" ); //设置服务端创建或销毁按钮的内容为“创建”。
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
		case MainDlgWndMsgTypCnctLstAddItem: //连接列表添加项目消息。
		{
			MainDlgWndMsgCnctLstAddItem * p_MainDlgWndMsgCnctLstAddItemPt = ( MainDlgWndMsgCnctLstAddItem * )wParam;
			LVITEM p_LvItem;

			if( p_MainDlgWndMsgCnctLstAddItemPt != NULL )
			{
				p_LvItem.mask = 0;
				p_LvItem.iItem = p_MainDlgWndMsgCnctLstAddItemPt->m_Num;
				p_LvItem.iSubItem = 0;
				ListView_InsertItem( g_CnctLstWndHdl, &p_LvItem ); // 如果成功插入，返回项目句柄

				ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstAddItemPt->m_Num, 1, ( p_MainDlgWndMsgCnctLstAddItemPt->m_IsSrvrOrClntCnct == 0 ) ? L"Tcp" : L"Audp" );
				ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstAddItemPt->m_Num, 2, ( LPWSTR )p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt );
				ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstAddItemPt->m_Num, 3, ( LPWSTR )p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeNameVstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstAddItemPt->m_RmtNodeSrvcVstrPt );
				free( p_MainDlgWndMsgCnctLstAddItemPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypCnctLstModifyItem: //连接列表修改项目消息。
		{
			MainDlgWndMsgCnctLstModifyItem * p_MainDlgWndMsgCnctLstModifyItemPt = ( MainDlgWndMsgCnctLstModifyItem * )wParam;
			LVITEM p_LvItem;

			if( p_MainDlgWndMsgCnctLstModifyItemPt != NULL )
			{
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 0, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt->m_Pt );
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 4, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt->m_Pt );
				if( p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt != NULL ) ListView_SetItemText( g_CnctLstWndHdl, p_MainDlgWndMsgCnctLstModifyItemPt->m_Num, 5, ( LPWSTR )p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt->m_Pt );
				
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_SignVstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_LclTkbkModeVstrPt );
				VstrDstoy( p_MainDlgWndMsgCnctLstModifyItemPt->m_RmtTkbkModeVstrPt );
				free( p_MainDlgWndMsgCnctLstModifyItemPt );
			}
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypCnctLstDelItem: //连接列表删除项目消息。
		{
			ListView_DeleteItem( g_CnctLstWndHdl, ( int )wParam );
			return ( INT_PTR )TRUE;
		}
		case MainDlgWndMsgTypClntLstAddItem: //客户端列表添加项目消息。
		{
			MainDlgWndMsgClntLstAddItem * p_MainDlgWndMsgClntLstAddItemPt = ( MainDlgWndMsgClntLstAddItem * )wParam;
			int p_ClntLstTotal = ListView_GetItemCount( g_ClntLstWndHdl );
			wchar_t p_TextTmpArr[ 1024 ];
			LVITEM p_LvItem;

			if( p_MainDlgWndMsgClntLstAddItemPt != NULL )
			{
				for( int p_Num = 0; p_Num < p_ClntLstTotal; p_Num++ )
				{
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
					if( wcscmp( p_TextTmpArr, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt->m_Pt ) != 0 ) continue;
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
					if( wcscmp( p_TextTmpArr, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt ) != 0 ) continue;
					ListView_GetItemText( g_ClntLstWndHdl, p_Num, 3, p_TextTmpArr, SzOfArr( p_TextTmpArr ) );
					if( wcscmp( p_TextTmpArr, ( wchar_t * )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt ) != 0 ) continue;
					Toast( g_MainDlgWndHdl, 3000, NULL, Cu8vstr( "已存在相同的客户端的服务端，无需重复添加。" ) );
					goto MainDlgWndMsgTypClntLstAddItemOut;
				}

				p_LvItem.mask = 0;
				p_LvItem.iItem = p_ClntLstTotal;
				p_LvItem.iSubItem = 0;
				ListView_InsertItem( g_ClntLstWndHdl, &p_LvItem ); // 如果成功插入，返回项目句柄

				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 1, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt->m_Pt );
				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 2, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt->m_Pt );
				ListView_SetItemText( g_ClntLstWndHdl, p_ClntLstTotal, 3, ( LPWSTR )p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt->m_Pt );
				
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
	_CrtSetBreakAlloc( 30 );
	#endif

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
			//连接列表添加字段。
			LVCOLUMN p_LvCol;
			p_LvCol.mask = LVCF_WIDTH;
			p_LvCol.cx = 18;
			ListView_InsertColumn( g_CnctLstWndHdl, 0, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 0, &p_LvCol );
			p_LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.pszText = L"协议";
			p_LvCol.cx = 38;
			ListView_InsertColumn( g_CnctLstWndHdl, 1, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 1, &p_LvCol );
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.pszText = L"名称";
			p_LvCol.cx = 143;
			ListView_InsertColumn( g_CnctLstWndHdl, 2, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 2, &p_LvCol );
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.pszText = L"服务";
			p_LvCol.cx = 42;
			ListView_InsertColumn( g_CnctLstWndHdl, 3, &p_LvCol );
			ListView_InsertColumn( g_ClntLstWndHdl, 3, &p_LvCol );
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.pszText = L"本端对讲模式";
			p_LvCol.cx = 99;
			ListView_InsertColumn( g_CnctLstWndHdl, 4, &p_LvCol );
			//ListView_InsertColumn( g_ClntLstWndHdl, 4, &p_LvCol );
			p_LvCol.fmt = LVCFMT_CENTER;
			p_LvCol.pszText = L"远端对讲模式";
			p_LvCol.cx = 99;
			ListView_InsertColumn( g_CnctLstWndHdl, 5, &p_LvCol );
			//ListView_InsertColumn( g_ClntLstWndHdl, 5, &p_LvCol );
			
			//连接列表设置样式。
			ListView_SetExtendedListViewStyleEx( g_CnctLstWndHdl, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
			ListView_SetExtendedListViewStyleEx( g_ClntLstWndHdl, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
		}
		g_PtbBtnWndHdl = GetDlgItem( g_MainDlgWndHdl, PtbBtnId ); //设置一键即按即广播按钮窗口的句柄。
		g_AdoInptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoInptDvcCbBoxId ); //设置音频输入设备组合框窗口的句柄。
		g_AdoOtptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseAdoOtptDvcCbBoxId ); //设置音频输出设备组合框窗口的句柄。
		g_VdoInptDvcCbBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, UseVdoInptDvcCbBoxId ); //设置视频输入设备组合框窗口的句柄。
		g_LogLstBoxWndHdl = GetDlgItem( g_MainDlgWndHdl, LogLstBoxId ); //设置日志列表框窗口的句柄。
		g_VdoInptPrvwTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoInptPrvwTxtId ); //设置视频输入预览文本框窗口的句柄。
		g_VdoOtptDspyTxtWndHdl = GetDlgItem( g_MainDlgWndHdl, VdoOtptDspyTxtId ); //设置视频输出显示文本框窗口的句柄。
		g_VdoInptPrvwWndHdl = g_VdoInptPrvwTxtWndHdl;
		g_VdoOtptDspyWndHdl = g_VdoOtptDspyTxtWndHdl;
		g_PttDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( PttDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_PttBtnWndHdl = GetDlgItem( g_PttDlgWndHdl, PttBtnId );

		g_StngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( StngDlgId ), g_MainDlgWndHdl, WndMsgPocsPocdr );
		g_AjbStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( AjbStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SaveStsToTxtFileStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SaveStsToTxtFileStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SaveAdoVdoInptOtptToAviFileStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_SpeexAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( SpeexAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecmStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecmStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
		g_WebRtcAecStngDlgWndHdl = CreateDialog( g_IstnsHdl, MAKEINTRESOURCE( WebRtcAecStngDlgId ), g_StngDlgWndHdl, WndMsgPocsPocdr );
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
			if( wcscmp( p_Argv[ p_Argn ], L"-SrvrUrl" ) == 0 )
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
			else if( wcscmp( p_Argv[ p_Argn ], L"-None" ) == 0 )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseNoneTkbkModeRdBtnId );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-Ado" ) == 0 )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseAdoTkbkModeRdBtnId );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-Vdo" ) == 0 )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseVdoTkbkModeRdBtnId );
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-AdoVdo" ) == 0 )
			{
				CheckRadioButton( g_MainDlgWndHdl, UseNoneTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId, UseAdoVdoTkbkModeRdBtnId );
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
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoInptPrvwWndHdl" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &g_VdoInptPrvwWndHdl, sizeof( g_VdoInptPrvwWndHdl ) );
				}
			}
			else if( wcscmp( p_Argv[ p_Argn ], L"-VdoOtptDspyWndHdl" ) == 0 )
			{
				if( p_Argn + 1 < p_Argc )
				{
					p_Argn++;
					StrToInt( p_Argv[ p_Argn ], Utf16, , , , , &g_VdoOtptDspyWndHdl, sizeof( g_VdoOtptDspyWndHdl ) );
				}
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
					SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntCnctBtnId ), BM_CLICK, 0, 0 );
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
				g_MyNtwkMediaPocsThrdPt->PttBtnDown( 1, NULL );
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				g_MyNtwkMediaPocsThrdPt->PttBtnUp( 1, NULL );
			}
		}
		else if( p_Msg.hwnd == g_PtbBtnWndHdl )
		{
			if( p_Msg.message == WM_LBUTTONDOWN )
			{
				BdctInit();
			}
			else if( p_Msg.message == WM_LBUTTONUP )
			{
				BdctDstoy();
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
