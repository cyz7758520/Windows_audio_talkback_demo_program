#pragma once

#include "ClntMediaPocsThrd.h"

class MyClntMediaPocsThrdCls : public ClntMediaPocsThrdCls //我的客户端媒体处理线程。
{
public:
	HWND m_MainDlgWndHdl; //存放主对话框窗口的句柄。
	int32_t m_TkbkClntNum; //存放对讲客户端的序号。
	
	MyClntMediaPocsThrdCls( HWND MainDlgWndHdl ) { m_MainDlgWndHdl = MainDlgWndHdl; }
	~MyClntMediaPocsThrdCls() {}
	
	int Init( const void * LicnCodePt );
	void Dstoy();
	int TkbkInit( const void * LicnCodePt );
	void TkbkDstoy();
	int BdctInit( const void * LicnCodePt );
	void BdctDstoy();
	
	//用户定义的初始化函数。
	void UserInit();
	
	//用户定义的销毁函数。
	void UserDstoy();
	
	//用户定义的处理函数。
	void UserPocs();

	//用户定义的消息函数。
	int UserMsg( unsigned int MsgTyp, void * MsgParmPt, size_t MsgParmLenByt );
	
	//用户定义的设备改变函数。
	void UserDvcChg( Vstr * AdoInptDvcNameVstrPt, Vstr * AdoOtptDvcNameVstrPt, Vstr * VdoInptDvcNameVstrPt );

	//用户定义的读取音视频输入帧函数。
	void UserReadAdoVdoInptFrm( int16_t * AdoInptPcmSrcFrmPt, int16_t * AdoInptPcmRsltFrmPt, size_t AdoInptPcmFrmLenUnit, int32_t AdoInptPcmRsltFrmVoiceActSts,
								uint8_t * AdoInptEncdRsltFrmPt, size_t AdoInptEncdRsltFrmLenByt, int32_t AdoInptEncdRsltFrmIsNeedTrans,
								uint8_t * VdoInptBgraSrcFrmPt, int32_t VdoInptBgraSrcFrmWidth, int32_t VdoInptBgraSrcFrmHeight, size_t VdoInptBgraSrcFrmLenByt,
								uint8_t * VdoInptYu12RsltFrmPt, int32_t VdoInptYu12RsltFrmWidth, int32_t VdoInptYu12RsltFrmHeight, size_t VdoInptYu12RsltFrmLenByt,
								uint8_t * VdoInptEncdRsltFrmPt, size_t VdoInptEncdRsltFrmLenByt );

	//用户定义的写入音频输出帧函数。
	void UserWriteAdoOtptFrm( int32_t AdoOtptStrmIdx,
							  int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
							  uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmSzByt, size_t * AdoOtptEncdSrcFrmLenBytPt );

	//用户定义的获取音频输出帧函数。
	void UserGetAdoOtptFrm( int32_t AdoOtptStrmIdx,
							int16_t * AdoOtptPcmSrcFrmPt, size_t AdoOtptPcmFrmLenUnit,
							uint8_t * AdoOtptEncdSrcFrmPt, size_t AdoOtptEncdSrcFrmLenByt );

	//用户定义的写入视频输出帧函数。
	void UserWriteVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							  uint8_t * VdoOtptYu12SrcFrmPt, int32_t * VdoOtptYu12SrcFrmWidthPt, int32_t * VdoOtptYu12SrcFrmHeightPt,
							  uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmSzByt, size_t * VdoOtptEncdSrcFrmLenBytPt );

	//用户定义的获取视频输出帧函数。
	void UserGetVdoOtptFrm( uint32_t VdoOtptStrmIdx,
							uint8_t * VdoOtptYu12SrcFrmPt, int32_t VdoOtptYu12SrcFrmWidth, int32_t VdoOtptYu12SrcFrmHeight,
							uint8_t * VdoOtptEncdSrcFrmPt, size_t VdoOtptEncdSrcFrmLenByt );

	//用户定义的显示日志函数。
	void UserShowLog( Vstr * InfoVstrPt );

	//用户定义的显示Toast函数。
	void UserShowToast( Vstr * InfoVstrPt );
	
	//用户定义的对讲客户端连接初始化函数。
	void UserTkbkClntCnctInit( int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的对讲客户端连接销毁函数。
	void UserTkbkClntCnctDstoy();
	
	//用户定义的对讲客户端连接状态函数。
	void UserTkbkClntCnctSts( int32_t CurCnctSts );
	
	//用户定义的对讲客户端我的对讲索引函数。
	void UserTkbkClntMyTkbkIdx( int32_t MyTkbkIdx );
	
	//用户定义的对讲客户端本端对讲模式函数。
	void UserTkbkClntLclTkbkMode( int32_t OldLclTkbkMode, int32_t NewLclTkbkMode );
	
	//用户定义的对讲客户端对讲信息初始化函数。
	void UserTkbkClntTkbkInfoInit( TkbkClnt::TkbkInfo * TkbkInfoPt );
	
	//用户定义的对讲客户端对讲信息销毁函数。
	void UserTkbkClntTkbkInfoDstoy( TkbkClnt::TkbkInfo * TkbkInfoPt );
	
	//用户定义的对讲客户端对讲信息远端对讲模式函数。
	void UserTkbkClntTkbkInfoRmtTkbkMode( TkbkClnt::TkbkInfo * TkbkInfoPt, int32_t OldRmtTkbkMode, int32_t NewRmtTkbkMode );
	
	//用户定义的对讲客户端测试网络延迟函数。
	void UserTkbkClntTstNtwkDly( uint64_t NtwkDlyMsec );

	//用户定义的广播客户端初始化函数。
	void UserBdctClntInit();

	//用户定义的广播客户端销毁函数。
	void UserBdctClntDstoy();
	
	//用户定义的广播客户端连接初始化函数。
	void UserBdctClntCnctInit( BdctClnt::CnctInfo * CnctInfoPt, int32_t IsTcpOrAudpPrtcl, Vstr * RmtNodeNameVstrPt, Vstr * RmtNodeSrvcVstrPt );
	
	//用户定义的广播客户端连接销毁函数。
	void UserBdctClntCnctDstoy( BdctClnt::CnctInfo * CnctInfoPt );
	
	//用户定义的广播客户端连接状态函数。
	void UserBdctClntCnctSts( BdctClnt::CnctInfo * CnctInfoPt, int32_t CurCnctSts );
	
	void SetToUseAdoInpt();
	void SetNotUseAdoInpt();
	void SetToUseAdoOtpt();
	void SetNotUseAdoOtpt();
	void SetToUseAdoOtptStrm( int TkbkIdx );
	void SetNotUseAdoOtptStrm( int TkbkIdx );
	void SetToUseVdoInpt();
	void SetNotUseVdoInpt();
	void SetToUseVdoOtpt();
	void SetNotUseVdoOtpt();
	void SetToUseVdoOtptStrm( int TkbkIdx );
	void SetNotUseVdoOtptStrm( int TkbkIdx );
};
