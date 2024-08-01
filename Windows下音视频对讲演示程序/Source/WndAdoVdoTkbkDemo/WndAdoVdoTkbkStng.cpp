#include "Func.h"
#include "TinyXml2.h"
#include "WndAdoVdoTkbkDemo.h"
#include "WndAdoVdoTkbkStng.h"

void RefresAdoVdohDvc(); //刷新音视频设备。

//保存设置到Xml文件。
void SaveStngToXmlFile()
{
	tinyxml2::XMLDocument p_XMLDocument;
	tinyxml2::XMLElement * p_StngXMLElementPt;
	tinyxml2::XMLElement * p_TmpXMLElement1Pt;
	tinyxml2::XMLElement * p_TmpXMLElement2Pt;
	tinyxml2::XMLElement * p_TmpXMLElement3Pt;
	tinyxml2::XMLError p_XMLError;
	Vstr * p_U16TxtVstrPt = NULL;
	Vstr * p_U8TxtVstrPt = NULL;

	if( VstrInit( &p_U16TxtVstrPt, Utf16, 1024, ) != 0 )
	{
		LOGE( Cu8vstr( "初始化文本动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_U8TxtVstrPt, Utf8, 1024, ) != 0 )
	{
		LOGE( Cu8vstr( "初始化文本动态字符串失败。" ) );
		goto Out;
	}

	p_StngXMLElementPt = p_XMLDocument.NewElement( "Stng" );
	p_XMLDocument.InsertEndChild( p_StngXMLElementPt );

	//主对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "Main" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_MainDlgWndHdl, SrvrUrlCbBoxId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SrvrUrl" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		for( int p_Num = 0, p_ClntLstTotal = ListView_GetItemCount( g_ClntLstWndHdl ); p_Num < p_ClntLstTotal; p_Num++ )
		{
			p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "ClntLstItem" );
			p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

			ListView_GetItemText( g_ClntLstWndHdl, p_Num, 0, ( LPWSTR )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
			VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, ,  );
			p_TmpXMLElement3Pt = p_XMLDocument.NewElement( "Prtcl" );
			p_TmpXMLElement3Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
			p_TmpXMLElement2Pt->InsertEndChild( p_TmpXMLElement3Pt );
				
			ListView_GetItemText( g_ClntLstWndHdl, p_Num, 1, ( LPWSTR )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
			VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, ,  );
			p_TmpXMLElement3Pt = p_XMLDocument.NewElement( "RmtNodeName" );
			p_TmpXMLElement3Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
			p_TmpXMLElement2Pt->InsertEndChild( p_TmpXMLElement3Pt );
				
			ListView_GetItemText( g_ClntLstWndHdl, p_Num, 2, ( LPWSTR )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
			VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, ,  );
			p_TmpXMLElement3Pt = p_XMLDocument.NewElement( "RmtNodeSrvc" );
			p_TmpXMLElement3Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
			p_TmpXMLElement2Pt->InsertEndChild( p_TmpXMLElement3Pt );
		}

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseAdoInptTkbkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseAdoOtptTkbkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseVdoInptTkbkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseVdoOtptTkbkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
		
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoInptIsMute" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoOtptIsMute" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoInptIsBlack" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoOtptIsBlack" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		{
			int p_TmpInt;

			p_TmpInt = SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_U16TxtVstrPt->m_Pt );
				VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
				p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoInptDvc" );
				p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
				p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
			}
			p_TmpInt = SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_U16TxtVstrPt->m_Pt );
				VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
				p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoOtptDvc" );
				p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
				p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
			}
			p_TmpInt = SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_GETCURSEL, 0, 0 );
			if( p_TmpInt != CB_ERR )
			{
				SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_GETLBTEXT, p_TmpInt, ( LPARAM )p_U16TxtVstrPt->m_Pt );
				VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
				p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoInptDvc" );
				p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
				p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
			}
		}

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsDrawAdoWavfm" ); //这里不加"ToWnd"是为了与Android端的"ToSurface"保持一致。
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//服务端设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SrvrStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );
		
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsAutoActCnct" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SrvrStngDlgWndHdl, IsAutoActCnctCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
		
		GetDlgItemText( g_SrvrStngDlgWndHdl, MaxCnctNumEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "MaxCnctNum" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}
	
	//客户端设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "ClntStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );
		
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "XfrMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_ClntStngDlgWndHdl, UsePttRdBtnId ) == BST_CHECKED ) ? "Ptt" : "Rt" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
		
		GetDlgItemText( g_ClntStngDlgWndHdl, MaxCnctTimesEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "MaxCnctTimes" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
		
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsReferRmtTkbkModeSetTkbkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_ClntStngDlgWndHdl, IsReferRmtTkbkModeSetTkbkModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "Stng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseWhatRecvOtptFrm" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId ) == BST_CHECKED ) ? "Cntnr" : "Ajb" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveStsToTxtFile" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsPrintLogShowToast" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUsePrvntSysSleep" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
		
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsTstNtwkDly" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoVdoInptOtptToAviFile" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "Effect" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseEffectLowRdBtnId ) == BST_CHECKED ) ? "Low" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseEffectMidRdBtnId ) == BST_CHECKED ) ? "Mid" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseEffectHighRdBtnId ) == BST_CHECKED ) ? "High" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseEffectSuperRdBtnId ) == BST_CHECKED ) ? "Super" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseEffectPremiumRdBtnId ) == BST_CHECKED ) ? "Premium" : "Super" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoSmplRate" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId ) == BST_CHECKED ) ? 8000 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate16000RdBtnId ) == BST_CHECKED ) ? 16000 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate32000RdBtnId ) == BST_CHECKED ) ? 32000 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoSmplRate48000RdBtnId ) == BST_CHECKED ) ? 48000 : 16000 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoFrmLen" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId ) == BST_CHECKED ) ? 10 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen20msRdBtnId ) == BST_CHECKED ) ? 20 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseAdoFrmLen30msRdBtnId ) == BST_CHECKED ) ? 30 : 20 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseSystemAecNsAgc" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseWhatAec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoAecRdBtnId ) == BST_CHECKED ) ? "NoAec" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexAecRdBtnId ) == BST_CHECKED ) ? "SpeexAec" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecmRdBtnId ) == BST_CHECKED ) ? "WebRtcAecm" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcAecRdBtnId ) == BST_CHECKED ) ? "WebRtcAec" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexWebRtcAecRdBtnId ) == BST_CHECKED ) ? "SpeexWebRtcAec" : "SpeexWebRtcAec" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseWhatNs" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseNoNsRdBtnId ) == BST_CHECKED ) ? "NoNs" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexPrpocsNsRdBtnId ) == BST_CHECKED ) ? "SpeexPrpocsNs" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsxRdBtnId ) == BST_CHECKED ) ? "WebRtcNsx" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseWebRtcNsRdBtnId ) == BST_CHECKED ) ? "WebRtcNs" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseRNNoiseRdBtnId ) == BST_CHECKED ) ? "RNNoise" : "RNNoise" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseSpeexPrpocs" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoUseWhatCodec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UsePcmRdBtnId ) == BST_CHECKED ) ? "Pcm" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseSpeexCodecRdBtnId ) == BST_CHECKED ) ? "SpeexCodec" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseOpusCodecRdBtnId ) == BST_CHECKED ) ? "OpusCodec" : "SpeexCodec" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoInptOtptToWaveFile" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoSmplRate" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId ) == BST_CHECKED ) ? 12 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate15RdBtnId ) == BST_CHECKED ) ? 15 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate24RdBtnId ) == BST_CHECKED ) ? 24 :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoSmplRate30RdBtnId ) == BST_CHECKED ) ? 30 : 24 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "UseWhatVdoFrmSz" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId ) == BST_CHECKED ) ? "Prset" :
									 ( IsDlgButtonChecked( g_StngDlgWndHdl, UseVdoFrmSzOtherRdBtnId ) == BST_CHECKED ) ? "Other" : "Prset" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoFrmSzPrset" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoFrmSzOtherWidth" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoFrmSzOtherHeight" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VdoUseWhatCodec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_StngDlgWndHdl, UseYu12RdBtnId ) == BST_CHECKED ) ? "Yu12" :
									 ( IsDlgButtonChecked( g_MainDlgWndHdl, UseOpenH264CodecRdBtnId ) == BST_CHECKED ) ? "OpenH264Codec" : "OpenH264Codec" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//自适应抖动缓冲器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "AjbStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AAjbMinNeedBufFrmCnt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AAjbMaxNeedBufFrmCnt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AAjbMaxCntuLostFrmCnt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AAjbAdaptSensitivity" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VAjbMinNeedBufFrmCnt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VAjbMaxNeedBufFrmCnt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VAjbAdaptSensitivity" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//保存状态到Txt文件设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SaveStsToTxtFileStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "FullPath" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//保存音视频输入输出到Avi文件设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SaveAdoVdoInptOtptToAviFileStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "FullPath" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileWrBufSzBytEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WrBufSzByt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoInpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoOtpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveVdoInpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveVdoOtpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//Speex声学回音消除器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SpeexAecStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "FilterLenMsec" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseRec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoMutp" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoCntu" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoSupes" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoSupesAct" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveMemFile" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//WebRtc定点版声学回音消除器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "WebRtcAecmStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseCNGMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "Delay" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//WebRtc浮点版声学回音消除器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "WebRtcAecStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "EchoMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "Delay" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseDelayAgstcMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseExtdFilterMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseRefinedFilterAdaptAecMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseAdaptAdjDelay" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveMemFile" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//SpeexWebRtc三重声学回音消除器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SpeexWebRtcAecStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WorkMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId ) == BST_CHECKED ) ? "SpeexAecWebRtcAecm" :
									 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? "WebRtcAecmWebRtcAec" :
									 ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId ) == BST_CHECKED ) ? "SpeexAecWebRtcAecmWebRtcAec" : "SpeexAecWebRtcAecmWebRtcAec" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecFilterLenMsec" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecIsUseRec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecEchoMutp" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecEchoCntu" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecEchoSupes" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexAecEchoSupesAct" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecmIsUseCNGMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecmEchoMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecmDelay" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecEchoMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecDelay" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecIsUseDelayAgstcMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecIsUseExtdFilterMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecIsUseRefinedFilterAdaptAecMode" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WebRtcAecIsUseAdaptAdjDelay" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseSameRoomAec" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SameRoomEchoMinDelay" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//Speex预处理器的噪音抑制设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SpeexPrpocsNsStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseNs" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "NoiseSupes" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseDereverb" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//WebRtc定点版噪音抑制器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "WebRtcNsxStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );
	
		GetDlgItemText( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "PolicyMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//WebRtc浮点版噪音抑制器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "WebRtcNsStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );
	
		GetDlgItemText( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "PolicyMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//Speex预处理器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SpeexPrpocsStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseVad" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VadProbStart" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "VadProbCntu" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsUseAgc" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AgcLevel" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AgcIncrement" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AgcDecrement" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AgcMaxGain" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//Speex编解码器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SpeexCodecStng" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexEncdUseCbrOrVbr" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId ) == BST_CHECKED ) ? "Cbr" : "Vbr" );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexEncdQualt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexEncdCmplxt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexEncdPlcExptLossRate" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "SpeexDecdIsUsePrcplEnhsmt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//保存音频到Wave文件设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "SaveAdoInptOtptToWaveFile" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoInpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoInptSrcFullPath" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoInptRsltFullPath" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "IsSaveAdoOtpt" );
		p_TmpXMLElement2Pt->SetText( ( IsDlgButtonChecked( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId ) == BST_CHECKED ) ? 1 : 0 );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "AdoOtptSrcFullPath" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "WrBufSzByt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	//OpenH264编解码器设置对话框。
	{
		p_TmpXMLElement1Pt = p_XMLDocument.NewElement( "OpenH264Codec" );
		p_StngXMLElementPt->InsertEndChild( p_TmpXMLElement1Pt );

		GetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "OpenH264EncdVdoType" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "OpenH264EncdEncdBitrate" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "OpenH264EncdBitrateCtrlMode" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "OpenH264EncdIDRFrmIntvl" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );

		GetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId, ( wchar_t * )p_U16TxtVstrPt->m_Pt, p_U16TxtVstrPt->m_SzChr );
		VstrCpy( p_U8TxtVstrPt, p_U16TxtVstrPt, , );
		p_TmpXMLElement2Pt = p_XMLDocument.NewElement( "OpenH264EncdCmplxt" );
		p_TmpXMLElement2Pt->SetText( ( const char * )p_U8TxtVstrPt->m_Pt );
		p_TmpXMLElement1Pt->InsertEndChild( p_TmpXMLElement2Pt );
	}

	p_XMLError = p_XMLDocument.SaveFile( "Stng.xml" );
	if( p_XMLError == tinyxml2::XML_SUCCESS )
	{
		VstrCpy( p_U16TxtVstrPt, Cu16vstr( L"保存设置到Stng.xml文件成功。" ), , );
		LOGI( p_U16TxtVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_U16TxtVstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}
	else
	{
		VstrFmtCpy( p_U16TxtVstrPt, Cu16vstr( L"保存设置到Stng.xml文件失败。原因：%s。" ), p_XMLDocument.ErrorIDToName( p_XMLError ) );
		LOGE( p_U16TxtVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_U16TxtVstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}
	
	Out:
	if( p_U16TxtVstrPt != NULL )
	{
		VstrDstoy( p_U16TxtVstrPt );
	}
	if( p_U8TxtVstrPt != NULL )
	{
		VstrDstoy( p_U8TxtVstrPt );
	}
	return;
}

//从Xml文件读取设置。
void ReadStngFromXmlFile()
{
	tinyxml2::XMLDocument p_XMLDocument;
	tinyxml2::XMLElement * p_StngXMLElementPt;
	tinyxml2::XMLElement * p_TmpXMLElement1Pt;
	tinyxml2::XMLElement * p_TmpXMLElement2Pt;
	tinyxml2::XMLElement * p_TmpXMLElement3Pt;
	tinyxml2::XMLError p_XMLError;
	Vstr * p_U16TxtVstrPt = NULL;
	Vstr * p_U8TxtVstrPt = NULL;

	if( VstrInit( &p_U16TxtVstrPt, Utf16, 1024, ) != 0 )
	{
		LOGE( Cu8vstr( "初始化文本动态字符串失败。" ) );
		goto Out;
	}
	if( VstrInit( &p_U8TxtVstrPt, Utf8, 1024, ) != 0 )
	{
		LOGE( Cu8vstr( "初始化文本动态字符串失败。" ) );
		goto Out;
	}

	p_XMLError = p_XMLDocument.LoadFile( "Stng.xml" );
	if( p_XMLError == tinyxml2::XML_SUCCESS )
	{
		VstrCpy( p_U16TxtVstrPt, Cu16vstr( L"从Stng.xml文件读取设置成功。" ), , );
		LOGI( p_U16TxtVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_U16TxtVstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}
	else
	{
		VstrFmtCpy( p_U16TxtVstrPt, Cu16vstr( L"从Stng.xml文件读取设置失败。原因：%s。" ), p_XMLDocument.ErrorIDToName( p_XMLError ) );
		LOGE( p_U16TxtVstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , p_U16TxtVstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
		goto Out;
	}
	
	for( p_StngXMLElementPt = p_XMLDocument.FirstChildElement(); p_StngXMLElementPt != NULL; p_StngXMLElementPt = p_StngXMLElementPt->NextSiblingElement() )
	{
		if( strcmp( p_StngXMLElementPt->Name(), "Stng" ) == 0 )
		{
			for( p_TmpXMLElement1Pt = p_StngXMLElementPt->FirstChildElement(); p_TmpXMLElement1Pt != NULL; p_TmpXMLElement1Pt = p_TmpXMLElement1Pt->NextSiblingElement() )
			{
				if( strcmp( p_TmpXMLElement1Pt->Name(), "Main" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "SrvrUrl" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_MainDlgWndHdl, SrvrUrlCbBoxId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "ClntLstItem" ) == 0 )
						{
							MainDlgWndMsgClntLstAddItem * p_MainDlgWndMsgClntLstAddItemPt = ( MainDlgWndMsgClntLstAddItem * )malloc( sizeof( MainDlgWndMsgClntLstAddItem ) );
							VstrInit( &p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt, Utf16, ,  );
							VstrInit( &p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt, Utf16, ,  );
							VstrInit( &p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt, Utf16, ,  );
							for( p_TmpXMLElement3Pt = p_TmpXMLElement2Pt->FirstChildElement(); p_TmpXMLElement3Pt != NULL; p_TmpXMLElement3Pt = p_TmpXMLElement3Pt->NextSiblingElement() )
							{
								if( strcmp( p_TmpXMLElement3Pt->Name(), "Prtcl" ) == 0 )
								{
									VstrCpy( p_MainDlgWndMsgClntLstAddItemPt->m_PrtclVstrPt, Cu8vstr( p_TmpXMLElement3Pt->GetText() ), , );
								}
								else if( strcmp( p_TmpXMLElement3Pt->Name(), "RmtNodeName" ) == 0 )
								{
									VstrCpy( p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeNameVstrPt, Cu8vstr( p_TmpXMLElement3Pt->GetText() ), , );
								}
								else if( strcmp( p_TmpXMLElement3Pt->Name(), "RmtNodeSrvc" ) == 0 )
								{
									VstrCpy( p_MainDlgWndMsgClntLstAddItemPt->m_RmtNodeSrvcVstrPt, Cu8vstr( p_TmpXMLElement3Pt->GetText() ), , );
								}
							}
							SendMessage( g_MainDlgWndHdl, MainDlgWndMsgTypClntLstAddItem, ( WPARAM )p_MainDlgWndMsgClntLstAddItemPt, 0 );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseAdoInptTkbkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseAdoOtptTkbkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseVdoInptTkbkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseVdoOtptTkbkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoInptIsMute" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoOtptIsMute" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoInptIsBlack" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoOtptIsBlack" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoInptDvc" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SendMessage( g_AdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoOtptDvc" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SendMessage( g_AdoOtptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoInptDvc" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SendMessage( g_VdoInptDvcCbBoxWndHdl, CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsDrawAdoWavfm" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SrvrStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "IsAutoActCnct" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SrvrStngDlgWndHdl, IsAutoActCnctCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SrvrStngDlgWndHdl, IsAutoActCnctCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "MaxCnctNum" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SrvrStngDlgWndHdl, MaxCnctNumEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "ClntStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "XfrMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Ptt" ) == 0 )
							{
								CheckRadioButton( g_ClntStngDlgWndHdl, UsePttRdBtnId, UseRtFdRdBtnId, UsePttRdBtnId );
							}
							else
							{
								CheckRadioButton( g_ClntStngDlgWndHdl, UsePttRdBtnId, UseRtFdRdBtnId, UseRtFdRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "MaxCnctTimes" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_ClntStngDlgWndHdl, MaxCnctTimesEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsReferRmtTkbkModeSetTkbkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_ClntStngDlgWndHdl, IsReferRmtTkbkModeSetTkbkModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_ClntStngDlgWndHdl, IsReferRmtTkbkModeSetTkbkModeCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "Stng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "UseWhatRecvOtptFrm" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Cntnr" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId, UseCntnrRecvOtptFrmRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveStsToTxtFile" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsPrintLogShowToast" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUsePrvntSysSleep" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsTstNtwkDly" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoVdoInptOtptToAviFile" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "Effect" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Low" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectLowRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "Mid" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectMidRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "High" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectHighRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "Super" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectSuperRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectPremiumRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoSmplRate" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "8000" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate8000RdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "16000" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "32000" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate32000RdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate48000RdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoFrmLen" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "10" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen10msRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "20" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen30msRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseSystemAecNsAgc" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseWhatAec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "NoAec" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseNoAecRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "SpeexAec" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexAecRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "WebRtcAecm" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecmRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "WebRtcAec" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseWhatNs" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "NoNs" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseNoNsRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "SpeexPrpocsNs" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseSpeexPrpocsNsRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "WebRtcNsx" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsxRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "WebRtcNs" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseSpeexPrpocs" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoUseWhatCodec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Pcm" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UsePcmRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "SpeexCodec" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseOpusCodecRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoInptOtptToWaveFile" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoSmplRate" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "12" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate12RdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "15" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "24" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate24RdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate30RdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "UseWhatVdoFrmSz" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Prset" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzOtherRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoFrmSzPrset" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "120×160" ) == 0 )
							{
								SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"120×160" );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "240×320" ) == 0 )
							{
								SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"240×320" );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "480×640" ) == 0 )
							{
								SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"480×640" );
							}
							else
							{
								SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"960×1280" );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoFrmSzOtherWidth" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoFrmSzOtherHeight" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VdoUseWhatCodec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Yu12" ) == 0 )
							{
								CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseYu12RdBtnId );
							}
							else
							{
								CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "AjbStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "AAjbMinNeedBufFrmCnt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AAjbMaxNeedBufFrmCnt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AAjbMaxCntuLostFrmCnt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AAjbAdaptSensitivity" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VAjbMinNeedBufFrmCnt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VAjbMaxNeedBufFrmCnt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VAjbAdaptSensitivity" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SaveStsToTxtFileStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "FullPath" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SaveAdoVdoInptOtptToAviFileStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "FullPath" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WrBufSzByt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileWrBufSzBytEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoInpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoOtpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveVdoInpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveVdoOtpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SpeexAecStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "FilterLenMsec" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecFilterLenMsecEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseRec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsUseRecCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoMutp" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoMutpEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoCntu" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoCntuEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoSupes" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoSupesAct" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexAecStngDlgWndHdl, SpeexAecEchoSupesActEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveMemFile" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexAecStngDlgWndHdl, SpeexAecIsSaveMemFileCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "WebRtcAecmStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseCNGMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmIsUseCNGModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmEchoModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "Delay" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcAecmStngDlgWndHdl, WebRtcAecmDelayEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "WebRtcAecStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "EchoMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcAecStngDlgWndHdl, WebRtcAecEchoModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "Delay" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcAecStngDlgWndHdl, WebRtcAecDelayEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseDelayAgstcMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseExtdFilterMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseRefinedFilterAdaptAecMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseAdaptAdjDelay" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveMemFile" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_WebRtcAecStngDlgWndHdl, WebRtcAecIsSaveMemFileCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SpeexWebRtcAecStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "WorkMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "SpeexAecWebRtcAecm" ) == 0 )
							{
								CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId );
							}
							else if( strcmp( p_TmpXMLElement2Pt->GetText(), "WebRtcAecmWebRtcAec" ) == 0 )
							{
								CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeWebRtcAecmWebRtcAecRdBtnId );
							}
							else
							{
								CheckRadioButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId, SpeexWebRtcAecWorkModeSpeexAecWebRtcAecmWebRtcAecRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecFilterLenMsec" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecFilterLenMsecEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecIsUseRec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecIsUseRecCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecEchoMutp" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoMutpEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecEchoCntu" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoCntuEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecEchoSupes" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexAecEchoSupesAct" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSpeexAecEchoSupesActEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecmIsUseCNGMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmIsUseCNGModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecmEchoMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmEchoModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecmDelay" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecmDelayEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecEchoMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecEchoModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecDelay" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecDelayEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecIsUseDelayAgstcMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseDelayAgstcModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecIsUseExtdFilterMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseExtdFilterModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecIsUseRefinedFilterAdaptAecMode" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseRefinedFilterAdaptAecModeCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WebRtcAecIsUseAdaptAdjDelay" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecWebRtcAecIsUseAdaptAdjDelayCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseSameRoomAec" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecIsUseSameRoomAecCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SameRoomEchoMinDelay" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexWebRtcAecStngDlgWndHdl, SpeexWebRtcAecSameRoomEchoMinDelayEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SpeexPrpocsNsStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseNs" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseNsCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "NoiseSupes" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsNoiseSupesEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseDereverb" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexPrpocsNsStngDlgWndHdl, SpeexPrpocsIsUseDereverbCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "WebRtcNsxStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "PolicyMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcNsxStngDlgWndHdl, WebRtcNsxPolicyModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "WebRtcNsStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "PolicyMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_WebRtcNsStngDlgWndHdl, WebRtcNsPolicyModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SpeexPrpocsStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseVad" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseVadCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VadProbStart" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbStartEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "VadProbCntu" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsVadProbCntuEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsUseAgc" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsIsUseAgcCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AgcLevel" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcLevelEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AgcIncrement" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcIncrementEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AgcDecrement" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcDecrementEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AgcMaxGain" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexPrpocsStngDlgWndHdl, SpeexPrpocsAgcMaxGainEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SpeexCodecStng" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexEncdUseCbrOrVbr" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "Cbr" ) == 0 )
							{
								CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseCbrRdBtnId );
							}
							else
							{
								CheckRadioButton( g_SpeexCodecStngDlgWndHdl, SpeexEncdUseCbrRdBtnId, SpeexEncdUseVbrRdBtnId, SpeexEncdUseVbrRdBtnId );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexEncdQualt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexEncdCmplxt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexEncdPlcExptLossRate" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "SpeexDecdIsUsePrcplEnhsmt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );
							}
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "SaveAdoInptOtptToWaveFile" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoInpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoInptSrcFullPath" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoInptRsltFullPath" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "IsSaveAdoOtpt" ) == 0 )
						{
							if( strcmp( p_TmpXMLElement2Pt->GetText(), "0" ) == 0 )
							{
								CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId, BST_UNCHECKED );
							}
							else
							{
								CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId, BST_CHECKED );
							}
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "AdoOtptSrcFullPath" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "WrBufSzByt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
				else if( strcmp( p_TmpXMLElement1Pt->Name(), "OpenH264Codec" ) == 0 )
				{
					for( p_TmpXMLElement2Pt = p_TmpXMLElement1Pt->FirstChildElement(); p_TmpXMLElement2Pt != NULL; p_TmpXMLElement2Pt = p_TmpXMLElement2Pt->NextSiblingElement() )
					{
						if( strcmp( p_TmpXMLElement2Pt->Name(), "OpenH264EncdVdoType" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "OpenH264EncdEncdBitrate" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "OpenH264EncdEncdBitrateCtrlMode" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "OpenH264EncdIDRFrmIntvl" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
						else if( strcmp( p_TmpXMLElement2Pt->Name(), "OpenH264EncdCmplxt" ) == 0 )
						{
							VstrCpy( p_U16TxtVstrPt, Cu8vstr( p_TmpXMLElement2Pt->GetText() ), , );
							SetDlgItemText( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId, ( LPCWSTR )p_U16TxtVstrPt->m_Pt );
						}
					}
				}
			}
		}
	}

	Out:
	if( p_U16TxtVstrPt != NULL )
	{
		VstrDstoy( p_U16TxtVstrPt );
	}
	if( p_U8TxtVstrPt != NULL )
	{
		VstrDstoy( p_U8TxtVstrPt );
	}
}

//删除设置Xml文件。
void DelStngXmlFile()
{
	if( DeleteFile( L"Stng.xml" ) != 0 )
	{
		g_ErrInfoVstr.Cpy( Cu8vstr( "删除设置文件Stng.xml成功。" ) );
		LOGI( g_ErrInfoVstr.m_VstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}
	else
	{
		GetWinSysErrInfo( GetLastError(), g_ErrInfoVstr.m_VstrPt );
		g_ErrInfoVstr.Ins( 0, Cu8vstr( "删除设置文件Stng.xml失败。原因：" ) );
		LOGE( g_ErrInfoVstr.m_VstrPt );
		{ Vstr * p_ErrInfoVstrPt = NULL; VstrInit( &p_ErrInfoVstrPt, Utf16, , g_ErrInfoVstr.m_VstrPt ); PostMessage( g_MainDlgWndHdl, MainDlgWndMsgTypShowLog, ( WPARAM )p_ErrInfoVstrPt, 0 ); }
	}
}

//重置设置。
void ResetStng()
{
	//设置服务端。
	CheckDlgButton( g_SrvrStngDlgWndHdl, IsAutoActCnctCkBoxId, BST_CHECKED );
	SetWindowText( GetDlgItem( g_SrvrStngDlgWndHdl, MaxCnctNumEdTxtId ), L"10" );

	//设置传输协议。
	CheckRadioButton( g_ClntStngDlgWndHdl, UsePttRdBtnId, UseRtFdRdBtnId, UseRtFdRdBtnId );
	SetWindowText( GetDlgItem( g_ClntStngDlgWndHdl, MaxCnctTimesEdTxtId ), L"5" );
	CheckDlgButton( g_ClntStngDlgWndHdl, IsReferRmtTkbkModeSetTkbkModeCkBoxId, BST_CHECKED );
	
	//设置服务端和客户端的Url。
	{
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_RESETCONTENT, ( WPARAM )0, ( LPARAM )0 ); //清空服务端Url组合框。
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_RESETCONTENT, ( WPARAM )0, ( LPARAM )0 ); //清空客户端的服务端Url组合框。

		struct addrinfo p_LclNodeHintsAddrInfo; //存放本地节点条件地址信息结构体。
		struct addrinfo * p_LclNodeAddrInfoListPt = NULL; //存放本地节点地址信息结构体链表指针。
		wchar_t p_LclNodeAddrStrArr[ INET6_ADDRSTRLEN ]; //存放本地节点地址字符串的数组。
		VstrCls p_LclNodeUrlVstr; //存放本地节点Url动态字符串。

		//本地节点Url动态字符串初始化。
		p_LclNodeUrlVstr.Init( Utf16 );

		//初始化本地节点条件地址信息结构体。
		p_LclNodeHintsAddrInfo.ai_canonname = NULL;
		p_LclNodeHintsAddrInfo.ai_next = NULL;
		p_LclNodeHintsAddrInfo.ai_addr = NULL;
		p_LclNodeHintsAddrInfo.ai_addrlen = 0;
		p_LclNodeHintsAddrInfo.ai_flags = AI_PASSIVE; //获取用于调用bind()函数的地址信息。
		p_LclNodeHintsAddrInfo.ai_family = 0;
		p_LclNodeHintsAddrInfo.ai_protocol = 0;
		p_LclNodeHintsAddrInfo.ai_socktype = 0;

		//根据指定的本地节点名称、本地节点服务、本地节点条件地址信息，获取本地节点符合条件的地址信息。
		getaddrinfo( "", "", &p_LclNodeHintsAddrInfo, &p_LclNodeAddrInfoListPt );

		//设置服务端Url组合框和客户端Url组合框的内容。
		for( struct addrinfo * p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListPt; p_LclNodeAddrInfoListLoopPt != NULL; p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListLoopPt->ai_next )
		{
			if( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_family == AF_INET ) //如果为IPv4的地址。
			{
				InetNtop( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_family, &( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_addr ), p_LclNodeAddrStrArr, SzOfArr( p_LclNodeAddrStrArr ) );
				p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://%z16s:12345" ), p_LclNodeAddrStrArr );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://%z16s:12345" ), p_LclNodeAddrStrArr );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
			}
		}
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://0.0.0.0:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://0.0.0.0:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://127.0.0.1:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://127.0.0.1:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		for( struct addrinfo * p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListPt; p_LclNodeAddrInfoListLoopPt != NULL; p_LclNodeAddrInfoListLoopPt = p_LclNodeAddrInfoListLoopPt->ai_next )
		{
			if( ( ( struct sockaddr_in * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin_family == AF_INET6 ) //如果为IPv6的地址。
			{
				InetNtop( ( ( struct sockaddr_in6 * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin6_family, &( ( ( struct sockaddr_in6 * )p_LclNodeAddrInfoListLoopPt->ai_addr )->sin6_addr ), p_LclNodeAddrStrArr, SzOfArr( p_LclNodeAddrStrArr ) );
				p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://[%z16s]:12345" ), p_LclNodeAddrStrArr );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://[%z16s]:12345" ), p_LclNodeAddrStrArr );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
				SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
			}
		}
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://[::]:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://[::]:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Tcp://[::1]:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		p_LclNodeUrlVstr.FmtCpy( Cu8vstr( "Audp://[::1]:12345" ), p_LclNodeAddrStrArr );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_ADDSTRING, 0, ( LPARAM )p_LclNodeUrlVstr.m_VstrPt->m_Pt );

		if( p_LclNodeAddrInfoListPt != NULL ) freeaddrinfo( p_LclNodeAddrInfoListPt ); //销毁本地节点地址信息结构体链表。

		//设置默认选择项。
		SendMessage( GetDlgItem( g_MainDlgWndHdl, SrvrUrlCbBoxId ), CB_SETCURSEL, 1, 0 ); //设置服务端Url组合框默认选择第二个Audp协议的Url。
		SendMessage( GetDlgItem( g_MainDlgWndHdl, ClntSrvrUrlCbBoxId ), CB_SETCURSEL, 1, 0 ); //设置客户端Url组合框默认选择第二个Audp协议的Url。
	}
	
	//设置使用什么对讲模式。
	CheckDlgButton( g_MainDlgWndHdl, UseAdoInptTkbkModeCkBoxId, BST_CHECKED );
	CheckDlgButton( g_MainDlgWndHdl, UseAdoOtptTkbkModeCkBoxId, BST_CHECKED );
	CheckDlgButton( g_MainDlgWndHdl, UseVdoInptTkbkModeCkBoxId, BST_UNCHECKED );
	CheckDlgButton( g_MainDlgWndHdl, UseVdoOtptTkbkModeCkBoxId, BST_UNCHECKED );
	
	//设置音频输入出是否静音、视频输入输出是否黑屏。
	CheckDlgButton( g_MainDlgWndHdl, AdoInptIsMuteCkBoxId, BST_UNCHECKED );
	CheckDlgButton( g_MainDlgWndHdl, AdoOtptIsMuteCkBoxId, BST_UNCHECKED );
	CheckDlgButton( g_MainDlgWndHdl, VdoInptIsBlackCkBoxId, BST_UNCHECKED );
	CheckDlgButton( g_MainDlgWndHdl, VdoOtptIsBlackCkBoxId, BST_UNCHECKED );
	
	//设置音频输入设备、音频输出设备。
	RefresAdoVdohDvc();

	//设置是否绘制音频波形到窗口。
	CheckDlgButton( g_MainDlgWndHdl, IsDrawAdoWavfmToWndCkBoxId, BST_CHECKED );

	//设置设置。
	{
		CheckRadioButton( g_StngDlgWndHdl, UseCntnrRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId, UseAjbRecvOtptFrmRdBtnId );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveStsToTxtFileCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsPrintLogShowToastCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsUsePrvntSysSleepCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsTstNtwkDlyCkBoxId, BST_CHECKED );
		CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoVdoInptOtptToAviFileCkBoxId, BST_CHECKED );

		//设置视频帧的大小。
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_RESETCONTENT, ( WPARAM )0, ( LPARAM )0 );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"120×160" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"240×320" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"480×640" );
		SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_ADDSTRING, ( WPARAM )0, ( LPARAM )L"960×1280" );
		SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherWidthEdTxtId ), L"640" );
		SetWindowText( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzOtherHeightEdTxtId ), L"480" );
		
		//设置使用什么效果等级。
		SendMessage( g_StngDlgWndHdl, WM_COMMAND, UseEffectSuperRdBtnId, 0 );
		
		//设置自适应抖动缓冲器。
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMinNeedBufFrmCntEdTxtId ), L"5" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxNeedBufFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbMaxCntuLostFrmCntEdTxtId ), L"20" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, AAjbAdaptSensitivityEdTxtId ), L"1.0" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMinNeedBufFrmCntEdTxtId ), L"3" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbMaxNeedBufFrmCntEdTxtId ), L"24" );
		SetWindowText( GetDlgItem( g_AjbStngDlgWndHdl, VAjbAdaptSensitivityEdTxtId ), L"1.0" );

		//设置保存状态到Txt文件。
		SetWindowText( GetDlgItem( g_SaveStsToTxtFileStngDlgWndHdl, SaveStsToTxtFileFullPathEdTxtId ), L"Sts.txt" );
		
		//设置保存音视频输入输出到Avi文件。
		SetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileFullPathEdTxtId ), L"AdoVdoInptOtpt.avi" );
		SetWindowText( GetDlgItem( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileWrBufSzBytEdTxtId ), L"8192" );
		CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoInptCkBoxId, BST_CHECKED );
		CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveAdoOtptCkBoxId, BST_CHECKED );
		CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoInptCkBoxId, BST_CHECKED );
		CheckDlgButton( g_SaveAdoVdoInptOtptToAviFileStngDlgWndHdl, SaveAdoVdoInptOtptToAviFileIsSaveVdoOtptCkBoxId, BST_CHECKED );

		//设置保存音频输入输出到Wave文件。
		CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoInptCkBoxId, BST_CHECKED );
		SetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptSrcFullPathEdTxtId ), L"AdoInptSrc.wav" );
		SetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoInptRsltFullPathEdTxtId ), L"AdoInptRslt.wav" );
		CheckDlgButton( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileIsSaveAdoOtptCkBoxId, BST_CHECKED );
		SetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileAdoOtptSrcFullPathEdTxtId ), L"AdoOtptSrc.wav" );
		SetWindowText( GetDlgItem( g_SaveAdoInptOtptToWaveFileStngDlgWndHdl, SaveAdoInptOtptToWaveFileWrBufSzBytEdTxtId ), L"8192" );
	}
}

//效果等级：低。
void EffectLow()
{
	CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectLowRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate8000RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecmRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseSpeexPrpocsNsRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate12RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
	SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"120×160" );
	CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

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
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"1" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"1" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"1" ); //影响比特率。
	CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"10" ); //影响比特率。
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"12" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );

}

//效果等级：中。
void EffectMid()
{
	CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectMidRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseWebRtcAecRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsxRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
	SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"240×320" );
	CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

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
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"4" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"4" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"40" ); //影响比特率。
	CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"20" ); //影响比特率。
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"15" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );
}

//效果等级：高。
void EffectHigh()
{
	CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectHighRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseWebRtcNsRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate15RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
	SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"480×640" );
	CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

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
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"8" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"8" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"80" ); //影响比特率。
	CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"40" ); //影响比特率。
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"15" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"0" );
}

//效果等级：超。
void EffectSuper()
{
	CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectSuperRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate16000RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate24RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
	SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"480×640" );
	CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

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
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"10" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"10" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"100" ); //影响比特率。
	CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"60" ); //影响比特率。
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"24" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"1" );
}

//效果等级：特。
void EffectPremium()
{
	CheckRadioButton( g_StngDlgWndHdl, UseEffectLowRdBtnId, UseEffectPremiumRdBtnId, UseEffectPremiumRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoSmplRate8000RdBtnId, UseAdoSmplRate48000RdBtnId, UseAdoSmplRate32000RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseAdoFrmLen10msRdBtnId, UseAdoFrmLen30msRdBtnId, UseAdoFrmLen20msRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSystemAecNsAgcCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseNoAecRdBtnId, UseSpeexWebRtcAecRdBtnId, UseSpeexWebRtcAecRdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseNoNsRdBtnId, UseRNNoiseRdBtnId, UseRNNoiseRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsUseSpeexPrpocsCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UsePcmRdBtnId, UseOpusCodecRdBtnId, UseSpeexCodecRdBtnId );
	CheckDlgButton( g_StngDlgWndHdl, IsSaveAdoInptOtptToWaveFileCkBoxId, BST_CHECKED );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoSmplRate12RdBtnId, UseVdoSmplRate30RdBtnId, UseVdoSmplRate30RdBtnId );
	CheckRadioButton( g_StngDlgWndHdl, UseVdoFrmSzPrsetRdBtnId, UseVdoFrmSzOtherRdBtnId, UseVdoFrmSzPrsetRdBtnId );
	SendMessage( GetDlgItem( g_StngDlgWndHdl, VdoFrmSzPrsetCbBoxId ), CB_SELECTSTRING, ( WPARAM )-1, ( LPARAM )L"960×1280" );
	CheckRadioButton( g_StngDlgWndHdl, UseYu12RdBtnId, UseOpenH264CodecRdBtnId, UseOpenH264CodecRdBtnId );

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
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdQualtEdTxtId ), L"10" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdCmplxtEdTxtId ), L"10" ); //影响比特率。
	SetWindowText( GetDlgItem( g_SpeexCodecStngDlgWndHdl, SpeexEncdPlcExptLossRateEdTxtId ), L"100" ); //影响比特率。
	CheckDlgButton( g_SpeexCodecStngDlgWndHdl, SpeexDecdIsUsePrcplEnhsmtCkBoxId, BST_CHECKED );

	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdVdoTypeEdTxtId ), L"0" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdEncdBitrateEdTxtId ), L"80" ); //影响比特率。
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdBitrateCtrlModeEdTxtId ), L"3" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdIDRFrmIntvlEdTxtId ), L"30" );
	SetWindowText( GetDlgItem( g_OpenH264CodecStngDlgWndHdl, OpenH264EncdCmplxtEdTxtId ), L"2" );
}
